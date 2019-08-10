#ifndef GLSLSCENE_CONTEXT_H
#define GLSLSCENE_CONTEXT_H

#include <gs/rendering/properties.h>
#include <SDL.h>
#include <memory>

namespace gs
{
	enum class RenderingApi
	{
		DEFAULT, // not specified
		OPENGL,
		OPENGL_ES,
	};

	enum class RenderingApiProfile
	{
		DEFAULT, // not specified
		CORE,
		COMPATIBILITY,
	};

	enum class ForwardCompatibility
	{
		DEFAULT, // not specified, none
		FORWARD_COMPATIBILITY,
	};

	class CfgValuePair;
	class ResourceManager;
	class SceneManager;
	class RenderPassManager;
	class FileChangeMonitoring;

	class Context
	{
	public:
		Context();
		~Context();
		bool isError() const { return mIsError; }
		void run();

	private:
		std::unique_ptr<CfgValuePair> mSceneConfig;

		// to change the parameters for context creation the scene config should
		// be used!
		RenderingApi mRenderApiVersion = RenderingApi::DEFAULT;
		RenderingApiProfile mProfile = RenderingApiProfile::DEFAULT;
		ForwardCompatibility mForward = ForwardCompatibility::DEFAULT;
		int mMajorVersion = -1; // -1 for not specified
		int mMinorVersion = -1; // -1 for not specified

		bool mIsError;
		bool mIsSdlInit;
		SDL_Window* mWindow;
		//OpenGL context
		SDL_GLContext mContext; // SDL_GLContext is a void* (typedef void *SDL_GLContext)

		std::string mSceneDirName;

		std::string mSceneFilename;
		unsigned int mHotReloadingId = 0;
		std::shared_ptr<FileChangeMonitoring> mFileMonitoring;
		bool mReloadSceneNow = false;

		Properties mProperties;

		std::unique_ptr<ResourceManager> mResourceManager;
		std::unique_ptr<SceneManager> mSceneManager;
		std::unique_ptr<RenderPassManager> mPassManager;

		// select and load config file for scene
		bool selectScene();
		void initContext();
		void destroyContext();

		std::string contextCreationParameters() const;

		bool useVaoVersionForMesh() const;

		static void hotReloading(unsigned int callbackId,
				const std::string& filename,
				const std::shared_ptr<void>& fileMonitoring, void* thisContext);
		void hotReloadingSceneFile(unsigned int callbackId, const std::string& filename);
		void reload();
	};
}

#endif

