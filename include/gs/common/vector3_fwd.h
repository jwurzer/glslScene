#ifndef GLSLSCENE_VECTOR3_FWD_H
#define GLSLSCENE_VECTOR3_FWD_H

#include <stdint.h>

namespace gs
{
	template <typename T> class Vector3;
	typedef Vector3<int> Vector3i;
	typedef Vector3<unsigned int> Vector3u;
	typedef Vector3<float> Vector3f;
	typedef Vector3<int8_t> Vector3i8;
	typedef Vector3<int16_t> Vector3i16;
	typedef Vector3<int32_t> Vector3i32;
	typedef Vector3<int64_t> Vector3i64;
}

#endif // GLSLSCENE_VECTOR3_FWD_H
