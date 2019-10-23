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
#  include <config.h>
#endif

#include <string.h>
#include "fetch-pool.h"

typedef struct EpsFetchDataList {
	int did_fetch;
	int id;
	EpsFetchData * data;
	struct EpsFetchDataList * next;
} EpsFetchDataList;

typedef struct EpsFetchDataPool {
	int required_count;
	int retained_count;
	int fetched_count;
	int serial_number;
	EpsFetchDataList * list;
} EpsFetchDataPool;

static EpsFetchData *
retain_fetch_data(const EpsFetchData * fetch_p)
{
	EpsFetchData *data_p = (EpsFetchData *) eps_malloc(sizeof(EpsFetchData));
	if (data_p) {
		if (fetch_p->duplicate) {
			data_p->duplicate = 1;
			data_p->raster_p = (char *) eps_malloc(fetch_p->raster_bytes);
			if (data_p->raster_p) {
				memcpy(data_p->raster_p, fetch_p->raster_p, fetch_p->raster_bytes);
			} else {
				if (data_p) {
					eps_free(data_p);
					data_p = NULL;
				}
			}
		} else {
			data_p->duplicate = 0;
			data_p->raster_p = fetch_p->raster_p; /* just assigned */
		}

		data_p->raster_bytes = fetch_p->raster_bytes;
		data_p->pixel_num = fetch_p->pixel_num;
	}

	return data_p;
}

static void
release_fetch_data(EpsFetchData * data_p)
{
	if (data_p) {
		if (data_p->duplicate) {
			if (data_p->raster_p) {
				eps_free(data_p->raster_p);
				data_p->raster_p = NULL;
			}
		}
		eps_free(data_p);
		data_p = NULL;
	}
}

static EpsFetchDataList * /* return valid head node of list */
datalist_add(EpsFetchDataList *list, EpsFetchData *data, int id)
{
	EpsFetchDataList * node = list;
	EpsFetchDataList * head = list;
	EpsFetchDataList * prev = list;

	while (node) {
		if (node->did_fetch == 1) {
			node->did_fetch = 0;
			node->id = id;
			release_fetch_data(node->data);
			node->data = retain_fetch_data(data);
			break;
		} else {
			prev = node;
			node = node->next;
		}
	}

	if (node == NULL) {
		node = (EpsFetchDataList *) eps_malloc(sizeof(EpsFetchDataList));
		if (node) {
			node->did_fetch = 0;
			node->id = id;
			node->data = retain_fetch_data(data);
			node->next = NULL;
			if (prev) {
				prev->next = node;
			} else {
				head = node;
			}
		}
	}

	return head;
}

static EpsFetchData *
datalist_fetch(EpsFetchDataList *list, int id)
{
	EpsFetchDataList * node = list;
	EpsFetchData * data = NULL;

	while (node) {
		if (node->did_fetch == 0 && node->id == id) {
			node->did_fetch = 1;
			data = node->data;
			break;
		}

		node = node->next;
	}

	return data;
}

FETCHPOOL
fetchpool_create_instance(int data_count)
{
	EpsFetchDataPool *pool = (EpsFetchDataPool *) eps_malloc(sizeof(EpsFetchDataPool));
	if (pool) {
		pool->required_count = data_count;
		pool->retained_count = 0;
		pool->fetched_count = 0;
		pool->serial_number = 0;
		pool->list = NULL;
	}

	return (FETCHPOOL) pool;
}

void
fetchpool_destroy_instance(FETCHPOOL instance)
{
	EpsFetchDataPool *pool = (EpsFetchDataPool *) instance;
	EpsFetchDataList * node = NULL;
	if (pool) {
		while (pool->list) {
			node = pool->list;
			release_fetch_data(node->data);
			pool->list = node->next;
			eps_free(node);
		}

		eps_free(pool);
		pool = NULL;
	}
}

int
fetchpool_add_data(FETCHPOOL instance, EpsFetchData *data_p)
{
	EpsFetchDataPool *pool = (EpsFetchDataPool *) instance;
	EpsFetchDataList *list = NULL;
	int error = 1;

	do {
		if (pool == NULL) {
			break;
		}

		list = datalist_add(pool->list, data_p, pool->serial_number);
		if (list == NULL) {
			break;
		} 

		if (data_p && data_p->raster_p) {
			pool->serial_number++;	
			pool->retained_count++;
		}

		pool->list = list;

		error = 0;
		
	} while (0);

	return error;
}

EpsFetchData *
fetchpool_fetch_data(FETCHPOOL instance)
{
	EpsFetchDataPool *pool = (EpsFetchDataPool *) instance;
	EpsFetchData *data_p = NULL;
	int id;

	do {
		if (pool == NULL) {
			break;
		}

		data_p = datalist_fetch(pool->list, pool->fetched_count);
		if (data_p == NULL) {
			break;
		}
		
		pool->fetched_count++;
		pool->retained_count--;

	} while (0);

	return data_p;
}

void
fetchpool_get_status(FETCHPOOL instance, EpsRasterFetchStatus *status)
{
	EpsFetchDataPool *pool = (EpsFetchDataPool *) instance;
	char *message = NULL;

	do {
		if (pool == NULL) {
			message = "ERROR";
			*status = EPS_RASTER_FETCH_STATUS_ERROR;
			break;
		}

		if (pool->fetched_count == pool->required_count) {
			message = "COMPLETED";
			*status = EPS_RASTER_FETCH_STATUS_COMPLETED;
			break;
		}

		if (pool->retained_count == 0) {
			message = "NEED RASTER";
			*status = EPS_RASTER_FETCH_STATUS_NEED_RASTER;
			break;
		} else if (pool->retained_count > 0) {
			message = "HAS RASTER";
			*status = EPS_RASTER_FETCH_STATUS_HAS_RASTER;
			break;
		} else {
			message = "ERROR";
			*status = EPS_RASTER_FETCH_STATUS_ERROR;
			break;
		}

	} while (0);

#if DEBUG_VERBOSE
	if (pool) {
		debuglog(("%s : required(%d), retained(%d), fetched(%d), serial(%d)", message, pool->required_count, pool->retained_count, pool->fetched_count, pool->serial_number));
	}
#endif

}

