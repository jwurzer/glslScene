#ifndef GLSLSCENE_COLOR_H
#define GLSLSCENE_COLOR_H

#include <stdint.h>

namespace gs
{
	class Color;

	class ColorU32
	{
	public:
		union {
			uint8_t mRgba[4];
			struct {
				uint8_t r, g, b, a;
			};
			uint32_t mCol32;
		};

		ColorU32() :r(0), g(0), b(0), a(0) {}
		ColorU32(uint8_t r, uint8_t g, uint8_t b) :r(r), g(g), b(b), a(255) {}
		ColorU32(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :r(r), g(g), b(b), a(a) {}
		explicit ColorU32(const Color& c);
		uint32_t getArgb() const;
		uint32_t getRgb() const;

		static ColorU32 rgba(uint8_t* rgba);
		static ColorU32 rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		static ColorU32 rgb(uint8_t r, uint8_t g, uint8_t b);
		static ColorU32 argb(uint32_t argb);
		// alpha value is set to FF. --> no diff between 0xFF112233 and 0x00112233
		static ColorU32 rgb(uint32_t rgb);
		// black ... rgb(0, 0, 0) with alpha 255
		static const ColorU32& black();
		// white ... rgb(255, 255, 255)
		static const ColorU32& white();
		// none ... rgba(0, 0, 0, 0) same as black with alpha 0
		static const ColorU32& none();
		// red ... rgb(255, 0, 0)
		static const ColorU32& red();
		// green ... rgb(0, 128, 0)
		static const ColorU32& green();
		// blue ... rgb(0, 0, 255)
		static const ColorU32& blue();
	};

	class Color
	{
	public:
		union {
			float rgba[4];
			struct {
				float r, g, b, a;
			};
		};

		Color() :r(0.0f), g(0.0f), b(0.0f), a(0.0f) {}
		Color(float r, float g, float b) :r(r), g(g), b(b), a(1.0f) {}
		Color(float r, float g, float b, float a) :r(r), g(g), b(b), a(a) {}
		Color(const float* rgba) :r(rgba[0]), g(rgba[1]), b(rgba[2]), a(rgba[3]) {}
		explicit Color(const ColorU32& col) :r(((float)col.r) / 255.0f),
				g(((float)col.g / 255.0f)), b(((float)col.b / 255.0f)),
				a(((float)col.a / 255.0f)) {}
		void set(float r, float g, float b) { this->r = r; this->g = g; this->b = b; this->a = 1.0f; }
		void set(float r, float g, float b, float a) { this->r = r; this->g = g; this->b = b; this->a = a; }

		static Color argb(uint32_t argb);
		// black ... rgb(0, 0, 0) with alpha 255
		static const Color& black();
		// white ... rgb(255, 255, 255)
		static const Color& white();
		// none ... rgba(0, 0, 0, 0) same as black with alpha 0
		static const Color& none();
		// red ... rgb(255, 0, 0)
		static const Color& red();
		// green ... rgb(0, 128, 0)
		static const Color& green();
		// blue ... rgb(0, 0, 255)
		static const Color& blue();
	};
}

#endif
