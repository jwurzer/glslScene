#version 330 core

out vec4 uFragColor;

in vec4 fColor;

void main(void)
{
	uFragColor = fColor;
}
