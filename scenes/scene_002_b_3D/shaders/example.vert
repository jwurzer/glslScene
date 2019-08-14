#version 120

//uniform float uTime;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uInverseModelMatrix;

attribute vec3 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord;
attribute vec4 aColor;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 vTexCoord;
varying vec3 vFragPos;

void main()
{
	vColor = aColor;
	vNormal = mat3(transpose(uInverseModelMatrix)) * aNormal;
	vTexCoord = aTexCoord;
	vec4 pos = vec4(aPosition, 1.0);

	vFragPos = vec3(uModelMatrix * pos);
	
	gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * pos;
}
