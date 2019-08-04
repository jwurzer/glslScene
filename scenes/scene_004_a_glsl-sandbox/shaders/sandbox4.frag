#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void main( void ) {
	vec2 position = gl_FragCoord.xy - resolution / 2.0;
	float color = smoothstep(2.0, 4.0, abs(length(position) - 50.0 + sin(atan(position.y, position.x) * 4.0 - 3.141 / 2.0) * 7.0*sin(time)));
	gl_FragColor = vec4(vec3(color), 1.0 );

}
