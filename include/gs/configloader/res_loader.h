#ifndef GLSLSCENE_RES_LOADER_H
#define GLSLSCENE_RES_LOADER_H

#include <gs/res/resource_id.h>
#include <memory>

namespace cfg
{
	class NameValuePair;
}

namespace gs
{
	class ResourceManager;

	namespace resloader
	{
		bool addResources(gs::ResourceManager& rm, const cfg::NameValuePair& cfgValue);
		TResourceId addResource(gs::ResourceManager& rm, const cfg::NameValuePair& cfgValue);
		TTextureId addTexture(gs::ResourceManager& rm, const cfg::NameValuePair& cfgValuePair);
		TShaderId addShaderProgram(gs::ResourceManager& rm, const cfg::NameValuePair& cfgValuePair);
		TMeshId addMesh(gs::ResourceManager& rm, const cfg::NameValuePair& cfgValuePair);
		TFramebufferId addFramebuffer(gs::ResourceManager& rm, const cfg::NameValuePair& cfgValuePair);
	}
}

#endif //GLSLSCENE_RES_LOADER_H
