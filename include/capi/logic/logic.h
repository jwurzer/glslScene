#ifndef GLSLSCENE_CAPI_LOGIC_H
#define GLSLSCENE_CAPI_LOGIC_H

#include <capi/ecs/entity.h>
#include <capi/rendering/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*TLogicInitFunc)(void* logicInstance);
typedef void (*TLogicUpdateFunc)(void* logicInstance, Entity* e, Properties* p);
typedef void (*TLogicDestroyFunc)(void* logicInstance);

struct Logic
{
	void* logicInstance;
	TLogicInitFunc logicInit;
	TLogicUpdateFunc logicUpdate;
	TLogicDestroyFunc logicDestroy;
};

#ifdef __cplusplus
}
#endif

#endif //GLSLSCENE_CAPI_LOGIC_H
