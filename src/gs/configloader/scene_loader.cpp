#include <gs/configloader/scene_loader.h>
#include <gs/common/cfg.h>
#include <gs/system/log.h>
#include <gs/ecs/entity.h>
#include <gs/ecs/child_entities.h>
#include <gs/ecs/logic_component.h>
#include <gs/ecs/transform_component.h>
#include <gs/ecs/texture_component.h>
#include <gs/ecs/shader_component.h>
#include <gs/ecs/mesh_component.h>
#include <gs/scene/glsl_sandbox_logic.h>
#include <gs/scene/rotate_logic.h>
#include <gs/scene/light_matrix_logic.h>
#include <gs/configloader/uniform_attr_loader.h>
#include <gs/configloader/res_loader.h>
#include <gs/configloader/render_pass_loader.h>
#include <gs/res/resource_manager.h>
#include <gs/rendering/render_pass_manager.h>
#include <gs/scene/scene_manager.h>

namespace gs
{
	namespace
	{
		bool addLogic(const CfgValue& cfgValue,
				LogicComponent& logic) {
			for (const auto &vp : cfgValue.mArray) { // vp ... value pair
				std::string logicName = vp.mName.mText;
				if (vp.mName.mArray.size()) {
					logicName = vp.mName.mArray[0].mValue.mText;
				}

				if (logicName == "glsl-sandbox-logic") {
					logic.addLogic(std::unique_ptr<GlslSandboxLogic>(new GlslSandboxLogic()));
				}
				else if (logicName == "rotate-logic") {
					if (vp.mName.mArray.size() != 5) {
						LOGE("Wrong rotate-logic format.\n");
						return false;
					}
					logic.addLogic(std::unique_ptr<RotateLogic>(new RotateLogic(
							vp.mName.mArray[1].mValue.mFloatingPoint,
							vp.mName.mArray[2].mValue.mFloatingPoint,
							vp.mName.mArray[3].mValue.mFloatingPoint,
							vp.mName.mArray[4].mValue.mFloatingPoint)));
				}
				else if (logicName == "light-matrix-logic") {
					if (vp.mName.mArray.size() != 3) {
						LOGE("Wrong light-matrix-logic format.\n");
						return false;
					}
					logic.addLogic(std::unique_ptr<LightMatrixLogic>(new LightMatrixLogic(
							vp.mName.mArray[1].mValue.mText,
							vp.mName.mArray[2].mValue.mInteger)));
				}
				else {
					LOGE("Logic %s is not supported.\n", logicName.c_str());
					return false;
				}
			}
			return true;
		}

		bool addTexture(const CfgValue& cfgValue,
				TextureComponent& texture,
				const ResourceManager& rm)
		{
			std::string texIdName[8];
			unsigned int texIdNameCount[8] = {};
			bool texNull[8];
			unsigned int texNullCount[8] = {};
			CfgReadRule cfgRules[] = {
					CfgReadRule("tex-id-unit-0", &texIdName[0], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[0]),
					CfgReadRule("tex-id-unit-0", &texNull[0], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[0]),
					CfgReadRule("tex-id-unit-1", &texIdName[1], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[1]),
					CfgReadRule("tex-id-unit-1", &texNull[1], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[1]),
					CfgReadRule("tex-id-unit-2", &texIdName[2], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[2]),
					CfgReadRule("tex-id-unit-2", &texNull[2], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[2]),
					CfgReadRule("tex-id-unit-3", &texIdName[3], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[3]),
					CfgReadRule("tex-id-unit-3", &texNull[3], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[3]),
					CfgReadRule("tex-id-unit-4", &texIdName[4], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[4]),
					CfgReadRule("tex-id-unit-4", &texNull[4], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[4]),
					CfgReadRule("tex-id-unit-5", &texIdName[5], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[5]),
					CfgReadRule("tex-id-unit-5", &texNull[5], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[5]),
					CfgReadRule("tex-id-unit-6", &texIdName[6], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[6]),
					CfgReadRule("tex-id-unit-6", &texNull[6], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[6]),
					CfgReadRule("tex-id-unit-7", &texIdName[7], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &texIdNameCount[7]),
					CfgReadRule("tex-id-unit-7", &texNull[7], CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &texNullCount[7]),
					CfgReadRule("")
			};
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.sectionGet(
					cfgRules, false, false, true, false, false, 0, &nextPos);
			if (storeCnt < 0) {
				LOGE("Can't read texture section correct! Wrong format! error: %zd\n", storeCnt);
				return false;
			}
			for (int i = 0; i < 8; ++i) {
				if (texIdNameCount[i] + texNullCount[i] > 1) {
					LOGE("tex-id-unit-%d is used twice or more which is not allowed.\n", i);
					return false;
				}
				if (texIdNameCount[i]) {
					TResourceId id = rm.getResourceId(texIdName[i]);
					if (!id) {
						LOGE("Can't find texture id name %s for tex-id-unit-%d.\n", texIdName[i].c_str(), i);
						return false;
					}
					texture.setTextureId(id, i);
				}
				else if (texNullCount[i]) {
					if (!texNull[i]) {
						LOGE("Texture id for tex-id-unit-%d should be null but isn't.\n", i);
						return false;
					}
					texture.setTextureId(0, i);
				}
			}
			return true;
		}

