#ifndef GLSLSCENE_RENDER_PASS_H
#define GLSLSCENE_RENDER_PASS_H

#include <res/resource_id.h>
#include <scene/projection.h>
#include <common/color.h>
#include <glm/mat4x4.hpp>

namespace gs
{
	class RenderPass
	{
	public:
		TResourceId mFramebufferId;
		// TODO: viewport size could be also added here. Now always use resolution of the framebuffer for viewport
		Color mClearColor;
		Projection mProjection;
		glm::mat4 mViewMatrix = glm::mat4(1.0f);
		unsigned int mSceneId;
		bool mDepthTest = false;

		RenderPass(TResourceId fbId, const Color& clearColor, unsigned int sceneId)
				:mFramebufferId(fbId), mClearColor(clearColor), mProjection(), mSceneId(sceneId) {}
	};
}

#endif //GLSLSCENE_RENDER_PASS_H
