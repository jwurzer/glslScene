#version 120

uniform float uTime;
uniform float uSpeed;

attribute vec4 aColor;
attribute vec2 aTexCoord0;
attribute vec3 aPosition;

varying vec4 vColor;
varying vec2 vTexCoord0;

void main()
{
	vTexCoord0 = aTexCoord0;
	float radius = sqrt(aPosition.x * aPosition.x + aPosition.y * aPosition.y);
	//float factor = sin(uTime * 4.0) * 0.5 + 0.5;
	//float factor = sin(radius * 30) * 0.5 + 0.5;
	float factor = sin(radius * 30 - uTime * uSpeed) * 0.5 + 0.5;
	vec4 color = vec4(aColor.rgb, factor);
	vColor = color;
	vec3 pos = aPosition;
	pos.z += factor * 0.05;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(pos, 1.0);
}