		bool addShader(const CfgValue& cfgValue,
				ShaderComponent& shader,
				const ResourceManager& rm)
		{
			std::string shaderProgramIdName;
			unsigned int shaderProgramIdNameCount;
			bool shaderProgramIdNull;
			unsigned int shaderProgramIdNullCount;

			// for the simple version shader-id instead of shader-program-id
			std::string shaderIdName;
			unsigned int shaderIdNameCount;
			bool shaderIdNull;
			unsigned int shaderIdNullCount;

			CfgReadRule cfgRules[] = {
					CfgReadRule("shader-program-id", &shaderProgramIdName, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &shaderProgramIdNameCount),
					CfgReadRule("shader-program-id", &shaderProgramIdNull, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &shaderProgramIdNullCount),
					CfgReadRule("shader-id", &shaderIdName, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &shaderIdNameCount),
					CfgReadRule("shader-id", &shaderIdNull, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_NULL, &shaderIdNullCount),
					CfgReadRule("")
			};
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.sectionGet(
					cfgRules, false, false, true, false, false, 0, &nextPos);
			if (storeCnt < 0) {
				LOGE("Can't read shader section correct! Wrong format! error: %zd\n", storeCnt);
				return false;
			}
			if (shaderProgramIdNameCount + shaderProgramIdNullCount + shaderIdNameCount + shaderIdNullCount != 1) {
				LOGE("None or more id's for shader are not allowed.\n");
				return false;
			}
			if (shaderIdNameCount) {
				shaderProgramIdName = shaderIdName;
			}
			if (shaderIdNullCount) {
				shaderProgramIdNull = shaderIdNull;
			}
			if (shaderProgramIdNameCount) {
				TResourceId id = rm.getResourceId(shaderProgramIdName);
				if (!id) {
					LOGE("Can't find shader id name %s for shader-program-id.\n", shaderProgramIdName.c_str());
					return false;
				}
				shader.setShaderProgramId(id);
			}
			else {
				shader.setShaderProgramId(0);
			}
			if (nextPos < cfgValue.mArray.size()) {
				std::vector<Uniform> uniforms;
				std::vector<Attribute> attributes;
				if (!uniformattrloader::loadUniformsAndAttributes(uniforms,
						attributes, cfgValue, nextPos, "uniforms")) {
					LOGE("One or more uniforms are not correct.\n");
					return false;
				}
				shader.setUniforms(uniforms);
			}
			return true;
		}

		bool addMesh(const CfgValue& cfgValue,
				MeshComponent& mesh,
				ResourceManager& rm)
		{
			std::string meshIdName;
			unsigned int meshIdNameCount = 0;
			int pageIndex = 0;
			unsigned int pageIndexCount = 0;
			const CfgValuePair* vgArray = nullptr;
			unsigned int vgArrayCount = 0;
			const std::vector<CfgValuePair>* color = nullptr;
			unsigned int colorCount = 0;
			CfgReadRule cfgRulesGraphic[] = {
					CfgReadRule("mesh-id", &meshIdName, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &meshIdNameCount),
					CfgReadRule("mesh", &vgArray, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &vgArrayCount),
					CfgReadRule("color", &color, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &colorCount),
					CfgReadRule("")
			};
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.sectionGet(
					cfgRulesGraphic, false, false, true, false, false, 0, &nextPos);
			if (storeCnt < 0) {
				LOGE("Can't read mesh section correct! Wrong format! error: %zd\n", storeCnt);
				return false;
			}
			if (meshIdNameCount) {
				TResourceId id = rm.getResourceId(meshIdName);
				if (!id) {
					LOGE("Can't find graphic id name %s for graphic-id.\n", meshIdName.c_str());
					return false;
				}
				mesh.setGraphicId(id);
			}
			if (vgArray) {
				TResourceId id = resloader::addMesh(rm, *vgArray);
				if (!id) {
					LOGE("Can't create mesh for graphic id %s.\n", meshIdName.c_str());
					return false;
				}
				mesh.setGraphicId(id);
			}
			if (colorCount) {
				const std::vector<CfgValuePair>& arr = *color;
				if (arr.size() != 4) {
					return false;
				}
				mesh.setColor(arr[0].mValue.mFloatingPoint,
						arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
						arr[3].mValue.mFloatingPoint);
			}
			return true;
		}

