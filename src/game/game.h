/*
 * game.h
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
 *  @defgroup game game
 *  @brief Responsible for the management of the game itself.
 *
 *  This module is the central processing point of the game scene.
 *  It holds references to all existing objects and takes care that
 *  the a game round is correctly started and finished. Further it
 *  processes any inputs from the keyboard to control the bombermans.
 *
 *  @{
 */

#ifndef GAME_H_
#define GAME_H_

#include <SDL/SDL.h>
#include "core/common.h"

#define GAME_WORLD_WIDTH		15		/**< width of the world in fields */
#define GAME_WORLD_HEIGHT		11		/**< height of the world in fields */

/**
 *  This enum type defines an identifier for each game object type.
 */
typedef enum {
	OBJ_NONE		= 0,		/**< no object */
	OBJ_BOMBERMAN 	= 1,		/**< type id for a bomberman object */
	OBJ_BOMB 		= 2,		/**< type id for a bomb object */
	OBJ_EXPLOSION 	= 3,		/**< type id for an explosion object */
	OBJ_ROCK 		= 4,		/**< type id for a rock object */
	OBJ_BOX 		= 5,		/**< type id for a box object */
	OBJ_UPGRADE		= 6,		/**< type id for an upgrade object */
} ObjectType;

/**
 *  This enum type defines an identifier for each bomberman color.
 */
typedef enum {
	COL_BLUE		= 1,	/**< color id for a blue bomberman */
	COL_RED			= 2,	/**< color id for a red bomberman */
	COL_GREEN		= 3,	/**< color id for a green bomberman */
	COL_YELLOW		= 4,	/**< color id for a yellow bomberman */
} Color;

/**
 *  This struct holds the data for a game object.
 *  It is the base of all game objects.
 */
typedef struct {
	ObjectType 		type;		/**< type id of the game object */
	Vector 			pos;		/**< position of the game object */
} GameObject;

extern void game_init();
extern void game_destroy();

extern GameObject * game_get_field(Vector pos);
extern void game_set_field(Vector pos, GameObject *obj);
extern void game_get_field_coords(Vector pos, SDL_Rect *coords);
extern void game_draw(SDL_Surface *sprite, Vector pos, SDL_Rect *clip);
extern void game_draw_floating(SDL_Surface *sprite, VectorF pos, SDL_Rect *clip);

extern void game_free_object(GameObject *obj);

extern void game_print_world_layout();

#endif /* GAME_H_ */

/** @} */
