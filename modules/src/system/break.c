/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#include "system.h"
#include <unistd.h>
#include <missing_proto.h>

char *sys_break(int incr)
{
	char *brk = (char*)sbrk(incr);

	if ((brk == (char *) 0) || (brk == (char *)-1))
		return ILL_BREAK;
	return brk;
}