		/**
		 * @param startIndex Index of the first sub-entity
		 */
		bool loadEntities(const CfgValue& cfgValue,
				unsigned int startIndex,
				const std::shared_ptr<Entity>& oe,
				Scene::TIdMap& idMap,
				ResourceManager& rm,
				bool isTkObjectAllowed)
		{
			/*
			order of components or entity properties
			id (type: text, optional, is used to search for a entity at the scene, must be unique per scene)
			name (property, type: text, optional, doesn't have to be unique)
			active (property, type: bool, optional, default: true)
			transform2d
			transform2d-ex
			transform3d
			clipping2d
			texture
			mesh
			text2d
			entity (for using childEntities)
			*/
			std::string idName;
			std::string name;
			bool isActive = false;
			const CfgValue* compLogic = nullptr;
			const CfgValue* compTransform = nullptr;
			//const CfgValue* compTransform2dEx = nullptr;
			//const CfgValue* compTransform3d = nullptr;
			//const CfgValue* compClipping2d = nullptr;
			const CfgValue* compTexture = nullptr;
			const CfgValue* compShader = nullptr;
			const CfgValue* compMesh = nullptr;
			//const CfgValue* compText2d = nullptr;
			unsigned int idNameCount = 0;
			unsigned int nameCount = 0;
			unsigned int isActiveCount = 0;
			unsigned int compLogicCount = 0;
			unsigned int compTransformCount = 0;
			//unsigned int compTransform2dExCount = 0;
			//unsigned int compTransform3dCount = 0;
			//unsigned int compClipping2dCount = 0;
			unsigned int compTextureCount = 0;
			unsigned int compShaderCount = 0;
			unsigned int compMeshCount = 0;
			//unsigned int compText2dCount = 0;

			CfgReadRule cfgRulesEntity[] = {
					CfgReadRule("id", &idName, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &idNameCount),
					CfgReadRule("name", &name, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &nameCount),
					CfgReadRule("active", &isActive, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_TEXT, &isActiveCount),
					CfgReadRule("logic-component", &compLogic, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compLogicCount),
					CfgReadRule("transform-component", &compTransform, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compTransformCount),
					//CfgReadRule("transform2d-ex-component", &compTransform2dEx, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compTransform2dExCount),
					//CfgReadRule("transform3d-component", &compTransform3d, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compTransform3dCount),
					//CfgReadRule("clipping2d-component", &compClipping2d, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compClipping2dCount),
					CfgReadRule("texture-component", &compTexture, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compTextureCount),
					CfgReadRule("shader-component", &compShader, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compShaderCount),
					CfgReadRule("mesh-component", &compMesh, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compMeshCount),
					//CfgReadRule("text2d-component", &compText2d, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ARRAY, &compText2dCount),
					CfgReadRule("")
			};
			unsigned int cnt = cfgValue.mArray.size();
			for (unsigned int i = startIndex; i < cnt; ++i) {
				const auto& vp = cfgValue.mArray[i]; // vp ... value pair
				if (vp.mName.mText != "entity") {
					LOGE("Only a entity is here allowed! '%s' is not supported here\n",
							vp.mName.mText.c_str());
					vp.mName.printFilePositionAsError();
					return false;
				}
				size_t nextPos = 0;
				ssize_t storeCnt = vp.mValue.sectionGet(
						cfgRulesEntity, false, false, true, false, false, 0, &nextPos);
				if (storeCnt < 0) {
					LOGE("Can't read entity section correct! Wrong format! error: %zd\n", storeCnt);
					vp.mValue.printFilePositionAsError();
					return false;
				}
				//LOGI("storeCnt %zd, nextPos %zu, array size %zu\n", storeCnt, nextPos, vp.mValue.mArray.size());
				std::shared_ptr<Entity> child = Entity::create();
				oe->childEntities().addChild(child);
				if (idNameCount) {
					if (!idMap.insert(Scene::TIdMap::value_type(idName, child)).second) {
						LOGE("id name '%s' already used.\n", idName.c_str());
						return false;
					}
				}
				if (nameCount) {
					child->setName(name.c_str());
				}
				if (isActiveCount) {
					child->setActive(isActive);
				}
				if (compLogicCount) {
					if (!addLogic(*compLogic, child->logic())) {
						LOGE("Add logic component failed\n");
						return false;
					}
				}
				if (compTransformCount) {
					if (!sceneloader::addTransform(*compTransform, child->transform2d())) {
						LOGE("Add transform component failed\n");
						return false;
					}
				}
				if (compTextureCount) {
					if (!addTexture(*compTexture, child->texture(), rm)) {
						LOGE("Add texture component failed\n");
						return false;
					}
				}
				if (compShaderCount) {
					if (!addShader(*compShader, child->shader(), rm)) {
						LOGE("Add shader component failed\n");
						return false;
					}
				}
				if (compMeshCount) {
					if (!addMesh(*compMesh, child->mesh(), rm)) {
						LOGE("Add mesh component failed\n");
						return false;
					}
				}
				if (nextPos < vp.mValue.mArray.size()) {
					if (!loadEntities(vp.mValue, nextPos, child, idMap, rm, isTkObjectAllowed)) {
						LOGE("Child entity error\n");
						return false;
					}
				}
			}
			return true;
		}
	}
}

