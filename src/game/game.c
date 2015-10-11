/*
 * game.c
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

#include <string.h>			// memset function
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include "core/core.h"
#include "gfx.h"
#include "bomberman.h"
#include "rock.h"
#include "box.h"
#include "bomb.h"
#include "explosion.h"
#include "upgrade.h"
#include "game.h"



#define PADDING					50									/**< padding around the playfield in pixels */
#define NUM_UPG_BOMB			10									/**< number of bomb upgrades in total */
#define NUM_UPG_EXPL			10									/**< number of explosion upgrades in total */

static Size   		 field_size;									/**< size of one field. this is calculated during initialization */
static Vector 		 screen_offset;									/**< offset which we use to start drawing */

static int			 countdown_index;								/**< counter which is used for the countdown at the beginning of a game */
static bool			 gameover;										/**< wether the game is over */

static GameObject	*a_world[GAME_WORLD_WIDTH][GAME_WORLD_HEIGHT];	/**< the 2D array containing all game objects except the bombermans */
static List			*l_bombermans = NULL;							/**< the list containing all bomberman objects */

static TTF_Font		*f_default;										/**< font for text */
static SDL_Surface  *s_countdown[3];								/**< surfaces containing messages for countdown */
static SDL_Surface  *s_gameover;									/**< surface containing game over message */

static SDL_Surface	*s_screen;										/**< screen surface of the gfx module */
static SDL_Surface	*s_grass;										/**< grass sprite */
static SDL_Surface  *s_rock;										/**< rock sprite */

static Mix_Chunk	*a_countdown1;									/**< audio sample for countdown */
static Mix_Chunk	*a_countdown2;									/**< audio sample for countdown */
static Mix_Chunk	*a_gameover;									/**< audio sample for game over */

static GameObject	*bm_keyboard1 = NULL;							/**< the bomberman object which is mapped to the first keyboard layout */
static GameObject	*bm_keyboard2 = NULL;							/**< the bomberman object which is mapped to the second keyboard layout */

static int 			 evt_gfx_draw;									/**< id of the gfx-draw event handler */
static int			 evt_gfx_draw_text;								/**< id of the gfx-draw event handler text messages */
static int 			 evt_sdl_key_down;								/**< id of the sdl-key-down event handler */
static int 			 evt_sdl_key_up;								/**< id of the sdl-key-up event handler */
static int 			 evt_scene_changed;								/**< id of the scene-changed event handler */
static int			 evt_bomberman_died;							/**< id of the bomberman-died event handler */

static int			 tmr_game_init;									/**< id of the game-init timer */

// --- Default Playfield ------------------------------------------------------------------------------------------------------------------

/**
 *  The default world layout which is loaded at beginning.
 *
 *  @attention		Note that the y component comes first in this 2-dimensional array.
 *  				Because it's much simpler to write the layout in C code, this is different than at a_world.
 */
static const ObjectType default_world[GAME_WORLD_HEIGHT][GAME_WORLD_WIDTH] = {
		{ OBJ_NONE, OBJ_NONE, OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_NONE, OBJ_NONE },
		{ OBJ_NONE, OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_NONE },
		{ OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX  },
		{ OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX  },
		{ OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX  },
		{ OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX  },
		{ OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX  },
		{ OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX  },
		{ OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX  },
		{ OBJ_NONE, OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_BOX,  OBJ_ROCK, OBJ_NONE },
		{ OBJ_NONE, OBJ_NONE, OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_BOX,  OBJ_NONE, OBJ_NONE },
};

// --- Event Handlers ---------------------------------------------------------------------------------------------------------------------

/**
 *  Event handler for the gfx-draw event.
 *  This function draws the background of the game.
 */
static void _game_evt_gfx_draw(void *event_data, void *user_data)
{
	int x, y;

	// Draw border around the playfield
	for (x = -1; x < GAME_WORLD_WIDTH + 1; x++) {
		game_draw(s_rock, vrecti(x, -1), NULL);
		game_draw(s_rock, vrecti(x, GAME_WORLD_HEIGHT), NULL);
	}
	for (y = 0; y < GAME_WORLD_HEIGHT; y++) {
		game_draw(s_rock, vrecti(-1, y), NULL);
		game_draw(s_rock, vrecti(GAME_WORLD_WIDTH, y), NULL);
	}


	// Draw grass background
	for (y = 0; y < GAME_WORLD_HEIGHT; y++) {
		for (x = 0; x < GAME_WORLD_WIDTH; x++) {
			game_draw(s_grass, vrecti(x, y), NULL);
		}
	}
}

