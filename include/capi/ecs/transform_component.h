#ifndef GLSLSCENE_CAPI_TRANSFORM_COMPONENT_H
#define GLSLSCENE_CAPI_TRANSFORM_COMPONENT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void TransformComponent;

void transformReset(TransformComponent* t);
void transformTranslate2f(TransformComponent* t, float x, float y);
void transformTranslate3f(TransformComponent* t, float x, float y, float z);
void transformRotateDegree(TransformComponent* t, float angle);
void transformRotateDegreeCenter(TransformComponent* t, float angle, float centerX, float centerY);
void transformRotateDegreeAxis(TransformComponent* t, float angle, float rotAxisX, float rotAxisY, float rotAxisZ);
void transformRotateRadian(TransformComponent* t, float angle);
void transformRotateRadianCenter(TransformComponent* t, float angle, float centerX, float centerY);
void transformRotateRadianAxis(TransformComponent* t, float angle, float rotAxisX, float rotAxisY, float rotAxisZ);
void transformScale2f(TransformComponent* t, float scaleX, float scaleY);
void transformScaleCenter2f(TransformComponent* t, float scaleX, float scaleY, float centerX, float centerY);
void transformScale3f(TransformComponent* t, float scaleX, float scaleY, float scaleZ);
void transformScaleCenter3f(TransformComponent* t, float scaleX, float scaleY, float scaleZ, float centerX, float centerY, float centerZ);

#ifdef __cplusplus
}
#endif

#endif //GLSLSCENE_CAPI_TRANSFORM_COMPONENT_H
