#version 330

uniform float uTime;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uEntityMatrix;

// only for testing using locations not starting with 0 and 1
layout (location=6) in vec4 aColor;
layout (location=3) in vec2 aPosition;
//in vec2 aPosition;
//in vec4 aColor;

out vec4 vColor;

void main()
{
	float factor = sin(uTime * 2.0) * 0.5 + 0.5;
	//vec4 color = vec4(aColor.rgb * factor, aColor.a);
	//vec4 color = aColor * factor;
	vec4 color = vec4(aColor.rgb, aColor.a * factor);
	vColor = color;
	vec4 pos = vec4(aPosition.xy, 0.0, 1.0);
	//vec4 pos = aPosition;

	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * pos;
}
