#include <gs/res/shader_program.h>
#include <gs/system/log.h>
#include <gs/common/fs.h>
#include <gs/rendering/gl_api.h>
#include <gs/res/uniform.h>
#include <gs/rendering/properties.h>
#include <gs/rendering/matrices.h>
#include <glm/gtc/type_ptr.hpp>

#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER 0 // for not supported
#endif
#ifndef GL_TESS_CONTROL_SHADER
#define GL_TESS_CONTROL_SHADER 0 // for not supported
#endif
#ifndef GL_TESS_EVALUATION_SHADER
#define GL_TESS_EVALUATION_SHADER 0 // for not supported
#endif
#ifndef GL_GEOMETRY_SHADER
#define GL_GEOMETRY_SHADER 0 // for not supported
#endif

namespace gs
{
	namespace
	{
		GLuint getGlShaderType(ShaderType shaderType)
		{
			switch (shaderType) {
				case ShaderType::COMPUTE_SHADER:
					return GL_COMPUTE_SHADER;
				case ShaderType::VERTEX_SHADER:
					return GL_VERTEX_SHADER;
				case ShaderType::TESS_CONTROL_SHADER:
					return GL_TESS_CONTROL_SHADER;
				case ShaderType::TESS_EVALUATION_SHADER:
					return GL_TESS_EVALUATION_SHADER;
				case ShaderType::GEOMETRY_SHADER:
					return GL_GEOMETRY_SHADER;
				case ShaderType::FRAGMENT_SHADER:
					return GL_FRAGMENT_SHADER;
			}
			return 0;
		}

		std::string getShaderTypeAsString(ShaderType shaderType)
		{
			switch (shaderType) {
				case ShaderType::COMPUTE_SHADER:
					return "COMPUTE SHADER";
				case ShaderType::VERTEX_SHADER:
					return "VERTEX SHADER";
				case ShaderType::TESS_CONTROL_SHADER:
					return "TESS CONTROL SHADER";
				case ShaderType::TESS_EVALUATION_SHADER:
					return "TESS EVALUATION SHADER";
				case ShaderType::GEOMETRY_SHADER:
					return "GEOMETRY SHADER";
				case ShaderType::FRAGMENT_SHADER:
					return "FRAGMENT SHADER";
			}
			return "unknown";
		}

		GLuint createAndLoadShader(ShaderType shaderType, const std::string& sourceCode)
		{
			GLuint glShaderType = getGlShaderType(shaderType);
			if (!glShaderType) {
				return 0;
			}
			GLuint shader = glCreateShader(glShaderType);

			// Get strings for glShaderSource.
			const char* source = sourceCode.c_str();
			glShaderSource(shader, 1, &source, nullptr);

			glCompileShader(shader);

			GLint isCompiled = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
			if(isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> errorLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

				LOGE("compile error for %s:\n%s\n",
						getShaderTypeAsString(shaderType).c_str(), errorLog.data());

				// Exit with failure.
				glDeleteShader(shader); // Don't leak the shader.
				return 0;
			}

			return shader;
		}

		GLuint createAndCompileShaderProgram(const std::vector<ShaderLoadInfo>& shaderSources)
		{
			std::vector<GLuint> shaders;
			shaders.resize(shaderSources.size(), 0);
			unsigned int i = 0;
			for (const auto& info : shaderSources) {
				shaders[i] = createAndLoadShader(info.getShaderType(), info.getFilenameOrSource());
				if (!shaders[i]) {
					break;
				}
				++i;
			}
			if (i != shaderSources.size()) {
				// i has now the index of the failed shader (which need not to be deleted
				// because it was not created)
				for (unsigned int ii = 0; ii < i; ++ii) {
					glDeleteShader(shaders[ii]);
				}
				return 0;
			}

			GLuint program = glCreateProgram();

			// Attach our shaders to our program
			for (GLuint shaderId : shaders) {
				glAttachShader(program, shaderId);
			}

			// Link our program
			glLinkProgram(program);

			// Note the different functions here: glGetProgram* instead of glGetShader*.
			GLint isLinked = 0;
			glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
			if (isLinked == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

				// We don't need the program anymore.
				glDeleteProgram(program);
				// Don't leak shaders either.
				for (GLuint shaderId : shaders) {
					glDeleteShader(shaderId);
				}

				// Use the infoLog as you see fit.
				LOGE("linking error:\n%s\n", infoLog.data());

				// In this simple program, we'll just leave
				return 0;
			}

			// Always detach shaders after a successful link.
			for (GLuint shaderId : shaders) {
				glDetachShader(program, shaderId);
			}

			// also delete the shader objects because they are not used for other shader programs
			for (GLuint shaderId : shaders) {
				glDeleteShader(shaderId);
			}

			return program;
		}

