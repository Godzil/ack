#include "em_private.h"

/* $Header$ */

CC_scon(str, siz)
	char *str;
	arith siz;
{
	COMMA();
	SCON(str, siz);
}