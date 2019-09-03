#include <gs/gui/gui_manager.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl2.h>
#include <imgui/imgui_impl_opengl3.h>

#include <gs/context_properties.h>

#include <gs/res/resource.h>
#include <gs/res/texture.h>
#include <gs/res/shader_program.h>
#include <gs/res/mesh.h>
#include <gs/res/framebuffer.h>
#include <gs/res/resource_manager.h>
#include <gs/res/uniform.h>

#include <gs/scene/scene.h>
#include <gs/scene/scene_manager.h>

#include <gs/ecs/entity.h>
#include <gs/ecs/transform_component.h>
#include <gs/ecs/shader_component.h>
#include <gs/ecs/mesh_component.h>
#include <gs/ecs/child_entities.h>

#include <gs/rendering/renderer.h>
#include <gs/rendering/render_pass.h>
#include <gs/rendering/render_pass_manager.h>

#include <gs/system/file_change_monitoring.h>
#include <gs/system/log.h>
#include <gs/common/fs.h>

#include <glm/gtc/type_ptr.hpp>

#ifdef _MSC_VER
#define strncpy(dst, src, n) strncpy_s(dst, n, src, strlen(src))
#endif

namespace gs
{
	namespace
	{
		void IntentTextV(const char* fmt, va_list args)
		{
			//ImGui::BulletTextV(fmt, args);
			//ImGui::Bullet();
			ImGui::Indent();
			ImGui::TextV(fmt, args);
			ImGui::Unindent();
		}

		void IntentText(const char* fmt, ...)
		{
			va_list args;
			va_start(args, fmt);
			IntentTextV(fmt, args);
			va_end(args);
		}

		void addMatrix(const glm::mat4& m)
		{
			char matStr[256];
			snprintf(matStr, 256,
					"%f %f %f %f\n"
					"%f %f %f %f\n"
					"%f %f %f %f\n"
					"%f %f %f %f\n",
					m[0][0],
					m[1][0],
					m[2][0],
					m[3][0],
					m[0][1],
					m[1][1],
					m[2][1],
					m[3][1],
					m[0][2],
					m[1][2],
					m[2][2],
					m[3][2],
					m[0][3],
					m[1][3],
					m[2][3],
					m[3][3]);
			IntentText(matStr);
#if 0 // this would be the same (interpreted as an one dim. array)
			float* f = glm::value_ptr(m);
			snprintf(matStr, 256,
					"%f %f %f %f\n"
					"%f %f %f %f\n"
					"%f %f %f %f\n"
					"%f %f %f %f\n",
					f[0],
					f[4],
					f[8],
					f[12],

					f[1],
					f[5],
					f[9],
					f[13],

					f[2],
					f[6],
					f[10],
					f[14],

					f[3],
					f[7],
					f[11],
					f[15]
			);
			IntentText(matStr);
#endif
		}

		void addTransformCompToMenu(const TransformComponent& tc)
		{
			if (!ImGui::TreeNode("transform-component"))
			{
				return; // no ImGui::TreePop() here!
			}
			addMatrix(tc.getMatrix());
			ImGui::TreePop();
		}

		void addShaderCompToMenu(const ShaderComponent& sc)
		{
			if (!ImGui::TreeNode("shader-component"))
			{
				return; // no ImGui::TreePop() here!
			}
			IntentText("shader-id (number): %u", sc.getShaderProgramId());
			ImGui::TreePop();
		}

		void addMeshCompToMenu(const MeshComponent& mc)
		{
			if (!ImGui::TreeNode("mesh-component"))
			{
				return; // no ImGui::TreePop() here!
			}
			IntentText("mesh-id (number): %u", mc.getGraphicId());

			ImGui::TreePop();
		}

		void addEntityToMenu(const std::shared_ptr<Entity>& e, unsigned int index)
		{
			if (!e) {
				return;
			}

			char strId[32];
			snprintf(strId, 32, "entitiy_%u", index);
			if (!ImGui::TreeNode(strId, "entity"))
			{
				return; // no ImGui::TreePop() here!
			}

			IntentText("active: %s", e->isActive() ? "true" : "false");

			const TransformComponent* tc = e->getConstTransform();
			if (tc) {
				addTransformCompToMenu(*tc);
			}

			const ShaderComponent* sc = e->getConstShader();
			if (sc) {
				addShaderCompToMenu(*sc);
			}

			const MeshComponent* mc = e->getConstMesh();
			if (mc) {
				addMeshCompToMenu(*mc);
			}

			const ChildEntities* ce = e->getConstChildEntities();
			if (ce) {
				unsigned int i = 0;
				for (const auto& child : ce->getEntities()) {
					addEntityToMenu(child, i);
					++i;
				}
			}
			ImGui::TreePop();
		}

