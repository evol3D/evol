/*!
 * \file module.h
 */
#pragma once

typedef void *evolmodule_t;

evolmodule_t
ev_module_open(const char *modpath);

void
ev_module_close(evolmodule_t module);

void *
ev_module_getfn(evolmodule_t module, const char *fn_name);
