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

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dlfcn.h>

#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/raster.h>

#include "raster.h"
#include "memory.h"
#include "raster_to_epson.h"
#include "pagemanager.h"
#include "filter_option.h"
#include "raster-helper.h"

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

#define safeFree(ptr,releaseFunc) {	\
	if ((ptr) != NULL) { 		\
		releaseFunc((ptr)); 	\
		(ptr) = NULL; 		\
	} 				\
}

extern cups_raster_t * 		Raster;
extern ppd_file_t * 		PPD;
extern char			JobName[EPS_JOBNAME_BUFFSIZE];
extern const char *		JobOptions;
extern int			JobCanceled;

static EPCGInitialize		epcgInitialize = NULL;
static EPCGRelease		epcgRelease = NULL;
static EPCGGetVersion		epcgGetVersion = NULL; 
static EPCGSetResource		epcgSetResource = NULL;
static EPCGGetOptionList	epcgGetOptionList = NULL;
static EPCGGetChoiceList	epcgGetChoiceList = NULL;
static EPCGSetPrintOption	epcgSetPrintOption = NULL;
static EPCGGetPageAttribute	epcgGetPageAttribute = NULL;
static EPCGStartJob		epcgStartJob = NULL;
static EPCGStartPage		epcgStartPage = NULL;
static EPCGRasterOut		epcgRasterOut = NULL;
static EPCGEndPage		epcgEndPage = NULL;
static EPCGEndJob		epcgEndJob = NULL;

#if DEBUG
static int page_no = 0;
static int pageHeight = 0;
#endif

int rasterSource(char *buf, int bufSize)
{
	int readBytes = 0;
	if (JobCanceled == 0) {
		readBytes = cupsRasterReadPixels(Raster, buf, bufSize);
	} else {
		readBytes = (-1); /* error */
	} 

	return readBytes;
}

static void * memAlloc(size_t size)
{
	return malloc(size);
}

static void memFree(void * ptr)
{
	free(ptr);
}

static EPS_INT32 getLocalTime (EPS_LOCAL_TIME * epsTime)
{
	time_t now;
	struct tm *t;

	now = time(NULL);
	t = (struct tm *)localtime(&now);

	epsTime->year = (EPS_UINT16)t->tm_year + 1900;
	epsTime->mon = (EPS_UINT8)t->tm_mon + 1;
	epsTime->day = (EPS_UINT8)t->tm_mday;
	epsTime->hour = (EPS_UINT8)t->tm_hour;
	epsTime->min = (EPS_UINT8)t->tm_min;
	epsTime->sec = (EPS_UINT8)t->tm_sec;

	return 0;	
}

static EPS_INT32 resOpen(EPS_INT8* resPath)
{
	return open(resPath, O_RDONLY);
}

static EPS_INT32 resRead(EPS_INT32 fd, EPS_INT8* buffer, EPS_INT32 bufSize)
{
	return read(fd, buffer, bufSize);
}

static EPS_INT32 resSeek(EPS_INT32 fd, EPS_INT32 offset, EPS_SEEK origin)
{
	int seek;
	switch(origin) {
		case EPS_SEEK_SET: seek = SEEK_SET; break;
		case EPS_SEEK_CUR: seek = SEEK_CUR; break;
		case EPS_SEEK_END: seek = SEEK_END; break;
		default: break;
	}
	return lseek(fd, offset, seek);
}

static EPS_INT32 resClose(EPS_INT32 fd)
{
	return close(fd);
}


static EPS_INT32 printStream (EPS_INT8* data, EPS_INT32 size)
{
	return fwrite(data, 1, size, stdout);
}

static int pipeOut(HANDLE handle, char* data, int dataSize, int pixelCount)
{
	(void) handle;

#if DEBUG
	pageHeight++;
#endif

	return epcgRasterOut(data, dataSize, pixelCount);
}

