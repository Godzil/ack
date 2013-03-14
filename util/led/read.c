/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <stdio.h>
#include <out.h>
#include "const.h"
#include "assert.h"
#include "debug.h"
#include "arch.h"
#include "memory.h"
#include "defs.h"

int	infile;	/* The current input file. */

void rd_fatal()
{
	fatal("read error");
}
