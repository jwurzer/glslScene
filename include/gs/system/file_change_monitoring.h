#ifndef GLSLSCENE_FILE_CHANGE_MONITORING_H
#define GLSLSCENE_FILE_CHANGE_MONITORING_H

#include <string>
#include <memory>
#include <map>
#include <set>
#include <thread>
#include <mutex>
#if defined(_MSC_VER)
//#define _WIN32_WINNT 0x0550
#include <windows.h>
#endif

namespace gs
{
	/**
	 * Supports linux and windows. On windows the callback function for changes is delayed by 300ms.
	 * The delay is necessary to prevent multiple calls for one file modification.
	 * See: https://stackoverflow.com/questions/14036449/c-winapi-readdirectorychangesw-receiving-double-notifications
	 */
	class FileChangeMonitoring
	{
	public:
		/**
		 * @param callbackId This is the callback id which was returned by addFile().
		 * @param filename This is the original filename which was used by
		 *                 the function call addFile().
		 * @param param1 This is the parameter which was used by
		 *                 the function call addFile().
		 * @param param2 This is the parameter which was used by
		 *                 the function call addFile().
		 */
		typedef void (*TCallback)(unsigned int callbackId,
				const std::string& filename,
				const std::shared_ptr<void>& param1, void* param2);

		FileChangeMonitoring(bool useSeparateMonitoringThread);
		~FileChangeMonitoring();
		/**
		 * Add a file with full path (relative or absolute) for monitoring.
		 * The parent directory of this file must exist. The file itself
		 * can exist but doesn't have to exist.
		 * If a file like "foo.txt" is added then the directory "." is used.
		 *
		 * If this added file is changed (write changed), or created, or
		 * renamed to the added filename then the callback function will
		 * be called.
		 *
		 * return Return the callback id. 0 for error or greater zero for success (= callback id).
		 */
		unsigned int addFile(const std::string& filename, TCallback callback,
				const std::shared_ptr<void>& param1, void* param2);
		/**
		 * @param callbackId To remove a file the callback id which was returned
		 *                   by addFile() is necessary.
		 * @return True for success (file is removed). False for error.
		 */
		bool removeFile(unsigned int callbackId);
		void removeAllFiles();
		std::string toString() const;

		void checkChanges();
//	private:
		class Callback
		{
		public:
			unsigned int mFileCallbackId;
			TCallback mCbFunc;
			std::shared_ptr<void> mParam1;
			void* mParam2;
			// full filename with some "nice" modifications. Like foo.txt will
			// be ./foo.txt or foo///a//b.txt will be foo/a/b.txt.
			std::string mFilename;
			// This is the original filename which was used for the function
			// call addFile().
			std::string mOrigFilename;
			unsigned int mCallCount;

			Callback(unsigned int fileCallbackId,
					TCallback callbackFunc, const std::shared_ptr<void>& param1,
					void* param2, const std::string& filename,
					const std::string& origFilename)
					:mFileCallbackId(fileCallbackId), mCbFunc(callbackFunc),
					mParam1(param1), mParam2(param2), mFilename(filename),
					mOrigFilename(origFilename), mCallCount(0) {}
		};

		typedef std::map<unsigned int /* callback id */, Callback> TCallbackMap;

		class Filename
		{
		public:
			// filename without path. e.g. for filename ./foo/a/b.txt the
			// basename b.txt will be stored.
			std::string mBasename;
			TCallbackMap mCallbacks;

			Filename(const std::string& basename,
					const std::string& filename,
					const std::string& origFilename,
					unsigned int fileCallbackId,
					TCallback callbackFunc,
					const std::shared_ptr<void>& param1,
					void* param2)
					:mBasename(basename), mCallbacks()
			{
				mCallbacks.insert(TCallbackMap::value_type(fileCallbackId,
						Callback(fileCallbackId, callbackFunc, param1, param2, filename, origFilename)));
			}
		};
		typedef std::map<std::string /* basename */, Filename> TFilenameMap;

