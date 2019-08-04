#ifndef GLSLSCENE_CORE_SHADER_STACK_H
#define GLSLSCENE_CORE_SHADER_STACK_H

#include <common/rect.h>
#include <rendering/properties.h>
#include <rendering/matrices.h>
#include <memory>

//#define ENABLE_SHADER_STACK_STATISTIC

namespace gs
{
	class ShaderProgram;

	class ShaderStack
	{
	public:
		ShaderStack();
		~ShaderStack();

		void resetStack();

		// called one per screen
		void setGlobalProperties(const Properties& p);
		void setMatrices(const Matrices& m);

		void pushShaderProgram(ShaderProgram* shader);
		void popShaderProgram();

		// bind or unbind correct shader for rendering
		void bindForRendering();

		const ShaderProgram* getCurrentBindedShader() const { return mCurrentBindedShader; }

		enum EDefs
		{
			MAX_LAYERS = 32,
		};
	private:
		class ShaderBindInfo
		{
		public:
			ShaderProgram* shader = nullptr;
		};

#if defined(_MSC_VER)
		ShaderBindInfo mLayers[MAX_LAYERS];
#else
		std::array<ShaderBindInfo, MAX_LAYERS> mLayers;
#endif
		int mNextLayerIndex;
		ShaderProgram* mCurrentBindedShader;
		ShaderProgram* mShaderToBind;

		Properties mProperties;
		Matrices mMatrices;

		void bindShaderProgram(ShaderProgram* shaderProgram);
	};
}

#endif //GLSLSCENE_CORE_SHADER_STACK_H
