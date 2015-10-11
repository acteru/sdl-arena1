/*
 * debug.h
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
 *  @defgroup debug debug
 *  @brief Contains debugging facilities.
 *
 *	This module contains helper functions to debug the application.
 *
 *  @{
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#ifdef DEBUG

extern int malloc_count;
extern int free_count;

extern inline void * malloc_dbg(int size_t);
extern inline void free_dbg(void *ptr);

#define malloc		malloc_dbg
#define free		free_dbg

#endif

#endif /* DEBUG_H_ */

/** @} */
