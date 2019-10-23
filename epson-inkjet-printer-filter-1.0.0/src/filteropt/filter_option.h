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
#ifndef __EPS_FILTER_OPTION_H__

#define __EPS_FILTER_OPTION_H__

#include <cups/ppd.h>
#include "raster.h"
#include "filter_option_define.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct {
	EpsPageLayout	pageLayout;
	EpsRotate180	rotate180;
	EpsMirrorImage	mirrorImage;
	int 		useWatermark;
	char 		watermarkFilePath [512];
	float		size_ratio;
	EpsPageWatermarkPosition	watermarkPosition;
	EpsPageWatermarkDensity		watermarkDensity;
	EpsPageWatermarkColor		watermarkColor;
} EpsFilterPrintOption;

ppd_attr_t * get_ppd_attr(const char * name, int isFirst);
char * get_default_choice (const char *key);
char * get_option_for_job (const char * key);
int setup_filter_option (EpsFilterPrintOption *filterPrintOption);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __EPS_FILTER_OPTION_H__ */
