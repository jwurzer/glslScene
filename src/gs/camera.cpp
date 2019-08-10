#include <gs/camera.h>

#if !defined(_MSC_VER)
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
//#include <glm/glm.hpp>
#include <glm/gtx/spline.hpp>

using namespace std;

gs::Camera::Camera()
	:mIsPerspective(false),
	mViewAngle(45.0f),
	mZNear(1.0f),
	mZFar(500.0f),
	mPosPoint(0, 0, 0),
	mViewVector(0, 0, -1),
	mUpVector(0, 1, 0),
	mPrevSeconds(0.0),
	mMoveForward(false),
	mMoveBackward(false),
	mRotateLeft(false),
	mRotateRight(false),
	mRotateUp(false),
	mRotateDown(false),
	mRotateClockwise(false),
	mRotateAntiClockwise(false),
	mRotateDistanceLeft(false),
	mRotateDistanceRight(false),
	mStartTime(0.0),
	mPointDiffTime(2.0),
	mPoints(),
	mViewPoints()
{
#if 1
	mPoints.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
	mViewPoints.push_back(glm::vec3(5.0f, 2.92f, 5.0f));
#else
	mPoints.push_back(glm::vec3(-10.0f, -10.0f, -10.0f));
	mViewPoints.push_back(glm::vec3(-5.0f, -2.92f, -5.0f));
#endif

	mPoints.push_back(glm::vec3(5.0f, 10.0f, 10.0f));
	mViewPoints.push_back(glm::vec3(2.0f, 5.0f, 4.0f));

	mPoints.push_back(glm::vec3(-15.0f, 6.0f, 10.0f));
	mViewPoints.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	mPoints.push_back(glm::vec3(10.0f, 4.0f, 8.0f));
	mViewPoints.push_back(glm::vec3(-2.0f, 3.0f, -5.0f));

	mPoints.push_back(glm::vec3(5.0f, 14.0f, 8.0f));
	mViewPoints.push_back(glm::vec3(5.0f, 5.0f, 5.0f));

	update(0.0);
}

gs::Camera::~Camera()
{
}

void gs::Camera::update(float seconds)
{
#if 1
	updateCustomPosition(seconds);
	mPrevSeconds = seconds;
#else
	if (seconds < mStartTime) {
		updateCustomPosition(seconds);
		mPrevSeconds = seconds;
		return;
	}
	float moveTime = seconds - mStartTime;
	unsigned int index = static_cast<unsigned int>(moveTime / mPointDiffTime);
	float percent = (moveTime - mPointDiffTime * float(index)) / mPointDiffTime;
	if (index >= mPoints.size()) {
		updateCustomPosition(seconds);
		mPrevSeconds = seconds;
		return;
	}

	mPosPoint = getInterPoint(mPoints, index, percent);
#if 0
	mViewVector = getInterPoint(mViewPoints, index, percent) - mPosPoint;
#else
	glm::quat dir = getInterQuatPoint(index, percent);
	mViewVector = glm::axis(dir);
#endif
	mUpVector = getUpVector(mViewVector);

	mPrevSeconds = seconds;
#endif
}

void gs::Camera::moveForward(bool forward)
{
	mMoveForward = forward;
}

void gs::Camera::moveBackward(bool backward)
{
	mMoveBackward = backward;
}

void gs::Camera::rotateLeft(bool left)
{
	mRotateLeft = left;
}

void gs::Camera::rotateRight(bool right)
{
	mRotateRight = right;
}

void gs::Camera::rotateUp(bool up)
{
	mRotateUp = up;
}

void gs::Camera::rotateDown(bool down)
{
	mRotateDown = down;
}

void gs::Camera::rotateClockwise(bool clockwise)
{
	mRotateClockwise = clockwise;
}

void gs::Camera::rotateAntiClockwise(bool antiClockwise)
{
	mRotateAntiClockwise = antiClockwise;
}

bool gs::Camera::isPerspective() const
{
	return mIsPerspective;
}

float gs::Camera::getViewAngle() const
{
	return mViewAngle;
}

float gs::Camera::getZNear() const
{
	return mZNear;
}

float gs::Camera::getZFar() const
{
	return mZFar;
}

void gs::Camera::setLookAt(const glm::vec3& eye, const glm::vec3& center,
		const glm::vec3& up)
{
	mPosPoint = eye;
	mViewVector = glm::normalize(center - mPosPoint);
	mUpVector = glm::normalize(up);
}

void gs::Camera::getPropertiesForLookAt(glm::vec3& eye, glm::vec3& center,
		glm::vec3& up) const
{
	eye = mPosPoint;
	center = mPosPoint + mViewVector;
	up = mUpVector;
}

glm::vec3 gs::Camera::getPos() const
{
	return mPosPoint;
}

glm::vec3 gs::Camera::getViewVector() const
{
	return mViewVector;
}

glm::vec3 gs::Camera::getInterPoint(const std::vector<glm::vec3>& points,
		unsigned int index, float percent) const
{
	if (index + 1 >= points.size()) {
		return points[index];
	}

	glm::vec3 start = points[index];
	glm::vec3 end = points[index + 1];
	glm::vec3 beforeStart = start;
	if (index > 0) {
		beforeStart = points[index - 1];
	}
	glm::vec3 afterEnd = end;
	if (index + 2 < points.size()) {
		afterEnd = points[index + 2];
	}

	//cout << "percent: " << percent << " i " << index << endl;
#if 0
	return glm::vec3(start.x + (end.x - start.x) * percent,
			start.y + (end.y - start.y) * percent,
			start.z + (end.z - start.z) * percent);
#else
	return glm::catmullRom(beforeStart, start, end, afterEnd, percent);
#endif
}

