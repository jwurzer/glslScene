#version 110

uniform sampler2D texSmiley;
uniform sampler2D texBg;

void main()
{
	vec4 col1 = texture2D(texSmiley, gl_TexCoord[0].xy);
	vec4 col2 = texture2D(texBg, gl_TexCoord[1].xy);
	
	gl_FragColor = (col1 + col2 + gl_Color) * 0.3333;
}