/**
 *  Event handler for the gfx-draw event.
 *  This function draws text messages.
 */
static void _game_evt_gfx_draw_text(void *event_data, void *user_data)
{
	SDL_Rect dest;

	// Draw countdown
	if (countdown_index >= 0) {
		SDL_Surface *s_msg = s_countdown[countdown_index];

		dest.x = GFX_SCREEN_WIDTH / 2 - s_msg->w / 2;
		dest.y = GFX_SCREEN_HEIGHT / 2 - s_msg->h / 2;
		SDL_BlitSurface(s_msg, NULL, s_screen, &dest);
	}

	// Draw game over message
	if (gameover) {
		dest.x = GFX_SCREEN_WIDTH / 2 - s_gameover->w / 2;
		dest.y = GFX_SCREEN_HEIGHT / 2 - s_gameover->h / 2;
		SDL_BlitSurface(s_gameover, NULL, s_screen, &dest);
	}
}

/**
 *  Event handler for the sdl-key-down event.
 *  This function handles key down events for the game.
 */
static void _game_evt_sdl_key_down(void *event_data, void *user_data)
{
	SDL_KeyboardEvent *event = (SDL_KeyboardEvent *)event_data;

	// Get the direction
	Direction dir1, dir2;
	dir1 = bomberman_get_direction(bm_keyboard1);
	dir2 = bomberman_get_direction(bm_keyboard2);

	// Apply key down event
	switch (event->keysym.sym) {
		case SDLK_UP:
			dir1 |= DIR_UP;
			break;

		case SDLK_DOWN:
			dir1 |= DIR_DOWN;
			break;

		case SDLK_LEFT:
			dir1 |= DIR_LEFT;
			break;

		case SDLK_RIGHT:
			dir1 |= DIR_RIGHT;
			break;

		case SDLK_RETURN:
			bomberman_lay_bomb(bm_keyboard1);
			break;


		case SDLK_w:
			dir2 |= DIR_UP;
			break;

		case SDLK_s:
			dir2 |= DIR_DOWN;
			break;

		case SDLK_a:
			dir2 |= DIR_LEFT;
			break;

		case SDLK_d:
			dir2 |= DIR_RIGHT;
			break;

		case SDLK_TAB:
			if (!gameover) {
				bomberman_lay_bomb(bm_keyboard2);
			}
			break;

		case SDLK_ESCAPE:
			scene_pop();
			return;

		default:
			break;
	}

	if (!gameover) {
		// Set direction again
		bomberman_set_direction(bm_keyboard1, dir1);
		bomberman_set_direction(bm_keyboard2, dir2);
	}
}

/**
 *  Event handler for the sdl-key-up event.
 *  This function handles key up events for the game.
 */
static void _game_evt_sdl_key_up(void *event_data, void *user_data)
{
	SDL_KeyboardEvent *event = (SDL_KeyboardEvent *)event_data;

	// Get the direction
	Direction dir1, dir2;
	dir1 = bomberman_get_direction(bm_keyboard1);
	dir2 = bomberman_get_direction(bm_keyboard2);

	// Apply key up event
	switch (event->keysym.sym) {
		case SDLK_UP:
			dir1 &= ~DIR_UP;
			break;

		case SDLK_DOWN:
			dir1 &= ~DIR_DOWN;
			break;

		case SDLK_LEFT:
			dir1 &= ~DIR_LEFT;
			break;

		case SDLK_RIGHT:
			dir1 &= ~DIR_RIGHT;
			break;


		case SDLK_w:
			dir2 &= ~DIR_UP;
			break;

		case SDLK_s:
			dir2 &= ~DIR_DOWN;
			break;

		case SDLK_a:
			dir2 &= ~DIR_LEFT;
			break;

		case SDLK_d:
			dir2 &= ~DIR_RIGHT;
			break;

		default:
			break;
	}

	// Set the direction again
	bomberman_set_direction(bm_keyboard1, dir1);
	bomberman_set_direction(bm_keyboard2, dir2);
}

/**
 *  Event handler for the scene-changed event.
 *  This starts respectively stops the game.
 */
