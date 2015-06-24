/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Id$ */

#include "flt_misc.h"
#include <em_arith.h>

void flt_arith2flt(arith n, flt_arith *e, int uns)
{
	/*	Convert the arith "n" to a flt_arith "e".
	*/

	if (!uns && n < 0) {
		e->flt_sign = 1;
		n = -n;
	}
	else	e->flt_sign = 0;
	if (sizeof(arith) == 4) {
		e->m1 = 0; e->m2 = n;
	}
	else {
		e->m2 = n & 0xffffffffL;
		n >>= 1;
		n &= ~((arith) 1 << (8*sizeof(arith)-1));
		e->m1 = (n >> 31);
	}
	if (n == 0) {
		e->flt_exp = 0;
		return;
	}
	e->flt_exp = 63;
		
	flt_status = 0;
	flt_nrm(e);
}
