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

#include "raster.h"

#ifndef __EPS_SCALE_H__
#define __EPS_SCALE_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef void * SCALE;

typedef struct EpsScaleOpt {
	EpsRasterPipe * pipe;
	int do_scaling;
	int bytes_per_pixel;
	int src_print_area_x;
	int src_print_area_y;
	int prt_print_area_x;
	int prt_print_area_y;
} EpsScaleOpt;

int eps_init_scale (RASTERPIPE *, PIPEOPT);
int eps_process_scale (RASTERPIPE, char *, int, int, int *);
int eps_free_scale (RASTERPIPE);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPS_SCALER_H__ */
