#include <res/mesh.h>

#include <res/shader_program.h>
#include <common/vertex.h>
#include <rendering/gl_api.h>
#include <system/log.h>
#include <sstream>

namespace gs
{
	namespace
	{
		unsigned int getPrimitive(PrimitiveType primType)
		{
			switch (primType)
			{
			case PrimitiveType::POINTS:
				return GL_POINTS;
			case PrimitiveType::LINES:
				return GL_LINES;
			case PrimitiveType::LINE_LOOP:
				return GL_LINE_LOOP;
			case PrimitiveType::LINE_STRIP:
				return GL_LINE_STRIP;
			case PrimitiveType::TRIANGLES:
				return GL_TRIANGLES;
			case PrimitiveType::TRIANGLE_STRIP:
				return GL_TRIANGLE_STRIP;
			case PrimitiveType::TRIANGLE_FAN:
				return GL_TRIANGLE_FAN;
			case PrimitiveType::QUADS:
				return GL_QUADS;
			case PrimitiveType::QUAD_STRIP:
				return GL_QUAD_STRIP;
			case PrimitiveType::POLYGON:
				return GL_POLYGON;
			}
			return 0;
		}
	}
}

gs::Mesh::Mesh(bool useVaoVersion)
		:Resource(std::weak_ptr<FileChangeMonitoring>()),
		mPrimitiveType(PrimitiveType::TRIANGLES),
		mVertices(),
		mVertexSize(0),
		mVertexCount(0),
		mPosCount(0),
		mTexCount(0),
		mColorCount(0),
		mCustomCount(0),
		mUseVaoVersion(useVaoVersion),
		mChanged(false),
		mVbo(0),
		mVao(0)
{
}

gs::Mesh::~Mesh()
{
	if (mUseVaoVersion) {
		if (mVao) {
			glDeleteVertexArrays(1, &mVao);
		}
		if (mVbo) {
			glDeleteBuffers(1, &mVbo);
		}
	}
}

void gs::Mesh::clear()
{
	mChanged = true;
	mVertices.clear();
	mVertexSize = 0;
	mVertexCount = 0;
	mPosCount = 0;
	mTexCount = 0;
	mColorCount = 0;
	mCustomCount = 0;
}

/**
 * @param vertices Buffersize in bytes must be vertexSize * vertexCount, unit: bytes
 * @param vertexSize must be the same as (posCount + texCount * 2 + colorCount + customCount) * sizeof(float), unit: bytes
 * @param vertexCount Count of vertices
 * @param posCount 2 for x,y, 3 for x,y,z
 * @param texCount 1 for one texture s,t, 2 for s0,t0,s1,t1, N for s0,t0 to s<N-1>,t<N-1>
 * @param colorCount 3 for r,g,b, 4 for r,g,b,a
 * @param customCount can be any value
 */
bool gs::Mesh::addVertices(const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount)
{
	if (!vertices ||
			!vertexSize || (vertexSize % sizeof(float)) || !vertexCount ||
			vertexSize != (posCount + texCount * 2 + colorCount + customCount) * sizeof(float)) {
		LOGE("Wrong parameters\n");
		return false;
	}
	if (!mVertexCount) {
		mVertexSize = vertexSize;
		mPosCount = posCount;
		mTexCount = texCount;
		mColorCount = colorCount;
		mCustomCount = customCount;
	}
	else {
		// --> mesh has already one or more vertices --> check if the new
		// added vertices are compatible
		if (mVertexSize != vertexSize ||
				mPosCount != posCount ||
				mTexCount != texCount ||
				mColorCount != colorCount ||
				mCustomCount != customCount) {
			return false;
		}
	}
	mVertexCount += vertexCount;
	unsigned int bufSizeAsFloats = vertexSize * vertexCount / sizeof(float);
	const float* buf = static_cast<const float*>(vertices);
	mVertices.insert(mVertices.end(), buf, buf + bufSizeAsFloats);
	mChanged = true;
	return true;
}

