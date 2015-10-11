/*
 * explosion.c
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
 *  @addtogroup explosion
 *  @{
 */

#include <SDL/SDL.h>
#include "gfx.h"
#include "core/core.h"
#include "game.h"
#include "explosion.h"
#include "bomb.h"



#define EXPLOSION_TIME	6

#define SPRITE_EXP_CENTER		0
#define SPRITE_EXP_HORIZONTAL	1
#define SPRITE_EXP_VERTICAL		2
#define SPRITE_EXP_RIGHTEND		3
#define SPRITE_EXP_LEFTEND		4
#define SPRITE_EXP_UPPEREND		5
#define SPRITE_EXP_LOWEREND		6

/**
 * Struct used for all explosion objects
 */
typedef struct {
	GameObject base;							/**< data from the base class */
	int time;									/**< time, how long explosion must be on */
	int sprite;									/**< sprite (direction) used by the explosion object */
	int sprite_index;							/**< sprite index (intensity) currently used */
} ExplosionObject;

static List 		*l_explosions = NULL;		/**< list of all existing explosion objects */

static int		 	 evt_gfx_draw;				/**< id of the gfx-draw event handler */
static int			 tmr_step;					/**< id of the step timer */

static SDL_Surface	*s_explosion[5];			/**< sprites for the explosion */
static SDL_Rect		 s_explosion_clips[7];		/**< clips for the sprites */

static Mix_Chunk	*a_explosion;				/**< sample of an explosion */


/**
 *  Creates a single explosion object. Only used in explosion_create().
 *  This function checks whether we can create an explosion on the field or not.
 *
 *  If there is already an explosion object on the field it is being replaced.
 *  The event explosion-hit is raised if there is another object occupying the field.
 *
 *  @param pos		position of the new
 *  @param sprite	index of the sprite
 *  @param obj		[out] where to store the pointer to the newly created explosion object
 *
 *  @returns		1, if the explosion has been created, otherwise 0.
 */
static int _explosion_create_field(Vector pos, int sprite, ExplosionObject **obj)
{
	// Check whether field is valid
	if (pos.x < 0 || pos.x >= GAME_WORLD_WIDTH || pos.y < 0 || pos.y >= GAME_WORLD_HEIGHT) return 0;

	// Check whether field is empty
	GameObject *field = game_get_field(pos);
	if (field != NULL) {
		if (field->type == OBJ_EXPLOSION) {
			explosion_free(field);
		}
		else {
			event_raise("explosion-hit", field);
			return 0;
		}
	}

	// Create explosion object
	ExplosionObject *explosion = (ExplosionObject *)malloc(sizeof(ExplosionObject));
	explosion->base.type = OBJ_EXPLOSION;
	explosion->time = EXPLOSION_TIME;
	explosion->sprite = sprite;
	explosion->sprite_index = 2;

	game_set_field(pos, (GameObject *)explosion);
	l_explosions = list_append(l_explosions, explosion);

	// Return the object, if a pointer location was specified
	if (obj) *obj = explosion;

	return 1;
}

/**
 *  Event handler function for the gfx-draw event.
 *  Draws all explosions.
 */
static void _explosion_evt_gfx_draw(void *event_data, void *user_data)
{
	List *link = list_first(l_explosions);

	// Draw all existing explosion fields
	while (link) {
		ExplosionObject *explosion = (ExplosionObject *)link->data;

		// The explosion is represented by sprites :D
		game_draw(s_explosion[explosion->sprite_index], explosion->base.pos, &s_explosion_clips[explosion->sprite]);

		link = list_next(link);
	}
}

/**
 *  Timer callback function for the timer countdown.
 *  This timer determines how long an explosion lasts.
 */
static void _explosion_tmr_step(void *user_data)
{
	List *link = list_first(l_explosions);

	// Count each explosion down
	while(link) {
		ExplosionObject *explosion = (ExplosionObject *)link->data;
		link = list_next(link);

		explosion->time--;

		// Calculate which explosion sprite to draw
		float phase = (float)(explosion->time - 1) / EXPLOSION_TIME * 6.0f;
		int index_by_phase[] = { 0, 1, 2, 3, 4, 3, 2 };
		explosion->sprite_index = index_by_phase[(int)phase];

		if(explosion->time <= 0) {
			explosion_free((GameObject *)explosion);
		}
	}
}

/**
 *  Initializes this module.
 */
