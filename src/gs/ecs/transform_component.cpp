
#include <gs/ecs/transform_component.h>
#include <glm/gtc/matrix_transform.hpp>
#include <gs/system/log.h>

gs::TransformComponent::TransformComponent(std::weak_ptr<Entity> entity)
		:Component(entity), mTransform()
{
	reset();
}

gs::TransformComponent::~TransformComponent()
{
}

gs::TransformComponent& gs::TransformComponent::reset()
{
	setChanged();
	mTransform = glm::mat4(1.0f);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::combine(const glm::mat4& transform)
{
	setChanged();
	mTransform *= transform; // combine
	return *this;
}

gs::TransformComponent& gs::TransformComponent::translate(float x, float y)
{
	setChanged();
	mTransform = glm::translate(mTransform, glm::vec3(x, y, 0.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::translate(const glm::vec2& offset)
{
	setChanged();
	mTransform = glm::translate(mTransform, glm::vec3(offset, 0.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::translate(float x, float y, float z)
{
	setChanged();
	mTransform = glm::translate(mTransform, glm::vec3(x, y, z));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::translate(const glm::vec3& offset)
{
	setChanged();
	mTransform = glm::translate(mTransform, offset);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateDegree(float angle)
{
	setChanged();
	mTransform = glm::rotate(mTransform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateDegree(float angle, float centerX,
		float centerY)
{
	setChanged();
	mTransform = glm::translate(mTransform, glm::vec3(centerX, centerY, 0.0f));
	mTransform = glm::rotate(mTransform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	mTransform = glm::translate(mTransform, glm::vec3(-centerX, -centerY, 0.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateDegree(float angle,
		const glm::vec2& center)
{
	rotateDegree(angle, center.x, center.y);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateDegree(float angle, float rotAxisX, float rotAxisY, float rotAxisZ)
{
	setChanged();
	mTransform = glm::rotate(mTransform, glm::radians(angle), glm::vec3(rotAxisX, rotAxisY, rotAxisZ));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateDegree(float angle, const glm::vec3& rotAxis)
{
	setChanged();
	mTransform = glm::rotate(mTransform, glm::radians(angle), rotAxis);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateRadian(float angle)
{
	setChanged();
	mTransform = glm::rotate(mTransform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateRadian(float angle, float centerX,
		float centerY)
{
	setChanged();
	mTransform = glm::translate(mTransform, glm::vec3(centerX, centerY, 0.0f));
	mTransform = glm::rotate(mTransform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
	mTransform = glm::translate(mTransform, glm::vec3(-centerX, -centerY, 0.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateRadian(float angle,
		const glm::vec2& center)
{
	rotateRadian(angle, center.x, center.y);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateRadian(float angle, float rotAxisX, float rotAxisY, float rotAxisZ)
{
	setChanged();
	mTransform = glm::rotate(mTransform, angle, glm::vec3(rotAxisX, rotAxisY, rotAxisZ));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::rotateRadian(float angle, const glm::vec3& rotAxis)
{
	setChanged();
	mTransform = glm::rotate(mTransform, angle, rotAxis);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(float scaleX, float scaleY)
{
	setChanged();
	mTransform = glm::scale(mTransform, glm::vec3(scaleX, scaleY, 1.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(float scaleX, float scaleY,
		float centerX, float centerY)
{
	setChanged();
	mTransform = glm::translate(mTransform, glm::vec3(centerX, centerY, 0.0f));
	mTransform = glm::scale(mTransform, glm::vec3(scaleX, scaleY, 1.0f));
	mTransform = glm::translate(mTransform, glm::vec3(-centerX, -centerY, 0.0f));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(float scaleX, float scaleY, float scaleZ)
{
	setChanged();
	mTransform = glm::scale(mTransform, glm::vec3(scaleX, scaleY, scaleZ));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(float scaleX, float scaleY, float scaleZ,
		float centerX, float centerY, float centerZ)
{
	setChanged();
	mTransform = glm::translate(mTransform, glm::vec3(centerX, centerY, centerZ));
	mTransform = glm::scale(mTransform, glm::vec3(scaleX, scaleY, scaleZ));
	mTransform = glm::translate(mTransform, glm::vec3(-centerX, -centerY, -centerZ));
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(const glm::vec2& factors)
{
	scale(factors.x, factors.y);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(const glm::vec2& factors,
		const glm::vec2& center)
{
	scale(factors.x, factors.y, center.x, center.y);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(const glm::vec3& factors)
{
	scale(factors.x, factors.y, factors.z);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::scale(const glm::vec3& factors, const glm::vec3& center)
{
	scale(factors.x, factors.y, factors.z, center.x, center.y, center.z);
	return *this;
}

gs::TransformComponent& gs::TransformComponent::skewX(float skewAngle)
{
	//setChanged();
	LOGI("TODO: use skewX\n");
	return *this;
}

gs::TransformComponent& gs::TransformComponent::skewY(float skewAngle)
{
	//setChanged();
	LOGI("TODO: use skewY\n");
	return *this;
}

gs::TransformComponent& gs::TransformComponent::matrix(float a, float b, float c,
		float d, float e, float f)
{
	//setChanged();
	LOGI("TODO: matrix\n");
	//mTransform.matrix(a, b, c, d, e, f);
	return *this;
}

void gs::TransformComponent::getTransform2d(glm::mat4& out) const
{
	out *= mTransform;
}
