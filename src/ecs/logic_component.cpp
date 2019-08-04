#include <ecs/logic_component.h>
#include <ecs/logic.h>
#include <system/log.h>

gs::LogicComponent::LogicComponent(const std::weak_ptr<gs::Entity> &entity)
		:Component(entity), mLogics()
{
}

gs::LogicComponent::~LogicComponent()
{
}

void gs::LogicComponent::addLogic(std::unique_ptr<Logic> logic)
{
	mLogics.push_back(std::move(logic));
}

void gs::LogicComponent::handleEvent(ResourceManager& rm, const Properties& p, const SDL_Event& evt)
{
	if (mLogics.empty()) {
		return;
	}
	std::shared_ptr<Entity> e = getEntity();
	if (!e) {
		return;
	}
	for (auto& l : mLogics) {
		l->handleEvent(e, rm, p, evt);
	}
}

void gs::LogicComponent::update(ResourceManager& rm, const Properties& p)
{
	if (mLogics.empty()) {
		return;
	}
	std::shared_ptr<Entity> e = getEntity();
	if (!e) {
		return;
	}
	for (auto& l : mLogics) {
		l->update(e, rm, p);
	}
}

