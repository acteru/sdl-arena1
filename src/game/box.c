/*
 * box.c
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
#include "box.h"

/**
 *  This struct holds the data for a box object.
 */
typedef struct {
	GameObject base;							/**< data from the base class */
	GameObject *content;						/**< the game object which is in the box. may be NULL or an upgrade */
	int sprite;									/**< current sprite index used by the box */
} BoxObject;

static List			*l_boxes = NULL;			/**< list of all existing box objects */

static SDL_Surface	*s_box;						/**< box sprites */
static SDL_Rect		 s_box_clips[7];			/**< clips for box sprites */

static int			 evt_gfx_draw;				/**< id of the gfx-draw event handler */
static int			 evt_explosion_hit;			/**< id of the explosion-hit event handler */

static int			 tmr_step;					/**< id of the timer which is used to create explosion animation */

/**
 *  Event handler for the gfx-draw event.
 *  Draws all boxes.
 */
static void _box_evt_gfx_draw(void *event_data, void *user_data)
{
	// Loop through all box objects
	List *link = list_first(l_boxes);
	while (link) {
		BoxObject *box = (BoxObject *)link->data;
		game_draw(s_box, box->base.pos, &s_box_clips[box->sprite]);
		link = list_next(link);
	}
}

/**
 *  Event handler for the explosion-hit event.
 *  If a box is hit by an explosion, it is removed here.
 */
static void _box_evt_explosion_hit(void *event_data, void *user_data)
{
	GameObject *obj = (GameObject *)event_data;

	if (obj->type == OBJ_BOX) {
		BoxObject *box = (BoxObject *)obj;

		// Initialize the animation. The box is freed if animation has finished.
		if (box->sprite == 0) {
			box->sprite = 1;
		}
	}
}

/**
 *  Timer callback function for the timer step.
 *  Used for animation while being destroyed by an explosion.
 */
static void _box_tmr_step(void *user_data)
{
	// Loop through all box objects
	List *link = list_first(l_boxes);
	while (link) {
		BoxObject *box = (BoxObject *)link->data;
		link = list_next(link);

		// Update the sprite being used
		if (box->sprite > 0) {
			box->sprite++;
			if (box->sprite > 7) {
				// Get content
				GameObject *content = box->content;
				Vector pos = box->base.pos;

				// Free the box
				box->content = NULL;
				box_free((GameObject *)box);

				// Unpack content
				if (content) {
					game_set_field(pos, content);
				}
			}
		}
	}
}

/**
 *  Initializes this module.
 */
void box_init()
{
	// Register events
	evt_gfx_draw = event_connect("gfx-draw", 0, _box_evt_gfx_draw, NULL, EVENT_HANDLER_ENABLED);
	evt_explosion_hit = event_connect("explosion-hit", 0, _box_evt_explosion_hit, NULL, EVENT_HANDLER_ENABLED);

	// Create timers
	tmr_step = timer_create(100, _box_tmr_step, NULL, TIMER_ENABLED);

	// Load sprites
	s_box = assert_sprite("sprites\\box.png");

	// Initialize clips
	Size clip_size = { 60, 60 };
	sprite_get_clips(s_box_clips, clip_size, 7);
}

/**
 *  Destroys this module freeing any allocated data.
 */
void box_destroy()
{
	// Free all objects
	box_free_all();

	// Unregister events
	event_disconnect(evt_gfx_draw);
	event_disconnect(evt_explosion_hit);

	// Free timers
	timer_free(tmr_step);

	// Free sprites
	SDL_FreeSurface(s_box);
}

/**
 *  Creates a new box object.
 *
 *  @param pos		initial position of the new object
 *  @returns		the newly created box object
 */
GameObject * box_create(Vector pos)
{
	BoxObject *box = (BoxObject *)malloc(sizeof(BoxObject));
	box->base.type = OBJ_BOX;
	box->content = NULL;
	box->sprite = 0;

	game_set_field(pos, (GameObject *)box);
	l_boxes = list_append(l_boxes, box);

	return (GameObject *)box;
}

/**
 *  Frees a box object.
 *  If the box object has a content, this object will be freed too.
 *
 *  @param box		a box object
 */
void box_free(GameObject *box)
{
	BoxObject *obj = (BoxObject *)box;

	game_set_field(obj->base.pos, NULL);
	l_boxes = list_remove(l_boxes, obj);

	if (obj->content) {
		game_free_object(obj->content);
	}

	free(obj);
}

/**
 *  Frees all existing box objects.
 */
void box_free_all()
{
	// Loop through all box objects
	List *link = list_first(l_boxes);
	while (link) {
		BoxObject *box = (BoxObject *)link->data;
		link = list_next(link);

		box_free((GameObject *)box);
	}
}

/**
 *  Distribute content to random boxes.
 *  If there is no empty box available, this function does nothing.
 *
 *  @param content		array of content to distribute
 *  @param n_content	number of objects in the array
 *  @returns			number of distributed objects
 */
int box_distribute(GameObject *content[], int n_content)
{
	BoxObject *box;

	// Create a list of all empty boxes
	List *empty_boxes = NULL;
	int empty_box_count = 0;

	List *link = list_first(l_boxes);
	while (link) {
		box = (BoxObject *)link->data;

		if (!box->content) {
			// Use prepend because it's faster
			empty_boxes = list_prepend(empty_boxes, box);
			empty_box_count++;
		}

		link = list_next(link);
	}

	int i;
	for (i = 0; i < n_content; i++) {
		// Break, if there are no more empty boxes
		if (empty_box_count <= 0) break;

		box = (BoxObject *)list_nth(empty_boxes, random(0, empty_box_count - 1))->data;
		box->content = content[i];
		empty_boxes = list_remove(empty_boxes, box);

		empty_box_count--;
	}

	empty_boxes = list_free(empty_boxes);

	return i;
}

/** @} */
