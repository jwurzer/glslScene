#include <configloader/res_loader.h>

#include <configloader/uniform_attr_loader.h>
#include <common/cfg.h>
#include <common/str.h>
#include <system/log.h>
#include <res/texture.h>
#include <res/resource_manager.h>
#include <res/mesh.h>
#include <res/framebuffer.h>
#include <res/creation.h>
#include <res/shader_info.h>

bool gs::resloader::addResources(ResourceManager& rm, const CfgValuePair& cfgValue)
{
	if (!cfgValue.mValue.isArray()) {
		return false;
	}
	bool rv = true;
	if (cfgValue.mName.mText == "resources") {
		for (const CfgValuePair& vpRes : cfgValue.mValue.mArray) {
			if (!addResource(rm, vpRes)) {
				LOGE("Can't add resource '%s' to resource manager.\n",
						vpRes.mName.mText.c_str());
				rv = false;
			}
		}
	}
	return rv;
}

gs::TResourceId gs::resloader::addResource(ResourceManager& rm, const CfgValuePair& cfgValue)
{
	if (cfgValue.mName.mText == "texture") {
		return addTexture(rm, cfgValue);
	}
	else if (cfgValue.mName.mText == "shader") {
		return addShaderProgram(rm, cfgValue);
	}
	else if (cfgValue.mName.mText == "mesh") {
		return addMesh(rm, cfgValue);
	}
	else if (cfgValue.mName.mText == "framebuffer") {
		return addFramebuffer(rm, cfgValue);
	}
	return 0;
}

gs::TTextureId gs::resloader::addTexture(ResourceManager& rm, const CfgValuePair& cfgValuePair)
{
	if (cfgValuePair.mName.mText != "texture") {
		return 0;
	}
	std::string idName;
	std::string resFilename;
	const std::vector<CfgValuePair>* resPairArray = nullptr;
	unsigned int resFilenameCount = 0;
	unsigned int resPairArrayCount = 0;
	bool useMipmap = false;
	std::string minFilter;
	std::string magFilter;

	CfgReadRule cfgRules[] = {
			CfgReadRule("id", &idName, CfgReadRule::RULE_MUST_EXIST),
			CfgReadRule("filename", &resFilename, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &resFilenameCount),
			CfgReadRule("filenames", &resPairArray, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &resPairArrayCount),
			CfgReadRule("mipmap", &useMipmap, CfgReadRule::RULE_MUST_EXIST),
			CfgReadRule("min-filter", &minFilter, CfgReadRule::RULE_MUST_EXIST),
			CfgReadRule("mag-filter", &magFilter, CfgReadRule::RULE_MUST_EXIST),
			CfgReadRule("")
	};

	size_t nextPos = 0;
	ssize_t storeCnt = cfgValuePair.mValue.sectionGet(
			cfgRules, false, false, false, false, false, 0, &nextPos);
	if (storeCnt != 5) {
		LOGE("texture config is wrong\n");
		return 0;
	}
	TexMipmap texMipmap = useMipmap ? TexMipmap::MIPMAP : TexMipmap::NO_MIPMAP;
	TexFilter texMinFilter = TexFilter::NEAREST;
	TexFilter texMagFilter = TexFilter::NEAREST;
	if (minFilter == "linear") {
		texMinFilter = TexFilter::LINEAR;
	}
	else if (minFilter == "nearest") {
		texMinFilter = TexFilter::NEAREST;
	}
	else {
		LOGE("min-filter value is not allowed\n");
		return 0;
	}
	if (magFilter == "linear") {
		texMagFilter = TexFilter::LINEAR;
	}
	else if (magFilter == "nearest") {
		texMagFilter = TexFilter::NEAREST;
	}
	else {
		LOGE("mag-filter value is not allowed\n");
		return 0;
	}
	if (resFilenameCount + resPairArrayCount != 1) {
		LOGE("Only filename or filenames can be used.\n");
		return 0;
	}
	if (resFilenameCount) {
		TTextureId texId = rm.addTexture(idName, resFilename, texMipmap, texMinFilter, texMagFilter);
		if (!texId) {
			LOGE("Add texture failed\n");
			return 0;
		}
		return texId;
	}
	else if (resPairArrayCount) {
		if (!resPairArray) {
			LOGE("No valid pointer for filenames\n");
			return 0;
		}
		const std::vector<CfgValuePair>& arr = *resPairArray;
		unsigned int removeCount = 0;
		std::string idSubName = str::getStringAndRemoveEndingWithCh(
				idName, '*', true, &removeCount);
		//LOGI("array size %zu, remove cnt %u, %s - %s\n", arr.size(),
		//		removeCount, idName.c_str(), idSubName.c_str());
		if (!removeCount) {
			LOGE("No * at id for array.\n");
			return 0;
		}
		char idNameWithIndex[64];
		for (unsigned int i = 0; i < arr.size(); ++i) {
			snprintf(idNameWithIndex, 64, "%s%0*u", idSubName.c_str(), removeCount, i);
			//LOGI("Name: %s\n", idNameWithIndex);
			if (!rm.addTexture(idNameWithIndex, arr[i].mName.mText, texMipmap, texMinFilter, texMagFilter)) {
				LOGE("Add texture failed\n");
				return 0;
			}
		}
	}
	else {
		LOGE("No filename and no filenames is used.\n");
		return 0;
	}
	return 1;
}

