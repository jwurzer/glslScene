#include <capi/system/log.h>

#undef LOG_VERBOSE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_WARN
#undef LOG_ERROR
#undef LOG_FATAL

#undef LOG

#undef LOGV
#undef LOGD
#undef LOGI
#undef LOGW
#undef LOGE
#undef LOGF

#undef LOGVT
#undef LOGDT
#undef LOGIT
#undef LOGWT
#undef LOGET
#undef LOGFT

#include <gs/system/log.h>

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
void logPrint(int prio, const char* filename, int lineNumber, const char *tag, const char *fmt, ...)
{
	va_list args;
	va_start (args, fmt);
	gs::logVprint(prio, filename, lineNumber, tag, fmt, args);
	va_end (args);
}

