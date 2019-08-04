#ifndef GLSLSCENE_TEXTURE_COMPONENT_H
#define GLSLSCENE_TEXTURE_COMPONENT_H

#include <ecs/component.h>
#include <res/resource_id.h>

namespace gs
{
	class TextureComponent: public Component
	{
	public:
		enum EDefs
		{
			MAX_TEXTURE_UNITS = 8,
		};
		/**
		 * This pointer can be used for anything.
		 * The pointer will be NOT copied at the clone function.
		 * The clone() function leave this pointer at the source unchanged and
		 * set this pointer to NULL at the destination!!!!!
		 *
		 * This pointer is used by the rendering engine for quick access of resources.
		 */
		mutable std::shared_ptr<void> mSharedCustomData;

		TextureComponent(std::weak_ptr<Entity> entity);
		virtual ~TextureComponent();

		virtual EComponent getType() const { return COMPONENT_TEXTURE; }

		/**
		 * Reset all to default beside of mCustomData. mCustomData is unchanged!
		 */
		void reset();

		// reset all other textures (if multitexturing is used) and only
		// set one texture.
		void setTextureId(TTextureId textureId, unsigned int textureUnit);
		unsigned int getUsedFlags() const { return mIsUsedFlags; }
		const TTextureId* getTextureIds() const { return mTextureIds; }
	private:
		unsigned int mIsUsedFlags;
		TTextureId mTextureIds[MAX_TEXTURE_UNITS];
		//vertex::ETexCoordinateType mTexCoordType = vertex::TEX_NORMALIZED;
	};
}

#endif /* GLSLSCENE_TEXTURE_COMPONENT_H */
