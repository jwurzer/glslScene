#ifndef GLSLSCENE_LOGIC_COMPONENT_H
#define GLSLSCENE_LOGIC_COMPONENT_H

#include <ecs/component.h>
#include <SDL_events.h>
#include <vector>

namespace gs
{
	class Properties;
	class ResourceManager;
	class Logic;

	class LogicComponent : public Component
	{
	public:
		LogicComponent(const std::weak_ptr<Entity>& entity);
		virtual ~LogicComponent();

		virtual EComponent getType() const { return COMPONENT_LOGIC; }

		void addLogic(std::unique_ptr<Logic> logic);

		void handleEvent( ResourceManager& rm, const Properties& p, const SDL_Event& evt);

		void update(ResourceManager& rm, const Properties& p);
	private:
		std::vector<std::unique_ptr<Logic> > mLogics;
	};
}

#endif //GLSLSCENE_LOGIC_COMPONENT_H
