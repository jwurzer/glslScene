#ifndef GLSLSCENE_CAPI_EXAMPLE_LOGIC_H
#define GLSLSCENE_CAPI_EXAMPLE_LOGIC_H

#include <capi/logic/logic.h>
#include <capi/rendering/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CapiExample
{
	float xPos;
	float yPos;
};

int capiExampleInit(void* logicInstance);
void capiExampleUpdate(void* logicInstance, Entity* e, Properties* p);
void capiExampleDestroy(void* logicInstance);

#ifdef __cplusplus
}
#endif

#endif //GLSLSCENE_CAPI_EXAMPLE_LOGIC_H
