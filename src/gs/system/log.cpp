#include <gs/system/log.h>

#include <stdio.h>
#include <string.h>
#include <mutex>
#ifdef _WIN32
#include <Windows.h>
#include <iostream>
#else
#ifdef __ANDROID__
#include <android/log.h>
#include <unistd.h>
#else
//#include <iostream> // for wcout
#include <unistd.h> // for write
#endif
#include <sys/syscall.h>
#endif
#include <SDL.h>

// tmp string is created on the stack with MAX_LOG_MSG
//#define MAX_LOG_MSG 10240
#define MAX_LOG_MSG 102400

#define STR_FMT_PRIO "%s: "
#define STR_FMT_FILE_LINE "%s:%d: "
#define STR_FMT_TAG "%s: "

static std::mutex sOutSync;

static const char* logPrioStr[] =
{
	"?",
	"?",
	"V", // LOG_VERBOSE = 2, // is used by LOGV - means: extensive, detailed, in-depth
	"D", // LOG_DEBUG,       // is used by LOGD
	"I", // LOG_INFO,        // is used by LOGI
	"W", // LOG_WARN,        // is used by LOGW
	"E", // LOG_ERROR,       // is used by LOGE
	"F"  // LOG_FATAL,       // is used by LOGF
};

namespace gs
{
namespace
{
	LogBuffer logBuf;

	int getLogHeader(char* headerStr, size_t headerSize, int prio,
			const char* filename, int lineNumber, const char *tag)
	{
#ifdef _WIN32
		DWORD tid = GetCurrentThreadId();
#else
#ifdef __ANDROID__
		pid_t tid = gettid();
#else
		pid_t tid = syscall(SYS_gettid);
#endif
#endif
#ifdef _WIN32
		int idLen = sprintf_s(headerStr, headerSize, "%llu: ", (unsigned long long)tid);
#else
		int idLen = snprintf(headerStr, headerSize, "%llu: ", (unsigned long long)tid);
#endif
		headerStr += idLen;
		headerSize -= idLen;

#ifdef _WIN32
		idLen = sprintf_s(headerStr, headerSize, "%" PRIu32 ": ", SDL_GetTicks());
#else
		idLen = snprintf(headerStr, headerSize, "%" PRIu32 ": ", SDL_GetTicks());
#endif
		headerStr += idLen;
		headerSize -= idLen;


		if (filename && lineNumber >= 0) {
			const char* onlyFilename = filename;
			for (size_t i = 0; filename[i]; i++) {
#ifdef _WIN32
				if (filename[i] == '\\') {
#else
				if (filename[i] == '/') {
#endif
					onlyFilename = filename + i + 1;
				} 
			}
			if (tag) {
				return snprintf(headerStr, headerSize,
						STR_FMT_PRIO STR_FMT_FILE_LINE STR_FMT_TAG,
						logPrioStr[prio], onlyFilename, lineNumber, tag);
			}
			return snprintf(headerStr, headerSize, STR_FMT_PRIO STR_FMT_FILE_LINE,
					logPrioStr[prio], onlyFilename, lineNumber);
		}
		if (tag) {
			return snprintf(headerStr, headerSize, STR_FMT_PRIO STR_FMT_TAG,
					logPrioStr[prio], tag);
		}
		return snprintf(headerStr, headerSize, STR_FMT_PRIO, logPrioStr[prio]);
	}

	void printMessage(int prio, char* fullMsg)
	{
		int len = strlen(fullMsg);
		if (len > 0 && fullMsg[len - 1] == '\n') {
			fullMsg[len - 1] = '\0';
			--len;
		}
		std::lock_guard<std::mutex> lock(sOutSync);
		logBuf.write(prio, fullMsg);
#ifdef _WIN32
		std::cout << fullMsg << std::endl;
#else
#ifdef __ANDROID__
		//#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_INFO, "sfml-activity", __VA_ARGS__))
		__android_log_print(ANDROID_LOG_INFO, "glslScene", "%ld %s", (unsigned long)gettid(), fullMsg);
#else
		write(1, fullMsg, len);
		write(1, "\n", 1);
#endif
#endif
	}
}
}
/*
 * Send a simple string to the log.
 */
void gs::logWrite(int prio, const char* filename, int lineNumber, const char *tag, const char *text)
{
	char logHeader[MAX_LOG_MSG];
	int headerLen = getLogHeader(logHeader, MAX_LOG_MSG, prio, filename, lineNumber, tag);

#ifdef _WIN32
	strncat_s(logHeader, MAX_LOG_MSG, text, MAX_LOG_MSG - headerLen - 1);
#else
	strncat(logHeader, text, MAX_LOG_MSG - headerLen - 1);
#endif
	logHeader[MAX_LOG_MSG - 1] = '\0';

	printMessage(prio, logHeader);
}

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
void gs::logPrint(int prio, const char* filename, int lineNumber, const char *tag, const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	logVprint(prio, filename, lineNumber, tag, fmt, args);
	va_end (args);
}

/*
 * A variant of log_print() that takes a va_list to list
 * additional parameters.
 */
void gs::logVprint(int prio, const char* filename, int lineNumber, const char *tag,
		const char *fmt, va_list ap)
{
#if 0
	char logHeader[MAX_LOG_MSG];
	int headerLen = getLogHeader(logHeader, MAX_LOG_MSG, prio, filename, lineNumber, tag);

	char logMsg[MAX_LOG_MSG];
	vsnprintf(logMsg, MAX_LOG_MSG, fmt, ap);

	strncat(logHeader, logMsg, MAX_LOG_MSG - headerLen - 1);

	printMessage(prio, logHeader);
#else
	char logMsg[MAX_LOG_MSG];
	vsnprintf(logMsg, MAX_LOG_MSG, fmt, ap);

	logWrite(prio, filename, lineNumber, tag, logMsg);
#endif
}

const gs::LogBuffer* gs::getLogBuffer()
{
	return &logBuf;
}

void gs::logLockMutex()
{
	sOutSync.lock();
}

void gs::logUnlockMutex()
{
	sOutSync.unlock();
}

#if 0
template<> gs::Log<char>& gs::Log<char>::operator<<(const std::wstring& v) {
	mMsg << NStringConverter::convertWstrToUtf8(v);
	return *this;
}

template<> gs::Log<char>& gs::Log<char>::operator<<(const wchar_t* v) {
	mMsg << NStringConverter::convertWstrToUtf8(v);
	return *this;
}
#endif
