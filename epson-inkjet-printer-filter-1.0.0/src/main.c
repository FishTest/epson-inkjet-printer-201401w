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

#include <cups/cups.h>
#include <cups/ppd.h>
#include <cups/raster.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>

#include "raster_to_epson.h"
#include "debuglog.h"
#include "memory.h"

/*
 * $$$ CUPS Filter Options $$$
 *
 * printer  - The name of the printer queue
 *            (normally this is the name of the program being run)
 * job      - The numeric job ID for the job being printed
 * user     - The string from the originating-user-name attribute
 * title    - The string from the job-name attribute
 * copies   - The numeric value from the number-copies attribute
 * options  - String representations of the job template attributes, separated
 *            by spaces. Boolean attributes are provided as "name" for true
 *            values and "noname" for false values. All other attributes are
 *            provided as "name=value" for single-valued attributes and
 *            "name=value1,value2,...,valueN" for set attributes
 * filename - The request file
 */

cups_raster_t * Raster = NULL;
ppd_file_t * PPD = NULL;
char JobName[EPS_JOBNAME_BUFFSIZE] = { 0 };
const char * JobOptions = NULL;
int JobCanceled = 0;

static void cancel_job(int sig)
{
	debuglog(("JOB CANCELED"));
	(void) sig;
	JobCanceled = 1;
}

static void sig_set(void)
{
#ifdef HAVE_SIGSET /* Use System V signals over POSIX to avoid bugs */
	sigset(SIGTERM, cancel_job);
#elif defined(HAVE_SIGACTION)
	memset(&action, 0, sizeof(action));
	sigemptyset(&action.sa_mask);
	action.sa_handler = cancel_job;
	sigaction(SIGTERM, &action, NULL);
#else
	signal(SIGTERM, cancel_job);
#endif /* HAVE_SIGSET */
}

int
main (int argc, char *argv[])
{

	int fd;
	int result;
	char *ppd_path;     

	DUMP_HEAP_INIT();

	sig_set();

	result = 1; /* error */

	do {
		if (argc < 6 || argc > 7) {
			fprintf (stderr, "Insufficient options.");
			break;
		}

		if (argc == 7) {
			fd = open (argv[6], O_RDONLY);
			if (fd < 0) {
				perror ("open");
				break;
			}
		} else {
			fd = 0;
		}

		strncpy(JobName, argv[1], sizeof(JobName) - 1);
		JobOptions = argv[5];

		Raster = cupsRasterOpen (fd, CUPS_RASTER_READ);
		if (Raster == NULL) {
			fprintf (stderr, "Can't open CUPS raster file.");
			break;
		}

		ppd_path = (char *) cupsGetPPD (argv[0]);
		PPD = ppdOpenFile (ppd_path);
		if (PPD == NULL) {
			fprintf (stderr, "Can't open PPD file.");
			break;
		}

		if (printJob () != 0) {
			break;
		}

		result = 0; /* ok */

	} while (0);

	if (PPD) {
    		ppdClose (PPD);
	}

	if (Raster) {
		cupsRasterClose (Raster);
	}

	DUMP_HEAP_USAGE();

	return result;
}

