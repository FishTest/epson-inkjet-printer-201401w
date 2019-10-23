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

#include "debuglog.h"
#include "memory.h"

#ifndef __EPS_RASTER_H__
#define __EPS_RASTER_H__ 
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef void * HANDLE;
typedef void * RASTER;
typedef void * RASTERPIPE;
typedef void * PIPEOBJ;
typedef void * PIPEOPT;
typedef void * PIPEOUT_HANDLE;
typedef int (*RASTEROUT_FUNC) (HANDLE, char*, int, int);

typedef enum {
	EPS_PAGE_WATERMARK_POSITION_CENTER,
	EPS_PAGE_WATERMARK_POSITION_TOPLEFT,
	EPS_PAGE_WATERMARK_POSITION_TOP,
	EPS_PAGE_WATERMARK_POSITION_TOPRIGHT,
	EPS_PAGE_WATERMARK_POSITION_LEFT,
	EPS_PAGE_WATERMARK_POSITION_RIGHT,
	EPS_PAGE_WATERMARK_POSITION_BOTTOMLEFT,
	EPS_PAGE_WATERMARK_POSITION_BOTTOM,
	EPS_PAGE_WATERMARK_POSITION_BOTTOMRIGHT,
} EpsPageWatermarkPosition;

typedef enum {
	EPS_PAGE_WATERMARK_DENSITY_LEVEL1, /* Light */
	EPS_PAGE_WATERMARK_DENSITY_LEVEL2,
	EPS_PAGE_WATERMARK_DENSITY_LEVEL3,
	EPS_PAGE_WATERMARK_DENSITY_LEVEL4,
	EPS_PAGE_WATERMARK_DENSITY_LEVEL5, 
	EPS_PAGE_WATERMARK_DENSITY_LEVEL6, /* Dark */
} EpsPageWatermarkDensity;

typedef enum {
	EPS_PAGE_WATERMARK_COLOR_BLACK,
	EPS_PAGE_WATERMARK_COLOR_BLUE,
	EPS_PAGE_WATERMARK_COLOR_LIME,
	EPS_PAGE_WATERMARK_COLOR_AQUA,
	EPS_PAGE_WATERMARK_COLOR_RED,
	EPS_PAGE_WATERMARK_COLOR_FUCHSIA,
	EPS_PAGE_WATERMARK_COLOR_YELLOW,
} EpsPageWatermarkColor;

typedef enum {
	EPS_PAGE_WATERMARK_SIZE_10 = 1,
	EPS_PAGE_WATERMARK_SIZE_20,
	EPS_PAGE_WATERMARK_SIZE_30,
	EPS_PAGE_WATERMARK_SIZE_40,
	EPS_PAGE_WATERMARK_SIZE_50,
	EPS_PAGE_WATERMARK_SIZE_60,
	EPS_PAGE_WATERMARK_SIZE_70,
	EPS_PAGE_WATERMARK_SIZE_80,
	EPS_PAGE_WATERMARK_SIZE_90,
	EPS_PAGE_WATERMARK_SIZE_100
} EpsPageWatermarkSize;

typedef struct EpsPageWatermarkOption {
	int use;
	char *filepath;
	float size_ratio; 
	EpsPageWatermarkPosition position;
	EpsPageWatermarkDensity density;
	EpsPageWatermarkColor color;
} EpsPageWatermarkOption;

typedef struct EpsPageInfo {
	int bytes_per_pixel;
	int src_print_area_x;
	int src_print_area_y;
	int prt_print_area_x;
	int prt_print_area_y;
	int scale;
	int mirror;
	int reverse;
	EpsPageWatermarkOption watermark;
} EpsPageInfo;

typedef struct EpsRasterInit {
	HANDLE drv_handle;
	RASTEROUT_FUNC raster_output;
} EpsRasterOpt;

typedef int (*PIPEOUT_FUNC) (PIPEOUT_HANDLE, char *, int, int, int *);

typedef struct EpsRasterPipe {
	RASTERPIPE self;	
	PIPEOPT opt;
	PIPEOBJ obj;
	PIPEOUT_FUNC output;
	PIPEOUT_HANDLE output_h;
	int (* pipe_init) (RASTERPIPE *, PIPEOPT);
	int (* pipe_process) (RASTERPIPE, char *, int, int, int *);
	int (* pipe_free) (RASTERPIPE);
} EpsRasterPipe;

typedef enum {
	EPS_RASTER_PROCESS_MODE_PRINTING,
	EPS_RASTER_PROCESS_MODE_FETCHING,
} EpsRasterProcessMode;

typedef struct EpsRasterPipeline {
	EpsRasterProcessMode process_mode;
	EpsPageInfo page;
	EpsRasterPipe ** pipeline;
	int numpipe;
	union {
		int duplecate;
	} mode;
} EpsRasterPipeline;

typedef enum {
	EPS_RASTER_FETCH_STATUS_HAS_RASTER,
	EPS_RASTER_FETCH_STATUS_NEED_RASTER,
	EPS_RASTER_FETCH_STATUS_COMPLETED,
	EPS_RASTER_FETCH_STATUS_ERROR,
} EpsRasterFetchStatus;

int eps_raster_init (RASTER *, EpsRasterOpt *, EpsRasterPipeline *);
int eps_raster_print (RASTER, char *, int, int, int *);
int eps_raster_fetch (RASTER, char *, int, int, EpsRasterFetchStatus *);
int eps_raster_free (RASTER);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPS_RASTER_H__ */
