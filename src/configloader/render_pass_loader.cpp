#include <configloader/render_pass_loader.h>
#include <rendering/render_pass.h>
#include <rendering/render_pass_manager.h>
#include <configloader/scene_loader.h>
#include <scene/scene_manager.h>
#include <res/resource_manager.h>
#include <ecs/transform_component.h>
#include <common/cfg.h>
#include <system/log.h>
#include <camera.h>
#include <glm/gtc/matrix_transform.hpp>

namespace gs
{
	namespace
	{
		// v = viewvector
		glm::vec3 getUpVector(glm::vec3 v)
		{
			v = glm::normalize(v);

			double xz = sqrt(v.x * v.x + v.z * v.z);
			if (xz > 1.0) {
				xz = 1.0;
			}
			double rad = acos(xz);
			double rad2 = M_PI/2.0 - rad;
			double xz2 = cos(rad2);
			double factor = xz2 / xz;

			glm::vec3 v2(v.x * factor, xz, v.z * factor);
			if (v.y > 0) {
				v2.x *= -1.0;
				v2.z *= -1.0;
			}
			return v2;
		}

		bool setViewMatrix(RenderPass& pass, const gs::CfgValuePair& cfgValue)
		{
			if (cfgValue.mName.mText != "view-matrix") {
				LOGE("no projection section! '%s'\n",
						cfgValue.mName.mText.c_str());
				return false;
			}
			const CfgValue* lookAt = nullptr;
			const CfgValue* transformCfg= nullptr;
			const CfgValue* cameraCfg = nullptr;
			CfgReadRule cfgRules[] = {
					CfgReadRule("look-at", &lookAt, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ALL),
					CfgReadRule("transform", &transformCfg, CfgReadRule::RULE_OPTIONAL),
					CfgReadRule("camera", &cameraCfg, CfgReadRule::RULE_OPTIONAL),
					CfgReadRule("")
			};
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.mValue.sectionGet(
					cfgRules, false, false, false, false, false, 0, &nextPos);
			if (storeCnt < 0) {
				LOGE("projection config is wrong, rv %d\n", int(storeCnt));
				return false;
			}
			if (cameraCfg) {
				if (cameraCfg->mArray.size() != 2 && cameraCfg->mArray.size() != 3) {
					LOGE("Wrong camera value format.\n");
					return false;
				}
				if (!cameraCfg->mArray[0].mValue.isBool()) {
					LOGE("First camera value must be a boolean.\n");
					return false;
				}
				if (cameraCfg->mArray[0].mValue.mBool) {
					if (transformCfg) {
						LOGE("camera can't be combined.\n");
						return false;
					}
					pass.mCamera = std::make_shared<Camera>();
					pass.mCamera->setSpeed(cameraCfg->mArray[1].mValue.mFloatingPoint);
					if (cameraCfg->mArray.size() >= 3) {
						pass.mCamera->setRotateDistance(cameraCfg->mArray[2].mValue.mFloatingPoint);
					}
				}
			}
			glm::mat4& viewMatrix = pass.mViewMatrix;
			if (lookAt) {
				size_t cnt = lookAt->mArray.size();
				if (cnt != 3 && cnt != 6 && cnt != 9) {
					LOGE("Wrong value format for look-at\n");
					return false;
				}
				glm::vec3 eye(lookAt->mArray[0].mValue.mFloatingPoint,
						lookAt->mArray[1].mValue.mFloatingPoint,
						lookAt->mArray[2].mValue.mFloatingPoint);
				glm::vec3 center(0.0f, 0.0f, 0.0f);
				if (cnt >= 6) {
					center.x = lookAt->mArray[3].mValue.mFloatingPoint;
					center.y = lookAt->mArray[4].mValue.mFloatingPoint;
					center.z = lookAt->mArray[5].mValue.mFloatingPoint;
				}
				glm::vec3 up(0.0f, 0.0f, 0.0f);
				if (cnt == 9) {
					up.x = lookAt->mArray[6].mValue.mFloatingPoint;
					up.y = lookAt->mArray[7].mValue.mFloatingPoint;
					up.z = lookAt->mArray[8].mValue.mFloatingPoint;
				}
				else {
					glm::vec3 viewVec = center - eye;
					up = getUpVector(viewVec);
				}
				viewMatrix = glm::lookAt(eye, center, up);
				if (pass.mCamera) {
					pass.mCamera->setLookAt(eye, center, up);
				}
			}
			if (transformCfg) {
				TransformComponent transform{std::weak_ptr<Entity>()};
				if (!sceneloader::addTransform(*transformCfg, transform)) {
					return false;
				}
				viewMatrix *= transform.getMatrix();
			}
			return true;
		}

