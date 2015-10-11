/*
 * bomberman.c
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
 *  @addtogroup bomberman
 *  @{
 */

#include <stdlib.h>
#include <math.h>
#include <SDL/SDL.h>
#include "core/core.h"
#include "game.h"
#include "bomberman.h"
#include "bomb.h"
#include "upgrade.h"


/**
 * Struct used for all bomberman objects
 */
typedef struct {
	GameObject base;				/**< data from the base class */
	VectorF pos_exact;				/**< exact position of the bomberman */
	Direction dir;					/**< direction in which the bomberman wants to walk */
	BombermanColor color;			/**< color of the bomberman, used for selecting sprite sheet */
	bool alive;						/**< wether the bomberman is still alive */
	UpgradeInfo upgrades;			/**< information about collected upgrades */

	int sprite;						/**< sprite which is currently used */
	int sprite_index;				/**< index of sprite which is currently used */
	int sprite_inc;					/**< wether to increment or decrement sprite index */
} BombermanObject;

#define SPRITE_WALK_DOWN		1	/**< sprite for walking down */
#define SPRITE_WALK_LEFT		4	/**< sprite for walking left */
#define SPRITE_WALK_RIGHT		7	/**< sprite for walking right */
#define SPRITE_WALK_UP			10	/**< sprite for walking up */
#define SPRITE_DEAD				19	/**< sprite for a dead bomberman */

static List 		*l_bombermans = NULL;		/**< list of all existing bomberman objects */

static SDL_Surface	*s_bomberman[4];			/**< sprites for bomberman */
static SDL_Rect		 s_bomberman_clips[20];		/**< clips which may be applied to all sprites */

static Mix_Chunk	*a_step;					/**< audio sample for a step */

static int 			 evt_gfx_draw;				/**< id of the gfx-draw event handler */
static int			 evt_bomb_explode;			/**< id of the bomb-explode event handler */
static int 			 tmr_step;					/**< id of the timer step */

/**
 *  Checks wether a bomberman can walk over a specified field.
 *
 *  @param pos		a position
 *  @returns		TRUE, if a bomberman can walk over the field, otherwise FALSE
 */
static bool _bomberman_check_pos(Vector pos)
{
	if (pos.x >= 0 && pos.x < GAME_WORLD_WIDTH &&
		pos.y >= 0 && pos.y < GAME_WORLD_HEIGHT) {

		GameObject *object = game_get_field(pos);

		if (object == NULL || object->type == OBJ_EXPLOSION || object->type == OBJ_UPGRADE) {
			return TRUE;
		}
	}
	return FALSE;
}

/**
 *  Event handler for the gfx-draw event.
 *  Draws all existing bombermans.
 */
static void _bomberman_evt_gfx_draw(void *event_data, void *user_data)
{
	// Loop through all bomberman objects
	List *link = list_first(l_bombermans);
	while (link) {
		BombermanObject *bobj = (BombermanObject *)link->data;

		// Select clip and draw the sprite
		int clip_index = bobj->sprite + bobj->sprite_index;
		game_draw_floating(s_bomberman[bobj->color], bobj->pos_exact, &s_bomberman_clips[clip_index]);

		link = list_next(link);
	}
}

/**
 *  Event handler for the bomb-explode event.
 *  Gives the bomb back.
 */
static void _bomberman_evt_bomb_explode(void *event_data, void *user_data)
{
	GameObject *bomb = (GameObject *)event_data;
	GameObject *owner = bomb_get_owner(bomb);

	// If the owner is a bomberman, give him his bomb back.
	if (owner->type == OBJ_BOMBERMAN) {
		BombermanObject *bobj = (BombermanObject *)owner;
		bobj->upgrades.bombs_available++;
	}
}

/**
 *  Timer callback function for timer step.
 *  Calculate all movement and checks wether a bomberman
 *  walks over an explosion or upgrade.
 */
