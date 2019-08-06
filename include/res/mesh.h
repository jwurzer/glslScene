#ifndef GLSLSCENE_MESH_H
#define GLSLSCENE_MESH_H

#include <res/resource.h>
#include <rendering/gl_api.h>
#include <common/primitive.h>
#include <vector>

namespace gs
{
	struct VertexV3C4;
	class ShaderProgram;

	class Mesh : public Resource
	{
	public:
		Mesh(bool useVaoVersion, float scaleForShowNormals);
		virtual ~Mesh();

		virtual ResType getType() const { return ResType::MESH; }
		virtual bool load() { return true; }
		virtual void unload() {}

		void clear();

		/**
		 * @param vertices Buffersize in bytes must be sizeof(float) * vertexSize * vertexCount
		 * @param vertexSize must be the same as (posCount + texCount * 2 + colorCount + customCount) * sizeof(float), unit: bytes
		 * @param vertexCount Count of vertices
		 * @param posCount 2 for x,y, 3 for x,y,z
		 * @param normalCount 0 for no normal or 3 for normal (nx, ny, nz)
		 * @param texCount 1 for one texture s,t, 2 for s0,t0,s1,t1, N for s0,t0 to s<N-1>,t<N-1>
		 * @param colorCount 3 for r,g,b, 4 for r,g,b,a
		 * @param customCount can be any value
		 * @return
		 */
		bool addVertices(const void* vertices,
				unsigned int vertexSize, unsigned int vertexCount,
				unsigned int posCount,
				unsigned int normalCount,
				unsigned int texCount,
				unsigned int colorCount,
				unsigned int customCount);

		bool addVertices(const VertexV3C4* vertices,
				unsigned int vertexCount);

		void setPrimitiveType(PrimitiveType primitiveType) { mPrimitiveType = primitiveType; }

		unsigned int getVertexCount() const { return mVertexCount; }

		/**
		 * @param shaderProgram Can be null.
		 */
		void bind(const ShaderProgram* shaderProgram);

		void draw();
		void unbind(const ShaderProgram* shaderProgram);

		void drawNormals();

		std::string toString() const;
	private:
		PrimitiveType mPrimitiveType;

		std::vector<float> mVertices;
		unsigned int mVertexSize; // in bytes!
		unsigned int mVertexCount;

		unsigned int mPosCount;
		unsigned int mNormalCount;
		unsigned int mTexCount;
		unsigned int mColorCount;
		unsigned int mCustomCount;

		struct VertexNormal
		{
			float x, y, z;
			float r, g, b, a;
		};
		std::vector<VertexNormal> mNormalVertices;

		bool mUseVaoVersion;

		float mScaleForShowNormals;

		bool mChanged = false;
		GLuint mVbo;
		GLuint mVao;
		bool mShaderAttrConfigIsUsedForVao = false;
		const ShaderProgram* mShaderForVao = nullptr;
		unsigned int mShaderLoadCounterForVao = 0;

		void bindNoVaoVersion(const ShaderProgram* shaderProgram);
		void unbindNoVaoVersion(const ShaderProgram* shaderProgram);

		void bindVaoVersion(const ShaderProgram* shaderProgram);
		void unbindVaoVersion(const ShaderProgram* shaderProgram);

		void createVao(const ShaderProgram* shaderProgram);
	};
}

#endif //GLSLSCENE_MESH_H
