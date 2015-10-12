/*
 * event.h
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
 *  @defgroup event event
 *  @brief Provides basic functions to get event-driven programming functionality.
 *
 *  This module provides basic functions to get event-driven programming functionality.
 *  One can connect to a specified event by using the event_connect() function.
 *  An event is specified by any name. If someone uses the event_raise() function
 *  every event handler connected to the given event is called synchronously.
 *
 *  @{
 */

#ifndef EVENT_H_
#define EVENT_H_

/**
 *  Prototype for an event handler function.
 *
 *  @param event_data	data which was passed to the event_raise() function.
 *  @param user_data	data which was passed to the event_connect() function.
 */
typedef void (*EventHandler)(void *event_data, void *user_data);

/**
 *  The state of an event handler.
 */
typedef enum {
	EVENT_HANDLER_DISABLED = 0x00, /**< the event handler is disabled and won't be called if the event is raised. */
	EVENT_HANDLER_ENABLED  = 0x01, /**< the event handler is enabled and will be called if the event is raised. */
} EventHandlerState;

extern void event_init();
extern void event_destroy();

extern int event_connect(char *name, int priority, EventHandler handler, void *user_data, EventHandlerState handler_state);
extern void event_handler_set_state(int id, EventHandlerState handler_state);
extern void event_handler_set_user_data(int id, void *user_data);
extern void event_disconnect(int id);
extern void event_raise(char *name, void *event_data);
extern void event_print_structure();

#endif /* event_H_ */

/** @} */
