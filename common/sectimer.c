#include "../uip/clock.h"
#include "sectimer.h"


void sectimer_set(struct sectimer *t, uint32_t seconds) {
    timer_set(&t->timer, CLOCK_SECOND);
    t->seconds = seconds;
    t->elapsed = 0;
}

void sectimer_reset(struct sectimer *t) {
    timer_reset(&t->timer);
    t->elapsed = 0;
}

bool sectimer_tryrestart(struct sectimer *t) {
    if (timer_tryrestart(&t->timer)) {
        t->elapsed++;
        if (t->elapsed >= t->seconds) {
            t->elapsed = 0;
            return true;
        }
    }
    return false;
}
