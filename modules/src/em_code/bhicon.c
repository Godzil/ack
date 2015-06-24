#include "em_code.h"
#include "em_private.h"

/* $Id$ */

void CC_bhicon(int op, arith n, char *s, arith sz, int i)
{
	/*	BSS or HOL with size n, initial value an ICON (s, sz),
		and flag i
	*/
	PS(op);
	CST(n);
	COMMA();
	WCON(sp_icon, s, sz);
	COMMA();
	CST((arith) i);
	NL();
}
