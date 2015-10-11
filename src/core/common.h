/*
 * common.h
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
 *  @defgroup common common
 *  @brief Contains declarations of general data types and some general purpose functions.
 *  @{
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>

typedef int bool;		/**< a data type which can be either TRUE or FALSE */
#define TRUE		1	/**< value 1 which represents a logical "true" */
#define FALSE		0	/**< value 0 which represents a logical "false" */

/**
 *  Represents a vector in a 2 dimensional space with integer values.
 */
typedef struct {
	int x;
	int y;
} Vector;

/**
 *  Represents a vector in a 2 dimensional space with float values.
 */
typedef struct {
	float x;
	float y;
} VectorF;

/**
 *  Represents a size in a 2 dimensional space with integer values.
 */
typedef struct {
	int w;		/**< width */
	int h;		/**< height */
} Size;

/**
 *  Represents a direction.
 */
typedef enum {
	DIR_NONE 	= 0x00,
	DIR_RIGHT 	= 0x01,
	DIR_LEFT 	= 0x02,
	DIR_UP		= 0x04,
	DIR_DOWN 	= 0x08,
} Direction;

// --- General functions ---

extern void common_init(int argc, char *argv[]);
extern void common_destroy();

extern char * application_get_path();
extern void application_quit();


extern void assert_ret(int retval, int noerror, char *msg, char *(*err_msg)());
extern void assert_ptr(void *ptr, char *msg, char *(*err_msg)());

extern int rand2(int min, int max);
extern int fround(float value);

// --- Vectors ---

extern Vector vrecti(int x, int y);
extern VectorF vrect(float x, float y);

// --- Text functions ---

extern TTF_Font * assert_font(char *file, int ptsize);

// --- Audio functions ---

extern Mix_Chunk * assert_sample(char *file);

// --- Sprite functions ---

extern SDL_Surface * assert_sprite(char *file);
extern void sprite_get_clips(SDL_Rect clips[], Size sprite_size, int num_sprites);

#endif /* COMMON_H_ */

/** @} */
