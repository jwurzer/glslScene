#include <gs/res/texture.h>
#include <gs/common/rect.h>
#include <gs/common/color.h>
#include <gs/system/log.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <string.h>

namespace gs
{
	namespace
	{
		GLint getOpenGLWrapping(TexWrap wrap)
		{
			switch (wrap) {
				case TexWrap::DEFAULT:
					return GL_REPEAT;
				case TexWrap::REPEAT:
					return GL_REPEAT;
				case TexWrap::MIRRORED_REPEAT:
					return GL_MIRRORED_REPEAT;
				case TexWrap::CLAMP_TO_EDGE:
					return GL_CLAMP_TO_EDGE;
				case TexWrap::CLAMP_TO_BORDER:
					return GL_CLAMP_TO_BORDER;
			}
			return GL_REPEAT;
		}
	}
}

gs::Texture::Texture(const std::weak_ptr<FileChangeMonitoring>& fcm,
		const std::string& filename, TexMipmap mipmap, TexFilter minFilter,
		TexFilter magFilter, TexWrap wrap, const Color& clampBorderColor)
		:Resource(fcm), mFilename(filename),
		mMipmap(mipmap), mMinFilter(minFilter), mMagFilter(magFilter),
		mWrap(wrap), mClampBorderColor(clampBorderColor),
		mWidth(0), mHeight(0),
		mBytePerPixel(0), mUpdateArea(0, 0, 0, 0), mData(nullptr),
		mGlTexId(0)
{
}

gs::Texture::~Texture()
{
	unload();
}

gs::ResType gs::Texture::getType() const
{
	return ResType::TEXTURE;
}

#if 0
bool gs::Texture::exist() const
{
	if (mWidth && mHeight && mBytePerPixel && mData) {
		return true;
	}
	return false;
}
#endif

bool gs::Texture::load()
{
	if (!mFilename.empty()) {
		unload();

		if (!loadFromFile()) {
			return false;
		}
	}
	else {
		if (!mData || !mWidth || !mHeight) {
			LOGE("No buffer exist!\n");
			return false;
		}
	}

	glGenTextures(1, &mGlTexId);
	LOGI("opengl: tex id %u\n", mGlTexId);
	glBindTexture(GL_TEXTURE_2D, mGlTexId);
	switch (mMipmap)
	{
		case TexMipmap::NO_MIPMAP:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
					0, GL_RGBA, GL_UNSIGNED_BYTE, mData);
			break;
		case TexMipmap::MIPMAP:
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, mWidth, mHeight,
					GL_RGBA, GL_UNSIGNED_BYTE, mData);
#ifdef COLORING_TEX_FOR_DEBUGGING
			coloringTextureForDebugging(*this);
#endif
			break;
	}

	switch (mMinFilter)
	{
		case TexFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					(mMipmap == TexMipmap::MIPMAP) ?
							GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
			break;
		case TexFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					(mMipmap == TexMipmap::MIPMAP) ?
							GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
			break;
	}

	switch (mMagFilter)
	{
		case TexFilter::NEAREST:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
					GL_NEAREST);
			break;
		case TexFilter::LINEAR:
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
					GL_LINEAR);
			break;
	}

	if (mWrap != TexWrap::DEFAULT) {
		GLint glWrap = getOpenGLWrapping(mWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, glWrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, glWrap);
		if (mWrap == TexWrap::CLAMP_TO_BORDER) {
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, mClampBorderColor.rgba);
		}
	}

	glBindTexture(GL_TEXTURE_2D, 0); // unbind
	if (getHotReloadingFileCount() == 0) {
		addFileForHotReloading(mFilename);
	}
	return true;
}

void gs::Texture::unload()
{
	glDeleteTextures(1, &mGlTexId);
	mWidth = 0;
	mHeight = 0;
	mBytePerPixel = 0;
	delete[] mData;
	mUpdateArea = RectInt(0, 0, 0, 0);
	mData = NULL;
}