		void addSceneToMenu(const Scene& scene)
		{
			char strId[32];
			snprintf(strId, 32, "scene_%u", scene.getIdNumber());
			const char* idName = scene.getIdName().c_str();
			if (!ImGui::TreeNode(strId, "scene - id: %u%s%s",
					scene.getIdNumber(), idName[0] ? ", " : "", idName)) {
				return; // no ImGui::TreePop() here!
			}

			IntentText("id (number): %u", scene.getIdNumber());
			IntentText("id (name): %s", scene.getIdName().c_str());

			addEntityToMenu(scene.getRootOe(), 0);

			ImGui::TreePop();
		}

		bool addResToMenu(const Resource& res, const char* resTypeName)
		{
			char strId[32];
			snprintf(strId, 32, "res_%u", res.getIdNumber());
			const char* idName = res.getIdName().c_str();
			if (!ImGui::TreeNode(strId, "%s - id: %u%s%s", resTypeName,
					res.getIdNumber(), idName[0] ? ", " : "", idName)) {
				return false; // no ImGui::TreePop() here!
			}
			IntentText("id (number): %u", res.getIdNumber());
			IntentText("id (name): %s", res.getIdName().c_str());
			// no ImGui::TreePop() here! --> must be done by the caller of addResToMenu()
			return true;
		}

		void addTextureResToMenu(const Texture& tex)
		{
			if (!addResToMenu(tex, "texture")) {
				return; // no ImGui::TreePop() here!
			}
			IntentText("filename: %s", tex.getFilename().c_str());
			IntentText("resolution: %ux%u", tex.getWidth(), tex.getHeight());
			IntentText("mipmap: %s", (tex.getMipmap() == TexMipmap::MIPMAP) ? "true" : "false");
			IntentText("minification filter: %s",
					(tex.getMinFilter() == TexFilter::LINEAR) ? "linear" : "nearest");
			IntentText("magnification filter: %s",
					(tex.getMagFilter() == TexFilter::LINEAR) ? "linear" : "nearest");
			ImGui::TreePop();
		}

		const char* getShaderTypeStr(ShaderType type)
		{
			switch (type) {
				case ShaderType::COMPUTE_SHADER:
					return "compute";
				case ShaderType::VERTEX_SHADER:
					return "vertex";
				case ShaderType::TESS_CONTROL_SHADER:
					return "tess-control";
				case ShaderType::TESS_EVALUATION_SHADER:
					return "tess-evaluation";
				case ShaderType::GEOMETRY_SHADER:
					return "geometry";
				case ShaderType::FRAGMENT_SHADER:
					return "fragment";
			}
			return "unknown";
		}

		const char* getUniformTypeStr(UniformType type)
		{
			switch (type) {
				case UniformType::INVALID:
					return "invalid";
				case UniformType::INT:
					return "int";
				case UniformType::FLOAT:
					return "float";
				case UniformType::VEC2:
					return "vec2";
				case UniformType::VEC3:
					return "vec3";
				case UniformType::VEC4:
					return "vec4";
				case UniformType::SAMPLER2D:
					return "sampler2D";
				case UniformType::MAT4X4:
					return "mat4";
			}
			return "unknown";
		}

