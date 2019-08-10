#include <gs/configloader/uniform_attr_loader.h>

#include <gs/system/log.h>
#include <gs/common/cfg.h>
#include <gs/res/uniform.h>

bool gs::uniformattrloader::loadUniformsAndAttributes(
		std::vector<Uniform> &outUniforms,
		std::vector<Attribute> &outAttributes,
		const CfgValue &cfgValue, unsigned int startIndex,
		const std::string &nameForErrorMsg)
{
	size_t arraySize = cfgValue.mArray.size();
	if (startIndex >= arraySize) {
		return true;
	}

	size_t uniformCount = 0;
	size_t attrCount = 0;
	for (size_t i = startIndex; i < arraySize; ++i) {
		const CfgValuePair& vp = cfgValue.mArray[i];
		if (vp.mName.mText.empty()) {
			continue;
		}
		if (vp.mName.mText[0] == 'u') {
			++uniformCount;
		}
		else if (vp.mName.mText[0] == 'a') {
			++attrCount;
		}
	}

	outUniforms.reserve(outUniforms.size() + uniformCount);
	outAttributes.reserve(outAttributes.size() + attrCount);

	LOGI("uniforms begin %u, (maybe max) end %zu\n", startIndex, arraySize);
	for (size_t i = startIndex; i < arraySize; ++i) {
		const CfgValuePair& vp = cfgValue.mArray[i];
		if (vp.mName.isArray()) {
			if (vp.mName.mArray.size() == 3) {
				const std::vector<CfgValuePair>& arr = vp.mName.mArray;
				if (arr[0].mValue.mText != "uniform") {
					LOGE("%s: Must start with 'uniform'\n", nameForErrorMsg.c_str());
					return false;
				}
				Uniform uniform;
				uniform.mName = arr[2].mValue.mText;
				const std::string& uniformType = arr[1].mValue.mText;
				if (uniformType == "int") {
					uniform.mType = UniformType::INT;
				}
				else if (uniformType == "float") {
					uniform.mType = UniformType::FLOAT;
				}
				else if (uniformType == "vec2") {
					uniform.mType = UniformType::VEC2;
				}
				else if (uniformType == "vec3") {
					uniform.mType = UniformType::VEC3;
				}
				else if (uniformType == "vec4") {
					uniform.mType = UniformType::VEC4;
				}
				else if (uniformType == "sampler2d") {
					uniform.mType = UniformType::SAMPLER2D;
				}
				else if (uniformType == "mat4" ||
						uniformType == "mat4x4") {
					uniform.mType = UniformType::MAT4X4;
				}
				else {
					LOGE("Type %s is not supported.\n", uniformType.c_str());
					return false;
				}
				if (vp.mValue.mType == CfgValue::TYPE_TEXT) {
					if (vp.mValue.mText == "time") {
						uniform.mSource = UniformSource::ABSOLUTE_TIME_SEC;
					}
					else if (vp.mValue.mText == "delta-time") {
						uniform.mSource = UniformSource::DELTA_TIME_SEC;
					}
					else if (vp.mValue.mText == "mouse-pos-factor") {
						uniform.mSource = UniformSource::MOUSE_POS_FACTOR;
					}
					else if (vp.mValue.mText == "mouse-pos-pixel") {
						uniform.mSource = UniformSource::MOUSE_POS_PIXEL;
					}
					else if (vp.mValue.mText == "viewport-pos-pixel") {
						uniform.mSource = UniformSource::VIEWPORT_POS_PIXEL;
					}
					else if (vp.mValue.mText == "viewport-size-pixel") {
						uniform.mSource = UniformSource::VIEWPORT_SIZE_PIXEL;
					}
					else if (vp.mValue.mText == "view-size") {
						uniform.mSource = UniformSource::VIEW_SIZE;
					}
					else if (vp.mValue.mText == "view-ratio") {
						uniform.mSource = UniformSource::VIEW_RATIO;
					}
					else if (vp.mValue.mText == "projection-matrix") {
						uniform.mSource = UniformSource::PROJECTION_MATRIX;
					}
					else if (vp.mValue.mText == "view-matrix") {
						uniform.mSource = UniformSource::VIEW_MATRIX;
					}
					else if (vp.mValue.mText == "inverse-view-matrix") {
						uniform.mSource = UniformSource::INVERSE_VIEW_MATRIX;
					}
					else if (vp.mValue.mText == "model-matrix") {
						uniform.mSource = UniformSource::MODEL_MATRIX;
					}
					else if (vp.mValue.mText == "model-view-matrix") {
						uniform.mSource = UniformSource::MODEL_VIEW_MATRIX;
					}
					else if (vp.mValue.mText == "entity-matrix") {
						uniform.mSource = UniformSource::ENTITY_MATRIX;
					}
					else if (vp.mValue.mText == "mvp-matrix") {
						uniform.mSource = UniformSource::MVP_MATRIX;
					}
					else {
						LOGE("Doesn't support value '%s'\n", vp.mValue.mText.c_str());
						return false;
					}
				}
				else if (vp.mValue.isArray()) {
					switch (vp.mValue.mArray.size()) {
						case 2:
							if (vp.mValue.mArray[0].mValue.mType == CfgValue::TYPE_TEXT) {
								if (vp.mValue.mArray[0].mValue.mText == "relative-time") {
									uniform.mSource = UniformSource::RELATIVE_TIME_SEC;
									uniform.mValue.mFloat = vp.mValue.mArray[1].mValue.mFloatingPoint;
								}
								else {
									LOGE("Doesn't support value '%s'\n", vp.mValue.mArray[0].mValue.mText.c_str());
									return false;
								}
								break;
							}
							if (uniform.mType != UniformType::VEC2) {
								LOGE("Must be the type vec2 for two values.\n");
								return false;
							}
							uniform.mSource = UniformSource::CUSTOM_VALUE;
							uniform.mValue.mVec2.x = vp.mValue.mArray[0].mValue.mFloatingPoint;
							uniform.mValue.mVec2.y = vp.mValue.mArray[1].mValue.mFloatingPoint;
							break;
						case 3:
							if (uniform.mType != UniformType::VEC3) {
								LOGE("Must be the type vec3 for three values.\n");
								return false;
							}
							uniform.mSource = UniformSource::CUSTOM_VALUE;
							uniform.mValue.mVec3.x = vp.mValue.mArray[0].mValue.mFloatingPoint;
							uniform.mValue.mVec3.y = vp.mValue.mArray[1].mValue.mFloatingPoint;
							uniform.mValue.mVec3.z = vp.mValue.mArray[2].mValue.mFloatingPoint;
							break;
						case 4:
							if (uniform.mType != UniformType::VEC4) {
								LOGE("Must be the type vec4 for four values.\n");
								return false;
							}
							uniform.mSource = UniformSource::CUSTOM_VALUE;
							uniform.mValue.mVec4.x = vp.mValue.mArray[0].mValue.mFloatingPoint;
							uniform.mValue.mVec4.y = vp.mValue.mArray[1].mValue.mFloatingPoint;
							uniform.mValue.mVec4.z = vp.mValue.mArray[2].mValue.mFloatingPoint;
							uniform.mValue.mVec4.w = vp.mValue.mArray[3].mValue.mFloatingPoint;
							break;
						case 16:
							if (uniform.mType != UniformType::MAT4X4) {
								LOGE("Must be the type mat4 for four values.\n");
								return false;
							}
							uniform.mSource = UniformSource::CUSTOM_VALUE;
							for (int i = 0; i < 16; ++i) {
								uniform.mValue.mMat4.m[i] = vp.mValue.mArray[i].mValue.mFloatingPoint;
							}
							break;
						default:
							LOGE("%zu values not supported.\n", vp.mValue.mArray.size());
							break;
					}
				}
				else {
					// --> value is no text and no array --> should be a single number
					if (!vp.mValue.isNumber()) {
						LOGE("value is not a number\n");
						return false;
					}
					uniform.mSource = UniformSource::CUSTOM_VALUE;
					switch (uniform.mType) {
						case UniformType::INT:
						case UniformType::SAMPLER2D:
							uniform.mValue.mInt = vp.mValue.mInteger;
							break;
						case UniformType::FLOAT:
							uniform.mValue.mFloat = vp.mValue.mFloatingPoint;
							break;
						default:
							LOGE("Uniform type is not supported for this value.\n");
							return false;
					}
				}
				outUniforms.push_back(uniform);
			}
			else if (vp.mName.mArray.size() == 2) {
				const std::vector<CfgValuePair>& arr = vp.mName.mArray;
				if (arr[0].mValue.mText != "attribute") {
					LOGE("%s: Must start with 'attribute'\n", nameForErrorMsg.c_str());
					return false;
				}
				if (vp.mValue.mArray.size() != 2) {
					LOGE("%s: Attribute must have two values. <comp-offset> and <comp-count>\n", nameForErrorMsg.c_str());
					return false;
				}
				Attribute attribute(arr[1].mValue.mText,
						vp.mValue.mArray[0].mValue.mInteger,
						vp.mValue.mArray[1].mValue.mInteger);
				outAttributes.push_back(attribute);
			}
			else {
				LOGE("%s: wrong uniform format\n", nameForErrorMsg.c_str());
				return false;
			}
		}
		else {
			LOGE("%s: %s is a not supported uniform\n", nameForErrorMsg.c_str(), vp.mName.mText.c_str());
			return false;
		}
	}
	return true;
}

