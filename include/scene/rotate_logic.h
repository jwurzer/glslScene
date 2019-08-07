#ifndef GLSLSCENE_ROTATE_LOGIC_H
#define GLSLSCENE_ROTATE_LOGIC_H

#include <ecs/logic.h>
#include <common/vector2.h>

namespace gs
{
	class RotateLogic: public Logic
	{
	public:
		RotateLogic(float angleSpeed, float rotateAxisX, float rotateAxisY, float rotateAxisZ);
		virtual ~RotateLogic();

		virtual void handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p, const SDL_Event& evt) override;
		virtual void update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p) override;
	private:
		float mAngleSpeed;
		float mRotateAxisX;
		float mRotateAxisY;
		float mRotateAxisZ;
	};
}

#endif
