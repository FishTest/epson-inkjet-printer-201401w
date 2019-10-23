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

#ifndef __RASTER_TO_PRINTER_H_
#define __RASTER_TO_PRINTER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <cups/raster.h>
#include "epcgdef.h"

typedef EPS_ERR_CODE (* EPCGInitialize) (
	EPS_INT8*	modelName, 
	EPS_RES_FUNC*	resFuncPtrs
);

typedef EPS_ERR_CODE (* EPCGRelease) (
	void
);

typedef void (* EPCGGetVersion) (
	EPS_INT32*	major,
	EPS_INT32*	minor
);

typedef EPS_ERR_CODE (* EPCGSetResource) (
	EPS_INT32	resID,
	EPS_INT8*	resPath
);

typedef EPS_ERR_CODE (* EPCGGetOptionList) (
	EPS_INT32*	optionCount,
	EPS_INT8**	optionList
);

typedef EPS_ERR_CODE (* EPCGGetChoiceList) (
	EPS_INT8*	option,
	EPS_INT32*	choiceCount,
	EPS_INT8**	choiceList
);

typedef EPS_ERR_CODE (* EPCGSetPrintOption) (
	EPS_INT8*	option,
	EPS_INT8*	choice
);

typedef EPS_ERR_CODE (* EPCGGetPageAttribute) (
	EPS_UINT32	id,
	EPS_PVOID	value
);

typedef EPS_ERR_CODE (* EPCGStartJob) (
	EPS_PrintStream printStream,
	const EPS_INT8*	jobName
);

typedef EPS_ERR_CODE (* EPCGStartPage) (
	void
);

typedef EPS_ERR_CODE (* EPCGRasterOut) (
	EPS_INT8*	data,
	EPS_INT32	dataSize,
	EPS_INT32	pixelCount	
);

typedef EPS_ERR_CODE (* EPCGEndPage) (
	EPS_BOOL	bAbort
);

typedef EPS_ERR_CODE (* EPCGEndJob) (
	void
);

int printJob (void);

#ifdef __cplusplus
}
#endif

#endif /* __RASTER_TO_PRINTER_H_ */