		bool loadAttributeLocations(GLuint shaderProgramId, std::vector<Attribute>& attributes)
		{
			unsigned int nextAllowedOffset = 0;
			for (auto& a : attributes) {
				if (a.mCompOffset < nextAllowedOffset) {
					LOGE("attribute component offset must be >= %u.\n", nextAllowedOffset);
					return false;
				}
				if (!a.mCompCount) {
					LOGE("attribute component count can't be zero.\n");
					return false;
				}
				nextAllowedOffset = a.mCompOffset + a.mCompCount;
				a.mLocation = glGetAttribLocation(shaderProgramId, a.mName.c_str());
				if (a.mLocation == -1) {
					LOGW("attribute %s can't be found\n", a.mName.c_str());
				}
				LOGI("attribute %s on location %d\n", a.mName.c_str(), a.mLocation);
			}
			return true;
		}

		void resetAttributeLocations(std::vector<Attribute>& attributes)
		{
			for (auto& a : attributes) {
				a.mLocation = -1;
			}
		}

		bool loadUniformLocations(GLuint shaderProgramId, std::vector<Uniform>& uniforms)
		{
			for (Uniform& u : uniforms) {
				u.mLocation = glGetUniformLocation(shaderProgramId, u.mName.c_str());
				if (u.mLocation == -1) {
					LOGW("uniform %s can't be found\n", u.mName.c_str());
					continue;
				}
				LOGI("uniform %s on location %d\n", u.mName.c_str(), u.mLocation);
				// TODO check uniform types... (if uniform exist)
			}
			return true;
		}

		void resetUniformLocations(std::vector<Uniform>& uniforms)
		{
			for (Uniform& u : uniforms) {
				u.mLocation = -1;
			}
		}

		void setUniformFromFloat(const Uniform& u, float x, float y, float z, float w)
		{
			if (u.mLocation == -1) {
				return;
			}
			switch (u.mType) {
				case UniformType::INT:
					glUniform1i(u.mLocation, static_cast<GLint>(x));
					break;
				case UniformType::FLOAT:
					glUniform1f(u.mLocation, x);
					break;
				case UniformType::VEC2:
					glUniform2f(u.mLocation, x, y);
					break;
				case UniformType::VEC3:
					glUniform3f(u.mLocation, x, y, z);
					break;
				case UniformType::VEC4:
					glUniform4f(u.mLocation, x, y, z, w);
					break;
				default:
					LOGW("type %d is not supported by this function.\n", int(u.mType));
					break;
			}
		}

