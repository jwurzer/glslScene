#ifndef GLSLSCENE_CAPI_LOG_H
#define GLSLSCENE_CAPI_LOG_H

#define LOG_VERBOSE 2 // is used by LOGV - means: extensive, detailed, in-depth
#define LOG_DEBUG 3   // is used by LOGD
#define LOG_INFO 4    // is used by LOGI - should be used for normal usage
#define LOG_WARN 5    // is used by LOGW - should be used for warnings
#define LOG_ERROR 6   // is used by LOGE - should be used for errors
#define LOG_FATAL 7   // is used by LOGF

#define LOG(prio, tag, fmt, ...) logPrint(prio, __FILE__, __LINE__, tag, fmt, ##__VA_ARGS__)

#define LOGV(fmt, ...) LOGVT(0, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) LOGDT(0, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) LOGIT(0, fmt, ##__VA_ARGS__)
#define LOGW(fmt, ...) LOGWT(0, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) LOGET(0, fmt, ##__VA_ARGS__)
#define LOGF(fmt, ...) LOGFT(0, fmt, ##__VA_ARGS__)

#define LOGVT(tag, fmt, ...) LOG(LOG_VERBOSE, tag, fmt, ##__VA_ARGS__)
#define LOGDT(tag, fmt, ...) LOG(LOG_DEBUG, tag, fmt, ##__VA_ARGS__)
#define LOGIT(tag, fmt, ...) LOG(LOG_INFO, tag, fmt, ##__VA_ARGS__)
#define LOGWT(tag, fmt, ...) LOG(LOG_WARN, tag, fmt, ##__VA_ARGS__)
#define LOGET(tag, fmt, ...) LOG(LOG_ERROR, tag, fmt, ##__VA_ARGS__)
#define LOGFT(tag, fmt, ...) LOG(LOG_FATAL, tag, fmt, ##__VA_ARGS__)

#ifdef __cplusplus
extern "C" {
#endif

void logPrint(int prio, const char* filename, int lineNumber, const char *tag, const char *fmt, ...)
#if defined(__GNUC__)
		__attribute__ ((format(printf, 5, 6)))
#endif
		;

#ifdef __cplusplus
}
#endif

#endif //GLSLSCENE_CAPI_LOG_H
