#ifndef GLSLSCENE_RENDER_PASS_MANAGER_H
#define GLSLSCENE_RENDER_PASS_MANAGER_H

#include <rendering/render_pass.h>
#include <vector>

namespace gs
{
	class Entity;
	class Renderer;
	class SceneManager;
	class ResourceManager;
	class Properties;

	class RenderPassManager
	{
	public:
		RenderPassManager();
		~RenderPassManager();
		void addPass(const RenderPass& pass);
		void renderAllPasses(Renderer &renderer,
				const SceneManager& sm, const ResourceManager& rm,
				const Properties& properties);
		bool isValid() const;
	private:
		std::vector<RenderPass> mPasses;
	};
}

#endif //GLSLSCENE_RENDER_PASS_MANAGER_H