static void _bomberman_tmr_step(void *user_data)
{
	static int sprite_delay = 0;

	// This variable is used to slow down animation speed.
	sprite_delay++;

	// Loop through all bomberman objects
	List *link = list_first(l_bombermans);
	while (link) {
		BombermanObject *bobj = (BombermanObject *)link->data;
		link = list_next(link);

		// If the bomberman isn't alive anymore, just draw the right sprite
		if (!bobj->alive){
			if (sprite_delay % 10 == 0 && bobj->sprite_index < 0){
				bobj->sprite_index++;
			}
			continue;
		}

		// If the bomberman wants to walk up
		if (bobj->dir & DIR_UP) {
			Vector pos_next = bobj->base.pos;
			pos_next.y--;

			bobj->sprite = SPRITE_WALK_UP;

			if (_bomberman_check_pos(pos_next) || bobj->pos_exact.y > (float)bobj->base.pos.y + 0.05f) {
				bobj->pos_exact.y -= 0.1f;
				bobj->pos_exact.x = bobj->base.pos.x;
			}
		}

		// If the bomberman wants to walk down
		else if (bobj->dir & DIR_DOWN) {
			Vector pos_next = bobj->base.pos;
			pos_next.y++;

			bobj->sprite = SPRITE_WALK_DOWN;

			if (_bomberman_check_pos(pos_next) || bobj->pos_exact.y < (float)bobj->base.pos.y - 0.05f) {
				bobj->pos_exact.y += 0.1f;
				bobj->pos_exact.x = bobj->base.pos.x;
			}
		}

		// If the bomberman wants to walk right
		else if (bobj->dir & DIR_RIGHT) {
			Vector pos_next = bobj->base.pos;
			pos_next.x++;

			bobj->sprite = SPRITE_WALK_RIGHT;

			if (_bomberman_check_pos(pos_next) || bobj->pos_exact.x < (float)bobj->base.pos.x - 0.05f) {
				bobj->pos_exact.x += 0.1f;
				bobj->pos_exact.y = bobj->base.pos.y;
			}
		}

		// If the bomberman wants to walk left
		else if (bobj->dir & DIR_LEFT) {
			Vector pos_next = bobj->base.pos;
			pos_next.x--;

			bobj->sprite = SPRITE_WALK_LEFT;

			if (_bomberman_check_pos(pos_next) || bobj->pos_exact.x > (float)bobj->base.pos.x + 0.05f) {
				bobj->pos_exact.x -= 0.1f;
				bobj->pos_exact.y = bobj->base.pos.y;
			}
		}

		// Calculate integer position
		bobj->base.pos = vrecti(fround(bobj->pos_exact.x), fround(bobj->pos_exact.y));

		// If the bomberman is walking, create an animation
		if (sprite_delay % 6 == 0 && bobj->dir != DIR_NONE) {
			bobj->sprite_index += bobj->sprite_inc;
			if (bobj->sprite_index == -1) bobj->sprite_inc =  1;
			if (bobj->sprite_index ==  1) bobj->sprite_inc = -1;
		}

		// If the bomberman doesn't walk, select the sprite where the bomberman stays still
		if (bobj->dir == DIR_NONE) {
			bobj->sprite_index = 0;
		}
		// Otherwise create a sound effect
		else if (sprite_delay % 12 == 0) {
			Mix_PlayChannel(-1, a_step, 0);
		}

		// Check wether the bomberman walks over an explosion or upgrade
		GameObject *object = game_get_field(bobj->base.pos);
		if (object != NULL) {
			if (object->type == OBJ_EXPLOSION) {
				// Die bomberman, die!
				bobj->alive = FALSE;
				bobj->sprite = SPRITE_DEAD;
				bobj->sprite_index = -3;

				// Raise event
				event_raise("bomberman-died", bobj);
			}

			if (object->type == OBJ_UPGRADE) {
				// Apple upgrade and free the object
				upgrade_apply(object, &bobj->upgrades);
				upgrade_free(object);
			}
		}
	}
}

/**
 *  Initializes this module.
 */
void bomberman_init()
{
	// Register events
	evt_gfx_draw = event_connect("gfx-draw", 0, _bomberman_evt_gfx_draw, NULL, EVENT_HANDLER_ENABLED);
	evt_bomb_explode = event_connect("bomb-explode", 0, _bomberman_evt_bomb_explode, NULL, EVENT_HANDLER_ENABLED);

	// Initialize timers
	tmr_step = timer_create(20, _bomberman_tmr_step, NULL, TIMER_ENABLED);

	// Load sprites
	s_bomberman[0] = assert_sprite("sprites\\bomberman1.png");
	s_bomberman[1] = assert_sprite("sprites\\bomberman2.png");
	s_bomberman[2] = assert_sprite("sprites\\bomberman3.png");
	s_bomberman[3] = assert_sprite("sprites\\bomberman4.png");

	// Load samples
	a_step = assert_sample("sounds\\step.ogg");

	// Initialize clips
	Size sprite_size = { 60, 90 };
	sprite_get_clips(s_bomberman_clips, sprite_size, 20);
}

