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
#include <stdbool.h>
#include "uip/timer.h"
#include "actsig.h"

/* Static function prototypes. */

/**
 * Toggle signal pin.
 *
 * @param signal Signal object.
 */
static void _actsig_toggle(struct actsig_signal *signal);

/**
 * Set signal on.
 *
 * @param signal Signal object.
 */
static inline void _actsig_set_on(struct actsig_signal *signal);

/**
 * Set signal off.
 *
 * @param signal Signal object.
 */
static inline void _actsig_set_off(struct actsig_signal *signal);

/* Implementation. */

void actsig_init(struct actsig_signal *signal, uint8_t pin, volatile uint8_t *ddr, volatile uint8_t *port, uint16_t interval) {
    signal->pin = pin;
    signal->ddr = ddr;
    signal->port = port;
    signal->interval = interval;
    signal->normal_state = false;
    signal->is_signaling = false;

    // TODO: fix this
    timer_set(&signal->signal_timer, CLOCK_SECOND / 2);

    /* Set output. */
    *ddr |= _BV(pin);

    /* Set high state. */
    *port |= _BV(pin);
}

void actsig_notify(struct actsig_signal *signal) {
    if (!signal->is_signaling) {
        _actsig_toggle(signal);
        signal->is_signaling = true;
        timer_restart(&signal->signal_timer);
    }
}

void actsig_set_normal_on(struct actsig_signal *signal) {
    signal->normal_state = true;
    _actsig_set_on(signal);
}

void actsig_set_normal_off(struct actsig_signal *signal) {
    signal->normal_state = false;
    _actsig_set_off(signal);
}

void actsig_process(struct actsig_signal *signal) {
    if (signal->is_signaling && timer_expired(&signal->signal_timer)) {
        _actsig_toggle(signal);
        signal->is_signaling = false;
    }
}

void _actsig_toggle(struct actsig_signal *signal) {
    if (signal->normal_state) {
        /* Signal is normaly on. */
        if (signal->is_signaling) {
            /* Signal is currently off. */
            _actsig_set_on(signal);
        } else {
            /* Signal is currently on. */
            _actsig_set_off(signal);
        }
    } else {
        /* Signal is normaly off. */
        if (signal->is_signaling) {
            /* Signal is currently on. */
            _actsig_set_off(signal);
        } else {
            /* Signal is currently off. */
            _actsig_set_on(signal);
        }
    }
}

static inline void _actsig_set_on(struct actsig_signal *signal) {
    *signal->port &= ~(_BV(signal->pin));
}

static inline void _actsig_set_off(struct actsig_signal *signal) {
    *signal->port |= _BV(signal->pin);
}
