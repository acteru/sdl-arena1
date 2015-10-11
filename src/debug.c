/*
 * debug.c
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
 *  @addtogroup debug
 *  @{
 */

#include <stdio.h>
#include <stdlib.h>

int malloc_count = 0;	/**< number of times malloc was called */
int free_count = 0;		/**< number of times free was called */

/**
 *  Debug function which calls malloc and may print informations in real time.
 */
inline void * malloc_dbg(int size_t)
{
	void *ptr;

	malloc_count++;
	ptr = malloc(size_t);

#ifdef DEBUG_MALLOC
	printf("malloc: 0x%p (%3d bytes)\n", ptr, size_t);
#endif

	return ptr;
}

/**
 *  Debug function which calls free and may print informations in real time.
 */
inline void free_dbg(void *ptr)
{
	free_count++;
	free(ptr);

#ifdef DEBUG_MALLOC
	printf("free  : 0x%p\n", ptr);
#endif
}

/** @} */
