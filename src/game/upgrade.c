/*
 * upgrade.c
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
 *  @addtogroup upgrade
 *  @{
 */

#include <SDL/SDL.h>
#include "core/core.h"
#include "game.h"
#include "upgrade.h"



/**
 *  This struct holds the data for a upgrade object.
 */
typedef struct {
	GameObject base;						/**< data from the base class */
	UpgradeType type;						/**< type of the upgrade (equal to the sprite index) */
} UpgradeObject;

static List			*l_upgrades = NULL;		/**< list of all existing upgrade objects */

static SDL_Surface	*s_upgrade;				/**< upgrade sprite */
static SDL_Rect		 s_upgrade_clips[7];	/**< clips for the upgrade sprite */

static Mix_Chunk	*a_pick;				/**< sound sample for an upgrade pick */

static int			 evt_gfx_draw;			/**< id of the gfx-draw event handler */
static int			 evt_explosion_hit;		/**< id of the explosion-hit event handler */


/**
 *  Event handler for the gfx-draw event.
 *  Draws all upgrades.
 */
static void _upgrade_evt_gfx_draw(void *event_data, void *user_data)
{
	// Loop through all upgrade objects
	List *link = list_first(l_upgrades);
	while (link) {
		UpgradeObject *upgrade = (UpgradeObject *)link->data;

		if (upgrade->base.pos.x >= 0 && upgrade->base.pos.y >= 0) {
			game_draw(s_upgrade, upgrade->base.pos, &s_upgrade_clips[upgrade->type]);
		}

		link = list_next(link);
	}
}

/**
 *  Event handler for the explosion-hit event.
 *  If an upgrade is hit by an explosion, it is removed here.
 */
static void _upgrade_evt_explosion_hit(void *event_data, void *user_data)
{
	GameObject *obj = (GameObject *)event_data;

	if (obj->type == OBJ_UPGRADE) {
		// Free upgrade
		upgrade_free(obj);
	}
}

/**
 *  Initializes this module.
 */
void upgrade_init()
{
	// Register events
	evt_gfx_draw = event_connect("gfx-draw", 0, _upgrade_evt_gfx_draw, NULL, EVENT_HANDLER_ENABLED);
	evt_explosion_hit = event_connect("explosion-hit", 0, _upgrade_evt_explosion_hit, NULL, EVENT_HANDLER_ENABLED);

	// Load sprites
	s_upgrade = assert_sprite("sprites/upgrades.png");

	// Load samples
	a_pick = assert_sample("sounds/pick.ogg");

	// Initialize clips
	Size clip_size = { 60, 60 };
	sprite_get_clips(s_upgrade_clips, clip_size, 7);
}

/**
 *  Destroys this module freeing any allocated data.
 */
void upgrade_destroy()
{
	// Free all objects
	upgrade_free_all();

	// Unregister events
	event_disconnect(evt_gfx_draw);
	event_disconnect(evt_explosion_hit);

	// Free sprites
	SDL_FreeSurface(s_upgrade);

	// Free samples
	Mix_FreeChunk(a_pick);
}

/**
 *  Creates a new upgrade object.
 *
 *  @param pos		initial position of the new object ({-1, -1}, if the upgrade is not on the field
 *  @param type		type of the upgrade
 *  @returns		the newly created upgrade object
 */
GameObject * upgrade_create(Vector pos, UpgradeType type)
{
	UpgradeObject *upgrade = (UpgradeObject *)malloc(sizeof(UpgradeObject));
	upgrade->base.type = OBJ_UPGRADE;
	upgrade->base.pos = pos;
	upgrade->type = type;

	game_set_field(pos, (GameObject *)upgrade);

	l_upgrades = list_append(l_upgrades, upgrade);

	return (GameObject *)upgrade;
}

/**
 *  Frees an upgrade object.
 *
 *  @param upgrade		an upgrade object
 */
void upgrade_free(GameObject *upgrade)
{
	game_set_field(upgrade->pos, NULL);

	l_upgrades = list_remove(l_upgrades, upgrade);

	free(upgrade);
}

/**
 *  Frees all existing upgrade objects.
 */
void upgrade_free_all()
{
	// Loop through all upgrade objects
	List *link = list_first(l_upgrades);
	while (link) {
		UpgradeObject *upgrade = (UpgradeObject *)link->data;
		link = list_next(link);

		upgrade_free((GameObject *)upgrade);
	}
}

/**
 *  Applies an upgrade to a target.
 *
 *  @param upgrade		an upgrade object
 *  @param target		a target to which the upgrade shall be applied
 */
void upgrade_apply(GameObject *upgrade, UpgradeInfo *target)
{
	UpgradeObject *object = (UpgradeObject *)upgrade;

	// Apply upgrade
	switch (object->type) {
		case UPG_BOMB:
			target->bombs_available++;
			break;

		case UPG_EXPL:
			target->exp_info.length++;
			break;

		default:
			break;
	}

	// Play sound
	Mix_PlayChannel(-1, a_pick, 0);
}

/** @} */
