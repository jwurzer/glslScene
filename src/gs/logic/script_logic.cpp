#include <gs/logic/script_logic.h>

#include <gs/system/file_change_monitoring.h>
#include <gs/system/log.h>
#include <gs/rendering/properties.h>
#include <interpreter.h>

#include <capi/ecs/entity.h>
#include <capi/ecs/transform_component.h>
#include <capi/rendering/properties.h>

namespace
{
	void picocSetPointerVar(Picoc *pc, const char *varname, void*& ptr)
	{
		VariableDefinePlatformVar(pc, nullptr, (char *) varname,
				pc->VoidPtrType, (union AnyValue *) &ptr, true);
	}

	void picocClearVar(Picoc *pc, const char *var)
	{
		if (VariableDefined(pc, TableStrRegister(pc, var))) {
			TableDelete(pc, &pc->GlobalTable, TableStrRegister(pc, var));
		}
	}

	void picocEntityTransformComponent(struct ParseState *Parser,
			 struct Value *ReturnValue,
			 struct Value **Param,
			 int NumArgs)
	{
		ReturnValue->Val->Pointer = entityTransformComponent(Param[0]->Val->Pointer);
	}

	void picocTransformReset(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformReset(Param[0]->Val->Pointer);
	}

	void picocTransformTranslate2f(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformTranslate2f(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP);
	}

	void picocTransformTranslate3f(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformTranslate3f(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP);
	}

	void picocTransformRotateDegree(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformRotateDegree(Param[0]->Val->Pointer,
				Param[1]->Val->FP);
	}

	void picocTransformRotateDegreeCenter(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformRotateDegreeCenter(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP);
	}

	void picocTransformRotateDegreeAxis(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformRotateDegreeAxis(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP, Param[4]->Val->FP);
	}

	void picocTransformRotateRadian(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformRotateRadian(Param[0]->Val->Pointer,
				Param[1]->Val->FP);
	}

	void picocTransformRotateRadianCenter(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformRotateRadianCenter(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP);
	}

	void picocTransformRotateRadianAxis(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformRotateRadianAxis(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP,
				Param[4]->Val->FP);
	}

	void picocTransformScale2f(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformScale2f(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP);
	}

	void picocTransformScaleCenter2f(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformScaleCenter2f(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP,
				Param[4]->Val->FP);
	}

	void picocTransformScale3f(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformScale3f(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP);
	}

	void picocTransformScaleCenter3f(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		transformScaleCenter3f(Param[0]->Val->Pointer,
				Param[1]->Val->FP, Param[2]->Val->FP, Param[3]->Val->FP,
				Param[4]->Val->FP, Param[5]->Val->FP, Param[6]->Val->FP);
	}


