#include <gs/scene/projection.h>
#include <gs/rendering/properties.h>
#include <gs/rendering/gl_api.h>
#include <glm/gtc/matrix_transform.hpp>

gs::Projection::Projection()
		:mProjectionMode(ProjectionMode::IDENTITY_MATRIX)
{
}

void gs::Projection::setOrtho(float left, float right, float bottom, float top,
		float nearVal, float farVal)
{
	setOrtho(left, right, bottom, top, nearVal, farVal, ProjectionMode::ORTHO_CUSTOM);
}

void gs::Projection::setOrthoShrinkToFit(float left, float right, float bottom,
		float top, float nearVal, float farVal)
{
	setOrtho(left, right, bottom, top, nearVal, farVal, ProjectionMode::ORTHO_CUSTOM_SHRINK_TO_FIT);
}

void gs::Projection::setOrthoWidthFit(float left, float right, float bottom,
		float top, float nearVal, float farVal)
{
	setOrtho(left, right, bottom, top, nearVal, farVal, ProjectionMode::ORTHO_CUSTOM_WIDTH_FIT);
}

void gs::Projection::setOrthoHeightFit(float left, float right, float bottom,
		float top, float nearVal, float farVal)
{
	setOrtho(left, right, bottom, top, nearVal, farVal, ProjectionMode::ORTHO_CUSTOM_HEIGHT_FIT);
}

void gs::Projection::setOrthoCrop(float left, float right, float bottom,
		float top, float nearVal, float farVal)
{
	setOrtho(left, right, bottom, top, nearVal, farVal, ProjectionMode::ORTHO_CUSTOM_CROP);
}

void gs::Projection::setPerspective(float fovy, float aspect, float zNear,
		float zFar)
{
	mProjectionMode = ProjectionMode::PERSPECTIVE_CUSTOM;
	mValue.mPerspective.mFovy = fovy;
	mValue.mPerspective.mAspect = aspect;
	mValue.mPerspective.mZNear = zNear;
	mValue.mPerspective.mZFar = zFar;
}

void gs::Projection::setPerspectiveWithWindowRatio(float fovy,
		float zNear, float zFar)
{
	mProjectionMode = ProjectionMode::PERSPECTIVE_WINDOW_RATIO;
	mValue.mPerspective.mFovy = fovy;
	mValue.mPerspective.mAspect = 1.0f; // is not used
	mValue.mPerspective.mZNear = zNear;
	mValue.mPerspective.mZFar = zFar;
}

void gs::Projection::setPerspectiveDegree(float fovy, float aspect, float zNear,
		float zFar)
{
	mProjectionMode = ProjectionMode::PERSPECTIVE_DEGREE_CUSTOM;
	mValue.mPerspective.mFovy = fovy;
	mValue.mPerspective.mAspect = aspect;
	mValue.mPerspective.mZNear = zNear;
	mValue.mPerspective.mZFar = zFar;
}

void gs::Projection::setPerspectiveDegreeWithWindowRatio(float fovy,
		float zNear, float zFar)
{
	mProjectionMode = ProjectionMode::PERSPECTIVE_DEGREE_WINDOW_RATIO;
	mValue.mPerspective.mFovy = fovy;
	mValue.mPerspective.mAspect = 1.0f; // is not used
	mValue.mPerspective.mZNear = zNear;
	mValue.mPerspective.mZFar = zFar;
}

void gs::Projection::setFrustum(float left, float right, float bottom,
		float top, float nearVal, float farVal)
{
	mProjectionMode = ProjectionMode::FRUSTUM_CUSTOM;
	mValue.mFrustum.mLeft = left;
	mValue.mFrustum.mRight = right;
	mValue.mFrustum.mBottom = bottom;
	mValue.mFrustum.mTop = top;
	mValue.mFrustum.mNearVal = nearVal;
	mValue.mFrustum.mFarVal = farVal;
}

