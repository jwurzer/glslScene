#ifndef GLSLSCENE_MATRICES_H
#define GLSLSCENE_MATRICES_H

#include <glm/mat4x4.hpp>

namespace gs
{
	struct Matrices
	{
		// projection-matrix
		// view-matrix
		glm::mat4 mModelMatrix = glm::mat4(1.0f);
		glm::mat4 mModelViewMatrix = glm::mat4(1.0f);
		glm::mat4 mEntityMatrix = glm::mat4(1.0f);
		glm::mat4 mMvpMatrix = glm::mat4(1.0f);
	};
}

#endif //GLSLSCENE_MATRICES_H
