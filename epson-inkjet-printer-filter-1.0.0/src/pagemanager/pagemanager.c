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
#include "raster.h"
#include "pagemanager.h"
#include "raster-helper.h"

extern int JobCanceled;

typedef struct PageManagerPrivateData {
	EpsPageRegion sourceRegion; 
	EpsRasterPipeline * pipeline;
	RASTER raster_h;
	char* raster_buf;
} PageManagerPrivateData;

static int
fetchRaster(EpsPageManager *pageManager)
{
	PageManagerPrivateData  *privateData = (PageManagerPrivateData *)pageManager->privateData;
	EpsRasterFetchStatus status;
	char * ptr = privateData->raster_buf;
	int bytes = privateData->sourceRegion.bytesPerLine;
	int pixels = privateData->sourceRegion.width;
	int error = 0;
	int did_fetch = 0;
	int read_bytes = 0;
	int nraster;

	while (error == 0 && did_fetch == 0 && JobCanceled == 0) {
		eps_raster_fetch(privateData->raster_h, NULL, 0, 0, &status);
		switch (status) {
			case EPS_RASTER_FETCH_STATUS_HAS_RASTER:
				error = eps_raster_fetch(privateData->raster_h, ptr, bytes, pixels, &status);
				if (error == 0) {
					did_fetch = 1;
				}
				break;
			case EPS_RASTER_FETCH_STATUS_NEED_RASTER:
				read_bytes = pageManager->rasterSource(ptr, pageManager->cupsBytesPerLine);
				if (read_bytes == 0) { /* Flushing raster pipeline */
					error = eps_raster_print(privateData->raster_h, NULL, bytes, pixels, &nraster);
				} else if (read_bytes > 0) {
					error = eps_raster_print(privateData->raster_h, ptr, bytes, pixels, &nraster);
				} else { /* Error */
					error = 1;
				}
				break;
			case EPS_RASTER_FETCH_STATUS_COMPLETED:
				did_fetch = 1;
				error = 1;
				break;
			default:
				error = 1;
				break;
		}
	}

	return (error == 0) ? 1 : 0;
}

EpsPageManager* pageManagerCreate(EpsPageRegion pageRegion, EpsFilterPrintOption filterPrintOption, EpsRasterSource rasterSource)
{
	EpsPageManager*		pageManager;
	EpsSubPageManager	subPageManager;
	PageManagerPrivateData  *privateData;	

	EpsPageInfo page = { 0 };
	EpsRasterOpt rasteropt;

	pageManager = (EpsPageManager *)eps_malloc(sizeof(EpsPageManager));
	if (pageManager == NULL) {
		return NULL;
	}

	privateData = (PageManagerPrivateData *) eps_malloc(sizeof(PageManagerPrivateData));
	if (privateData == NULL) {
		eps_free(pageManager);
		return NULL;
	}
	debuglog(("pageManager Created."));

	pageManager->rasterSource		= rasterSource;
	pageManager->pageRegion			= pageRegion;
	pageManager->pageLayout			= filterPrintOption.pageLayout;
	pageManager->cupsHeight			= pageRegion.height;
	pageManager->cupsBytesPerLine	= pageRegion.bytesPerLine;	
	pageManager->currentLine		= 0;
	pageManager->subPageManager = subPageManagerCreate(&(pageManager->pageRegion), filterPrintOption.pageLayout);
	if (pageManager->subPageManager == NULL) {
		eps_free(pageManager);
		eps_free(privateData);
		return NULL;
	}

	privateData->raster_buf = (char *)eps_malloc(pageManager->cupsBytesPerLine);
	if (privateData->raster_buf == NULL) {
		subPageManagerDestroy(pageManager->subPageManager);
		eps_free(pageManager);
		eps_free(privateData);

		return NULL;
	}

	{
		privateData->sourceRegion = pageRegion;

		rasteropt.drv_handle = NULL;
		rasteropt.raster_output = NULL;
		page.bytes_per_pixel = pageRegion.bitsPerPixel / 8;
		page.src_print_area_x = pageRegion.width;
		page.src_print_area_y = pageRegion.height; 
		
		page.prt_print_area_x = pageRegion.width;
		page.prt_print_area_y = pageRegion.height;
		
		page.mirror = filterPrintOption.mirrorImage;
		if (filterPrintOption.rotate180) {
			page.reverse = 1;
			if (page.mirror) {
				page.mirror = 0;
			} else {
				page.mirror = 1;
			}
		}
		
		page.watermark.use = filterPrintOption.useWatermark;
		if (page.watermark.use) {
			page.watermark.filepath = filterPrintOption.watermarkFilePath;
			page.watermark.size_ratio = filterPrintOption.size_ratio;
			page.watermark.position = filterPrintOption.watermarkPosition;
			page.watermark.density = filterPrintOption.watermarkDensity;
			page.watermark.color = filterPrintOption.watermarkColor;
		}
		privateData->pipeline = (EpsRasterPipeline *) raster_helper_create_pipeline(&page, EPS_RASTER_PROCESS_MODE_FETCHING);

		if (eps_raster_init(&privateData->raster_h, &rasteropt, privateData->pipeline)) {
			subPageManagerDestroy(pageManager->subPageManager);
			eps_free(pageManager);
			eps_free(privateData);
			return NULL;
		}
	}
	
	pageManager->privateData 		= privateData;	

	return pageManager;
}

