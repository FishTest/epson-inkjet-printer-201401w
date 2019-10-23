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
#include "reverse.h"

typedef struct EpsReverse {
	EpsReverseOpt * init_data;
	char ** rasters;
	int current;
	int flushed;
} EpsReverse;


///////////////////////////////////////////////////////////////////////////////
//
// * A P I for reverse (extern functions)
//
///////////////////////////////////////////////////////////////////////////////
int
eps_init_reverse (RASTERPIPE * reverse_p, PIPEOPT init_p)
{
	EpsReverse * p = NULL;
	char **rasters =  NULL;
	int eps_error = 0;
	int i = 0;

	if (init_p) {
		p = (EpsReverse *) eps_malloc(sizeof(EpsReverse));
		if (p) {
			p->init_data = (EpsReverseOpt *) init_p;
			p->rasters = (char **) eps_malloc(sizeof(char *) * p->init_data->num_raster);
			if (p->rasters) {
				rasters =  p->rasters;
				for (i = 0; i < p->init_data->num_raster; i++) {
					rasters[i] = NULL;
				}

				for (i = 0; i < p->init_data->num_raster; i++) {
					rasters[i] = (char *) eps_malloc(p->init_data->bytes_per_raster);
					memset(rasters[i], 0xFF, p->init_data->bytes_per_raster);
				}
			}

			p->current = p->init_data->num_raster - 1; /* last */
			p->flushed = 0;

			debuglog(("current pos. %d", p->current));

			*reverse_p = (REVERSE) p;
		} else {
			eps_error = 1;
		}
	} else {
		eps_error = 1;
	}

	return eps_error;
}

int
eps_process_reverse (RASTERPIPE reverse, char* raster_p, int raster_bytes, int pixel_num, int * outraster)
{
	EpsReverse * lp_reverse = NULL;
	EpsReverseOpt * lp_data = NULL;
	char *blank_raster_buf = NULL;
	int flush_raster = 0;
	int nbytes = 0;
	int npixels = 0;
	int error = 0;
	int nraster = 0;
	int margin = 0;
	int i = 0;

	do {
		*outraster = 0;
		error = 1;

		lp_reverse = (EpsReverse *) reverse;
		if (lp_reverse == NULL) {
			break;
		}

		lp_data = (EpsReverseOpt *) lp_reverse->init_data;
		if (lp_data == NULL) {
			break;
		}

		if (raster_p) { // reverse copying
			if (lp_reverse->current >= 0) {
				nbytes = (raster_bytes >= lp_data->bytes_per_raster) ? lp_data->bytes_per_raster : raster_bytes;
#ifdef DEBUG_VERBOSE
				debuglog(("reverse copying : (current=%d)", lp_reverse->current));
#endif
				memcpy(lp_reverse->rasters[lp_reverse->current], raster_p, nbytes);
			}
			lp_reverse->current--;
		} else { // printing (flushing)
			if (lp_reverse->flushed == 0) {
				lp_reverse->flushed = 1;
				nbytes = lp_data->bytes_per_raster;
				npixels = nbytes / lp_data->bytes_per_pixel;
				margin = (lp_reverse->current > 0) ? lp_reverse->current : 0;
				
				flush_raster = lp_data->num_raster;
				debuglog(("reverse printing start : (current=%d) %d rasters", lp_reverse->current, flush_raster));

				for (i = margin; i < flush_raster; i++) {
					error = lp_data->pipe->output(lp_data->pipe->output_h, lp_reverse->rasters[i], nbytes, npixels, &nraster);
					if (error == 0) {
						*outraster += nraster;
					} else {
						break;
					}
				}
				// signal flushing
				error = lp_data->pipe->output(lp_data->pipe->output_h, NULL, 0, 0, &nraster);
			}
		}

		error = 0;

	} while (0);

	return error;
}

int
eps_free_reverse (RASTERPIPE reverse)
{
	EpsReverse * lp_reverse = (EpsReverse *) reverse;
	char ** rasters =  NULL;

	if (lp_reverse) {
		if (lp_reverse->init_data) {
			rasters =  lp_reverse->rasters;
			if (rasters) {
				int i;
				for (i = 0; i < lp_reverse->init_data->num_raster; i++) {
					if (rasters[i]) {
						eps_free(rasters[i]);
					}
				}
				eps_free(lp_reverse->rasters);
			}
			eps_free(lp_reverse->init_data);
		}
		eps_free(lp_reverse);
	}

	return 0;
}