gs::TShaderId gs::resloader::addShaderProgram(ResourceManager& rm, const CfgValuePair& cfgValuePair)
{
	if (cfgValuePair.mName.mText != "shader") {
		return 0;
	}
	const CfgValue& cfgValue = cfgValuePair.mValue;
	size_t cnt = cfgValue.mArray.size();
	if (!cnt) {
		LOGE("Empty section for shader resource is not allowed.\n");
		return 0;
	}
	if (cfgValue.mArray[0].mName.mText != "id") {
		LOGE("id must be the first attribute of the shader section.\n");
		return 0;
	}
	std::string idName = cfgValue.mArray[0].mValue.mText;
	ShaderProgramLoadInfo shaderProgramInfos;
	shaderProgramInfos.mShaderInfos.reserve(cnt - 1);
	bool noMoreShaderIsAllowed = false;
	size_t uniformStartIndex = cnt; // will be reset to a correct value if uniforms are used
	for (size_t i = 1; i < cnt; ++i) {
		const CfgValuePair& vp = cfgValue.mArray[i];
		std::string name = vp.mName.mText;
		ShaderType shaderType = ShaderType::FRAGMENT_SHADER;
		if (name == "compute") {
			shaderType = ShaderType::COMPUTE_SHADER;
		}
		else if (name == "vertex") {
			shaderType = ShaderType::VERTEX_SHADER;
		}
		else if (name == "tess-control") {
			shaderType = ShaderType::TESS_CONTROL_SHADER;
		}
		else if (name == "tess-evaluation") {
			shaderType = ShaderType::TESS_EVALUATION_SHADER;
		}
		else if (name == "geometry") {
			shaderType = ShaderType::GEOMETRY_SHADER;
		}
		else if (name == "fragment") {
			shaderType = ShaderType::FRAGMENT_SHADER;
		}
		else {
			if (!noMoreShaderIsAllowed) {
				uniformStartIndex = i;
				noMoreShaderIsAllowed = true;
			}
			continue;
		}
		if (noMoreShaderIsAllowed) {
			// --> there was a shader type after a non shader type (uniform)
			LOGE("%s is not allowed after no shader type.\n", name.c_str());
			return 0;
		}
		shaderProgramInfos.mShaderInfos.emplace_back(shaderType, vp.mValue.mText, true);
	}
	shaderProgramInfos.mUniforms.clear();
	if (!uniformattrloader::loadUniformsAndAttributes(
			shaderProgramInfos.mUniforms,
			shaderProgramInfos.mAttributes,
			cfgValue, uniformStartIndex, "uniforms")) {
		LOGE("One or more uniforms are not correct.\n");
		return 0;
	}

	TShaderId shaderId = rm.addShaderProgram(idName, shaderProgramInfos);
	if (!shaderId) {
		LOGE("Add shader resource failed.\n");
		return 0;
	}
	return shaderId;
}

