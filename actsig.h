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

#ifndef __ACTSIG_H__
#define __ACTSIG_H__

#include <stdbool.h>
#include "uip/timer.h"

/**
 * signal structure.
 */
struct actsig_signal {
    uint8_t pin;                /**< Signal pin. */
    volatile uint8_t *ddr;      /**< Signal DDR register. */
    volatile uint8_t *port;     /**< Signal port register. */
    uint16_t interval;          /**< Signaling interval. */
    bool normal_state;          /**< Signal normal state */
    bool is_signaling;          /**< Is currently signaling. */
    struct timer signal_timer;  /**< Notify timer. */
};

void actsig_init(struct actsig_signal *signal, uint8_t pin, volatile uint8_t *ddr, volatile uint8_t *port, uint16_t interval);
void actsig_notify(struct actsig_signal *signal);
void actsig_set_normal_on(struct actsig_signal *signal);
void actsig_set_normal_off(struct actsig_signal *signal);
void actsig_process(struct actsig_signal *signal);

#endif
