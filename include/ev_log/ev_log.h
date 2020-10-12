#ifndef _EV_LOG_HEADER_
#define _EV_LOG_HEADER_
#include "rxi_logger/log.h"

enum {EV_LOG_TRACE, EV_LOG_DEBUG, EV_LOG_INFO, EV_LOG_WARN, EV_LOG_ERROR, EV_LOG_FATAL};

#ifdef DEBUG

#define ev_log_trace(...) log_log(EV_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_debug(...) log_log(EV_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_info(...) log_log(EV_LOG_INFO , __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_warn(...) log_log(EV_LOG_WARN , __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_error(...) log_log(EV_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_fatal(...) log_log(EV_LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#define ev_log_setlevel(...) log_set_level(__VA_ARGS__)

#else

#define ev_log_trace(...)
#define ev_log_debug(...)
#define ev_log_info(...)
#define ev_log_warn(...)
#define ev_log_error(...)
#define ev_log_fatal(...)

#define ev_log_setlevel(...)

#endif


#endif
