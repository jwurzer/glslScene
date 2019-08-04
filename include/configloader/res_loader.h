#ifndef GLSLSCENE_RES_LOADER_H
#define GLSLSCENE_RES_LOADER_H

#include <res/resource_id.h>
#include <memory>

namespace gs
{
	class ResourceManager;
	class CfgValuePair;

	namespace resloader
	{
		bool addResources(ResourceManager& rm, const CfgValuePair& cfgValue);
		TResourceId addResource(ResourceManager& rm, const CfgValuePair& cfgValue);
		TTextureId addTexture(ResourceManager& rm, const CfgValuePair& cfgValuePair);
		TShaderId addShaderProgram(ResourceManager& rm, const CfgValuePair& cfgValuePair);
		TMeshId addMesh(ResourceManager& rm, const CfgValuePair& cfgValuePair);
		TFramebufferId addFramebuffer(ResourceManager& rm, const CfgValuePair& cfgValuePair);
	}
}

#endif //GLSLSCENE_RES_LOADER_H
