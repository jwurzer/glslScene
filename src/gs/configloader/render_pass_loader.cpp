#include <gs/configloader/render_pass_loader.h>
#include <gs/rendering/render_pass.h>
#include <gs/rendering/render_pass_manager.h>
#include <gs/configloader/scene_loader.h>
#include <gs/scene/scene_manager.h>
#include <gs/res/resource_manager.h>
#include <gs/ecs/transform_component.h>
#include <gs/system/log.h>
#include <gs/camera.h>
#include <cfg/cfg.h>
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

		bool setViewMatrix(RenderPass& pass, const cfg::NameValuePair& cfgValue)
		{
			if (cfgValue.mName.mText != "view-matrix") {
				LOGE("no view section! '%s'\n",
						cfgValue.mName.mText.c_str());
				return false;
			}
			const cfg::Value* lookAt = nullptr;
			const cfg::Value* transformCfg= nullptr;
			const std::vector<cfg::Value>* cameraCfg = nullptr;
			cfg::SelectRule cfgRules[] = {
					cfg::SelectRule("look-at", &lookAt, cfg::SelectRule::RULE_OPTIONAL, cfg::SelectRule::ALLOW_ALL),
					cfg::SelectRule("transform", &transformCfg, cfg::SelectRule::RULE_OPTIONAL),
					cfg::SelectRule("camera", &cameraCfg, cfg::SelectRule::RULE_OPTIONAL),
					cfg::SelectRule("")
			};
			std::string errMsg;
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.mValue.objectGet(
					cfgRules, false, false, false, false, false, 0, &nextPos,
					cfg::EReset::RESET_POINTERS_TO_NULL, &errMsg);
			if (storeCnt < 0) {
				LOGE("%s: view config is wrong. rv = %zd, err msg: %s\n",
						cfgValue.mValue.getFilenameAndPosition().c_str(),
						storeCnt, errMsg.c_str());
				return false;
			}
			if (cameraCfg) {
				const std::vector<cfg::Value>& camArray = *cameraCfg;
				if (camArray.size() != 2 && camArray.size() != 3) {
					LOGE("Wrong camera value format. %zu\n", camArray.size());
					return false;
				}
				if (!camArray[0].isBool()) {
					LOGE("First camera value must be a boolean.\n");
					return false;
				}
				if (camArray[0].mBool) {
					if (transformCfg) {
						LOGE("camera can't be combined.\n");
						return false;
					}
					pass.mCamera = std::make_shared<Camera>();
					pass.mCamera->setSpeed(camArray[1].mFloatingPoint);
					if (camArray.size() >= 3) {
						pass.mCamera->setRotateDistance(camArray[2].mFloatingPoint);
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
				glm::vec3 eye(lookAt->mArray[0].mFloatingPoint,
						lookAt->mArray[1].mFloatingPoint,
						lookAt->mArray[2].mFloatingPoint);
				glm::vec3 center(0.0f, 0.0f, 0.0f);
				if (cnt >= 6) {
					center.x = lookAt->mArray[3].mFloatingPoint;
					center.y = lookAt->mArray[4].mFloatingPoint;
					center.z = lookAt->mArray[5].mFloatingPoint;
				}
				glm::vec3 up(0.0f, 0.0f, 0.0f);
				if (cnt == 9) {
					up.x = lookAt->mArray[6].mFloatingPoint;
					up.y = lookAt->mArray[7].mFloatingPoint;
					up.z = lookAt->mArray[8].mFloatingPoint;
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

		bool setProjection(Projection& projection, const cfg::NameValuePair& cfgValue)
		{
			if (cfgValue.mName.mText != "projection-matrix") {
				LOGE("no projection section! '%s'\n", cfgValue.mName.mText.c_str());
				return false;
			}
			const cfg::Value* ortho = nullptr;
			const cfg::Value* perspectiveDegree = nullptr;
			const cfg::Value* perspective = nullptr;
			const cfg::Value* frustum = nullptr;
			cfg::SelectRule cfgRules[] = {
					cfg::SelectRule("ortho", &ortho, cfg::SelectRule::RULE_OPTIONAL, cfg::SelectRule::ALLOW_ALL),
					cfg::SelectRule("perspective", &perspective, cfg::SelectRule::RULE_OPTIONAL, cfg::SelectRule::ALLOW_ALL),
					cfg::SelectRule("perspective-degree", &perspectiveDegree, cfg::SelectRule::RULE_OPTIONAL, cfg::SelectRule::ALLOW_ALL),
					cfg::SelectRule("frustum", &frustum, cfg::SelectRule::RULE_OPTIONAL, cfg::SelectRule::ALLOW_ALL),
					cfg::SelectRule("")
			};
			std::string errMsg;
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.mValue.objectGet(
					cfgRules, false, false, false, false, false, 0, &nextPos,
					cfg::EReset::RESET_POINTERS_TO_NULL, &errMsg);
			if (storeCnt < 0) {
				LOGE("%s: projection config is wrong. rv = %zd, err msg: %s\n",
						cfgValue.mValue.getFilenameAndPosition().c_str(),
						storeCnt, errMsg.c_str());
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
					const std::vector<cfg::Value>& v = ortho->mArray;
					float l = v[0].mFloatingPoint;
					float r = v[1].mFloatingPoint;
					float b = v[2].mFloatingPoint;
					float t = v[3].mFloatingPoint;
					float n = v[4].mFloatingPoint;
					float f = v[5].mFloatingPoint;
					if (ortho->mArray.size() == 6) {
						projection.setOrtho(l, r, b, t, n, f);
					}
					else {
						// --> size is 7
						std::string fitMode = v[6].mText;
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
					const std::vector<cfg::Value>& v = perspectiveDegree->mArray;
					if (v[1].mText == "window-ratio") {
						// index 1 is not used. only 0, 2 and 3 because 1 is window-ratio
						projection.setPerspectiveDegreeWithWindowRatio(
								v[0].mFloatingPoint,
								v[2].mFloatingPoint,
								v[3].mFloatingPoint);
					}
					else {
						projection.setPerspectiveDegree(
								v[0].mFloatingPoint,
								v[1].mFloatingPoint,
								v[2].mFloatingPoint,
								v[3].mFloatingPoint);
					}
				}
				else {
					LOGE("wrong value for perspective-degree\n");
					return false;
				}
			}
			if (perspective) {
				if (perspective->mArray.size() == 4) {
					const std::vector<cfg::Value>& v = perspective->mArray;
					if (v[1].mText == "window-ratio") {
						// index 1 is not used. only 0, 2 and 3 because 1 is window-ratio
						projection.setPerspectiveWithWindowRatio(
								v[0].mFloatingPoint,
								v[2].mFloatingPoint,
								v[3].mFloatingPoint);
					}
					else {
						projection.setPerspective(
								v[0].mFloatingPoint,
								v[1].mFloatingPoint,
								v[2].mFloatingPoint,
								v[3].mFloatingPoint);
					}
				}
				else {
					LOGE("wrong value for perspective\n");
					return false;
				}
			}
			if (frustum) {
				if (frustum->mArray.size() == 6) {
					const std::vector<cfg::Value>& v = frustum->mArray;
					projection.setFrustum(
							v[0].mFloatingPoint,
							v[1].mFloatingPoint,
							v[2].mFloatingPoint,
							v[3].mFloatingPoint,
							v[4].mFloatingPoint,
							v[5].mFloatingPoint);
				}
				else {
					LOGE("wrong value for frustum\n");
					return false;
				}
			}
			return true;
		}

		bool addRenderPass(RenderPassManager &pm,
				const SceneManager &sm, const ResourceManager &rm,
				const cfg::NameValuePair &cfgValue)
		{
			if (cfgValue.mName.mText != "render-pass") {
				LOGE("no render-pass section!\n");
				return false;
			}
			std::string fbIdName;
			std::string sceneIdName;
			const cfg::Value* clearColor;
			const cfg::NameValuePair* projectionCfg = nullptr;
			const cfg::NameValuePair* viewCfg = nullptr;
			bool depthTest = false;
			cfg::SelectRule cfgRules[] = {
					cfg::SelectRule("framebuffer-id", &fbIdName, cfg::SelectRule::RULE_MUST_EXIST),
					cfg::SelectRule("clear-color", &clearColor, cfg::SelectRule::RULE_MUST_EXIST, cfg::SelectRule::ALLOW_ARRAY),
					cfg::SelectRule("projection-matrix", &projectionCfg, cfg::SelectRule::RULE_OPTIONAL),
					cfg::SelectRule("view-matrix", &viewCfg, cfg::SelectRule::RULE_OPTIONAL),
					cfg::SelectRule("scene-id", &sceneIdName, cfg::SelectRule::RULE_MUST_EXIST),
					cfg::SelectRule("depth-test", &depthTest, cfg::SelectRule::RULE_OPTIONAL),
					cfg::SelectRule("")
			};

			std::string errMsg;
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValue.mValue.objectGet(
					cfgRules, false, false, false, false, false, 0, &nextPos,
					cfg::EReset::RESET_POINTERS_TO_NULL, &errMsg);
			if (storeCnt < 0) {
				LOGE("%s: render-pass config is wrong. rv = %zd, err msg: %s\n",
						cfgValue.mValue.getFilenameAndPosition().c_str(),
						storeCnt, errMsg.c_str());
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
			Color col(clearColor->mArray[0].mFloatingPoint,
					clearColor->mArray[1].mFloatingPoint,
					clearColor->mArray[2].mFloatingPoint,
					clearColor->mArray[3].mFloatingPoint);
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

		bool addRenderPassesFromRenderingSection(RenderPassManager &pm,
				const SceneManager &sm, const ResourceManager &rm,
				const cfg::NameValuePair &cfgValue)
		{
			if (cfgValue.mName.mText != "rendering") {
				return false;
			}
			int nr = 1;
			bool rv = true;
			int successCount = 0;
			for (const cfg::NameValuePair& vp : cfgValue.mValue.mObject) {
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
		const cfg::NameValuePair &cfg)
{
	if (!cfg.mValue.isObject()) {
		LOGE("%s: No object for rendering.\n",
				cfg.mValue.getFilenameAndPosition().c_str());
		return false;
	}
	if (cfg.mName.mText != "rendering") {
		LOGE("%s: No rendering keyword.\n",
				cfg.mName.getFilenameAndPosition().c_str());
		return false;
	}
	if (!addRenderPassesFromRenderingSection(pm, sm, rm, cfg)) {
		LOGE("Add passes from rendering failed.\n");
		return false;
	}
	return true;
}
