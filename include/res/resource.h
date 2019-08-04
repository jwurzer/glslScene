#ifndef GLSLSCENE_RESOURCE_H
#define GLSLSCENE_RESOURCE_H

#include <res/resource_id.h>
#include <string>
#include <vector>
#include <memory>

namespace gs
{
	enum class ResType
	{
		TEXTURE = 0,
		SHADER_PROGRAM,
		MESH,
		FRAMEBUFFER,
	};

	class FileChangeMonitoring;

	class Resource
	{
	public:
		Resource(const std::weak_ptr<FileChangeMonitoring>& fcm);
		virtual ~Resource();
		virtual ResType getType() const = 0;
		virtual bool load() = 0;
		virtual void unload() = 0;

		void setIdNumber(TResourceId idNumber) { mIdNumber = idNumber; }
		TResourceId getIdNumber() const { return mIdNumber; }

		void setIdName(const std::string& idName) { mIdName = idName; }
		const std::string& getIdName() const { return mIdName; }

	protected:
		unsigned int addFileForHotReloading(const std::string& filename);
		size_t getHotReloadingFileCount() const { return mHotReloadingFiles.size(); }
		void removeAllFilesForHotReloading();
	private:
		TResourceId mIdNumber;
		std::string mIdName;

		struct HotReloadingFile
		{
			std::string mFilename;
			unsigned int mCallbackId;

			HotReloadingFile(const std::string& filename, unsigned int callbackId)
					:mFilename(filename), mCallbackId(callbackId) {}
		};

		std::weak_ptr<FileChangeMonitoring> mFileMonitoring;
		std::vector<HotReloadingFile> mHotReloadingFiles;

		static void hotReloadingCb(unsigned int callbackId,
				const std::string& filename,
				const std::shared_ptr<void>& unused, void* thisResource);
		void hotReloading(unsigned int callbackId, const std::string& filename);
	};
}

#endif //GLSLSCENE_RESOURCE_H
