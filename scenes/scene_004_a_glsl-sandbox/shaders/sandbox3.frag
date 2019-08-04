// http://glslsandbox.com/e#56183.0

#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
varying vec2 surfacePosition;

void main(void)
{

	vec3 pos = vec3(surfacePosition*10.,time);

	vec3 bgcol = sin(pos+(length(cos(pos))*1.618))*0.5+0.5;

	gl_FragColor = vec4(bgcol, 1.0 );
}
