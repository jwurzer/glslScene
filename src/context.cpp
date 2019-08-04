#include <context.h>

#include <rendering/gl_api.h>
#include <rendering/renderer.h>
#include <rendering/render_pass_manager.h>
#include <system/file_change_monitoring.h>
#include <system/log.h>
#include <ecs/entity.h>
#include <res/mesh.h>
#include <res/resource_manager.h>
#include <scene/scene_manager.h>
#include <common/vertex.h>
#include <common/fs.h>
#include <configloader/sml_parser.h>
#include <configloader/scene_loader.h>
#include <common/cfg.h>
#include <scene/scene.h>
#include <camera.h>
#include <glm/gtc/quaternion.hpp>
#include <stdio.h>
#include <string>
#include <iostream>
#include <iomanip>      // std::setw
#include <vector>
#include <configloader/context_loader.h>
#include <configloader/res_loader.h>
#include <configloader/render_pass_loader.h>

//Screen dimension constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

namespace gs
{
	namespace
	{
		void moveWorldForCam(const Camera &cam)
		{
			if (cam.isPerspective()) {
				glm::vec3 eye, center, up;
				cam.getPropertiesForGluLookAt(eye, center, up);
				gluLookAt(eye.x, eye.y, eye.z, // eye x, y, z
						center.x, center.y, center.z, // center x, y, z
						up.x, up.y, up.z); // up vector x, y, z
			}
		}
	}
}

gs::Context::Context()
		:mIsError(true), mIsSdlInit(false), //mIsSdlImageInit(false),
		mWindow(nullptr), mContext(nullptr),
		mResourceManager(), mSceneManager(), mPassManager()
{
	mProperties.mWindowSizeI.mWidth = SCREEN_WIDTH;
	mProperties.mWindowSizeI.mHeight = SCREEN_HEIGHT;
	mProperties.mWindowSize.mWidth = static_cast<float>(mProperties.mWindowSizeI.mWidth);
	mProperties.mWindowSize.mHeight = static_cast<float>(mProperties.mWindowSizeI.mHeight);

	if (!selectScene()) {
		LOGE("Can't load scene file\n");
		mIsError = true;
		return;
	}
	if (!contextloader::getContextParameters(*mSceneConfig, mRenderApiVersion,
			mProfile, mForward, mMajorVersion, mMinorVersion)) {
		LOGW("Warning: Can't load parameters for context creation!\n");
	}
	initContext();
}

gs::Context::~Context()
{
	destroyContext();
}

