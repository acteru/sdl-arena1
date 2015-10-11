/*
 * common.c
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
 *  @addtogroup common
 *  @{
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include "common.h"



char		*app_path;

/**
 *  A helper function which gets the path of the application.
 *
 *  @note The returned path must be freed manually!
 *
 *  @param arg0		first argument passed to main
 *
 *  @returns		string containing the app path.
 */
char * _get_app_path(char *arg0)
{
	int len = strlen(arg0);
	int i;

	// Get index last separator
	for (i = len - 1; i > 0; i--) {
		if (arg0[i] == '/' || arg0[i] == '\\') {
			i++;
			break;
		}
	}

	// Create path
	char *ret = malloc(i + 1);
	strncpy(ret, arg0, i);
	ret[i] = '\0';

	return ret;
}

/**
 *  Initializes this module.
 */
void common_init(int argc, char *argv[])
{
	// Get path of application for further use
	app_path = _get_app_path(argv[0]);

	// Initialize SDL and open mixer API
	assert_ret(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER), 0, "couldn't initialize SDL", SDL_GetError);
	assert_ret(TTF_Init(), 0, "couldn't initialize TTF library", SDL_GetError);
	assert_ret(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024), 0, "couldn't initialize mixer API", Mix_GetError);

	// Allocate 16 mixing channels
	Mix_AllocateChannels(16);
}

/**
 *  Destroys this module freeing any allocated data.
 */
void common_destroy()
{
	free(app_path);

	Mix_CloseAudio();
	TTF_Quit();
	SDL_Quit();
}

/**
 *  Returns the path of the application.
 *
 *  @attention Don't modify or free this string!
 *
 *  @returns app path
 */
char * application_get_path()
{
	return app_path;
}

/**
 *  Puts a poison pill into the SDL event queue so that the application quits soon.
 */
void application_quit()
{
	SDL_Event evt;

	evt.quit.type = SDL_QUIT;
	SDL_PushEvent(&evt);
}

/**
 *  Checks a return value. If the return value is below zero, an error message is printed and the
 *  application exits.
 *
 *	@note This function should only be used during initialization.
 *
 *  @param retval	the return value to be checked
 *  @param noerror	the expected return value
 *  @param msg		the message to be printed in case of a failure
 *  @param err_msg	a pointer to a function which returns a more detailed error message
 */
void assert_ret(int retval, int noerror, char *msg, char *(*err_msg)())
{
	if (retval != noerror) {
		fprintf(stderr, "error: %s: %s\n", msg, err_msg());
		exit(EXIT_FAILURE);
	}
}

/**
 *  Checks a pointer. If the pointer is NULL, an error message is printed and the application exits.
 *
 *	@note This function should only be used during initialization.
 *
 *  @param ptr		the pointer to be checked
 *  @param msg		the message to be printed in case of a failure
 *  @param err_msg	a pointer to a function which returns a more detailed error message
 */
void assert_ptr(void *ptr, char *msg, char *(*err_msg)())
{
	if (!ptr) {
		fprintf(stderr, "error: %s: %s\n", msg, err_msg());
		exit(EXIT_FAILURE);
	}
}

/**
 *  A convenience function to get a random number matching a specified interval.
 *
 *  @param min		minimal value
 *  @param max		maximal value
 *
 *  @returns		random value
 */
int random(int min, int max)
{
	return rand() % (max - min) + min;
}

/**
 *  This function rounds a floating point value an integer value.
 *
 *  If the floating part is greater or equal to 0.5, the next bigger integer value is returned.
 *  Otherwise the next lower integer value is returned.
 *
 *  @param value	a float value
 *  @returns		rounded integer value
 */
int fround(float value)
{
	return (int)(value + 0.5f);
}

/**
 *  A convenience function to get a 2-dimensional integer vector with specified values.
 *
 *  @param x		x value
 *  @param y		y value
 *
 *  @returns		vector with x and y values
 */
Vector vrecti(int x, int y)
{
	Vector v = { x, y };
	return v;
}

/**
 *  A convenience funcion to get a 2-dimensional floating point vector with specified values.
 *
 *  @param x		x value
 *  @param y		y value
 *
 *  @returns		vector with x and y values
 */
VectorF vrect(float x, float y)
{
	VectorF v = { x, y };
	return v;
}

/**
 *  Loads a TTF font file. The application is exited if
 *  the font file couldn't be loaded.
 *
 *  @note The file name is expected to be relative to the application path.
 *
 *  @param file		file name of the TTF font file
 *  @param ptsize	size of the font
 *
 *  @returns		the loaded font
 */
TTF_Font * assert_font(char *file, int ptsize)
{
	char *path = malloc(strlen(app_path) + strlen(file) + 1);
	strcpy(path, app_path);
	strcat(path, file);

	TTF_Font *font = TTF_OpenFont(path, ptsize);
	assert_ptr(font, "couldn't load font", SDL_GetError);

	free(path);

	return font;
}

/**
 *  Loads an audio file. The application is exited if
 *  the audio file couldn't be loaded.
 *
 *  @note The file name is expected to be relative to the application path.
 *
 *  @param file		file name of the audio file
 *
 *  @returns		the loaded sample
 */
Mix_Chunk * assert_sample(char *file)
{
	char *path = malloc(strlen(app_path) + strlen(file) + 1);
	strcpy(path, app_path);
	strcat(path, file);

	Mix_Chunk *sample = Mix_LoadWAV(path);
	assert_ptr(sample, "couldn't load sample", Mix_GetError);

	free(path);

	return sample;
}

/**
 *  Loads a sprite file. The application is exited if
 *  the sprite file couldn't be loaded.
 *
 *  @note The file name is expected to be relative to the application path.
 *
 *  @param file		file name of the sprite file
 *
 *  @returns		the loaded sprite file
 */
SDL_Surface * assert_sprite(char *file)
{
	char *path = malloc(strlen(app_path) + strlen(file) + 1);
	strcpy(path, app_path);
	strcat(path, file);

	SDL_Surface *tmp = IMG_Load(path);
	assert_ptr(tmp, "couldn't load sprite", IMG_GetError);
	SDL_SetColorKey(tmp, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(tmp->format, 0xFF, 0x00, 0xFF));

	SDL_Surface *sprite;
	sprite = SDL_DisplayFormat(tmp);
	SDL_FreeSurface(tmp);

	free(path);

	return sprite;
}

/**
 *  Fills an array with clips to use for blitting from a sprite sheet.
 *
 *  @note This functions assumes that the sprites are aranged in one row.
 *
 *  @param clips		[out] array in which the clips are stored
 *  @param sprite_size	size in pixels of one sprite
 *  @param num_sprites	number of sprites in the sprite sheet
 */
void sprite_get_clips(SDL_Rect clips[], Size sprite_size, int num_sprites)
{
	SDL_Rect tmp;
	int i;

	tmp.w = sprite_size.w;
	tmp.h = sprite_size.h;
	tmp.x = tmp.y = 0;

	for (i = 0; i < num_sprites; i++) {
		clips[i] = tmp;
		tmp.x += sprite_size.w;
	}
}

/** @} */
