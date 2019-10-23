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
#ifndef __EPS_PAGE_MANAGER_H__

#define __EPS_PAGE_MANAGER_H__

#include <stdio.h>
#include "filter_option.h"
#include "subpagemanager.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define	EPS_ERROR	-1
#define	EPS_OK		0

typedef int (*EpsRasterSource)(char *buf, int bufSize);

typedef struct {
	EpsRasterSource		rasterSource;
	EpsPageRegion		pageRegion;
	EpsPageLayout		pageLayout;
	EpsSubPageManager	*subPageManager;
	int			cupsHeight;
	int			cupsBytesPerLine;
	int			currentLine;
	void *		privateData;
} EpsPageManager;

EpsPageManager* pageManagerCreate(EpsPageRegion pageRegion, EpsFilterPrintOption filterPrintOption, EpsRasterSource rasterSource);
void pageManagerDestroy(EpsPageManager *pageManager);
int pageManagerGetPageRegion(EpsPageManager *pageManager, EpsPageRegion *pageRegion);
int pageManagerGetRaster(EpsPageManager *pageManager, char *buf, int bufSize);
int pageManagerIsNextPage(EpsPageManager *pageManager);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __EPS_PAGE_MANAGER_H__ */

