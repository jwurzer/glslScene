#include <gs/context_properties.h>
#include <string.h>
#include <sstream>

namespace gs
{
	namespace
	{
		void getVersionAsStr(char* verStr, unsigned int maxLen, int majorVersion, int minorVersion)
		{
			if (majorVersion < 0 && minorVersion < 0) {
				strncpy(verStr, "default", maxLen);
				return;
			}
			if (majorVersion >= 0 && minorVersion < 0) {
				snprintf(verStr, maxLen, "%d.x", majorVersion);
				return;
			}
			if (majorVersion < 0 && minorVersion >= 0) {
				snprintf(verStr, maxLen, "?.%d (incorrect?)", minorVersion);
				return;
			}
			snprintf(verStr, maxLen, "%d.%d", majorVersion, minorVersion);
		}
	}
}

const char* gs::ContextProperties::getRenderApiAsStr() const
{
	switch (mRenderApiVersion) {
		case RenderingApi::DEFAULT:
			return "default";
		case RenderingApi::OPENGL:
			return "OpenGL";
		case RenderingApi::OPENGL_ES:
			return "OpenGL ES";
	}
	return "unknown";
}

const char* gs::ContextProperties::getRenderApiProfileAsStr() const
{
	switch (mProfile) {
		case RenderingApiProfile::DEFAULT:
			return "default";
		case RenderingApiProfile::CORE:
			return "core";
		case RenderingApiProfile::COMPATIBILITY:
			return "compatibility";
	}
	return "unknown";
}

const char* gs::ContextProperties::getForwardCompatibilityAsStr() const
{
	switch (mForward) {
		case ForwardCompatibility::DEFAULT:
			return "no (default)";
			break;
		case ForwardCompatibility::FORWARD_COMPATIBILITY:
			return "yes (activated)";
	}
	return "unknown";
}

void gs::ContextProperties::getVersion(char* verStr, unsigned int maxLen) const
{
	getVersionAsStr(verStr, maxLen, mMajorVersion, mMinorVersion);
}

std::string gs::ContextProperties::getVersion() const
{
	char verStr[32];
	getVersion(verStr, 32);
	return verStr;
}

void gs::ContextProperties::getCreatedVersion(char* verStr, unsigned int maxLen) const
{
	getVersionAsStr(verStr, maxLen, mCreatedMajorVersion, mCreatedMinorVersion);
}

std::string gs::ContextProperties::getCreatedVersion() const
{
	char verStr[32];
	getCreatedVersion(verStr, 32);
	return verStr;
}

std::string gs::ContextProperties::toString() const
{
	std::stringstream s;
	s << "Rendering API: " << getRenderApiAsStr();
	s << ", Profile: " << getRenderApiProfileAsStr();
	s << ", Forward-compatibility: " << getForwardCompatibilityAsStr();
	s << ", version: " << getVersion();
	return s.str();
}

bool gs::ContextProperties::useVaoVersionForMesh() const
{
	if (mRenderApiVersion == RenderingApi::OPENGL &&
			((mMajorVersion == 3 && mMinorVersion >= 2) || mMajorVersion > 3) &&
			mProfile == RenderingApiProfile::CORE) {
		return true;
	}
	if (mRenderApiVersion == RenderingApi::OPENGL_ES && mMajorVersion >= 2) {
		return true;
	}
	return false;
}

