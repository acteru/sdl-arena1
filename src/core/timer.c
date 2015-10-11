/*
 * timer.c
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
 *  @addtogroup timer
 *  @{
 */

#include <stdlib.h>
#include <SDL/SDL.h>
#include "list.h"
#include "event.h"
#include "timer.h"

#include "debug.h"


/**
 *  A struct which holds several informations about a timer.
 */
typedef struct {
	int 			 id;			/**< id of this timer */
	SDL_TimerID		 sdl_id;		/**< SDL timer id (used for SDL calls only) */
	TimerState		 state;			/**< state of this timer */
	int				 interval;		/**< interval of this timer */
	TimerHandler	 handler;		/**< timer handler function */
	void			*user_data;		/**< user supplied data which is passed to the timer handler function */
} TimerInfo;

static List 	*l_timers = NULL;	/**< list of all existing timers */
static int 		 next_id = 1;		/**< the id of the next timer created by the timer_create() function */

static int 		 evt_sdl_user;		/**< id of the sdl-user event handler */

// --- Static Functions -------------------------------------------------------

/**
 *  The callback function for all SDL timers. It pushes an SDL event
 *  so that the timer can be handled in the main thread.
 *
 *  @note	This function isn't called in the main thread!
 *
 *  @param interval		interval of the timer
 *  @param param		in our case this is a pointer to the TimerInfo struct of the timer
 *
 *  @returns			The new interval, this is the interval stored in the TimerInfo struct. Mostly, it will remain the same.
 */
static Uint32 _timer_thread_swap(Uint32 interval, void *param)
{
	SDL_Event event;
	TimerInfo *timer;

	timer = (TimerInfo *)param;

	event.user.type = SDL_USEREVENT;
	event.user.code = 1;
	event.user.data1 = timer;
	event.user.data2 = NULL;

	SDL_PushEvent(&event);

	return timer->interval;
}

/**
 *  Gets the list element in the l_timers list which holds the TimerInfo of a specified timer.
 *
 *  @param id		id of the timer
 *
 *  @returns		the list element or NULL if it couldn't be found.
 */
static List * _timer_get_link(int id)
{
	List *link;
	TimerInfo *timer;

	// Iterate through all timers
	link = list_first(l_timers);
	while (link) {
		timer = (TimerInfo *)link->data;

		// Is this the timer we are looking for? If yes, return the link.
		if (timer->id == id)
			return link;

		link = list_next(link);
	}

	// We couldn't find it
	return link;
}

/**
 *  Gets the TimerInfo of a specified timer stored in the l_timers list.
 *
 *  @param id		id of the event
 *
 *  @returns		the TimerInfo or NULL if it couldn't be found.
 */
static TimerInfo * _timer_get(int id)
{
	List *link;

	// Use function to get the link and return its data.
	link = _timer_get_link(id);
	return (link) ? (TimerInfo *)link->data : NULL;
}

/**
 *  Sets a new state for a specified timer. If this is enabling
 *  the timer, an SDL timer is created and started. If this is
 *  disabling the timer, the SDL timer is removed.
 *
 *  @param timer	TimerInfo of the timer beeing changed
 *  @param state	the new state of the timer
 */
static void _timer_set_state(TimerInfo *timer, TimerState state)
{
	// Do we have to enable the timer?
	if (timer->state == TIMER_DISABLED && state == TIMER_ENABLED) {
		timer->sdl_id = SDL_AddTimer(timer->interval, _timer_thread_swap, timer);
		timer->state = TIMER_ENABLED;
	}

	// Do we have to disable the timer?
	if (timer->state == TIMER_ENABLED && state == TIMER_DISABLED) {
		SDL_RemoveTimer(timer->sdl_id);
		timer->sdl_id = 0;
		timer->state = TIMER_DISABLED;
	}
}

/**
 *  Event handler function for the "sdl-user" event. This event handler is called
 *  if a timer has elapsed. It gets information about the elapsed timer and calls
 *  it's timer handler function.
 *
 *  @param event_data	the TimerInfo struct of the timer
 *  @param user_data	NULL
 */
