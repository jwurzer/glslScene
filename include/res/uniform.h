#ifndef GLSLSCENE_UNIFORM_H
#define GLSLSCENE_UNIFORM_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <stdint.h>

namespace gs
{
	enum class UniformType
	{
		INVALID = 0,
		INT,
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		SAMPLER2D, // using mInt to store the texture unit index (0 for first index, etc.)
		MAT4X4,
	};

	enum class UniformSource
	{
		// no value used
		INVALID = 0,

		// no value used
		ABSOLUTE_TIME_SEC,
		// using mFloat to store the reference time as seconds (time relative to start time)
		RELATIVE_TIME_SEC,
		// no value used
		DELTA_TIME_SEC,

		// no value used
		MOUSE_POS_FACTOR,
		// no value used
		MOUSE_POS_PIXEL,

		// no value used
		VIEWPORT_POS_PIXEL,
		// no value used
		VIEWPORT_SIZE_PIXEL,

		// no value used
		VIEW_SIZE,
		// no value used
		VIEW_RATIO,

		// no value used
		PROJECTION_MATRIX,
		// no value used
		VIEW_MATRIX,
		// no value used
		MODEL_MATRIX,
		// no value used
		MODEL_VIEW_MATRIX,
		// no value used
		ENTITX_MATRIX,
		// no value used
		MVP_MATRIX,

		// value is used (which one depends on UniformType)
		CUSTOM_VALUE,
	};

	class Uniform
	{
	public:
		std::string mName;
		UniformType mType;
		UniformSource mSource;
		union {
			int mInt;
			float mFloat;
			struct {
				float x, y;
			} mVec2;
			struct {
				float x, y, z;
			} mVec3;
			struct {
				float x, y, z, w;
			} mVec4;
		} mValue;
		int mLocation;

		Uniform()
				:mName(), mType(UniformType::INVALID),
				mSource(UniformSource::INVALID),
				mLocation(-1) {}
		Uniform(const std::string& name, int intValue)
				:mName(name), mType(UniformType::INT),
				mSource(UniformSource::CUSTOM_VALUE),
				mLocation(-1) { mValue.mInt = intValue; }
		Uniform(const std::string& name, float floatValue)
				:mName(name), mType(UniformType::FLOAT),
				mSource(UniformSource::CUSTOM_VALUE),
				mLocation(-1) { mValue.mFloat = floatValue; }
		Uniform(const std::string& name, const glm::vec2& v)
				:mName(name), mType(UniformType::VEC2),
				mSource(UniformSource::CUSTOM_VALUE),
				mLocation(-1) { mValue.mVec2 = { v.x, v.y }; }
		Uniform(const std::string& name, const glm::vec3& v)
				:mName(name), mType(UniformType::VEC3),
				mSource(UniformSource::CUSTOM_VALUE),
				mLocation(-1) { mValue.mVec3 = {v.x, v.y, v.z}; }
		Uniform(const std::string& name, const glm::vec4& v)
				:mName(name), mType(UniformType::VEC4),
				mSource(UniformSource::CUSTOM_VALUE),
				mLocation(-1) { mValue.mVec4 = { v.x, v.y, v.z, v.w }; }
	};
}

#endif //GLSLSCENE_UNIFORM_H
