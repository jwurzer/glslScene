#version 330

out vec4 uFragColor;

in vec4 vColor;

void main(void)
{
	uFragColor = vColor;
}
