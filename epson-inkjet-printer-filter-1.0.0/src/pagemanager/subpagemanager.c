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

#include "epcgdef.h"
#include "debuglog.h"
#include "memory.h"
#include "subpagemanager.h"

EpsSubPageManager* subPageManagerCreate(EpsPageRegion *pageRegion, EpsPageLayout pageLayout)
{
	int i;
	int width, height;
	int bytesPerPixel;
	EpsSubPageManager *subPageManager;
	
	subPageManager = (EpsSubPageManager *)eps_malloc(sizeof(EpsSubPageManager));
	if (subPageManager == NULL) {
		return NULL;
	}
	debuglog(("subPageManager Created."));

	subPageManager->pageLayout = pageLayout;
	bytesPerPixel = pageRegion->bitsPerPixel / 8;
	debuglog(("Before: width = %d, height = %d, bytesPerLine = %d", pageRegion->width, pageRegion->height, pageRegion->bytesPerLine));
	switch (subPageManager->pageLayout) {
		case EPS_PAGE_LAYOUT_1x1:
			subPageManager->vertical_num = 1;
			subPageManager->subPageSetRaster = subPageSetRasterRotate0;
			break;
				
		case EPS_PAGE_LAYOUT_2x1:
			subPageManager->vertical_num = 1;
			width = pageRegion->width;
			pageRegion->width =(pageRegion->height + 1) / 2;
			pageRegion->height = width;
			pageRegion->bytesPerLine = (pageRegion->width * bytesPerPixel + 3) / 4 * 4;
			subPageManager->subPageSetRaster = subPageSetRasterRotate90;
			break;

		case EPS_PAGE_LAYOUT_2x2:
			subPageManager->vertical_num = 2;
			pageRegion->width = pageRegion->width / 2;
			pageRegion->height = (pageRegion->height + 1) / 2;
			pageRegion->bytesPerLine = pageRegion->width * pageRegion->bitsPerPixel / 8;
			subPageManager->subPageSetRaster = subPageSetRasterRotate0;
			break;

		case EPS_PAGE_LAYOUT_3x3:
			subPageManager->vertical_num = 3;
			pageRegion->width = pageRegion->width / 3;
			pageRegion->height = (pageRegion->height + 2) / 3;
			pageRegion->bytesPerLine = pageRegion->width * pageRegion->bitsPerPixel / 8;
			subPageManager->subPageSetRaster = subPageSetRasterRotate0;
			break;

		case EPS_PAGE_LAYOUT_4x4:
			subPageManager->vertical_num = 4;
			pageRegion->width = pageRegion->width / 4;
			pageRegion->height = (pageRegion->height + 3) / 4;
			pageRegion->bytesPerLine = pageRegion->width * pageRegion->bitsPerPixel / 8;
			subPageManager->subPageSetRaster = subPageSetRasterRotate0;
			break;

		default:
			if (subPageManager != NULL) {
				eps_free(subPageManager);
			}
			return NULL;
	}
	debuglog(("After: width = %d, height = %d, bytesPerLine = %d", pageRegion->width, pageRegion->height, pageRegion->bytesPerLine));
	
	for (i = 0; i < subPageManager->vertical_num; i++) {
		if (subPageManager->pageLayout != EPS_PAGE_LAYOUT_2x1 && i==0) {
			height = 1;
		} else {
			height = pageRegion->height;
		}

		subPageManager->subPage[i] = subPageCreate(pageRegion->bytesPerLine * i, pageRegion->bytesPerLine, height, bytesPerPixel);

		if (subPageManager->subPage[i] == NULL) {
			if (subPageManager != NULL) {
				eps_free(subPageManager);
				return NULL;
			}
		}
	}
	
	return subPageManager;	
}

void subPageManagerDestroy(EpsSubPageManager *subPageManager)
{
	int i;
	
	if (subPageManager == NULL) {
		return;
	}
	
	for (i = 0; i < subPageManager->vertical_num; i++) {
		if (subPageManager->subPage[i] != NULL) {
			subPageDestroy(subPageManager->subPage[i]);
		}
	}
	
	eps_free(subPageManager);
	subPageManager = NULL;
	debuglog(("subPageManager Destroyed."));
}

int subPageManagerGetRaster(EpsSubPageManager *subPageManager, char *buf, int bufSize)
{
	int i;

	if (subPageManager == NULL) {
		return EPS_ERROR;
	}
		
	for (i = 0; i < subPageManager->vertical_num; i++) {
		if (subPageManager->subPage[i] != NULL) {
			if (subPageIsNextLine(subPageManager->subPage[i]) == TRUE) {
				return subPageGetRaster(subPageManager->subPage[i], buf, bufSize);
			}
		}
	}
	
	return EPS_ERROR;
}

int subPageManagerSetRaster(EpsSubPageManager *subPageManager, char *buf, int bufSize)
{
	int i;
	
	if (subPageManager == NULL) {
		return EPS_ERROR;
	}

	for (i = 0; i < subPageManager->vertical_num; i++) {
		if (subPageManager->subPage[i] != NULL) {
			subPageManager->subPageSetRaster(subPageManager->subPage[i], buf, bufSize);
		}
	}
	
	return EPS_OK;
}

int subPageManagerFlushRaster(EpsSubPageManager *subPageManager)
{
	int i;
	int ret;

	if (subPageManager == NULL) {
		return EPS_ERROR;
	}
	
	ret = EPS_OK;
	for (i = 0; i < subPageManager->vertical_num; i++) {
		ret = subPageFlushRaster(subPageManager->subPage[i]);
	}
	
	return ret;
}

int subPageManagerIsNextPage(EpsSubPageManager *subPageManager)
{
	int i;
	
	if (subPageManager == NULL) {
		return EPS_ERROR;
	}
		
	for (i = 0; i < subPageManager->vertical_num; i++) {
		if (subPageManager->subPage[i] != NULL) {
			if (subPageIsNextLine(subPageManager->subPage[i]) == TRUE) {
				return TRUE;
			}
		}
	}
	
	return FALSE;
}



