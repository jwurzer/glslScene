#ifndef GLSLSCENE_RESOURCE_ID_LIMIT_H
#define GLSLSCENE_RESOURCE_ID_LIMIT_H

#include <climits>

namespace gs
{
	enum : unsigned int
	{
		AUDIO_ID_INVALID = UINT_MAX,
		TEXTURE_ID_INVALID = UINT_MAX,
		GRAPHIC_ID_INVALID = UINT_MAX,
		FONT_ID_INVALID = UINT_MAX,
	};
}

#endif
