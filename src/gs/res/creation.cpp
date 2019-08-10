#include <gs/res/creation.h>
#include <gs/res/mesh.h>

bool gs::creation::addTriangles(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int normalCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount)
{
	mesh.setPrimitiveType(PrimitiveType::TRIANGLES);
	return mesh.addVertices(vertices, vertexSize, vertexCount, posCount, normalCount, texCount, colorCount, customCount);
}

bool gs::creation::addPointMesh(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int normalCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount,
		unsigned int xCnt, unsigned int yCnt)
{
	if (vertexCount != 3) {
		return false;
	}
	unsigned int compCount = vertexSize / sizeof(float);
	const float* v = static_cast<const float*>(vertices);

	std::vector<float> vStart(v, v + compCount);
	std::vector<float> vxDiff(v + compCount, v + compCount * 2);
	std::vector<float> vyDiff(v + compCount * 2, v + compCount * 3);

	float xScale = 1.0f / float(xCnt - 1);
	float yScale = 1.0f / float(yCnt - 1);
	for (unsigned int i = 0; i < compCount; ++i) {
		vxDiff[i] *= xScale;
		vyDiff[i] *= yScale;
	}
	
	std::vector<float> vyStart = vStart;
	for (unsigned int y = 0; y < yCnt; ++y) {
		std::vector<float> vx = vyStart;
		for (unsigned int x = 0; x < xCnt; ++x) {
			mesh.addVertices(vx.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			for (unsigned int i = 0; i < compCount; ++i) {
				vx[i] += vxDiff[i];
			}
		}
		for (unsigned int i = 0; i < compCount; ++i) {
			vyStart[i] += vyDiff[i];
		}
	}

	mesh.setPrimitiveType(PrimitiveType::POINTS);
	return true;
}

bool gs::creation::addTriangleMesh(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int normalCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount,
		unsigned int xCnt, unsigned int yCnt)
{
	if (vertexCount != 3) {
		return false;
	}
	unsigned int compCount = vertexSize / sizeof(float);
	const float* v = static_cast<const float*>(vertices);

	std::vector<float> vStart(v, v + compCount);
	std::vector<float> vxDiff(v + compCount, v + compCount * 2);
	std::vector<float> vyDiff(v + compCount * 2, v + compCount * 3);

	float xScale = 1.0f / float(xCnt - 1);
	float yScale = 1.0f / float(yCnt - 1);
	for (unsigned int i = 0; i < compCount; ++i) {
		vxDiff[i] *= xScale;
		vyDiff[i] *= yScale;
	}
	
	std::vector<float> vy1Start = vStart;
	std::vector<float> vy2Start = vStart;
	for (unsigned int i = 0; i < compCount; ++i) {
		vy2Start[i] += vyDiff[i];
	}
	for (unsigned int y = 0; y < yCnt - 1; ++y) {
		std::vector<float> vx1 = vy1Start; // top
		std::vector<float> vx1b = vy1Start; // top, right
		std::vector<float> vx2 = vy2Start; // bottom
		std::vector<float> vx2b = vy2Start; // bottom, right
		for (unsigned int i = 0; i < compCount; ++i) {
			vx1b[i] += vxDiff[i];
			vx2b[i] += vxDiff[i];
		}
		for (unsigned int x = 0; x < xCnt - 1; ++x) {
			mesh.addVertices(vx1.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx1b.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx2b.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx1.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx2b.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx2.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			for (unsigned int i = 0; i < compCount; ++i) {
				vx1[i] += vxDiff[i];
				vx2[i] += vxDiff[i];
				vx1b[i] += vxDiff[i];
				vx2b[i] += vxDiff[i];
			}
		}
		for (unsigned int i = 0; i < compCount; ++i) {
			vy1Start[i] += vyDiff[i];
			vy2Start[i] += vyDiff[i];
		}
	}

	mesh.setPrimitiveType(PrimitiveType::TRIANGLES);
	return true;
}

bool gs::creation::addQuadMesh(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int normalCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount,
		unsigned int xCnt, unsigned int yCnt)
{
	if (vertexCount != 3) {
		return false;
	}
	unsigned int compCount = vertexSize / sizeof(float);
	const float* v = static_cast<const float*>(vertices);

	std::vector<float> vStart(v, v + compCount);
	std::vector<float> vxDiff(v + compCount, v + compCount * 2);
	std::vector<float> vyDiff(v + compCount * 2, v + compCount * 3);

	float xScale = 1.0f / float(xCnt - 1);
	float yScale = 1.0f / float(yCnt - 1);
	for (unsigned int i = 0; i < compCount; ++i) {
		vxDiff[i] *= xScale;
		vyDiff[i] *= yScale;
	}
	
	std::vector<float> vy1Start = vStart;
	std::vector<float> vy2Start = vStart;
	for (unsigned int i = 0; i < compCount; ++i) {
		vy2Start[i] += vyDiff[i];
	}
	for (unsigned int y = 0; y < yCnt - 1; ++y) {
		std::vector<float> vx1 = vy1Start; // top
		std::vector<float> vx1b = vy1Start; // top, right
		std::vector<float> vx2 = vy2Start; // bottom
		std::vector<float> vx2b = vy2Start; // bottom, right
		for (unsigned int i = 0; i < compCount; ++i) {
			vx1b[i] += vxDiff[i];
			vx2b[i] += vxDiff[i];
		}
		for (unsigned int x = 0; x < xCnt - 1; ++x) {
			mesh.addVertices(vx1.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx1b.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx2b.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx1.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx2b.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			mesh.addVertices(vx2.data(), vertexSize, 1, posCount, normalCount, texCount, colorCount, customCount);
			for (unsigned int i = 0; i < compCount; ++i) {
				vx1[i] += vxDiff[i];
				vx2[i] += vxDiff[i];
				vx1b[i] += vxDiff[i];
				vx2b[i] += vxDiff[i];
			}
		}
		for (unsigned int i = 0; i < compCount; ++i) {
			vy1Start[i] += vyDiff[i];
			vy2Start[i] += vyDiff[i];
		}
	}

	mesh.setPrimitiveType(PrimitiveType::TRIANGLES);
	return true;
}

bool gs::creation::addQuads(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int normalCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount)
{
	mesh.setPrimitiveType(PrimitiveType::QUADS);
	return mesh.addVertices(vertices, vertexSize, vertexCount, posCount, normalCount, texCount, colorCount, customCount);
}

bool gs::creation::addRects(Mesh& mesh, const void* vertices,
		unsigned int vertexSize, unsigned int vertexCount,
		unsigned int posCount,
		unsigned int normalCount,
		unsigned int texCount,
		unsigned int colorCount,
		unsigned int customCount)
{
	return false;
}

