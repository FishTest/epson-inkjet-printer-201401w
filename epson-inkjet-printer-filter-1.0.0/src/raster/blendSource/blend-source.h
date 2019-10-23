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

#include "blend.h"

#ifndef __EPS_BLEND_SOURCE_H__
#define __EPS_BLEND_SOURCE_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct EpsPoint {
	int x;
	int y;
} EpsPoint;

typedef struct EpsSize {
	int width;
	int height;
} EpsSize;

typedef struct EpsRect {
	EpsPoint origin;
	EpsSize size;
} EpsRect;

typedef struct EpsColor {
	float red;
	float green;
	float blue;
	float alpha;
} EpsColor;

#define EPS_BLEND_SOURCE_OK	0
#define EPS_BLEND_SOURCE_ERROR	1

typedef int (*BlendSourceOpen)(void *privateData, const char* sourcePath, EpsSize size, EpsColor color);
typedef int (*BlendSourceBlendingPixels)(void *privateData, unsigned char* pixelBuf, int bytesPixelBuf, int pixelCount);
typedef int (*BlendSourceClose)(void *privateData);
typedef void (*BlendSourcePrivateFinalize)(void *privateData);

typedef struct EpsBlendSource {
	void *privateData;
	BlendSourceOpen open;
	BlendSourceBlendingPixels blendingPixels;
	BlendSourceClose close;
	BlendSourcePrivateFinalize finalize;
} EpsBlendSource;

typedef enum EpsSourceType {
	EPS_BLEND_SOURCE_TYPE_WATERMARK,
} EpsBlendSourceType;

EpsPoint epsMakePoint(int x, int y);
EpsSize epsMakeSize(int width, int height);
EpsRect epsMakeRect(int x, int y, int width, int height);
int is_current_raster_in_blending_bounds(int raster_index, EpsRect blending_bounds);

EpsBlendSource* blend_source_create_blender(EpsBlendSourceType source_type);
void blend_source_destroy_blender(EpsBlendSource* blender);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPS_BLEND_SOURCE_H__ */
