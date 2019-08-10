#ifndef GLSLSCENE_SHADER_INFO_H
#define GLSLSCENE_SHADER_INFO_H

#include <gs/res/uniform.h>
#include <gs/res/attribute.h>
#include <string>
#include <vector>

namespace gs
{
	enum class ShaderType
	{
		COMPUTE_SHADER,
		VERTEX_SHADER,
		TESS_CONTROL_SHADER,
		TESS_EVALUATION_SHADER,
		GEOMETRY_SHADER,
		FRAGMENT_SHADER
	};

	class ShaderLoadInfo
	{
	public:
		ShaderLoadInfo(ShaderType shaderType, const std::string& filenameOrSource, bool isFilename)
				:mShaderType(shaderType), mFilenameOrSource(filenameOrSource),
				mIsFilename(isFilename) {}
		ShaderType getShaderType() const { return mShaderType; }
		const std::string& getFilenameOrSource() const { return mFilenameOrSource; }
		bool isFilename() const { return mIsFilename; }
	private:
		ShaderType mShaderType;
		std::string mFilenameOrSource;
		bool mIsFilename;
	};

	class ShaderProgramLoadInfo
	{
	public:
		std::vector<ShaderLoadInfo> mShaderInfos;
		std::vector<Uniform> mUniforms;
		std::vector<Attribute> mAttributes;

		ShaderProgramLoadInfo() :mShaderInfos(), mUniforms(), mAttributes() {}
		//explicit ShaderProgramLoadInfo(const std::vector<ShaderLoadInfo>& shaderInfos)
		//		:mShaderInfos(shaderInfos), mUniforms() {}
		ShaderProgramLoadInfo(const std::vector<ShaderLoadInfo>& shaderInfos,
				const std::vector<Uniform>& uniforms,
				const std::vector<Attribute>& attributes)
				:mShaderInfos(shaderInfos), mUniforms(uniforms), mAttributes(attributes) {}
	};
}

#endif //GLSLSCENE_SHADER_H
