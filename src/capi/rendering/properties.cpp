#include <capi/rendering/properties.h>

#include <gs/rendering/properties.h>

int propertiesGetTsMsec(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return static_cast<int>(properties->mTsMsec);
}

float propertiesGetTsSec(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mTsSec;
}

float propertiesGetDeltaTimeSec(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mDeltaTimeSec;
}

int propertiesGetWindowWidthI(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mWindowSizeI.mWidth;
}

int propertiesGetWindowHeightI(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mWindowSizeI.mHeight;
}

float propertiesGetWindowWidth(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mWindowSize.mWidth;
}

float propertiesGetWindowHeight(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mWindowSize.mHeight;
}

float propertiesGetMousePosFactorX(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mMousePosFactor.x;
}

float propertiesGetMousePosFactorY(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mMousePosFactor.y;
}

float propertiesGetMousePosPixelX(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mMousePosPixel.x;
}

float propertiesGetMousePosPixelY(Properties* p)
{
	gs::Properties* properties = static_cast<gs::Properties*>(p);
	return properties->mMousePosPixel.y;
}