bool gs::Mesh::addVertices(const VertexV3C4* vertices,
		unsigned int vertexCount)
{
	return addVertices(vertices, sizeof(VertexV3C4), vertexCount, 3, 0, 4, 0);
}
void gs::Mesh::bind(const ShaderProgram* shaderProgram)
{
	if (!mVertexCount) {
		return;
	}

	if (mUseVaoVersion) {
		bindVaoVersion(shaderProgram);
	}
	else {
		bindNoVaoVersion(shaderProgram);
	}
}

void gs::Mesh::draw()
{
	if (!mVertexCount) {
		return;
	}

	glDrawArrays(getPrimitive(mPrimitiveType), 0, mVertexCount);
}

void gs::Mesh::unbind(const ShaderProgram* shaderProgram)
{
	if (!mVertexCount) {
		return;
	}

	if (mUseVaoVersion) {
		unbindVaoVersion(shaderProgram);
	}
	else {
		unbindNoVaoVersion(shaderProgram);
	}
}

std::string gs::Mesh::toString() const
{
	std::stringstream s;
	s << "vertex layout: " << mVertexSize << "(" << (mVertexSize / sizeof(float)) << ") " << mPosCount << " " << mTexCount << " " << mColorCount << "\n";
	bool isValid = (mVertices.size() * sizeof(float) == mVertexCount * mVertexSize);
	s << "vertex count: " << mVertexCount << (isValid ? " (valid)" : " (invalid)") << "\n";
	if (isValid) {
		unsigned int floatsPerVertex = mVertexSize / sizeof(float);
		for (unsigned int vi = 0, i = 0; vi < mVertexCount; ++vi) {
			for (unsigned int fi = 0; fi < floatsPerVertex; ++fi, ++i) {
				s << " " << mVertices[i];
			}
			s << "\n";
		}
	}

	return s.str();
}

void gs::Mesh::bindNoVaoVersion(const ShaderProgram* shaderProgram)
{
	if (shaderProgram) {
		const std::vector<Attribute>& attrs = shaderProgram->getAttributes();
		if (!attrs.empty()) {
			for (const auto &a : attrs) {
				if (a.mLocation != -1) {
					glEnableVertexAttribArray(a.mLocation);
					glVertexAttribPointer(a.mLocation, // index
							a.mCompCount, // components pro Vertex 2 for (x,y), 3 for (x,y,z)
							GL_FLOAT, // type of component
							GL_FALSE, // normalized
							mVertexSize, // offset between 2 vertices in array
							mVertices.data() +
									a.mCompOffset); // Pointer to the 1. component
				}
			}
			return;
		}
	}

	if (mPosCount) {
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(
				mPosCount, // components pro Vertex 2 for (x,y), 3 for (x,y,z)
				GL_FLOAT, // type of component
				mVertexSize, // offset between 2 vertices in array
				mVertices.data()); // Pointer to the 1. component
	}

	if (mColorCount) {
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(mColorCount, GL_FLOAT, mVertexSize,
				mVertices.data() + mPosCount + mTexCount * 2);
	}

	for (unsigned int i = 0; i < mTexCount; ++i) {
		glClientActiveTexture(GL_TEXTURE0 + i);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, mVertexSize,
				mVertices.data() + mPosCount + i * 2);
	}
}

void gs::Mesh::unbindNoVaoVersion(const ShaderProgram* shaderProgram)
{
	if (shaderProgram) {
		const std::vector<Attribute>& attrs = shaderProgram->getAttributes();
		if (!attrs.empty()) {
			for (const auto &a : attrs) {
				if (a.mLocation != -1) {
					glDisableVertexAttribArray(a.mLocation);
				}
			}
			return;
		}
	}

	if (mPosCount) {
		glDisableClientState(GL_VERTEX_ARRAY);
	}
	if (mColorCount) {
		glDisableClientState(GL_COLOR_ARRAY);
	}

	for (unsigned int i = 0; i < mTexCount; ++i) {
		glClientActiveTexture(GL_TEXTURE0 + i);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	}
}

