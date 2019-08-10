#include <gs/scene/scene_manager.h>

#include <gs/rendering/matrices.h>
#include <gs/rendering/properties.h>
#include <gs/scene/scene.h>
#include <gs/system/log.h>
#include <gs/ecs/entity.h>
#include <gs/ecs/transform_component.h>
#include <gs/ecs/logic_component.h>
#include <gs/ecs/child_entities.h>

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
		const Properties& propertiesOrig, const SDL_Event& evt)
{
	Properties properties = propertiesOrig;

	Matrices m;
	m.mModelMatrix = properties.mModelMatrix;

	m.mEntityMatrix = glm::mat4(1.0f);
	m.mModelMatrix *= m.mEntityMatrix;
	m.mModelViewMatrix = m.mModelMatrix * properties.mViewMatrix;
	m.mMvpMatrix = properties.mProjectionMatrix * m.mModelViewMatrix;
	properties.mModelMatrix = m.mModelMatrix;

	for (const auto& scene : mSceneByIdNumber) {
		handleEventEntity(scene.second->getRootOe(), rm, properties, evt);
	}
}

void gs::SceneManager::update(ResourceManager& rm, const Properties& propertiesOrig)
{
	Properties properties = propertiesOrig;

	Matrices m;
	m.mModelMatrix = properties.mModelMatrix;

	m.mEntityMatrix = glm::mat4(1.0f);
	m.mModelMatrix *= m.mEntityMatrix;
	m.mModelViewMatrix = m.mModelMatrix * properties.mViewMatrix;
	m.mMvpMatrix = properties.mProjectionMatrix * m.mModelViewMatrix;
	properties.mModelMatrix = m.mModelMatrix;

	for (const auto& scene : mSceneByIdNumber) {
		updateEntity(scene.second->getRootOe(), rm, properties);
	}
}

void gs::SceneManager::handleEventEntity(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& propertiesOrig, const SDL_Event& evt)
{
	Properties properties = propertiesOrig;
	if (e->getConstTransform()) {
		Matrices m;
		m.mModelMatrix = properties.mModelMatrix;

		m.mEntityMatrix = e->getConstTransform()->getMatrix();
		m.mModelMatrix *= m.mEntityMatrix;
		m.mModelViewMatrix = properties.mViewMatrix * m.mModelMatrix;
		m.mMvpMatrix = properties.mProjectionMatrix * m.mModelViewMatrix;
		properties.mModelMatrix = m.mModelMatrix;
	}

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
		const Properties& propertiesOrig)
{
	Properties properties = propertiesOrig;
	if (e->getConstTransform()) {
		Matrices m;
		m.mModelMatrix = properties.mModelMatrix;

		m.mEntityMatrix = e->getConstTransform()->getMatrix();
		m.mModelMatrix *= m.mEntityMatrix;
		m.mModelViewMatrix = properties.mViewMatrix * m.mModelMatrix;
		m.mMvpMatrix = properties.mProjectionMatrix * m.mModelViewMatrix;
		properties.mModelMatrix = m.mModelMatrix;
	}

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

