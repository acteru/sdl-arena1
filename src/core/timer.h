/*
 * timer.h
 * This file is part of Arena1
 *
 * Copyright (C) 2013
 *
 * Arena1 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Arena1 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Arena1. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 *  @defgroup timer timer
 *  @brief Simplifies the use of timers provided by the SDL library.
 *
 *  This module simplifies the use of the timers provided by the SDL
 *  library. It's main purpose is to ensure that the handler functions
 *  of the timers are called in the main thread. This is done by
 *  pushing an SDL user event if a timer has elapsed. This causes the main
 *  function to raise a normal event ("sdl-user") which is processed by
 *  this module directing the call to the timer handler function.
 *
 *  @{
 */

#ifndef TIMER_H_
#define TIMER_H_

/**
 *  Prototype for a timer handler function.
 *
 *  @param user_data		data which was passed to the timer_create() function.
 */
typedef void (*TimerHandler)(void *user_data);

/**
 *  The state of a timer.
 */
typedef enum {
	TIMER_DISABLED = 0x00,		/**< the timer is disabled and currently not running. */
	TIMER_ENABLED  = 0x01,		/**< the timer is enabled and currently running. */
} TimerState;

extern void timer_init();
extern void timer_destroy();

extern int timer_create(int interval, TimerHandler handler, void *user_data, TimerState state);
extern void timer_set_state(int id, TimerState state);
extern void timer_set_interval(int id, int interval);
extern void timer_set_user_data(int id, void* user_data);
extern void timer_free(int id);

#endif /* TIMER_H_ */

/** @} */
