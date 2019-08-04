#include <ecs/texture_component.h>
#include <res/resource_id_limit.h>
#include <common/lookup8.h>
#include <string.h>


gs::TextureComponent::TextureComponent(std::weak_ptr<Entity> entity)
		:Component(entity), mSharedCustomData(), mIsUsedFlags(0), mTextureIds{}
{
	reset();
}

gs::TextureComponent::~TextureComponent()
{
}

void gs::TextureComponent::reset()
{
	setChanged();
	mIsUsedFlags = 0;
	for (int i = 0; i < MAX_TEXTURE_UNITS; ++i) {
		mTextureIds[i] = TEXTURE_ID_INVALID;
	}
}

void gs::TextureComponent::setTextureId(TTextureId textureId,
		unsigned int textureUnit)
{
	if (textureUnit >= MAX_TEXTURE_UNITS) {
		return;
	}
	setChanged();
	if (textureId == TEXTURE_ID_INVALID) {
		mIsUsedFlags &= lookUpReset[textureUnit];
	}
	else {
		mIsUsedFlags |= lookUpSet[textureUnit];
	}
	mTextureIds[textureUnit] = textureId;
}

