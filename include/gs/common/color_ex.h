#ifndef GLSLSCENE_COLOR_EX_H
#define GLSLSCENE_COLOR_EX_H

#include <gs/common/color.h>
#include <stdint.h>
#include <memory>

#include <gs/common/rect_fwd.h>

namespace gs
{
	class Value;

	class ColorEx
	{
	public:
		ColorEx();
		ColorEx(const Color& color);
		ColorEx(const Value& r, const Value& g, const Value& b, int64_t refTs);
		ColorEx(const Value& r, const Value& g, const Value& b,
				const Value& a, int64_t refTs);
		virtual ~ColorEx();

		void setColorNone();
		void setColor(const Color& color);
		void setColor(const Value& r, const Value& g, const Value& b,
				int64_t refTs);
		void setColor(const Value& r, const Value& g, const Value& b,
				const Value& a, int64_t refTs);

		/**
		 * Add timeOffset to the current value of mRefTs.
		 * If no "extended color" is used (mRed, mGreen ... are not null) then
		 * it is also added but has no effect.
         */
		void shiftRefTs(int64_t timeOffset) { mRefTs += timeOffset; }
		void changeRefTs(int64_t absTimeOffset) { mRefTs = absTimeOffset; }

		bool isUsed() const { return mColorIsUsed; }
		const Color& getColor(int64_t ts, const RectFloat& scrSize) const;
	private:
		bool mColorIsUsed;
		mutable Color mColor;
		std::shared_ptr<const Value> mRed;
		std::shared_ptr<const Value> mGreen;
		std::shared_ptr<const Value> mBlue;
		std::shared_ptr<const Value> mAlpha;
		int64_t mRefTs;
	};
}

#endif
