
#include <common/color.h>

#if defined(_MSC_VER)
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

#define CONVERT_TO_FLOAT(c) ((c) / 255.0f)

#define R(c) CONVERT_TO_FLOAT(c)
#define G(c) CONVERT_TO_FLOAT(c)
#define B(c) CONVERT_TO_FLOAT(c)
#define A(c) CONVERT_TO_FLOAT(c)

namespace gs
{
	namespace {
		Color colBlack(R(0), G(0), B(0));
		Color colWhite(R(255), G(255), B(255));
		Color colNone(0.0f, 0.0f, 0.0f, 0.0f);
		Color colRed(R(255), G(0), B(0));
		Color colGreen(R(0), G(128), B(0));
		Color colBlue(R(0), G(0), B(255));

		ColorU32 col32Black(0, 0, 0);
		ColorU32 col32White(255, 255, 255);
		ColorU32 col32None(0, 0, 0, 0);
		ColorU32 col32Red(255, 0, 0);
		ColorU32 col32Green(0, 128, 0);
		ColorU32 col32Blue(0, 0, 255);
	}
}

gs::ColorU32::ColorU32(const Color& c)
		// + 0.5f for correct rounding
		:r(static_cast<uint8_t>(c.r * 255.0f + 0.5f)), g(static_cast<uint8_t>(c.g * 255.0f + 0.5f)),
		b(static_cast<uint8_t>(c.b * 255.0f + 0.5f)), a(static_cast<uint8_t>(c.a * 255.0f + 0.5f))
{
}

uint32_t gs::ColorU32::getArgb() const
{
	union {
		uint32_t colU32;
		unsigned char col[4];
	};
	col[1] = r; // red
	col[2] = g; // green
	col[3] = b; // blue
	col[0] = a; // alpha
	return htonl(colU32);
}

uint32_t gs::ColorU32::getRgb() const
{
	return getArgb() | 0xFF000000;
}

gs::ColorU32 gs::ColorU32::rgba(unsigned char* rgba)
{
	return ColorU32(rgba[0], rgba[1], rgba[2], rgba[3]);
}

gs::ColorU32 gs::ColorU32::rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return ColorU32(r, g, b, a);
}

gs::ColorU32 gs::ColorU32::rgb(unsigned char r, unsigned char g, unsigned char b)
{
	return ColorU32(r, g, b);
}

gs::ColorU32 gs::ColorU32::argb(uint32_t argb)
{
	union {
		uint32_t colU32;
		unsigned char col[4];
	};
	colU32 = htonl(argb);
	return ColorU32(col[1], col[2], col[3], col[0]);
}

// alpha value is set to FF. --> no diff between 0xFF112233 and 0x00112233
gs::ColorU32 gs::ColorU32::rgb(uint32_t rgb)
{
	return argb(rgb | 0xFF000000);
}

const gs::ColorU32& gs::ColorU32::black() { return col32Black; }
const gs::ColorU32& gs::ColorU32::white() { return col32White; }
const gs::ColorU32& gs::ColorU32::none() { return col32None; }
const gs::ColorU32& gs::ColorU32::red() { return col32Red; }
const gs::ColorU32& gs::ColorU32::green() { return col32Green; }
const gs::ColorU32& gs::ColorU32::blue() { return col32Blue; }

gs::Color gs::Color::argb(uint32_t argb)
{
	return Color(ColorU32::argb(argb));
}

const gs::Color& gs::Color::black() { return colBlack; }
const gs::Color& gs::Color::white() { return colWhite; }
const gs::Color& gs::Color::none() { return colNone; }
const gs::Color& gs::Color::red() { return colRed; }
const gs::Color& gs::Color::green() { return colGreen; }
const gs::Color& gs::Color::blue() { return colBlue; }
