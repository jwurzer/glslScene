// http://glslsandbox.com/e#56397.0

/*
 * Original shader from: https://www.shadertoy.com/view/ttsXDB
 */

#ifdef GL_ES
precision mediump float;
#endif

// glslsandbox uniforms
uniform float time;
uniform vec2 resolution;

// shadertoy emulation
#define iTime time
#define iResolution resolution

// --------[ Original ShaderToy begins here ]---------- //
//Based on the original formula from http://paulbourke.net/fractals/magnet/
//Modified with different constants which change based on time and iteration

//Thanks to FabriceNeyret2 for some additional optimizations
//(his golfed version is in the comments)

const int MAX_STEPS=128;
const float PI=3.14159; //acos(-1.);
const float AA=2.;

#define vecMul(a,b) vec2(a.x*b.x-a.y*b.y,a.x*b.y+a.y*b.x)
#define vecDiv(a,b) vec2(a.x*b.x+a.y*b.y,a.y*b.x-a.x*b.y)/(b.x*b.x+b.y*b.y)

mat2 matRot(float a){
	float c=cos(a),
          s=sin(a);
    return mat2(c,s,s,-c);
}

vec3 samplePoint(vec2 c){
    float fi=0.;
    vec2 z=vec2(0.);
    float maxl=0.;
    for(int i=0;i<MAX_STEPS;i++){
        vec2 numer = vecMul(z,z)+c; numer.x-=1.+.5*sin((iTime+fi)/2.6);
        vec2 denom = 2.*z+c;        denom.x-=1.5+.5*sin((iTime+fi)/2.);
        vec2 div   = vecDiv(numer,denom);
        z = vecMul(div,div);
        float d=dot(z,z);
        if(d>16.){
            break;
        }
        maxl=max(maxl,d);
        fi+=1.;
    }
    
    float ic=(5.*fi/float(MAX_STEPS));
    return (4.-sqrt(maxl))*(cos(vec3(1,2,3)-ic)+1.)/8.;
}

void mainImage( out vec4 fragColor, in vec2 fragCoord ){
    vec2 c=2.*(2.*fragCoord-iResolution.xy)/iResolution.y;
    float delt=(2./iResolution.y)/AA;
    vec3 col=vec3(0.);
    for(float y=0.;y<AA;y++){
    	for(float x=0.;x<AA;x++){
    		col+=samplePoint(c+vec2(x,y)*delt);
    	}
    }
    col/=(AA*AA);
    fragColor = vec4(col,1.0);
}
// --------[ Original ShaderToy ends here ]---------- //

void main(void)
{
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
