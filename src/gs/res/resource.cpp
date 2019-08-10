#include <gs/res/resource.h>
#include <gs/system/file_change_monitoring.h>
#include <gs/system/log.h>

gs::Resource::Resource(const std::weak_ptr<FileChangeMonitoring>& fcm)
		:mIdNumber(0), mIdName(), mFileMonitoring(fcm), mHotReloadingFiles()
{
}

gs::Resource::~Resource()
{
	removeAllFilesForHotReloading();
}

unsigned int gs::Resource::addFileForHotReloading(const std::string &filename)
{
	std::shared_ptr<FileChangeMonitoring> fcm(mFileMonitoring.lock());
	if (!fcm) {
		return 0;
	}
	unsigned int callbackId = fcm->addFile(filename, hotReloadingCb, std::shared_ptr<void>(), this);
	if (!callbackId) {
		return 0;
	}
	mHotReloadingFiles.emplace_back(filename, callbackId);
	return callbackId;
}

void gs::Resource::removeAllFilesForHotReloading()
{
	std::shared_ptr<FileChangeMonitoring> fcm(mFileMonitoring.lock());
	if (!fcm) {
		return;
	}
	for (const auto& file : mHotReloadingFiles) {
		if (!fcm->removeFile(file.mCallbackId)) {
			LOGW("Can't remove %s (cb id %u) for hot reloading\n", file.mFilename.c_str(), file.mCallbackId);
		}
	}
	mHotReloadingFiles.clear();
}

void gs::Resource::hotReloadingCb(unsigned int callbackId,
		const std::string& filename,
		const std::shared_ptr<void>& unused, void* thisResource)
{
	static_cast<Resource*>(thisResource)->hotReloading(callbackId, filename);
}

void gs::Resource::hotReloading(unsigned int callbackId, const std::string& filename)
{
	LOGI("hot reloading: reload %s\n", filename.c_str());
	load();
}