void gs::Texture::create(unsigned int width, unsigned int height, const ColorU32& color)
{
	unload();

	mWidth = width;
	mHeight = height;
	mBytePerPixel = sizeof(color.mCol32); // should be 4
	unsigned int lPixelCount(mWidth * mHeight);
	unsigned int lSize(lPixelCount * mBytePerPixel);
	mData = new unsigned char [lSize];
	union {
		unsigned char* data;
		uint32_t* p32;
	} ptr;
	ptr.data = mData;
	uint32_t c32 = color.mCol32;
	for (unsigned int i = 0; i < lPixelCount; i++) {
		ptr.p32[i] = c32;
	}
	mUpdateArea = RectInt(0, 0, mWidth, mHeight);
}

void gs::Texture::update(const RectInt& updateArea, const ColorU32& color)
{
	RectInt area = updateArea;
	area.regular();

	if (area.mLeft < 0) {
		area.mWidth += area.mLeft; // decrease width (left is negative)
		area.mLeft = 0;
	}
	if (area.mTop < 0) {
		area.mHeight += area.mTop; // decrease height (top is negative)
		area.mTop = 0;
	}
	if (area.mLeft >= int(mWidth) || area.mTop >= int(mHeight) ||
			area.mWidth <= 0 || area.mHeight <= 0) {
		return;
	}
	int xend = (area.mLeft + area.mWidth > int(mWidth)) ? mWidth : (area.mLeft + area.mWidth);
	int yend = (area.mTop + area.mHeight > int(mHeight)) ? mHeight : (area.mTop + area.mHeight);
	area.mWidth = xend - area.mLeft;
	area.mHeight = yend - area.mTop;

	union {
		unsigned char* data;
		uint32_t* p32;
	} ptr;
	ptr.data = mData;
	uint32_t c32 = color.mCol32;

	for (int yi = area.mTop; yi < yend; yi++) {
		uint32_t* line = &ptr.p32[yi * mWidth];
		for (int xi = area.mLeft; xi < xend; xi++) {
			line[xi] = c32;
		}
	}
	if (!mUpdateArea.mWidth || !mUpdateArea.mHeight) {
		// no update infos set --> use current area
		mUpdateArea = RectInt(area.mLeft, area.mTop, xend - area.mLeft, yend - area.mTop);
	}
	else {
		mUpdateArea = mUpdateArea.boundingBox(area);
	}
}

gs::TexMipmap gs::Texture::getMipmap() const
{
	return mMipmap;
}

gs::TexFilter gs::Texture::getMinFilter() const
{
	return mMinFilter;
}

gs::TexFilter gs::Texture::getMagFilter() const
{
	return mMagFilter;
}

gs::TexWrap gs::Texture::getTexWrap() const
{
	return mWrap;
}

unsigned int gs::Texture::getWidth() const
{
	return mWidth;
}

unsigned int gs::Texture::getHeight() const
{
	return mHeight;
}

unsigned int gs::Texture::getBytePerPixel() const
{
	return mBytePerPixel;
}

const unsigned char* gs::Texture::getData() const
{
	return mData;
}

gs::RectInt gs::Texture::getData(unsigned char* dst, const RectInt& area) const
{
	RectInt a;
	if (!area.intersects(RectInt(0, 0, mWidth, mHeight), a)) {
		return a;
	}
	int srcLineBytes = mWidth * mBytePerPixel;
	int dstLineBytes = a.mWidth * mBytePerPixel;
	unsigned char* src = mData + a.mTop * srcLineBytes + a.mLeft * mBytePerPixel;
	for (int yi = 0; yi < a.mHeight; yi++) {
		memcpy(dst, src, dstLineBytes);
		src += srcLineBytes;
		dst += dstLineBytes;
	}
	return a;
}

bool gs::Texture::loadFromFile()
{
	if (mFilename.size() < 4) {
		LOGE("to short filename\n");
		return false;
	}
	int x = 0, y = 0, channels_in_file = 0;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* pixelBuffer = stbi_load(mFilename.c_str(), &x, &y,
			&channels_in_file, STBI_rgb_alpha /* 4 */);
	if (!pixelBuffer || !x || !y) {
		LOGE("load image '%s' failed\n", mFilename.c_str());
		return false;
	}
	unsigned int bufSize = x * y * 4;
	mData = new unsigned char[bufSize];
	memcpy(mData, pixelBuffer, bufSize);
	stbi_image_free(pixelBuffer);

	mBytePerPixel = 4;
	mWidth = x;
	mHeight = y;
	mUpdateArea = RectInt(0, 0, mWidth, mHeight);
	return true;
}
