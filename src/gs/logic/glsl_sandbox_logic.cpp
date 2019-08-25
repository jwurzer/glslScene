#include <gs/logic/glsl_sandbox_logic.h>
#include <gs/system/log.h>
#include <gs/ecs/entity.h>
#include <gs/ecs/mesh_component.h>
#include <gs/ecs/shader_component.h>
#include <gs/res/resource_manager.h>
#include <gs/res/mesh.h>
#include <gs/res/shader_program.h>
#include <gs/rendering/properties.h>

gs::GlslSandboxLogic::GlslSandboxLogic()
		:mOffsetX(0.0f), mOffsetY(0.0f),
		mMouseBtnLeftIsPressed(false),
		mMouseBtnRightIsPressed(false),
		mMouseMoved(false),
		mMouseDiff(),
		mZoomFactor(1.0f)
{
}

gs::GlslSandboxLogic::~GlslSandboxLogic()
{
}

void gs::GlslSandboxLogic::handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& p, const SDL_Event& evt)
{
	switch (evt.type) {
		case SDL_WINDOWEVENT:
			if (evt.window.event == SDL_WINDOWEVENT_RESIZED) {
				LOGI("resize window to %dx%d\n", int(evt.window.data1), int(evt.window.data2));
			}
			updateMeshAndUniform(e, rm, p);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (evt.button.button == 1) { // right
				mMouseBtnLeftIsPressed = true;
			}
			else if (evt.button.button == 3) { // left
				mMouseBtnRightIsPressed = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (evt.button.button == 1) { // right
				mMouseBtnLeftIsPressed = false;
			}
			else if (evt.button.button == 3) { // left
				mMouseBtnRightIsPressed = false;
			}
			break;
		case SDL_MOUSEMOTION:
			mMouseDiff.x = float(evt.motion.xrel) / p.mWindowSize.mWidth * p.mWindowRatio.mWidth;
			mMouseDiff.y = float(-evt.motion.yrel) / p.mWindowSize.mHeight * p.mWindowRatio.mHeight;
			mMouseMoved = true;
			break;
	}
}

void gs::GlslSandboxLogic::update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& p)
{
	bool updateNecessary = false;
	if (mMouseMoved) {
		if (mMouseBtnLeftIsPressed) {
			mOffsetX -= mMouseDiff.x * mZoomFactor;
			mOffsetY -= mMouseDiff.y * mZoomFactor;
			updateNecessary = true;
		}
		if (mMouseBtnRightIsPressed) {
			mZoomFactor *= 1.0f - (mMouseDiff.x + mMouseDiff.y) * 5.0f;
			updateNecessary = true;
		}
	}
	if (updateNecessary) {
		updateMeshAndUniform(e, rm, p);
		LOGI("offset %f %f, zoom %f\n", mOffsetX, mOffsetY, mZoomFactor);
	}
	mMouseMoved = false;
}

void gs::GlslSandboxLogic::updateMeshAndUniform(const std::shared_ptr<Entity>& e,
		ResourceManager& rm, const Properties& p)
{
	const gs::MeshComponent* meshComp = e->getConstMesh();
	if (!meshComp) {
		LOGE("failed\n");
		return;
	}
	std::shared_ptr<gs::Mesh> mesh = rm.getMeshByIdNumber(meshComp->getGraphicId());
	if (!mesh) {
		LOGE("failed\n");
		return;
	}
	struct Vertex
	{
		float vx, vy; // vertex-position
		float sx, sy; // "surface-position"
	};
	float vx = 1.0;
	float vy = 1.0;
	float sx = p.mWindowRatio.mWidth * 0.5f * mZoomFactor;
	float sy = p.mWindowRatio.mHeight * 0.5f * mZoomFactor;
	Vertex vertices[] = {
			{-vx, -vy, -sx + mOffsetX, -sy + mOffsetY},
			{ vx, -vy,  sx + mOffsetX, -sy + mOffsetY},
			{-vx,  vy, -sx + mOffsetX,  sy + mOffsetY},
			{ vx, -vy,  sx + mOffsetX, -sy + mOffsetY},
			{ vx,  vy,  sx + mOffsetX,  sy + mOffsetY},
			{-vx,  vy, -sx + mOffsetX,  sy + mOffsetY}
	};
	mesh->clear();
	if (!mesh->addVertices(vertices, sizeof(Vertex), 6, 0, 0, 0, 0, 4)) {
		LOGW("Change mesh failed!\n");
	}

	const ShaderComponent* shaderComp = e->getConstShader();
	if (!shaderComp) {
		LOGE("failed\n");
		return;
	}
	std::shared_ptr<ShaderProgram> shader = rm.getShaderProgramByIdNumber(shaderComp->getShaderProgramId());
	if (!shader) {
		LOGE("failed\n");
		return;
	}
	glm::vec2 surfaceSize(sx * 2.0, sy * 2.0);
	if (!shader->changeUniformVec2("surfaceSize", surfaceSize)) {
		LOGW("Can't change uniform \"surfaceSize\"\n");
	}
	LOGI("surfaceSize is %f %f", surfaceSize.x, surfaceSize.y);
}