		void getUniformValue(const Uniform& u, char* outValueStr, unsigned int maxLen)
		{
			outValueStr[0] = '\0';
			if (u.mInverse) {
				if (maxLen < 9) { // 9 = 8 + incl \0
					return;
				}
				strncpy(outValueStr, "inverse-", maxLen);
				maxLen -= 8; // without \0
				outValueStr += 8;
			}
			switch (u.mSource) {
				// no value used
				case UniformSource::INVALID:
					strncpy(outValueStr, "invalid", maxLen);
					break;
				// no value used
				case UniformSource::ABSOLUTE_TIME_SEC:
					strncpy(outValueStr, "time", maxLen);
					break;
				// using mFloat to store the reference time as seconds (time relative to start time)
				case UniformSource::RELATIVE_TIME_SEC:
					strncpy(outValueStr, "relative-time", maxLen);
					break;
				// no value used
				case UniformSource::DELTA_TIME_SEC:
					strncpy(outValueStr, "delta-time", maxLen);
					break;

				// no value used
				case UniformSource::MOUSE_POS_FACTOR:
					strncpy(outValueStr, "mouse-pos-factor", maxLen);
					break;
				// no value used
				case UniformSource::MOUSE_POS_PIXEL:
					strncpy(outValueStr, "mouse-pos-pixel", maxLen);
					break;

				// no value used
				case UniformSource::VIEWPORT_POS_PIXEL:
					strncpy(outValueStr, "viewport-pos-pixel", maxLen);
					break;
				// no value used
				case UniformSource::VIEWPORT_SIZE_PIXEL:
					strncpy(outValueStr, "viewport-size-pixel", maxLen);
					break;

				// no value used
				case UniformSource::VIEW_SIZE:
					strncpy(outValueStr, "view-size", maxLen);
					break;
				// no value used
				case UniformSource::VIEW_RATIO:
					strncpy(outValueStr, "view-ratio", maxLen);
					break;

				// no value used
				case UniformSource::PROJECTION_MATRIX:
					strncpy(outValueStr, "projection-matrix", maxLen);
					break;
				// no value used
				case UniformSource::VIEW_MATRIX:
					strncpy(outValueStr, "view-matrix", maxLen);
					break;
				// no value used
				case UniformSource::MODEL_MATRIX:
					strncpy(outValueStr, "model-matrix", maxLen);
					break;
				// no value used
				case UniformSource::MODEL_VIEW_MATRIX:
					strncpy(outValueStr, "model-view-matrix", maxLen);
					break;
				// no value used
				case UniformSource::ENTITY_MATRIX:
					strncpy(outValueStr, "entity-matrix", maxLen);
					break;
				// no value used
				case UniformSource::MVP_MATRIX:
					strncpy(outValueStr, "mvp-matrix", maxLen);
					break;

				// value is used (which one depends on UniformType)
				case UniformSource::CUSTOM_VALUE:
					break;
			}

			if (u.mSource != UniformSource::CUSTOM_VALUE) {
				return;
			}

			// --> its a custom value
			switch (u.mType) {
				case UniformType::INVALID:
					strncpy(outValueStr, "custom, invalid", maxLen);
					break;
				case UniformType::INT:
					snprintf(outValueStr, maxLen, "%i", u.mValue.mInt);
					break;
				case UniformType::FLOAT:
					snprintf(outValueStr, maxLen, "%f", u.mValue.mFloat);
					break;
				case UniformType::VEC2:
					snprintf(outValueStr, maxLen, "%f %f",
							u.mValue.mVec2.x, u.mValue.mVec2.y);
					break;
				case UniformType::VEC3:
					snprintf(outValueStr, maxLen, "%f %f %f",
							u.mValue.mVec3.x, u.mValue.mVec3.y, u.mValue.mVec3.z);
					break;
				case UniformType::VEC4:
					snprintf(outValueStr, maxLen, "%f %f %f %f",
							u.mValue.mVec4.x, u.mValue.mVec4.y, u.mValue.mVec4.z, u.mValue.mVec4.w);
					break;
				case UniformType::SAMPLER2D:
					snprintf(outValueStr, maxLen, "%i", u.mValue.mInt);
					break;
				case UniformType::MAT4X4:
				{
					const float* f = u.mValue.mMat4.m;
					snprintf(outValueStr, maxLen,
							"\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
							f[0], f[4], f[8], f[12],
							f[1], f[5], f[9], f[13],
							f[2], f[6], f[10], f[14],
							f[3], f[7], f[11], f[15]);
					break;
				}
			}
		}

