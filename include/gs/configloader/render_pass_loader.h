#ifndef GLSLSCENE_RENDER_PASS_LOADER_H
#define GLSLSCENE_RENDER_PASS_LOADER_H

namespace gs
{
	class SceneManager;
	class RenderPassManager;
	class ResourceManager;
	class CfgValuePair;

	namespace renderpassloader
	{
		bool addRenderPasses(RenderPassManager& pm, const SceneManager& sm,
				const ResourceManager& rm, const CfgValuePair& cfg);
	}
}

#endif //GLSLSCENE_RENDER_PASS_LOADER_H
