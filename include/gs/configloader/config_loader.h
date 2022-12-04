#ifndef GLSLSCENE_CONFIG_LOADER_H
#define GLSLSCENE_CONFIG_LOADER_H

#include <string>

namespace cfg
{
	class NameValuePair;
}

namespace gs
{
	namespace configloader
	{
		bool getConfigAsTree(const std::string& configFilename, cfg::NameValuePair &root);
	}
}

#endif