void gs::Context::run()
{
	if (mIsError) {
		return;
	}

	uint32_t prevFpsTick = SDL_GetTicks();
	unsigned int frameCnt = 0;

	uint32_t prevTick = prevFpsTick;

	glClearColor(0, 0, 0, 1);

	mFileMonitoring = std::make_shared<FileChangeMonitoring>(false);
	Renderer renderer;
	renderer.switchToTextureUnitForLoading();

	bool running = true;

	const CfgValuePair& cfg = *mSceneConfig;

	mResourceManager.reset(new ResourceManager(mFileMonitoring, useVaoVersionForMesh()));
	mSceneManager.reset(new SceneManager());
	mPassManager.reset(new RenderPassManager());
	if (!sceneloader::reload(cfg, *mResourceManager, *mSceneManager, *mPassManager, true, true, true)) {
		LOGE("Can't load scene file.\n");
		return;
	}

	mHotReloadingId = mFileMonitoring->addFile(mSceneFilename, hotReloading, std::shared_ptr<void>(), this);

	bool newLoaded = true;

	while (running) {
		++frameCnt;

		mFileMonitoring->checkChanges();

		if (mReloadSceneNow) {
			reload();
			newLoaded = true;
		}

		SDL_Event e;
		uint32_t tick = SDL_GetTicks();

		uint32_t dtMsec = tick - prevTick;
		float tsSec = float(tick) * 0.001f;
		float dtSec = float(dtMsec) * 0.001f;

		mProperties.mTsMsec = tick;
		mProperties.mTsSec = tsSec;
		mProperties.mDeltaTimeSec = dtSec;

		if (newLoaded) {
			SDL_Event re; // resize event
			re.type = SDL_WINDOWEVENT;
			re.window.event = SDL_WINDOWEVENT_RESIZED;
			re.window.data1 = mProperties.mWindowSizeI.mWidth;
			re.window.data2 = mProperties.mWindowSizeI.mHeight;
			re.window.timestamp = tick;
			re.window.windowID = 0;
			mSceneManager->handleEvent(*mResourceManager, mProperties, re);
		}

		newLoaded = false;

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT:
					running = false;
					break;
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
						mProperties.mWindowSizeI.mWidth = e.window.data1;
						mProperties.mWindowSizeI.mHeight = e.window.data2;
						mProperties.mWindowSize.mWidth = static_cast<float>(mProperties.mWindowSizeI.mWidth);
						mProperties.mWindowSize.mHeight = static_cast<float>(mProperties.mWindowSizeI.mHeight);
						mProperties.mWindowRatio = Size2f(mProperties.mWindowSize.mWidth / mProperties.mWindowSize.mHeight, 1.0f);

						mProperties.mMousePosFactor = Vector2f(
								mProperties.mMousePosPixel.x / mProperties.mWindowSize.mWidth,
								mProperties.mMousePosPixel.y / mProperties.mWindowSize.mHeight);
					}
					break;
				case SDL_MOUSEMOTION:
					mProperties.mMousePosPixel = Vector2f(static_cast<float>(e.motion.x),
							static_cast<float>(mProperties.mWindowSize.mHeight - e.motion.y - 1));
					mProperties.mMousePosFactor = Vector2f(
							mProperties.mMousePosPixel.x / mProperties.mWindowSize.mWidth,
							mProperties.mMousePosPixel.y / mProperties.mWindowSize.mHeight);
#if 0
				LOGI("mouse pos: pixel: %f %f, factor %f %f\n",
					mProperties.mMousePosPixel.x,
					mProperties.mMousePosPixel.y,
					mProperties.mMousePosFactor.x,
					mProperties.mMousePosFactor.y);
#endif
					break;
			}
			// after updating the properties the event can be forwarded to the entities
			mSceneManager->handleEvent(*mResourceManager, mProperties, e);
		}


		mSceneManager->update(*mResourceManager, mProperties);

		mPassManager->renderAllPasses(renderer, *mSceneManager, *mResourceManager, mProperties);

		SDL_GL_SwapWindow(mWindow);

		// tick < prevFpsTick is only possible after ~49 days ;-P
		if (tick > prevFpsTick + 1000 || tick < prevFpsTick) {
			//printf("FPS %u\n", frameCnt);
			SDL_SetWindowTitle(mWindow,
					("glslScene: " + mSceneDirName + ", FPS: " + std::to_string(frameCnt)).c_str());
			prevFpsTick += 1000;
			frameCnt = 0;
		}

		prevTick = tick;
	}
	mFileMonitoring->removeFile(mHotReloadingId);
}

bool gs::Context::selectScene()
{
	std::string cwd = fs::getCwd();
	std::string rootDir = fs::findDirectoryOfFile(cwd, "root-dir-of-glsl-scene.txt");
	if (rootDir != cwd) {
		LOGI("cwd:\n%s\nroot dir of glslScene:\n%s\n",
				cwd.c_str(), rootDir.c_str());
		fs::changeCwd(rootDir);
		LOGI("cwd is changed to %s\n", fs::getCwd().c_str());
	}

	{
		SmlParser selectionParser(rootDir + "/scenes/selection.tml");
		CfgValuePair selectCfg;
		if (!selectionParser.getAsValuePairTree(selectCfg)) {
			LOGE("Can't load selection.tml from scenes correct.\n");
			return false;
		}
		CfgReadRule cfgRules[] = {
				CfgReadRule("scene-name", &mSceneDirName, CfgReadRule::RULE_MUST_EXIST),
				CfgReadRule("")
		};
		size_t nextPos = 0;
		ssize_t storeCnt = selectCfg.mValue.sectionGet(
			cfgRules, false, false, false, false, false, 0, &nextPos);
		if (storeCnt < 0) {
			LOGE("selection config is wrong, rv %d\n", int(storeCnt));
			return false;
		}
		mSceneDirName = fs::getRemoveEndingSlashes(mSceneDirName, '/');
		mSceneDirName = fs::getRemoveEndingSlashes(mSceneDirName, '\\');
	}
	
#if 0
	unsigned int dirCount = 0;
	std::vector<std::string> scenes;
	if (!fs::scanDirectory(rootDir + "/scenes", dirCount, scenes,
			true /* addSubDirs */, false /* addRegularFiles */)) {
		return false;
	}

	unsigned int sceneIndex = 0;
	if (sceneIndex >= dirCount) {
		return false;
	}
	if (!fs::changeCwd(rootDir + "/scenes/" + scenes[sceneIndex])) {
		return false;
	}
	LOGI("cwd is changed to %s\n", fs::getCwd().c_str());

	mSceneDirName = fs::getRemoveEndingSlashes(scenes[sceneIndex], '/');
#else
	if (!fs::changeCwd(rootDir + "/scenes/" + mSceneDirName)) {
		LOGE("Can't change to %s\n", (rootDir + "/scenes/" + mSceneDirName).c_str());
		return false;
	}
	LOGI("cwd is changed to %s\n", fs::getCwd().c_str());
#endif

	mSceneFilename = "scene.tml";
	SmlParser parser(mSceneFilename);
	mSceneConfig.reset(new CfgValuePair());
	if (!parser.getAsValuePairTree(*mSceneConfig)) {
		LOGE("Can't parse scene file. Error: %s\n",
				parser.getExtendedErrorMsg().c_str());
		return false;
	}
	return true;
}

