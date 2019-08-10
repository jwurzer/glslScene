#ifndef GLSLSCENE_CAMERA_H
#define GLSLSCENE_CAMERA_H

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtc/quaternion.hpp>
#include <SDL.h>
#include <vector>

namespace gs
{
	class Camera
	{
	public:
		/**
		 * @param rotation unit: degree
		 */
		Camera();
		virtual ~Camera();

		void update(float seconds);
		//void setSpeed(float speed);

		void moveForward(bool forward);
		void moveBackward(bool backward);

		void rotateLeft(bool left);
		void rotateRight(bool right);

		void rotateUp(bool up);
		void rotateDown(bool down);

		void rotateClockwise(bool clockwise);
		void rotateAntiClockwise(bool antiClockwise);

		bool isPerspective() const;

		float getViewAngle() const;
		float getZNear() const;
		float getZFar() const;

		void setSpeed(float speed) { mSpeed = speed; }
		void setRotateDistance(float rotateDistance) { mRotateDistance = rotateDistance; }
		void setLookAt(const glm::vec3& eye, const glm::vec3& center,
				const glm::vec3& up);
		/**
		 * @param eye OUT store eye x, y, z
		 * @param center OUT store center x, y, z
		 * @param up OUT store up x, y, z
		 */
		void getPropertiesForLookAt(glm::vec3& eye, glm::vec3& center,
				glm::vec3& up) const;
		glm::vec3 getPos() const;
		glm::vec3 getViewVector() const;
		//glm::mat4 getAxisAngleMatrix() const;

		/**
		 * @param v Must be a normalized vector
		 */
		static glm::vec3 getUpVector(const glm::vec3& v);

		void handleEvent(const SDL_Event& e);

	private:
		bool mIsPerspective;

		float mViewAngle;
		float mZNear;
		float mZFar;

		// eye position
		glm::vec3 mPosPoint;
		glm::vec3 mViewVector; // normalized
		glm::vec3 mUpVector; // normalized

		float mPrevSeconds;
		bool mMoveForward;
		bool mMoveBackward;

		float mSpeed = 1000.0f;
		float mRotateDistance = 0.0f;

		bool mRotateLeft;
		bool mRotateRight;
		bool mRotateUp;
		bool mRotateDown;

		bool mRotateClockwise;
		bool mRotateAntiClockwise;

		bool mRotateDistanceLeft;
		bool mRotateDistanceRight;

		// start time of the camera movment
		float mStartTime;
		// time between two points
		float mPointDiffTime;
		std::vector<glm::vec3> mPoints;
		std::vector<glm::vec3> mViewPoints;

		glm::vec3 getInterPoint(const std::vector<glm::vec3>& points,
				unsigned int index, float percent) const;

		glm::quat getInterQuatPoint(unsigned int index, float percent) const;

		void updateCustomPosition(float seconds);
	};
}

#endif
