/*
Copyright (c) 2009, Seiko Epson Corporation.
All rights reserved.

This program is covered by SEIKO EPSON CORPORATION SOFTWARE LICENSE AGREEMENT. 
*/
#ifndef __EPMCAPI_H__
#define __EPMCAPI_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "epmcdef.h"

EPS_ERR_CODE epmcInitialize (
	EPS_INT8* modelName, 
	EPS_RES_FUNC* resFuncPtrs,
	EPS_MAINTENANCE_LOCALE locale
);

EPS_ERR_CODE epmcRelease (
	void
);

void epmcGetVersion(
	EPS_INT32* major,
	EPS_INT32* minor
);

EPS_ERR_CODE epmcSetResource (
	EPS_INT32 resID,
	EPS_INT8* resPath
);

EPS_ERR_CODE epmcGetCommandList (
	EPS_INT32* commandCount,
	EPS_MAINTENANCE_COMMAND** commandList
);

EPS_MAINTENANCE_COMMAND_PARAMETER* epmcPerformCommand (
	EPS_PrintStream printStream,
	EPS_MAINTENANCE_COMMAND_PARAMETER* commandParameter,
	EPS_MAINTENANCE_STATUS* commandStatus 
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPMCAPI_H__ */

