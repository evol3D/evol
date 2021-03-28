/*!
 * \file type_import.h
 */
#include <evol/common/ev_macros.h>
#include <evol/core/module.h>
#include <evol/core/namespace.h>

#if !defined(TYPE_MODULE)
#error "TYPE_MODULE needs to be defined"
#endif

# if defined(TYPE)
#  undef TYPE
# endif

# define TYPE(name, ...) typedef __VA_ARGS__ name;
#include EV_STRINGIZE(TYPE_MODULE/meta/evmod.types)
#undef TYPE

#undef TYPE_MODULE
