#include <gs/configloader/context_loader.h>
#include <gs/common/cfg.h>
#include <gs/system/log.h>
#include <vector>

namespace gs
{
	namespace
	{
		bool getVersionParameters(const CfgValue& ver,
				ContextProperties& p)
		{
			if (!ver.isArray()) {
				LOGE("Wrong value for version\n");
				return false;
			}
			size_t count = ver.mArray.size();
			if (!count) {
				LOGE("No value for version\n");
				return false;
			}
			const std::string& apiStr = ver.mArray[0].mValue.mText;
			if (apiStr == "opengl" ||
					apiStr == "openGl" ||
					apiStr == "OpenGl") {
				p.mRenderApiVersion = RenderingApi::OPENGL;
			}
			else if (apiStr == "gles" || apiStr == "GLES") {
				p.mRenderApiVersion = RenderingApi::OPENGL_ES;
			}
			else {
				LOGE("Wrong value for rendering API (wrong value for version)\n");
				return false;
			}
			if (count < 2) {
				return true;
			}
			bool verNumberAllowed = true;
			bool profileAllowed = true;
			bool forwardAllowed = true;
			unsigned int verPosIndex = 1;
			if (ver.mArray[1].mValue.isText()) {
				const std::string &apiStr2 = ver.mArray[1].mValue.mText;
				if (apiStr2 == "es" || apiStr2 == "ES") {
					if (p.mRenderApiVersion != RenderingApi::OPENGL) {
						LOGE("'es' is only allowed if the word opengl is before\n");
						return false;
					}
					p.mRenderApiVersion = RenderingApi::OPENGL_ES;
					++verPosIndex;
				}
				else if (apiStr2 == "core" ||
						apiStr2 == "compatibility" ||
						apiStr2 == "compat") {
					if (p.mRenderApiVersion != RenderingApi::OPENGL) {
						LOGE("A profile is only allowed for opengl.\n");
						return false;
					}
					p.mProfile = (apiStr2 == "core") ?
							RenderingApiProfile::CORE :
							RenderingApiProfile::COMPATIBILITY;
					verNumberAllowed = false;
					profileAllowed = false;
				}
				else if (apiStr2 == "forward" || apiStr2 == "forward-compatibility") {
					p.mForward = ForwardCompatibility::FORWARD_COMPATIBILITY;
					verNumberAllowed = false;
					profileAllowed = false;
					forwardAllowed = false;
				}
				else {
					LOGE("'%s' is not allowed here\n", apiStr2.c_str());
					return false;
				}
			}

			if (verPosIndex >= count) {
				// no version is used
				return true;
			}

			if (ver.mArray[verPosIndex].mValue.isNumber()) {
				if (!verNumberAllowed) {
					LOGE("Version number is not allowed here.\n");
					return false;
				}
				if (ver.mArray[verPosIndex].mValue.isInteger()) {
					int majorVer = ver.mArray[verPosIndex].mValue.mInteger;
					if (majorVer < 1) {
						LOGE("Major opengl version must be >= 1 and not %d\n", majorVer);
						return false;
					}
					p.mMajorVersion = majorVer;
				}
				else if (ver.mArray[verPosIndex].mValue.isFloat()) {
					float verNumber = ver.mArray[verPosIndex].mValue.mFloatingPoint;
					if (verNumber < 1.0f) {
						LOGE("Major opengl version must be >= 1 and not %f\n", verNumber);
						return false;
					}
					p.mMajorVersion = static_cast<int>(verNumber + 0.0001f);
					p.mMinorVersion = static_cast<int>((verNumber - static_cast<float>(p.mMajorVersion)) * 10.0f + 0.0001f);
				}
				++verPosIndex;
			}

			unsigned int nextIndex = verPosIndex;
			if (p.mRenderApiVersion != RenderingApi::OPENGL) {
				// --> no profile and no forward is allowed
				if (nextIndex != count) {
					LOGE("Wrong count of arguments for version value.\n");
					return false;
				}
				return true;
			}
			// --> now it only can be opengl
			if (nextIndex >= count) {
				return true;
			}

			{
				const std::string &apiStr2 = ver.mArray[nextIndex].mValue.mText;
				if (apiStr2 == "core" ||
						apiStr2 == "compatibility" ||
						apiStr2 == "compat") {
					if (!profileAllowed) {
						LOGE("No profile is only allowed at this position.\n");
						return false;
					}
					p.mProfile = (apiStr2 == "core") ?
							RenderingApiProfile::CORE :
							RenderingApiProfile::COMPATIBILITY;
					++nextIndex;
				}
				else if (apiStr2 == "forward" || apiStr2 == "forward-compatibility") {
					if (!forwardAllowed) {
						LOGE("No forward is only allowed at this position.\n");
						return false;
					}
					p.mForward = ForwardCompatibility::FORWARD_COMPATIBILITY;
					forwardAllowed = false;
					++nextIndex;
				}
				else {
					LOGE("'%s' is not allowed here\n", apiStr2.c_str());
					return false;
				}
			}

			if (nextIndex >= count) {
				return true;
			}

			{
				const std::string &apiStr2 = ver.mArray[nextIndex].mValue.mText;
				if (apiStr2 == "forward" || apiStr2 == "forward-compatibility") {
					if (!forwardAllowed) {
						LOGE("No forward is only allowed at this position.\n");
						return false;
					}
					p.mForward = ForwardCompatibility::FORWARD_COMPATIBILITY;
					++nextIndex;
				}
				else {
					LOGE("'%s' is not allowed here\n", apiStr2.c_str());
					return false;
				}
			}
			if (nextIndex != count) {
				LOGE("impl. error for evaluate context version config\n");
				return false;
			}
			return true;
		}

		bool getParameters(const CfgValuePair& cfgValuePair,
				ContextProperties& p)
		{
			if (cfgValuePair.mName.mText != "context") {
				return false;
			}
			const CfgValue* version = nullptr;
			unsigned int versionCount = 0;
			CfgReadRule cfgRules[] = {
					CfgReadRule("rendering-api", &version, CfgReadRule::RULE_MUST_EXIST, CfgReadRule::ALLOW_ALL, &versionCount),
					CfgReadRule("")
			};
			size_t nextPos = 0;
			ssize_t storeCnt = cfgValuePair.mValue.sectionGet(
					cfgRules, false, false, false, false, false, 0, &nextPos);
			if (storeCnt < 0) {
				LOGE("context config is wrong (rendering-api exist?) %d\n", int(storeCnt));
				return false;
			}
			if (versionCount > 1) {
				LOGE("rendering-api can only be used once!\n");
				return false;
			}
			return getVersionParameters(*version, p);
		}
	}
}

bool gs::contextloader::getContextParameters(const CfgValuePair& cfgValue,
		ContextProperties& p)
{
	p.reset();

	if (!cfgValue.mValue.isArray()) {
		return false;
	}
	unsigned int contextCount = 0;
	const std::vector<CfgValuePair>& array = cfgValue.mValue.mArray;
	for (const CfgValuePair& vp : array) {
		if (vp.mName.mText == "context") {
			if (contextCount) {
				LOGE("Only one context is allowed.\n");
				p.reset();
				return false;
			}
			if (!getParameters(vp, p)) {
				LOGE("Wrong context config\n");
				p.reset();
				return false;
			}
			++contextCount;
		}
	}
	if (!contextCount) {
		// no context was used --> return false
		return false;
	}
	return true;
}