		void applyUniformValues(const std::vector<Uniform>& uniforms, const Properties& p, const Matrices& m)
		{
			for (const Uniform& u : uniforms) {
				switch (u.mSource) {
					case UniformSource::INVALID:
						LOGW("Uniform with invalid source.\n");
						break;
					// no value used
					case UniformSource::ABSOLUTE_TIME_SEC:
						setUniformFromFloat(u, p.mTsSec, 0.0f, 0.0f, 0.0f);
						break;
					// using mFloat to store the reference time as seconds (time relative to start time)
					case UniformSource::RELATIVE_TIME_SEC:
						setUniformFromFloat(u, p.mTsSec - u.mValue.mFloat, 0.0f, 0.0f, 0.0f);
						break;
					// no value used
					case UniformSource::DELTA_TIME_SEC:
						setUniformFromFloat(u, p.mDeltaTimeSec, 0.0f, 0.0f, 0.0f);
						break;
					// no value used
					case UniformSource::MOUSE_POS_FACTOR:
						setUniformFromFloat(u, p.mMousePosFactor.x, p.mMousePosFactor.y, 0.0f, 0.0f);
						break;
					// no value used
					case UniformSource::MOUSE_POS_PIXEL:
						setUniformFromFloat(u, p.mMousePosPixel.x, p.mMousePosPixel.y, 0.0f, 0.0f);
						break;
					// no value used
					case UniformSource::VIEWPORT_POS_PIXEL:
						setUniformFromFloat(u, p.mViewportPosPixel.x, p.mViewportPosPixel.y, 0.0f, 0.0f);
						break;
					// no value used
					case UniformSource::VIEWPORT_SIZE_PIXEL:
						setUniformFromFloat(u, p.mViewportSizePixel.mWidth, p.mViewportSizePixel.mHeight, 0.0f, 0.0f);
						break;
					// no value used
					case UniformSource::VIEW_SIZE:
						setUniformFromFloat(u, p.mViewSize.x, p.mViewSize.y, p.mViewSize.z, 0.0f);
						break;
					// no value used
					case UniformSource::VIEW_RATIO:
						setUniformFromFloat(u, p.mViewRatio.mWidth, p.mViewRatio.mHeight, 0.0f, 0.0f);
						break;
					case UniformSource::PROJECTION_MATRIX:
						if (u.mInverse) {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(glm::inverse(p.mProjectionMatrix)));
						}
						else {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(p.mProjectionMatrix));
						}
						break;
					case UniformSource::VIEW_MATRIX:
						if (u.mInverse) {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(glm::inverse(p.mViewMatrix)));
						}
						else {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(p.mViewMatrix));
						}
						break;
					case UniformSource::MODEL_MATRIX:
						if (u.mInverse) {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(glm::inverse(m.mModelMatrix)));
						}
						else {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mModelMatrix));
						}
						break;
					case UniformSource::MODEL_VIEW_MATRIX:
						if (u.mInverse) {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(glm::inverse(m.mModelViewMatrix)));
						}
						else {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mModelViewMatrix));
						}
						break;
					case UniformSource::ENTITY_MATRIX:
						if (u.mInverse) {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(glm::inverse(m.mEntityMatrix)));
						}
						else {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mEntityMatrix));
						}
						break;
					case UniformSource::MVP_MATRIX:
						if (u.mInverse) {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(glm::inverse(m.mMvpMatrix)));
						}
						else {
							glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mMvpMatrix));
						}
						break;
					// value is used (which one depends on UniformType)
					case UniformSource::CUSTOM_VALUE:
						if (u.mLocation == -1) {
							break;
						}
						switch (u.mType) {
							case UniformType::INVALID:
								LOGW("Uniform with invalid type.\n");
								break;
							case UniformType::INT:
							case UniformType::SAMPLER2D:
								glUniform1i(u.mLocation, u.mValue.mInt);
								break;
							case UniformType::FLOAT:
								glUniform1f(u.mLocation, u.mValue.mFloat);
								break;
							case UniformType::VEC2:
								glUniform2f(u.mLocation, u.mValue.mVec2.x, u.mValue.mVec2.y);
								break;
							case UniformType::VEC3:
								glUniform3f(u.mLocation, u.mValue.mVec3.x, u.mValue.mVec3.y, u.mValue.mVec3.z);
								break;
							case UniformType::VEC4:
								glUniform4f(u.mLocation, u.mValue.mVec4.x, u.mValue.mVec4.y, u.mValue.mVec4.z, u.mValue.mVec4.w);
								break;
							case UniformType::MAT4X4:
								glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, u.mValue.mMat4.m);
								break;
						}
						break;
				}
			}
		}

		void applyUniformMatricesOnly(const std::vector<Uniform>& uniforms, const Matrices& m)
		{
			for (const Uniform& u : uniforms) {
				switch (u.mSource) {
					case UniformSource::MODEL_MATRIX:
						glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mModelMatrix));
						break;
					case UniformSource::MODEL_VIEW_MATRIX:
						glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mModelViewMatrix));
						break;
					case UniformSource::ENTITY_MATRIX:
						glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mEntityMatrix));
						break;
					case UniformSource::MVP_MATRIX:
						glUniformMatrix4fv(u.mLocation, 1, GL_FALSE, glm::value_ptr(m.mMvpMatrix));
						break;
					default:
						break;
				}
			}
		}
	}
}

gs::ShaderProgram::ShaderProgram(const std::weak_ptr<FileChangeMonitoring>& fcm,
		const ShaderProgramLoadInfo& shaderProgramLoadInfo)
		:Resource(fcm), mShaderProgramLoadInfo(shaderProgramLoadInfo), mShaderSources(),
		mAddedSourceFilesForHotReloading(false), mShaderProgramId(0)
{
}

gs::ShaderProgram::~ShaderProgram()
{
	unload();
}

gs::ResType gs::ShaderProgram::getType() const
{
	return ResType::SHADER_PROGRAM;
}

