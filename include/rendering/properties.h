#ifndef GLSLSCENE_PROPERTIES_H
#define GLSLSCENE_PROPERTIES_H

#include <common/vector2.h>
#include <common/size2.h>
#include <string>
#include <stdint.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace gs
{
	class Properties
	{
	public:
		bool mUseGlTransforms = true;

		bool mDrawNormals = false;

		uint32_t mTsMsec = 0;
		float mTsSec = 0.0f;

		float mDeltaTimeSec = 0.0f;

		// that's always the resolution of the window.
		// Also if offscreen rendering is used.
		// The current framebuffer can have another resolution (if its not the default framebuffer)
		Size2i mWindowSizeI;
		Size2f mWindowSize;
		Size2f mWindowRatio;

		Vector2f mMousePosFactor;
		Vector2f mMousePosPixel;

		glm::mat4 mModelMatrix = glm::mat4(1.0f);
		//glm::mat4 mModelViewMatrix = glm::mat4(1.0f);

		// All the following members should not be used from a logic component
		// because the only have correct values at rendering and not at update.

		Vector2i mViewportPosPixelI; // always 0, 0
		Size2u mViewportSizePixelU; // currently always the size of the current framebuffer
		Vector2f mViewportPosPixel; // always 0.0f, 0.0f
		Size2f mViewportSizePixel; // currently always the size of the current framebuffer

		glm::vec3 mViewSize = glm::vec3(2.0f, 2.0f, 2.0f); // for identity projection
		Size2f mViewRatio = Size2f(1.0f, 1.0f);

		glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
		// move the world to the "camera"/eye. (worldspace --> eye space)
		glm::mat4 mViewMatrix = glm::mat4(1.0f);

		// is calc by glm::inverse(mViewMatrix) each render pass
		// e.g. Is useful to calculate the eye position in world space
		glm::mat4 mInverseViewMatrix = glm::mat4(1.0f);

		std::string toString() const;
	};
}

#endif //GLSLSCENE_PROPERTIES_H
