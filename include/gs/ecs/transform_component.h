#ifndef GLSLSCENE_TRANSFORM_COMPONENT_H
#define GLSLSCENE_TRANSFORM_COMPONENT_H

#include <gs/ecs/component.h>
#include <glm/detail/type_mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <stdint.h>
#include <memory>

namespace gs
{
	class TransformComponent: public Component
	{
	public:
		TransformComponent(std::weak_ptr<Entity> entity);
		virtual ~TransformComponent();

		virtual EComponent getType() const { return COMPONENT_TRANSFORM; }

		TransformComponent& reset();

		TransformComponent& combine(const glm::mat4& transform);
		TransformComponent& translate(float x, float y);
		TransformComponent& translate(const glm::vec2& offset);
		TransformComponent& translate(float x, float y, float z);
		TransformComponent& translate(const glm::vec3& offset);
		TransformComponent& rotateDegree(float angle);
		TransformComponent& rotateDegree(float angle, float centerX, float centerY);
		TransformComponent& rotateDegree(float angle, const glm::vec2& center);
		TransformComponent& rotateDegree(float angle, float rotAxisX, float rotAxisY, float rotAxisZ);
		TransformComponent& rotateDegree(float angle, const glm::vec3& rotAxis);
		TransformComponent& rotateRadian(float angle);
		TransformComponent& rotateRadian(float angle, float centerX, float centerY);
		TransformComponent& rotateRadian(float angle, const glm::vec2& center);
		TransformComponent& rotateRadian(float angle, float rotAxisX, float rotAxisY, float rotAxisZ);
		TransformComponent& rotateRadian(float angle, const glm::vec3& rotAxis);
		TransformComponent& scale(float scaleX, float scaleY);
		TransformComponent& scale(float scaleX, float scaleY, float centerX, float centerY);
		TransformComponent& scale(float scaleX, float scaleY, float scaleZ);
		TransformComponent& scale(float scaleX, float scaleY, float scaleZ, float centerX, float centerY, float centerZ);
		TransformComponent& scale(const glm::vec2& factors);
		TransformComponent& scale(const glm::vec2& factors, const glm::vec2& center);
		TransformComponent& scale(const glm::vec3& factors);
		TransformComponent& scale(const glm::vec3& factors, const glm::vec3& center);
		TransformComponent& skewX(float skewAngle);
		TransformComponent& skewY(float skewAngle);
		TransformComponent& matrix(float a, float b, float c, float d, float e, float f);

		/**
		 * The transform of this component is applied to parameter out.
		 * Parameter out will be not reset to a identity matrix.
         * @param ts
         * @param scrPos
         * @param out
         */
		void getTransform(glm::mat4 &out) const;
		const glm::mat4& getMatrix() const { return mTransform; }
	private:
		glm::mat4 mTransform;
	};
}
#endif