bool gs::ShaderProgram::load()
{
	const std::vector<ShaderLoadInfo>& shaderInfos = mShaderProgramLoadInfo.mShaderInfos;
	std::vector<ShaderLoadInfo> shaderSources;
	shaderSources.reserve(shaderInfos.size());
	int i = 0;
	bool loadAllFiles = true;
	for (const auto& info : shaderInfos) {
		if (info.isFilename()) {
			if (!mAddedSourceFilesForHotReloading) {
				addFileForHotReloading(info.getFilenameOrSource());
			}
			std::string fileContent = fs::readFileAsString(info.getFilenameOrSource());
			if (fileContent.empty()) {
				LOGE("Can't load %s (or file is empty)\n", info.getFilenameOrSource().c_str());
				loadAllFiles = false;
				continue;
			}
			shaderSources.emplace_back(shaderInfos[i].getShaderType(), fileContent, false);
		}
		else {
			shaderSources.emplace_back(shaderInfos[i]);
		}
		++i;
	}
	mAddedSourceFilesForHotReloading = true;
	if (!loadAllFiles) {
		LOGE("Can't load one or more files for the shader program.\n");
		return false;
	}

	GLuint shaderProgramId = createAndCompileShaderProgram(shaderSources);
	if (!shaderProgramId) {
		return false;
	}

	std::vector<Uniform> uniforms = mShaderProgramLoadInfo.mUniforms;
	if (!loadUniformLocations(shaderProgramId, uniforms)) {
		return false;
	}

	std::vector<Attribute> attributes = mShaderProgramLoadInfo.mAttributes;
	if (!loadAttributeLocations(shaderProgramId, attributes)) {
		return false;
	}

	// --> loading was successful --> unload old shader and apply new loaded shader

	unload();

	mShaderSources = shaderSources;
	mShaderProgramId = shaderProgramId;
	// copy the uniforms to update the new location uniforms
	mShaderProgramLoadInfo.mUniforms = uniforms;
	// copy the attributes to update the new location attributes
	mShaderProgramLoadInfo.mAttributes = attributes;

	++mLoadCounter;
	return true;
}

void gs::ShaderProgram::unload()
{
	resetUniformLocations(mShaderProgramLoadInfo.mUniforms);
	resetAttributeLocations(mShaderProgramLoadInfo.mAttributes);

	if (mShaderProgramId) {
		glDeleteProgram(mShaderProgramId);
		mShaderProgramId = 0;
	}
	mShaderSources.clear();
}

bool gs::ShaderProgram::changeUniformFloat(const std::string& name, float value)
{
	for (auto& u : mShaderProgramLoadInfo.mUniforms) {
		if (u.mName == name) {
			if (u.mType != UniformType::FLOAT) {
				LOGE("Uniform value can't be changed. %s is not a float.\n", u.mName.c_str());
				return false;
			}
			u.mValue.mFloat = value;
			return true;
		}
	}
	return false;
}

bool gs::ShaderProgram::changeUniformVec2(const std::string& name, const glm::vec2& v2)
{
	for (auto& u : mShaderProgramLoadInfo.mUniforms) {
		if (u.mName == name) {
			if (u.mType != UniformType::VEC2) {
				LOGE("Uniform value can't be changed. %s is not a vec2.\n", u.mName.c_str());
				return false;
			}
			u.mValue.mVec2.x = v2.x;
			u.mValue.mVec2.y = v2.y;
			return true;
		}
	}
	return false;
}

bool gs::ShaderProgram::changeUniformVec3(const std::string& name, const glm::vec3& v3)
{
	for (auto& u : mShaderProgramLoadInfo.mUniforms) {
		if (u.mName == name) {
			if (u.mType != UniformType::VEC3) {
				LOGE("Uniform value can't be changed. %s is not a vec3.\n", u.mName.c_str());
				return false;
			}
			u.mValue.mVec3.x = v3.x;
			u.mValue.mVec3.y = v3.y;
			u.mValue.mVec3.z = v3.z;
			return true;
		}
	}
	return false;
}

bool gs::ShaderProgram::changeUniformVec4(const std::string& name, const glm::vec4& v4)
{
	for (auto& u : mShaderProgramLoadInfo.mUniforms) {
		if (u.mName == name) {
			if (u.mType != UniformType::VEC4) {
				LOGE("Uniform value can't be changed. %s is not a vec4.\n", u.mName.c_str());
				return false;
			}
			u.mValue.mVec4.x = v4.x;
			u.mValue.mVec4.y = v4.y;
			u.mValue.mVec4.z = v4.z;
			u.mValue.mVec4.w = v4.w;
			return true;
		}
	}
	return false;
}

bool gs::ShaderProgram::changeUniformMat4(const std::string& name, const glm::mat4& m)
{
	for (auto& u : mShaderProgramLoadInfo.mUniforms) {
		if (u.mName == name) {
			if (u.mType != UniformType::MAT4X4) {
				LOGE("Uniform value can't be changed. %s is not a mat4.\n", u.mName.c_str());
				return false;
			}
			memcpy(u.mValue.mMat4.m, glm::value_ptr(m), sizeof(u.mValue.mMat4.m));
			return true;
		}
	}
	return false;
}

void gs::ShaderProgram::bind(const Properties& p, const Matrices& m)
{
	glUseProgram(mShaderProgramId);
	applyUniformValues(mShaderProgramLoadInfo.mUniforms, p, m);
}

void gs::ShaderProgram::bindMatricesOnly(const Matrices& m)
{
	glUseProgram(mShaderProgramId);
	applyUniformMatricesOnly(mShaderProgramLoadInfo.mUniforms, m);
}

