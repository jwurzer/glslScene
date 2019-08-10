#ifndef GLSLSCENE_SCENE_LOADER_H
#define GLSLSCENE_SCENE_LOADER_H

/*

# id = <id-scene-name> (optional)
# ====================
# Before the first entity a id for the scene can be used.

# entity
# ======
# order of components or entity properties
# id = <id-name> (type: text, optional, is used to search for a entity at the scene, must be unique per scene)
# name = <name> (property, type: text, optional, doesn't have to be unique)
# active = <true-or-false> (property, type: bool, optional, default: true)
# logic
# transform
# transform2d-ex
# transform3d
# clipping2d
# texture
# shader
# mesh
# text2d
# entity (for using childEntities)

# logic
# =====
# glsl-sandbox-logic

# transform
# =========
# translate <x> <y>
# scale <scale-x> <scale-y>
# scale <scale-x> <scale-y> <center-x> <center-y>
# rotate-degree <angle>
# rotate-degree <angle> <center-x> <center-y>
# rotate-radian <angle>
# rotate-radian <angle> <center-x> <center-y>
# matrix <a> <b> <c> <d> <e> <f>

# clipping2d
# ==========
# rect = <left> <top> <width> <height>

# texture
# =======
# tex-id-unit-0 = <id-name-or-null>
# tex-id-unit-1 = <id-name-or-null>
# tex-id-unit-2 = <id-name-or-null>
# tex-id-unit-3 = <id-name-or-null>
# tex-id-unit-4 = <id-name-or-null>
# tex-id-unit-5 = <id-name-or-null>
# tex-id-unit-6 = <id-name-or-null>
# tex-id-unit-7 = <id-name-or-null>

# shader
# ======
# shader-program-id = <id-name-or-null>

# mesh
# ====
# mesh-id = <id-name>
# mesh
# svg (currently not supported)
# color = <red> <green> <blue> <alpha>

# text2d
# =========
# font-id = <id-name> or default or default-mono
# text = <text>
# fill-color = <red> <green> <blue> <alpha>

*/

#include <gs/scene/scene.h>
#include <memory>

namespace gs
{
	class Entity;
	class ResourceManager;
	class SceneManager;
	class RenderPassManager;
	class CfgValue;
	class CfgValuePair;
	class TransformComponent;

	namespace sceneloader
	{
		bool reload(const CfgValuePair& cfg,
				ResourceManager& rm, SceneManager& sm, RenderPassManager& pm,
				bool reloadResourceManager, bool reloadSceneManager, bool reloadRenderPassManager);

		bool addTransform(const CfgValue& cfgValue,
				TransformComponent& transform2d);

		bool updateAndLoad(const CfgValuePair& cfg,
				SceneManager& sm, ResourceManager& rm);

		bool updateAndLoad(std::string& sceneIdName,
				std::shared_ptr<Entity>& root,
				Scene::TIdMap& idMap, const CfgValuePair& cfg,
				ResourceManager& rm);

		std::shared_ptr<Entity> loadScene(
				const CfgValuePair& cfg,
				bool alwaysAddExtraRootEntity,
				ResourceManager& rm);

		std::shared_ptr<Entity> loadScene(
				const CfgValuePair& cfg,
				Scene::TIdMap& idMap, bool alwaysAddExtraRootEntity,
				ResourceManager& rm);

		/**
		 * @param sceneIdName The id-name for the scene is only stored at this
		 *        parameter if a id for the scene is used in the file.
		 *        If no id is used then this parameter is unchanged.
		 */
		bool reloadScene(std::shared_ptr<Entity>& root,
				const CfgValuePair& cfg,
				Scene::TIdMap& idMap,
				bool alwaysAddExtraRootEntity,
				ResourceManager& rm, std::string& sceneIdName);
	}
}

#endif //GLSLSCENE_SCENE_LOADER_H
