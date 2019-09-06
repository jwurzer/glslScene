#ifndef GLSLSCENE_SCENE_H
#define GLSLSCENE_SCENE_H

#include <gs/scene/view.h>
#include <memory>
#include <string>
#include <map>

namespace cfg
{
	class NameValuePair;
}

namespace gs
{
	class Entity;
	class ResourceManager;
	class FileChangeMonitoring;

	/**
	 * For the scene format see at the end of this file.
	 */
	class Scene
	{
	public:
		typedef std::map<std::string /* entity id for search */, std::shared_ptr<Entity> > TIdMap;

		Scene();
		~Scene();

		bool updateAndLoad(
				const std::weak_ptr<FileChangeMonitoring>& fcm,
				const cfg::NameValuePair& cfg, ResourceManager& rm);

		void setIdNumber(unsigned int idNumber) { mIdNumber = idNumber; }
		unsigned int getIdNumber() const { return mIdNumber; }

		/**
		 * @return Name of the scene
		 */
		const std::string& getIdName() const { return mIdName; }
		View& getView() { return mView; }
		std::shared_ptr<Entity> getRootOe() const { return mRoot; }
		std::shared_ptr<Entity> getEntity(const std::string& entityIdName) const;
	private:
		unsigned int mIdNumber;
		std::string mIdName;

		View mView;
		// root of scene
		std::shared_ptr<Entity> mRoot;
		TIdMap mIdMap;
	};
}

#endif
