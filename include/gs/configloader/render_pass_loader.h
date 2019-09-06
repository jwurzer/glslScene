#ifndef GLSLSCENE_RENDER_PASS_LOADER_H
#define GLSLSCENE_RENDER_PASS_LOADER_H

namespace cfg
{
	class NameValuePair;
}

namespace gs
{
	class SceneManager;
	class RenderPassManager;
	class ResourceManager;

	namespace renderpassloader
	{
		bool addRenderPasses(gs::RenderPassManager& pm, const gs::SceneManager& sm,
				const gs::ResourceManager& rm, const cfg::NameValuePair& cfg);
	}
}

#endif //GLSLSCENE_RENDER_PASS_LOADER_H
