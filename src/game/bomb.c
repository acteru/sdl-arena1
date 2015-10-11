/*
 * bomb.c
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
 *  @addtogroup bomb
 *  @{
 */

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include "core/core.h"
#include "game.h"
#include "bomb.h"
#include "explosion.h"


#define BOMB_TIME		20						/**< time in 100ms until a bomb explodes */

/**
 * Struct used for all bomb objects
 */
typedef struct {
	GameObject  base;							/**< data from the base class */
	GameObject *owner;							/**< who has layed the bomb */
	ExplosionInfo *exp_info;					/**< information about the explosion created by this bomb */
	int time;									/**< time till bomb explode */
	int sprite;									/**< sprite currently used */
} BombObject;

static List 		*l_bombs = NULL;			/**< list of all existing bomb objects */

static SDL_Surface	*s_bomb;					/**< sprites for the explosion */
static SDL_Rect		 s_bomb_clips[3];			/**< clips for the sprites */

static Mix_Chunk	*a_drop;					/**< audio sample when droping a bomb */

static int		 	 evt_gfx_draw;				/**< id of the gfx-draw event handler */
static int			 evt_explosion_hit;			/**< id of the evt-explosion-hit event handler */
static int		 	 tmr_step;					/**< id of the step timer */

/**
 *  Event handler for the gfx-draw event.
 *  Draws all bombs.
 */
static void _bomb_evt_gfx_draw(void *event_data, void *user_data)
{
	// Loop through all bomb objects
	List *link = list_first(l_bombs);
	while(link) {
		BombObject *bomb = (BombObject *)link->data;
		game_draw(s_bomb, bomb->base.pos, &s_bomb_clips[bomb->sprite]);
		link = list_next(link);
	}
}

/**
 *  Event handler for the explosion-hit event.
 *  Lets a bomb explode if it is hit by an explosion.
 */
static void _bomb_evt_explosion_hit(void *event_data, void *user_data)
{
	GameObject *obj = (GameObject *)event_data;

	if (obj->type == OBJ_BOMB) {
		BombObject *bomb = (BombObject *)obj;

		// Set bomb timer to zero making it explode. The explosion cannot be done here because
		// this event is raised if a bomb is currently exploding which means that we are
		// in a loop through all bombs and can't delete bombs from the list.
		bomb->time = 0;
	}
}

/**
 *  Timer callback function for the timer countdown.
 *  This timer determines how long a bomb lasts before exploding.
 */
static void _bomb_tmr_step(void *user_data)
{
	List *link = list_first(l_bombs);

	// Count each bomb down
	while (link) {
		BombObject *bomb = (BombObject *)link->data;
		link = list_next(link);

		bomb->time--;


		// Calculate which explosion sprite to draw
		float phase = (float)(bomb->time - 1) / BOMB_TIME * 8.0f;
		int index_by_phase[] = { 0, 1, 2, 1 };
		bomb->sprite = index_by_phase[(int)phase % 4];
	}

	// Let bombs explode which are zero
	bool playsound = TRUE;
	link = list_first(l_bombs);
	while (link) {
		BombObject *bomb = (BombObject *)link->data;
		link = list_next(link);

		if (bomb->time <= 0) {
			Vector pos;

			// If time is off: raise event, delete bomb, create explosion
			event_raise("bomb-explode", bomb);
			pos = bomb->base.pos;
			bomb_free((GameObject *)bomb);
			explosion_create(pos, bomb->exp_info, playsound);

			// Make sure that the sound is just played once
			playsound = FALSE;

			// Start again, because this bomb could have triggered other bombs
			link = list_first(l_bombs);
		}
	}
}


/**
 *  Initializes this module.
 */
void bomb_init()
{
	// Register events
	evt_gfx_draw = event_connect("gfx-draw", 0, _bomb_evt_gfx_draw, NULL, EVENT_HANDLER_ENABLED);
	evt_explosion_hit = event_connect("explosion-hit", 0, _bomb_evt_explosion_hit, NULL, EVENT_HANDLER_ENABLED);

	// Initialize a timer bomb countdown
	tmr_step = timer_create_interval(100, _bomb_tmr_step, NULL, TIMER_ENABLED);

	// Load sprites
	s_bomb = assert_sprite("sprites\\bomb.png");

	// Load samples
	a_drop = assert_sample("sounds\\drop.ogg");

	// Initialize clips
	Size sprite_size = { 60, 60 };
	sprite_get_clips(s_bomb_clips, sprite_size, 3);
}


/**
 *  Destroys this module freeing any allocated data.
 */
void bomb_destroy()
{
	// Free all objects
	bomb_free_all();

	// Unregister events
	event_disconnect(evt_gfx_draw);
	event_disconnect(evt_explosion_hit);

	// Free timers
	timer_free(tmr_step);

	// Free sprites
	SDL_FreeSurface(s_bomb);

	// Free samples
	Mix_FreeChunk(a_drop);
}


/**
 *  Creates a new bomb object.
 *
 *  @param pos			initial position of the new object
 *  @param owner		who lays this bomb
 *  @param exp_info		information about the explosion this bomb creates
 *  @returns			the newly created bomb object
 */
GameObject * bomb_create(Vector pos, GameObject *owner, ExplosionInfo *exp_info)
{
	BombObject *bomb = (BombObject *)malloc(sizeof(BombObject));
	bomb->base.type = OBJ_BOMB;
	bomb->owner = owner;
	bomb->exp_info = exp_info;
	bomb->time = BOMB_TIME;
	bomb->sprite = 0;

	game_set_field(pos, (GameObject *)bomb);
	l_bombs = list_append(l_bombs, bomb);

	Mix_PlayChannel(-1, a_drop, 0);

	return (GameObject *)bomb;
}


/**
 *  Frees a bomb object.
 *
 *  @param bomb		a bomb object
 */
void bomb_free(GameObject *bomb)
{
	game_set_field(bomb->pos, NULL);
	l_bombs = list_remove(l_bombs, bomb);

	free(bomb);
}

/**
 *  Frees all existing bomb objects.
 */
void bomb_free_all()
{
	// Loop through all box objects
	List *link = list_first(l_bombs);
	while (link) {
		BombObject *bomb = (BombObject *)link->data;
		link = list_next(link);

		bomb_free((GameObject *)bomb);
	}
}

/**
 *  Gets the owner of a specific bomb.
 *
 *  @param bomb		a bomb object
 *  @returns	 	owner of the given bomb
 */
 GameObject * bomb_get_owner(GameObject *bomb)
 {
	 BombObject *obj = (BombObject *)bomb;
	 return obj->owner;
 }


/** @} */
