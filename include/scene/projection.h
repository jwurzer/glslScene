#ifndef GLSLSCENE_PROJECTION_H
#define GLSLSCENE_PROJECTION_H

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace gs
{
	enum class ProjectionMode
	{
		IDENTITY_MATRIX,
		ORTHO_WINDOW_SIZE,
		ORTHO_WINDOW_SIZE_CENTER,
		ORTHO_CUSTOM,
		ORTHO_CUSTOM_SHRINK_TO_FIT,
		ORTHO_CUSTOM_WIDTH_FIT,
		ORTHO_CUSTOM_HEIGHT_FIT,
		ORTHO_CUSTOM_CROP,
		PERSPECTIVE_WINDOW_RATIO,
		PERSPECTIVE_CUSTOM,
		PERSPECTIVE_DEGREE_WINDOW_RATIO,
		PERSPECTIVE_DEGREE_CUSTOM,
		FRUSTUM_CUSTOM,
	};

	class Properties;

	class Projection
	{
	public:
		ProjectionMode mProjectionMode;

		union {
			struct {
				float mLeft;
				float mRight;
				float mBottom;
				float mTop;
				float mNearVal;
				float mFarVal;
			} mOrtho;
			struct {
				float mLeft;
				float mRight;
				float mBottom;
				float mTop;
				float mNearVal;
				float mFarVal;
			} mFrustum;
			struct {
				float mFovy; // degree
				float mAspect;
				float mZNear;
				float mZFar;
			} mPerspective;
		} mValue;

		Projection();
		void setOrthoWindowSize() { mProjectionMode = ProjectionMode::ORTHO_WINDOW_SIZE; }
		void setOrthoWindowSizeCenter() { mProjectionMode = ProjectionMode::ORTHO_WINDOW_SIZE_CENTER; }
		void setOrtho(float left, float right, float bottom, float top, float nearVal, float farVal);
		void setOrthoShrinkToFit(float left, float right, float bottom, float top, float nearVal, float farVal);
		void setOrthoWidthFit(float left, float right, float bottom, float top, float nearVal, float farVal);
		void setOrthoHeightFit(float left, float right, float bottom, float top, float nearVal, float farVal);
		void setOrthoCrop(float left, float right, float bottom, float top, float nearVal, float farVal);
		void setPerspective(float fovy, float aspect, float zNear, float zFar);
		void setPerspectiveWithWindowRatio(float fovy, float zNear, float zFar);
		void setPerspectiveDegree(float fovy, float aspect, float zNear, float zFar);
		void setPerspectiveDegreeWithWindowRatio(float fovy, float zNear, float zFar);
		void setFrustum(float left, float right, float bottom, float top, float nearVal, float farVal);

		glm::mat4 applyProjection(const Properties& properties, float width, float height,
				glm::vec3& viewSize) const;
	private:
		void setOrtho(float left, float right, float bottom, float top,
				float nearVal, float farVal, ProjectionMode mode);
	};
}

#endif //GLSLSCENE_PROJECTION_H
