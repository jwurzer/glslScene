#version 120

uniform sampler2D uTexture;
uniform float uTime;

varying vec4 vColor;
varying vec2 vTexCoord0;

const int count = 15;

void main(void)
{
	float d = 0.01 * (sin(uTime) * 0.5 + 0.5);
	vec2 diff = vec2(d, 0.0);

	vec2 offset = vec2(-d * float(count) * 0.5, -d * float(count) * 0.5);
	vec4 texColor = vec4(0.0);
	for (int y = 0; y < count; ++y) {
		for (int x = 0; x < count; ++x) {
			offset.x += diff.x;
			texColor += texture2D(uTexture, vTexCoord0 + offset);
		}
		offset.x = -d * float(count) * 0.5;
		offset.y += diff.y;
	}
	texColor /= count * count;
	//gl_FragColor = vColor * texColor;
	gl_FragColor = vColor * 0.5 + texColor * 0.5;
}
