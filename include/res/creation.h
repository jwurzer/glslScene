#ifndef GLSLSCENE_CREATION_H
#define GLSLSCENE_CREATION_H

namespace gs
{
	class Mesh;

	namespace creation
	{
		bool addTriangles(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount);
		bool addQuads(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int texCount,
				unsigned int colorCount);
		bool addRects(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int texCount,
				unsigned int colorCount);
	}
}

#endif //GLSLSCENE_CREATION_H
