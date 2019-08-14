#ifndef GLSLSCENE_CONTEXT_H
#define GLSLSCENE_CONTEXT_H

#include <gs/rendering/properties.h>
#include <gs/context_properties.h>
#include <SDL.h>
#include <memory>

namespace gs
{
	class CfgValuePair;
	class ResourceManager;
	class SceneManager;
	class RenderPassManager;
	class GuiManager;
	class FileChangeMonitoring;

	class Context
	{
	public:
		Context();
		~Context();
		bool isError() const { return mIsError; }
		// return true if game loop was running (and scene was loaded)
		// return false if an error happend (game loop not running). e.g. loading error
		bool run();

	private:
		std::unique_ptr<CfgValuePair> mSceneConfig;

		// to change the parameters for context creation,
		// the scene config should be used!
		ContextProperties mContextProperties;

		bool mIsError;
		bool mIsSdlInit;
		SDL_Window* mWindow;
		//OpenGL context
		SDL_GLContext mContext; // SDL_GLContext is a void* (typedef void *SDL_GLContext)

		std::string mSceneFilename;
		unsigned int mHotReloadingId = 0;
		std::shared_ptr<FileChangeMonitoring> mFileMonitoring;
		bool mReloadSceneNow = false;

		Properties mProperties;

		std::unique_ptr<ResourceManager> mResourceManager;
		std::unique_ptr<SceneManager> mSceneManager;
		std::unique_ptr<RenderPassManager> mPassManager;
		std::unique_ptr<GuiManager> mGuiManager;

		// select and load config file for scene
		bool selectScene();
		void initContext();
		void destroyContext();

		static void hotReloading(unsigned int callbackId,
				const std::string& filename,
				const std::shared_ptr<void>& fileMonitoring, void* thisContext);
		void hotReloadingSceneFile(unsigned int callbackId, const std::string& filename);
		void reload();
	};
}

#endif

