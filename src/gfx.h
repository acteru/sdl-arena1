/*
 * gfx.h
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
 *  @defgroup gfx gfx
 *  @brief Responsible for SDL library initialization
 *
 *  This module initializes the SDL library.
 *  After initialization it begins to raise the gfx-draw event
 *  50 times per second. Before raising the event the entire screen
 *  is cleared. Other modules should draw their objects
 *  onto the surface provided by the event_data of this event, so
 *  they are correctly displayed after the event has been processed.
 *
 *  @{
 */

#ifndef GFX_H_
#define GFX_H_

#define GFX_SCREEN_WIDTH		1024		/**< width of the screen surface */
#define GFX_SCREEN_HEIGHT		768			/**< height of the screen surface */

extern void gfx_init();
extern void gfx_destroy();
extern SDL_Surface * gfx_get_screen();

#endif /* GFX_H_ */

/** @} */