	void picocPropertiesGetTsMsec(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->Integer = propertiesGetTsMsec(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetTsSec(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetTsSec(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetDeltaTimeSec(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetDeltaTimeSec(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetWindowWidthI(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->Integer = propertiesGetWindowWidthI(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetWindowHeightI(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->Integer = propertiesGetWindowHeightI(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetWindowWidth(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetWindowWidth(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetWindowHeight(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetWindowHeight(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetMousePosFactorX(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetMousePosFactorX(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetMousePosFactorY(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetMousePosFactorY(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetMousePosPixelX(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetMousePosPixelX(Param[0]->Val->Pointer);
	}

	void picocPropertiesGetMousePosPixelY(struct ParseState *Parser,
			struct Value *ReturnValue, struct Value **Param, int NumArgs)
	{
		ReturnValue->Val->FP = propertiesGetMousePosPixelY(Param[0]->Val->Pointer);
	}

	struct LibraryFunction picocEntityFunctions[] =
	{
		{picocEntityTransformComponent, "void* entityTransformComponent(void *);"},
		{NULL, NULL}
	};

	struct LibraryFunction picocTransformComponentFunctions[] =
	{
		{picocTransformReset, "void transformReset(void* t);"},
		{picocTransformTranslate2f,
				"void transformTranslate2f(void* t, float x, float y);"},
		{picocTransformTranslate3f,
				"void transformTranslate3f(void* t, float x, float y, float z);"},
		{picocTransformRotateDegree,
				"void transformRotateDegree(void* t, float angle);"},
		{picocTransformRotateDegreeCenter,
				"void transformRotateDegreeCenter(void* t, float angle, float centerX, float centerY);"},
		{picocTransformRotateDegreeAxis,
				"void transformRotateDegreeAxis(void* t, float angle, float rotAxisX, float rotAxisY, float rotAxisZ);"},
		{picocTransformRotateRadian,
				"void transformRotateRadian(void* t, float angle);"},
		{picocTransformRotateRadianCenter,
				"void transformRotateRadianCenter(void* t, float angle, float centerX, float centerY);"},
		{picocTransformRotateRadianAxis,
				"void transformRotateRadianAxis(void* t, float angle, float rotAxisX, float rotAxisY, float rotAxisZ);"},
		{picocTransformScale2f,
				"void transformScale2f(void* t, float scaleX, float scaleY);"},
		{picocTransformScaleCenter2f,
				"void transformScaleCenter2f(void* t, float scaleX, float scaleY, float centerX, float centerY);"},
		{picocTransformScale3f,
				"void transformScale3f(void* t, float scaleX, float scaleY, float scaleZ);"},
		{picocTransformScaleCenter3f,
				"void transformScaleCenter3f(void* t, float scaleX, float scaleY, float scaleZ, float centerX, float centerY, float centerZ);"},
		{NULL, NULL}
	};

	struct LibraryFunction picocPropertiesFunctions[] =
	{
		{picocPropertiesGetTsMsec, "int propertiesGetTsMsec(void* p);"},
		{picocPropertiesGetTsSec, "float propertiesGetTsSec(void* p);"},
		{picocPropertiesGetDeltaTimeSec, "float propertiesGetDeltaTimeSec(void* p);"},
		{picocPropertiesGetWindowWidthI, "int propertiesGetWindowWidthI(void* p);"},
		{picocPropertiesGetWindowHeightI, "int propertiesGetWindowHeightI(void* p);"},
		{picocPropertiesGetWindowWidth, "float propertiesGetWindowWidth(void* p);"},
		{picocPropertiesGetWindowHeight, "float propertiesGetWindowHeight(void* p);"},
		{picocPropertiesGetMousePosFactorX, "float propertiesGetMousePosFactorX(void* p);"},
		{picocPropertiesGetMousePosFactorY, "float propertiesGetMousePosFactorY(void* p);"},
		{picocPropertiesGetMousePosPixelX, "float propertiesGetMousePosPixelX(void* p);"},
		{picocPropertiesGetMousePosPixelY, "float propertiesGetMousePosPixelY(void* p);"},
		{NULL, NULL}
	};

	void registerFunctions(Picoc *pc)
	{
		IncludeRegister(pc, "capi/ecs/entity.h", NULL, &picocEntityFunctions[0], nullptr);
		IncludeRegister(pc, "capi/ecs/transform_component.h", NULL, &picocTransformComponentFunctions[0], nullptr);
		IncludeRegister(pc, "capi/rendering/properties.h", NULL, &picocPropertiesFunctions[0], nullptr);
	}
}

gs::ScriptLogic::ScriptLogic(const std::string &scriptFilename,
		const std::weak_ptr<FileChangeMonitoring>& fcm)
	:mScriptFilename(scriptFilename), mFcm(fcm), mIsLoaded(false),
	mIsFirst(true), mIsScriptError(false), mFileId(0)
{
	reload();

	std::shared_ptr<FileChangeMonitoring> fileMonitoring = mFcm.lock();
	if (fileMonitoring) {
		mFileId = fileMonitoring->addFile(scriptFilename, hotReloadingCallback,
				std::shared_ptr<void>(), this);
	}
}

gs::ScriptLogic::~ScriptLogic()
{
	if (mFileId) {
		std::shared_ptr<FileChangeMonitoring> fileMonitoring = mFcm.lock();
		if (fileMonitoring) {
			fileMonitoring->removeFile(mFileId);
		}
	}
	unload();
}

void gs::ScriptLogic::handleEvent(const std::shared_ptr<gs::Entity> &e,
		gs::ResourceManager &rm, const gs::Properties &p, const SDL_Event &evt)
{
	runScriptFunction(false, e, rm, p, &evt);
}

void gs::ScriptLogic::update(const std::shared_ptr<gs::Entity> &e,
		gs::ResourceManager &rm, const gs::Properties &p)
{
	runScriptFunction(true, e, rm, p, nullptr);
}

bool gs::ScriptLogic::reload()
{
	unload();
	return load();
}

bool gs::ScriptLogic::load()
{
	LOGI("start loading");
	int stackSize = 128000 * 4;
	PicocInitialize(&mPc, stackSize);

	registerFunctions(&mPc);

	std::string filename = "script-init";
	std::string source;
	source +=
			"typedef void Entity;\n"
			"typedef void TransformComponent;\n"
			"typedef void Properties;\n"
			"typedef void SdlEvent;\n"
			"\n"
			"struct NativePointers\n"
			"{\n"
			"	Entity * entity;\n"
			"	Properties * properties;\n"
			"	SdlEvent * event;\n"
			"};\n"
			"\n"
			"void callFunction(void * logicInstance, void * nativePtrs, int runUpdate)"
			"{\n"
			"	struct NativePointers * np = (struct NativePointers *)nativePtrs;\n"
			"	if (runUpdate) {"
			"		update(logicInstance, np->entity, np->properties);"
			"	}"
			"	else {"
			"		handleEvent(logicInstance, np->entity, np->properties, np->event);"
			"	}"
			"}\n"
			"\n"
			;

	if (PicocPlatformSetExitPoint(&mPc)) {
		LOGE("load C script failed!\n");
		return false;
	}

	PicocParse(&mPc, filename.c_str(), source.c_str(),
			source.length(),
			1 /* RunIt */,
			1 /* CleanupNow */,
			1 /* CleanupSource */,
			0 /* EnableDebugger */);

	PicocPlatformScanFile(&mPc, mScriptFilename.c_str());

	LOGI("Loaded");
	mIsLoaded = true;
	return true;
}

void gs::ScriptLogic::unload()
{
	if (!mIsLoaded) {
		return;
	}

	mIsLoaded = false;
	mIsFirst = true;
	mIsScriptError = false;

	std::string filename = "script-destroy";
	std::string source;
	source +=
			"destroy(g_picocLogicInstance);\n"
			"\n"
			;

	if (PicocPlatformSetExitPoint(&mPc)) {
		LOGE("destroy() at C script failed!\n");
		return;
	}

	PicocParse(&mPc, filename.c_str(), source.c_str(),
			source.length(),
			1 /* RunIt */,
			1 /* CleanupNow */,
			1 /* CleanupSource */,
			0 /* EnableDebugger */);

	PicocCleanup(&mPc);
}

void gs::ScriptLogic::runScriptFunction(bool isUpdateCall,
		const std::shared_ptr<Entity>& e,
		ResourceManager& rm,
		const Properties& p,
		const SDL_Event* evt)
{
	if (!mIsLoaded) {
		return;
	}

	if (mIsScriptError) {
		return;
	}

	if (PicocPlatformSetExitPoint(&mPc)) {
		mIsScriptError = true;
		LOGE("%s function failed. --> Disable %s script!\n",
				isUpdateCall ? "update()" : "handleEvent()", mScriptFilename.c_str());
		picocClearVar(&mPc, "g_picocNativePointers");
		return;
		//PicocCleanup(&mPc);
		//return mPc.PicocExitValue;
		//return false;
	}

	std::string filename = "script";
	std::string source;

	if (mIsFirst) {
		source +=
				"void * g_picocLogicInstance = create();\n"
				"\n"
				;
		mIsFirst = false;
	}

	struct NativePointers
	{
		Entity * entity;
		Properties * properties;
		void * event; // TODO replace with SdlEvent
	};

	Properties propCopy = p;

	struct NativePointers nativePointers = {e.get(), &propCopy};
	void* ptrToNativePointers = &nativePointers;
	picocSetPointerVar(&mPc, "g_picocNativePointers", ptrToNativePointers);

	if (isUpdateCall) {
		source += "callFunction(g_picocLogicInstance, g_picocNativePointers, 1);\n";
	}
	else {
		source += "callFunction(g_picocLogicInstance, g_picocNativePointers, 0);\n";
	}

	PicocParse(&mPc, filename.c_str(), source.c_str(),
			source.length(),
			1 /* RunIt */,
			1 /* CleanupNow */,
			1 /* CleanupSource */,
			0 /* EnableDebugger */);

	picocClearVar(&mPc, "g_picocNativePointers");
}

void gs::ScriptLogic::hotReloadingCallback(unsigned int callbackId,
		const std::string& filename,
		const std::shared_ptr<void>& param1, void* scriptLogic)
{
	static_cast<ScriptLogic*>(scriptLogic)->hotReloading(callbackId, filename, param1);
}

void gs::ScriptLogic::hotReloading(unsigned int callbackId,
		const std::string& filename,
		const std::shared_ptr<void>& param1)
{
	//fprintf(stderr, "TODO reload\n");
	reload();
}

