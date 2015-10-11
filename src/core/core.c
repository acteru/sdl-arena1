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
 *  @addtogroup core
 *  @{
 */

#include "common.h"
#include "event.h"
#include "timer.h"
#include "scene.h"
#include "core.h"



/**
 *  Initializes this module.
 */
void core_init(int argc, char *argv[])
{
#ifdef DEBUG
	common_init(argc, argv);
	printf("-> common initialized.\n");
	event_init();
	printf("-> event initialized.\n");
	timer_init();
	printf("-> timer initialized.\n");
	scene_init();
	printf("-> scene initialized.\n");
#else
	common_init(argc, argv);
	event_init();
	timer_init();
	scene_init();
#endif
}

/**
 *  Destroys this module freeing any allocated data.
 */
void core_destroy()
{
#ifdef DEBUG
	scene_destroy();
	printf("-> scene destroyed.\n");
	timer_destroy();
	printf("-> timer destroyed.\n");
	event_destroy();
	printf("-> event destroyed.\n");
	common_destroy();
	printf("-> common destroyed.\n");
#else
	scene_destroy();
	timer_destroy();
	event_destroy();
	common_destroy();
#endif
}

/** @} */
