#include <system/file_change_monitoring.h>

#include <system/log.h>
#include <common/fs.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(_MSC_VER)
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#else
#include <sys/inotify.h>
#include <unistd.h>
#endif

#if defined(_MSC_VER)
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif

// for examples see:
// https://opensourceforu.com/2011/04/getting-started-with-inotify/

#if !defined(_MSC_VER)
#define MAX_LEN 1024 /*Path length for a directory*/
#define MAX_EVENTS 1024 /*Max. number of events to process at one go*/
#define LEN_NAME 16 /*Assuming that the length of the filename won't exceed 16 bytes*/
#define EVENT_SIZE  ( sizeof (struct inotify_event) ) /*size of one event*/
#define BUF_LEN     ( MAX_EVENTS * ( EVENT_SIZE + LEN_NAME )) /*buffer to store the data of events*/
#endif

gs::FileChangeMonitoring::FileChangeMonitoring(bool useSeparateMonitoringThread)
		:mUseSeparateMonitoringThread(useSeparateMonitoringThread),
		mRunning(false), mInotifyFd(-1), mMonitoringThread(),
		mFilesByWatchId(), mFilesByName(), mFilesByCallbackId(),
		mNextFileCallbackId(1), mSync()
{
#if defined(_MSC_VER)
	// no extra handle is necessary --> nothing to do here
#else
	mInotifyFd = inotify_init1(useSeparateMonitoringThread ? 0 : IN_NONBLOCK);
	if (mInotifyFd == -1) {
		LOGE("Init inotify failed. %s\n", strerror(errno));
		return;
	}
#endif
	if (useSeparateMonitoringThread) {
		mRunning = true;
		mMonitoringThread = std::thread(posixMonitoringThread, this);
	}
}

gs::FileChangeMonitoring::~FileChangeMonitoring()
{
	removeAllFiles();

	if (mRunning) {
		{
			std::lock_guard<std::mutex> lock(mSync);
			mRunning = false;
#if defined(_MSC_VER)
			LOGI("TODO... monitoring\n");
#else
			int watchFd = inotify_add_watch(mInotifyFd, ".", IN_CLOSE_WRITE | IN_MOVED_TO);
			if (watchFd == -1) {
				LOGE("Add '.' for shutdown to watching failed. %s\n", strerror(errno));
			}
			if (inotify_rm_watch(mInotifyFd, watchFd) == -1) {
				LOGE("Remove file '.' for shutdown from watching failed. %s\n", strerror(errno));
			}
#endif
		}
		mMonitoringThread.join();
	}

#if !defined(_MSC_VER)
	if (mInotifyFd != -1) {
		close(mInotifyFd);
		mInotifyFd = -1;
	}
#endif
}

