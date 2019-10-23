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
#include "blend.h"

typedef struct EpsBlend {
	EpsBlendOpt *init_data;
	EpsBlendSource *blender;
	int raster_index;
} EpsBlend;

///////////////////////////////////////////////////////////////////////////////
//
// * A P I for blend (extern functions)
//
///////////////////////////////////////////////////////////////////////////////
int
eps_init_blend (RASTERPIPE * blend_p, PIPEOPT init_p)
{
	EpsBlend *blend = NULL;
	EpsBlendOpt *blendOpt = NULL;
	EpsBlendSource *blender = NULL;
	int eps_error = 1;

	do {
		if (init_p == NULL) {
			break;
		}

		blend = (EpsBlend *) eps_malloc(sizeof(EpsBlend));
		if (blend == NULL) {
			break;
		}

		blendOpt = (EpsBlendOpt *) init_p;
		blender = blend_source_create_blender(blendOpt->source_type);
		if (blender == NULL) {
			break;
		}

		if (blender->open(blender->privateData, blendOpt->source_path, blendOpt->bounds.size, blendOpt->color) == EPS_BLEND_SOURCE_ERROR) {
			break;
		}

		blend->raster_index = 0;
		blend->init_data = blendOpt;
		blend->blender = blender;

		*blend_p = (BLEND) blend;

		eps_error = 0;

	} while (0);

	return eps_error;
}

int
eps_process_blend (RASTERPIPE blend, char* raster_p, int raster_bytes, int pixel_num, int * outraster)
{
	EpsBlend * lp_blend = (EpsBlend *) blend;
	EpsBlendOpt * lp_data = NULL;
	char * blend_p = NULL;
	int blend_bytes;
	int blend_pixels;
	int bytes_per_pixel;
	int error = 0;
	int nraster = 0;

	*outraster = 0;

	if (lp_blend && (lp_data = (EpsBlendOpt *) lp_blend->init_data))  {
		if (raster_p) {
			if (is_current_raster_in_blending_bounds(lp_blend->raster_index, lp_data->bounds)) {
				bytes_per_pixel = raster_bytes / pixel_num;
				blend_p = raster_p + (lp_data->bounds.origin.x * bytes_per_pixel);
				blend_pixels = lp_data->bounds.size.width;
				blend_bytes = bytes_per_pixel *  blend_pixels;

				lp_blend->blender->blendingPixels(lp_blend->blender->privateData, blend_p, blend_bytes, blend_pixels);
			}

			error = lp_data->pipe->output(lp_data->pipe->output_h, raster_p, raster_bytes, pixel_num, &nraster);
			if (error == 0) {
				*outraster = 1;
			}
			lp_blend->raster_index++;
		} else {
			debuglog(("BLEND FLUSHING HERE ..."));
			lp_data->pipe->output(lp_data->pipe->output_h, NULL, 0, 0, &nraster);
		}
	}

	return error;
}

int
eps_free_blend (RASTERPIPE blend)
{
	EpsBlend * lp_blend = (EpsBlend *) blend;
	if (lp_blend) {
		if (lp_blend->init_data) {
			eps_free(lp_blend->init_data);
		}

		if (lp_blend->blender) {
			lp_blend->blender->close(lp_blend->blender->privateData);
			blend_source_destroy_blender(lp_blend->blender);
		}

		eps_free(lp_blend);
	}

	return 0;
}

