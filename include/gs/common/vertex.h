#ifndef GLSLSCENE_VERTEX_H
#define GLSLSCENE_VERTEX_H

namespace gs
{
	// https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glInterleavedArrays.xml

	// GL_V2F,
	// GL_V3F,
	// GL_C3F_V3F,
	// GL_N3F_V3F,
	// GL_C4F_N3F_V3F,
	// GL_T2F_V3F,
	// GL_T4F_V4F,
	// GL_T2F_C3F_V3F,
	// GL_T2F_N3F_V3F,
	// GL_T2F_C4F_N3F_V3F,
	// GL_T4F_C4F_N3F_V4F

	// GL_C4UB_V2F,
	// GL_C4UB_V3F,
	// GL_T2F_C4UB_V3F,

	struct VertexV2
	{
		float x, y;
	};

	struct VertexV3
	{
		float x, y, z;
	};

	struct VertexC3V3
	{
		float r, g, b;
		float x, y, z;
	};

	struct VertexV3C4
	{
		float x, y, z;
		float r, g, b, a;
	};

	struct VertexV3T2C4
	{
		float x, y, z;
		float s, t;
		float r, g, b, a;
	};

	struct VertexV3T2T2C4
	{
		float x, y, z;
		float s0, t0;
		float s1, t1;
		float r, g, b, a;
	};

	struct VertexV3T2T2T2C4
	{
		float x, y, z;
		float s0, t0;
		float s1, t1;
		float s2, t2;
		float r, g, b, a;
	};

	struct VertexV3T2T2T2T2C4
	{
		float x, y, z;
		float s0, t0;
		float s1, t1;
		float s2, t2;
		float s3, t3;
		float r, g, b, a;
	};
}

#endif //GLSLSCENE_VERTEX_H
