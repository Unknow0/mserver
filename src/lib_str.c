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
#include <utils/chunked_list.h>
#include <utils/chunked_string.h>

#include "lib.h"

void lib_str_init(lib_t *lib)
	{
	lib->lib_str=chunked_string_create(1024);
	lib_str_reset(lib);
	}

void lib_str_deinit(lib_t *lib)
	{
	chunked_string_destroy(lib->lib_str);
	}

void append(char *str)
	{
	char b[2]={0, 0};
	while(*str!=0)
		{
		b[0]=*str;
		switch(*str)
			{
			case '\\':
			case '"':
				chunked_string_add(lib->lib_str, "\\");
				chunked_string_add(lib->lib_str, b);
				break;
			case '\n':
				chunked_string_add(lib->lib_str, "\\n");
				break;
			case '\r':
				chunked_string_add(lib->lib_str, "\\r");
				break;
			case '\t':
				chunked_string_add(lib->lib_str, "\\t");
				break;
			default:
				chunked_string_add(lib->lib_str, b);
			}
		str++;
		}
	}

char buf[5];
extern logger_t *ll;
void lib_str_reset(lib_t *lib)
	{
	chunked_string_clear(lib->lib_str);
	chunked_string_add(lib->lib_str, "{");
	iterator_t *it=chunked_list_iterator(lib->entries);
	lib_entry *e=NULL;
	while(iterator_has_next(it))
		{
		if(e!=NULL)
			chunked_string_add(lib->lib_str, ",");

		e=iterator_next(it);
		
		chunked_string_add(lib->lib_str, "\"");
		append(e->path);
		chunked_string_add(lib->lib_str, "\":{\"title\":\"");
		append(e->name);
		chunked_string_add(lib->lib_str, "\",\"group\":\"");
		append(e->group->str);
		chunked_string_add(lib->lib_str, "\",\"album\":\"");
		append(e->album->str);
		chunked_string_add(lib->lib_str, "\",\"track\":");
		snprintf(buf, 5, "%d", e->track);
		chunked_string_add(lib->lib_str, buf);
		chunked_string_add(lib->lib_str, "}");
		}
	chunked_string_add(lib->lib_str, "}");
	iterator_dispose(it);
	}

void lib_str_print(lib_t *lib)
	{
	size_t i;
	struct str_chunk *c=lib->lib_str->head;
	while(c!=NULL)
		{
		for(i=0; i<c->len; i++)
			printf("%c", c->data[i]);
		c=c->next;
		}
	printf("\n");
	}