static void _game_evt_scene_changed(void *event_data, void *user_data)
{
	if (scene_check("game")) {
		// Enable events
		event_handler_set_state(evt_gfx_draw, EVENT_HANDLER_ENABLED);
		event_handler_set_state(evt_gfx_draw_text, EVENT_HANDLER_ENABLED);
		event_handler_set_state(evt_bomberman_died, EVENT_HANDLER_ENABLED);

		// Create bombermans
		bm_keyboard1 = bomberman_create(vrecti(GAME_WORLD_WIDTH - 1, GAME_WORLD_HEIGHT - 1), BM_WHITE);
		bm_keyboard2 = bomberman_create(vrecti(0, 0), BM_BLUE);

		l_bombermans = list_append(l_bombermans, bm_keyboard1);
		l_bombermans = list_append(l_bombermans, bm_keyboard2);

		// Initialize world (load default world)
		int x, y;
		for (y = 0; y < GAME_WORLD_HEIGHT; y++) {
			for (x = 0; x < GAME_WORLD_WIDTH; x++) {
				switch (default_world[y][x]) {
					case OBJ_BOX:
						box_create(vrecti(x, y));
						break;

					case OBJ_ROCK:
						rock_create(vrecti(x, y));
						break;

					default:
						break;
				}
			}
		}

		// Initialize upgrades
		Vector pos = { -1, -1 };
		GameObject *upgrades[NUM_UPG_BOMB + NUM_UPG_EXPL];
		int i = 0, z;

		for (z = 0; z < NUM_UPG_BOMB; z++) {
			upgrades[i] = upgrade_create(pos, UPG_BOMB);
			i++;
		}

		for (z = 0; z < NUM_UPG_EXPL; z++) {
			upgrades[i] = upgrade_create(pos, UPG_EXPL);
			i++;
		}

		box_distribute(upgrades, i);

		// Initialize countdown
		gameover = FALSE;
		countdown_index = 2;
		timer_set_state(tmr_game_init, TIMER_ENABLED);
		event_handler_set_state(evt_gfx_draw_text, EVENT_HANDLER_ENABLED);
		Mix_PlayChannel(-1, a_countdown1, 0);
	}
	else {
		// Disable events
		event_handler_set_state(evt_gfx_draw, EVENT_HANDLER_DISABLED);
		event_handler_set_state(evt_gfx_draw_text, EVENT_HANDLER_DISABLED);
		event_handler_set_state(evt_sdl_key_down, EVENT_HANDLER_DISABLED);
		event_handler_set_state(evt_sdl_key_up, EVENT_HANDLER_DISABLED);
		event_handler_set_state(evt_bomberman_died, EVENT_HANDLER_DISABLED);

		// Disable timers
		timer_set_state(tmr_game_init, TIMER_DISABLED);

		// Free all objects
		bomberman_free_all();
		bomb_free_all();
		explosion_free_all();
		box_free_all();
		rock_free_all();
		upgrade_free_all();

		// Free bomberman list
		l_bombermans = list_free(l_bombermans);
		bm_keyboard1 = bm_keyboard2 = NULL;
	}
}

/**
 *  Event handler for the bomberman-died event.
 *  Ends the game if less than 2 bombermans are alive.
 */
static void _game_evt_bomberman_died(void *event_data, void *user_data)
{
	int n_bombermans_alive = 0;

	// Count bombermans alive
	List *link = list_first(l_bombermans);
	while (link) {
		GameObject *bomberman = (GameObject *)link->data;

		if (bomberman_is_alive(bomberman)) {
			n_bombermans_alive++;
		}

		link = list_next(link);
	}

	if (n_bombermans_alive < 2) {
		gameover = TRUE;
		Mix_PlayChannel(-1, a_gameover, 0);

		// Make sure nobody dies
		bomb_free_all();

		// Stop all moving bombermans
		link = list_first(l_bombermans);
		while (link) {
			GameObject *bomberman = (GameObject *)link->data;
			bomberman_set_direction(bomberman, DIR_NONE);
			link = list_next(link);
		}
	}
}

// --- Timer Callback Functions -----------------------------------------------------------------------------------------------------------

/**
 *  Callback function for the game-init timer.
 *  This timer creates the countdown and finally starts the game by
 *  enabling keyboard events.
 */
static void _game_tmr_game_init(void *user_data)
{
	countdown_index--;

	if (countdown_index > 0) {
		// Lower tone
		Mix_PlayChannel(-1, a_countdown1, 0);
	}
	else if (countdown_index == 0) {
		// Higher tone
		Mix_PlayChannel(-1, a_countdown2, 0);

		// Enable events. The players can move if the "Go"-Sprite appears.
		event_handler_set_state(evt_sdl_key_up, EVENT_HANDLER_ENABLED);
		event_handler_set_state(evt_sdl_key_down, EVENT_HANDLER_ENABLED);
	}
	else {
		// Disable countdown related stuff
		timer_set_state(tmr_game_init, TIMER_DISABLED);
	}
}

