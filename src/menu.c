/*
 * menu.c
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
 *  @addtogroup menu
 *  @{
 */

#include <SDL/SDL.h>
#include "core/core.h"



static int 			 evt_gfx_draw;									/**< id of the gfx-draw event handler */
static int 			 evt_sdl_key_down;								/**< id of the sdl-key-down event handler */
static int 			 evt_scene_changed;								/**< id of the scene-changed event handler */

static SDL_Surface	*s_menu;

static void _menu_evt_gfx_draw(void *event_data, void *user_data)
{
	SDL_Surface *screen = (SDL_Surface *)event_data;

	SDL_BlitSurface(s_menu, NULL, screen, NULL);
}

static void _menu_evt_sdl_key_down(void *event_data, void *user_data)
{
	SDL_KeyboardEvent *event = (SDL_KeyboardEvent *)event_data;

	switch (event->keysym.sym) {
		case SDLK_RETURN:
			scene_push("game");
			break;

		case SDLK_ESCAPE:
			application_quit();
			break;

		default:
			break;
	}
}

static void _menu_evt_scene_changed(void *event_data, void *user_data)
{
	if (scene_check("menu")) {
		event_handler_set_state(evt_gfx_draw, EVENT_HANDLER_ENABLED);
		event_handler_set_state(evt_sdl_key_down, EVENT_HANDLER_ENABLED);
	}
	else {
		event_handler_set_state(evt_gfx_draw, EVENT_HANDLER_DISABLED);
		event_handler_set_state(evt_sdl_key_down, EVENT_HANDLER_DISABLED);
	}
}


void menu_init()
{
	evt_gfx_draw = event_connect("gfx-draw", 0, _menu_evt_gfx_draw, NULL, EVENT_HANDLER_DISABLED);
	evt_sdl_key_down = event_connect("sdl-key-down", 0, _menu_evt_sdl_key_down, NULL, EVENT_HANDLER_DISABLED);
	evt_scene_changed = event_connect("scene-changed", 0, _menu_evt_scene_changed, NULL, EVENT_HANDLER_ENABLED);

	s_menu = assert_sprite("sprites\\menu.png");
}

void menu_destroy()
{
	event_disconnect(evt_gfx_draw);
	event_disconnect(evt_sdl_key_down);
	event_disconnect(evt_scene_changed);

	SDL_FreeSurface(s_menu);
}

/** @} */
