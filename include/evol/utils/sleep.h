/*!
 * \file sleep.h
 */
#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <evol/common/ev_macros.h>

/*!
 * \brief A function that suspends the thread for a given amount of milliseconds
 * \param milliseconds The amount of milliseconds to sleep
 */
EVCOREAPI void
sleep_ms(
    double milliseconds);

#if defined(__cplusplus)
}
#endif
