/*
 * scene.c
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
 *  @addtogroup scene
 *  @{
 */

#include <stdlib.h>
#include <string.h>
#include "core.h"



static List *l_state_stack = NULL;		/**< the stack containing the scenes */

/**
 *  Initializes this module.
 */
void scene_init()
{
	// This is our first state
	scene_push("init");
}

/**
 *  Destroys this module freeing all allocated data.
 */
void scene_destroy()
{
	// Destroy the list
	list_free_full(l_state_stack, free);
}

/**
 *  Pushes a new scene onto the stack. This raises the scene-changed event.
 *
 *  @param name		name of the new scene
 */
void scene_push(char *name)
{
	char *cpy;

	// Create a copy of the name
	cpy = (char *)malloc(strlen(name) + 1);
	strcpy(cpy, name);

	// Push it onto our app state stack
	l_state_stack = list_prepend(l_state_stack, cpy);

	// Raise an event to notify about the change
	event_raise("scene-changed", cpy);
}

/**
 *  Gets the currently active scene.
 *
 *  @attention	The returned pointer is a pointer to the string on the
 *  			stack itself, do NOT modify or free it!
 *
 *  @return		currently active scene
 */
char * scene_get()
{
	// This should never happen because of the "init" state
	if (!l_state_stack)
		return NULL;

	return (char *)list_first(l_state_stack)->data;
}

/**
 *  Pops the currently active scene and thus returns to the last scene.
 *
 *  @note		The returned scene is the scene which is active after the
 *  			call of this function, not the scene which has been popped.
 *
 *  @attention	The returned pointer is a pointer to the string on the
 *  			stack itself, do NOT modify or free it!
 *
 *  @returns	the new scene (not the scene which has been popped!)
 */
char * scene_pop()
{
	List *state_link;

	// This should never happen because of the "init" state
	if (!l_state_stack)
		return NULL;

	// Pop an app state from the stack and free it
	state_link = list_first(l_state_stack);
	free(state_link->data);
	l_state_stack = list_delete_link(l_state_stack, state_link);

	char *new_scene = scene_get();

	// Raise an event to notify about the change
	event_raise("scene-changed", new_scene);

	// Hopefully not too confusing: This function does return the new app state,
	// not the app state which has been popped from the stack.
	return new_scene;
}

/**
 *  Checks, whether a specified scene is the active one.
 *  This is a convenience function which avoids using of the
 *  strcmp function of the string.h library in every module.
 *
 *  @param name		the scene to compare with
 *
 *  @returns		1, if the given scene is the active one
 */
int scene_check(char *name)
{
	return strcmp(scene_get(), name) == 0;
}

/** @} */
