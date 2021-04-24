/*!
 * \file sleep.h
 */
#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * \brief A function that suspends the thread for a given amount of milliseconds
 * \param milliseconds The amount of milliseconds to sleep
 */
void
sleep_ms(
    double milliseconds);

#if defined(__cplusplus)
}
#endif
