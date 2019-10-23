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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "mirror.h"

typedef struct EpsMirror {
	EpsMirrorOpt * init_data;
} EpsMirror;


///////////////////////////////////////////////////////////////////////////////
//
// * A P I for mirror (extern functions)
//
///////////////////////////////////////////////////////////////////////////////
int
eps_init_mirror (RASTERPIPE * mirror_p, PIPEOPT init_p)
{
	int eps_error = 0;
	EpsMirror * p;

	p = (EpsMirror *) eps_malloc(sizeof(EpsMirror));
	if (p && init_p) {
		p->init_data = (EpsMirrorOpt *) init_p;

		debuglog(("bytes per pixel : %d", p->init_data->bytes_per_pixel));

		*mirror_p = (MIRROR) p;
	} else {
		eps_error = 1;
	}

	return eps_error;
}

int
eps_process_mirror (RASTERPIPE mirror, char* raster_p, int raster_bytes, int pixel_num, int * outraster)
{
	EpsMirror * lp_mirror = (EpsMirror *) mirror;
	EpsMirrorOpt * lp_data = (EpsMirrorOpt *) lp_mirror->init_data;
	int error = 0;
	int nraster = 0;

	*outraster = 0;
	if (raster_p) {
		int bpp = lp_data->bytes_per_pixel;
		char * p = (char *)eps_malloc(raster_bytes);
		if (p) {
			memset(p, 0xFF, raster_bytes);
			char * left = raster_p;
			char * right = p + ((pixel_num - 1) *  bpp);

			int i;
			for (i = 0; i < pixel_num; i++) {
				memcpy(right, left, bpp);
				left  += bpp;
				right -= bpp;
			}

			error = lp_data->pipe->output(lp_data->pipe->output_h, p, raster_bytes, pixel_num, &nraster);
			if (error == 0) {
				*outraster = 1;
			}
			eps_free(p);
		} else {
			debuglog(("MIRROR MEMALLOC ERROR %d bytes", raster_bytes));
			error = 1;
		}
	} else {
		debuglog(("MIRROR FLUSHING HERE ..."));
		lp_data->pipe->output(lp_data->pipe->output_h, NULL, 0, 0, &nraster);
	}

	return error;
}

int
eps_free_mirror (RASTERPIPE mirror)
{
	EpsMirror * lp_mirror = (EpsMirror *) mirror;

	if (lp_mirror) {
		if (lp_mirror->init_data) {
			eps_free(lp_mirror->init_data);
		}
		eps_free(lp_mirror);
	}

	return 0;
}

