#include <rendering/texture_unit_stack.h>
#include <rendering/gl_api.h>

#include <res/texture.h>
#include <system/log.h>
#include <common/lookup8.h>

#ifdef ENABLE_TEXTURE_UNIT_STACK_STATISTIC
#define INCREASE_BIND_CALLS_COUNT \
	++unit.mStat.mBindCalls; \
	++mStat.mBindCalls; \

#else
#define INCREASE_BIND_CALLS_COUNT
#endif

#define SWITCH_TO_TEXTURE_UNIT(texUnitIndex) \
	if (mCurrentActiveTextureUnit != texUnitIndex) { \
		mCurrentActiveTextureUnit = texUnitIndex; \
		glActiveTexture(GL_TEXTURE0 + texUnitIndex); \
	}


gs::TextureUnitStack::TextureUnitStack()
		:mUnits(), mMustBeBinded(0),
		mCurrentActiveTextureUnit(0), mUsedTexUnitsFlags(0)
{
#if 0
	for (int i = 0; i < MAX_UNITS; ++i) {
		LOGI("%d: %08x\n", i, lookUpSet[i]);
	}
	for (int i = 0; i < MAX_UNITS; ++i) {
		LOGI("%d: %08x\n", i, lookUpReset[i]);
	}
	for (int i = 0; i < MAX_UNITS; ++i) {
		LOGI("%d: %08x\n", i, lookUpLimit[i]);
	}
	for (int i = 0; i < MAX_UNITS; ++i) {
		LOGI("%d: %08x\n", i, lookUpInvertLimit[i]);
	}
#endif
}

gs::TextureUnitStack::~TextureUnitStack()
{
}

void gs::TextureUnitStack::resetStack()
{
}

void gs::TextureUnitStack::pushTexture(Texture* tex, unsigned int textureUnit)
{
	TextureUnit& unit = mUnits[textureUnit];

	int index = unit.mNextLayerIndex;
	++unit.mNextLayerIndex;
	if (index >= MAX_LAYERS_PER_UNIT) {
		LOGE("Out of texture stack for push! %d >= %d\n", index, MAX_LAYERS_PER_UNIT);
		return;
	}

	unit.mLayers[index] = tex;
	unit.mTextureToBind = tex;
#if 0
	// version with mMustBeBinded - binding happend later
	if (unit.mCurrentBindedTexture != tex) {
		// set bit to true
		mMustBeBinded |= lookUpSet[textureUnit];
	}
	else {
		// set bit to false --> reset bit
		mMustBeBinded &= lookUpReset[textureUnit];
	}
#else
	// version without mMustBeBinded - binding happend immediately
	if (unit.mCurrentBindedTexture != tex) {
		bindTexture(tex, textureUnit);
		unit.mCurrentBindedTexture = tex;
		INCREASE_BIND_CALLS_COUNT
	}
	// set bit to false --> reset bit
	mMustBeBinded &= lookUpReset[textureUnit];
#endif
}

void gs::TextureUnitStack::popTexture(unsigned int textureUnit)
{
	TextureUnit& unit = mUnits[textureUnit];
	if (unit.mNextLayerIndex == 0) {
		LOGE("Out of texture stack! No texture on texture stack\n");
		return;
	}
	--unit.mNextLayerIndex;
	// now mNextLayerIndex has the "current layer index"
	// which is the "next layer index" for the next pushTexture()
	// but we need not the "current layer index" instead we need the
	// "previous layer index" and restore this texture
	int index = unit.mNextLayerIndex - 1;
	if (index >= MAX_LAYERS_PER_UNIT) {
		LOGE("Out of texture stack for pop! %d >= %d\n", index, MAX_LAYERS_PER_UNIT);
		return;
	}

	// if index is negative then no "previous layer" exist --> set 0 (no texture)
	Texture* tex = (index >= 0) ? unit.mLayers[index] : 0;
	unit.mTextureToBind = tex;
#if 1
	// version with mMustBeBinded - binding happend later
	if (unit.mCurrentBindedTexture != tex) {
		// set bit to true
		mMustBeBinded |= lookUpSet[textureUnit];
	}
	else {
		// set bit to false --> reset bit
		mMustBeBinded &= lookUpReset[textureUnit];
	}
#else
	// version without mMustBeBinded - binding happend immediately
	if (unit.mCurrentBindedTexture != tex) {
		bindTexture(tex, textureUnit);
		unit.mCurrentBindedTexture = tex;
		INCREASE_BIND_CALLS_COUNT
	}
	// set bit to false --> reset bit
	mMustBeBinded &= lookUpReset[textureUnit];
#endif
}

void gs::TextureUnitStack::bindForRendering()
{
	if (!mMustBeBinded) {
		return;
	}

	unsigned int mustBeBinded = mMustBeBinded;
	mMustBeBinded = 0;

	if (mustBeBinded & 0x01) {
		TextureUnit& unit = mUnits[0];
		bindTexture(unit.mTextureToBind, 0);
		unit.mCurrentBindedTexture = unit.mTextureToBind;
		INCREASE_BIND_CALLS_COUNT
		if (mustBeBinded == 1) {
			return;
		}
	}

	for (int i = 1; i < MAX_UNITS; ++i) {
		if (mustBeBinded & lookUpSet[i]) {
			TextureUnit& unit = mUnits[i];
			bindTexture(unit.mTextureToBind, i);
			unit.mCurrentBindedTexture = unit.mTextureToBind;
			INCREASE_BIND_CALLS_COUNT
			if (!(mustBeBinded & lookUpInvertLimit[i])) {
				return;
			}
		}
	}
}

void gs::TextureUnitStack::switchToTextureUnitForLoading()
{
	SWITCH_TO_TEXTURE_UNIT(MAX_UNITS)
}

void gs::TextureUnitStack::resetStatistic()
{
#ifdef ENABLE_TEXTURE_UNIT_STACK_STATISTIC
	for (int i = 0; i < MAX_UNITS; ++i) {
		mUnits[i].mStat.reset();
	}
	mStat.reset();
#endif
}

void gs::TextureUnitStack::printStatistic()
{
#ifdef ENABLE_TEXTURE_UNIT_STACK_STATISTIC
	std::stringstream s;
	s << mStat.mBindCalls << ": ";
	for (int i = 0; i < MAX_UNITS; ++i) {
		s << " " << mUnits[i].mStat.mBindCalls;
	}
	LOGI("%s\n", s.str().c_str());
#endif
}

void gs::TextureUnitStack::bindTexture(Texture* tex, unsigned int textureUnit)
{
	SWITCH_TO_TEXTURE_UNIT(textureUnit)

	if (tex) {
		// texture used
		GLuint texId = tex->getGlTexId();
		glBindTexture(GL_TEXTURE_2D, texId);
		if (texId) {
			glEnable(GL_TEXTURE_2D);
			mUsedTexUnitsFlags |= lookUpSet[textureUnit];
		}
		else {
			glDisable(GL_TEXTURE_2D);
			mUsedTexUnitsFlags &= lookUpReset[textureUnit];
		}
	}
	else {
		glDisable(GL_TEXTURE_2D);
		mUsedTexUnitsFlags &= lookUpReset[textureUnit];
	}
}