		void addUniformEdit(Uniform& u)
		{
			if (u.mSource != UniformSource::CUSTOM_VALUE) {
				return;
			}
			const float percent = 0.001f;
			const float minLimit = 0.001f;
			ImGuiColorEditFlags flags = ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar;
			ImGui::Indent();
			switch (u.mType) {
				case UniformType::SAMPLER2D:
				{
					//ImGuiDataType_S32 is for int!
					//ImS32 is a typedef of int!
					int s32_one = 1;
					ImGui::InputScalar(u.mName.c_str(), ImGuiDataType_S32, &u.mValue.mInt, &s32_one, NULL, "%d");
					break;
				}
				case UniformType::INT:
				{
					ImGui::DragInt(u.mName.c_str(), &u.mValue.mInt, 0.5f);
					break;
				}
				case UniformType::FLOAT:
				{
					float value = fabs(u.mValue.mFloat);
					float speed = value * percent;
					if (speed < minLimit) {
						speed = minLimit;
					}
					ImGui::DragFloat(u.mName.c_str(), &u.mValue.mFloat, speed);
					break;
				}
				case UniformType::VEC2:
				{
					float value = fmax(fabs(u.mValue.mVec2.x), fabs(u.mValue.mVec2.y));
					float speed = fabs(value * percent);
					if (speed < minLimit) {
						speed = minLimit;
					}
					ImGui::DragFloat2(u.mName.c_str(), &u.mValue.mVec2.x, speed);
					break;
				}
				case UniformType::VEC3:
				{
					float value = fmax(fmax(fabs(u.mValue.mVec3.x), fabs(u.mValue.mVec3.y)),
							fabs(u.mValue.mVec3.y));
					float speed = fabs(value * percent);
					if (speed < minLimit) {
						speed = minLimit;
					}
					ImGui::DragFloat3(u.mName.c_str(), &u.mValue.mVec3.x, speed);
					ImGui::ColorEdit3((u.mName + " (col)").c_str(), &u.mValue.mVec3.x, flags);
					break;
				}
				case UniformType::VEC4:
				{
					float value = fmax(fmax(fabs(u.mValue.mVec4.x), fabs(u.mValue.mVec4.y)),
							fmax(fabs(u.mValue.mVec4.z), fabs(u.mValue.mVec4.w)));
					float speed = fabs(value * percent);
					if (speed < minLimit) {
						speed = minLimit;
					}
					ImGui::DragFloat4(u.mName.c_str(), &u.mValue.mVec4.x, speed);
					ImGui::ColorEdit4((u.mName + " (col)").c_str(), &u.mValue.mVec4.x, flags);
					break;
				}
				default:
					// matrix currently not supported
					break;
			}
			ImGui::Unindent();
		}

		void addShaderProgResToMenu(ShaderProgram& shader)
		{
			if (!addResToMenu(shader, "shader")) {
				return; // no ImGui::TreePop() here!
			}
			const std::vector<ShaderLoadInfo>& loadInfos = shader.getShaderLoadInfos();
			for (const auto& i : loadInfos) {
				IntentText("%s: %s: %s", getShaderTypeStr(i.getShaderType()),
						i.isFilename() ? "file" : "direct-source",
						i.isFilename() ? i.getFilenameOrSource().c_str() : "...");
			}
			std::vector<Uniform>& uniforms = shader.getUniforms();
			char valueStr[256];
			for (auto& u : uniforms) {
				getUniformValue(u, valueStr, 256);
				IntentText("uniform %s %s: value: %s, location: %d", getUniformTypeStr(u.mType),
						u.mName.c_str(), valueStr, u.mLocation);
				addUniformEdit(u);
			}
			const std::vector<Attribute>& attrs = shader.getAttributes();
			for (const auto& a : attrs) {
				IntentText("attribute %s: offset: %u, count: %u, location: %d",
						a.mName.c_str(), a.mCompOffset, a.mCompCount, a.mLocation);
			}
			ImGui::TreePop();
		}

		void addMeshResToMenu(const Mesh& mesh)
		{
			if (!addResToMenu(mesh, "mesh")) {
				return; // no ImGui::TreePop() here!
			}
			IntentText("primitive type: %s", mesh.getPrimitiveTypeAsString());
			IntentText("vertex: count: %u", mesh.getVertexCount());
			if (ImGui::TreeNode("vertices:")) {
				const std::vector<std::string>& vertexStrings = mesh.verticesToStrings();
				size_t vCnt = vertexStrings.size();
				for (size_t vi = 0; vi < vCnt; ++vi) {
					IntentText("%5zu: %s", vi, vertexStrings[vi].c_str());
				}
				ImGui::TreePop();
			}
			IntentText("has normals: %s", mesh.hasNormals() ? "yes" : "no");
			ImGui::TreePop();
		}

		void addFramebufferResToMenu(const Framebuffer& fb)
		{
			if (!addResToMenu(fb, "framebuffer")) {
				return; // no ImGui::TreePop() here!
			}
			IntentText("resolution: %ux%u", fb.getWidth(), fb.getHeight());
			ImGui::TreePop();
		}