void gs::Context::initContext()
{
	LOGI("Config for context creation:\n%s\n", contextCreationParameters().c_str());

	// Initialize SDL lib
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return;
	}
	mIsSdlInit = true;


	if (mMajorVersion >= 0) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, mMajorVersion);
		LOGI("Set SDL GL context major version to %d\n", mMajorVersion);
	}
	if (mMinorVersion >= 0) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mMinorVersion);
		LOGI("Set SDL GL context minor version to %d\n", mMinorVersion);
	}

	{
		int sdlGlContextProfile = 0;
		if (mRenderApiVersion == RenderingApi::OPENGL_ES) {
			sdlGlContextProfile |= SDL_GL_CONTEXT_PROFILE_ES;
		}
		if (mProfile == RenderingApiProfile::CORE) {
			sdlGlContextProfile |= SDL_GL_CONTEXT_PROFILE_CORE;
		}
		else if (mProfile == RenderingApiProfile::COMPATIBILITY) {
			sdlGlContextProfile |= SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
		}
		if (sdlGlContextProfile != 0) {
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, sdlGlContextProfile);
			LOGI("Set SDL GL context profile mask to%s%s%s\n",
					(sdlGlContextProfile & SDL_GL_CONTEXT_PROFILE_CORE) ?
							" PROFILE_CORE" : "",
					(sdlGlContextProfile & SDL_GL_CONTEXT_PROFILE_COMPATIBILITY) ?
							" PROFILE_COMPATIBILITY" : "",
					(sdlGlContextProfile & SDL_GL_CONTEXT_PROFILE_ES) ?
							" PROFILE_ES" : "");
		}
	}

	// Create window
	mWindow = SDL_CreateWindow(("glslScene: " + mSceneDirName).c_str(),
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!mWindow) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return;
	}

	mContext = SDL_GL_CreateContext(mWindow);
	if (!mContext) {
		printf( "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return;
	}

	if (SDL_GL_MakeCurrent(mWindow, mContext) < 0) {
		fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return;
	}

#ifdef GLSLSCENE_USE_GLEW
	//Initialize GLEW
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK ) {
		printf( "Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		return;
	}
#else
	#ifdef GLSLSCENE_USE_GLAD
	// INITIALIZE GLAD:
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		fprintf(stderr, "gladLoadGLLoader failed!\n");
		return;
	}
	#else
		#error "no gl version for header files is configured"
	#endif
#endif

	SDL_GetWindowSize(mWindow, &mProperties.mWindowSizeI.mWidth, &mProperties.mWindowSizeI.mHeight);
	mProperties.mWindowSize.mWidth = static_cast<float>(mProperties.mWindowSizeI.mWidth);
	mProperties.mWindowSize.mHeight = static_cast<float>(mProperties.mWindowSizeI.mHeight);
	mProperties.mWindowRatio = Size2f(mProperties.mWindowSize.mWidth / mProperties.mWindowSize.mHeight, 1.0f);
	LOGI("Window size: %dx%d\n", mProperties.mWindowSizeI.mWidth, mProperties.mWindowSizeI.mHeight);

	mProperties.mUseGlTransforms = !useVaoVersionForMesh();

	LOGI("use gl transformations: %s\n", mProperties.mUseGlTransforms ? "yes" : "no");
	LOGI("use VAO version for mesh: %s\n", useVaoVersionForMesh() ? "yes" : "no");

	// all ok --> set is error to false ;-)
	mIsError = false;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// LOG OPENGL VERSION, VENDOR (IMPLEMENTATION), RENDERER, GLSL, ETC.:
	std::cout << std::setw(34) << std::left << "OpenGL Version: " <<
			(char*)glGetString(GL_VERSION) << std::endl;
	// GL_SHADING_LANGUAGE_VERSION is not available at OpenGL 1.0
	std::cout << std::setw(34) << std::left << "OpenGL Shading Language Version: " <<
			(char *)glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	std::cout << std::setw(34) << std::left << "OpenGL Vendor:" <<
			(char *)glGetString(GL_VENDOR) << std::endl;
	std::cout << std::setw(34) << std::left << "OpenGL Renderer:" <<
			(char *)glGetString(GL_RENDERER) << std::endl;
}

