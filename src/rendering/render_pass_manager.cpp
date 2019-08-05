#include <rendering/render_pass_manager.h>
#include <rendering/gl_api.h>
#include <rendering/properties.h>
#include <rendering/renderer.h>
#include <res/resource_manager.h>
#include <res/framebuffer.h>
#include <scene/scene_manager.h>
#include <scene/scene.h>
#include <system/log.h>
#include <camera.h>
#include <glm/gtc/type_ptr.hpp>

gs::RenderPassManager::RenderPassManager()
{
}

gs::RenderPassManager::~RenderPassManager()
{
}

void gs::RenderPassManager::addPass(const gs::RenderPass &pass)
{
	mPasses.push_back(pass);
}

void gs::RenderPassManager::handleEventForCameras(const SDL_Event& e)
{
	for (auto& pass : mPasses) {
		if (pass.mCamera) {
			pass.mCamera->handleEvent(e);
		}
	}
}

void gs::RenderPassManager::updateCameras(float seconds)
{
	for (auto& pass : mPasses) {
		if (pass.mCamera) {
			pass.mCamera->update(seconds);
		}
	}
}

void gs::RenderPassManager::renderAllPasses(Renderer &renderer,
		const SceneManager& sm, const ResourceManager& rm,
		const Properties& properties)
{
	Properties p = properties;

	size_t passCount = mPasses.size();
	for (size_t i = 0; i < passCount; ++i) {
		const RenderPass& pass = mPasses[i];

		Size2u resolution;
		if (!pass.mFramebufferId) {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, p.mWindowSizeI.mWidth, p.mWindowSizeI.mHeight);
			resolution = Size2u(p.mWindowSizeI.mWidth, p.mWindowSizeI.mHeight);
		}
		else {
			std::shared_ptr<Framebuffer> fb = rm.getFramebufferByIdNumber(pass.mFramebufferId);
			if (!fb) {
				LOGE("Can't find frame buffer!\n");
				continue;
			}
			fb->bind(); // also set the viewport!!!
			resolution = Size2u(fb->getWidth(), fb->getHeight());
		}
		p.mViewportPosPixelI = Vector2i(0, 0);
		p.mViewportSizePixelU = resolution;
		p.mViewportPosPixel = Vector2f(0.0f, 0.0f);
		p.mViewportSizePixel = Size2f(static_cast<float>(resolution.mWidth), static_cast<float>(resolution.mHeight));

		//mProperties.mViewSize = Size2f(static_cast<float>(width), static_cast<float>(height));
		//mProperties.mViewRatio = Size2f(float(width) / float(height), 1.0f);

		glClearColor(pass.mClearColor.r, pass.mClearColor.g, pass.mClearColor.b, pass.mClearColor.a);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		std::shared_ptr<Scene> scene = sm.getSceneByIdNumber(pass.mSceneId);
		if (!scene) {
			LOGE("Can't find scene for rendering!\n");
			continue;
		}

		glm::vec3 viewSize;
		p.mProjectionMatrix = pass.mProjection.applyProjection(p,
				float(resolution.mWidth), float(resolution.mHeight), viewSize);
		p.mViewSize = viewSize;
		p.mViewRatio = Size2f(viewSize.x / viewSize.y, 1.0f);

		p.mViewMatrix = pass.mViewMatrix;
		if (pass.mCamera) {
			glm::vec3 eye, center, up;
			pass.mCamera->getPropertiesForLookAt(eye, center, up);
			p.mViewMatrix = glm::lookAt(eye, center, up);
		}

		if (p.mUseGlTransforms) {
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glMultMatrixf(glm::value_ptr(p.mViewMatrix));
		}

		if (pass.mDepthTest) {
			glEnable(GL_DEPTH_TEST);
		}

		renderer.render(scene->getRootOe(), rm, p);

		if (pass.mDepthTest) {
			glDisable(GL_DEPTH_TEST);
		}

		renderer.switchToTextureUnitForLoading();
	}
}

bool gs::RenderPassManager::isValid() const
{
	if (mPasses.empty()) {
		return false;
	}
	size_t cnt = mPasses.size();
	for (size_t i = 0; i < cnt - 1; ++i) {
		if (!mPasses[i].mFramebufferId) {
			LOGE("framebuffer id 0 (default) is only allowed for the last render-pass\n");
			return false;
		}
	}
	if (mPasses.back().mFramebufferId) {
		LOGE("Last render-pass must use framebuffer id 0 (default).\n");
		return false;
	}
	return true;
}

