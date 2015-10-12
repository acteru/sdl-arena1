/*
 * main.c
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
 *  @defgroup main main
 *  @brief Contains the program entry point.
 *
 *  This module contains the program entry point where it initializes all modules
 *  and starts an event loop. If an application exit is requested, it
 *  destroys all modules so that every allocated data is freed before exiting
 *  the application.
 *
 *  @{
 */

#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include "core/core.h"
#include "game/game.h"
#include "menu.h"
#include "gfx.h"



int main (int argc, char *argv[])
{
	// Initialize random generator
	srand(time(NULL));

	// Initialize all modules
#ifdef DEBUG
	printf("initializing modules...\n");
	core_init(argc, argv);
	printf("core initialized.\n");
	gfx_init();
	printf("gfx initialized.\n");
	game_init();
	printf("game initialized.\n");
	menu_init();
	printf("menu initialized.\n");
#else
	core_init(argc, argv);
	gfx_init();
	game_init();
	menu_init();
#endif

	// Set initial scene
	scene_push("menu");

	// Begin main loop
	bool app_run = TRUE;
	while (app_run) {
		SDL_Event event;

		// Process events
		if (!SDL_WaitEvent(&event)) {
			// Exit if there was an error
			app_run = 0;
			continue;
		}

		switch (event.type) {
			case SDL_QUIT:
				app_run = FALSE;
				break;

			case SDL_KEYDOWN:
				event_raise("sdl-key-down", &event.key);
				break;

			case SDL_KEYUP:
				event_raise("sdl-key-up", &event.key);
				break;

			case SDL_MOUSEBUTTONDOWN:
				event_raise("sdl-mouse-down", &event.button);
				break;

			case SDL_MOUSEBUTTONUP:
				event_raise("sdl-mouse-up", &event.button);
				break;

			case SDL_MOUSEMOTION:
				event_raise("sdl-mouse-motion", &event.motion);
				break;

			case SDL_USEREVENT:
				event_raise("sdl-user", &event);
				break;
		}
	}

#ifdef DEBUG_EVENTS
	event_print_structure();
#endif

	// Destroy all modules
#ifdef DEBUG
	printf("destroying modules...\n");
	menu_destroy();
	printf("menu destroyed.\n");
	game_destroy();
	printf("game destroyed.\n");
	gfx_destroy();
	printf("gfx destroyed.\n");
	core_destroy();
	printf("core destroyed.\n");
	printf("\n");
#else
	menu_destroy();
	game_destroy();
	gfx_destroy();
	core_destroy();
#endif

#ifdef DEBUG
	printf("malloc count:     %6d\n", malloc_count);
	printf("free count:       %6d\n", free_count);
	printf("list alloc count: %6d\n", list_alloc_count);
	printf("list free coutn:  %6d\n", list_free_count);
#endif

	return 0;
}

/** @} */
