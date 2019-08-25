#ifndef GLSLSCENE_CAPI_PROPERTIES_H
#define GLSLSCENE_CAPI_PROPERTIES_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void Properties;

int propertiesGetTsMsec(Properties* p);
float propertiesGetTsSec(Properties* p);
float propertiesGetDeltaTimeSec(Properties* p);

int propertiesGetWindowWidthI(Properties* p);
int propertiesGetWindowHeightI(Properties* p);

float propertiesGetWindowWidth(Properties* p);
float propertiesGetWindowHeight(Properties* p);

float propertiesGetMousePosFactorX(Properties* p);
float propertiesGetMousePosFactorY(Properties* p);

float propertiesGetMousePosPixelX(Properties* p);
float propertiesGetMousePosPixelY(Properties* p);

#ifdef __cplusplus
}
#endif

#endif //GLSLSCENE_CAPI_PROPERTIES_H
