#ifndef GLSLSCENE_SCENE_MANAGER_H
#define GLSLSCENE_SCENE_MANAGER_H

#include <string>
#include <memory>
#include <map>
#include <SDL.h>

namespace gs
{
	class Scene;
	class Entity;
	class ResourceManager;
	class Properties;

	class SceneManager
	{
	public:
		typedef std::map<std::string /* id name */, std::shared_ptr<Scene> > TSceneByIdNameMap;
		typedef std::map<unsigned int /* id number */, std::shared_ptr<Scene> > TSceneByIdNumberMap;

		SceneManager();
		~SceneManager();

		unsigned int addScene(const std::shared_ptr<Scene>& scene);
		unsigned int getSceneIdByIdName(const std::string& idName) const;
		std::shared_ptr<Scene> getSceneByIdName(const std::string& idName) const;
		std::shared_ptr<Scene> getSceneByIdNumber(unsigned int idNumber) const;

		void handleEvent(ResourceManager& rm, const Properties& properties,
				const SDL_Event& evt);
		void update(ResourceManager& rm, const Properties& properties);

		unsigned int getSceneCount() const { return mSceneByIdNumber.size(); }

		const TSceneByIdNumberMap& getSceneMapByIdNumber() const { return mSceneByIdNumber; }
	private:
		TSceneByIdNameMap mSceneByIdName;
		TSceneByIdNumberMap mSceneByIdNumber;

		unsigned int mNextFreeSceneId;

		void handleEventEntity(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& properties, const SDL_Event& evt);
		void updateEntity(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& properties);
	};
}

#endif //GLSLSCENE_SCENE_MANAGER_H
