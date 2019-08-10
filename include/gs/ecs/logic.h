#ifndef GLSLSCENE_LOGIC_H
#define GLSLSCENE_LOGIC_H

#include <SDL_events.h>
#include <memory>

namespace gs
{
	class Entity;
	class ResourceManager;
	class Properties;

	class Logic
	{
	public:
		Logic();
		virtual ~Logic();

		virtual void handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p, const SDL_Event& evt) = 0;
		virtual void update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p) = 0;
	};
}

#endif //GLSLSCENE_LOGIC_H
