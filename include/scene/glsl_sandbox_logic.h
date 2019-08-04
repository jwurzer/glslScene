#ifndef GLSLSCENE_GLSL_SANDBOX_LOGIC_H
#define GLSLSCENE_GLSL_SANDBOX_LOGIC_H

#include <ecs/logic.h>
#include <common/vector2.h>

namespace gs
{
	class GlslSandboxLogic: public Logic
	{
	public:
		GlslSandboxLogic();
		virtual ~GlslSandboxLogic();

		virtual void handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p, const SDL_Event& evt) override;
		virtual void update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p) override;
	private:
		void updateMeshAndUniform(const std::shared_ptr<Entity>& e,
				ResourceManager& rm, const Properties& p);
		float mOffsetX, mOffsetY;
		bool mMouseBtnLeftIsPressed;
		bool mMouseBtnRightIsPressed;
		bool mMouseMoved;
		Vector2f mMouseDiff;
		float mZoomFactor;
	};
}

#endif //GLSLSCENE_GLSL_SANDBOX_LOGIC_H
