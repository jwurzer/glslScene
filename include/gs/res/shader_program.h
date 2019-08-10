#ifndef GLSLSCENE_SHADER_PROGRAM_H
#define GLSLSCENE_SHADER_PROGRAM_H

#include <gs/res/resource.h>
#include <gs/res/shader_info.h>
#include <gs/rendering/gl_api.h>
#include <glm/mat4x4.hpp>
#include <vector>

namespace gs
{
	class Properties;
	struct Matrices;

	class ShaderProgram : public Resource
	{
	public:
		ShaderProgram(const std::weak_ptr<FileChangeMonitoring>& fcm,
				const ShaderProgramLoadInfo& shaderProgramLoadInfo);
		virtual ~ShaderProgram();

		virtual ResType getType() const override;

		virtual bool load() override;

		virtual void unload() override;

		bool changeUniformFloat(const std::string& name, float value);
		bool changeUniformVec2(const std::string& name, const glm::vec2& v2);
		bool changeUniformVec3(const std::string& name, const glm::vec3& v3);
		bool changeUniformVec4(const std::string& name, const glm::vec4& v4);
		bool changeUniformMat4(const std::string& name, const glm::mat4& m);

		const ShaderProgramLoadInfo& getShaderProgramLoadInfo() const { return mShaderProgramLoadInfo; }
		const std::vector<ShaderLoadInfo>& getShaderSources() const { return mShaderSources; }
		//GLuint getGlShaderProgramId() const { return mShaderProgramId; }
		void bind(const Properties& p, const Matrices& m);
		// Warning: Correct program must be already binded!!!!!
		void bindMatricesOnly(const Matrices& m);
		const std::vector<Attribute>& getAttributes() const { return mShaderProgramLoadInfo.mAttributes; }
		unsigned int getLoadCounter() const { return mLoadCounter; }
	private:
		enum EDefs
		{
			MAX_UNITS = 8,
		};

		ShaderProgramLoadInfo mShaderProgramLoadInfo;
		std::vector<ShaderLoadInfo> mShaderSources;
		bool mAddedSourceFilesForHotReloading;
		unsigned int mLoadCounter = 0;

		GLuint mShaderProgramId;
	};
}

#endif //GLSLSCENE_SHADER_PROGRAM_H
