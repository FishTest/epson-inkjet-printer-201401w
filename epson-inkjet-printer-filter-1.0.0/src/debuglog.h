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
#ifndef __DEBUGLOG_H__
#define __DEBUGLOG_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if DEBUG
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
static char debuglog [2048];
static void
print_srcline (const char * filename, int line)
{
	memset(debuglog, 0, sizeof(debuglog));
	sprintf(debuglog, "[%s:%d] ", filename, line);
}

static void
print_log (const char * fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsprintf(debuglog + strlen(debuglog), fmt, ap);
	va_end(ap);

	#define LOGFILENAME "/tmp/"PACKAGE_NAME".txt"
	FILE * log = fopen(LOGFILENAME, "a+");
	if (log) {
		fprintf(log, "%s\n", debuglog);
		fclose(log);
	}

}
#define debuglog(a) print_srcline(__FUNCTION__, __LINE__); print_log a
#else
#define debuglog(a)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __DEBUGLOG_H__ */
