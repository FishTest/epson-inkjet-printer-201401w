/*
Copyright (c) 2009, Seiko Epson Corporation.
All rights reserved.

This program is covered by SEIKO EPSON CORPORATION SOFTWARE LICENSE AGREEMENT. 
*/
#ifndef __EPCGAPI_H__
#define __EPCGAPI_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "epcgdef.h"

EPS_ERR_CODE epcgInitialize (
	EPS_INT8*	modelName, 
	EPS_RES_FUNC*	resFuncPtrs
);

EPS_ERR_CODE epcgRelease (
	void
);

void epcgGetVersion(
	EPS_INT32*	major,
	EPS_INT32*	minor
);

EPS_ERR_CODE epcgSetResource (
	EPS_INT32	resID,
	EPS_INT8*	resPath
);

EPS_ERR_CODE epcgGetOptionList (
	EPS_INT32*	optionCount,
	EPS_INT8**	optionList
);

EPS_ERR_CODE epcgGetChoiceList (
	EPS_INT8*	option,
	EPS_INT32*	choiceCount,
	EPS_INT8**	choiceList
);

EPS_ERR_CODE epcgSetPrintOption (
	EPS_INT8*	option,
	EPS_INT8*	choice
);

EPS_ERR_CODE epcgGetPageAttribute (
	EPS_UINT32	id,
	EPS_PVOID	value
);

EPS_ERR_CODE epcgStartJob (
	EPS_PrintStream printStream,
	const EPS_INT8*	jobName
);

EPS_ERR_CODE epcgStartPage (
	void
);

EPS_ERR_CODE epcgRasterOut (
	EPS_INT8*	data,
	EPS_INT32	dataSize,
	EPS_INT32	pixelCount	
);

EPS_ERR_CODE epcgEndPage (
	EPS_BOOL	bAbort	
);

EPS_ERR_CODE epcgEndJob (
	void
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPCGAPI_H__ */