		void addConfiguredContextToMenu(const ContextProperties& cp)
		{
			if (!ImGui::TreeNode("configured")) {
				return; // no ImGui::TreePop() here!
			}
			IntentText("rendering api: %s", cp.getRenderApiAsStr());
			IntentText("profile: %s", cp.getRenderApiProfileAsStr());
			IntentText("forward compatibility: %s", cp.getForwardCompatibilityAsStr());

			char verStr[32];
			cp.getVersion(verStr, 32);
			IntentText("version: %s", verStr);

			ImGui::TreePop();
		}

		void addCreatedContextToMenu(const ContextProperties& cp)
		{
			if (!ImGui::TreeNode("created")) {
				return; // no ImGui::TreePop() here!
			}
			IntentText("gl version: %s", cp.mCreatedGlVersion.c_str());
			IntentText("glsl version: %s", cp.mCreatedGlslVersion.c_str());
			IntentText("vendor: %s", cp.mCreatedVendor.c_str());
			IntentText("renderer: %s", cp.mCreatedRenderer.c_str());

			char verStr[32];
			cp.getCreatedVersion(verStr, 32);
			IntentText("version: %s", verStr);

			ImGui::TreePop();
		}

		void addContextToMenu(const ContextProperties& cp)
		{
			addConfiguredContextToMenu(cp);
			addCreatedContextToMenu(cp);
		}

		void addFileChangeMonitoringToMenu(const FileChangeMonitoring& fcm)
		{
			if (ImGui::TreeNode("sorted by watch id")) {
				fcm.lock();
				char strId[32];
				for (const auto& it : fcm.getFilesByWatchId()) {
					snprintf(strId, 32, "watch-id_%ld", long(it.first));
					std::string wn = (it.second->getWatchnames().size() <= 1) ? "dir:" : "dirs:";
					for (const auto& watchname : it.second->getWatchnames()) {
						wn += " ";
						wn += watchname;
					}
					if (ImGui::TreeNode(strId, "watch id %ld, %s, files: %zu",
							long(it.first), wn.c_str(),
							it.second->getWatchIdCount())) {
						for (const auto& itName : it.second->getNames()) {
							const FileChangeMonitoring::Filename* fn = &itName.second;
							std::string cbIds = (fn->mCallbacks.size() <= 1) ?
									"cb id: " : "cb ids: ";
							bool isFirst = true;
							unsigned int reload = 0;
							for (const auto& cb : fn->mCallbacks) {
								if (isFirst) {
									isFirst = false;
								}
								else {
									cbIds += ", ";
								}
								cbIds += std::to_string(cb.first);
								reload += cb.second.mCallCount;
							}
							IntentText("%s, cb count: %zu, %s, reload: %u",
									itName.first.c_str(), fn->mCallbacks.size(),
									cbIds.c_str(), reload);
						}
						ImGui::TreePop();
					}
				}
				fcm.unlock();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("sorted by watch dir")) {
				fcm.lock();
				char strId[32];
				unsigned int i = 0;
				for (const auto& it : fcm.getFilesByWatchDir()) {
					snprintf(strId, 32, "watch-dir_%d", i);
					if (ImGui::TreeNode(strId, "watch dir: %s, watch id %ld, files: %zu",
							it.first.c_str(), long(it.second->getWatchId()),
							it.second->getWatchIdCount())) {
						for (const auto& itName : it.second->getNames()) {
							const FileChangeMonitoring::Filename* fn = &itName.second;
							std::string cbIds = (fn->mCallbacks.size() <= 1) ?
									"cb id: " : "cb ids: ";
							bool isFirst = true;
							unsigned int reload = 0;
							for (const auto& cb : fn->mCallbacks) {
								if (isFirst) {
									isFirst = false;
								}
								else {
									cbIds += ", ";
								}
								cbIds += std::to_string(cb.first);
								reload += cb.second.mCallCount;
							}
							IntentText("%s, cb count: %zu, %s, reload: %u",
									itName.first.c_str(), fn->mCallbacks.size(),
									cbIds.c_str(), reload);
						}
						ImGui::TreePop();
					}
					++i;
				}
				fcm.unlock();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("sorted by filename")) {
				fcm.lock();
				for (const auto& it : fcm.getFilesByName()) {
					const FileChangeMonitoring::Filename* fn =
							it.second.mFileEntry->getFilenameForFile(fs::getBasenameFromPath(it.first));
					if (!fn) {
						IntentText("%s, count: %u, watch id: %ld (callback not found!)",
								it.first.c_str(), it.second.mCount,
								long(it.second.mFileEntry->getWatchId()));
						continue;
					}
					std::string cbIds = (fn->mCallbacks.size() <= 1) ?
							"cb id: " : "cb ids: ";
					bool isFirst = true;
					unsigned int reload = 0;
					for (const auto& cb : fn->mCallbacks) {
						if (isFirst) {
							isFirst = false;
						}
						else {
							cbIds += ", ";
						}
						cbIds += std::to_string(cb.first);
						reload += cb.second.mCallCount;
					}
					IntentText("%s, count: %u, watch id: %ld, %s, reload: %u",
							it.first.c_str(), it.second.mCount,
							long(it.second.mFileEntry->getWatchId()),
							cbIds.c_str(), reload);
				}
				fcm.unlock();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("sorted by callback id")) {
				fcm.lock();
				for (const auto& it : fcm.getFilesByCallbackId()) {
					const FileChangeMonitoring::Callback* cb =
							it.second.mFileEntry->getCallbackForFile(it.second.mBasename, it.first);
					if (cb) {
						IntentText("cb id: %u, %s, watch id: %ld, reload: %u",
								it.first, cb->mOrigFilename.c_str(),
								long(it.second.mFileEntry->getWatchId()), cb->mCallCount);
					}
					else {
						IntentText("cb id: %u, basename: %s (callback not found!)", it.first, it.second.mBasename.c_str());
					}
				}
				fcm.unlock();
				ImGui::TreePop();
			}
#if 0
			if (ImGui::TreeNode("to string")) {
				// no lock()/unlock() because toString() use intern the mutex
				IntentText("%s", fcm.toString().c_str());
				ImGui::TreePop();
			}
#endif
		}

