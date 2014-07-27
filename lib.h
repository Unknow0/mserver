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

#include <container/chunked_list.h>
#include <container/chunked_string.h>
#include <container/string.h>

typedef struct
	{
	char *path;
	char *name;
	string_t *group;
	string_t *album;
	unsigned int track;
	} lib_entry;

int lib_init(char *libfile, char *libdir);
void lib_deinit();

void lib_str_init();
void lib_str_reset();
void lib_str_print();

extern chunked_list_t *lib;
extern size_t lib_count;
extern char *lib_path;
extern chunked_string_t *lib_str;

#endif