bool gs::sceneloader::reload(const CfgValuePair& cfg,
		ResourceManager& rm, SceneManager& sm, RenderPassManager& pm,
		bool reloadResourceManager, bool reloadSceneManager, bool reloadRenderPassManager)
{
	const CfgValuePair* contextCfg = nullptr;
	const CfgValuePair* resourcesCfg = nullptr;
	const CfgValuePair* scenesCfg = nullptr;
	const CfgValuePair* renderingCfg = nullptr;
	CfgReadRule cfgRulesGraphic[] = {
			CfgReadRule("context", &contextCfg, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ALL),
			CfgReadRule("resources", &resourcesCfg, CfgReadRule::RULE_MUST_EXIST, CfgReadRule::ALLOW_ALL),
			CfgReadRule("scenes", &scenesCfg, CfgReadRule::RULE_MUST_EXIST, CfgReadRule::ALLOW_ALL),
			CfgReadRule("rendering", &renderingCfg, CfgReadRule::RULE_MUST_EXIST, CfgReadRule::ALLOW_ALL),
			CfgReadRule("")
	};
	size_t nextPos = 0;
	ssize_t storeCnt = cfg.mValue.sectionGet(
			cfgRulesGraphic, false, false, true, false, false, 0, &nextPos);
	if (storeCnt < 0) {
		LOGE("scene-file must have the sections 'resources', 'scenes' and 'rendering'. error: %zd\n", storeCnt);
		return false;
	}

	if (reloadResourceManager) {
		LOGI("Start loading resources.\n");
		resloader::addResources(rm, *resourcesCfg);
		LOGI("res list:\n%s\n", rm.toString().c_str());
	}

	if (reloadSceneManager) {
		if (!sceneloader::updateAndLoad(*scenesCfg, sm, rm)) {
			LOGW("Warning: Can't load scenes successful!\n");
		}
	}

	if (reloadRenderPassManager) {
		if (!renderpassloader::addRenderPasses(pm, sm, rm, *renderingCfg)) {
			LOGW("Warning: Can't load render passes successful!\n");
		}
	}

	if (!pm.isValid()) {
		LOGE("No valid rendering pass\n");
		return false;
	}
	return true;
}

