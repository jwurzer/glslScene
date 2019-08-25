#ifndef GLSLSCENE_SCRIPT_LOGIC_H
#define GLSLSCENE_SCRIPT_LOGIC_H

#include <gs/logic/logic.h>
#include <gs/common/vector2.h>
#include <picoc.h>
#include <string>

namespace gs
{
	class FileChangeMonitoring;

	class ScriptLogic: public Logic
	{
	public:
		ScriptLogic(const std::string& scriptFilename,
				const std::weak_ptr<FileChangeMonitoring>& fcm);
		virtual ~ScriptLogic();

		virtual void handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p, const SDL_Event& evt) override;
		virtual void update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p) override;
	private:
		std::string mScriptFilename;
		std::weak_ptr<FileChangeMonitoring> mFcm;
		Picoc mPc;
		bool mIsLoaded;
		bool mIsFirst;
		bool mIsScriptError;
		unsigned int mFileId;

		bool reload();
		bool load();
		void unload();

		void runScriptFunction(bool isUpdateCall,
				const std::shared_ptr<Entity>& e,
				ResourceManager& rm,
				const Properties& p,
				const SDL_Event* evt);
		static void hotReloadingCallback(unsigned int callbackId,
				const std::string& filename,
				const std::shared_ptr<void>& param1, void* scriptLogic);
		void hotReloading(unsigned int callbackId,
				const std::string& filename,
				const std::shared_ptr<void>& param1);
	};
}

#endif
