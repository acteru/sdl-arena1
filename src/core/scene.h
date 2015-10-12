/*
 * scene.h
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
 *  @defgroup scene scene
 *  @brief Manages the scenes of the application.
 *
 *  This module is used to manage the current scene of the application.
 *  A scene is treated as something like an application state. Are we
 *  currently in the main menu? Are we playing a game right now?
 *  This needs to be done centraly because this way it's easy to propagate
 *  a change of the scene to all modules. They can then react and for
 *  example enable or disable event handlers.
 *
 *  The scenes are saved in a stack layout. So for example if we push the
 *  "in-game" scene by calling the scene_push() function and we were previously
 *  in the "main-menu" scene. We can fall back to the main menu simply by
 *  calling the scene_pop() function which requires no other arguments.
 *
 *  @note	A scene is nothing else than a string which identifies it.
 *
 *  @note	You should make sure that scenes which are pushed, are popped
 *  		later to avoid a constantly growing stack.
 *
 *  @{
 */

#ifndef SCENE_H_
#define SCENE_H_

extern void scene_init();
extern void scene_destroy();

extern void scene_push(char *name);
extern char * scene_pop();
extern char * scene_get();
extern int scene_check(char *name);

#endif /* SCENE_H_ */

/** @} */
