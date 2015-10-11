/*
 * list.c
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
 *  @addtogroup list
 *  @{
 */

#include <stdlib.h>
#include "list.h"


int list_alloc_count = 0;		/**< may be used for debugging */
int list_free_count = 0;		/**< may be used for debugging */

// --- Private ---

/**
 *  Allocates space for one list element.
 *  It is called by list_append(), list_prepend() and list_insert().
 *
 *  @returns		the newly allocated list element.
 */
static List * _list_alloc_link()
{
	List *link;

	link = (List *)malloc(sizeof(List));
	link->next = link->prev = NULL;

	list_alloc_count++;

	return link;
}

/**
 *  Frees one list element.
 *
 *  @param link		a list element
 */
static void _list_free_link(List *link)
{
	free(link);
	list_free_count++;
}

// --- Inserting ---

/**
 *  Adds a new element on to the end of the list.
 *
 *  @param list		a list
 *  @param data		the data for the new element
 *
 *  @returns		the new start of the list
 */
List * list_append(List *list, void *data)
{
	List *link;
	List *last;

	// Create the new link
	link = _list_alloc_link();
	link->data = data;

	// Are there already elements in the list?
	if (list) {
		// Append the new link
		last = list_last(list);
		last->next = link;
		link->prev = last;

		return list;
	}
	else {
		// This element represents the new list
		return link;
	}
}

/**
 *  Adds a new element on to the start of the list.
 *
 *  @param list		a list
 *  @param data		the data for the new element
 *
 *  @returns		the new start of the list
 */
List * list_prepend(List *list, void *data)
{
	List *link;

	// Create the new link
	link = _list_alloc_link();
	link->data = data;

	// Are there already elements in the list?
	if (list) {
		// Set up links
		list->prev = link;
		link->next = list;
	}

	// This elements is now the first one
	return link;
}

/**
 *  Inserts a new element into the list at the given position.
 *
 *  @param list			a list
 *  @param data			the data for the new element
 *  @param position		the position to insert the element
 *
 *  @returns			the new start of the list
 */
List * list_insert(List *list, void *data, int position)
{
	List *link;
	List *tmp;

	// If the position is below zero, we will append the element instead
	if (position < 0)
		return list_append(list, data);
	// If the position is 0, it's the same as prepending
	if (position == 0)
		return list_prepend(list, data);

	// Create the new link
	link = _list_alloc_link();
	link->data = data;

	// Get the element which is currently at the position
	tmp = list_nth(list, position);
	// If we couldn't get it, position must be out of range. So let's append
	// the new link instead
	if (!tmp)
		return list_append(list, data);

	// Set up the new links
	tmp->prev->next = link;
	link->prev = tmp->prev;

	link->next = tmp;
	tmp->prev = link;

	// The list remains the same, because if we are here, the element is
	// not the first and there were already elements in the list.
	return list;
}

/**
 *  Inserts a new element into the list, using the given comparison function to determine its position.
 *
 *  @param list		a list
 *  @param data		the data for the new element
 *  @param func		the function to compare elements in the list. It should return a number > 0 if the first parameter comes after the second parameter in the sort order.
 *
 *  @returns		the new start of the list
 */
List * list_insert_sorted(List *list, void *data, CompareFunc func)
{
	List *tmp_list = list;
	List *new_list;
	int cmp;

	// Create the new link
	new_list = _list_alloc_link();
	new_list->data = data;

	// Are there no elements in the list?
	if (!list) {
		return new_list;
	}

	// Do comparison until the reached element is bigger than
	// the new one
	cmp = func(data, tmp_list->data);
	while (tmp_list->next && cmp > 0) {
		tmp_list = tmp_list->next;
		cmp = func(data, tmp_list->data);
	}

	// Check, whether this element is the last one and the
	// new element goes behind it.
	if (!tmp_list->next && cmp > 0) {
		tmp_list->next = new_list;
		new_list->prev = tmp_list;
		return list;
	}

	// If we reach this point, tmp_list is the element which is
	// directly after the new element. Now let's check whether
	// it isn't the first one and finally connect the elements.
	if (tmp_list->prev) {
		tmp_list->prev->next = new_list;
		new_list->prev = tmp_list->prev;
	}
	new_list->next = tmp_list;
	tmp_list->prev = new_list;

	if (tmp_list == list)
		return new_list;	// New element is now the first one
	else
		return list;		// First element remains the same
}

