#ifndef __SECTIMER_H__
#define __SECTIMER_H__

#include "../uip/timer.h"

struct sectimer {
    struct timer timer;
    uint32_t seconds;
    uint32_t elapsed;
};

void sectimer_set(struct sectimer *t, uint32_t seconds);
void sectimer_reset(struct sectimer *t);
bool sectimer_tryrestart(struct sectimer *t);

#endif
