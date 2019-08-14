#version 120

attribute vec2 aTexCoord0;
attribute vec3 aPosition;

varying vec2 vTexCoord0;

void main()
{
	vTexCoord0 = aTexCoord0;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(aPosition, 1.0);
}
