#version 120

uniform float uTime;

void main(void)
{
	vec4 col = gl_Color;
	float factor =
		(sin(gl_FragCoord.x * 0.1 + sin(uTime * 2.0) * 3.14) * 0.5 + 0.5) * 0.5 +
		(sin(gl_FragCoord.y * 0.1 + sin(uTime) * 3.14) * 0.5 + 0.5) * 0.5;
	col.rgb *= factor;
	gl_FragColor = col;
}
