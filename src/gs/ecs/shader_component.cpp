#include <gs/ecs/shader_component.h>
#include <gs/res/resource_id_limit.h>
#include <gs/common/lookup8.h>
#include <string.h>


gs::ShaderComponent::ShaderComponent(const std::weak_ptr<Entity>& entity)
		:Component(entity), mSharedCustomData(),
		mShaderProgramId(0), mUniforms()
{
	reset();
}

gs::ShaderComponent::~ShaderComponent()
{
}

void gs::ShaderComponent::reset()
{
	setChanged();
	mShaderProgramId = 0;
	mUniforms.clear();
}

void gs::ShaderComponent::setShaderProgramId(gs::TShaderId shaderProgramId)
{
	setChanged();
	mShaderProgramId = shaderProgramId;
}

void gs::ShaderComponent::setUniformI(const std::string& name, int value)
{
	setUniform(Uniform(name, value));
}

void gs::ShaderComponent::setUniformF(const std::string& name, float value)
{
	setUniform(Uniform(name, value));
}

void gs::ShaderComponent::setUniformTime(const std::string& name)
{
	Uniform uniform;
	uniform.mName = name;
	uniform.mSource = UniformSource::ABSOLUTE_TIME_SEC;
	setUniform(uniform);
}

void gs::ShaderComponent::setUniformRelativeTime(const std::string& name, int64_t refTs)
{
	Uniform uniform;
	uniform.mName = name;
	uniform.mSource = UniformSource::RELATIVE_TIME_SEC;
	setUniform(uniform);
}

void gs::ShaderComponent::setUniform(const Uniform& uniform)
{
	setChanged();
	size_t cnt = mUniforms.size();
	for (size_t i = 0; i < cnt; ++i) {
		if (mUniforms[i].mName == uniform.mName) {
			mUniforms[i] = uniform;
			return;
		}
	}
	mUniforms.push_back(uniform);
}

void gs::ShaderComponent::setUniforms(const std::vector<gs::Uniform>& uniforms)
{
	for (const auto& u : uniforms) {
		setUniform(u);
	}
}

