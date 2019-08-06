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
				unsigned int normalCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount);
		bool addPointMesh(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int normalCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount,
				unsigned int xCnt, unsigned int yCnt);
		bool addTriangleMesh(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int normalCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount,
				unsigned int xCnt, unsigned int yCnt);
		bool addQuadMesh(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int normalCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount,
				unsigned int xCnt, unsigned int yCnt);
		bool addQuads(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int normalCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount);
		bool addRects(Mesh& mesh, const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int normalCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount);
	}
}

#endif //GLSLSCENE_CREATION_H
