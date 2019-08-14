#ifndef GLSLSCENE_LOG_H
#define GLSLSCENE_LOG_H

#define GLSLSCENE_DEBUG

/*
 * Logging with printf() style
 * 
 * For fixed size types like uint32_t, int32_t, uint64_t, ...
 * include <inttypes.h> file.
 * 
 * printf("for uint32_t use %"PRIu32" \n", x);
 * printf("for int64_t like timestamp use %"PRId64" or %"PRIi64" \n", x, x);
 */

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h> // --> no extra including for printing uint32_t with PRIu32 ...
#include <stdarg.h> // for va_list
#include <sstream>
#include <array>

#ifdef LOG_DISABLED
#define LOG(prio, tag, fmt, ...) {}
#else
#if defined(GLSLSCENE_DEBUG)
// with filenames
#define LOG(prio, tag, fmt, ...) gs::logPrint(prio, __FILE__, __LINE__, tag, fmt, ##__VA_ARGS__)
#else
// without filenames
#define LOG(prio, tag, fmt, ...) gs::logPrint(prio, NULL, 0, tag, fmt, ##__VA_ARGS__)
#endif
#endif

#define LOGV(fmt, ...) LOGVT(0, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) LOGDT(0, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) LOGIT(0, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) LOGWT(0, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) LOGET(0, fmt, ##__VA_ARGS__)
#define LOGF(fmt, ...) LOGFT(0, fmt, ##__VA_ARGS__)

#define LOGVT(tag, fmt, ...) LOG(gs::LOG_VERBOSE, tag, fmt, ##__VA_ARGS__)
#define LOGDT(tag, fmt, ...) LOG(gs::LOG_DEBUG, tag, fmt, ##__VA_ARGS__)
#define LOGIT(tag, fmt, ...) LOG(gs::LOG_INFO, tag, fmt, ##__VA_ARGS__)
#define LOGWT(tag, fmt, ...) LOG(gs::LOG_WARN, tag, fmt, ##__VA_ARGS__)
#define LOGET(tag, fmt, ...) LOG(gs::LOG_ERROR, tag, fmt, ##__VA_ARGS__)
#define LOGFT(tag, fmt, ...) LOG(gs::LOG_FATAL, tag, fmt, ##__VA_ARGS__)

// macros for log streams

// TODO log streams there should be a IFLOG macros
// see http://stackoverflow.com/questions/1389538/cancelling-stdcout-code-lines-using-preprocessor
// --> IFLOG(LOGSV << "test message");

// with filenames
#define LOGS(prio, tag) gs::LogC(prio, __FILE__, __LINE__, tag)
// without filenames
//#define LOGS(prio, tag) gs::LogC(prio, NULL, 0, tag)

#define LOGSV LOGSVT(0)
#define LOGSD LOGSDT(0)
#define LOGSI LOGSIT(0)
#define LOGSW LOGSWT(0)
#define LOGSE LOGSET(0)
#define LOGSF LOGSFT(0)

#define LOGSVT(tag) LOGS(gs::LOG_VERBOSE, tag)
#define LOGSDT(tag) LOGS(gs::LOG_DEBUG, tag)
#define LOGSIT(tag) LOGS(gs::LOG_INFO, tag)
#define LOGSWT(tag) LOGS(gs::LOG_WARN, tag)
#define LOGSET(tag) LOGS(gs::LOG_ERROR, tag)
#define LOGSFT(tag) LOGS(gs::LOG_FATAL, tag)

#define MAX_LOG_ENTRIES 512

namespace gs
{
	typedef enum {
		LOG_VERBOSE = 2, // is used by LOGV - means: extensive, detailed, in-depth
		LOG_DEBUG,       // is used by LOGD
		LOG_INFO,        // is used by LOGI - should be used for normal usage
		LOG_WARN,        // is used by LOGW - should be used for warnings
		LOG_ERROR,       // is used by LOGE - should be used for errors
		LOG_FATAL,       // is used by LOGF
	} LogPriority;

	/*
	 * Send a simple string to the log.
	 */
	void logWrite(int prio, const char* filename, int lineNumber, const char *tag, const char *text);

	/*
	 * Send a formatted string to the log, used like printf(fmt,...)
	 */
	void logPrint(int prio, const char* filename, int lineNumber, const char *tag, const char *fmt, ...)
#if defined(__GNUC__)
		__attribute__ ((format(printf, 5, 6)))
#endif
		;

	/*
	 * A variant of log_print() that takes a va_list to list
	 * additional parameters.
	 */
	void logVprint(int prio, const char* filename, int lineNumber, const char *tag,
			const char *fmt, va_list ap);

	class LogEntry
	{
	public:
		int mLogPriority;
		std::string mLogMsg;
		LogEntry() :mLogPriority(LOG_VERBOSE), mLogMsg() {}
	};

	class LogBuffer
	{
	public:
		unsigned int mNextIndex = 0;
		unsigned int mCount = 0;
		std::array<LogEntry, MAX_LOG_ENTRIES> mLogs;

		void write(int prio, const char* log)
		{
			mLogs[mNextIndex].mLogPriority = prio;
			mLogs[mNextIndex].mLogMsg = log;

			++mNextIndex;
			mNextIndex %= MAX_LOG_ENTRIES;

			++mCount;
			if (mCount > MAX_LOG_ENTRIES) {
				mCount = MAX_LOG_ENTRIES;
			}
		}
	};


	// always return a valid log buffer. (never return null)
	// To access the logs logLockMutex()/logUnlockMutex() should be used.
	const LogBuffer* getLogBuffer();
	// should (NEVER) be used!!! Only if you want to write to log fd without logging functions,
	// or you want to print the LogBuffer
	void logLockMutex();
	// should (NEVER) be used!!! Only if you want to write to log fd without logging functions
	void logUnlockMutex();

	template <class T>
	class Log
	{
	public:
		Log(int prio, const char* filename, int lineNumber, const char *tag)
				:mPrio(prio), mFilename(filename), mLineNumber(lineNumber), mTag(tag), mMsg()
		{
		}

		~Log()
		{
			logWrite(mPrio, mFilename, mLineNumber, mTag, mMsg.str().c_str());
		}

		template <class TT>
		Log &operator<<(const TT &v)
		{
			mMsg << v;
			return *this;
		}

		// see comments from http://stackoverflow.com/questions/772355/how-to-inherit-from-stdostream
		Log &operator<<(std::basic_ostream<T>& (*func)(std::basic_ostream<T>&))
		{
			func(mMsg);
			return *this;
		}

		// this member functions are necessary for the template specialization
		// with the template specialization the mixing of normal and wide
		// characters are possible ;-)

		Log &operator<<(const std::string& v)
		{
			mMsg << v;
			return *this;
		}

		Log &operator<<(const std::wstring& v)
		{
			mMsg << v;
			return *this;
		}

		Log &operator<<(const wchar_t* v)
		{
			mMsg << v;
			return *this;
		}

	private:
		int mPrio;
		const char* mFilename;
		int mLineNumber;
		const char *mTag;
		std::basic_stringstream<T> mMsg;
	};

	// template specialization see answers from http://stackoverflow.com/questions/22083208/compile-time-check-template-type-c
	//template<> Log<char>& Log<char>::operator<<(const std::wstring& v);
	//template<> Log<char>& Log<char>::operator<<(const wchar_t* v);

	typedef Log<char> LogC;
}

#endif
