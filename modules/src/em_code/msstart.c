#include "em_private.h"

/* $Header$ */

CC_msstart(cst)
	int cst;
{
	/*	start of message
	*/
	PS(ps_mes);
	CST((arith)cst);
}