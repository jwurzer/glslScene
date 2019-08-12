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

namespace gs
{
	class CfgValuePair;
	class ContextProperties;

	namespace contextloader
	{
		bool getContextParameters(const CfgValuePair& cfgValue,
				ContextProperties& p);
	}
}

#endif //GLSLSCENE_CONTEXT_LOADER_H
