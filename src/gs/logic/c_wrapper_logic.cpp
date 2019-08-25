#include <gs/logic/c_wrapper_logic.h>

#include <gs/ecs/entity.h>
#include <gs/rendering/properties.h>

#include <capi/ecs/entity.h>
#include <capi/rendering/properties.h>

gs::CWrapperLogic::CWrapperLogic(std::unique_ptr<::Logic> logic)
		:mLogic(std::move(logic))
{
	mLogic->logicInit(mLogic->logicInstance);
}

gs::CWrapperLogic::~CWrapperLogic()
{
	mLogic->logicDestroy(mLogic->logicInstance);
}

void gs::CWrapperLogic::handleEvent(const std::shared_ptr<gs::Entity> &e,
		gs::ResourceManager &rm, const gs::Properties &p, const SDL_Event &evt)
{
}

void gs::CWrapperLogic::update(const std::shared_ptr<gs::Entity> &e,
		gs::ResourceManager &rm, const gs::Properties &p)
{
	gs::Properties propCopy = p;
	mLogic->logicUpdate(mLogic->logicInstance, e.get(), &propCopy);
}
