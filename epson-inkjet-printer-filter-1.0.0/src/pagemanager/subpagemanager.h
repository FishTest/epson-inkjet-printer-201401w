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
#ifndef __EPS_SUBPAGE_MANAGER_H__

#define __EPS_SUBPAGE_MANAGER_H__

#include <stdio.h>
#include "filter_option_define.h"
#include "subpage.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct {
	int width;
	int height;
	int bytesPerLine;
	int bitsPerPixel;
} EpsPageRegion;

typedef int (*EpsSetRaster)(EpsSubPage *subPage, char *raster, int bufSize);

typedef struct {
	EpsSubPage	*subPage[4];
	int		vertical_num;
	EpsPageLayout	pageLayout;
	EpsSetRaster	subPageSetRaster;
} EpsSubPageManager;

EpsSubPageManager* subPageManagerCreate(EpsPageRegion *pageRegion, EpsPageLayout pageLayout);
void subPageManagerDestroy(EpsSubPageManager *subPageManager);
int subPageManagerGetRaster(EpsSubPageManager *subPageManager, char *buf, int bufSize);
int subPageManagerSetRaster(EpsSubPageManager *subPageManager, char *buf, int bufSize);
int subPageManagerFlushRaster(EpsSubPageManager *subPageManager);
int subPageManagerIsNextPage(EpsSubPageManager *subPageManager);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __EPS_SUBPAGE_MANAGER_H__ */

