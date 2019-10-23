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

#include <string.h>
#include "epcgdef.h"
#include "debuglog.h"
#include "memory.h"
#include "subpage.h"

EpsSubPage* subPageCreate(int raster_start, int bytesPerLine, int height, int bytesPerPixel)
{
	EpsSubPage* subPage = NULL;
	
	subPage = (EpsSubPage *)eps_malloc(sizeof(EpsSubPage));
	if (subPage == NULL) {
		return NULL;
	}
	debuglog(("subPage Created."));
	
	subPage->raster_start	= raster_start;
	subPage->bytesPerLine	= bytesPerLine;
	subPage->height		= height;
	subPage->bufferedLine	= 0;
	subPage->fetchedLine	= 0;
	subPage->status		= EPS_SUBPAGE_STATUS_BUFFERING;
	subPage->bytesPerPixel	= bytesPerPixel;
	subPage->raster		= (char *)eps_malloc(bytesPerLine * height);
	if (subPage->raster == NULL) {
		return NULL;
	}
	
	return subPage;
}

int subPageDestroy(EpsSubPage *subPage)
{
	if (subPage == NULL) {
		return EPS_ERROR;
	}
	
	if (subPage->raster != NULL) {
		eps_free(subPage->raster);
	}
	
	eps_free(subPage);
	subPage = NULL;
	debuglog(("subPage Destroyed."));

	return EPS_OK;
}

int subPageGetRaster(EpsSubPage *subPage, char *buf, int bufSize)
{
	if (subPage == NULL) {
		return EPS_ERROR;
	}
	
	if (subPage->status != EPS_SUBPAGE_STATUS_BUFFERING_COMPLETE) {
		return EPS_ERROR;
	}

	if (subPage->fetchedLine < subPage->height) {
		memcpy(buf, subPage->raster + subPage->fetchedLine * subPage->bytesPerLine, bufSize);
		subPage->fetchedLine++;
#ifdef DEBUG_VERBOSE
		debuglog(("suBpageGetRaster. : %p", subPage));
#endif		
	} else {
		return EPS_ERROR;
	}
	
	if (subPage->fetchedLine >= subPage->height) {
		subPage->bufferedLine = 0;
		//Due to subpage is reused for rendering data, it should be cleaned by 0xFF
		//in order to avoid unexpected line may occur
		memset(subPage->raster, 0xff, subPage->bytesPerLine * subPage->height);
		subPage->status = EPS_SUBPAGE_STATUS_BUFFERING;
	}

	return EPS_OK;
}

int subPageFlushRaster(EpsSubPage *subPage)
{
	if (subPage == NULL) {
		return EPS_ERROR;
	}
	
	if (subPage->bufferedLine > 0) {
		subPage->fetchedLine = 0;
		subPage->status = EPS_SUBPAGE_STATUS_BUFFERING_COMPLETE;
		return EPS_OK;
	}

	return EPS_ERROR;
}

int subPageIsNextLine(EpsSubPage *subPage)
{
	if (subPage == NULL) {
		return FALSE;
	}
	
	if (subPage->status == EPS_SUBPAGE_STATUS_BUFFERING_COMPLETE
			&& subPage->fetchedLine < subPage->height) {
		return TRUE;
	}
	
	return FALSE;
}

int subPageSetRasterRotate0(EpsSubPage *subPage, char *raster, int bufSize)
{
	if (subPage == NULL) {
		return EPS_ERROR;
	}
	
	if (subPage->status != EPS_SUBPAGE_STATUS_BUFFERING) {
		return EPS_ERROR;
	}

	if (subPage->bytesPerLine > bufSize) {
		return EPS_ERROR;
	}
		
	memcpy(subPage->raster + subPage->bytesPerLine * subPage->bufferedLine,
		raster + subPage->raster_start, subPage->bytesPerLine);
	subPage->bufferedLine++;
#ifdef DEBUG_VERBOSE
	debuglog(("subPageSetRaster. : %p", subPage));
#endif	
	if (subPage->bufferedLine >= subPage->height) {
		subPage->fetchedLine = 0;
		subPage->status = EPS_SUBPAGE_STATUS_BUFFERING_COMPLETE;
	}
	
	return EPS_OK;
}

int subPageSetRasterRotate90(EpsSubPage *subPage, char *raster, int bufSize)
{
	int y, width, height;

	if (subPage == NULL) {
		return EPS_ERROR;
	}
	
	if (subPage->status != EPS_SUBPAGE_STATUS_BUFFERING) {
		return EPS_ERROR;
	}

	if (subPage->height > bufSize) {
		height = bufSize;
	} else {
		height = subPage->height;
	}
	width = (subPage->bytesPerLine / subPage->bytesPerPixel - 1) - subPage->bufferedLine;

	if (width >= 0)
	{
		for (y = 0; y < height; y++) {
			subPage->raster[y * subPage->bytesPerLine + subPage->bytesPerPixel * width]
				= raster[subPage->raster_start + subPage->bytesPerPixel * y];
			subPage->raster[y * subPage->bytesPerLine + subPage->bytesPerPixel * width + 1]
				= raster[subPage->raster_start + subPage->bytesPerPixel * y + 1];
			subPage->raster[y * subPage->bytesPerLine + subPage->bytesPerPixel * width + 2]
				= raster[subPage->raster_start + subPage->bytesPerPixel * y + 2];
		}
		subPage->bufferedLine++;
	}
#ifdef DEBUG_VERBOSE
	debuglog(("subPageSetRaster. : %p", subPage));
#endif	
	if (subPage->bufferedLine >= subPage->bytesPerLine / subPage->bytesPerPixel) {
		subPage->fetchedLine = 0;
		subPage->status = EPS_SUBPAGE_STATUS_BUFFERING_COMPLETE;
	}
	
	return EPS_OK;

}
