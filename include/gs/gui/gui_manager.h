#ifndef GLSLSCENE_GUI_MANAGER_H
#define GLSLSCENE_GUI_MANAGER_H

#include <SDL.h>

namespace gs
{
	class Renderer;
	class RenderPassManager;
	class SceneManager;
	class ResourceManager;
	class ContextProperties;
	class Properties;
	class FileChangeMonitoring;

	class GuiManager
	{
	public:
		GuiManager();
		~GuiManager();

		// SDL_GLContext is a void* (typedef void *SDL_GLContext)
		void initImGui(SDL_Window* window, SDL_GLContext context,
				const ContextProperties& contextProperties);
		void destroyImGui();

		void handleEvent(const SDL_Event& e);
		void render(Renderer &renderer, const RenderPassManager& pm,
				const SceneManager& sm, const ResourceManager& rm,
				const ContextProperties& cp, const Properties& properties,
				const FileChangeMonitoring& fcm);
	private:
		SDL_Window* mWindow;
		SDL_GLContext mContext; // SDL_GLContext is a void* (typedef void *SDL_GLContext)
		bool mUseOpenGl3;
	};
}

#endif //GLSLSCENE_GUI_MANAGER_H
