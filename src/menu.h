/*
 * menu.h
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
 *  @defgroup menu menu
 *  @brief Represents the main menu.
 *
 *  This module draws the main menu and gives the user the choice of
 *  starting a game or exiting the application.
 *
 *  @{
 */

#ifndef MENU_H_
#define MENU_H_

#include <SDL/SDL.h>
#include "core/common.h"


#define MENU_WORLD_WIDTH		15		/**< width of the menu */
#define MENU_WORLD_HEIGHT		11		/**< height of the menu */


extern void menu_init();
extern void menu_destroy();

#endif /* MENU_H_ */

/** @} */
