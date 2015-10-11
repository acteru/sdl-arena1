/*
 * upgrade.h
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
 *  @defgroup upgrade upgrade
 *  @brief Represents the class for upgrades.
 *
 *  This module represents the class for upgrade objects.
 *  Upgrades are static objects which can be taken by bombermans.
 *
 *  @{
 */

#ifndef UPGRADE_H_
#define UPGRADE_H_

#include "game.h"
#include "explosion.h"

/**
 *  This enum type defines an identifier for each upgrade type.
 */
typedef enum {
	UPG_BOMB		= 0,	/**< type id for a bomb upgrade */
	UPG_SPEED		= 1,	/**< type id for a speed upgrade */
	UPG_KICK		= 2,	/**< type id for a kick upgrade */
	UPG_BOX			= 3,	/**< type id for a box upgrade */
	UPG_EXPL		= 4,	/**< type id for an explosion upgrade */
	UPG_VIRUS		= 5,	/**< type id for a virus upgrade */
	UPG_EXPL_BIG	= 6,	/**< type id for a big explosion upgrade */
} UpgradeType;

typedef struct {
	GameObject		*bomberman;				/*<< the bomberman to which this info belongs to */
	int				 bombs_available;		/*<< number of bombs the bomberman has left */
	ExplosionInfo	 exp_info;				/*<< information about the explosion created by bombs of this bomberman */
} UpgradeInfo;

extern void upgrade_init();
extern void upgrade_destroy();

extern GameObject * upgrade_create();
extern void upgrade_free(GameObject *upgrade);
extern void upgrade_free_all();

extern void upgrade_apply(GameObject *upgrade, UpgradeInfo *target);

#endif /* UPGRADE_H_ */

/** @} */
