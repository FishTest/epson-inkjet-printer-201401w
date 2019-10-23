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
#include "blend-source.h"

/* implementation (blend-watermark.c) */
extern int blend_watermark_initialize_instance(EpsBlendSource *instance);

EpsPoint epsMakePoint(int x, int y)
{
	EpsPoint point;
	point.x = x;
	point.y = y;
	return point;
}

EpsSize epsMakeSize(int width, int height)
{
	EpsSize size;
	size.width = width;
	size.height = height;
	return size;
}

EpsRect epsMakeRect(int x, int y, int width, int height)
{
	EpsRect rect;
	rect.origin = epsMakePoint(x, y);
	rect.size = epsMakeSize(width, height);
	return rect;
}

/* return 1 : yes, 0 : no */
int is_current_raster_in_blending_bounds(int raster_index, EpsRect blending_bounds)
{
	int start = blending_bounds.origin.y;
	int end = start + blending_bounds.size.height;
	return (raster_index >= start && raster_index <= end);
}

EpsBlendSource* blend_source_create_blender(EpsBlendSourceType sourceType)
{
	EpsBlendSource *blender = (EpsBlendSource *) eps_malloc(sizeof(EpsBlendSource));
	if (blender) {
		if (sourceType == EPS_BLEND_SOURCE_TYPE_WATERMARK) {
			blend_watermark_initialize_instance(blender);
		}
	}

	return blender;
}

void blend_source_destroy_blender(EpsBlendSource* blender)
{
	if (blender) {
		blender->finalize(blender->privateData);
		eps_free(blender);
	}
}
