#include <capi/ecs/entity.h>

#include <gs/ecs/entity.h>

TransformComponent* entityTransformComponent(Entity* e)
{
	gs::Entity* entity = static_cast<gs::Entity*>(e);
	return &entity->transform();
}

