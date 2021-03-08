/*!
 * \file ev_macros.h
 */
#pragma once

#if defined(_WIN32)
#define EV_OS_WINDOWS
#elif defined(__linux__)
#define EV_OS_LINUX
#else
#error("Unknown OS")
#endif

#if defined(_MSC_VER)
#define EV_CC_MSVC
#elif defined(__GNUC__)
#define EV_CC_GCC
#else
#error("Unknown Compiler")
#endif

#if defined(EV_CC_MSVC)

#define EV_EXPORT __declspec(dllexport)
#define EV_IMPORT __declspec(dllimport)

#define EV_UNUSED
#define EV_ALIGN(n)

#elif defined(EV_CC_GCC)

#define EV_EXPORT
#define EV_IMPORT

#define EV_UNUSED __attribute__((unused))
#define EV_ALIGN(n) __attribute__((aligned(x)))

#else

/*!
 * \brief Attribute to export a function from a DLL
 */
#define EV_EXPORT

/*!
 * \brief Attribute to silence compiler warnings for unused functions/variables
 */
#define EV_UNUSED

/*!
 * \brief Attribute to specify the minimum alignment of a variable or structure
 */
#define EV_ALIGN(n)

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

#if defined(DEBUG)
#define Debug if(1)
#else
/*!
 * \brief Code that executes only when compiled in debug mode.
 */
#define Debug if(0)


#define EV_SUB_CharCode_A 1
#define EV_SUB_CharCode_B 2
#define EV_SUB_CharCode_C 3
#define EV_SUB_CharCode_D 4
#define EV_SUB_CharCode_E 5
#define EV_SUB_CharCode_F 6
#define EV_SUB_CharCode_G 7
#define EV_SUB_CharCode_H 8
#define EV_SUB_CharCode_I 9
#define EV_SUB_CharCode_J 10
#define EV_SUB_CharCode_K 11
#define EV_SUB_CharCode_L 12
#define EV_SUB_CharCode_M 13
#define EV_SUB_CharCode_N 14
#define EV_SUB_CharCode_O 15
#define EV_SUB_CharCode_P 16
#define EV_SUB_CharCode_Q 17
#define EV_SUB_CharCode_R 18
#define EV_SUB_CharCode_S 19
#define EV_SUB_CharCode_T 20
#define EV_SUB_CharCode_U 21
#define EV_SUB_CharCode_V 22
#define EV_SUB_CharCode_W 23
#define EV_SUB_CharCode_X 24
#define EV_SUB_CharCode_Y 25
#define EV_SUB_CharCode_Z 26
#define EV_CHARCODE(L) EV_CONCAT(EV_SUB_CharCode, L)

#define EV_VA_ARGS_NARG(...) EV_VA_ARGS_NARG_IMPL(__VA_ARGS__, EV_VA_ARGS_RSEQ_N())
#define EV_VA_ARGS_NARG_IMPL(...) EV_VA_ARGS_ARG_N(__VA_ARGS__)
#define EV_VA_ARGS_ARG_N(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, N, ...) N
#define EV_VA_ARGS_RSEQ_N() 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

#define EV_SUB_EACH_1(I, ...) EV_CHARCODE(I)
#define EV_SUB_EACH_2(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_1(__VA_ARGS__))
#define EV_SUB_EACH_3(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_2(__VA_ARGS__))
#define EV_SUB_EACH_4(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_3(__VA_ARGS__))
#define EV_SUB_EACH_5(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_4(__VA_ARGS__))
#define EV_SUB_EACH_6(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_5(__VA_ARGS__))
#define EV_SUB_EACH_7(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_6(__VA_ARGS__))
#define EV_SUB_EACH_8(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_7(__VA_ARGS__))
#define EV_SUB_EACH_9(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_8(__VA_ARGS__))
#define EV_SUB_EACH_10(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_9(__VA_ARGS__))
#define EV_SUB_EACH_11(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_10(__VA_ARGS__))
#define EV_SUB_EACH_12(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_11(__VA_ARGS__))
#define EV_SUB_EACH_13(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_12(__VA_ARGS__))
#define EV_SUB_EACH_14(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_13(__VA_ARGS__))
#define EV_SUB_EACH_15(I, ...) EV_CONCAT(EV_CHARCODE(I), EV_SUB_EACH_14(__VA_ARGS__))

#define EV_SUB_EACH_IMPL(N, ...) EV_CONCAT(EV_SUB_EACH_, N)(__VA_ARGS__)
#define EV_SUB_EACH(...) EV_SUB_EACH_IMPL(EV_VA_ARGS_NARG(__VA_ARGS__), __VA_ARGS__)

#define EV_MOD_ID_EQ(A, B) EV_SUB_EACH(A) == EV_SUB_EACH(B)

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
#endif

#if defined(EV_OS_WINDOWS)
# define EV_BREAK_IF(cond) cond ? __debugbreak():0
#elif defined(EV_OS_LINUX)
#include <signal.h>
# define EV_BREAK_IF(cond) cond ? raise(SIGTRAP):0
#else
# define EV_BREAK_IF(cond)
#endif

#if defined(DEBUG)
# define EV_DEBUG_BREAK_IF EV_BREAK_IF
#else
# define EV_DEBUG_BREAK_IF
#endif
