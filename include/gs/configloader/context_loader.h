#ifndef GLSLSCENE_CONTEXT_LOADER_H
#define GLSLSCENE_CONTEXT_LOADER_H

#include <gs/context.h>

/*
# context
# -------
# version = <...>

version = opengl [<major>.<minor>] [<profile>] [forward]
version = opengl es [<major>.<minor>]
version = gles [<major>.<minor>]

profile can be core or compatibility (or short version compat)
*/

namespace cfg
{
	class NameValuePair;
}

namespace gs
{
	class ContextProperties;

	namespace contextloader
	{
		bool getContextParameters(const cfg::NameValuePair& cfgValue,
				gs::ContextProperties& p);
	}
}

#endif //GLSLSCENE_CONTEXT_LOADER_H
