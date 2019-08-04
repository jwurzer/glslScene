#version 330

layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 texCoord0;
layout (location = 3) in vec2 texCoord1;

out vec4 fragColor;
out vec2 fragTexCoord0;
out vec2 fragTexCoord1;

void main()
{
	fragColor = color;
	fragTexCoord0 = texCoord0;
	fragTexCoord1 = texCoord1;
	gl_Position = vec4(pos, 0.0f, 1.0f);
}
