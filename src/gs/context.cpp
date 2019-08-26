#include <gs/context.h>

#include <gs/rendering/gl_api.h>
#include <gs/rendering/renderer.h>
#include <gs/rendering/render_pass_manager.h>
#include <gs/system/file_change_monitoring.h>
#include <gs/system/log.h>
#include <gs/ecs/entity.h>
#include <gs/res/mesh.h>
#include <gs/res/resource_manager.h>
#include <gs/scene/scene_manager.h>
#include <gs/gui/gui_manager.h>
#include <gs/common/vertex.h>
#include <gs/common/fs.h>
#include <gs/configloader/sml_parser.h>
#include <gs/configloader/scene_loader.h>
#include <gs/common/cfg.h>
#include <gs/scene/scene.h>
#include <gs/camera.h>
#include <gs/configloader/context_loader.h>
#include <gs/configloader/res_loader.h>
#include <gs/configloader/render_pass_loader.h>
#include <glm/gtc/quaternion.hpp>
#include <stdio.h>
#include <string>
#include <iostream>
#include <iomanip>      // std::setw
#include <vector>

//Screen dimension constants
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

gs::Context::Context(const std::string& progname)
		:mIsError(true), mIsSdlInit(false), //mIsSdlImageInit(false),
		mWindow(nullptr), mContext(nullptr),
		mResourceManager(), mSceneManager(), mPassManager()
{
	mProperties.mWindowSizeI.mWidth = SCREEN_WIDTH;
	mProperties.mWindowSizeI.mHeight = SCREEN_HEIGHT;
	mProperties.mWindowSize.mWidth = static_cast<float>(mProperties.mWindowSizeI.mWidth);
	mProperties.mWindowSize.mHeight = static_cast<float>(mProperties.mWindowSizeI.mHeight);

	if (!selectScene(progname)) {
		LOGE("Can't load scene file\n");
		mIsError = true;
		return;
	}
	if (!contextloader::getContextParameters(*mSceneConfig, mContextProperties)) {
		LOGW("Warning: Can't load parameters for context creation!\n");
	}
	initContext();
}

gs::Context::~Context()
{
	destroyContext();
}

bool gs::Context::run()
{
	if (mIsError) {
		return false;
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

	mResourceManager.reset(new ResourceManager(mFileMonitoring, mContextProperties.useVaoVersionForMesh()));
	mSceneManager.reset(new SceneManager());
	mPassManager.reset(new RenderPassManager());
	if (!sceneloader::reload(cfg, mFileMonitoring, *mResourceManager, *mSceneManager, *mPassManager, true, true, true)) {
		LOGE("Can't load scene file.\n");
		return false;
	}

	mHotReloadingId = mFileMonitoring->addFile(mSceneFilename, hotReloading, std::shared_ptr<void>(), this);

	bool newLoaded = true;

	bool isWireframe = false;
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
			mPassManager->handleEventForCameras(re);
			mGuiManager->handleEvent(re);
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
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_ESCAPE) {
						mGuiManager->toggleEnableDisable();
					}
					else if (e.key.keysym.sym == '1') {
						isWireframe = !isWireframe;
						if (isWireframe) {
							glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						}
						else {
							glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						}
					}
					else if (e.key.keysym.sym == '2') {
						mProperties.mDrawNormals = !mProperties.mDrawNormals;
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
			mPassManager->handleEventForCameras(e);
			mGuiManager->handleEvent(e);
		}


		mSceneManager->update(*mResourceManager, mProperties);
		//mPassManager->updateCameras(mProperties.mDeltaTimeSec);
		mPassManager->updateCameras(mProperties.mTsSec);

		mPassManager->renderAllPasses(renderer, *mSceneManager, *mResourceManager, mProperties);
		//glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		mGuiManager->render(renderer, *mPassManager, *mSceneManager, *mResourceManager,
				mContextProperties, mProperties, *mFileMonitoring);

		SDL_GL_SwapWindow(mWindow);

		// tick < prevFpsTick is only possible after ~49 days ;-P
		if (tick > prevFpsTick + 1000 || tick < prevFpsTick) {
			//LOGI("FPS %u\n", frameCnt);
			SDL_SetWindowTitle(mWindow,
					("glslScene: " + mContextProperties.mSceneDirName +
					", FPS: " + std::to_string(frameCnt) +
					"   -   ESC for enable/disable menu").c_str());
			prevFpsTick += 1000;
			frameCnt = 0;
		}

		prevTick = tick;
	}
	mFileMonitoring->removeFile(mHotReloadingId);
	return true;
}

