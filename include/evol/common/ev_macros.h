/*!
 * \file ev_macros.h
 */
#pragma once

#include <buildconfig.h>

/*!
 * \brief Macro to get a type's alignment
 */
#define EV_ALIGNOF _Alignof

/*!
 * \brief Attribute to specify the minimum alignment of a variable or structure
 */
#define EV_ALIGNAS _Alignas

#if defined(EV_CC_MSVC)

#define EV_EXPORT __declspec(dllexport)
#define EV_IMPORT __declspec(dllimport)

#define EV_UNUSED

#elif defined(EV_CC_GCC)

#define EV_EXPORT
#define EV_IMPORT

#define EV_UNUSED __attribute__((unused))

#else

/*!
 * \brief Attribute to export a function from a DLL
 */
#define EV_EXPORT

/*!
 * \brief Attribute to silence compiler warnings for unused functions/variables
 */
#define EV_UNUSED

#endif

#define EV_UNUSED_PARAM(A) (void)A

#if defined(EV_CORE_FILE)
#define EVCOREAPI EV_EXPORT
#else
#define EVCOREAPI EV_IMPORT
#endif

#if defined(EV_MODULE_DEFINE)
#define EVMODAPI EV_EXPORT
#else
#define EVMODAPI
#endif

#define EV_CONCAT_IMPL2(a, b) a##b
#define EV_CONCAT_IMPL(a, b) EV_CONCAT_IMPL2(a,b)

/*!
 * \brief Macro to concatenate two tokens together
 */
#define EV_CONCAT(a, b) EV_CONCAT_IMPL(a, b)

#define EV_STRINGIZE_IMPL(...) #__VA_ARGS__

/*!
 * \brief Macro to wrap tokens in double quotations ( " )
 */
#define EV_STRINGIZE(...) EV_STRINGIZE_IMPL(__VA_ARGS__)

/*!
 * \brief Macro to create constant string values that have the same variable name and value
 */
#define STRING_VAR(x) static const char *x = EV_STRINGIZE(x)

#if (defined(EV_BUILD_DEBUG) || defined(EV_BUILD_DEBUGOPT))
#define DEBUG(x) x
#else
/*!
 * \brief Code that is only included when compiled in debug mode.
 */
#define DEBUG(x)
#endif

#define EV_VA_ARGS_NARG(...) EV_VA_ARGS_NARG_IMPL(__VA_ARGS__, EV_VA_ARGS_RSEQ_N())
#define EV_VA_ARGS_NARG_IMPL(...) EV_VA_ARGS_ARG_N(__VA_ARGS__)
#define EV_VA_ARGS_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, N, ...) N
#define EV_VA_ARGS_RSEQ_N() 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define EV_EXPAND(...) __VA_ARGS__

#define FOREACH_LAST1(OP, LAST_OP, a, ...) LAST_OP(a)
#define FOREACH_LAST2(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST1(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST3(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST2(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST4(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST3(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST5(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST4(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST6(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST5(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST7(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST6(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST8(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST7(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST9(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST8(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST10(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST9(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST11(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST10(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST12(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST11(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST13(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST12(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST14(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST13(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST15(OP, LAST_OP, a, ...) OP(a) FOREACH_LAST14(OP, LAST_OP, __VA_ARGS__)
#define FOREACH_LAST(OP, LAST_OP, ...) EV_CONCAT(FOREACH_LAST, EV_VA_ARGS_NARG(__VA_ARGS__))(OP, LAST_OP, __VA_ARGS__)

#define FOREACH1(OP, a, ...) OP(a)
#define FOREACH2(OP, a, ...) OP(a) FOREACH1(OP, __VA_ARGS__)
#define FOREACH3(OP, a, ...) OP(a) FOREACH2(OP, __VA_ARGS__)
#define FOREACH4(OP, a, ...) OP(a) FOREACH3(OP, __VA_ARGS__)
#define FOREACH5(OP, a, ...) OP(a) FOREACH4(OP, __VA_ARGS__)
#define FOREACH6(OP, a, ...) OP(a) FOREACH5(OP, __VA_ARGS__)
#define FOREACH7(OP, a, ...) OP(a) FOREACH6(OP, __VA_ARGS__)
#define FOREACH8(OP, a, ...) OP(a) FOREACH7(OP, __VA_ARGS__)
#define FOREACH9(OP, a, ...) OP(a) FOREACH8(OP, __VA_ARGS__)
#define FOREACH10(OP, a, ...) OP(a) FOREACH9(OP, __VA_ARGS__)
#define FOREACH11(OP, a, ...) OP(a) FOREACH10(OP, __VA_ARGS__)
#define FOREACH12(OP, a, ...) OP(a) FOREACH11(OP, __VA_ARGS__)
#define FOREACH13(OP, a, ...) OP(a) FOREACH12(OP, __VA_ARGS__)
#define FOREACH14(OP, a, ...) OP(a) FOREACH13(OP, __VA_ARGS__)
#define FOREACH15(OP, a, ...) OP(a) FOREACH14(OP, __VA_ARGS__)
#define FOREACH(OP, ...) EV_CONCAT(FOREACH, EV_VA_ARGS_NARG(__VA_ARGS__))(OP, __VA_ARGS__)

#define EV_TYPE_NAME(Type) EV_CONCAT(EV_TYPE_, Type)
#define EV_TYPE_NEW(Type) EV_CONCAT(EV_TYPE_NEW_, Type)
#define EV_TYPE_FREE(Type) EV_CONCAT(EV_TYPE_FREE_, Type)

#define CONFIG_ENTRY(config_name, store_name, Type, defval) do { \
    Type var = NULL; \
    ev_lua_getvar_s(config_name, var); \
    evstore_set(GLOBAL_STORE, &(evstore_entry_t){ \
      .key = store_name, \
      .type = EV_TYPE_NAME(Type), \
      .data = EV_TYPE_NEW(Type)((var == NULL)?defval:var), \
      .free = EV_TYPE_FREE(Type), \
    }); \
  } while(0);

#if defined(EV_OS_WINDOWS)
# define EV_BREAK_IF(cond) cond ? __debugbreak():0
#elif defined(EV_OS_LINUX)
#include <signal.h>
# define _EV_BREAK_IF(cond) cond ? raise(SIGTRAP):0
#else
# define _EV_BREAK_IF(cond)
#endif

#if (defined(EV_BUILD_DEBUG) || defined(EV_BUILD_DEBUGOPT))
# define EV_DEBUG_BREAK_IF _EV_BREAK_IF
#else
# define EV_DEBUG_BREAK_IF
#endif

#if (defined(EV_BUILD_DEBUG) || defined(EV_BUILD_DEBUGOPT))
#include <assert.h>
# define DEBUG_ASSERT assert
#else
# define DEBUG_ASSERT
#endif

# define UNIMPLEMENTED() assert(!"Hit an unimplemented path")