		// FileEntry is used for a directory and stores all names from
		// regular files which should be watched.
		class FileEntry
		{
		public:
			FileEntry(
					FileChangeMonitoring* fileChangeMonitoring,
#if defined(_MSC_VER)
					HANDLE watchId,
#else
					int watchId,
#endif
					const std::string& watchname, // directory of watchId
					const std::string& filename,
					const std::string& origFilename,
					unsigned int fileCallbackId, TCallback callbackFunc,
					const std::shared_ptr<void>& param1, void* param2);
			/**
			 * @param filename Full "nice" filename like foo/a/b.txt
			 * @param origFilename Full original filename like foo////a//b.txt
			 */
			bool addFile(const std::string& filename,
					const std::string& origFilename, bool mustExist,
					unsigned int fileCallbackId, TCallback callbackFunc,
					const std::shared_ptr<void>& param1, void* param2);
			bool removeFile(const std::string& filename, unsigned int callbackId,
					std::string& filenameForCallback,
					bool& wasRemovedFromMap);
			bool isFile(const std::string& filename, unsigned int callbackId) const;
			// return null if not found
			Callback* getCallbackForFile(const std::string& filename, unsigned int callbackId);
			const Callback* getCallbackForFile(const std::string& filename, unsigned int callbackId) const;
			Filename* getFilenameForFile(const std::string& filename);
			const Filename* getFilenameForFile(const std::string& filename) const;
			unsigned int getFilenameCount(const std::string& filename) const;
			std::string toString() const;
			void setOrAddWatchDir(const std::string& watchDir) { mWatchnames.insert(watchDir); }
#if defined(_MSC_VER)
			HANDLE getWatchId() const { return mWatchId; }
#else
			int getWatchId() const { return mWatchId; }
#endif
			const std::set<std::string>& getWatchnames() const { return mWatchnames; }
			size_t getWatchIdCount() const { return mNames.size(); }
			TFilenameMap& getNames() { return mNames; }
			const TFilenameMap& getNames() const { return mNames; }
			FileChangeMonitoring* getFileChangeMonitoring() const { return mFcm; }
#if defined(_MSC_VER)
			HANDLE mWatchId;
			std::set<std::string> mWatchnames;
			OVERLAPPED mOverlapped;
			DWORD mNotifyFilter;
			BYTE mBuffer[32 * 1024];
		private:
#else
		private:
			int mWatchId;
			std::set<std::string> mWatchnames;
#endif
			// watch count is the count how often inotify_add_watch() return the
			// same watch id
			// unsigned int mWatchIdCount; // is removed. same as mNames.size()

			// key is the basename (not the filename)
			TFilenameMap mNames;
			FileChangeMonitoring* mFcm;
		};

		class FileEntryCountPair
		{
		public:
			// Count the watched files with the same filename (not original
			// filename, the "nice" filename is used)
			unsigned int mCount;
			std::shared_ptr<FileEntry> mFileEntry;
			FileEntryCountPair() :mCount(0), mFileEntry() {}
		};

		class FileEntryNamePair
		{
		public:
			std::string mBasename;
			std::shared_ptr<FileEntry> mFileEntry;
			FileEntryNamePair() :mBasename(), mFileEntry() {}
			FileEntryNamePair(const std::string& name,
					const std::shared_ptr<FileEntry>& fileEntry)
					:mBasename(name), mFileEntry(fileEntry)
			{}
		};

#if defined(_MSC_VER)
		typedef std::map<HANDLE /* watch id */, std::shared_ptr<FileEntry> > TFileMapByWatchId;
#else
		typedef std::map<int /* watch id */, std::shared_ptr<FileEntry> > TFileMapByWatchId;
#endif
		typedef std::map<std::string /* watchdir */, std::shared_ptr<FileEntry> > TFileMapByWatchDir;
		typedef std::map<std::string /* filename */, FileEntryCountPair> TFileMapByName;
		typedef std::map<unsigned int /* callback id */, FileEntryNamePair> TFileMapByCallbackId;

		typedef std::map<unsigned int /* callback id */, uint32_t /* last change ts */> TChangeMapByCallbackId;

		void lock() const { mSync.lock(); }
		unsigned int getCallCount() const { return mFileChangeMonitoringCallCount; }
		const TFileMapByWatchId& getFilesByWatchId() const { return mFilesByWatchId; }
		const TFileMapByWatchDir& getFilesByWatchDir() const { return mFilesByWatchDir; }
		const TFileMapByName& getFilesByName() const { return mFilesByName; }
		const TFileMapByCallbackId& getFilesByCallbackId() const { return mFilesByCallbackId; }
		void unlock() const { mSync.unlock(); }
	private:
		bool mUseSeparateMonitoringThread;
		bool mRunning;
		int mInotifyFd;
		std::thread mMonitoringThread;

		TChangeMapByCallbackId mChangeMap;
		// Files which are changed at some directories which are monitored.
		// The changed file itself don't need to be a monitoring file.
		unsigned int mFileChangeCount;
		// Files which are changed and monitored!!!!
		unsigned int mFileChangeMonitoringCallCount;
		TFileMapByWatchId mFilesByWatchId;
		TFileMapByWatchDir mFilesByWatchDir;
		TFileMapByName mFilesByName;
		TFileMapByCallbackId mFilesByCallbackId;
		unsigned int mNextFileCallbackId;
		mutable std::mutex mSync;

		static void *posixMonitoringThread(void *thisFileChangeMonitoring);
		void monitoringThread();
		int monitoringFileChanges();

#if defined(_MSC_VER)
		static void CALLBACK FileChangedCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);
#endif
	};
}

#endif /* FILE_CHANGE_MONITORING_H */
