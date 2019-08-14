#version 120

uniform sampler2D uDiffuseMap;
uniform sampler2D uSpecularMap;

uniform vec3 uLightColor;
uniform float uShininess;

uniform float uAmbientStrength;
uniform float uDiffuseStrength;
uniform float uSpecularStrength;

uniform mat4 uInverseViewMatrix; // for camera position
uniform mat4 uLightModelMatrix0;

varying vec4 vColor;
varying vec3 vNormal;
varying vec2 vTexCoord;
varying vec3 vFragPos;

void main(void)
{
	// light position at world space
	vec3 lightPos = vec3(uLightModelMatrix0 * vec4(0.0, 0.0, 0.0, 1.0));
	//vec3 lightPos = vec3(250.0, 150.0, 0.0);
	vec3 norm = normalize(vNormal);
	vec3 lightDir = normalize(lightPos - vFragPos);
	// diffuseStrength = strength of reflected diffuse light
	float diffuseStrength = max(dot(norm, lightDir), 0.0) * uDiffuseStrength;

	vec3 eyePos = vec3(uInverseViewMatrix * vec4(0.0, 0.0, 0.0, 1.0));
	vec3 viewDir = normalize(eyePos - vFragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), uShininess);
	float specularStrength = uSpecularStrength * spec;

	vec4 diffuseTexColor = texture2D(uDiffuseMap, vTexCoord);
	vec4 diffuseColor = vec4(diffuseTexColor.rgb * (uAmbientStrength + diffuseStrength), diffuseTexColor.a) *
			vColor * vec4(uLightColor, 1.0);

	vec4 specularTexColor = texture2D(uSpecularMap, vTexCoord);
	vec4 specularColor = vec4(specularTexColor.rgb * specularStrength, 1.0);

	vec4 finalColor = diffuseColor + specularColor;
	//finalColor.a = 1.0;
	//float finalStrength = min(uAmbientStrength + diffuseStrength + specularStrength, 1.0);

	//gl_FragColor = finalColor * vec4(uLightColor, 1.0);
	gl_FragColor = finalColor;
}
