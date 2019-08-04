#include <scene/scene_manager.h>

#include <scene/scene.h>
#include <system/log.h>
#include <ecs/entity.h>
#include <ecs/logic_component.h>
#include <ecs/child_entities.h>

gs::SceneManager::SceneManager()
		:mSceneByIdName(), mSceneByIdNumber(), mNextFreeSceneId(2)
{
}

gs::SceneManager::~SceneManager()
{
}

unsigned int gs::SceneManager::addScene(const std::shared_ptr<Scene>& scene)
{
	std::string idName = scene->getIdName();
	if (idName.empty()) {
		LOGE("A empty id name for a scene is not allowed.\n");
		return 0;
	}
	TSceneByIdNameMap::const_iterator itByName = mSceneByIdName.find(idName);
	if (itByName != mSceneByIdName.cend()) {
		LOGE("id name '%s' is already used for a scene.\n", idName.c_str());
		return 0;
	}
	unsigned int idNumber = mNextFreeSceneId;
	++mNextFreeSceneId;

	//scene->setIdName(idName);
	scene->setIdNumber(idNumber);

	mSceneByIdName[idName] = scene;
	mSceneByIdNumber[idNumber] = scene;
	return idNumber;
}

unsigned int gs::SceneManager::getSceneIdByIdName(const std::string& idName) const
{
	TSceneByIdNameMap::const_iterator it = mSceneByIdName.find(idName);
	if (it == mSceneByIdName.cend()) {
		return 0;
	}
	return it->second->getIdNumber();
}

std::shared_ptr<gs::Scene> gs::SceneManager::getSceneByIdName(const std::string& idName) const
{
	TSceneByIdNameMap::const_iterator it = mSceneByIdName.find(idName);
	if (it == mSceneByIdName.cend()) {
		return std::shared_ptr<gs::Scene>();
	}
	return it->second;
}

std::shared_ptr<gs::Scene> gs::SceneManager::getSceneByIdNumber(unsigned int idNumber) const
{
	TSceneByIdNumberMap::const_iterator it = mSceneByIdNumber.find(idNumber);
	if (it == mSceneByIdNumber.cend()) {
		return std::shared_ptr<gs::Scene>();
	}
	return it->second;
}

void gs::SceneManager::handleEvent(ResourceManager& rm,
		const Properties& properties, const SDL_Event& evt)
{
	for (const auto& scene : mSceneByIdNumber) {
		handleEventEntity(scene.second->getRootOe(), rm, properties, evt);
	}
}

void gs::SceneManager::update(ResourceManager& rm, const Properties& properties)
{
	for (const auto& scene : mSceneByIdNumber) {
		updateEntity(scene.second->getRootOe(), rm, properties);
	}
}

void gs::SceneManager::handleEventEntity(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& properties, const SDL_Event& evt)
{
	if (e->getConstLogic()) {
		e->logic().handleEvent(rm, properties, evt);
	}

	const gs::ChildEntities* children = e->getConstChildEntities();
	if (children) {
		for (const std::shared_ptr<Entity>& child : children->getEntities()) {
			handleEventEntity(child, rm, properties, evt);
		}
	}
}

void gs::SceneManager::updateEntity(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& properties)
{
	if (e->getConstLogic()) {
		e->logic().update(rm, properties);
	}

	const gs::ChildEntities* children = e->getConstChildEntities();
	if (children) {
		for (const std::shared_ptr<Entity>& child : children->getEntities()) {
			updateEntity(child, rm, properties);
		}
	}
}

