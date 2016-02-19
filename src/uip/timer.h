/**
 * \defgroup timer Timer library
 *
 * The timer library provides functions for setting, resetting and
 * restarting timers, and for checking if a timer has expired. An
 * application must "manually" check if its timers have expired; this
 * is not done automatically.
 *
 * A timer is declared as a \c struct \c timer and all access to the
 * timer is made by a pointer to the declared timer.
 *
 * \note The timer library uses the \ref clock "Clock library" to
 * measure time. Intervals should be specified in the format used by
 * the clock library.
 *
 * @{
 */

#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdbool.h>
#include "clock.h"

/**
 * A timer.
 *
 * This structure is used for declaring a timer. The timer must be set
 * with timer_set() before it can be used.
 *
 * \hideinitializer
 */
struct timer {
    clock_time_t start;
    clock_time_t interval;
};

/**
 * Initiate timer.
 *
 * @param timer Timer object.
 * @param time Timer interval.
 */
void timer_set(struct timer *timer, clock_time_t interval);

inline void timer_reset(struct timer *timer);

inline bool timer_tryreset(struct timer *timer);

inline void timer_restart(struct timer *timer);

inline bool timer_tryrestart(struct timer *timer);

/**
 * Check if timer has expired.
 *
 * @return true if expired, false otherwise.
 */
inline bool timer_expired(struct timer *timer);

#endif /* __TIMER_H__ */

/** @} */
