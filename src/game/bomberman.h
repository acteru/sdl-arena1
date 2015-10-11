/*
 * bomberman.h
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
 *  @defgroup bomberman bomberman
 *  @brief Represents the class for bomberman objects.
 *
 *  This module represents the class for bomberman objects.
 *  It handles the bomberman by controlling movement and abilities.
 *
 *  @{
 */

#ifndef BOMBERMAN_H_
#define BOMBERMAN_H_

#include "core/common.h"
#include "game.h"
#include "explosion.h"

/**
 *  Represents a color for a bomberman.
 */
typedef enum {
	BM_WHITE	= 0,	/**< white bomberman */
	BM_BLUE		= 1,	/**< blue bomberman */
	BM_ORANGE	= 2,	/**< orange bomberman */
	BM_BLACK	= 3,	/**< black bomberman */
} BombermanColor;

extern void bomberman_init();
extern void bomberman_destroy();

extern GameObject * bomberman_create(Vector pos, BombermanColor color);
extern void bomberman_free(GameObject *bomberman);
extern void bomberman_free_all();

extern void bomberman_set_direction(GameObject *bomberman, Direction dir);
extern Direction bomberman_get_direction(GameObject *bomberman);

extern void bomberman_lay_bomb(GameObject *bomberman);

extern bool bomberman_is_alive(GameObject *bomberman);

#endif /* BOMBERMAN_H_ */

/** @} */
