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
#ifndef __EPS_SUBPAGE_H__

#define __EPS_SUBPAGE_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define	EPS_ERROR	-1
#define	EPS_OK		0

typedef enum  {
	EPS_SUBPAGE_STATUS_BUFFERING = 0,
	EPS_SUBPAGE_STATUS_BUFFERING_COMPLETE
} EpsSubPageStatus;

typedef struct {
	char	*raster;
	int		raster_start;
	int		bytesPerLine;
	int		height;
	int		bufferedLine;
	int		fetchedLine;
	EpsSubPageStatus status;
	int		bytesPerPixel;
} EpsSubPage;

EpsSubPage* subPageCreate(int raster_start, int bytesPerLine, int height, int bytesPerPixel);
int subPageDestroy(EpsSubPage *subPage);
int subPageGetRaster(EpsSubPage *subPage, char *buf, int bufSize);
int subPageFlushRaster(EpsSubPage *subPage);
int subPageIsNextLine(EpsSubPage *subPage);
int subPageSetRasterRotate0(EpsSubPage *subPage, char *raster, int bufSize);
int subPageSetRasterRotate90(EpsSubPage *subPage, char *raster, int bufSize);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __EPS_SUBPAGE_H__ */

