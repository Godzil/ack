/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* COMPILER ARITHMETIC */

/*	Normally the compiler does its internal arithmetics in longs
	native to the source machine, which is always good for local
	compilations, and generally OK too for cross compilations
	downwards and sidewards.  For upwards cross compilation and
	to save storage on small machines, SPECIAL_ARITHMETICS will
	be handy.
*/

#include	"spec_arith.h"

#ifndef	SPECIAL_ARITHMETICS

#include <em_arith.h>		/* obtain definition of "arith"	*/

#else	/* SPECIAL_ARITHMETICS */

/* not implemented yet */
#include <stdint.h>

typedef int32_t arith;		/* dummy */

#endif	/* SPECIAL_ARITHMETICS */
