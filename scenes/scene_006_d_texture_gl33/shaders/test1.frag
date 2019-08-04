#version 330

uniform sampler2D texSmiley;
uniform sampler2D texBg;

in vec4 fragColor;
in vec2 fragTexCoord0;
in vec2 fragTexCoord1;

void main()
{
	vec4 col1 = texture2D(texSmiley, fragTexCoord0);
	vec4 col2 = texture2D(texBg, fragTexCoord1);
	
	gl_FragColor = (col1 + col2 + fragColor) * 0.3333;
}
