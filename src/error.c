/*****************************************************************************\
 *  $Id$
 *****************************************************************************
 *  Copyright (C) 2001-2002 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Andrew Uselton (uselton2@llnl.gov>
 *  UCRL-CODE-2002-008.
 *  
 *  This file is part of PowerMan, a remote power management program.
 *  For details, see <http://www.llnl.gov/linux/powerman/>.
 *  
 *  PowerMan is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *  
 *  PowerMan is distributed in the hope that it will be useful, but WITHOUT 
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 *  for more details.
 *  
 *  You should have received a copy of the GNU General Public License along
 *  with PowerMan; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
\*****************************************************************************/

/*
 * Somewhat like the sys_err() of Stevens, "UNIX Network Programming."
 */

#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>

#include "powerman.h"
#include "list.h"
#include "error.h"
#include "wrappers.h"

static char *err_prog = NULL;	/* basename of calling program */
static bool err_ttyvalid = TRUE;	/* use stderr until told otherwise */

#define ERROR_BUFLEN 1024

/*
 * Initialize this module with the name of the program.
 */
void err_init(char *prog)
{
    char *p = strrchr(prog, '/');	/* store only the basename */

    err_prog = p ? p + 1 : prog;
}

/*
 * Accessor function for syslog_enable.
 */
void err_notty(void)
{
    err_ttyvalid = FALSE;
}

/*
 * Report error message on either stderr or syslog, then exit.
 */
void err_exit(bool errno_valid, const char *fmt, ...)
{
    va_list ap;
    char buf[ERROR_BUFLEN];
    int er = errno;

    assert(err_prog!= NULL);

    va_start(ap, fmt);
    vsnprintf(buf, ERROR_BUFLEN, fmt, ap);
    /* overflow ignored on purpose */
    va_end(ap);

    if (errno_valid) {
	if (err_ttyvalid)
	    fprintf(stderr, "%s: %s: %s\n", err_prog, buf, strerror(er));
	else
	    syslog(LOG_ERR, "%s: %s", buf, strerror(er));
    } else {
	if (err_ttyvalid)
	    fprintf(stderr, "%s: %s\n", err_prog, buf);
        else
	    syslog(LOG_ERR, "%s", buf);
    }

#ifndef NDEBUG
	  fprintf(stderr, "memory not reclaimed: %d\n", Memory());
#endif
    exit(1);
}

/*
 * vi:softtabstop=4
 */