/**
 *  Destroys this module freeing any allocated data.
 */
void bomberman_destroy()
{
	// Free all objects
	bomberman_free_all();

	// Unregister events
	event_disconnect(evt_gfx_draw);
	timer_free(tmr_step);

	// Free sprites
	SDL_FreeSurface(s_bomberman[0]);
	SDL_FreeSurface(s_bomberman[1]);
	SDL_FreeSurface(s_bomberman[2]);
	SDL_FreeSurface(s_bomberman[3]);

	// Free samples
	Mix_FreeChunk(a_step);
}

/**
 *  Creates a new bomberman object.
 *
 *  @param pos		initial position
 *  @param color	color of the bomberman
 *  @returns		newly created object
 */
GameObject * bomberman_create(Vector pos, BombermanColor color)
{
	BombermanObject *bobj = (BombermanObject *)malloc(sizeof(BombermanObject));
	bobj->base.type = OBJ_BOMBERMAN;
	bobj->base.pos = pos;

	bobj->pos_exact = vrect(pos.x, pos.y);
	bobj->dir = DIR_NONE;
	bobj->alive = TRUE;
	bobj->color = color;

	bobj->upgrades.bomberman = (GameObject *)bobj;
	bobj->upgrades.bombs_available = 1;
	bobj->upgrades.exp_info.length = 3;

	bobj->sprite = SPRITE_WALK_DOWN;
	bobj->sprite_index = 0;
	bobj->sprite_inc = 1;

	l_bombermans = list_append(l_bombermans, bobj);

	return (GameObject *)bobj;
}

/**
 *  Frees an existing bomberman object.
 */
void bomberman_free(GameObject *bomberman)
{
	l_bombermans = list_remove(l_bombermans, bomberman);
	free(bomberman);
}

/**
 *  Frees all existing bomberman objects.
 */
void bomberman_free_all()
{
	// Loop through all bomberman objects
	List *link = list_first(l_bombermans);
	while (link) {
		BombermanObject *bobj = (BombermanObject *)link->data;
		link = list_next(link);

		// Free everyone
		bomberman_free((GameObject *)bobj);
	}
}

/**
 *  Sets a new direction for a bomberman.
 *
 *  @param bomberman	a bomberman object
 *  @param dir			new direction
 */
void bomberman_set_direction(GameObject *bomberman, Direction dir)
{
	BombermanObject *bobj = (BombermanObject *)bomberman;
	bobj->dir = dir;
}

/**
 *  Gets the direction in which the bomberman is currently walking.
 *
 *  @param bomberman	a bomberman object
 *  @returns			the direction
 */
Direction bomberman_get_direction(GameObject *bomberman)
{
	BombermanObject *bobj = (BombermanObject *)bomberman;
	return bobj->dir;
}

/**
 *  Makes a bomberman laying a bomb. This function doesn't do anything
 *  if the bomberman hasn't any bomb left.
 *
 *  @param bomberman	a bomberman object
 */
void bomberman_lay_bomb(GameObject *bomberman)
{
	BombermanObject *bobj = (BombermanObject *)bomberman;

	if (!game_get_field(bobj->base.pos) && bobj->alive && bobj->upgrades.bombs_available) {
		bomb_create(bobj->base.pos, (GameObject *)bobj, &bobj->upgrades.exp_info);
		bobj->upgrades.bombs_available--;
	}
}

/**
 *  Returns wether a bomberman is still alive.
 *
 *  @param bomberman	a bomberman object
 *  @returns			TRUE, if the bomberman is alive. Otherwise FALSE
 */
bool bomberman_is_alive(GameObject *bomberman)
{
	BombermanObject *bobj = (BombermanObject *)bomberman;
	return bobj->alive;
}

/** @} */
