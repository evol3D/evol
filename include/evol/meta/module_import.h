/*!
 * \file module_import.h
 */
#include <evol/common/ev_macros.h>
#include <evol/common/ev_types.h>

#if !defined(IMPORT_MODULE)
#error "Either IMPORT_MODULE need to be defined"
#endif

# define TYPE_MODULE IMPORT_MODULE
# define NAMESPACE_MODULE IMPORT_MODULE
# define EVENT_MODULE IMPORT_MODULE

#include <evol/meta/type_import.h>
#include <evol/meta/namespace_import.h>
#include <evol/meta/event_include.h>

#undef IMPORT_MODULE
