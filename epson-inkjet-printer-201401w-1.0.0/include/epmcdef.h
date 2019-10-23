/*
Copyright (c) 2009, Seiko Epson Corporation.
All rights reserved.

This program is covered by SEIKO EPSON CORPORATION SOFTWARE LICENSE AGREEMENT. 
*/
#ifndef __EPMCDEF_H__
#define __EPMCDEF_H__

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "err.h"

/*------------------------------- Define Basic Data Types ------------------------------*/
/*******************************************|********************************************/
typedef unsigned char  EPS_UINT8;       /* unsigned  8-bit  Min: 0          Max: 255        */
typedef unsigned short EPS_UINT16;      /* unsigned 16-bit  Min: 0          Max: 65535      */
typedef unsigned int   EPS_UINT32;      /* unsigned 32-bit  Min: 0          Max: 4294967295 */
typedef char           EPS_INT8;        /*   signed  8-bit  Min: -128       Max: 127        */
typedef short          EPS_INT16;       /*   signed 16-bit  Min: -32768     Max: 32767      */
typedef int            EPS_INT32;       /*   signed 32-bit  Min:-2147483648 Max: 2147483647 */
typedef float          EPS_FLOAT;       /*    float 32-bit  Min:3.4E-38     Max: 3.4E+38    */
typedef void*          EPS_PVOID;       /* Void pointer type                                */
typedef EPS_INT32      EPS_BOOL;        /* Boolean type                                     */
typedef EPS_INT32      EPS_ERR_CODE;    /* Error code for API's and routines                */

/*--------------------------------- Basic State Types ----------------------------------*/
/*******************************************|********************************************/
#ifndef NULL
#define NULL            0 
#endif

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

typedef enum {
	EPS_ERR_NONE			=     0,
	EPS_ERR_ERROR			=    -1,
	EPS_ERR_OPR_FAIL		= -1000,
	EPS_ERR_MEMORY_ALLOCATION	= -1001,
	EPS_ERR_INVALID_CALL 		= -1012,
	EPS_ERR_LIB_INTIALIZED		= -1050,
    	EPS_ERR_INV_ARGUMENT		= -1200,
    	EPS_ERR_INV_FUNCPTR		= -1300,
} EPS_RUN_TIME_ERROR;

typedef enum {
	EPS_SEEK_SET = 0,
	EPS_SEEK_CUR,
	EPS_SEEK_END,
} EPS_SEEK;	

typedef struct _tagEPS_LOCAL_TIME_ {
    EPS_UINT16	year;
    EPS_UINT8	mon;
    EPS_UINT8	day;
    EPS_UINT8	hour;
    EPS_UINT8	min;
    EPS_UINT8	sec;
} EPS_LOCAL_TIME;

typedef void* (* EPS_MemAlloc) (size_t size);
typedef void (* EPS_MemFree) (void* memblock);
typedef EPS_INT32 (* EPS_GetLocalTime) (EPS_LOCAL_TIME* epsTime);
typedef EPS_INT32 (* EPS_ResOpen) (EPS_INT8* resPath);
typedef EPS_INT32 (* EPS_ResRead) (EPS_INT32 fd, EPS_INT8* buffer, EPS_INT32 bufSize);
typedef EPS_INT32 (* EPS_ResSeek) (EPS_INT32 fd, EPS_INT32 offset, EPS_SEEK origin);
typedef EPS_INT32 (* EPS_ResClose) (EPS_INT32 fd);
typedef EPS_INT32 (* EPS_PrintStream) (EPS_INT8 * data, EPS_INT32 size);

typedef struct _tagEPS_RES_FUNC_ {
	EPS_INT32		size;
	EPS_MemAlloc		memAlloc;
	EPS_MemFree		memFree;
	EPS_GetLocalTime	getLocalTime;
	EPS_ResOpen		resOpen;
	EPS_ResRead		resRead;
	EPS_ResSeek		resSeek;
	EPS_ResClose		resClose;
} EPS_RES_FUNC;

typedef enum {
	EPS_MAINTENANCE_COMMAND_TYPE_UNKNOWN = 0,
	EPS_MAINTENANCE_NOZZLECHECK,
	EPS_MAINTENANCE_HEADCLEANING,
	EPS_MAINTENANCE_PRINTHEADALIGNMENT,
} EPS_MAINTENANCE_COMMAND_TYPE;

typedef enum {
	EPS_MAINTENANCE_STATUS_UNKNOWN = 0,
	EPS_MAINTENANCE_PROCESSING,
	EPS_MAINTENANCE_COMPLETED,
	EPS_MAINTENANCE_CANCELED,
	EPS_MAINTENANCE_ERROR,
	EPS_MAINTENANCE_NEED_NOZZLECHECK,
	EPS_MAINTENANCE_NEED_HEADCLEANING,
} EPS_MAINTENANCE_STATUS;

typedef struct _tagMaintenanceUIPicker{
	EPS_INT8* utf8Label;
	EPS_INT32 choiceNumber;
	EPS_INT32 defaultChoice;
	EPS_INT32 userChoice;
} EPS_MAINTENANCE_UI_PICKER;

typedef struct _tagMaintenanceUIButton {
	EPS_INT8* utf8Label;
	void * reservedData;
} EPS_MAINTENANCE_UI_BUTTON;

typedef struct _tagMaintenanceUIBMP {
	EPS_INT8* utf8Label;
	EPS_INT8* bmp_path;
} EPS_MAINTENANCE_UI_BMP;

typedef struct _tagMaintenanceCommandParameter_ {
	EPS_INT8* utf8Message;
	EPS_INT32 pickerCount;
	EPS_MAINTENANCE_UI_PICKER* pickerList; 
	EPS_INT32 buttonCount;
	EPS_MAINTENANCE_UI_BUTTON* buttonList; 
	EPS_INT32 buttonSelected;
	EPS_INT32 bmpCount;
	EPS_MAINTENANCE_UI_BMP* bmpList;
	void * reservedData;
} EPS_MAINTENANCE_COMMAND_PARAMETER;

typedef struct _tagMaintenanceCommand_ {
	EPS_MAINTENANCE_COMMAND_TYPE commandType;
	EPS_INT8* utf8CommandName;
	EPS_MAINTENANCE_COMMAND_PARAMETER* initParameter;
} EPS_MAINTENANCE_COMMAND;

typedef enum {
	EPS_MAINTENANCE_LOCALE_EN = 0,
	EPS_MAINTENANCE_LOCALE_JA,
	EPS_MAINTENANCE_LOCALE_FR,
	EPS_MAINTENANCE_LOCALE_DE,
	EPS_MAINTENANCE_LOCALE_IT,
	EPS_MAINTENANCE_LOCALE_PT,
	EPS_MAINTENANCE_LOCALE_ES,
	EPS_MAINTENANCE_LOCALE_NL,
	EPS_MAINTENANCE_LOCALE_RU,
	EPS_MAINTENANCE_LOCALE_ZH,
	EPS_MAINTENANCE_LOCALE_KO,
	EPS_MAINTENANCE_LOCALE_ZH_TW,
} EPS_MAINTENANCE_LOCALE;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPMCDEF_H__ */