// --- Public Functions -------------------------------------------------------------------------------------------------------------------

/**
 *  Initializes this module.
 */
void game_init()
{
	int field_size1, field_size2;

	// Initialize the playfield
	memset(a_world, 0, sizeof(a_world));

	// Calculate size of one field and the offset
	field_size1 = (GFX_SCREEN_WIDTH - 2 * PADDING) / GAME_WORLD_WIDTH;
	field_size2 = (GFX_SCREEN_HEIGHT - 2 * PADDING) / GAME_WORLD_HEIGHT;

	if (field_size1 < field_size2) {
		field_size.w = field_size.h = field_size1;
		screen_offset.x = PADDING;
		screen_offset.y = PADDING + ((GFX_SCREEN_HEIGHT - 2 * PADDING) - (field_size1 * GAME_WORLD_HEIGHT)) / 2;
	}
	else {
		field_size.w = field_size.h = field_size2;
		screen_offset.x = PADDING + ((GFX_SCREEN_WIDTH - 2 * PADDING) - (field_size2 * GAME_WORLD_WIDTH)) / 2;
		screen_offset.y = PADDING;
	}

	// Register events
	evt_gfx_draw = event_connect("gfx-draw", 1, _game_evt_gfx_draw, NULL, EVENT_HANDLER_DISABLED);
	evt_gfx_draw_text = event_connect("gfx-draw", -1, _game_evt_gfx_draw_text, NULL, EVENT_HANDLER_DISABLED);
	evt_sdl_key_down = event_connect("sdl-key-down", 0, _game_evt_sdl_key_down, NULL, EVENT_HANDLER_DISABLED);
	evt_sdl_key_up = event_connect("sdl-key-up", 0, _game_evt_sdl_key_up, NULL, EVENT_HANDLER_DISABLED);
	evt_scene_changed = event_connect("scene-changed", 0, _game_evt_scene_changed, NULL, EVENT_HANDLER_ENABLED);
	evt_bomberman_died = event_connect("bomberman-died", 0, _game_evt_bomberman_died, NULL, EVENT_HANDLER_DISABLED);

	// Create timers
	tmr_game_init = timer_create_interval(1000, _game_tmr_game_init, NULL, TIMER_DISABLED);

	// Load sprites
	s_screen = gfx_get_screen();
	s_grass = assert_sprite("sprites\\grass.png");
	s_rock = assert_sprite("sprites\\rock.png");

	// Load fonts
	f_default = assert_font("FreeSans.ttf", 200);

	// Render texts
	SDL_Color color = { 255, 0, 0 };
	s_countdown[2] = TTF_RenderText_Solid(f_default, "Ready", color);
	s_countdown[1] = TTF_RenderText_Solid(f_default, "Set", color);
	s_countdown[0] = TTF_RenderText_Solid(f_default, "Go!", color);
	s_gameover =	 TTF_RenderText_Solid(f_default, "Game Over", color);

	// Load audio files
	a_countdown1 = assert_sample("sounds\\countdown-a.ogg");
	a_countdown2 = assert_sample("sounds\\countdown-b.ogg");
	a_gameover =   assert_sample("sounds\\gameover.ogg");

	// Init all sub-modules
	bomberman_init();
	bomb_init();
	explosion_init();
	rock_init();
	box_init();
	upgrade_init();
}

/**
 *  Destroys this module freeing any allocated data.
 */
void game_destroy()
{
	// Unregister events
	event_disconnect(evt_gfx_draw);
	event_disconnect(evt_gfx_draw_text);
	event_disconnect(evt_sdl_key_down);
	event_disconnect(evt_sdl_key_up);
	event_disconnect(evt_scene_changed);
	event_disconnect(evt_bomberman_died);

	// Free timers
	timer_free(tmr_game_init);

	// Free sprites
	SDL_FreeSurface(s_grass);
	SDL_FreeSurface(s_rock);

	// Free texts
	SDL_FreeSurface(s_countdown[2]);
	SDL_FreeSurface(s_countdown[1]);
	SDL_FreeSurface(s_countdown[0]);
	SDL_FreeSurface(s_gameover);

	// Free fonts
	TTF_CloseFont(f_default);

	// Free audio files
	Mix_FreeChunk(a_countdown1);
	Mix_FreeChunk(a_countdown2);
	Mix_FreeChunk(a_gameover);

	// Destroy all sub-modules
	bomberman_destroy();
	bomb_destroy();
	explosion_destroy();
	rock_destroy();
	box_destroy();
	upgrade_destroy();

	// Free other stuff
	l_bombermans = list_free(l_bombermans);
}

