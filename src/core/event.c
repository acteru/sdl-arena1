/*
 * event.c
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
 *  @addtogroup event
 *  @{
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "list.h"
#include "event.h"



#define EVENT_NAME_MAX_LENGTH		32		/**< the maximum length of an event name (including null-terminator) */

/**
 *  A struct which holds several informations about an event.
 *
 *  @private
 */
typedef struct {
	char			 	 name[EVENT_NAME_MAX_LENGTH];	/**< name of the event */
	unsigned long	 	 raise_count;					/**< number of times this event was raised */
	List				*l_event_handlers;				/**< list containing event handler informations */
} EventInfo;

/**
 *  A struct which holds several informations about an event handler.
 *
 *  @private
 */
typedef struct {
	int				 	 id;			/**< id of this event handler */
	int					 priority;		/**< priority of this event handler */
	EventInfo			*event;			/**< event to which this event handler belongs to */
	EventHandler 	 	 handler;		/**< a pointer to the event handler function itself */
	EventHandlerState	 state;			/**< current state of this handler */
	void 				*user_data;		/**< user supplied data which is passed to the event handler */
} EventHandlerInfo;

static List 	*l_events = NULL;		/**< list which contains all existing events */
static int		 next_id = 1;			/**< the id of the next event handler created by the event_connect() function */

// --- Static Functions -------------------------------------------------------

/**
 *  A compare function used to find an event with list_find_custom().
 */
static int _event_find_event(void *event, void *name)
{
	return strcmp(((EventInfo *)event)->name, (char *)name);
}

/**
 *  A compare function used to find an event handler with list_find_custom().
 */
static int _event_find_handler(void *handler, void *id)
{
	return ((EventHandlerInfo *)handler)->id - *((int *)id);
}

/**
 *  A compare function used to insert event handlers with list_insert_sorted().
 */
static int _event_insert_handler(void *a, void *b)
{
	return ((EventHandlerInfo *)b)->priority - ((EventHandlerInfo *)a)->priority;
}

/**
 *  Gets the list element in the l_events list which holds the EventInfo of a specified event.
 *
 *  @param name		name of the event
 *
 *  @returns		the list element or NULL if it couldn't be found.
 */
static List * _event_get_event_link(char *name)
{
	return list_find_custom(l_events, name, _event_find_event);
}

/**
 *  Gets the EventInfo of a specified event stored in the l_events list.
 *
 *  @param name		name of the event
 *
 *  @returns		the EventInfo or NULL if it couldn't be found.
 */
static EventInfo * _event_get_event(char *name)
{
	List *link;

	// Use function to get the link and then return its data.
	link = _event_get_event_link(name);
	return (link) ? (EventInfo *)link->data : NULL;
}

/**
 *  Gets the list element in the l_event_handlers list of the owning event which holds the
 *  EventHandlerInfo of a specified event handler.
 *
 *  @param id		id of the event handler
 *
 *  @returns		the list element or NULL if it couldn't be found.
 */
static List * _event_get_handler_link(int id)
{
	List *event_link;
	List *handler_link;

	EventInfo *event;

	// Iterate through all events
	event_link = list_first(l_events);
	while (event_link) {
		event = (EventInfo *)event_link->data;

		// Try to find the event handler in the list of this event
		handler_link = list_find_custom(event->l_event_handlers, &id, _event_find_handler);
		if (handler_link)
			return handler_link;

		event_link = list_next(event_link);
	}

	// We couldn't find it.
	return NULL;
}

/**
 *  Gets the EventHandlerInfo of a specified event handler stored in the l_event_handlers
 *  list of any event in the l_events list.
 *
 *  @param			id of the event handler
 *
 *  @returns		the EventHandlerInfo or NULL if it couldn't be found.
 */
static EventHandlerInfo * _event_get_handler(int id)
{
	List *link;

	// Use function to get the link and then return its data.
	link = _event_get_handler_link(id);
	return (link) ? (EventHandlerInfo *)link->data : NULL;
}

// --- Public Functions -------------------------------------------------------

/**
 *  Initializes this module.
 */
void event_init()
{
	// We don't have to do something here. This function is just to adhere to
	// our convention that every module must have an init() and a destroy()
	// function.
	return;
}

/**
 *  Destroys this module freeing any allocated data.
 */
void event_destroy()
{
	List *link;
	EventInfo *event;

	// Iterate through all events
	link = list_first(l_events);
	while (link) {
		event = (EventInfo *)link->data;

		// Clear the entire list of handlers, freeing the data too.
		list_free_full(event->l_event_handlers, free);

		link = list_next(link);
	}

	// Clear the entire list of events, freeing the data too.
	list_free_full(l_events, free);
}

/**
 *  Connects an event handler function to an event. If the event handler is enabled,
 *  this function will be called anytime the event_raise() function is called
 *  for this event.
 *
 *  @attention	Under no circumstances the event handler function should block for a long time
 *  			because no other events can be processed as long as an event handler is running!
 *
 *  @note		The event handlers with the highest priority values are called first.
 *  			In order to use this functionality properly a convention for this event should
 *  			be made. If there is no convention, you should use 0 as priority.
 *
 *  @param name				name of the event
 *  @param priority			a priority value
 *  @param handler			pointer to the event handler function
 *  @param user_data		data which will be passed to the event handler function
 *  @param handler_state	the initial state of this event handler
 *
 *  @returns				the id of the new event handler
 */