void gs::Mesh::bindVaoVersion(const ShaderProgram* shaderProgram)
{
	if (mVbo == 0) {
		glGenBuffers(1, &mVbo);
		// copy vertices array in a buffer for OpenGL
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		mChanged = false;
	}
	else if (mChanged) {
		LOGI("Mesh changed. --> Change data of VBO\n");
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		// see https://www.khronos.org/opengl/wiki/Buffer_Object_Streaming
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), nullptr, GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(float), mVertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		mChanged = false;
	}
	bool shaderHasAttrConfig = shaderProgram ?
			!shaderProgram->getAttributes().empty() : false;

	bool vaoChangeNecessary = false;

	if (mVao &&
			(mShaderAttrConfigIsUsedForVao != shaderHasAttrConfig ||
				(mShaderAttrConfigIsUsedForVao && shaderHasAttrConfig &&
						(mShaderForVao != shaderProgram ||
						mShaderLoadCounterForVao != shaderProgram->getLoadCounter())
				)
			)
	) {
		LOGI("VAO infos must be changed!\n");
		vaoChangeNecessary = true;
	}

	if (mVao == 0 || vaoChangeNecessary) {
		if (mVao) {
			glDeleteVertexArrays(1, &mVao);
			LOGI("Recreate VAO\n");
		}
		createVao(shaderProgram); // also calls glBindVertexArray(mVao) --> after that no extra call is necessary
		mShaderAttrConfigIsUsedForVao = shaderHasAttrConfig;
		mShaderForVao = shaderProgram;
		mShaderLoadCounterForVao = shaderProgram ? shaderProgram->getLoadCounter() : 0;
	}
	else {
		glBindVertexArray(mVao);
	}
}

void gs::Mesh::unbindVaoVersion(const ShaderProgram* shaderProgram)
{
	glBindVertexArray(0);
}

void gs::Mesh::createVao(const ShaderProgram* shaderProgram)
{
	glGenVertexArrays(1, &mVao);
	glBindVertexArray(mVao);
	glBindBuffer(GL_ARRAY_BUFFER, mVbo);

	if (shaderProgram) {
		const std::vector<Attribute>& attrs = shaderProgram->getAttributes();
		if (!attrs.empty()) {
			for (const auto &a : attrs) {
				if (a.mLocation != -1) {
					glVertexAttribPointer(a.mLocation, // index
							a.mCompCount, // components pro Vertex 2 for (x,y), 3 for (x,y,z)
							GL_FLOAT, // type of component
							GL_FALSE, // normalized
							mVertexSize, // offset between 2 vertices in array
							(float*)(nullptr) + a.mCompOffset); // Pointer to the 1. component
					glEnableVertexAttribArray(a.mLocation);
				}
			}
			return;
		}
	}

	if (mPosCount) {
		glVertexAttribPointer(0, // index
				mPosCount, // components pro Vertex 2 for (x,y), 3 for (x,y,z)
				GL_FLOAT, // type of component
				GL_FALSE, // normalized
				mVertexSize, // offset between 2 vertices in array
				(float*)(nullptr) + 0); // Pointer to the 1. component
		glEnableVertexAttribArray(0);
	}

	if (mColorCount) {
		glVertexAttribPointer(1, // index
				mColorCount, // components pro Vertex 2 for (x,y), 3 for (x,y,z)
				GL_FLOAT, // type of component
				GL_FALSE, // normalized
				mVertexSize, // offset between 2 vertices in array
				(float*)(nullptr) + mPosCount + mTexCount * 2); // Pointer to the 1. component
		glEnableVertexAttribArray(1);
	}

	for (unsigned int i = 0; i < mTexCount; ++i) {
		glVertexAttribPointer(2 + i, // index
				2, // components pro Vertex 2 for (x,y), 3 for (x,y,z)
				GL_FLOAT, // type of component
				GL_FALSE, // normalized
				mVertexSize, // offset between 2 vertices in array
				(float*)(nullptr) + mPosCount + i * 2); // Pointer to the 1. component
		glEnableVertexAttribArray(2 + i);
	}
}