glm::mat4 gs::Projection::applyProjection(const gs::Properties &p, float width, float height,
		glm::vec3& viewSize)
{
	glm::mat4 projectionMatrix(1.0f);
	viewSize = glm::vec3(2.0f, 2.0f, 2.0f); // for identity projection

	if (p.mUseGlTransforms) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	}

	switch (mProjectionMode) {
		case ProjectionMode::IDENTITY_MATRIX:
			break;
		case ProjectionMode::ORTHO_WINDOW_SIZE:
			if (p.mUseGlTransforms) {
				glOrtho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
			}
			projectionMatrix = glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
			viewSize = glm::vec3(width, height, 2.0f);
			break;
		case ProjectionMode::ORTHO_WINDOW_SIZE_CENTER:
		{
			float hw = width * 0.5f; // half width
			float hh = height * 0.5f; // half height
			if (p.mUseGlTransforms) {
				glOrtho(-hw, hw, -hh, hh, -1.0f, 1.0f);
			}
			projectionMatrix = glm::ortho(-hw, hw, -hh, hh, -1.0f, 1.0f);
			viewSize = glm::vec3(width, height, 2.0f);
			break;
		}
		case ProjectionMode::ORTHO_CUSTOM:
		case ProjectionMode::ORTHO_CUSTOM_SHRINK_TO_FIT:
		case ProjectionMode::ORTHO_CUSTOM_WIDTH_FIT:
		case ProjectionMode::ORTHO_CUSTOM_HEIGHT_FIT:
		case ProjectionMode::ORTHO_CUSTOM_CROP:
		{
			float viewLeft = mValue.mOrtho.mLeft;
			float viewRight = mValue.mOrtho.mRight;
			float viewBottom = mValue.mOrtho.mBottom;
			float viewTop = mValue.mOrtho.mTop;
			float viewNear = mValue.mOrtho.mNearVal;
			float viewFar = mValue.mOrtho.mFarVal;

			if (mProjectionMode == ProjectionMode::ORTHO_CUSTOM) {
				if (p.mUseGlTransforms) {
					glOrtho(viewLeft, viewRight, viewBottom, viewTop, viewNear, viewFar);
				}
				projectionMatrix = glm::ortho(viewLeft, viewRight,
						viewBottom, viewTop, viewNear, viewFar);
				// TODO should be the viewSize signed? (not abs() ?)
				viewSize = glm::vec3(std::abs(viewRight - viewLeft), std::abs(viewTop - viewBottom), std::abs(viewFar - viewNear));
				break;
			}

			float viewWidth = viewRight - viewLeft;
			float absViewWidth = std::abs(viewWidth);
			float halfWidth = viewWidth * 0.5f;
			float centerX = viewLeft + halfWidth;
			//float widthFactor = viewWidth / width;

			float viewHeight = viewTop - viewBottom;
			float absViewHeight = std::abs(viewHeight);
			float halfHeight = viewHeight * 0.5f;
			float centerY = viewBottom + halfHeight;
			//float heightFactor = viewHeight / height;

			float resRatio = width / height;
			float viewRatio = viewWidth / viewHeight;
			float absResRatio = std::abs(resRatio);
			float absViewRatio = std::abs(viewRatio);

			if (mProjectionMode == ProjectionMode::ORTHO_CUSTOM_SHRINK_TO_FIT) {
				if (absResRatio < absViewRatio) {
					absViewHeight = absViewWidth / absResRatio;
				}
				else {
					absViewWidth = absViewHeight * absResRatio;
				}
			}
			else if (mProjectionMode == ProjectionMode::ORTHO_CUSTOM_WIDTH_FIT) {
				absViewHeight = absViewWidth / absResRatio;
			}
			else if (mProjectionMode == ProjectionMode::ORTHO_CUSTOM_HEIGHT_FIT) {
				absViewWidth = absViewHeight * absResRatio;
			}
			else if (mProjectionMode == ProjectionMode::ORTHO_CUSTOM_CROP) {
				if (absResRatio < absViewRatio) {
					absViewWidth = absViewHeight * absResRatio;
				}
				else {
					absViewHeight = absViewWidth / absResRatio;
				}
			}

			viewWidth = viewWidth < 0.0f ? -absViewWidth : absViewWidth;
			viewHeight = viewHeight < 0.0f ? -absViewHeight : absViewHeight;

			halfWidth = viewWidth * 0.5f;
			halfHeight = viewHeight * 0.5f;

			viewLeft = centerX - halfWidth;
			viewRight = centerX + halfWidth;
			viewBottom = centerY - halfHeight;
			viewTop = centerY + halfHeight;

			if (p.mUseGlTransforms) {
				glOrtho(viewLeft, viewRight, viewBottom, viewTop, viewNear, viewFar);
			}
			projectionMatrix = glm::ortho(
					viewLeft, viewRight, viewBottom, viewTop, viewNear, viewFar);
			// TODO should be the viewSize signed? (not abs() ?)
			viewSize = glm::vec3(std::abs(viewRight - viewLeft), std::abs(viewTop - viewBottom), std::abs(viewFar - viewNear));
			break;
		}
		case ProjectionMode::PERSPECTIVE_WINDOW_RATIO:
			if (p.mUseGlTransforms) {
				gluPerspective(glm::degrees(mValue.mPerspective.mFovy),
						width / height,
						mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			}
			projectionMatrix = glm::perspective(mValue.mPerspective.mFovy,
					width / height,
					mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			break;
		case ProjectionMode::PERSPECTIVE_CUSTOM:
			if (p.mUseGlTransforms) {
				gluPerspective(glm::degrees(mValue.mPerspective.mFovy), mValue.mPerspective.mAspect,
						mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			}
			projectionMatrix = glm::perspective(
					mValue.mPerspective.mFovy, mValue.mPerspective.mAspect,
					mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			break;
		case ProjectionMode::PERSPECTIVE_DEGREE_WINDOW_RATIO:
			if (p.mUseGlTransforms) {
				gluPerspective(mValue.mPerspective.mFovy,
						width / height,
						mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			}
			projectionMatrix = glm::perspective(glm::radians(mValue.mPerspective.mFovy),
					width / height,
					mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			break;
		case ProjectionMode::PERSPECTIVE_DEGREE_CUSTOM:
			if (p.mUseGlTransforms) {
				gluPerspective(mValue.mPerspective.mFovy, mValue.mPerspective.mAspect,
						mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			}
			projectionMatrix = glm::perspective(
					glm::radians(mValue.mPerspective.mFovy), mValue.mPerspective.mAspect,
					mValue.mPerspective.mZNear, mValue.mPerspective.mZFar);
			break;
		case ProjectionMode::FRUSTUM_CUSTOM:
			if (p.mUseGlTransforms) {
				glFrustum(mValue.mFrustum.mLeft, mValue.mFrustum.mRight,
						mValue.mFrustum.mBottom, mValue.mFrustum.mTop,
						mValue.mFrustum.mNearVal, mValue.mFrustum.mFarVal);
			}
			projectionMatrix = glm::frustum(
					mValue.mFrustum.mLeft, mValue.mFrustum.mRight,
					mValue.mFrustum.mBottom, mValue.mFrustum.mTop,
					mValue.mFrustum.mNearVal, mValue.mFrustum.mFarVal);
			break;
	}
	mProjectionMatrix = projectionMatrix;
	return projectionMatrix;
}

void gs::Projection::setOrtho(float left, float right, float bottom, float top,
		float nearVal, float farVal, ProjectionMode mode)
{
	mProjectionMode = mode;
	mValue.mOrtho.mLeft = left;
	mValue.mOrtho.mRight = right;
	mValue.mOrtho.mBottom = bottom;
	mValue.mOrtho.mTop = top;
	mValue.mOrtho.mNearVal = nearVal;
	mValue.mOrtho.mFarVal = farVal;
}