static int load_core_library (HANDLE * handle)
{
	debuglog(("TRACE IN"));
	
	HANDLE lib_handle = NULL;
	int error = 1;
	ppd_attr_t * attr = NULL;
	EPS_RES_FUNC resFunc;
	char library [PATH_MAX];

	do {
		attr = get_ppd_attr ("epcgCoreLibrary", 1);
		if (attr == NULL) {
			break;
		}

		snprintf(library, sizeof(library), "%s/%s", CORE_LIBRARY_PATH, attr->value);
		lib_handle = dlopen(library, RTLD_LAZY);
		if (lib_handle == NULL) {
			debuglog(("Failed to dlopen(%s)->%s", attr->value, dlerror()));
			break;
		}

		/* Setting of library function */
		epcgInitialize = (EPCGInitialize) dlsym (lib_handle, "epcgInitialize");
		epcgRelease = (EPCGRelease) dlsym (lib_handle, "epcgRelease");
		epcgGetVersion = (EPCGGetVersion) dlsym (lib_handle, "epcgGetVersion");
		epcgSetResource = (EPCGSetResource) dlsym (lib_handle, "epcgSetResource");
		epcgGetOptionList= (EPCGGetOptionList) dlsym (lib_handle, "epcgGetOptionList");
		epcgGetChoiceList= (EPCGGetChoiceList) dlsym (lib_handle, "epcgGetChoiceList");
		epcgSetPrintOption= (EPCGSetPrintOption) dlsym (lib_handle, "epcgSetPrintOption");
		epcgGetPageAttribute= (EPCGGetPageAttribute) dlsym (lib_handle, "epcgGetPageAttribute");
		epcgStartJob= (EPCGStartJob) dlsym (lib_handle, "epcgStartJob");
		epcgStartPage= (EPCGStartPage) dlsym (lib_handle, "epcgStartPage");
		epcgRasterOut= (EPCGRasterOut) dlsym (lib_handle, "epcgRasterOut");
		epcgEndPage= (EPCGEndPage) dlsym (lib_handle, "epcgEndPage");
		epcgEndJob= (EPCGEndJob) dlsym (lib_handle, "epcgEndJob");

		if (epcgInitialize == NULL
			|| epcgRelease == NULL
			|| epcgGetVersion == NULL
			|| epcgSetResource == NULL
			|| epcgGetOptionList == NULL
			|| epcgGetChoiceList == NULL
			|| epcgSetPrintOption == NULL
			|| epcgGetPageAttribute == NULL
			|| epcgStartJob == NULL
			|| epcgStartPage == NULL
			|| epcgRasterOut == NULL
			|| epcgEndPage == NULL
			|| epcgEndJob == NULL) {
			debuglog(("Failed to dlsym"));
			break;
		}

		resFunc.size = sizeof(EPS_RES_FUNC);
		resFunc.memAlloc = memAlloc;
		resFunc.memFree = memFree;
		resFunc.getLocalTime = getLocalTime;
		resFunc.resOpen = resOpen;
		resFunc.resRead = resRead;
		resFunc.resSeek = resSeek;
		resFunc.resClose= resClose;
		
		debuglog(("Model name : %s", PPD->modelname));

		error = epcgInitialize (PPD->modelname, &resFunc);
		if (error) {
			break;
		}

	} while (0);

	if(error && lib_handle) {
		dlclose (lib_handle);
		lib_handle = NULL;
	}

	*handle = lib_handle;
		
	debuglog(("TRACE OUT=%d", error));

	return error;
}

static int setup_option (void)
{
	debuglog(("TRACE IN"));

	EPS_INT32 optionCount = 0;
	EPS_INT8** optionList = NULL;
	char * option = NULL;
	char * choice = NULL;

	ppd_attr_t * attr = NULL;
	int isFirst = 0;

	int error = 1;
	int i;

	char resource [PATH_MAX];

	do {
		isFirst = 1;
		while (attr = get_ppd_attr ("epcgResourceData", isFirst)) {
			memset(resource, 0x00, sizeof(resource));
			sprintf(resource, "%s/%s", CORE_RESOURCE_PATH, attr->value);

			error = epcgSetResource(atoi(attr->spec), resource);
			if (error) {
				break;
			}

			isFirst = 0;
		}
		
		if (error) {
			break;
		}

		error = epcgGetOptionList (&optionCount, NULL);
		if (error) {
			break;
		}
		
		optionList = (EPS_INT8**) eps_malloc (optionCount * sizeof(EPS_INT8*));
		if (optionList == NULL) {
			break;
		}

		error = epcgGetOptionList (&optionCount, optionList);
		if (error) {
			break;
		}

		debuglog(("Job Options =%s", JobOptions));

		for (i = 0; i < optionCount; i++) {
			option = optionList[i];
			choice = get_option_for_job (option);
			if (choice == NULL) {
				choice = get_default_choice (option);
				if (choice == NULL) {
					error = 1;
					break;
				}
			}

			debuglog(("Option=%s Choice=%s", option, choice));
			error = epcgSetPrintOption(option, choice);
			if (error) {
				break;
			}
		}

		if (error) {
			break;
		}
	
	} while (0);	

	if (optionList) {
		eps_free (optionList);
	}

	debuglog(("TRACE OUT=%d", error));

	return error;
}

static int unload_core_library (HANDLE * handle)
{
	debuglog(("TRACE IN"));

	epcgRelease();
	
	if (handle) {
		dlclose(handle);
	}

	debuglog(("TRACE OUT=%d", 0));

	return 0;
}

