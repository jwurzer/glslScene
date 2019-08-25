#ifndef GLSLSCENE_LIGHT_MATRIX_LOGIC_H
#define GLSLSCENE_LIGHT_MATRIX_LOGIC_H

#include <gs/logic/logic.h>
#include <gs/common/vector2.h>
#include <string>

namespace gs
{
	class LightMatrixLogic: public Logic
	{
	public:
		LightMatrixLogic(const std::string& shaderIdName, unsigned int lightNr);
		virtual ~LightMatrixLogic();

		virtual void handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p, const SDL_Event& evt) override;
		virtual void update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p) override;
	private:
		const std::string mShaderIdName;
		unsigned int mLightNr;
		bool mWarningIsPrinted = false;
	};
}

#endif
