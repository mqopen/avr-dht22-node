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

#include <avr/io.h>
#include "uip/timer.h"
#include "actsig.h"

void actsig_init(struct actsig_signal *signal, uint8_t pin, volatile uint8_t *ddr, volatile uint8_t *port, uint16_t interval) {
    signal->pin = pin;
    signal->ddr = ddr;
    signal->port = port;
    signal->interval = interval;
    signal->normal_state = false;
    signal->is_signaling = false;

    timer_set(&signal->signal_timer, CLOCK_SECOND * interval / 1000);

    *ddr |= _BV(pin);
    *port |= _BV(pin);
}

void actsig_notify(struct actsig_signal *signal) {
    if (!signal->is_signaling) {
        *signal->port &= ~(_BV(signal->pin));
        signal->is_signaling = true;
        timer_restart(&signal->signal_timer);
    }
}

void actsig_set_normal_on(struct actsig_signal *signal) {

}

void actsig_set_normal_off(struct actsig_signal *signal) {
}

void actsig_process(struct actsig_signal *signal) {
    if (timer_tryrestart(&signal->signal_timer)) {
        *signal->port |= _BV(signal->pin);
        signal->is_signaling = false;
    }
}