void gs::Context::destroyContext()
{
	if (mContext) {
		SDL_GL_DeleteContext(mContext);
	}
	if (mWindow) {
		SDL_DestroyWindow(mWindow);
	}
	if (mIsSdlInit) {
		SDL_Quit();
	}
}

std::string gs::Context::contextCreationParameters() const
{
	std::stringstream s;
	s << "Rendering API: ";
	switch (mRenderApiVersion) {
		case RenderingApi::DEFAULT:
			s << "default";
			break;
		case RenderingApi::OPENGL:
			s << "OpenGL";
			break;
		case RenderingApi::OPENGL_ES:
			s << "OpenGL ES";
			break;
	}
	s << ", Profile: ";
	switch (mProfile) {
		case RenderingApiProfile::DEFAULT:
			s << "default";
			break;
		case RenderingApiProfile::CORE:
			s << "core";
			break;
		case RenderingApiProfile::COMPATIBILITY:
			s << "compatibility";
			break;
	}
	s << ", Forward-compatibility: ";
	switch (mForward) {
		case ForwardCompatibility::DEFAULT:
			s << "no (default)";
			break;
		case ForwardCompatibility::FORWARD_COMPATIBILITY:
			s << "yes (activated)";
			break;
	}
	s << ", version: ";
	if (mMajorVersion < 0 && mMinorVersion < 0) {
		s << "default";
	}
	else if (mMajorVersion >= 0 && mMinorVersion < 0) {
		s << mMajorVersion << ".x";
	}
	else if (mMajorVersion < 0 && mMinorVersion >= 0) {
		s << "?." << mMinorVersion << " (incorrect?)";
	}
	else {
		s << mMajorVersion << "." << mMinorVersion;
	}
	return s.str();
}

bool gs::Context::useVaoVersionForMesh() const
{
	if (mRenderApiVersion == RenderingApi::OPENGL &&
			((mMajorVersion == 3 && mMinorVersion >= 2) || mMajorVersion > 3) &&
			mProfile == RenderingApiProfile::CORE) {
		return true;
	}
	if (mRenderApiVersion == RenderingApi::OPENGL_ES &&
			mMajorVersion >= 2) {
		return true;
	}
	return false;
}

void gs::Context::hotReloading(unsigned int callbackId,
		const std::string& filename,
		const std::shared_ptr<void>& fileMonitoring, void* thisContext)
{
	static_cast<Context*>(thisContext)->hotReloadingSceneFile(callbackId, filename);
}

void gs::Context::hotReloadingSceneFile(unsigned int callbackId,
		const std::string& filename)
{
	mReloadSceneNow = true;
}

void gs::Context::reload()
{
	mReloadSceneNow = false;

	std::unique_ptr<CfgValuePair> sceneConfig(new CfgValuePair());
	SmlParser parser(mSceneFilename);
	if (!parser.getAsValuePairTree(*sceneConfig)) {
		LOGE("Parse scene config file failed!\n");
		return;
	}
	bool reloadResourceManager = true;
	bool reloadSceneManager = true;
	bool reloadRenderPassManager = true;
	std::unique_ptr<ResourceManager> resourceManager;
	std::unique_ptr<SceneManager> sceneManager;
	std::unique_ptr<RenderPassManager> passManager;
	ResourceManager* rm = mResourceManager.get();
	SceneManager* sm = mSceneManager.get();
	RenderPassManager* pm = mPassManager.get();
	std::shared_ptr<FileChangeMonitoring> fileMonitoring = std::make_shared<FileChangeMonitoring>(false);
	if (reloadResourceManager) {
		resourceManager.reset(new ResourceManager(fileMonitoring, useVaoVersionForMesh()));
		rm = resourceManager.get();
	}
	if (reloadSceneManager) {
		sceneManager.reset(new SceneManager());
		sm = sceneManager.get();
	}
	if (reloadRenderPassManager) {
		passManager.reset(new RenderPassManager());
		pm = passManager.get();
	}
	LOGI("Start reloading scene.\n");
	if (!sceneloader::reload(*sceneConfig, *rm, *sm, *pm,
			reloadResourceManager, reloadSceneManager, reloadRenderPassManager)) {
		LOGE("Can't load scene file.\n");
		return;
	}
	LOGI("loaded successful.\n");
	mSceneConfig = std::move(sceneConfig);
	if (reloadResourceManager) {
		mResourceManager = std::move(resourceManager);
	}
	if (reloadSceneManager) {
		mSceneManager = std::move(sceneManager);
	}
	if (reloadRenderPassManager) {
		mPassManager = std::move(passManager);
	}
	LOGI("Switch hot reloading.\n");
	mFileMonitoring = fileMonitoring;
	mHotReloadingId = mFileMonitoring->addFile(mSceneFilename, hotReloading, std::shared_ptr<void>(), this);
	LOGI("Reload and apply scene file.\n");
}

