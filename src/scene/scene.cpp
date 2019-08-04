#include <scene/scene.h>

#include <ecs/entity.h>
#include <configloader/scene_loader.h>

gs::Scene::Scene()
		:mIdNumber(0), mIdName(), mView(ViewMode::WINDOW_SIZE),
		mRoot(Entity::create()), mIdMap()
{
}

gs::Scene::~Scene()
{
	mRoot->removeMeFromParent();
}

bool gs::Scene::updateAndLoad(const CfgValuePair& cfg, ResourceManager& rm)
{
	return sceneloader::updateAndLoad(mIdName, mRoot, mIdMap, cfg, rm);
}

std::shared_ptr<gs::Entity> gs::Scene::getEntity(const std::string& entityIdName) const
{
	const auto it = mIdMap.find(entityIdName);
	if (it == mIdMap.end()) {
		return std::shared_ptr<Entity>();
	}
	return it->second;
}