static void _timer_evt_sdl_user(void *event_data, void *user_data)
{
	SDL_UserEvent *event = (SDL_UserEvent *)event_data;
	TimerInfo *timer;

	// If the event has the right code we call the
	// function which handles the elapsed timer.
	if (event->code == 1) {
		timer = (TimerInfo *)event->data1;
		timer->handler(timer->user_data);
	}
}

// --- Public Functions -------------------------------------------------------

/**
 *  Initializes this module.
 */
void timer_init()
{
	// Register events
	evt_sdl_user = event_connect("sdl-user", 0, _timer_evt_sdl_user, NULL, EVENT_HANDLER_ENABLED);
}

/**
 *  Destroys this module freeing any allocated data.
 */
void timer_destroy()
{
	List *link;
	TimerInfo *timer;

	// Unregister events
	event_disconnect(evt_sdl_user);

	// Iterate through all timers
	link = list_first(l_timers);
	while (link) {
		timer = (TimerInfo *)link->data;

		// Disable all timers
		_timer_set_state(timer, TIMER_DISABLED);

		link = list_next(link);
	}

	// Release resources
	list_free_full(l_timers, free);
}

/**
 *  Creates a new timer.
 *
 *  @attention	Under no circumstances the timer handler function should block for a long time because no other events
 *  			can be processed as long as an timer handler is running!
 *
 *  @param interval		interval of the timer
 *  @param handler		the timer handler function which will be called everytime the timer elapses
 *  @param user_data	data which will be passed to the timer handler function everytime the timer elapses
 *  @param state		the initial state of the timer
 *
 *  @returns			the id of the created timer
 */
int timer_create(int interval, TimerHandler handler, void *user_data, TimerState state)
{
	TimerInfo *timer;

	// Create a new struct containing informations about the timer
	timer = (TimerInfo *)malloc(sizeof(TimerInfo));
	timer->id = next_id++;
	timer->sdl_id = 0;
	timer->state = TIMER_DISABLED;
	timer->interval = interval;
	timer->handler = handler;
	timer->user_data = user_data;

	l_timers = list_append(l_timers, timer);

	// Apply the initial state
	_timer_set_state(timer, state);

	return timer->id;
}

/**
 *  Sets the state of a timer specified by its id.
 *  If there is no timer with the given id, this function does nothing.
 *
 *  @param id		id of the timer
 *  @param state	the new state of the timer
 */
void timer_set_state(int id, TimerState state)
{
	TimerInfo *timer;

	// Get the timer information
	timer = _timer_get(id);
	if (!timer)
		return;

	// Set the new state...
	_timer_set_state(timer, state);
}

/**
 *  Sets a new interval for an existing timer.
 *  If there is no timer with the given id, this function does nothing.
 *
 *  @param id			id of the timer
 *  @param interval		the new interval of the timer
 */
void timer_set_interval(int id, int interval)
{
	TimerInfo *timer;

	// Get the timer information
	timer = _timer_get(id);
	if (!timer)
		return;

	// Set the interval
	timer->interval = interval;
}

/**
 *  Updates the user_data for an existing timer. This data is passed to the timer handler
 *  function each time the timer elapses.
 *  If there is no timer with the given id, this function does nothing.
 *
 *  @param id			id of the timer
 *  @param user_data	the data which will be passed to the timer handler function in future
 */
void timer_set_user_data(int id, void* user_data)
{
	TimerInfo *timer;

	// Get the timer information
	timer = _timer_get(id);
	if (!timer)
		return;

	// Set the new user_data
	timer->user_data = user_data;
}

/**
 *  Disables and frees a previously created timer.
 *  If there is no timer with the given id, this function does nothing.
 *  After calling this function the given id is no longer valid.
 *
 *  @param id		id of the timer
 */
void timer_free(int id)
{
	List *link;
	TimerInfo *timer;

	// Try to find the appropriate timer
	link = _timer_get_link(id);
	if (link) {
		timer = (TimerInfo *)link->data;

		// Disable it...
		_timer_set_state(timer, TIMER_DISABLED);

		// ...and free it
		free(timer);
		l_timers = list_delete_link(l_timers, link);
	}
}

/** @} */