		bool setProjection(Projection& projection, const gs::CfgValuePair& cfgValue)
		{
			if (cfgValue.mName.mText != "projection-matrix") {
				LOGE("no projection section! '%s'\n", cfgValue.mName.mText.c_str());
				return false;
			}
			const CfgValue* ortho = nullptr;
			const CfgValue* perspectiveDegree = nullptr;
			const CfgValue* perspective = nullptr;
			const CfgValue* frustum = nullptr;
			CfgReadRule cfgRules[] = {
					CfgReadRule("ortho", &ortho, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ALL),
					CfgReadRule("perspective", &perspective, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ALL),
					CfgReadRule("perspective-degree", &perspectiveDegree, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ALL),
					CfgReadRule("frustum", &frustum, CfgReadRule::RULE_OPTIONAL, CfgReadRule::ALLOW_ALL),
					CfgReadRule("")
			};
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.mValue.sectionGet(
					cfgRules, false, false, false, false, false, 0, &nextPos);
			if (storeCnt < 0) {
				LOGE("projection config is wrong, rv %d\n", int(storeCnt));
				return false;
			}
			if (ortho) {
				if (ortho->mText == "window-size") {
					projection.setOrthoWindowSize();
				}
				else if (ortho->mText == "window-size-center") {
					projection.setOrthoWindowSizeCenter();
				}
				else if (ortho->mArray.size() == 6 || ortho->mArray.size() == 7) {
					const std::vector<CfgValuePair>& v = ortho->mArray;
					float l = v[0].mValue.mFloatingPoint;
					float r = v[1].mValue.mFloatingPoint;
					float b = v[2].mValue.mFloatingPoint;
					float t = v[3].mValue.mFloatingPoint;
					float n = v[4].mValue.mFloatingPoint;
					float f = v[5].mValue.mFloatingPoint;
					if (ortho->mArray.size() == 6) {
						projection.setOrtho(l, r, b, t, n, f);
					}
					else {
						// --> size is 7
						std::string fitMode = v[6].mValue.mText;
						if (fitMode == "stretch") {
							projection.setOrtho(l, r, b, t, n, f);
						}
						else if (fitMode == "shrink-to-fit") {
							projection.setOrthoShrinkToFit(l, r, b, t, n, f);
						}
						else if (fitMode == "width-fit") {
							projection.setOrthoWidthFit(l, r, b, t, n, f);
						}
						else if (fitMode == "height-fit") {
							projection.setOrthoHeightFit(l, r, b, t, n, f);
						}
						else if (fitMode == "crop") {
							projection.setOrthoCrop(l, r, b, t, n, f);
						}
						else {
							LOGE("wrong value for ortho. '%s' is no format-mode\n", fitMode.c_str());
							return false;
						}
					}
				}
				else {
					LOGE("wrong value for ortho\n");
					return false;
				}
			}
			if (perspectiveDegree) {
				if (perspectiveDegree->mArray.size() == 4) {
					const std::vector<CfgValuePair>& v = perspectiveDegree->mArray;
					if (v[1].mValue.mText == "window-ratio") {
						// index 1 is not used. only 0, 2 and 3 because 1 is window-ratio
						projection.setPerspectiveDegreeWithWindowRatio(
								v[0].mValue.mFloatingPoint,
								v[2].mValue.mFloatingPoint,
								v[3].mValue.mFloatingPoint);
					}
					else {
						projection.setPerspectiveDegree(
								v[0].mValue.mFloatingPoint,
								v[1].mValue.mFloatingPoint,
								v[2].mValue.mFloatingPoint,
								v[3].mValue.mFloatingPoint);
					}
				}
				else {
					LOGE("wrong value for perspective-degree\n");
					return false;
				}
			}
			if (perspective) {
				if (perspective->mArray.size() == 4) {
					const std::vector<CfgValuePair>& v = perspective->mArray;
					if (v[1].mValue.mText == "window-ratio") {
						// index 1 is not used. only 0, 2 and 3 because 1 is window-ratio
						projection.setPerspectiveWithWindowRatio(
								v[0].mValue.mFloatingPoint,
								v[2].mValue.mFloatingPoint,
								v[3].mValue.mFloatingPoint);
					}
					else {
						projection.setPerspective(
								v[0].mValue.mFloatingPoint,
								v[1].mValue.mFloatingPoint,
								v[2].mValue.mFloatingPoint,
								v[3].mValue.mFloatingPoint);
					}
				}
				else {
					LOGE("wrong value for perspective\n");
					return false;
				}
			}
			if (frustum) {
				if (frustum->mArray.size() == 6) {
					const std::vector<CfgValuePair>& v = frustum->mArray;
					projection.setFrustum(
							v[0].mValue.mFloatingPoint,
							v[1].mValue.mFloatingPoint,
							v[2].mValue.mFloatingPoint,
							v[3].mValue.mFloatingPoint,
							v[4].mValue.mFloatingPoint,
							v[5].mValue.mFloatingPoint);
				}
				else {
					LOGE("wrong value for frustum\n");
					return false;
				}
			}
			return true;
		}

