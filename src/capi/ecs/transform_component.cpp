#include <capi/ecs/transform_component.h>

#include <gs/ecs/transform_component.h>

void transformReset(TransformComponent* t)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->reset();
}

void transformTranslate2f(TransformComponent* t, float x, float y)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->translate(x, y);
}

void transformTranslate3f(TransformComponent* t, float x, float y, float z)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->translate(x, y, z);
}

void transformRotateDegree(TransformComponent* t, float angle)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->rotateDegree(angle);
}

void transformRotateDegreeCenter(TransformComponent* t, float angle, float centerX, float centerY)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->rotateDegree(angle, centerX, centerY);
}

void transformRotateDegreeAxis(TransformComponent* t, float angle, float rotAxisX, float rotAxisY, float rotAxisZ)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->rotateDegree(angle, rotAxisX, rotAxisY, rotAxisZ);
}

void transformRotateRadian(TransformComponent* t, float angle)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->rotateRadian(angle);
}

void transformRotateRadianCenter(TransformComponent* t, float angle, float centerX, float centerY)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->rotateRadian(angle, centerX, centerY);
}

void transformRotateRadianAxis(TransformComponent* t, float angle, float rotAxisX, float rotAxisY, float rotAxisZ)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->rotateRadian(angle, rotAxisX, rotAxisY, rotAxisZ);
}

void transformScale2f(TransformComponent* t, float scaleX, float scaleY)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->scale(scaleX, scaleY);
}

void transformScaleCenter2f(TransformComponent* t, float scaleX, float scaleY, float centerX, float centerY)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->scale(scaleX, scaleY, centerX, centerY);
}

void transformScale3f(TransformComponent* t, float scaleX, float scaleY, float scaleZ)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->scale(scaleX, scaleY, scaleZ);
}

void transformScaleCenter3f(TransformComponent* t, float scaleX, float scaleY, float scaleZ, float centerX, float centerY, float centerZ)
{
	gs::TransformComponent* transform = static_cast<gs::TransformComponent*>(t);
	transform->scale(scaleX, scaleY, scaleZ, centerX, centerY, centerZ);
}
