/*
 * Copyright (C) Ivo Slanina <ivo.slanina@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