void explosion_init()
{
	// Register events
	evt_gfx_draw = event_connect("gfx-draw", 0, _explosion_evt_gfx_draw, NULL, EVENT_HANDLER_ENABLED);

	// Initialize a timer for explosion countdown
	tmr_step = timer_create_interval(100, _explosion_tmr_step, NULL, TIMER_ENABLED);

	// Load sprites
	s_explosion[0] = assert_sprite("sprites\\explosion5.png");
	s_explosion[1] = assert_sprite("sprites\\explosion4.png");
	s_explosion[2] = assert_sprite("sprites\\explosion3.png");
	s_explosion[3] = assert_sprite("sprites\\explosion2.png");
	s_explosion[4] = assert_sprite("sprites\\explosion1.png");

	// Load samples
	a_explosion = assert_sample("sounds\\explosion.ogg");

	// Initialize clips
	Size sprite_size = { 60, 60 };
	sprite_get_clips(s_explosion_clips, sprite_size, 7);
}

/**
 *  Destroys this module freeing any allocated data.
 */
void explosion_destroy()
{
	// Free all objects
	explosion_free_all();

	// Unregister events
	event_disconnect(evt_gfx_draw);

	// Destroy timer
	timer_free(tmr_step);

	// Free sprites
	SDL_FreeSurface(s_explosion[0]);
	SDL_FreeSurface(s_explosion[1]);
	SDL_FreeSurface(s_explosion[2]);
	SDL_FreeSurface(s_explosion[3]);
	SDL_FreeSurface(s_explosion[4]);

	// Free samples
	Mix_FreeChunk(a_explosion);
}

/**
 * 	Creates a new explosion, calculates all explosion fields.
 * 	Adds all explosion fields into list of explosions.
 *
 *	The explosion-created event is raised when finished.
 *
 *  @param pos			position of the origin of explosion
 *  @param info			information about the explosion to create
 *  @param playsound	whether to play the explosion sample
 *  @returns			pointer of origin explosion object, type GameObject
 */
GameObject * explosion_create(Vector pos, ExplosionInfo *info, bool playsound)
{
	int a;
	Vector temp;

	// Callculate explosion fields...
	// For x+
	temp = pos;
	for(a = 1; a < info->length; a++) {
		// Set offset
		temp.x += 1;

		// Choose the right sprite
		int sprite = SPRITE_EXP_HORIZONTAL;
		if (a == info->length - 1) {
			sprite = SPRITE_EXP_RIGHTEND;
		}

		// Create an explosion object on this field, break if it fails
		if (!_explosion_create_field(temp, sprite, NULL)) {
			break;
		}
	}

	// For x-
	temp = pos;
	for(a = 1; a < info->length; a++) {
		// Set offset
		temp.x -= 1;

		// Choose the right sprite
		int sprite = SPRITE_EXP_HORIZONTAL;
		if (a == info->length - 1) {
			sprite = SPRITE_EXP_LEFTEND;
		}

		// Create an explosion object on this field, break if it fails
		if (!_explosion_create_field(temp, sprite, NULL)) {
			break;
		}
	}

	// For y+
	temp = pos;
	for(a = 1; a < info->length; a++) {
		// Set offset
		temp.y += 1;

		// Choose the right sprite
		int sprite = SPRITE_EXP_VERTICAL;
		if (a == info->length - 1) {
			sprite = SPRITE_EXP_LOWEREND;
		}

		// Create an explosion object on this field, break if it fails
		if (!_explosion_create_field(temp, sprite, NULL)) {
			break;
		}
	}

	// For y-
	temp = pos;
	for(a = 1; a < info->length; a++) {
		// Set offset
		temp.y -= 1;

		// Choose the right sprite
		int sprite = SPRITE_EXP_VERTICAL;
		if (a == info->length - 1) {
			sprite = SPRITE_EXP_UPPEREND;
		}

		// Create an explosion object on this field, break if it fails
		if (!_explosion_create_field(temp, sprite, NULL)) {
			break;
		}
	}

	// Create source explosion field, return this object
	ExplosionObject *explosion = NULL;
	_explosion_create_field(pos, SPRITE_EXP_CENTER, &explosion);

	// Play sound
	if (playsound) {
		Mix_PlayChannel(-1, a_explosion, 0);
	}

	return (GameObject *)explosion;
}


/**
 *  Frees an explosion object.
 *
 *  @param explosion		a explosion object
 */
void explosion_free(GameObject *explosion)
{
	game_set_field(explosion->pos, NULL);
	l_explosions = list_remove(l_explosions, explosion);

	free(explosion);
}

/**
 *  Frees all existing explosion objects.
 */
void explosion_free_all()
{
	// Loop through all box explosion objects
	List *link = list_first(l_explosions);
	while (link) {
		ExplosionObject *explosion = (ExplosionObject *)link->data;
		link = list_next(link);

		explosion_free((GameObject *)explosion);
	}
}

/** @} */
