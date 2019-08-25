#include <gs/logic/capi_example_logic.h>

#include <capi/system/log.h>

int capiExampleInit(void* logicInstance)
{
	LOGI("Call init for capiExample");
}

void capiExampleUpdate(void* logicInstance, Entity* e, Properties* p)
{
	TransformComponent* t = entityTransformComponent(e);
	transformReset(t);
	float time = propertiesGetTsSec(p);
	transformRotateDegree(t, time * 5);
	//LOGI("Call update for capiExample");
}

void capiExampleDestroy(void* logicInstance)
{
	LOGI("Call destroy for capiExample");
}