		void createGlslSceneMenu(bool isFirstShow, Renderer &renderer,
			const RenderPassManager& pm, const SceneManager& sm,
			const ResourceManager& rm, const ContextProperties& cp,
			const Properties& properties,
			const FileChangeMonitoring& fcm,
			bool* showWindow,
			bool& showLogConsole)
		{
			ImGuiWindowFlags windowFlags = 0;
			//windowFlags |= ImGuiWindowFlags_NoTitleBar;
			//windowFlags |= ImGuiWindowFlags_NoScrollbar;
			//windowFlags |= ImGuiWindowFlags_MenuBar;
			//windowFlags |= ImGuiWindowFlags_NoMove;
			//windowFlags |= ImGuiWindowFlags_NoResize;
			//windowFlags |= ImGuiWindowFlags_NoCollapse;
			//windowFlags |= ImGuiWindowFlags_NoNav;
			//windowFlags |= ImGuiWindowFlags_NoBackground;
			//windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

			char tmpLabel[64];

			if (isFirstShow) {
				ImGui::SetNextWindowPos(ImVec2(10, 170));
				ImGui::SetNextWindowSize(ImVec2(400, 420));
			}

			// Main body of the Demo window starts here.
			if (!ImGui::Begin("glslScene", showWindow, windowFlags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			IntentText("scene project: %s", cp.mSceneDirName.c_str());
			IntentText("log console: %s", showLogConsole ? "visible" : "not visible");

			ImGui::Indent();
			if (ImGui::Button(!showLogConsole ? "Show log console" : "Hide log console")) {
				showLogConsole = !showLogConsole;
			}
			ImGui::Unindent();

			if (ImGui::CollapsingHeader("context"))
			{
				addContextToMenu(cp);
			}

			snprintf(tmpLabel, 64, "resources - count: %u", rm.getResourceCount());
			if (ImGui::CollapsingHeaderEx("resources", tmpLabel))
			{
				const ResourceManager::TResByIdNumberMap& resMap = rm.getResourceMapByIdNumber();
				for (const auto& it : resMap) {
					switch (it.second->getType()) {
						case ResType::TEXTURE:
							addTextureResToMenu(*static_cast<Texture*>(it.second.get()));
							break;
						case ResType::SHADER_PROGRAM:
							addShaderProgResToMenu(*static_cast<ShaderProgram*>(it.second.get()));
							break;
						case ResType::MESH:
							addMeshResToMenu(*static_cast<Mesh*>(it.second.get()));
							break;
						case ResType::FRAMEBUFFER:
							addFramebufferResToMenu(*static_cast<Framebuffer*>(it.second.get()));
							break;
					}
				}
			}

			snprintf(tmpLabel, 64, "scenes - count: %u", sm.getSceneCount());
			if (ImGui::CollapsingHeaderEx("scenes", tmpLabel))
			{
				const SceneManager::TSceneByIdNumberMap& sceneMap = sm.getSceneMapByIdNumber();
				for (const auto& it : sceneMap) {
					addSceneToMenu(*it.second);
				}
			}

			snprintf(tmpLabel, 64, "rendering - count: %zu", pm.getPasses().size());
			if (ImGui::CollapsingHeaderEx("rendering", tmpLabel))
			{
				const std::vector<RenderPass>& passes = pm.getPasses();
				char strId[32];
				unsigned int i = 0;
				for (const auto& p : passes) {
					snprintf(strId, 32, "render-pass_%u", i);
					if (ImGui::TreeNode(strId, "render-pass"))
					{
						IntentText("framebuffer-id (number): %u", p.mFramebufferId);
						IntentText("framebuffer resolution: %ux%u", p.mResolution.mWidth, p.mResolution.mHeight);
						IntentText("clear-color: %f %f %f %f",
								p.mClearColor.r,
								p.mClearColor.g,
								p.mClearColor.b,
								p.mClearColor.a);
						if (ImGui::TreeNode("projection-matrix:"))
						{
							addMatrix(p.mProjection.getProjectionMatrix());
							ImGui::TreePop();
						}
						if (ImGui::TreeNode("view-matrix:"))
						{
							addMatrix(p.mViewMatrix);
							ImGui::TreePop();
						}
						IntentText("camera: %s", p.mCamera ? "is used" : "not used");
						IntentText("scene-id (number): %u", p.mSceneId);
						IntentText("depth-test: %s", p.mDepthTest ? "true" : "false");
						ImGui::TreePop();
					}
					++i;
				}
			}

			fcm.lock();
			snprintf(tmpLabel, 64, "hot reloading - file change monitoring - reload: %u",
					fcm.getCallCount());
			fcm.unlock();
			if (ImGui::CollapsingHeaderEx("hot_reloading", tmpLabel))
			{
				addFileChangeMonitoringToMenu(fcm);
			}

			ImGui::End();
		}

		void createLogMenu(bool isFirstShow, bool* showWindow)
		{
			ImGuiWindowFlags windowFlags = 0;
			if (isFirstShow) {
				ImGui::SetNextWindowPos(ImVec2(10, 10));
				ImGui::SetNextWindowSize(ImVec2(780, 150));
			}
			if (!ImGui::Begin("log console", showWindow, windowFlags))
			{
				// Early out if the window is collapsed, as an optimization.
				ImGui::End();
				return;
			}

			const ImVec4 col[]{
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // 0
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // 1
				ImVec4(0.0f, 0.0f, 0.8f, 1.0f), // LOG_VERBOSE = 2
				ImVec4(0.0f, 0.8f, 0.0f, 1.0f), // LOG_DEBUG
				ImVec4(1.0f, 1.0f, 1.0f, 1.0f), // LOG_INFO
				ImVec4(0.9f, 0.9f, 0.0f, 1.0f), // LOG_WARN
				ImVec4(0.8f, 0.0f, 0.0f, 1.0f), // LOG_ERROR
				ImVec4(1.0f, 0.0f, 0.0f, 1.0f)  // LOG_FATAL
			};

			const LogBuffer* lb = getLogBuffer();
			logLockMutex();
			if (lb->mCount > lb->mNextIndex) {
				// --> log buffer is full and overwritten
				for (unsigned int i = lb->mNextIndex; i < MAX_LOG_ENTRIES; ++i) {
					ImGui::PushStyleColor(ImGuiCol_Text, col[lb->mLogs[i].mLogPriority]);
					ImGui::TextWrapped("%s", lb->mLogs[i].mLogMsg.c_str());
					ImGui::PopStyleColor();
				}
			}
			for (unsigned int i = 0; i < lb->mNextIndex; ++i) {
				ImGui::PushStyleColor(ImGuiCol_Text, col[lb->mLogs[i].mLogPriority]);
				ImGui::TextWrapped("%s", lb->mLogs[i].mLogMsg.c_str());
				ImGui::PopStyleColor();
			}
			logUnlockMutex();
			ImGui::End();
		}

		void createDemo()
		{
			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			bool show_demo_window = false;
			ImGui::ShowDemoWindow(&show_demo_window);

			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin(
					"Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Text(
					"This is some useful text.");               // Display some text (you can use a format strings too)
			ImGui::Checkbox("Demo Window",
					&show_demo_window);      // Edit bools storing our window open/close state
			bool show_another_window = true;
			ImGui::Checkbox("Another Window", &show_another_window);

			ImGui::SliderFloat("float", &f, 0.0f,
					1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
			ImGui::ColorEdit3("clear color",
					(float *) &clear_color); // Edit 3 floats representing a color

			if (ImGui::Button(
					"Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate,
					ImGui::GetIO().Framerate);
			ImGui::End();
		}

	}
}

gs::GuiManager::GuiManager()
		:mShow(true), mShowMainMenu(true), mShowLogConsole(true),
		mIsFirstShowMainMenu(true), mIsFirstShowLogConsole(true),
		mWindow(nullptr), mContext(nullptr), mUseOpenGl3(false)
{
}

gs::GuiManager::~GuiManager()
{
	destroyImGui();
}

void gs::GuiManager::initImGui(SDL_Window* window, SDL_GLContext context,
		const ContextProperties& contextProperties)
{
	destroyImGui();

	mWindow = window;
	mContext = context;
	mUseOpenGl3 = contextProperties.useVaoVersionForMesh();

	if (!mWindow || !mContext) {
		// null pointer is not allowed --> no ImGui init
		return;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(mWindow, mContext);
	if (mUseOpenGl3) {
		const char* glsl_version = "#version 130";
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
	else {
		ImGui_ImplOpenGL2_Init();
	}

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}

void gs::GuiManager::destroyImGui()
{
	if (!mWindow || !mContext) {
		return;
	}

	// Cleanup
	if (mUseOpenGl3) {
		ImGui_ImplOpenGL3_Shutdown();
	}
	else {
		ImGui_ImplOpenGL2_Shutdown();
	}
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	mWindow = nullptr;
	mContext = nullptr;
}

void gs::GuiManager::handleEvent(const SDL_Event& e)
{
	ImGui_ImplSDL2_ProcessEvent(&e);
}

void gs::GuiManager::render(Renderer &renderer, const RenderPassManager& pm,
		const SceneManager& sm, const ResourceManager& rm,
		const ContextProperties& cp, const Properties& properties,
		const FileChangeMonitoring& fcm)
{
#if 0
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, mProperties.mWindowSizeI.mWidth, mProperties.mWindowSizeI.mHeight);
	if (mProperties.mUseGlTransforms) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, mProperties.mWindowSize.mWidth, 0.0f, mProperties.mWindowSize.mHeight, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
#endif

	//glUseProgram(0);
#if 0
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_SCISSOR_TEST);
#endif

	renderer.getTextureUnitStack().bindForRendering();
	renderer.getShaderStack().bindForRendering();

	renderer.getTextureUnitStack().switchToTextureUnit(0);

	// Start the Dear ImGui frame
	if (mUseOpenGl3) {
		ImGui_ImplOpenGL3_NewFrame();
	}
	else {
		ImGui_ImplOpenGL2_NewFrame();
	}
	ImGui_ImplSDL2_NewFrame(mWindow);
	ImGui::NewFrame();

	//createDemo();

	if (mShow) {
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 0.75f));
		if (mShowLogConsole) {
			createLogMenu(mIsFirstShowLogConsole, &mShowLogConsole);
			mIsFirstShowLogConsole = false;
		}
		if (mShowMainMenu) {
			createGlslSceneMenu(mIsFirstShowMainMenu, renderer, pm, sm, rm, cp, properties, fcm, &mShowMainMenu, mShowLogConsole);
			mIsFirstShowMainMenu = false;
		}
		ImGui::PopStyleColor();
	}

	// Rendering
	ImGui::Render();
	ImGuiIO& io = ImGui::GetIO();
	//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y); // TODO
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glClear(GL_COLOR_BUFFER_BIT);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	//renderer.getTextureUnitStack().switchToTextureUnit(0); // TODO
	//glEnable(GL_TEXTURE_2D);
	if (mUseOpenGl3) {
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	else {
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	}

	renderer.switchToTextureUnitForLoading();
}

void gs::GuiManager::toggleEnableDisable()
{
	if (!mShow || (!mShowMainMenu && !mShowLogConsole)) {
		mShow = true;
		mShowMainMenu = true;
	}
	else {
		mShow = false;
	}
}

