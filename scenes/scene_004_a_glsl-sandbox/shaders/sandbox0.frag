uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void main(void) {
	vec2 position = (gl_FragCoord.xy / resolution.xy);
	float sinus = (sin((position.x + time / 10.0) * 30.0) + 2.0) * 0.2;
	float factor = sinus * 0.5 + 0.5;
	vec4 color = vec4(1.0, 1.0, 0.0, 1.0);
	gl_FragColor = factor * color;
	//gl_FragColor = maxColor * factor;
}
