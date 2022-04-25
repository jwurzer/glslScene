#ifndef GLSLSCENE_TEXTURE_H
#define GLSLSCENE_TEXTURE_H

#include <gs/res/resource.h>
#include <gs/rendering/gl_api.h>
#include <gs/common/rect.h>
#include <gs/common/color.h>
#include <string>

namespace gs
{
	enum class TexMipmap
	{
		NO_MIPMAP = 0,
		MIPMAP,
	};

	enum class TexFilter
	{
		NEAREST = 0,
		LINEAR,
	};

	enum class TexWrap
	{
		DEFAULT = 0, // use the default behaviour of opengl, default of GL should be GL_REPEAT
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
	};

	class ColorU32;

	class Texture: public Resource
	{
	public:
		Texture(const std::weak_ptr<FileChangeMonitoring>& fcm,
				const std::string& filename,
				TexMipmap mipmap, TexFilter minFilter, TexFilter magFilter,
				TexWrap wrap = TexWrap::DEFAULT,
				const Color& clampBorderColor = Color::black());
		virtual ~Texture();

		virtual ResType getType() const override;

		virtual bool load() override;

		virtual void unload() override;

		void resetUpdateInfo() { mUpdateArea = RectInt(0, 0, 0, 0); }

		void create(unsigned int width, unsigned int height, const ColorU32& color);
		void update(const RectInt& updateArea, const ColorU32& color);
		const std::string& getFilename() const { return mFilename; }
		TexMipmap getMipmap() const;
		TexFilter getMinFilter() const;
		TexFilter getMagFilter() const;
		TexWrap getTexWrap() const;
		unsigned int getWidth() const;
		unsigned int getHeight() const;
		unsigned int getBytePerPixel() const;
		const unsigned char* getData() const;
		RectInt getData(unsigned char* dst, const RectInt& area) const;
		const RectInt& getUpdateArea() const { return mUpdateArea; }

		GLuint getGlTexId() const { return mGlTexId; }
	private:
		std::string mFilename;
		TexMipmap mMipmap;
		TexFilter mMinFilter;
		TexFilter mMagFilter;
		TexWrap mWrap;
		Color mClampBorderColor;

		unsigned int mWidth;
		unsigned int mHeight;
		unsigned int mBytePerPixel; // should be 4 (4 for RGBA)
		RectInt mUpdateArea;
		/**
		 * mData[0] start with the upper left pixel of the image
		 */
		unsigned char* mData; // has a size of mWidth * mHeight * mBytePerPixel

		GLuint mGlTexId;

		bool loadFromFile();
	};
}

#endif //GLSLSCENE_TEXTURE_H
