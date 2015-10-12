/*
 * gfx.c
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
 *  @addtogroup gfx
 *  @{
 */

#include <SDL/SDL.h>
#include "core/core.h"
#include "gfx.h"



static SDL_Surface 		*screen;						/**< surface which is displayed */

static int 				 tmr_draw;						/**< id of the timer which raises the gfx-draw event */

/**
 *  Callback function for tmr_draw.
 *  This function is called 50 times per seconds. It clears the screen, let all modules redraw
 *  by raising the gfx-draw event and then displays the result on the screen by flipping the
 *  double buffers.
 *
 *  @param user_data		NULL
 */
static void _gfx_tmr_draw(void *user_data)
{
	// Clear
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));

	// Raise draw event
	event_raise("gfx-draw", screen);

	// Show
	SDL_Flip(screen);
}

/**
 *  Initializes this module.
 */
void gfx_init()
{
	// Create a window
	screen = SDL_SetVideoMode(GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
	assert_ptr(screen, "couldn't set video mode", SDL_GetError);
	SDL_WM_SetCaption("Arena 1", "Arena 1");

	// Initialize a timer for drawing (50 FPS)
	tmr_draw = timer_create_interval(20, _gfx_tmr_draw, NULL, TIMER_ENABLED);
}

/**
 *  Destroys this module freeing any allocated data.
 */
void gfx_destroy()
{
	// Free resources
	timer_free(tmr_draw);
}

/**
 *  Gets the SDL surface which is presented to the user.
 *
 *  @attention	Draw to this surface during a gfx-draw event only!
 *
 *  @returns 	the SDL surface
 */
SDL_Surface * gfx_get_screen()
{
	return screen;
}

/** @} */
