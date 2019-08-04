
uniform float uTime;

void main()
{
	float factor = sin(uTime * 2.0) * 0.5 + 0.5;
	//vec4 color = vec4(gl_Color.rgb * factor, gl_Color.a);
	//vec4 color = gl_Color * factor;
	vec4 color = vec4(gl_Color.rgb, gl_Color.a * factor);
	gl_FrontColor = color;
	gl_BackColor = color;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
