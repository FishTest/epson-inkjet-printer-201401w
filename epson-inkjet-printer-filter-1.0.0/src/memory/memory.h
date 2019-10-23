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

#include <stdlib.h>
#include "debuglog.h"

#ifndef __EPS_MEMORY_H__
#define __EPS_MEMORY_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef DEBUG
#define YES_DUMP_HEAP_USAGE
#endif

#ifdef YES_DUMP_HEAP_USAGE
void eps_heap_usage_start(void);
void eps_heap_usage_end(void);
void * eps_malloc_trace(char * filename, int line, size_t size);
void eps_free_trace(char * filename, int line, void * ptr);
#define DUMP_HEAP_INIT()	eps_heap_usage_start()
#define DUMP_HEAP_USAGE()	eps_heap_usage_end()
#define eps_malloc(size)	eps_malloc_trace(__FILE__, __LINE__, size)
#define eps_free(ptr)		eps_free_trace(__FILE__, __LINE__, ptr)
#else
#define DUMP_HEAP_INIT()
#define DUMP_HEAP_USAGE()
void * eps_malloc(size_t size);
void eps_free(void * ptr);
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPS_MEMORY_H__ */
