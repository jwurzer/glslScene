#include <gs/rendering/properties.h>
#include <sstream>

std::string gs::Properties::toString() const
{
	std::stringstream s;
	s << "ts: " << mTsSec << "s (" << mTsMsec << "ms), dt: " << mDeltaTimeSec <<
			", win: " << mWindowSize.mWidth << "x" << mWindowSize.mHeight <<
			", mouse " << mMousePosPixel.x << "/" << mMousePosPixel.y <<
			" (" << mMousePosFactor.x << "/" << mMousePosFactor.y <<
			"), viewport: " << mViewportPosPixel.x <<
			"," << mViewportPosPixel.y <<
			"," << mViewportSizePixel.mWidth <<
			"," << mViewportSizePixel.mHeight <<
			", view: " << mViewSize.x << "x" << mViewSize.y << "x" << mViewSize.z <<
			" (" << mViewRatio.mWidth << ":" << mViewRatio.mHeight << ")";
	return s.str();
}

