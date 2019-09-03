#include <gs/res/framebuffer.h>
#include <gs/res/texture.h>
#include <gs/common/color.h>
#include <gs/system/log.h>

gs::Framebuffer::Framebuffer(int width, int height)
		:Resource(std::weak_ptr<FileChangeMonitoring>()),
		mUseWindowWidth(true),
		mUseWindowHeight(true),
		mWidth(1),
		mHeight(1),
		mFbo(0),
		//mTex(0),
		mTexRes(),
		mRbo(0)
{
	if (width >= 0) {
		mUseWindowWidth = false;
		mWidth = width;
	}
	if (height >= 0) {
		mUseWindowHeight = false;
		mHeight = height;
	}
	createFramebuffer();
}

gs::Framebuffer::~Framebuffer()
{
	deleteFramebuffer();
}

void gs::Framebuffer::bind(int windowWidth, int windowHeight)
{
	bool recreate = false;
	if (mUseWindowWidth && windowWidth != mWidth) {
		mWidth = windowWidth;
		recreate = true;
	}
	if (mUseWindowHeight && windowHeight != mHeight) {
		mHeight = windowHeight;
		recreate = true;
	}
	if (recreate) {
		recreateFramebuffer();
	}

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
	glViewport(0, 0, mWidth, mHeight); // Render on the whole framebuffer, complete from the lower left corner to the upper right
}

bool gs::Framebuffer::createFramebuffer()
{
	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	glGenFramebuffers(1, &mFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

#if 0
	// The texture we're going to render to
	glGenTextures(1, &mTex);
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, mTex);
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
	mTexRes = std::make_shared<Texture>(std::weak_ptr<FileChangeMonitoring>(), "",
			TexMipmap::NO_MIPMAP, TexFilter::NEAREST, TexFilter::NEAREST);
	mTexRes->create(mWidth, mHeight, ColorU32::white());
	mTexRes->load();
#endif

#if 1
	// The depth buffer
	glGenRenderbuffers(1, &mRbo);
	glBindRenderbuffer(GL_RENDERBUFFER, mRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRbo);
#endif

	// Set "renderedTexture" as our colour attachement #0
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTex, 0);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexRes->getGlTexId(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTexRes->getGlTexId(), 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		LOGE("Wrong framebuffer status\n");
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

bool gs::Framebuffer::recreateFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

	mTexRes->create(mWidth, mHeight, ColorU32::white());
	mTexRes->load();


	glBindRenderbuffer(GL_RENDERBUFFER, mRbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mWidth, mHeight);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mRbo);

	// Set "renderedTexture" as our colour attachement #0
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTex, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexRes->getGlTexId(), 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		LOGE("Wrong framebuffer status\n");
		return false;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void gs::Framebuffer::deleteFramebuffer()
{
	if (mFbo) {
		glDeleteFramebuffers(1, &mFbo);
	}
#if 0
	if (mTex) {
		glDeleteTextures(1, &mTex);
	}
#else
	mTexRes.reset();
#endif
	if (mRbo) {
		glDeleteRenderbuffers(1, &mRbo);
	}
}
