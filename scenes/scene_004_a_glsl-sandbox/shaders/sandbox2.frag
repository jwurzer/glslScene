// http://glslsandbox.com/e#56414.0

#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

void main1( void ) {

	vec2 position = ( gl_FragCoord.xy / resolution.xy ) + mouse / 4.0;

	float color = 0.0;
	color += sin( position.x * cos( time / 15.0 ) * 80.0 ) + cos( position.y * cos( time / 15.0 ) * 10.0 );
	color += sin( position.y * sin( time / 10.0 ) * 40.0 ) + cos( position.x * sin( time / 25.0 ) * 40.0 );
	color += sin( position.x * sin( time / 5.0 ) * 10.0 ) + sin( position.y * sin( time / 35.0 ) * 80.0 );
	color *= sin( time / 10.0 ) * 0.5;

	gl_FragColor = vec4( vec3( color, color * 0.5, sin( color + time / 3.0 ) * 0.75 ), 1.0 );

}

void main(void) {
	vec2 position = (gl_FragCoord.xy / resolution.xy);
	
	float sinus = (sin((position.x + time / 10.0) * 30.0) + 2.0) * 0.2;
	float color = 0.0;
	if (abs(sinus - position.y) < 0.01)
	{
		color = 0.0;
	}
	
	float dr = sinus - position.y;
	float dg = - sinus + position.y;
	float db = 1.0 - pow(20.0 * (sinus - position.y), 2.0);
	
	gl_FragColor = vec4( vec3(color + dr, color + dg, color + db), 1);
}