unsigned int gs::FileChangeMonitoring::addFile(const std::string& origFilename,
		TCallback callback, const std::shared_ptr<void>& param1, void* param2)
{
	std::string dirname;
	std::string basename;
	std::string niceFilename = fs::getReduceAndRemoveEndingSlashes(origFilename, '/');
#if defined(_MSC_VER)
	niceFilename = fs::getReduceAndRemoveEndingSlashes(niceFilename, '\\');
#endif
	fs::getDirAndBaseForPath(niceFilename, dirname, basename);
	if (basename.empty()) {
		LOGE("No basename for path exist!\n");
		return 0;
	}
	// dirname always includes a '/' at the end!
	std::string fname = dirname + basename;
	const std::string& watchname = dirname;

	struct stat filestat;
	if (stat(dirname.c_str(), &filestat) == -1) {
		// is a problem. files with not exist should be allowed...
#if defined(_MSC_VER)
		char errMsg[128] = {};
		strerror_s(errMsg, 128, errno);
		LOGE("Read file stat from parent directory of '%s' failed. %s\n", fname.c_str(), errMsg);
#else
		LOGE("Read file stat from parent directory of '%s' failed. %s\n", fname.c_str(), strerror(errno));
#endif
		return 0;
	}
	if (!S_ISDIR(filestat.st_mode)) {
		LOGE("Parent directory is not a directory\n");
		return 0;
	}

	std::lock_guard<std::mutex> lock(mSync);
	
	unsigned int fileId = mNextFileCallbackId;
	++mNextFileCallbackId;

	{
		auto fileEntryIt = mFilesByName.find(fname);
		if (fileEntryIt != mFilesByName.end()) {
			if (!fileEntryIt->second.mFileEntry->addFile(fname, origFilename, true, fileId, callback, param1, param2)) {
				LOGE("Fatal implementation error! Can't find name for file entry!\n");
				return 0;
			}
			++fileEntryIt->second.mCount;
			mFilesByCallbackId[fileId] = FileEntryNamePair(basename, fileEntryIt->second.mFileEntry);
			return fileId;
		}
	}
	
#if defined(_MSC_VER)
	HANDLE watchFd = CreateFile(watchname.c_str(), FILE_LIST_DIRECTORY,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, 
			OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if (watchFd == INVALID_HANDLE_VALUE) {
		LOGE("Add file '%s' for watching failed. INVALID_HANDLE_VALUE\n", fname.c_str());
		return 0;
	}
#else
	int watchFd = inotify_add_watch(mInotifyFd, watchname.c_str(),
			IN_CLOSE_WRITE | IN_MOVED_TO);
	if (watchFd == -1) {
		LOGE("Add file '%s' for watching failed. %s\n", fname.c_str(), strerror(errno));
		return 0;
	}
#endif
	auto fileEntryItById = mFilesByWatchId.find(watchFd);
	std::shared_ptr<FileEntry> fentry;
	if (fileEntryItById == mFilesByWatchId.end()) {
		// not found --> create new one
		// FileEntry set the watch counter to 1!
		fentry = std::shared_ptr<FileEntry>(new FileEntry(watchFd, fname, origFilename, fileId, callback, param1, param2));
#if defined(_MSC_VER)
		fentry->mOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		fentry->mNotifyFilter = FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE;
		if (!ReadDirectoryChangesW(
				fentry->mWatchId, fentry->mBuffer, sizeof(fentry->mBuffer), false /* recursive */,
				fentry->mNotifyFilter, NULL, &fentry->mOverlapped, FileChangedCallback)) {
			LOGE("ReadDirectoryChangesW failed!!!!!\n");
			CloseHandle(fentry->mOverlapped.hEvent);
			CloseHandle(fentry->mWatchId);
			return 0;
		}
#endif
		mFilesByWatchId[watchFd] = fentry;
		mFilesByName[fname].mFileEntry = fentry;
		++mFilesByName[fname].mCount;
		mFilesByCallbackId[fileId] = FileEntryNamePair(basename, fentry);
	}
	else {
		fentry = fileEntryItById->second;
		if (!fentry->addFile(fname, origFilename, false, fileId, callback, param1, param2)) {
			LOGE("Fatal implementation error! Can't add name for file entry!\n");
			return 0;
		}
		mFilesByName[fname].mFileEntry = fentry;
		++mFilesByName[fname].mCount;
		mFilesByCallbackId[fileId] = FileEntryNamePair(basename, fentry);
	}
	return fileId;
}

bool gs::FileChangeMonitoring::removeFile(unsigned int callbackId)
{
	std::lock_guard<std::mutex> lock(mSync);

	auto fileEntryItByCbId = mFilesByCallbackId.find(callbackId);
	if (fileEntryItByCbId == mFilesByCallbackId.end()) {
		LOGE("Can't find file for callback id %u\n", callbackId);
		return false;
	}
	FileEntryNamePair entryAndName = fileEntryItByCbId->second;
	const std::shared_ptr<FileEntry>& fentry = entryAndName.mFileEntry;
	const std::string& basename= entryAndName.mBasename;
	bool wasRemovedFromMap = false;
	std::string fname;
	if (!fentry->removeFile(basename, callbackId, fname, wasRemovedFromMap)) {
		LOGE("Can't remove file for callback id %u\n", callbackId);
		return false;
	}
	mFilesByCallbackId.erase(fileEntryItByCbId); // always remove it from cb map

	auto fileEntryItByName = mFilesByName.find(fname);
	if (fileEntryItByName == mFilesByName.end()) {
		LOGE("Can't find filename %s (should not be possible)\n", fname.c_str());
		return false;
	}
	--fileEntryItByName->second.mCount;
	if (fileEntryItByName->second.mCount == 0) {
		mFilesByName.erase(fileEntryItByName);
	}

	if (fentry->getWatchIdCount()) {
		return true;
	}

	auto fileEntryItById = mFilesByWatchId.find(fentry->getWatchId());
	if (fileEntryItById == mFilesByWatchId.end()) {
		LOGE("Can't find file entry for %s with id %d\n",
				fname.c_str(), fentry->getWatchId());
		return false;
	}
	mFilesByWatchId.erase(fileEntryItById);

#if defined(_MSC_VER)
	CancelIo(fentry->getWatchId());
	if (!ReadDirectoryChangesW(
			fentry->mWatchId, fentry->mBuffer, sizeof(fentry->mBuffer), false /* recursive */,
			fentry->mNotifyFilter, NULL, &fentry->mOverlapped, nullptr)) {
		LOGE("Remove file '%s' from watching failed.\n", fname.c_str());
	}
	if (!HasOverlappedIoCompleted(&fentry->mOverlapped))
	{
		SleepEx(5, TRUE);
	}
	CloseHandle(fentry->mOverlapped.hEvent);
	CloseHandle(fentry->getWatchId());
#else
	if (inotify_rm_watch(mInotifyFd, fentry->getWatchId()) == -1) {
		LOGE("Remove file '%s' from watching failed. %s\n", fname.c_str(), strerror(errno));
	}
#endif
	return true;
}

void gs::FileChangeMonitoring::removeAllFiles()
{
	std::lock_guard<std::mutex> lock(mSync);
	if (mInotifyFd != -1) {
		for (const auto& it : mFilesByWatchId) {
#if defined(_MSC_VER)
			auto& fentry = it.second;
			CancelIo(fentry->getWatchId());
			if (!ReadDirectoryChangesW(
					fentry->mWatchId, fentry->mBuffer, sizeof(fentry->mBuffer), false /* recursive */,
					fentry->mNotifyFilter, NULL, &fentry->mOverlapped, nullptr)) {
				LOGE("Remove file with watch id %p from watching failed.\n", (void*)fentry->getWatchId());
			}
			if (!HasOverlappedIoCompleted(&fentry->mOverlapped))
			{
				SleepEx(5, TRUE);
			}
			CloseHandle(fentry->mOverlapped.hEvent);
			CloseHandle(fentry->getWatchId());
#else
			if (inotify_rm_watch(mInotifyFd, it.first) == -1) {
				LOGE("Remove file with watch id %d from watching failed. %s\n",
						it.first, strerror(errno));
			}
#endif
		}
	}
	mFilesByWatchId.clear();
	mFilesByName.clear();
	mFilesByCallbackId.clear();
}

std::string gs::FileChangeMonitoring::toString() const
{
	std::stringstream s;
	s << "files by watch id, map entry count: " << mFilesByWatchId.size() << "\n";
	s << "files by filename, map entry count: " << mFilesByName.size() << "\n";
	s << "files by callback, map entry count: " << mFilesByCallbackId.size() << "\n";
	for (const auto& it : mFilesByWatchId) {
		s << it.second->toString();
		for (const auto& nameIt : it.second->getNames()) {
			for (const auto& cbIt : nameIt.second.mCallbacks) {
				if (mFilesByName.find(cbIt.second.mFilename) == mFilesByName.end()) {
					s << "error: " << cbIt.second.mFilename << " can't be found at files by name map\n";
				}
			}
		}
	}
	for (const auto& it : mFilesByName) {
		const std::string& name = it.first;
		const FileEntry& e = *it.second.mFileEntry;
		if (!e.getFilenameCount(fs::getBasenameFromPath(name))) {
			s << "error: " << name << " exist at files by name map but not at file entry\n";
		}
	}
	for (const auto& it : mFilesByCallbackId) {
		unsigned int cbId = it.first;
		const FileEntryNamePair& fen = it.second;
		if (!fen.mFileEntry->isFile(fen.mBasename, cbId)) {
			s << "error: Can't find file with name " << fen.mBasename <<
					" and cb id " << cbId << " at file entry\n";
		}
	}
	return s.str();
}

void gs::FileChangeMonitoring::checkChanges()
{
	if (mUseSeparateMonitoringThread) {
		LOGW("checkChanges() has no effect! Separate thread for file change monitoring is used!\n");
		return;
	}

	int changeCount = monitoringFileChanges();
	if (changeCount < 0) {
		LOGW("monitoringFileChanges() return an error\n");
	}
	else if (changeCount > 0) {
		LOGI("%d files changed\n", changeCount);
	}
}

void *gs::FileChangeMonitoring::posixMonitoringThread(void *thisFileChangeMonitoring)
{
#if defined(_MSC_VER)
	LOGE("TODO: thread-version is not supported for windows (TODO: add blocking for monitoringFileChanges())\n");
	return NULL;
#endif
	static_cast<FileChangeMonitoring*>(thisFileChangeMonitoring)->monitoringThread();
	return NULL;
}

void gs::FileChangeMonitoring::monitoringThread()
{
	int changeCount = 0;
	LOGI("File change monitoring started\n");
	while (mRunning && (changeCount = monitoringFileChanges()) > 0) {
		LOGI("%d files changed\n", changeCount);
	}
	if (!mRunning) {
		LOGI("File change monitoring thread was stopped by destructor\n");
	}
	LOGI("File change monitoring finished\n");
}

int gs::FileChangeMonitoring::monitoringFileChanges()
{
#if defined(_MSC_VER)
	MsgWaitForMultipleObjectsEx(0, NULL, 0, QS_ALLINPUT, MWMO_ALERTABLE);
	return 0;
#else
	char buffer[BUF_LEN];
	ssize_t length = 0;
	length = read(mInotifyFd, buffer, BUF_LEN);
	if (length == -1) {
		if (!mUseSeparateMonitoringThread && (errno == EAGAIN || errno == EWOULDBLOCK)) {
			// ok is opened with non-block --> this error is because no file changed
			return 0;
		}
		LOGE("read for inotify failed (%d). %s\n", errno, strerror(errno));
		return -1;
	}

	int changeCount = 0;
	//LOGI("One or more files changed\n");
	struct inotify_event *event;
	for (ssize_t i = 0; i < length; i += EVENT_SIZE + event->len) {
		++changeCount;
		event = (struct inotify_event *)&buffer[i];
#if 0
		uint32_t mask = event->mask;
		LOGI("watch descriptor %d\n", event->wd);
		LOGI("%" PRIx32 "\n", mask);
		if (mask & IN_ACCESS) {
			LOGI("IN_ACCESS %x\n", IN_ACCESS);
		}
		if (mask & IN_ATTRIB) {
			LOGI("IN_ATTRIB %x\n", IN_ATTRIB);
		}
		if (mask & IN_CLOSE_WRITE) {
			LOGI("IN_CLOSE_WRITE %x\n", IN_CLOSE_WRITE);
		}
		if (mask & IN_CLOSE_NOWRITE) {
			LOGI("IN_CLOSE_NOWRITE %x\n", IN_CLOSE_NOWRITE);
		}
		if (mask & IN_CREATE) {
			LOGI("IN_CREATE %x\n", IN_CREATE);
		}
		if (mask & IN_DELETE) {
			LOGI("IN_DELETE %x\n", IN_DELETE);
		}
		if (mask & IN_DELETE_SELF) {
			LOGI("IN_DELETE_SELF %x\n", IN_DELETE_SELF);
		}
		if (mask & IN_MODIFY) {
			LOGI("IN_MODIFY %x\n", IN_MODIFY);
		}
		if (mask & IN_MOVE_SELF) {
			LOGI("IN_MOVE_SELF %x\n", IN_MOVE_SELF);
		}
		if (mask & IN_MOVED_FROM) {
			LOGI("IN_MOVED_FROM %x\n", IN_MOVED_FROM);
		}
		if (mask & IN_MOVED_TO) {
			LOGI("IN_MOVED_TO %x\n", IN_MOVED_TO);
		}
		if (mask & IN_OPEN) {
			LOGI("IN_OPEN %x\n", IN_OPEN);
		}
		if (mask & IN_IGNORED) {
			LOGI("IN_IGNORED %x\n", IN_IGNORED);
		}
		if (mask & IN_ISDIR) {
			LOGI("IN_ISDIR %x\n", IN_ISDIR);
		}
		if (mask & IN_Q_OVERFLOW) {
			LOGI("IN_Q_OVERFLOW %x\n", IN_Q_OVERFLOW);
		}
		if (mask & IN_UNMOUNT) {
			LOGI("IN_UNMOUNT %x\n", IN_UNMOUNT);
		}
		if (event->len > 0) {
			LOGI("evt name: %s\n", event->name);
		}
#endif
		std::lock_guard<std::mutex> lock(mSync);
		auto fileEntryIt = mFilesByWatchId.find(event->wd);
		if (fileEntryIt != mFilesByWatchId.end()) {
			//LOGI("%s\n", fileEntryIt->second->toString().c_str());
			if (event->len > 0) {
				std::string basename = event->name;
				const FileEntry& fe = *fileEntryIt->second;
				auto fnameIt = fe.getNames().find(basename);
				if (fnameIt != fe.getNames().end()) {
					for (const auto& cbIt : fnameIt->second.mCallbacks) {
						const Callback& cb = cbIt.second;
						LOGI("file %s, orig name %s changed!\n", cb.mFilename.c_str(), cb.mOrigFilename.c_str());
						cb.mCbFunc(cb.mFileCallbackId, cb.mOrigFilename, cb.mParam1, cb.mParam2);
					}
				}
			}
		}
		else {
			if (!(event->mask & IN_IGNORED)) {
				LOGW("Can't find file entry for watch id %d\n", event->wd);
			}
		}
	}
	return changeCount;
#endif
}

/////////////////////////////////

gs::FileChangeMonitoring::FileEntry::FileEntry(
#if defined(_MSC_VER)
		HANDLE watchId,
#else
		int watchId,
#endif
		const std::string& filename,
		const std::string& origFilename, unsigned int fileCallbackId,
		TCallback callbackFunc, const std::shared_ptr<void>& param1, void* param2)
		:mWatchId(watchId), mNames()
{
	if (!addFile(filename, origFilename, false, fileCallbackId, callbackFunc, param1, param2)) {
		LOGE("Fatal error! Can't add filename for file entry!\n");
	}
}

bool gs::FileChangeMonitoring::FileEntry::addFile(const std::string& filename,
		const std::string& origFilename, bool mustExist,
		unsigned int fileCallbackId, TCallback callbackFunc,
		const std::shared_ptr<void>& param1, void* param2)
{
	std::string basename = fs::getBasenameFromPath(filename);
	auto fnameIt = mNames.find(basename);
	if (fnameIt == mNames.end()) {
		// not found --> create new one if allowed
		if (mustExist) {
			return false;
		}
		// struct Filename init counter with 1! --> no ++mCount necessary!
		auto rv = mNames.insert(TFilenameMap::value_type(basename, Filename(basename,
				filename, origFilename, fileCallbackId, callbackFunc, param1, param2)));
		if (!rv.second) {
			return false;
		}
		fnameIt = rv.first;
	}
	else {
		Filename& fn = fnameIt->second;
		fn.mCallbacks.insert(TCallbackMap::value_type(fileCallbackId,
			Callback(fileCallbackId, callbackFunc, param1, param2, filename, origFilename)));
	}
	return true;
}

bool gs::FileChangeMonitoring::FileEntry::removeFile(
		const std::string& basename, unsigned int callbackId,
		std::string& filenameForCallback,
		bool& wasRemovedFromMap)
{
	wasRemovedFromMap = false;
	auto fnameIt = mNames.find(basename);
	if (fnameIt == mNames.end()) {
		return false;
	}
	Filename& fn = fnameIt->second;
	auto cbIt = fn.mCallbacks.find(callbackId);
	if (cbIt == fn.mCallbacks.end()) {
		return false;
	}
	filenameForCallback = cbIt->second.mFilename;
	fn.mCallbacks.erase(cbIt);
	if (!fn.mCallbacks.size()) {
		// counter is zero --> remove the entry from the map
		mNames.erase(fnameIt);
		wasRemovedFromMap = true;
	}
	return true;
}

bool gs::FileChangeMonitoring::FileEntry::isFile(const std::string& filename,
		unsigned int callbackId) const
{
	auto fnameIt = mNames.find(filename);
	if (fnameIt == mNames.end()) {
		return false;
	}
	const Filename& fn = fnameIt->second;
	auto cbIt = fn.mCallbacks.find(callbackId);
	if (cbIt == fn.mCallbacks.end()) {
		return false;
	}
	return true;
}

unsigned int gs::FileChangeMonitoring::FileEntry::getFilenameCount(const std::string& filename) const
{
	auto it = mNames.find(filename);
	if (it == mNames.end()) {
		return 0;
	}
	return it->second.mCallbacks.size();
}

std::string gs::FileChangeMonitoring::FileEntry::toString() const
{
	std::stringstream s;
	std::stringstream s2;
	s << "watch id: " << mWatchId << ", watch id count (map size): " << mNames.size();
	unsigned int count = 0;
	for (const auto& it : mNames) {
		s2 << " name-key: " << it.first << ", basename: " << it.second.mBasename << ", count: " << it.second.mCallbacks.size() << "\n";
		for (const auto& cbIt : it.second.mCallbacks) {
			s2 << "  callback id: " << cbIt.second.mFileCallbackId <<
					", filename: " << cbIt.second.mFilename << "\n";
		}
		count += it.second.mCallbacks.size();
	}
	s << ", total count: " << count << "\n";
	return s.str() + s2.str();
}

#if defined(_MSC_VER)
void CALLBACK gs::FileChangeMonitoring::FileChangedCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
{
	//LOGI("WatchCallback is called ;-)\n");
	if (!dwNumberOfBytesTransfered) {
		return;
	}
	if (dwErrorCode != ERROR_SUCCESS) {
		return;
	}

	FileEntry* fe = reinterpret_cast<FileEntry*>((char*)lpOverlapped - offsetof(FileEntry, mOverlapped));
#if 0
	PFILE_NOTIFY_INFORMATION notify;
	size_t offset = 0;
	TCHAR filename[MAX_PATH];
	do
	{
		notify = (PFILE_NOTIFY_INFORMATION) &fe->mBuffer[offset];
		offset += notify->NextEntryOffset;
#if defined(UNICODE)
		{
			lstrcpynW(filename, notify->FileName,
				min(MAX_PATH, notify->FileNameLength / sizeof(WCHAR) + 1));
		}
#else
		{
			int count = WideCharToMultiByte(CP_ACP, 0, notify->FileName,
				notify->FileNameLength / sizeof(WCHAR),
				filename, MAX_PATH - 1, NULL, NULL);
			filename[count] = TEXT('\0');
		}
#endif
		LOGI("filename: %s\n", filename);
	} while (notify->NextEntryOffset != 0);
#endif
	// TODO check with filename and only call this callback
	//LOGI("%s\n", fe->toString().c_str());
	for (const auto& filenameIt : fe->getNames()) {
		const Filename& filename = filenameIt.second;
		for (const auto& cbIt : filename.mCallbacks)
		{
			const Callback& cb = cbIt.second;
			LOGI("file %s, orig name %s changed!\n", cb.mFilename.c_str(), cb.mOrigFilename.c_str());
			cb.mCbFunc(cb.mFileCallbackId, cb.mOrigFilename, cb.mParam1, cb.mParam2);
		}
	}
	//fe->f
}
#endif

