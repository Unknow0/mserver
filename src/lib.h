/*******************************************************************************
 * This file is part of mserver.
 *
 * mserver is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * mserver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with mserver; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 ******************************************************************************/
#ifndef _LIB_H
#define _LIB_H

#include <utils/chunked_list.h>
#include <utils/chunked_string.h>
#include <utils/string.h>
#include <utils/watch.h>
#include <utils/bus.h>

typedef struct
	{
	char *path;
	char *name;
	string_t *group;
	string_t *album;
	unsigned int track;
	} lib_entry;

typedef struct library
	{
	const char *dbfile;
	const char *base_path;
	size_t base_path_size;

	chunked_list_t *entries;

	chunked_string_t *lib_str;
	pthread_t check_thread;

	watch_t *watch;

	bus_t *bus;
	} lib_t;

extern lib_t *lib;

lib_t *lib_create(const char *libfile, const char *libdir);
void lib_destroy(lib_t *);

char *lib_canonize(lib_t *, const char *);

void lib_str_init(lib_t *);
void lib_str_reset(lib_t *);
void lib_str_print(lib_t *);
void lib_str_deinit(lib_t *);

#endif
