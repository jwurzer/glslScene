#include <gs/logic/light_matrix_logic.h>
#include <gs/system/log.h>
#include <gs/ecs/entity.h>
#include <gs/ecs/transform_component.h>
#include <gs/ecs/shader_component.h>
#include <gs/res/shader_program.h>
#include <gs/rendering/properties.h>
#include <gs/res/resource_manager.h>

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

