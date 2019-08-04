#ifndef GLSLSCENE_UNIFORM_ATTR_LOADER_H
#define GLSLSCENE_UNIFORM_ATTR_LOADER_H

#include <res/uniform.h>
#include <res/attribute.h>
#include <vector>
#include <string>

namespace gs
{
	class Uniform;
	class Attribute;
	class CfgValue;

	namespace uniformattrloader
	{
		/**
		 * uniform <type> <var-name> = <special-keyword>
		 * uniform int <var-name> = <value>
		 * uniform float <var-name> = <value>
		 * uniform vec2 <var-name> = <value> <value>
		 * uniform vec3 <var-name> = <value> <value> <value>
		 * uniform vec4 <var-name> = <value> <value> <value> <value>
		 * uniform sampler2d <var-name> = <texture-unit-index>    ... (0 - 7)
		 *
		 * special-keyword:
		 *
		 * time
		 * relative-time <ref-time-sec>
		 * delta-time
		 * mouse-pos-factor
		 * mouse-pos-pixel
		 * viewport-pos-pixel
		 * viewport-size-pixel
		 * view-size
		 * view-ratio
		 * projection-matrix
		 * view-matrix
		 * model-matrix
		 * model-view-matrix
		 * entity-matrix
		 * mvp-matrix
		 *
		 * attribute <var-name> = <component-offset> <component-count>
		 */
		bool loadUniformsAndAttributes(std::vector<Uniform> &outUniforms,
				std::vector<Attribute> &outAttributes,
				const CfgValue &cfgValue, unsigned int startIndex,
				const std::string &nameForErrorMsg);
	}
}

#endif