int event_connect(char *name, int priority, EventHandler handler, void *user_data, EventHandlerState handler_state)
{
	EventInfo *event;

	// Check whether the passed name is too long for our struct.
	if (strlen(name) > EVENT_NAME_MAX_LENGTH - 1)
		return 0;

	// Get the event the caller want to connect to. If we can't find it, we create a new one.
	event = _event_get_event(name);
	if (!event) {
		event = (EventInfo *)malloc(sizeof(EventInfo));
		strcpy(event->name, name);
		event->l_event_handlers = NULL;
		event->raise_count = 0;
		l_events = list_append(l_events, event);
	}

	// Initialize a new struct containing information about the event handler.
	EventHandlerInfo *handler_info = (EventHandlerInfo *)malloc(sizeof(EventHandlerInfo));
	handler_info->id = next_id++;
	handler_info->priority = priority;
	handler_info->event = event;
	handler_info->state = handler_state;
	handler_info->handler = handler;
	handler_info->user_data = user_data;

	// Insert the handler into the list.
	event->l_event_handlers = list_insert_sorted(event->l_event_handlers, handler_info, _event_insert_handler);

	// Return the generated ID.
	return handler_info->id;
}

/**
 *  Changes the state of a specified event handler. This function doesn't do
 *  anything, if no event handler with the specified id exists.
 *
 *  @param id					id of the event handler
 *  @param handler_state		the new state of the event handler
 */
void event_handler_set_state(int id, EventHandlerState handler_state)
{
	EventHandlerInfo *handler;

	handler = _event_get_handler(id);

	if (handler)
		handler->state = handler_state;
}

/**
 *  Updates the user data of a specified event handler. This function
 *  doesn't do anything, if no event handler with the specified id exists.
 *
 *  @param id					id of the event handler
 *  @param user_data			the new user data
 */
void event_handler_set_user_data(int id, void *user_data)
{
	EventHandlerInfo *handler;

	handler = _event_get_handler(id);

	if (handler)
		handler->user_data = user_data;
}

/**
 *  Disconnects a specified event handler. After calling this function
 *  the event handler no longer exists, so the id is no longer valid.
 *  This function doesn't do anything, if no event handler with the specified
 *  id exists.
 *
 *  @param id		id of the event handler
 */
void event_disconnect(int id)
{
	List *link;
	EventHandlerInfo *handler;
	EventInfo *event;

	// Get the link of the list representing the appropriate handler.
	// If we couldn't find it, we can return.
	link = _event_get_handler_link(id);
	if (!link)
		return;

	// If we have the link, we can gather the other interesting data.
	handler = (EventHandlerInfo *)link->data;
	event = handler->event;

	// Remove the link from the list of event handlers and free the data.
	// Even if the new list contains no handlers, we are going to keep
	// the EventInfo, maybe someone wants to connect again later.
	event->l_event_handlers = list_delete_link(event->l_event_handlers, link);
	free(handler);
}

/**
 *  Raises a specified event. Every event handler which is connected
 *  to this event and which is enabled will be called passing the
 *  given event_data to it.
 *
 *  @note This function is blocking until all event handlers are called.
 *
 *  @param name				name of the event
 *  @param event_data		data which will be passed to the event handler functions
 */
void event_raise(char *name, void *event_data)
{
	List *event_link;
	List *prev_event_link;
	List *handler_link;

	EventInfo *event;
	EventInfo *prev_event;
	EventHandlerInfo *handler;

	// First we have to get the event, if we don't find it, we can return.
	event_link = _event_get_event_link(name);
	if (!event_link)
		return;
	event = (EventInfo *)event_link->data;

	// Now iterate through every event handler
	handler_link = list_first(event->l_event_handlers);
	while (handler_link) {
		handler = (EventHandlerInfo *)handler_link->data;

		// If the handler is enabled, call the event handler function with
		// the appropriate arguments.
		if (handler->state == EVENT_HANDLER_ENABLED) {
			handler->handler(event_data, handler->user_data);
		}

		handler_link = list_next(handler_link);
	}

	// Increase the counter
	event->raise_count++;

	// Check whether the event before this one in the event list has a smaller raise_count.
	// If yes, exchange the positions. This way the list will always be sorted, so that
	// often raised event are found quicker in the list.
	prev_event_link = list_prev(event_link);
	if (prev_event_link) {
		prev_event = (EventInfo *)list_prev(event_link)->data;

		if (prev_event->raise_count < event->raise_count) {
			prev_event_link->data = event;
			event_link->data = prev_event;
		}
	}
}

/**
 *  Prints the current structure of events to stdout.
 *  This may be useful for debugging purposes.
 */
void event_print_structure()
{
	List *event_link, *handler_link;

	EventInfo *event_info;
	EventHandlerInfo *handler_info;

	// Loop trough all events
	event_link = list_first(l_events);
	while (event_link) {
		event_info = (EventInfo *)event_link->data;

		printf("%s (raise count: %lu)\n", event_info->name, event_info->raise_count);

		// Loop trough all handlers
		handler_link = list_first(event_info->l_event_handlers);
		while (handler_link) {
			handler_info = (EventHandlerInfo *)handler_link->data;

			printf("-> id: %3d  priority: %2d  state: %1d  handler: 0x%p\n",
					handler_info->id,
					handler_info->priority,
					handler_info->state,
					handler_info->handler);

			handler_link = list_next(handler_link);
		}

		event_link = list_next(event_link);
	}
}

/** @} */
