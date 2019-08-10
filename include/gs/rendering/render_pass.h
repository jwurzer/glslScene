#ifndef GLSLSCENE_RENDER_PASS_H
#define GLSLSCENE_RENDER_PASS_H

#include <gs/res/resource_id.h>
#include <gs/scene/projection.h>
#include <gs/common/color.h>
#include <glm/mat4x4.hpp>
#include <memory>

namespace gs
{
	class Camera;

	class RenderPass
	{
	public:
		TResourceId mFramebufferId;
		// TODO: viewport size could be also added here. Now always use resolution of the framebuffer for viewport
		Color mClearColor;
		Projection mProjection;
		glm::mat4 mViewMatrix = glm::mat4(1.0f);
		std::shared_ptr<Camera> mCamera; // null if not used
		unsigned int mSceneId;
		bool mDepthTest = false;

		RenderPass(TResourceId fbId, const Color& clearColor, unsigned int sceneId);
		~RenderPass();
	};
}

#endif //GLSLSCENE_RENDER_PASS_H
