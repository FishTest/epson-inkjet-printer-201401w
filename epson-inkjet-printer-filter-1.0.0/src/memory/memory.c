/*
   Copyright (C) Seiko Epson Corporation 2009.
 
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, write to the Free  Software Foundation, Inc., 
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "debuglog.h"

#ifdef YES_DUMP_HEAP_USAGE
static int maxusage = 0;
static int curusage = 0;

void
eps_heap_usage_start(void)
{
	maxusage = 0;
	curusage = 0;
}

void
eps_heap_usage_end(void)
{
	debuglog(("============================================================="));
	debuglog(("HEAP USAGE : max(%d bytes) leak(%d bytes)", maxusage, curusage));
	debuglog(("=============================================================")); 
}

void *
eps_malloc_trace(char * filename, int line, size_t size)
{
	void * p = calloc(size + sizeof(unsigned int), 1);
	if(p) {
		unsigned int * pi = (unsigned int *) p;
		*pi++ = size;
		p = (void *) pi;

		curusage += size;
		if(curusage > maxusage) {
			maxusage = curusage;
		}

#ifdef DEBUG_VERBOSE
		debuglog(("MEMALLOC (%s:%d) address %#x, size %d, max %d, leak %d", filename, line, p, size, maxusage, curusage));
#endif
	} else {
		debuglog(("MEMALLOC FAILED %d bytes !", size));
	}
	return p;
}

void
eps_free_trace(char * filename, int line, void * ptr)
{
	if(ptr) {
		unsigned int * pi = (unsigned int *) ptr;
		pi--;
		curusage -= *pi;

#ifdef DEBUG_VERBOSE
		debuglog(("MEMFREE (%s:%d): address %#x, size %d, leak %d", filename, line, ptr, *pi, curusage));
#endif

		free(pi);
	}
}
#else
void *
eps_malloc(size_t size)
{
	return calloc(size, 1);
}

void
eps_free(void * ptr)
{
	if (ptr) {
		free(ptr);
	}
}
#endif

