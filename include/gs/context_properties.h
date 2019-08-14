#ifndef GLSLSCENE_CONTEXT_PROPERTIES_H
#define GLSLSCENE_CONTEXT_PROPERTIES_H

#include <string>

namespace gs
{
	enum class RenderingApi
	{
		DEFAULT, // not specified
		OPENGL,
		OPENGL_ES,
	};

	enum class RenderingApiProfile
	{
		DEFAULT, // not specified
		CORE,
		COMPATIBILITY,
	};

	enum class ForwardCompatibility
	{
		DEFAULT, // not specified, none
		FORWARD_COMPATIBILITY,
	};

	class ContextProperties
	{
	public:
		// to change the parameters for context creation,
		// the scene config should be used!
		RenderingApi mRenderApiVersion = RenderingApi::DEFAULT;
		RenderingApiProfile mProfile = RenderingApiProfile::DEFAULT;
		ForwardCompatibility mForward = ForwardCompatibility::DEFAULT;
		int mMajorVersion = -1; // -1 for not specified
		int mMinorVersion = -1; // -1 for not specified

		std::string mCreatedGlVersion;
		std::string mCreatedGlslVersion;
		std::string mCreatedVendor;
		std::string mCreatedRenderer;
		int mCreatedMajorVersion = -1; // -1 for not specified
		int mCreatedMinorVersion = -1; // -1 for not specified

		std::string mSceneDirName;

		void reset()
		{
			mRenderApiVersion = RenderingApi::DEFAULT;
			mProfile = RenderingApiProfile::DEFAULT;
			mForward = ForwardCompatibility::DEFAULT;
			mMajorVersion = -1; // -1 for not specified
			mMinorVersion = -1; // -1 for not specified
		}
		const char* getRenderApiAsStr() const;
		const char* getRenderApiProfileAsStr() const;
		const char* getForwardCompatibilityAsStr() const;
		void getVersion(char* verStr, unsigned int maxLen) const;
		std::string getVersion() const;
		void getCreatedVersion(char* verStr, unsigned int maxLen) const;
		std::string getCreatedVersion() const;
		std::string toString() const;
		bool useVaoVersionForMesh() const;
	};
}

#endif //GLSLSCENE_CONTEXT_PROPERTIES_H
