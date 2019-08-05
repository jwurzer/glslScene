#include <rendering/render_pass.h>
#include <camera.h>

gs::RenderPass::RenderPass(TResourceId fbId, const Color& clearColor, unsigned int sceneId)
		:mFramebufferId(fbId), mClearColor(clearColor), mProjection(), mCamera(), mSceneId(sceneId)
{
}

gs::RenderPass::~RenderPass()
{
}
