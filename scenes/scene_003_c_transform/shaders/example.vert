
uniform float uTime;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uModelViewProjectionMatrix;
uniform mat4 uEntityMatrix;

attribute vec4 aPosition;
attribute vec4 aColor;

void main()
{
	float factor = sin(uTime * 2.0) * 0.5 + 0.5;
	//vec4 color = vec4(aColor.rgb * factor, aColor.a);
	//vec4 color = aColor * factor;
	vec4 color = vec4(aColor.rgb, aColor.a * factor);
	gl_FrontColor = color;
	gl_BackColor = color;
	vec4 pos = vec4(aPosition.xyz, 1.0);

	//vec4 pos = aPosition;



	// gl_... matrices
	// ===============

	// 1) variant
	//gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos;

	// 1) variant in single steps - calc each position
	//vec4 globalPos = gl_ModelViewMatrix * pos;
	//gl_Position = gl_ProjectionMatrix * globalPos;

	// 1) variant in single steps - create one matrix
	//mat4 mvp = gl_ProjectionMatrix * gl_ModelViewMatrix;
	//gl_Position = mvp * pos;

	// 1) variant in single steps - create one matrix - WRONG VERSION
	//mat4 mvp = gl_ModelViewMatrix * gl_ProjectionMatrix;
	//gl_Position = mvp * pos;


	// 2) variant
	//gl_Position = gl_ModelViewProjectionMatrix * pos;



	// own matrices
	// ============

	// 1) variant
	//gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * pos;

	// 1) variant in single steps - calc each position
	//vec4 posGlobal = uModelMatrix * pos;
	//vec4 posView = uViewMatrix * posGlobal;
	//gl_Position = uProjectionMatrix  * posView;

	// 1) variant in single steps - calc each position - WRONG VERSION
	//vec4 pos1 = uProjectionMatrix * pos;
	//vec4 pos2 = uViewMatrix * pos1;
	//gl_Position = uModelMatrix * pos2;

	// 1) variant in single steps - create one matrix
	//mat4 mvp = uProjectionMatrix * uViewMatrix * uModelMatrix;
	//gl_Position = mvp * pos;

	// 1) variant in single steps - create one matrix
	//mat4 mv = uViewMatrix * uModelMatrix;
	//mat4 mvp = uProjectionMatrix * mv;
	//gl_Position = mvp * pos;

	// 1) variant in single steps - create one matrix - WRONG VERSION
	//mat4 mv = uViewMatrix * uModelMatrix;
	//mat4 mvp = mv * uProjectionMatrix; // WRONG !!!!!!!!
	//gl_Position = mvp * pos;

	// 1) variant in single steps - create one matrix
	mat4 vp = uProjectionMatrix * uViewMatrix;
	mat4 mvp = vp * uModelMatrix;
	//mat4 mvp = uModelMatrix * vp; // that would be wrong...
	gl_Position = mvp * pos;
	//gl_Position = pos * mvp; // that would be wrong...

	// 2) variant
	//gl_Position = uProjectionMatrix * uModelViewMatrix * pos;

	// 3) variant
	//gl_Position = uModelViewProjectionMatrix * pos;
}
