#ifndef GLSLSCENE_GL_API_H
#define GLSLSCENE_GL_API_H

#include <gs/rendering/gl_version.h>

#ifdef GLSLSCENE_USE_GLEW
	#include <GL/glew.h>
#else
	#ifdef GLSLSCENE_USE_GLAD
		#include <glad/glad.h>
		//#include <GL/gl.h>
		#include <GL/glu.h>
	#else
		#error "no gl version for header files is configured"
	#endif
#endif

#endif