static int print_page (void)
{
	debuglog(("TRACE IN"));

	cups_page_header_t header;

	EpsRasterPipeline * pipeline = NULL;
	char * image_raw = NULL;
	RASTER raster_h;

	EPS_INT32 printableWidth;
	EPS_INT32 printableHeight;
	EPS_INT32 flipVertical;
	EPS_INT32 flipHorizontal;
	EPS_BOOL bAbort;

	EpsPageInfo page = { 0 };
	EpsRasterOpt rasteropt;

	int error;
	size_t nraster;
	int i;
	EpsPageManager		*pageManager;
	EpsPageRegion		 pageRegion;
	EpsFilterPrintOption	filterPrintOption;

	rasteropt.drv_handle = NULL;
	rasteropt.raster_output = pipeOut;


	error = setup_filter_option (&filterPrintOption);
	if(error) {
		error = 1;
		return error;
	}

	while (JobCanceled == 0 && error == 0 && cupsRasterReadHeader (Raster, &header)) {

		error = 0;
		raster_h = NULL;

		pageRegion.width = header.cupsWidth;
		pageRegion.height = header.cupsHeight;
		pageRegion.bytesPerLine = header.cupsBytesPerLine;
		pageRegion.bitsPerPixel = header.cupsBitsPerPixel;
		pageManager = pageManagerCreate(pageRegion, filterPrintOption, rasterSource);
		if (pageManager == NULL) {
			error = 1;
			break;
		}
		pageManagerGetPageRegion(pageManager, &pageRegion);
		
		image_raw = (char * ) eps_malloc(pageRegion.bytesPerLine);
		if (image_raw == NULL) {
			error = 1;
			break;
		}

		epcgGetPageAttribute (EPS_PAGEATTRIB_PRINTABLEAREA_WIDTH, &printableWidth);
		epcgGetPageAttribute (EPS_PAGEATTRIB_PRINTABLEAREA_HEIGHT, &printableHeight);
		epcgGetPageAttribute (EPS_PAGEATTRIB_FLIP_VERTICAL, &flipVertical);
		epcgGetPageAttribute (EPS_PAGEATTRIB_FLIP_HORIZONTAL, &flipHorizontal);

		page.bytes_per_pixel = pageRegion.bitsPerPixel / 8;
		page.src_print_area_x = pageRegion.width;
		page.src_print_area_y = pageRegion.height; 

		{
			page.prt_print_area_x = printableWidth;
			page.prt_print_area_y = printableHeight;
			page.reverse = (flipVertical) ? 1 : 0;
			page.mirror = (flipHorizontal) ? 1 : 0;
			page.scale = ((page.src_print_area_x != page.prt_print_area_x) || (page.src_print_area_y != page.prt_print_area_y)) ? 1 : 0;
		}

		do {
			pipeline = (EpsRasterPipeline *) raster_helper_create_pipeline(&page, EPS_RASTER_PROCESS_MODE_PRINTING);
			if (eps_raster_init(&raster_h, &rasteropt, pipeline)) {
				error = 1;
				break;
			}

			if (epcgStartPage()) {
				epcgEndPage(TRUE);  /* Abort */
				error = 1;
				break;
			}

			for (i = 0; i < pageRegion.height; i++) {
				if ((pageManagerGetRaster(pageManager, image_raw, pageRegion.bytesPerLine) != EPS_OK) || (JobCanceled)) {
					error = 1;
					break;
				}

				if (eps_raster_print(raster_h, image_raw, pageRegion.bytesPerLine, pageRegion.width, (int *)&nraster)) {
					error  = 1;
					break;
				}
			}

			// flushing page
			eps_raster_print(raster_h, NULL, 0, 0, (int *)&nraster);

			bAbort = (error) ? TRUE : FALSE;
			if (epcgEndPage (bAbort)) {
				error = 1;
			}

#if DEBUG
			debuglog(("page_no = %d, pageHeight = %d", ++page_no, pageHeight));
			pageHeight = 0;
#endif

			safeFree(raster_h, eps_raster_free);
			safeFree(pipeline, raster_helper_destroy_pipeline);

		} while (error == 0 && pageManagerIsNextPage(pageManager) == TRUE);

		safeFree(image_raw, eps_free);
		safeFree(raster_h, eps_raster_free);
		safeFree(pipeline, raster_helper_destroy_pipeline);
		safeFree(pageManager, pageManagerDestroy);
	}

	safeFree(image_raw, eps_free);
	safeFree(raster_h, eps_raster_free);
	safeFree(pipeline, raster_helper_destroy_pipeline);
	safeFree(pageManager, pageManagerDestroy);

	debuglog(("TRACE OUT=%d", error));

	return error;
}

int printJob (void)
{
	debuglog(("TRACE IN"));

	HANDLE lib_handle = NULL;
	EPS_BOOL jobStarted = FALSE;
	int error = 1; 

	do {
		error = load_core_library (&lib_handle);
		if(error) {
			break;
		}

		error = setup_option ();
		if(error) {
			break;
		}

		debuglog(("Job name : %s", JobName));

		error = epcgStartJob((EPS_PrintStream) printStream, JobName);
		if(error) {
			break;
		}

		jobStarted = TRUE;

		error = print_page ();
		if(error) {
			break;
		}

		error = 0;

	} while (0);

	if (jobStarted == TRUE) {
		epcgEndJob();
	}

	if (lib_handle) {
		unload_core_library (lib_handle);
	}

	debuglog(("TRACE OUT=%d", error));

	return error;
}
