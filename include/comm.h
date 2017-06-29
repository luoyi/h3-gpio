#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif


#ifndef LOG_FUNC_V
#define LOG_FUNC_V vfprintf
#endif

#ifndef LOG_FUNC
#define LOG_FUNC fprintf
#endif

#ifndef LOG_HANDLE
#define LOG_HANDLE stderr
#endif

#define LOG_TIME LOG_FUNC(LOG_HANDLE, "[%s] ", get_time())
#define LOG_POS LOG_FUNC(LOG_HANDLE, "%s,%d: ", __FILE__, __LINE__)
#define LOG_ERRNO LOG_FUNC(LOG_HANDLE, "[errno:%d:%s] ", errno, strerror(errno))
#define LOG_ERRORF LOG_TIME;LOG_POS;log_errorf
#define LOG_ERRORFEX LOG_TIME;LOG_POS;LOG_ERRNO;log_errorf
/*
#define LOG_ERRORF(...) do{LOG_TIME;\
LOG_POS;\
LOG_FUNC(LOG_HANDLE, __VA_ARGS__);\
LOG_FUNC(LOG_HANDLE, "\n");\
}while(0)
*/
#define LOG_ERROR(msg) do{LOG_ERRORF(msg);}while(0)

#ifdef  QP_DEBUG
#define DBG_MSG LOG_ERRORF
#else
#define DBG_MSG log_null
#endif

#define FATAL_ERRORF LOG_TIME;LOG_POS;log_fatal_errorf

static inline char *
get_time()
{
	time_t t;
	char *time_string;
                      
	t = time(NULL);   
	time_string = ctime(&t);
	time_string[strlen(time_string) - 1] = '\0';
	return (time_string);
}

static inline void
log_errorf(const char *msg, ...)
{
	va_list arg;

	va_start(arg, msg);
	LOG_FUNC_V(LOG_HANDLE, msg, arg);
	va_end(arg);

	LOG_FUNC(LOG_HANDLE, "\n");
}

static inline void
log_fatal_errorf(const char *msg, ...)
{
	va_list arg;

	va_start(arg, msg);
	LOG_FUNC_V(LOG_HANDLE, msg, arg);
	va_end(arg);

	LOG_FUNC(LOG_HANDLE, "\n");
	exit(-1);
}

static inline void 
log_null(const char *msg, ...)
{
	return ;
}

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
