/*
 * core.h
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
 *  @defgroup core core
 *  @brief Main module for the framework.
 *
 *	This module may be used to initialize and destroy all sub-modules
 *	of the framework.
 *
 *  @{
 */

#ifndef CORE_H_
#define CORE_H_

#include "common.h"
#include "list.h"
#include "event.h"
#include "timer.h"
#include "scene.h"

extern void core_init();
extern void core_destroy();

#endif /* CORE_H_ */

/** @} */
