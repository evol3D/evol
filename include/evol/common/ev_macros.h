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

#define EV_EXPORT
#define EV_UNUSED
#define EV_ALIGN(n)

#endif

#define EV_CONCAT_IMPL(a, b) a##b
#define EV_CONCAT(a, b) EV_CONCAT_IMPL(a, b)

#define EV_STRINGIZE_IMPL(...) #__VA_ARGS__
#define EV_STRINGIZE(...) EV_STRINGIZE_IMPL(__VA_ARGS__)
