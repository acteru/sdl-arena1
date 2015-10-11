/*
 * rock.h
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
 *  @defgroup box box
 *  @brief Represents the class for rock objects.
 *
 *  This module represents the class for rock objects.
 *  Rocks are not destroyable static objects.
 *
 *  @{
 */

#ifndef ROCK_H_
#define ROCK_H_

#include "core/common.h"
#include "game.h"

extern void rock_init();
extern void rock_destroy();

extern GameObject * rock_create(Vector pos);
extern void rock_free(GameObject *rock);
extern void rock_free_all();

#endif /* ROCK_H_ */

/** @} */
