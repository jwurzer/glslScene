#ifndef GLSLSCENE_RENDERER_H
#define GLSLSCENE_RENDERER_H

#include <rendering/properties.h>
#include <rendering/texture_unit_stack.h>
#include <rendering/shader_stack.h>
#include <rendering/matrices.h>
#include <memory>
#include <vector>
#include <SDL.h>

namespace gs
{
	class Entity;
	class ResourceManager;
	class View;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();

		void switchToTextureUnitForLoading() { mTextureStack.switchToTextureUnitForLoading(); }

		void render(const std::shared_ptr<Entity>& e, const ResourceManager& rm,
				const Properties& properties);
	private:
		std::vector<Matrices> mMatrixStack;
		TextureUnitStack mTextureStack;
		ShaderStack mShaderStack;

		void renderEntity(const std::shared_ptr<Entity>& e, const ResourceManager& rm,
				const Properties& properties);
	};
}

#endif
