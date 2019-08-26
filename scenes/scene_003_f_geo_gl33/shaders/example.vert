#version 330 core

// only for testing using locations not starting with 0 and 1
layout (location=6) in vec4 aColor;
layout (location=3) in vec2 aPosition;
in float aSides;

//in vec2 aPosition;
//in vec4 aColor;

out vec4 vColor;
out float vSides;

void main()
{
	//float factor = sin(uTime * 2.0) * 0.5 + 0.5;
	//vec4 color = vec4(aColor.rgb * factor, aColor.a);
	//vec4 color = aColor * factor;
	//vec4 color = vec4(aColor.rgb, aColor.a * factor);
	vColor = aColor;
	vSides = aSides;
	gl_Position = vec4(aPosition.xy, 0.0, 1.0);
	//vec4 pos = aPosition;

	//gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * pos;
}
