#include "em_code.h"
#include "em_private.h"

/* $Id$ */

void CC_msstart(int cst)
{
	/*	start of message
	*/
	PS(ps_mes);
	CST((arith)cst);
}
