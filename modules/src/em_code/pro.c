#include "em_code.h"
#include "em_private.h"

/* $Id$ */

void CC_pro(char *pnam, arith l)
{
	/*	PRO pseudo with procedure name pnam and # of locals l
	*/
	PS(ps_pro);
	PNAM(pnam);
	COMMA();
	CST(l);
	NL();
}
