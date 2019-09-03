#include "gs/logic/rotate_logic.h"
#include <gs/system/log.h>
#include <gs/ecs/entity.h>
#include <gs/ecs/transform_component.h>
#include <gs/rendering/properties.h>

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
	e->transform().rotateDegree(mAngleSpeed * p.mDeltaTimeSec, mRotateAxisX, mRotateAxisY, mRotateAxisZ);
}