void pageManagerDestroy(EpsPageManager *pageManager)
{
	PageManagerPrivateData  *privateData;

	if (pageManager == NULL) {
		return;
	}

	privateData = (PageManagerPrivateData *)pageManager->privateData;
	if (privateData) {
		if (privateData->raster_buf) {
			eps_free(privateData->raster_buf);
		}

		if (privateData->raster_h) {
			eps_raster_free(privateData->raster_h);
		}

		if (privateData->pipeline) {
			raster_helper_destroy_pipeline(privateData->pipeline);
		}

		eps_free(privateData);
	}

	subPageManagerDestroy(pageManager->subPageManager);
	eps_free(pageManager);
	pageManager = NULL;

	debuglog(("pageManager Destroyed."));
}

int pageManagerGetPageRegion(EpsPageManager *pageManager, EpsPageRegion *pageRegion)
{
	if (pageManager == NULL) {
		return EPS_ERROR;
	}
	
	*pageRegion = pageManager->pageRegion;
	debuglog(("width = %d, Height =%d, bytesPerLine = %d, bitsPerPixel = %d", 
				pageManager->pageRegion.width, pageManager->pageRegion.height,
				pageManager->pageRegion.bytesPerLine, pageManager->pageRegion.bitsPerPixel));

	return EPS_OK;
}

int pageManagerGetRaster(EpsPageManager *pageManager, char *buf, int bufSize)
{
	PageManagerPrivateData  *privateData = NULL;
	int error = EPS_OK;

	if (pageManager == NULL) {
		return EPS_ERROR;
	}

	privateData = (PageManagerPrivateData *)pageManager->privateData;
	if (privateData == NULL) {
		return EPS_ERROR;
	}
	
	while (1) {
		if (subPageManagerGetRaster(pageManager->subPageManager, buf, bufSize) == EPS_OK) {
			break;
		}
		
		if (fetchRaster(pageManager) == 0) {
			//when printing poster, some subpages contain redundant line at the bottom
			//to avoid such situation, redundant lines must be cleaned by 0xFF
			if ((pageManager->subPageManager->pageLayout != EPS_PAGE_LAYOUT_2x1) && (pageManager->subPageManager->pageLayout != EPS_PAGE_LAYOUT_1x1)){
				memset(privateData->raster_buf, 0xff, pageManager->cupsBytesPerLine);
				subPageManagerSetRaster(pageManager->subPageManager, privateData->raster_buf, pageManager->cupsBytesPerLine);
			}
			else
			{
				if (subPageManagerFlushRaster(pageManager->subPageManager) != EPS_OK)
					break;
			}
			continue;
		}
	
		if (pageManager->currentLine >= pageManager->cupsHeight) {
			if (subPageManagerFlushRaster(pageManager->subPageManager) == EPS_OK) {
				continue;
			} else {
				break;
			}
		}

		subPageManagerSetRaster(pageManager->subPageManager, privateData->raster_buf, pageManager->cupsBytesPerLine);
		pageManager->currentLine++;
	}
	
	return error;
}

int pageManagerIsNextPage(EpsPageManager *pageManager)
{
	if (pageManager == NULL) {
		debuglog(("IsNextPage = ERROR"));
		return FALSE;
	}
	
	if (pageManager->currentLine < pageManager->cupsHeight) {
		debuglog(("IsNextPage = TRUE"));
		return TRUE;
	}
	
	if (subPageManagerIsNextPage(pageManager->subPageManager) == TRUE) {
		debuglog(("IsNextPage = TRUE"));
		return TRUE;	
	}
	
	debuglog(("IsNextPage = FALSE"));
	return FALSE;
}

