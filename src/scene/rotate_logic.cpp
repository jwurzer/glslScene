#include <scene/rotate_logic.h>
#include <system/log.h>
#include <ecs/entity.h>
#include <ecs/transform_component.h>
#include <rendering/properties.h>

gs::RotateLogic::RotateLogic(float angleSpeed, float rotateAxisX, float rotateAxisY, float rotateAxisZ)
		:mAngleSpeed(angleSpeed),
		mRotateAxisX(rotateAxisX),
		mRotateAxisY(rotateAxisY),
		mRotateAxisZ(rotateAxisZ)
{
}

gs::RotateLogic::~RotateLogic()
{
}

void gs::RotateLogic::handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& p, const SDL_Event& evt)
{
}

void gs::RotateLogic::update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& p)
{
	e->transform2d().rotateDegree(mAngleSpeed * p.mDeltaTimeSec, mRotateAxisX, mRotateAxisY, mRotateAxisZ);
}
