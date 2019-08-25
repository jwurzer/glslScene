#version 120

void main()
{
	gl_FrontColor = gl_Color;
	gl_BackColor = gl_Color;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