		bool addRenderPass(gs::RenderPassManager &pm,
				const gs::SceneManager &sm, const gs::ResourceManager &rm,
				const gs::CfgValuePair &cfgValue)
		{
			if (cfgValue.mName.mText != "render-pass") {
				LOGE("no render-pass section!\n");
				return false;
			}
			std::string fbIdName;
			std::string sceneIdName;
			const CfgValue* clearColor;
			const CfgValuePair* projectionCfg = nullptr;
			const CfgValuePair* viewCfg = nullptr;
			bool depthTest = false;
			CfgReadRule cfgRules[] = {
					CfgReadRule("framebuffer-id", &fbIdName, CfgReadRule::RULE_MUST_EXIST),
					CfgReadRule("clear-color", &clearColor, CfgReadRule::RULE_MUST_EXIST),
					CfgReadRule("projection-matrix", &projectionCfg, CfgReadRule::RULE_OPTIONAL),
					CfgReadRule("view-matrix", &viewCfg, CfgReadRule::RULE_OPTIONAL),
					CfgReadRule("scene-id", &sceneIdName, CfgReadRule::RULE_MUST_EXIST),
					CfgReadRule("depth-test", &depthTest, CfgReadRule::RULE_OPTIONAL),
					CfgReadRule("")
			};

			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.mValue.sectionGet(
					cfgRules, false, false, false, false, false, 0, &nextPos);
			if (storeCnt < 0) {
				LOGE("render-pass config is wrong\n");
				return false;
			}
			TFramebufferId fbId = 0;
			if (fbIdName == "default") {
				if (rm.getResourceId("default")) {
					LOGE("'default' is used by a resource for its id name. But its used by the default framebuffer (window/screen)\n");
					return false;
				}
				fbId = 0; // for binding default framebuffer
			}
			else {
				fbId = rm.getResourceId(fbIdName);
				if (!fbId) {
					LOGE("Can't find id name '%s' at resource manager for framebuffer.\n", fbIdName.c_str());
					return false;
				}
			}
			if (!clearColor) {
				LOGE("No clear-color is set.\n");
				return false;
			}
			if (clearColor->mArray.size() != 4) {
				LOGE("clear-color need four values for rgba.\n");
				return false;
			}
			Color col(clearColor->mArray[0].mValue.mFloatingPoint,
					clearColor->mArray[1].mValue.mFloatingPoint,
					clearColor->mArray[2].mValue.mFloatingPoint,
					clearColor->mArray[3].mValue.mFloatingPoint);
			unsigned int sceneId = sm.getSceneIdByIdName(sceneIdName);
			if (!sceneId) {
				LOGE("scene with id name '%s' can't be found.\n", sceneIdName.c_str());
				return false;
			}

			RenderPass pass(fbId, col, sceneId);
			if (projectionCfg) {
				if (!setProjection(pass.mProjection, *projectionCfg)) {
					LOGE("set projection failed.\n");
					return false;
				}
			}
			if (viewCfg) {
				if (!setViewMatrix(pass, *viewCfg)) {
					LOGE("set view failed.\n");
					return false;
				}
			}
			pass.mDepthTest = depthTest;
			pm.addPass(pass);
			return true;
		}

		bool addRenderPassesFromRenderingSection(gs::RenderPassManager &pm,
				const gs::SceneManager &sm, const gs::ResourceManager &rm,
				const gs::CfgValuePair &cfgValue)
		{
			if (cfgValue.mName.mText != "rendering") {
				return false;
			}
			int nr = 1;
			bool rv = true;
			int successCount = 0;
			for (const CfgValuePair& vp : cfgValue.mValue.mArray) {
				if (!addRenderPass(pm, sm, rm, vp)) {
					LOGE("Can't add the %d. renderpass to render pass manager.\n", nr);
					rv = false;
				}
				else {
					++successCount;
				}
				++nr;
			}
			LOGI("Load %d renderpasses\n", successCount);
			return rv;
		}
	}
}

bool gs::renderpassloader::addRenderPasses(gs::RenderPassManager &pm,
		const gs::SceneManager &sm, const gs::ResourceManager &rm,
		const gs::CfgValuePair &cfg)
{
	if (!cfg.mValue.isArray()) {
		return false;
	}
	if (cfg.mName.mText != "rendering") {
		return false;
	}
	if (!addRenderPassesFromRenderingSection(pm, sm, rm, cfg)) {
		LOGE("Add passes from rendering failed.\n");
		return false;
	}
	return true;
}