/**
 *  Gets the object which is currently at a specified position.
 *
 *  @note Bomberman objects aren't handled by this function.
 *
 *  @param pos		position to check
 *  @returns		pointer to the game object or NULL if the field is free.
 */
GameObject * game_get_field(Vector pos)
{
	return a_world[pos.x][pos.y];
}


/**
 *  Sets an object to a specified position.
 *
 *	@attention Don't forget to free the object which is previously on the field!
 *
 *  @note Bomberman objects aren't handled by this function.
 *
 *  @param pos		position at which to set the object
 *  @param obj		a game object
 */
void game_set_field(Vector pos, GameObject *obj)
{
	// Apply position to object
	if (obj) obj->pos = pos;

	// Check whether object position is valid
	if (pos.x < 0 || pos.x >= GAME_WORLD_WIDTH) return;
	if (pos.y < 0 || pos.y >= GAME_WORLD_HEIGHT) return;

	a_world[pos.x][pos.y] = obj;
}

/**
 *  Gets the screen coordinates of a specified field.
 *  This function is useful for drawing.
 *
 *  @param pos		position of the field for which to get the coordinates
 *  @param coords	[out] where to store the coordinates
 */
void game_get_field_coords(Vector pos, SDL_Rect *coords)
{
	coords->w = field_size.w;
	coords->h = field_size.h;

	coords->x = screen_offset.x + pos.x * field_size.w;
	coords->y = screen_offset.y + pos.y * field_size.h;
}

/**
 *  Draws a sprite onto the playfield.
 *
 *  @param sprite	sprite which shall be drawn
 *  @param pos		coordinates of the field where to draw the surface
 *  @param clip		part of the surface which to draw (optional)
 */
void game_draw(SDL_Surface *sprite, Vector pos, SDL_Rect *clip)
{
	game_draw_floating(sprite, vrect((int)pos.x, (int)pos.y), clip);
}

/**
 *  Draws a sprite onto the playfield.
 *  This function uses a floating point vector as position. This means that objects
 *  can be drawn between fields.
 *
 *  @param sprite	sprite which shall be drawn
 *  @param pos		coordinates of the field where to draw the surface
 *  @param clip		part of the surface which to draw (optional)
 */
void game_draw_floating(SDL_Surface *sprite, VectorF pos, SDL_Rect *clip)
{
	Vector posi;
	SDL_Rect dest;

	// Get coordinates
	posi = vrecti(fround(pos.x), fround(pos.y));
	game_get_field_coords(posi, &dest);

	// Apply floating part
	dest.x += (pos.x - (float)posi.x) * dest.w;
	dest.y += (pos.y - (float)posi.y) * dest.h;

	// Adjust y coordinate for sprites which are higher than one field height
	if (clip) {
		dest.y = dest.y + dest.h - clip->h;
	}
	else {
		dest.y = dest.y + dest.h - sprite->h;
	}

	// Blit surface
	SDL_BlitSurface(sprite, clip, s_screen, &dest);
}

/**
 *  Frees an arbitrary object by calling the approtiate free function.
 *
 *  @param obj		a game object
 */
void game_free_object(GameObject *obj)
{
	if (obj->type == OBJ_BOMBERMAN) {
		// Remove from list
		l_bombermans = list_remove(l_bombermans, obj);
	}
	else {
		// Remove from world array
		a_world[obj->pos.x][obj->pos.y] = NULL;
	}

	// Call the right free function
	switch (obj->type) {
		case OBJ_BOMBERMAN:
			bomberman_free(obj);
			break;

		case OBJ_BOMB:
			bomb_free(obj);
			break;

		case OBJ_EXPLOSION:
			explosion_free(obj);
			break;

		case OBJ_ROCK:
			rock_free(obj);
			break;

		case OBJ_BOX:
			box_free(obj);
			break;

		case OBJ_UPGRADE:
			upgrade_free(obj);
			break;

		case OBJ_NONE:
			break;
	}
}

/**
 *  Prints a layout of the world in the current state.
 *  May be useful for debugging purposes.
 */
void game_print_world_layout()
{
	int y, x;
	for (y = 0; y < GAME_WORLD_HEIGHT; y++) {
		for (x = 0; x < GAME_WORLD_WIDTH; x++) {
			if (a_world[x][y]) {
				printf("%1d", a_world[x][y]->type);
			}
			else {
				printf("0");
			}
		}
		printf("\n");
	}
}

/** @} */
