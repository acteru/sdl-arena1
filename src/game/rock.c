/*
 * rock.c
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
 *  @addtogroup gfx
 *  @{
 */

#include <SDL/SDL.h>
#include "core/core.h"
#include "game.h"
#include "rock.h"



/**
 *  This struct holds the data for a rock object.
 */
typedef struct {
	GameObject base;						/**< data from the base class */
} RockObject;

static List			*l_rocks = NULL;		/**< list of all existing rock objects */
static SDL_Surface	*s_rock;				/**< rock sprite */
static int			 evt_gfx_draw;			/**< id of the gfx-draw event handler */

/**
 *  Event handler for the gfx-draw event.
 *  Draws all rocks.
 */
static void _rock_evt_gfx_draw(void *event_data, void *user_data)
{
	// Loop through all rock objects
	List *link = list_first(l_rocks);
	while (link) {
		RockObject *rock = (RockObject *)link->data;
		game_draw(s_rock, rock->base.pos, NULL);
		link = list_next(link);
	}
}

/**
 *  Initializes this module.
 */
void rock_init()
{
	// Register events
	evt_gfx_draw = event_connect("gfx-draw", 0, _rock_evt_gfx_draw, NULL, EVENT_HANDLER_ENABLED);

	// Load sprites
	s_rock = assert_sprite("sprites\\rock.png");
}

/**
 *  Destroys this module freeing any allocated data.
 */
void rock_destroy()
{
	// Free all objects
	rock_free_all();

	// Unregister events
	event_disconnect(evt_gfx_draw);

	// Free sprites
	SDL_FreeSurface(s_rock);
}

/**
 *  Creates a new rock object.
 *
 *  @param pos		initial position of the new object
 *  @returns		the newly created rock object
 */
GameObject * rock_create(Vector pos)
{
	RockObject *rock = (RockObject *)malloc(sizeof(RockObject));
	rock->base.type = OBJ_ROCK;

	game_set_field(pos, (GameObject *)rock);
	l_rocks = list_append(l_rocks, rock);

	return (GameObject *)rock;
}

/**
 *  Frees a rock object.
 *
 *  @param rock		a rock object
 */
void rock_free(GameObject *rock)
{
	game_set_field(rock->pos, NULL);
	l_rocks = list_remove(l_rocks, rock);

	free(rock);
}

/**
 *  Frees all existing rock objects.
 */
void rock_free_all()
{
	// Loop through all rock objects
	List *link = list_first(l_rocks);
	while (link) {
		RockObject *rock = (RockObject *)link->data;
		link = list_next(link);

		rock_free((GameObject *)rock);
	}
}

/** @} */