gs::TMeshId gs::resloader::addMesh(ResourceManager& rm, const CfgValuePair& cfgValuePair)
{
	if (cfgValuePair.mName.mText != "mesh") {
		return 0;
	}
	const char* nameForErr = "mesh";
	const CfgValue& cfgValue = cfgValuePair.mValue;
	if (cfgValue.mArray.size() < 1) {
		LOGE("Wrong size. Must be at least 1 for id.\n");
		//LOGE("Wrong size. Must be at least 1 for vertex-layout and one more attribute.\n");
		return 0;
	}
	std::string idName;
	int startIndex = 0; // index of vertex-layout
	if (cfgValue.mArray[0].mName.mText == "id") {
		idName = cfgValue.mArray[0].mValue.mText;
		startIndex = 1;
	}
	float scaleForShowNormals = 1.0f;
	if (cfgValue.mArray.size() > static_cast<unsigned int>(startIndex) &&
			cfgValue.mArray[startIndex].mName.mText == "scale-for-show-normals") {
		scaleForShowNormals = cfgValue.mArray[startIndex].mValue.mFloatingPoint;
		++startIndex;
	}
	if ((startIndex == 1 || startIndex == 2) && cfgValue.mArray.size() == startIndex) {
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(rm.useVaoVersionForMesh(), scaleForShowNormals);
		LOGW("Create a empty mesh!\n");
		return rm.addResource(idName, mesh);
	}
	if ((startIndex == 1 || startIndex == 2) && cfgValue.mArray.size() < static_cast<unsigned int>(startIndex) + 2) {
		LOGE("Wrong size. Must be at least %d if id is used.\n", startIndex + 2);
		return 0;
	}

	if (cfgValue.mArray[startIndex].mName.mText != "vertex-layout" &&
			cfgValue.mArray[startIndex].mName.mText != "custom-vertex-layout") {
		LOGE("%s%s: No vertex-layout or custom-vertex-layout attribute.\n", nameForErr,
				cfgValue.mArray[startIndex].mName.getFilePosition().c_str());
		return 0;
	}
	if (cfgValue.mArray[startIndex].mValue.mArray.size() != 1) {
		LOGE("%s%s: Only exactly one array entry is allowed for the vertex-layout\n",
				nameForErr, cfgValue.mArray[startIndex].mValue.getFilePosition().c_str());
		return 0;
	}
	const std::vector<CfgValuePair>& layout = cfgValue.mArray[startIndex].mValue.mArray[0].mName.mArray;
	size_t layoutSize = layout.size();
	if (layoutSize < 2) {
		LOGE("Wrong size. Must be at least 2 for x and y.\n");
		return 0;
	}
	if (layout[0].mValue.mText != "x" ||
			layout[1].mValue.mText != "y") {
		return 0;
	}
	unsigned int layoutPosCount = (layoutSize > 2 && layout[2].mValue.mText == "z") ? 3 : 2;
	size_t i = layoutPosCount;

	unsigned int layoutNormalCount = 0;
	if (layoutSize - i >= 3) {
		if (layout[i].mValue.mText == "nx" &&
				layout[i + 1].mValue.mText == "ny" &&
				layout[i + 2].mValue.mText == "nz") {
			layoutNormalCount = 3;
			i += 3;
		}
	}

	unsigned int layoutTexCount = 0;
	for (; i + 1 < layoutSize; i += 2) {
		if (layout[i + 0].mValue.mText != "s" + std::to_string(layoutTexCount) ||
				layout[i + 1].mValue.mText != "t" + std::to_string(layoutTexCount)) {
			break;
		}
		++layoutTexCount;
	}
	unsigned int layoutColorCount = 0;
	if (layoutSize - i >= 3) {
		if (layout[i].mValue.mText == "r" &&
				layout[i + 1].mValue.mText == "g" &&
				layout[i + 2].mValue.mText == "b") {
			layoutColorCount = 3;
			i += 3;
		}
		if (layoutSize - i >= 1) {
			if (layout[i].mValue.mText == "a") {
				layoutColorCount = 4;
				++i;
			}
		}
	}
	if (i > layoutSize) {
		// should not be possible
		LOGE("Impl. error\n");
		return 0;
	}
	unsigned int layoutCustomCount = 0;
	if (i != layoutSize) {
		layoutCustomCount = layoutSize - i;
		if (cfgValue.mArray[startIndex].mName.mText != "custom-vertex-layout") {
			LOGE("Custom count of %u is only allowed for a custom-vertex-layout.\n",
					layoutCustomCount);
			return 0;
		}
	}
	std::vector<float> vertex(layoutSize, 0.0f);
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(rm.useVaoVersionForMesh(), scaleForShowNormals);
	size_t cnt = cfgValue.mArray.size();
	for (size_t i = startIndex + 1; i < cnt; ++i) {
		size_t vertexCnt = cfgValue.mArray[i].mValue.mArray.size();
		if (!vertexCnt) {
			LOGE("%s%s: %s has not vertices\n", nameForErr,
					cfgValue.mArray[i].mName.getFilePosition().c_str(),
					cfgValue.mArray[i].mName.mText.c_str());
			return 0;
		}
		std::vector<float> vertices;
		for (const auto& vpair : cfgValue.mArray[i].mValue.mArray) {
			unsigned int vertexAttrCount = vpair.mName.mArray.size();
			if (vertexAttrCount != layoutSize) {
				LOGE("%s%s: Size of array has a wrong size (%u != %zu)\n",
						nameForErr,
						vpair.mName.getFilePosition().c_str(),
						vertexAttrCount, layoutSize);
				return 0;
			}
			const auto &vAttrs = vpair.mName.mArray;
			for (unsigned int i = 0; i < vertexAttrCount; ++i) {
				vertex[i] = vAttrs[i].mValue.mFloatingPoint;
			}
			// now the vertex has the correct values
			vertices.insert(vertices.end(), vertex.begin(), vertex.end());
		}
		if (vertices.size() != layoutSize * vertexCnt) {
			LOGE("Vertices have wrong size\n");
			return 0;
		}

		if (cfgValue.mArray[i].mName.mArray.size() == 3 &&
				cfgValue.mArray[i].mName.mArray[0].mValue.mText == "point-mesh") {
			if (vertexCnt != 3) {
				LOGE("Must be 3 vertices\n");
				return 0;
			}
			if (!creation::addPointMesh(*mesh, vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount,
					cfgValue.mArray[i].mName.mArray[1].mValue.mInteger,
					cfgValue.mArray[i].mName.mArray[2].mValue.mInteger
				)) {
				LOGE("Can't add point-mesh\n");
				return 0;
			}
		}
		else if (cfgValue.mArray[i].mName.mArray.size() == 3 &&
				cfgValue.mArray[i].mName.mArray[0].mValue.mText == "triangle-mesh") {
			if (vertexCnt != 3) {
				LOGE("Must be 3 vertices\n");
				return 0;
			}
			if (!creation::addTriangleMesh(*mesh, vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount,
					cfgValue.mArray[i].mName.mArray[1].mValue.mInteger,
					cfgValue.mArray[i].mName.mArray[2].mValue.mInteger
				)) {
				LOGE("Can't add triangle-mesh\n");
				return 0;
			}
		}
		else if (cfgValue.mArray[i].mName.mArray.size() == 3 &&
				cfgValue.mArray[i].mName.mArray[0].mValue.mText == "quad-mesh") {
			if (vertexCnt != 3) {
				LOGE("Must be 3 vertices\n");
				return 0;
			}
			if (!creation::addQuadMesh(*mesh, vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount,
					cfgValue.mArray[i].mName.mArray[1].mValue.mInteger,
					cfgValue.mArray[i].mName.mArray[2].mValue.mInteger
				)) {
				LOGE("Can't add quad-mesh\n");
				return 0;
			}
		}
		else if (cfgValue.mArray[i].mName.mText == "points") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add points\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::POINTS);
		}
		else if (cfgValue.mArray[i].mName.mText == "lines") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add lines\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::LINES);
		}
		else if (cfgValue.mArray[i].mName.mText == "line-loop") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add line-loop\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::LINES);
		}
		else if (cfgValue.mArray[i].mName.mText == "line-strip") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add line-strip\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::LINE_STRIP);
		}
		else if (cfgValue.mArray[i].mName.mText == "triangles") {
			if (vertexCnt % 3) {
				LOGE("Must be a muliple of 3 for triangles\n");
				return 0;
			}
			if (!creation::addTriangles(*mesh, vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add triangles\n");
				return 0;
			}
		}
		else if (cfgValue.mArray[i].mName.mText == "triangle-strip") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add triangle-strip\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::TRIANGLE_STRIP);
		}
		else if (cfgValue.mArray[i].mName.mText == "triangle-fan") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add triangle-fan\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::TRIANGLE_FAN);
		}
		else if (cfgValue.mArray[i].mName.mText == "quads") {
			if (vertices.size() % 4) {
				LOGE("Must be a muliple of 4 for quads\n");
				return 0;
			}
			if (!creation::addQuads(*mesh, vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add quads\n");
				return 0;
			}
		}
		else if (cfgValue.mArray[i].mName.mText == "quad-strip") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add quad-strip\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::QUAD_STRIP);
		}
		else if (cfgValue.mArray[i].mName.mText == "polygon") {
			if (!mesh->addVertices(vertices.data(),
					layoutSize * sizeof(float), vertexCnt,
					layoutPosCount,
					layoutNormalCount,
					layoutTexCount,
					layoutColorCount,
					layoutCustomCount)) {
				LOGE("Can't add polygon\n");
				return 0;
			}
			mesh->setPrimitiveType(PrimitiveType::POLYGON);
		}
		else if (cfgValue.mArray[i].mName.mText == "rects") {
			if (vertices.size() % 2) {
				LOGE("Must be a muliple of 2 for rects\n");
				return 0;
			}
			LOGE("TODO rects\n");
			return 0;
#if 0
			size_t cnt = vertices.size();
			for (size_t i = 0; i < cnt; i += 2) {
				Vertex v1 = vertices[i]; // left top
				Vertex v3 = vertices[i + 1]; // right bottom
				Color c(
						v1.mColor.r * 0.5f + v3.mColor.r * 0.5f,
						v1.mColor.g * 0.5f + v3.mColor.g * 0.5f,
						v1.mColor.b * 0.5f + v3.mColor.b * 0.5f,
						v1.mColor.a * 0.5f + v3.mColor.a * 0.5f);
				// right top
				Vertex v2(Vector2f(v3.mPosition.x, v1.mPosition.y),
						v1.mColorType, v1.mBorderType, c,
						Vector2f(v3.mTexCoords.x, v1.mTexCoords.y));
				// left bottom
				Vertex v4(Vector2f(v1.mPosition.x, v3.mPosition.y),
						v3.mColorType, v3.mBorderType, c,
						Vector2f(v1.mTexCoords.x, v3.mTexCoords.y));
				va::addQuad(va, v1, v2, v3, v4, 0 /* tex id */,
						vertex::TEX_NORMALIZED,
						va::QUAD_TWO_TRI_V123_V341,
						va::BORDER_MODE_TYPE_FROM_VERTEX);
			}
			if (texCoords.size() > 0 && layoutTexCount > 1) {
				size_t texCnt = texCoords.size();
				unsigned int additionalTexCount = layoutTexCount - 1;
				if (additionalTexCount > 7) {
					LOGE("Max additional tex count per vertex is 7. %u is not allowed.\n",
							additionalTexCount);
					return std::shared_ptr<VectorGraphics>();
				}
				if (texCnt % (additionalTexCount * 2)) {
					LOGE("Wrong tex count for rect. %zu != %u * 2\n",
							texCnt, additionalTexCount);
					return std::shared_ptr<VectorGraphics>();
				}
				Vector2f quadTexCoordsA[7 * 4]; // 7 * 4 = maximum possible count
				Vector2f* quadTexCoordsB = quadTexCoordsA + additionalTexCount;
				Vector2f* quadTexCoordsC = quadTexCoordsB + additionalTexCount;
				Vector2f* quadTexCoordsD = quadTexCoordsC + additionalTexCount;
				for (size_t i = 0; i < texCnt; i += additionalTexCount * 2) {
					const Vector2f* rectTexCoords = texCoords.data() + i;
					for (unsigned int ti = 0; ti < additionalTexCount; ++ti) {
						// left top
						Vector2f texA = rectTexCoords[ti];
						// right bottom
						Vector2f texC = rectTexCoords[ti + additionalTexCount];
						// right top
						Vector2f texB(texC.x, texA.y);
						// left bottom
						Vector2f texD(texA.x, texC.y);
						quadTexCoordsA[ti] = texA;
						quadTexCoordsB[ti] = texB;
						quadTexCoordsC[ti] = texC;
						quadTexCoordsD[ti] = texD;
					}
					if (!va::addQuadTexCoords(va, quadTexCoordsA,
							additionalTexCount, va::QUAD_TWO_TRI_V123_V341)) {
						LOGE("Can't add tex coords for rect\n");
						return std::shared_ptr<VectorGraphics>();
					}
				}
			}
#endif
		}
		else {
			LOGE("%s%s: %s is not supported\n", nameForErr,
					cfgValue.mArray[i].mName.getFilePosition().c_str(),
					cfgValue.mArray[i].mName.mText.c_str());
			return 0;
		}
	}
	if (!mesh->getVertexCount()) {
		LOGW("Create a empty mesh!\n");
	}
	return rm.addResource(idName, mesh);
}

