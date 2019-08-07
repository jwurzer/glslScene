#include <scene/light_matrix_logic.h>
#include <system/log.h>
#include <ecs/entity.h>
#include <ecs/transform_component.h>
#include <ecs/shader_component.h>
#include <res/shader_program.h>
#include <rendering/properties.h>
#include <res/resource_manager.h>

gs::LightMatrixLogic::LightMatrixLogic(const std::string& shaderIdName, unsigned int lightNr)
		:mShaderIdName(shaderIdName),
		mLightNr(lightNr)
{
}

gs::LightMatrixLogic::~LightMatrixLogic()
{
}

void gs::LightMatrixLogic::handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& p, const SDL_Event& evt)
{
}

void gs::LightMatrixLogic::update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
		const Properties& p)
{
	std::shared_ptr<gs::ShaderProgram> shader = rm.getShaderProgramByIdName(mShaderIdName);
	if (!shader) {
		LOGE("failed\n");
		return;
	}
	std::string nr = std::to_string(mLightNr);

	bool model = shader->changeUniformMat4("uLightModelMatrix" + nr, p.mModelMatrix);
	if (!model && !mWarningIsPrinted) {
		LOGW("No uLightModelMatrix%u exist!\n", mLightNr);
		mWarningIsPrinted = true;
	}
}