// --- Using ---

/**
 *  Gets the last element in a list.
 *
 *  @param list		a list
 *
 *  @returns		the last element in the list or NULL if the list has no elements
 */
List * list_last(List *list)
{
	// Step trough the list until we don't have a next element
	if (list) {
		while (list->next)
			list = list->next;
	}

	return list;
}

/**
 *  Gets the element at the given position in a list.
 *
 *  @param list		a list
 *  @param n		the position of the element, counting from 0
 *
 *  @returns		the element or NULL if the position is off the end of the list
 */
List * list_nth(List *list, int n)
{
	// Step through the list until we reached the nth element
	while ((n-- > 0) && list)
		list = list->next;

	return list;
}

/**
 *  Finds the element in a list which contains the given data.
 *
 *  @param list		a list
 *  @param data		the element data to find
 *
 *  @returns		the found list element or NULL if it is not found.
 */
List * list_find(List *list, void *data)
{
	while (list) {
		if (list->data == data)
			return list;

		list = list->next;
	}

	return NULL;
}

/**
 *  Finds an element in a list, using a supplied function to find the desired
 *  element. It iterates over the list, calling the given function which should
 *  return 0 when the desired element is found. The function takes two
 *  pointer arguments, the list element's data as the first argument
 *  and the given user data.
 *
 *  @param list		a list
 *  @param data		user data passed to the function
 *  @param func		the function to call for each element. It should return 0 when the desired element is found
 *
 *  @returns		the found list element or NULL if it is not found
 */
List * list_find_custom(List *list, void *data, CompareFunc func)
{
	while (list) {
		if (func(list->data, data) == 0)
			return list;

		list = list->next;
	}

	return NULL;
}

/**
 *  Gets the number of elements in a list.
 *
 *  @param list		a list
 *
 *  @returns		the number of elements in the list
 */
int list_length(List* list)
{
	int length = 0;

	// Step trough the list and count the items
	while (list) {
		length++;
		list = list->next;
	}

	return length;
}

// --- Removing ---

/**
 * Removes an element from a list.
 * If two elements contain the same data, only the first is removed.
 * If none of the elements contain the data, the list is unchanged.
 *
 * @param list		a list
 * @param data		the data of the element to remove
 */
List * list_remove(List *list, void *data)
{
	List *tmp;

	tmp = list_find(list, data);
	if (tmp)
		return list_delete_link(list, tmp);
	else {
		return list;
	}
}

/**
 *  Removes the node link from the list and frees it.
 *
 *  @param list		a list
 *  @param link		node to delete from list
 *
 *  @returns		the new start of the list
 */
List * list_delete_link(List *list, List *link)
{
	// Set the links of the neighbour elements
	if (link->prev)
		link->prev->next = link->next;
	if (link->next)
		link->next->prev = link->prev;

	// Was this the first element?
	if (link == list)
		list = list->next;

	// Free the link
	_list_free_link(link);

	return list;
}

/**
 *  Frees all of the memory used by a list.
 *
 *  @note	If list elements contain dynamically-allocated memory, you should either use
 *  		list_free_full() or free them manually first.
 *
 *  @param	list	a list
 *
 *  @returns		the new list (NULL)
 */
List * list_free(List *list)
{
	List *tmp;

	// Iterate through the list and free each element
	while (list) {
		tmp = list->next;

		_list_free_link(list);

		list = tmp;
	}

	return NULL;
}

/**
 *  Frees all the memory used by a list and calls the specified destroy function on
 *  every element's data.
 *
 *  @param list			a list
 *  @param free_func	the function to be called to free each element's data
 *
 *  @returns	the new list (NULL)
 */
List * list_free_full(List *list, void (*free_func)(void *data))
{
	List *tmp;

	// Iterate through the list and free each element after calling
	// the function which the user passed to free the element's
	// data.
	while (list) {
		tmp = list->next;

		if (list->data)
			free_func(list->data);

		_list_free_link(list);

		list = tmp;
	}

	return NULL;
}

/** @} */
