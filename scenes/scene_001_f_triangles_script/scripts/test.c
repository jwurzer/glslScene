#include <stdio.h>
#include <stdlib.h>

#include <capi/ecs/entity.h>
#include <capi/ecs/transform_component.h>
#include <capi/rendering/properties.h>

struct MyLogic
{
	int x;
	int y;
};

void* create()
{
	struct MyLogic* logic = malloc(sizeof(struct MyLogic));
	fprintf(stderr, "logic create new instance at address %p\n", logic);
	return logic;
}

void handleEvent(void* logicInstance, Entity* e, Properties* p, SdlEvent* evt)
{
	//fprintf(stderr, "hello handleEvent\n");
}

void update(void* logicInstance, Entity* e, Properties* p)
{
	//fprintf(stderr, "hello update %p %p %p\n", logicInstance, e, p);
	TransformComponent* t = entityTransformComponent(e);
	transformReset(t);
	float time = propertiesGetTsSec(p);
	transformRotateDegree(t, time * 315.0f);
}

void destroy(void* logicInstance)
{
	fprintf(stderr, "logi c destroy instance from address %p\n", logicInstance);
	free(logicInstance);
}
