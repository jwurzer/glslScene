#ifndef GLSLSCENE_CAPI_ENTITY_H
#define GLSLSCENE_CAPI_ENTITY_H

#include <capi/ecs/transform_component.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void Entity;

TransformComponent* entityTransformComponent(Entity* e);

#ifdef __cplusplus
}
#endif

#endif //GLSLSCENE_CAPI_ENTITY_H
