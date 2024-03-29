#include <gs/rendering/renderer.h>
#include <gs/rendering/gl_api.h>
#include <gs/ecs/entity.h>
#include <gs/ecs/transform_component.h>
#include <gs/ecs/texture_component.h>
#include <gs/ecs/shader_component.h>
#include <gs/ecs/mesh_component.h>
#include <gs/ecs/child_entities.h>
#include <gs/res/mesh.h>
#include <gs/res/resource_manager.h>
#include <gs/scene/view.h>
#include <gs/common/lookup8.h>
#include <gs/common/sdl_events_trace.h>
#include <gs/system/log.h>
#include <glm/gtc/type_ptr.hpp>

gs::Renderer::Renderer()
		:mTextureStack(), mShaderStack()
{
	mMatrixStack.reserve(10);
}

gs::Renderer::~Renderer()
{
}

void gs::Renderer::render(const std::shared_ptr<Entity>& e, const ResourceManager& rm,
		const Properties& propertiesOrig)
{
	Properties properties = propertiesOrig;
	mMatrixStack.clear();

	Matrices m;
	m.mEntityMatrix = glm::mat4(1.0f);
	m.mModelMatrix *= m.mEntityMatrix;
	m.mModelViewMatrix = properties.mViewMatrix * m.mModelMatrix;
	//m.mModelViewMatrix = m.mModelMatrix * properties.mViewMatrix;
	m.mMvpMatrix = properties.mProjectionMatrix * m.mModelViewMatrix;
	properties.mModelMatrix = m.mModelMatrix;
	mMatrixStack.push_back(m); // push after m is set correct

	mShaderStack.setGlobalProperties(properties);
	mShaderStack.setMatrices(mMatrixStack.back());

	//LOGI("properties:\n%s\n", mProperties.toString().c_str());

	renderEntity(e, rm, properties);

	mShaderStack.resetStack();
	switchToTextureUnitForLoading();
}

void gs::Renderer::renderEntity(const std::shared_ptr<Entity>& e, const ResourceManager& rm,
		const Properties& propertiesOrig)
{
	Properties properties = propertiesOrig;
	if (e->getConstTransform()) {
		if (properties.mUseGlTransforms) {
			glPushMatrix();
			glMultMatrixf(glm::value_ptr(e->getConstTransform()->getMatrix()));
		}
		// create a copy of the last matrices and copy it to the matrix stack
		// The copy is necessary because to calc m.mModelMatrix the previous value is needed
		mMatrixStack.push_back(mMatrixStack.back());
		Matrices& m = mMatrixStack.back();
		m.mEntityMatrix = e->getConstTransform()->getMatrix();
		m.mModelMatrix *= m.mEntityMatrix;
		m.mModelViewMatrix = properties.mViewMatrix * m.mModelMatrix;
		m.mMvpMatrix = properties.mProjectionMatrix * m.mModelViewMatrix;
		properties.mModelMatrix = m.mModelMatrix;
		mShaderStack.setMatrices(m);
	}

	const gs::TextureComponent* tc = e->getConstTexture();
	unsigned int texUnitPushedFlags = 0;
	if (tc) {
		const TTextureId* tids = tc->getTextureIds();
		unsigned int usedFlags = tc->getUsedFlags();
		for (int i = 0; i < TextureComponent::MAX_TEXTURE_UNITS; ++i) {
			if (usedFlags & lookUpSet[i]) {
				TTextureId tid = tids[i];
				if (tid) {
					std::shared_ptr<Texture> tex = rm.getTextureByIdNumber(tid);
					if (tex) {
						mTextureStack.pushTexture(tex.get(), i);
						texUnitPushedFlags |= lookUpSet[i];
					}
				}
				else {
					mTextureStack.pushTexture(nullptr, i);
					texUnitPushedFlags |= lookUpSet[i];
				}
				if (!(usedFlags & lookUpInvertLimit[i])) {
					break;
				}
			}
		}
	}

	const ShaderComponent* sc = e->getConstShader();
	bool shaderPushed = false;
	if (sc) {
		TShaderId sid = sc->getShaderProgramId();
		if (sid) {
			std::shared_ptr<ShaderProgram> shader = rm.getShaderProgramByIdNumber(sid);
			if (shader) {
				mShaderStack.pushShaderProgram(shader.get());
				shaderPushed = true;
			}
		}
		else {
			mShaderStack.pushShaderProgram(nullptr);
			shaderPushed = true;
		}
	}

	if (e->getConstMesh()) {
		std::shared_ptr<Mesh> m = rm.getMeshByIdNumber(e->getConstMesh()->getGraphicId());
		if (m) {
			mTextureStack.bindForRendering();
			mShaderStack.bindForRendering();
			m->bind(mShaderStack.getCurrentBindedShader());
			m->draw();
			m->unbind(mShaderStack.getCurrentBindedShader());

			if (properties.mDrawNormals) {
				m->drawNormals();
			}
		}
	}
	if (e->getConstChildEntities()) {
		const gs::ChildEntities* children = e->getConstChildEntities();
		for (const std::shared_ptr<Entity>& child : children->getEntities()) {
			renderEntity(child, rm, properties);
		}
	}

	if (shaderPushed) {
		mShaderStack.popShaderProgram();
	}

	if (tc) {
		for (int i = 0; i < TextureComponent::MAX_TEXTURE_UNITS; ++i) {
			if (texUnitPushedFlags & lookUpSet[i]) {
				mTextureStack.popTexture(i);
				if (!(texUnitPushedFlags & lookUpInvertLimit[i])) {
					break;
				}
			}
		}
	}

	if (e->getConstTransform()) {
		if (properties.mUseGlTransforms) {
			glPopMatrix();
		}
		mMatrixStack.pop_back();
		mShaderStack.setMatrices(mMatrixStack.back());
	}
}
