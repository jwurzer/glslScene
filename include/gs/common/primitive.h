#ifndef GLSLSCENE_PRIMITIVE_H
#define GLSLSCENE_PRIMITIVE_H

namespace gs
{
	// see https://www.khronos.org/opengl/wiki/Primitive
	enum class PrimitiveType
	{
		POINTS = 0,
		LINES,
		LINE_LOOP,
		LINE_STRIP,
		TRIANGLES,
		TRIANGLE_STRIP,
		TRIANGLE_FAN,
		QUADS,
		QUAD_STRIP,
		POLYGON,
	};
}

#endif //GLSLSCENE_PRIMITIVE_H
