/*
Copyright (c) 2009, Seiko Epson Corporation.
All rights reserved.

This program is covered by SEIKO EPSON CORPORATION SOFTWARE LICENSE AGREEMENT. 
*/
#ifndef __EPCGDEF_H__
#define __EPCGDEF_H__

#include <sys/types.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define _USE_HT_MODULE

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

#define EPS_JOBNAME_BUFFSIZE	32

typedef enum {
	EPS_ERR_NONE				=     0,
	EPS_ERR_ERROR				=    -1,
	EPS_ERR_OPR_FAIL			= -1000,
	EPS_ERR_MEMORY_ALLOCATION	= -1001,
	EPS_ERR_INVALID_CALL 		= -1012,
	EPS_ERR_LIB_INTIALIZED		= -1050,
    EPS_ERR_INV_ARGUMENT		= -1200,
    EPS_ERR_INV_FUNCPTR			= -1300,
} EPS_RUN_TIME_ERROR;

typedef enum {
	EPS_SEEK_SET = 0,
	EPS_SEEK_CUR,
	EPS_SEEK_END,
} EPS_SEEK;	

typedef enum {
	EPS_PAGEATTRIB_PRINTABLEAREA_WIDTH,
	EPS_PAGEATTRIB_PRINTABLEAREA_HEIGHT,
	EPS_PAGEATTRIB_FLIP_VERTICAL,
	EPS_PAGEATTRIB_FLIP_HORIZONTAL,
} EPS_PAGEATTRIB;

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

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EPCGDEF_H__ */

