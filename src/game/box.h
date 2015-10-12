/*
 * box.h
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
 *  @brief Represents the class for box objects.
 *
 *  This module represents the class for box objects.
 *  Boxs are destroyable static objects.
 *
 *  @{
 */

#ifndef BOX_H_
#define BOX_H_

#include "core/common.h"
#include "game.h"

extern void box_init();
extern void box_destroy();

extern GameObject * box_create(Vector pos);
extern void box_free(GameObject *box);
extern void box_free_all();

extern int box_distribute(GameObject *content[], int n_content);

#endif /* BOX_H_ */

/** @} */
