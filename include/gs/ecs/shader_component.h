#ifndef GLSLSCENE_SHADER_COMPONENT_H
#define GLSLSCENE_SHADER_COMPONENT_H

#include <gs/ecs/component.h>
#include <gs/res/resource_id.h>
#include <gs/res/uniform.h>
#include <vector>

namespace gs
{
	class ShaderComponent: public Component
	{
	public:
		enum EDefs
		{
			MAX_TEXTURE_UNITS = 8,
		};
		/**
		 * This pointer can be used for anything.
		 * The pointer will be NOT copied at the clone function.
		 * The clone() function leave this pointer at the source unchanged and
		 * set this pointer to NULL at the destination!!!!!
		 *
		 * This pointer is used by the rendering engine for quick access of resources.
		 */
		mutable std::shared_ptr<void> mSharedCustomData;

		ShaderComponent(const std::weak_ptr<Entity>& entity);
		virtual ~ShaderComponent();

		virtual EComponent getType() const { return COMPONENT_SHADER; }

		/**
		 * Reset all to default beside of mCustomData. mCustomData is unchanged!
		 */
		void reset();

		void setShaderProgramId(TShaderId shaderProgramId);

		void setUniformI(const std::string& name, int value);
		void setUniformF(const std::string& name, float value);
		void setUniformTime(const std::string& name);
		void setUniformRelativeTime(const std::string& name, int64_t refTs);
		void setUniform(const Uniform& uniform);
		void setUniforms(const std::vector<gs::Uniform>& uniforms);

		TShaderId getShaderProgramId() const { return mShaderProgramId; }
	private:
		TShaderId mShaderProgramId;
		std::vector<Uniform> mUniforms;
	};
}

#endif /* GLSLSCENE_SHADER_COMPONENT_H */
