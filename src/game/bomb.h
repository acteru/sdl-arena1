/*
 * bomb.h
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
 *  @defgroup bomb bomb
 *  @brief Represents the class for bomb objects.
 *
 *  This module is home of all bomb objects.
 *  It is responsable for creating and handling of bombs.
 *  It is used by the modules game, gfx and bomberman (the last is an indirect
 *  collaborator, direct collaborator is game, by key entry).
 *
 *  @{
 */

#ifndef BOMB_H_
#define BOMB_H_

#include "core/common.h"
#include "game.h"
#include "explosion.h"

extern void bomb_init();
extern void bomb_destroy();

extern GameObject * bomb_create(Vector pos, GameObject *owner, ExplosionInfo *exp_info);
extern void bomb_free(GameObject *bomb);
extern void bomb_free_all();

extern GameObject * bomb_get_owner(GameObject *bomb);


#endif /* BOMB_H_ */

/** @} */
