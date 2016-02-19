#include <stdbool.h>
#include "clock.h"
#include "timer.h"

void timer_set(struct timer *timer, clock_time_t interval) {
    timer->interval = interval;
    timer->start = clock_time();
}

inline void timer_reset(struct timer *timer) {
    timer->start += timer->interval;
}

inline bool timer_tryreset(struct timer *timer) {
    if (timer_expired(timer)) {
        timer_reset(timer);
        return true;
    } else {
        return false;
    }
}

inline void timer_restart(struct timer *timer) {
    timer->start = clock_time();
}

inline bool timer_tryrestart(struct timer *timer) {
    if (timer_expired(timer)) {
        timer_restart(timer);
        return true;
    } else {
        return false;
    }
}

inline bool timer_expired(struct timer *timer) {
    return clock_time() - timer->start >= timer->interval;
}
