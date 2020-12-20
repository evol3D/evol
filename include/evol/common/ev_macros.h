/*!
 * \file ev_macros.h
 */
#pragma once

#if defined(EV_CC_MSVC)

#define EV_EXPORT
#define EV_UNUSED
#define EV_ALIGN(n)

#elif defined(EV_CC_GCC)

#define EV_EXPORT
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

#define EV_CONCAT_IMPL(a, b) a##b

/*!
 * \brief Macro to concatenate two tokens together
 */
#define EV_CONCAT(a, b) EV_CONCAT_IMPL(a, b)

#define EV_STRINGIZE_IMPL(...) #__VA_ARGS__

/*!
 * \brief Macro to wrap tokens in double quotations ( " )
 */
#define EV_STRINGIZE(...) EV_STRINGIZE_IMPL(__VA_ARGS__)
