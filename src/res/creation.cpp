#include <res/creation.h>
#include <res/mesh.h>

bool gs::creation::addTriangles(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount)
{
	return mesh.addVertices(vertices, vertexSize, vertexCount, posCount, texCount, colorCount, customCount);
}

bool gs::creation::addQuads(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int texCount,
		unsigned int colorCount)
{
	return false;
}

bool gs::creation::addRects(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int texCount,
		unsigned int colorCount)
{
	return false;
}

