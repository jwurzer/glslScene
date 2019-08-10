#include <gs/rendering/shader_stack.h>
#include <gs/rendering/gl_api.h>
#include <gs/res/shader_program.h>

#include <gs/system/log.h>

#ifdef ENABLE_SHADER_STACK_STATISTIC
#define INCREASE_BIND_CALLS_COUNT \
	++mStat.mBindCalls; \

#else
#define INCREASE_BIND_CALLS_COUNT
#endif

gs::ShaderStack::ShaderStack()
		:mLayers{}, mNextLayerIndex(0),
		mCurrentBindedShader(nullptr), mShaderToBind(nullptr),
		mProperties()
{
}

gs::ShaderStack::~ShaderStack()
{
}

void gs::ShaderStack::resetStack()
{
	mNextLayerIndex = 0;
	mCurrentBindedShader = nullptr;
	mShaderToBind = nullptr;
	glUseProgram(0);
}

void gs::ShaderStack::setGlobalProperties(const Properties& p)
{
	mProperties = p;
}

void gs::ShaderStack::pushShaderProgram(ShaderProgram* shader)
{
	int index = mNextLayerIndex;
	++mNextLayerIndex;
	if (index >= MAX_LAYERS) {
		LOGE("Out of shader program stack for push! %d >= %d\n", index, MAX_LAYERS);
		return;
	}
	ShaderBindInfo& bindInfo = mLayers[index];
	bindInfo.shader = shader;
	mShaderToBind = shader;
#if 0
	// version with mMustBeBinded - binding happend later
	if (mCurrentBindedShader != shader) {
		// set bit to true
		//mMustBeBinded |= lookUpSet[textureUnit];
	}
	else {
		// set bit to false --> reset bit
		//mMustBeBinded &= lookUpReset[textureUnit];
	}
#else
	// version without mMustBeBinded - binding happend immediately
	if (mCurrentBindedShader != shader) {
		bindShaderProgram(shader);
		mCurrentBindedShader = shader;
		INCREASE_BIND_CALLS_COUNT
	}
	// set bit to false --> reset bit
	//mMustBeBinded &= lookUpReset[textureUnit];
#endif
}

void gs::ShaderStack::popShaderProgram()
{
	if (mNextLayerIndex == 0) {
		LOGE("Out of shader program stack! No shader program on shader program stack\n");
		return;
	}
	--mNextLayerIndex;
	// now mNextLayerIndex has the "current layer index"
	// which is the "next layer index" for the next pushShaderProgram()
	// but we need not the "current layer index" instead we need the
	// "previous layer index" and restore this shader program
	int index = mNextLayerIndex - 1;
	if (index >= MAX_LAYERS) {
		LOGE("Out of shader program stack for pop! %d >= %d\n", index, MAX_LAYERS);
		return;
	}

	// if index is negative then no "previous layer" exist --> set nullptr (no shader)
	ShaderProgram* shader = (index >= 0) ? mLayers[index].shader : nullptr;
	mShaderToBind = shader;
#if 1
	// version with mMustBeBinded - binding happend later
	if (mCurrentBindedShader != shader) {
		// set bit to true
		//mMustBeBinded |= lookUpSet[textureUnit];
	}
	else {
		// set bit to false --> reset bit
		//mMustBeBinded &= lookUpReset[textureUnit];
	}
#else
	// version without mMustBeBinded - binding happend immediately
	if (mCurrentBindedShader != shader) {
		bindShaderProgram(shader);
		mCurrentBindedShader = shader;
		INCREASE_BIND_CALLS_COUNT
	}
	// set bit to false --> reset bit
	//mMustBeBinded &= lookUpReset[textureUnit];
#endif
}

void gs::ShaderStack::bindForRendering()
{
	if (mCurrentBindedShader != mShaderToBind) {
		bindShaderProgram(mShaderToBind);
		mCurrentBindedShader = mShaderToBind;
		INCREASE_BIND_CALLS_COUNT
	}
}

void gs::ShaderStack::bindShaderProgram(ShaderProgram* shaderProgram)
{
	if (shaderProgram) {
		shaderProgram->bind(mProperties, mMatrices);
	}
	else {
		glUseProgram(0);
	}
}

void gs::ShaderStack::setMatrices(const gs::Matrices &m)
{
	mMatrices = m;

	if (mCurrentBindedShader == mShaderToBind) {
		// current binded shader is already binded but matrices changed
		// --> set the new matrices to the shader
		if (!mCurrentBindedShader) {
			// currently there is no shader used!
			return;
		}
		mCurrentBindedShader->bindMatricesOnly(m);
	}
}

