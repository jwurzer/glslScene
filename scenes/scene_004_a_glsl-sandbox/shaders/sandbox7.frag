// http://glslsandbox.com/e#56381.0

#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.14159265359
#define TWO_PI 6.28318530718

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;

vec3 hsb2rgb( in vec3 c ){
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0,
                     0.0,
                     1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}

vec3 rgb2hsb( in vec3 c ){
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz),
                 vec4(c.gb, K.xy),
                 step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r),
                 vec4(c.r, p.yzx),
                 step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)),
                d / (q.x + e),
                q.x);
}

void main() {
	vec2 st = (gl_FragCoord.xy*2.0-resolution)/min(resolution.x,resolution.y);
	vec2 pq = gl_FragCoord.xy/resolution;
	
	vec3 color=vec3(1.);

    vec2 toCenter=st;
    float angle=atan(toCenter.y,toCenter.x)/TWO_PI;
    float radius=length(toCenter);
    //color=hsb2rgb(vec3((angle/TWO_PI)+.5,1.,1.));
    color = hsb2rgb(vec3((angle)+fract(time*2.),radius,1.0));
    float circle = (0.03) / abs(0.6 - length(st));
    color = color * circle;
	gl_FragColor = vec4(color,1.0);
}