bool gs::Context::selectScene(const std::string& progname)
{
	LOGI("binary: %s\n", progname.c_str());
	std::string cwd = fs::getCwd();
	std::string rootDir = fs::findDirectoryOfFile(cwd, "root-dir-of-glsl-scene.txt");
	if (rootDir != cwd) {
		LOGI("cwd:\n%s\nroot dir of glslScene:\n%s\n",
				cwd.c_str(), rootDir.c_str());
		if (rootDir.empty()) {
			LOGI("Can't find root-dir-of-glsl-scene.txt. Try it with path from program filename.\n");
			std::string dirname = fs::getDirnameFromPath(progname);
			rootDir = fs::findDirectoryOfFile(dirname, "root-dir-of-glsl-scene.txt");
			if (rootDir.empty()) {
				LOGE("Can't find root-dir-of-glsl-scene.txt");
				return false;
			}
		}
		fs::changeCwd(rootDir);
		LOGI("cwd is changed to %s (%s)\n", fs::getCwd().c_str(), rootDir.c_str());
	}

	{
		SmlParser selectionParser("scenes/selection.tml");
		CfgValuePair selectCfg;
		if (!selectionParser.getAsValuePairTree(selectCfg)) {
			LOGE("Can't load selection.tml from scenes correct.\n");
			return false;
		}
		CfgReadRule cfgRules[] = {
				CfgReadRule("scene-name", &mContextProperties.mSceneDirName, CfgReadRule::RULE_MUST_EXIST),
				CfgReadRule("")
		};
		size_t nextPos = 0;
		ssize_t storeCnt = selectCfg.mValue.sectionGet(
			cfgRules, false, false, false, false, false, 0, &nextPos);
		if (storeCnt < 0) {
			LOGE("selection config is wrong, rv %d\n", int(storeCnt));
			return false;
		}
		mContextProperties.mSceneDirName = fs::getRemoveEndingSlashes(mContextProperties.mSceneDirName, '/');
		mContextProperties.mSceneDirName = fs::getRemoveEndingSlashes(mContextProperties.mSceneDirName, '\\');
	}
	
#if 0
	unsigned int dirCount = 0;
	std::vector<std::string> scenes;
	if (!fs::scanDirectory("scenes", dirCount, scenes,
			true /* addSubDirs */, false /* addRegularFiles */)) {
		return false;
	}

	unsigned int sceneIndex = 0;
	if (sceneIndex >= dirCount) {
		return false;
	}
	if (!fs::changeCwd("scenes/" + scenes[sceneIndex])) {
		return false;
	}
	LOGI("cwd is changed to %s\n", fs::getCwd().c_str());

	mContextProperties.mSceneDirName = fs::getRemoveEndingSlashes(scenes[sceneIndex], '/');
#else
	if (!fs::changeCwd("scenes/" + mContextProperties.mSceneDirName)) {
		LOGE("Can't change to %s\n", ("scenes/" + mContextProperties.mSceneDirName).c_str());
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
	LOGI("Config for context creation:\n%s\n", mContextProperties.toString().c_str());

	// Initialize SDL lib
	//if (SDL_Init(SDL_INIT_VIDEO) < 0) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return;
	}
	mIsSdlInit = true;

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


	if (mContextProperties.mMajorVersion >= 0) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, mContextProperties.mMajorVersion);
		LOGI("Set SDL GL context major version to %d\n", mContextProperties.mMajorVersion);
	}
	if (mContextProperties.mMinorVersion >= 0) {
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mContextProperties.mMinorVersion);
		LOGI("Set SDL GL context minor version to %d\n", mContextProperties.mMinorVersion);
	}

	{
		int sdlGlContextProfile = 0;
		if (mContextProperties.mRenderApiVersion == RenderingApi::OPENGL_ES) {
			sdlGlContextProfile |= SDL_GL_CONTEXT_PROFILE_ES;
		}
		if (mContextProperties.mProfile == RenderingApiProfile::CORE) {
			sdlGlContextProfile |= SDL_GL_CONTEXT_PROFILE_CORE;
		}
		else if (mContextProperties.mProfile == RenderingApiProfile::COMPATIBILITY) {
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
	mWindow = SDL_CreateWindow(("glslScene: " + mContextProperties.mSceneDirName).c_str(),
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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

	SDL_GL_SetSwapInterval(1); // enable vsync

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

	mProperties.mUseGlTransforms = !mContextProperties.useVaoVersionForMesh();

	LOGI("use gl transformations: %s\n", mProperties.mUseGlTransforms ? "yes" : "no");
	LOGI("use VAO version for mesh: %s\n", mContextProperties.useVaoVersionForMesh() ? "yes" : "no");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_STENCIL_TEST);
	//glDisable(GL_SCISSOR_TEST);
	//glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glStencilMask(0xffffffff);
	//glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	//glStencilFunc(GL_ALWAYS, 0, 0xffffffff);

	// LOG OPENGL VERSION, VENDOR (IMPLEMENTATION), RENDERER, GLSL, ETC.:
	mContextProperties.mCreatedGlVersion = (char*)glGetString(GL_VERSION);
	LOGI("OpenGL Version (string): %s\n", mContextProperties.mCreatedGlVersion.c_str());

	{
		const char* s = mContextProperties.mCreatedGlVersion.c_str();
		for (; *s && (*s < '0' || *s > '9'); ++s)
			;
		size_t len = strlen(s);
		if (len >= 1) { // access s[1] is no problem also if len is 1 because of \0.
			if (s[0] == '1' && (s[1] < '0' || s[1] > '9')) {
				LOGE("At least OpenGL version 2.0 must be supported! Version %s (%s) is to low (not supported by glslScene).\n",
						mContextProperties.mCreatedGlVersion.c_str(), s);
				return;
			}
			LOGI("OpenGL version: %s\n", s);
		}
		else {
			LOGW("Can't check if OpenGL version is >= 2.0 with gl version string.\n");
		}
	}

	// all ok --> set is error to false ;-)
	mIsError = false;

	// GL_SHADING_LANGUAGE_VERSION is not available at OpenGL 1.0
	mContextProperties.mCreatedGlslVersion =
			(char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
	mContextProperties.mCreatedVendor = (char *)glGetString(GL_VENDOR);
	mContextProperties.mCreatedRenderer = (char *)glGetString(GL_RENDERER);
	glGetIntegerv(GL_MAJOR_VERSION, &mContextProperties.mCreatedMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &mContextProperties.mCreatedMinorVersion);

	LOGI("OpenGL Shading Language Version: %s\n",
			mContextProperties.mCreatedGlslVersion.c_str());
	LOGI("OpenGL Vendor: %s\n", mContextProperties.mCreatedVendor.c_str());
	LOGI("OpenGL Renderer: %s\n", mContextProperties.mCreatedRenderer.c_str());
	LOGI("OpenGL Version (int values): %d.%d\n",
			mContextProperties.mCreatedMajorVersion,
			mContextProperties.mCreatedMinorVersion);

	mGuiManager.reset(new GuiManager());
	mGuiManager->initImGui(mWindow, mContext, mContextProperties);
}

void gs::Context::destroyContext()
{
	mGuiManager->destroyImGui();

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
#ifdef USE_NEW_FILE_MONITORING_AT_SCENE_RELOAD
	std::shared_ptr<FileChangeMonitoring> fileMonitoring = std::make_shared<FileChangeMonitoring>(false);
#else
	// works also without recreate the file monitoring
	std::shared_ptr<FileChangeMonitoring> fileMonitoring = mFileMonitoring;
#endif
	if (reloadResourceManager) {
		resourceManager.reset(new ResourceManager(fileMonitoring,
				mContextProperties.useVaoVersionForMesh()));
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
	if (!sceneloader::reload(*sceneConfig, fileMonitoring, *rm, *sm, *pm,
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
#ifdef USE_NEW_FILE_MONITORING_AT_SCENE_RELOAD
	LOGI("Switch hot reloading.\n");
	mFileMonitoring = fileMonitoring;
	mHotReloadingId = mFileMonitoring->addFile(mSceneFilename, hotReloading, std::shared_ptr<void>(), this);
#endif
	LOGI("Reload and apply scene file.\n");
}

