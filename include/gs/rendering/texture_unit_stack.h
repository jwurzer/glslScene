#ifndef GLSLSCENE_CORE_TEXTURE_UNIT_STACK_H
#define GLSLSCENE_CORE_TEXTURE_UNIT_STACK_H

#include <array>
#include <memory>

//#define ENABLE_TEXTURE_UNIT_STACK_STATISTIC

namespace gs
{
	class Texture;

	class TextureUnitStack
	{
	public:
		TextureUnitStack();
		~TextureUnitStack();

		void resetStack();

		void pushTexture(Texture* tex, unsigned int textureUnit);
		void popTexture(unsigned int textureUnit);

		// bind or unbind correct textures for rendering
		void bindForRendering();

		void switchToTextureUnitForLoading();
		void switchToTextureUnit(unsigned int textureUnit);

		void resetStatistic();
		void printStatistic();

		enum EDefs
		{
			MAX_UNITS = 8,
			MAX_LAYERS_PER_UNIT = 32,
		};

	private:
#ifdef ENABLE_TEXTURE_UNIT_STACK_STATISTIC
		class Statistic
		{
		public:
			int mBindCalls = 0;
			void reset()
			{
				mBindCalls = 0;
			}
		};
#endif

		class TextureUnit
		{
		public:
			// value of 0 means no binding at this layer or no texture must be "bind"
			std::array<Texture*, MAX_LAYERS_PER_UNIT> mLayers {};
			int mNextLayerIndex = 0;

			// true means: must be binded or must be "unbinded" if next value is 0
			// false means: no changes are outstanding
			// if mCurrentBindedTexture and mTextureToBind have a different value
			// then mMustBeBinded is true or "out of range/layers"
			Texture* mCurrentBindedTexture = nullptr;
			Texture* mTextureToBind = nullptr;
#ifdef ENABLE_CORE_TEXTURE_UNIT_STACK_STATISTIC
			Statistic mStat;
#endif
		};

		std::array<TextureUnit, MAX_UNITS> mUnits;
		unsigned int mMustBeBinded;

#ifdef ENABLE_CORE_TEXTURE_UNIT_STACK_STATISTIC
		Statistic mStat;
#endif

		unsigned int mCurrentActiveTextureUnit; // 0 for GL_TEXTURE0, 1 for GL_TEXTURE1, ...
		unsigned int mUsedTexUnitsFlags;

		void bindTexture(Texture* tex, unsigned int textureUnit);
	};
}

#endif /* GLSLSCENE_TEXTURE_UNIT_STACK_H */
