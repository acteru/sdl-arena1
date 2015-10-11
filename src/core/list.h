/*
 * list.h
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
 *  @file list.h
 *  This file is based on the GList module of the GLib library.
 *  @see https://developer.gnome.org/glib/stable/glib-Doubly-Linked-Lists.html
 *
 *  @defgroup list list
 *  @brief Provides functions to work with a doubly-linked list.
 *
 *  This module provides functions to work with a doubly-linked list which
 *  can be iterated over in both directions.
 *
 *  It is based on the GList module of the GLib library.
 *  @see https://developer.gnome.org/glib/stable/glib-Doubly-Linked-Lists.html
 *
 *  @{
 */

#ifndef LIST_H_
#define LIST_H_

/**
 *  The List struct is used for each element in a doubly-linked list.
 */
typedef struct _List List;
struct _List {
	void	*data;	/**< holds the element's data, which can be a pointer to any kind of data, or any integer value using casts. */
	List 	*next;	/**< contains the link to the next element in the list */
	List 	*prev; 	/**< contains the link to the previous element in the list */
};

/**
 *  Specifies the type of a comparison function used to compare two values.
 *  The function should return a negative integer if the
 *  first value comes before the second, 0 if they are equal, or a positive
 *  integer if the first value comes after the second.
 *
 *  @param a		a value.
 *  @param b		a value to compare with.
 *
 *  @returns		negative value if a < b; zero if a = b; positive value if a > b.
 */
typedef int (*CompareFunc) (void *a, void *b);

// --- Inserting ---

extern List * list_append(List *list, void *data);
extern List * list_prepend(List *list, void *data);
extern List * list_insert(List *list, void *data, int position);
extern List * list_insert_sorted(List *list, void *data, CompareFunc func);

// --- Using ---

/**
 *  A convenience macro to get the next element in a list.
 *
 *  @param link		an element in a list
 *
 *  @returns		the next element or NULL if there are no next elements
 */
#define list_next(link)		((link)->next)

/**
 *  A convenience macro to get the previous element in a list.
 *
 *  @param link		an element in a list
 *
 *  @returns		the previous element or NULL if there are no previous elements
 */
#define list_prev(link)		((link)->prev)

/**
 *  Gets the first element in a list (which is the list itself).
 *
 *  @param list		a list
 *
 *  @returns		the first element in the list or NULL if the list has no elements
 */
#define list_first(list)	(list)

extern List * list_last(List *list);
extern List * list_nth(List *list, int n);
extern List * list_find(List *list, void *data);
extern List * list_find_custom(List *list, void *data, CompareFunc func);

extern int list_length(List* list);

// --- Removing ---

extern List * list_remove(List *list, void *data);
extern List * list_delete_link(List *list, List *link);
extern List * list_free(List *list);
extern List * list_free_full(List *list, void (*free_func)(void *data));


#ifdef DEBUG
extern int list_alloc_count;
extern int list_free_count;
#endif

#endif /* LIST_H_ */

/** @} */