glm::quat gs::Camera::getInterQuatPoint(unsigned int index, float percent) const
{
	float rad = float(M_PI) / 100.0f;

	glm::quat start = glm::normalize(glm::angleAxis(float(glm::degrees(rad)),
			glm::normalize(mViewPoints[index] - mPoints[index])));
	if (index + 1 >= mViewPoints.size()) {
		return start;
	}

	glm::quat end = glm::normalize(glm::angleAxis(float(glm::degrees(rad)),
			glm::normalize(mViewPoints[index + 1] - mPoints[index + 1])));

	glm::quat beforeStart = start;
	if (index > 0) {
		beforeStart = glm::normalize(glm::angleAxis(float(glm::degrees(rad)),
				glm::normalize(mViewPoints[index - 1] - mPoints[index - 1])));
	}
	glm::quat afterEnd = end;
	if (index + 2 < mViewPoints.size()) {
		afterEnd = glm::normalize(glm::angleAxis(float(glm::degrees(rad)),
				glm::normalize(mViewPoints[index + 2] - mPoints[index + 2])));
	}

	return glm::catmullRom(beforeStart, start, end, afterEnd, percent);
}

void gs::Camera::updateCustomPosition(float seconds)
{
	float rotSpeed = 0.015f;

	double diffSec = seconds - mPrevSeconds;
	int moveForwardBackward((mMoveForward == mMoveBackward) ? 0 :
			(mMoveForward ? -1 : 1));

	if (moveForwardBackward) {
		glm::vec3 cam_axis = mViewVector;
		cam_axis *= float(-moveForwardBackward) * diffSec * mSpeed;
		mPosPoint += cam_axis;
	}

	int rotateLeftRight((mRotateLeft == mRotateRight) ? 0 :
			(mRotateLeft ? -1 : 1));
	if (rotateLeftRight) {
		float speed = rotSpeed;
		float degree = float(glm::degrees(float(-rotateLeftRight) * diffSec * speed));
		glm::quat q = glm::normalize(glm::angleAxis(degree, mUpVector));
		mViewVector = q * mViewVector;
	}

	int rotateUpDown((mRotateUp == mRotateDown) ? 0 :
			(mRotateUp ? -1 : 1));
	if (rotateUpDown) {
		float speed = rotSpeed;
		float degree = float(glm::degrees(float(-rotateUpDown) * diffSec * speed));
		glm::quat q = glm::normalize(glm::angleAxis(degree, glm::cross(mViewVector, mUpVector)));
		mViewVector = q * mViewVector;
		mUpVector = q * mUpVector;
	}

	int rotateClockwise((mRotateClockwise == mRotateAntiClockwise) ? 0 :
			(mRotateClockwise ? -1 : 1));
	if (rotateClockwise) {
		float speed = rotSpeed;
		float degree = float(glm::degrees(float(-rotateClockwise) * diffSec * speed));
		glm::quat q = glm::normalize(glm::angleAxis(degree, mViewVector));
		mUpVector = q * mUpVector;
	}

	int rotateDistLeftRight((mRotateDistanceLeft == mRotateDistanceRight) ? 0 :
			(mRotateDistanceLeft ? -1 : 1));
	if (rotateDistLeftRight) {
		float speed = rotSpeed;
		float degree = float(glm::degrees(float(-rotateDistLeftRight) * diffSec * speed));
		mPosPoint += mViewVector * mRotateDistance;
		glm::quat q = glm::normalize(glm::angleAxis(degree, mUpVector));
		mViewVector = q * mViewVector;
		mPosPoint -= mViewVector * mRotateDistance;
	}
}

glm::vec3 gs::Camera::getUpVector(const glm::vec3& v)
{
	double xz = sqrt(v.x * v.x + v.z * v.z);
	if (xz > 1.0) {
		xz = 1.0;
	}
	double rad = acos(xz);
	double rad2 = M_PI/2.0 - rad;
	double xz2 = cos(rad2);
	double factor = xz2 / xz;

	glm::vec3 v2(v.x * factor, xz, v.z * factor);
	if (v.y > 0) {
		v2.x *= -1.0;
		v2.z *= -1.0;
	}
	return v2;
}

void gs::Camera::handleEvent(const SDL_Event& e)
{
	switch (e.type) {
		case SDL_KEYDOWN:
		case SDL_KEYUP: {
			bool isPressed = (e.key.state == SDL_PRESSED);
			switch (e.key.keysym.sym) {
				case SDLK_RETURN:
					moveForward(isPressed);
					break;
				case SDLK_BACKSPACE:
					moveBackward(isPressed);
					break;
				case SDLK_LEFT:
					rotateLeft(isPressed);
					break;
				case SDLK_RIGHT:
					rotateRight(isPressed);
					break;
				case SDLK_UP:
					rotateUp(isPressed);
					break;
				case SDLK_DOWN:
					rotateDown(isPressed);
					break;
				case SDLK_PAGEUP:
					rotateAntiClockwise(isPressed);
					break;
				case SDLK_PAGEDOWN:
					rotateClockwise(isPressed);
					break;
				case SDLK_8:
					mRotateDistanceLeft = isPressed;
					break;
				case SDLK_9:
					mRotateDistanceRight = isPressed;
					break;
			}
			break;
		}
	}
}