bool gs::sceneloader::addTransform(const CfgValue& cfgValue,
		TransformComponent& transform2d)
{
	for (const auto &vp : cfgValue.mArray) { // vp ... value pair
		const std::vector<CfgValuePair> &arr = vp.mName.mArray;
		size_t cnt = arr.size();
		if (!cnt) {
			return false;
		}
		const std::string &transformName = vp.mName.mArray.front().mValue.mText;
		if (transformName == "translate") {
			// translate <x> <y>
			// translate <x> <y> <z>
			if (cnt == 3) {
				transform2d.translate(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint);
			}
			else if (cnt == 4) {
				transform2d.translate(arr[1].mValue.mFloatingPoint,
						arr[2].mValue.mFloatingPoint, arr[3].mValue.mFloatingPoint);
			}
			else {
				return false;
			}
		} else if (transformName == "scale") {
			// scale <scale-x> <scale-y>
			// scale <scale-x> <scale-y> <center-x> <center-y>
			// scale <scale-x> <scale-y> <scale-z>
			// scale <scale-x> <scale-y> <scale-z> <center-x> <center-y> <center-z>
			if (cnt == 3) {
				transform2d.scale(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint);
			} else if (cnt == 5) {
				transform2d.scale(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
						arr[3].mValue.mFloatingPoint, arr[4].mValue.mFloatingPoint);
			} else if (cnt == 4) {
				transform2d.scale(arr[1].mValue.mFloatingPoint,
						arr[2].mValue.mFloatingPoint, arr[3].mValue.mFloatingPoint);
			} else if (cnt == 7) {
				transform2d.scale(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
						arr[3].mValue.mFloatingPoint, arr[4].mValue.mFloatingPoint,
						arr[5].mValue.mFloatingPoint, arr[6].mValue.mFloatingPoint);
			} else {
				return false;
			}
		} else if (transformName == "rotate-degree") {
			// rotate-degree <angle>
			// rotate-degree <angle> <center-x> <center-y>
			// rotate-degree <angle> <rot-axis-x> <rot-axis-y> <rot-axis-z>
			if (cnt == 2) {
				transform2d.rotateDegree(arr[1].mValue.mFloatingPoint);
			} else if (cnt == 4) {
				transform2d.rotateDegree(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
						arr[3].mValue.mFloatingPoint);
			} else if (cnt == 5) {
				transform2d.rotateDegree(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
						arr[3].mValue.mFloatingPoint, arr[4].mValue.mFloatingPoint);
			} else {
				return false;
			}
		} else if (transformName == "rotate-radian") {
			// rotate-radian <angle>
			// rotate-radian <angle> <center-x> <center-y>
			if (cnt == 2) {
				transform2d.rotateRadian(arr[1].mValue.mFloatingPoint);
			} else if (cnt == 4) {
				transform2d.rotateRadian(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
						arr[3].mValue.mFloatingPoint);
			} else if (cnt == 5) {
				transform2d.rotateRadian(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
						arr[3].mValue.mFloatingPoint, arr[4].mValue.mFloatingPoint);
			} else {
				return false;
			}
		} else if (transformName == "matrix") {
			// matrix <a> <b> <c> <d> <e> <f>
			if (cnt != 7) {
				return false;
			}
			transform2d.matrix(arr[1].mValue.mFloatingPoint, arr[2].mValue.mFloatingPoint,
					arr[3].mValue.mFloatingPoint, arr[4].mValue.mFloatingPoint,
					arr[5].mValue.mFloatingPoint, arr[6].mValue.mFloatingPoint);
		} else {
			return false;
		}
	}
	return true;
}

bool gs::sceneloader::updateAndLoad(const CfgValuePair& cfg,
		SceneManager& sm, ResourceManager& rm)
{
	if (!cfg.mValue.isArray()) {
		return false;
	}
	bool rv = true;
	const std::vector<CfgValuePair>& array = cfg.mValue.mArray;
	for (const CfgValuePair& vp : array) {
		if (vp.mName.mText == "scene") {
			std::shared_ptr<Scene> scene = std::make_shared<Scene>();
			if (!scene->updateAndLoad(vp, rm)) {
				LOGE("Load scene failed.\n");
				rv = false;
				continue;
			}
			if (!sm.addScene(scene)) {
				LOGE("Can't add scene to scene manager.\n");
				rv = false;
				continue;
			}
		}
	}
	return rv;
}

