#version 120

uniform sampler2D uTexFb;
uniform float uTime;

varying vec2 vTexCoord0;

const int count = 15;

void main(void)
{
	//gl_FragColor = texture2D(uTexFb, vTexCoord0);
	//float diff = 0.01 * (sin(uTime) * 0.5 + 0.5);
	float diff = 0.001;

	float offset = -diff * float(count) * 0.5;
	vec4 texColor = vec4(0.0);
	float texCoordY = vTexCoord0.y;
	texCoordY += sin((vTexCoord0.x + uTime * 0.1) * 100.0) * 0.005;
	for (int x = 0; x < count; ++x) {
		offset += diff;
		texColor += texture2D(uTexFb, vec2(vTexCoord0.x + offset, texCoordY));
	}

	texColor /= float(count);
	gl_FragColor = texColor;
}
