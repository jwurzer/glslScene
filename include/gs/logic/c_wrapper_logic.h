#ifndef GLSLSCENE_C_WRAPPER_LOGIC_H
#define GLSLSCENE_C_WRAPPER_LOGIC_H

#include <gs/logic/logic.h>
#include <capi/logic/logic.h>

namespace gs
{
	class CWrapperLogic: public Logic
	{
	public:
		CWrapperLogic(std::unique_ptr<::Logic> logic);
		virtual ~CWrapperLogic();

		virtual void handleEvent(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p, const SDL_Event& evt) override;
		virtual void update(const std::shared_ptr<Entity>& e, ResourceManager& rm,
				const Properties& p) override;
	private:
		std::unique_ptr<::Logic> mLogic;
	};
}

#endif //GLSLSCENE_C_WRAPPER_LOGIC_H
