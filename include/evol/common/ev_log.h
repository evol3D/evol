/**
 * Copyright (c) 2020 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <evol/common/ev_macros.h>

#define LOG_VERSION "0.1.0"

typedef struct {
  va_list     ap;
  const char *fmt;
  const char *file;
  struct tm * time;
  void *      udata;
  int         line;
  int         level;
} log_Event;

typedef void (*log_LogFn)(log_Event *ev);
typedef void (*log_LockFn)(bool lock, void *udata);

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define ev_log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define ev_log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

EVCOREAPI const char *
log_level_string(int level);
EVCOREAPI void
log_set_lock(log_LockFn fn, void *udata);
EVCOREAPI void
log_set_level(int level);
EVCOREAPI void
log_set_quiet(bool enable);
EVCOREAPI int
log_add_callback(log_LogFn fn, void *udata, int level);
EVCOREAPI int
log_add_fp(FILE *fp, int level);

EVCOREAPI void
log_log(int level, const char *file, int line, const char *fmt, ...);
