#ifndef GLSLSCENE_FRAMEBUFFER_H
#define GLSLSCENE_FRAMEBUFFER_H

#include <res/resource.h>
#include <rendering/gl_api.h>

namespace gs
{
	class Texture;

	class Framebuffer: public Resource
	{
	public:
		Framebuffer(int width, int height);
		virtual ~Framebuffer();

		virtual ResType getType() const { return ResType::FRAMEBUFFER; }
		virtual bool load() { return true; }
		virtual void unload() {}

		const std::shared_ptr<Texture>& getTexRes() const { return mTexRes; };

		void bind();
		unsigned int getWidth() const { return mWidth; }
		unsigned int getHeight() const { return mHeight; }
	private:
		bool mUseWindowWidth;
		bool mUseWindowHeight;
		unsigned int mWidth;
		unsigned int mHeight;

		GLuint mFbo; // frame buffer object
		std::shared_ptr<Texture> mTexRes; // texture for rendering
		//GLuint mTex; // texture for rendering
		GLuint mRbo; // render buffer object

		bool createFramebuffer();
		void deleteFramebuffer();
	};
}

#endif //GLSLSCENE_FRAMEBUFFER_H