gs::TFramebufferId gs::resloader::addFramebuffer(ResourceManager& rm, const CfgValuePair& cfgValuePair)
{
	if (cfgValuePair.mName.mText != "framebuffer") {
		return 0;
	}
	std::string idName;
	const CfgValue* sizeValue = nullptr;
	CfgReadRule cfgRules[] = {
			CfgReadRule("id", &idName, CfgReadRule::RULE_MUST_EXIST),
			CfgReadRule("size", &sizeValue, CfgReadRule::RULE_MUST_EXIST),
			CfgReadRule("")
	};

	size_t nextPos = 0;
	ssize_t storeCnt = cfgValuePair.mValue.sectionGet(
			cfgRules, false, false, false, false, false, 0, &nextPos);
	if (storeCnt < 0) {
		LOGE("framebuffer config is wrong\n");
		return 0;
	}
	int width = 0, height = 0;
	if (sizeValue->mText == "window-size") {
		width = -1;
		height = -1;
	}
	else if (sizeValue->mArray.size() == 2) {
		const std::vector<CfgValuePair>& arr = sizeValue->mArray;
		if (arr[0].mValue.mText == "window-size") {
			width = -1;
		}
		else {
			width = arr[0].mValue.mInteger;
		}
		if (arr[1].mValue.mText == "window-size") {
			height = -1;
		}
		else {
			height = arr[1].mValue.mInteger;
		}
	}
	else {
		LOGE("Wrong value for array\n");
		return 0;
	}
	std::shared_ptr<Framebuffer> fb = std::make_shared<Framebuffer>(width, height);
	return rm.addResource(idName, fb);
}

