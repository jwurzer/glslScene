#ifndef GLSLSCENE_RECT_FWD_H
#define GLSLSCENE_RECT_FWD_H

#include <stdint.h>

namespace gs
{
	template <typename T> class Rect;
	typedef Rect<int> RectInt;
	typedef Rect<float> RectFloat;
	typedef Rect<int8_t> RectI8;
	typedef Rect<int16_t> RectI16;
	typedef Rect<int32_t> RectI32;
	typedef Rect<int64_t> RectI64;
}

#endif // GLSLSCENE_RECT_FWD_H
