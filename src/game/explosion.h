/*
 * explosion.h
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
 *  @defgroup explosion explosion
 *  @brief Represents the class for explosion objects.
 *
 * In this module the explosions are handled.
 *
 *  @{
 */
#ifndef EXPLOSION_H_
#define EXPLOSION_H_

typedef struct {
	int length;
} ExplosionInfo;

extern void explosion_init();
extern void explosion_destroy();

extern GameObject * explosion_create(Vector pos, ExplosionInfo *info, bool playsound);
extern void explosion_free(GameObject *explosion);
extern void explosion_free_all();


#endif /* EXPLOSION_H_ */

/** @} */
