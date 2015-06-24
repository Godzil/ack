#include "em_code.h"
#include "em_private.h"

/* $Id$ */

void CC_bhdnam(int op, arith n, char *s, arith off, int i)
{
	/*	BSS or HOL with size n, initial value a dnam(s, off),
		and flag i
	*/
	PS(op);
	CST(n);
	COMMA();
	NOFF(s, off);
	COMMA();
	CST((arith) i);
	NL();
}