bool gs::sceneloader::updateAndLoad(std::string& sceneIdName,
		std::shared_ptr<Entity>& root,
		Scene::TIdMap& idMap, const CfgValuePair& cfg,
		ResourceManager& rm)
{
	const CfgValuePair* sceneCfg = &cfg;
	if (cfg.mName.mText != "scene") {
		for (const auto &vp : cfg.mValue.mArray) {
			if (vp.mName.mText == "scene") {
				sceneCfg = &vp;
			}
		}
	}
	std::string tmpSceneIdName;
	if (!root->getConstChildEntities()) {
		if (!sceneloader::reloadScene(root, *sceneCfg, idMap, true /* alwaysAddExtraRootEntity */, rm, tmpSceneIdName)) {
			return false;
		}
		sceneIdName = tmpSceneIdName;
		return true;
	}

	LOGI("reload scene\n");
	std::shared_ptr<Entity> tmpRoot;
	Scene::TIdMap tmpIdMap;
	if (!sceneloader::reloadScene(tmpRoot, *sceneCfg, tmpIdMap, true /* alwaysAddExtraRootEntity */, rm, tmpSceneIdName)) {
		return false;
	}
	idMap.swap(tmpIdMap);
	root->childEntities().clear();
	if (tmpRoot && tmpRoot->getConstChildEntities()) {
		ChildEntities& tmpChildEntities = tmpRoot->childEntities();
		// create a copy of the vector
		std::vector<std::shared_ptr<Entity> > tmpChildren = tmpChildEntities.getEntities();
		size_t childrenCount = tmpChildren.size();
		for (size_t i = childrenCount; i --> 0; ) {
			if (!tmpChildEntities.removeChild(tmpChildren[i])) {
				LOGW("Can't remove new child to add it at old root entity\n");
			}
		}
		for (size_t i = 0; i < childrenCount; ++i) {
			if (!root->childEntities().addChild(tmpChildren[i])) {
				LOGW("Can't add new child to old root entity\n");
			}
		}
	}
	sceneIdName = tmpSceneIdName;
	return true;
}

std::shared_ptr<gs::Entity> gs::sceneloader::loadScene(const CfgValuePair& cfg,
		bool alwaysAddExtraRootEntity, ResourceManager& rm)
{
	Scene::TIdMap idMap;
	return loadScene(cfg, idMap, alwaysAddExtraRootEntity, rm);
}

std::shared_ptr<gs::Entity> gs::sceneloader::loadScene(const CfgValuePair& cfg,
		Scene::TIdMap& idMap, bool alwaysAddExtraRootEntity, ResourceManager& rm)
{
	std::shared_ptr<Entity> root;
	std::string sceneIdName;
	if (!reloadScene(root, cfg, idMap, alwaysAddExtraRootEntity, rm, sceneIdName)) {
		return std::shared_ptr<Entity>();
	}
	return root;
}

/**
 * @param sceneIdName The id-name for the scene is only stored at this
 *        parameter if a id for the scene is used in the file.
 *        If no id is used then this parameter is unchanged.
 */
bool gs::sceneloader::reloadScene(std::shared_ptr<Entity>& root,
		const CfgValuePair& cfg,
		Scene::TIdMap& idMap,
		bool alwaysAddExtraRootEntity,
		ResourceManager& rm, std::string& sceneIdName)
{
	if (!cfg.isSection()) {
		return false;
	}
	if (cfg.mValue.mArray.empty()) {
		LOGE("No array entry exist\n");
		return false;
	}
	size_t firstEntityIndex = 0;
	if (cfg.mValue.mArray.front().mName.mText == "id") {
		if (!cfg.mValue.mArray.front().mValue.isText()) {
			LOGE("Id for scene must be a text\n");
			return false;
		}
		sceneIdName = cfg.mValue.mArray.front().mValue.mText;
		++firstEntityIndex;
		if (cfg.mValue.mArray.size() <= 1) {
			LOGE("After id an entity must follow!\n");
			return false;
		}
	}
	if (cfg.mValue.mArray[firstEntityIndex].mName.mText != "entity") {
		LOGE("Must start with a entity (directly or after id)!\n");
		return false;
	}

	if (!root) {
		root = Entity::create();
	}
	else {
		if (root->getConstChildEntities()) {
			root->childEntities().clear();
		}
	}
	if (!loadEntities(cfg.mValue, firstEntityIndex, root, idMap, rm, true)) {
		LOGE("Load scene failed.\n");
		return false;
	}
	if (!alwaysAddExtraRootEntity) {
		// --> not always add root --> check if necessary
		const ChildEntities* children = root->getConstChildEntities();
		if (children && children->getEntities().size() == 1) {
			std::shared_ptr<Entity> newRoot = children->getChild(0);
			if (root->childEntities().removeChild(newRoot)) {
				root = newRoot;
			}
		}
	}
	return true;
}
