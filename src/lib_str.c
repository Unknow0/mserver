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
#include <stdio.h>
#include <logger.h>
#include <container/chunked_list.h>
#include <container/chunked_string.h>

#include "lib.h"

chunked_string_t *lib_str;

void lib_str_init()
	{
	lib_str=chunked_string_create(1024);
	lib_str_reset();
	lib_str_print();
	}


char buf[5];
extern logger_t *ll;
void lib_str_reset()
	{
	chunked_string_clear(lib_str);
	chunked_string_add(lib_str, "{");
	iterator_t *it=chunked_list_iterator(lib);
	lib_entry *e=NULL;
	while(iterator_has_next(it))
		{
		if(e!=NULL)
			chunked_string_add(lib_str, ",");

		e=iterator_next(it);
		
		chunked_string_add(lib_str, "\"");
		chunked_string_add(lib_str, e->path);
		chunked_string_add(lib_str, "\":{\"title\":\"");
		chunked_string_add(lib_str, e->name);
		chunked_string_add(lib_str, "\",\"group\":\"");
		chunked_string_add(lib_str, e->group->str);
		chunked_string_add(lib_str, "\",\"album\":\"");
		chunked_string_add(lib_str, e->album->str);
		chunked_string_add(lib_str, "\",\"track\":");
		snprintf(buf, 5, "%d", e->track);
		chunked_string_add(lib_str, buf);
		chunked_string_add(lib_str, "}");
		}
	chunked_string_add(lib_str, "}");
	}

void lib_str_print()
	{
	struct str_chunk *c=lib_str->head;
	while(c!=NULL)
		{
		printf("%.1024s", c->data);
		c=c->next;
		}
	}
