#include "em_private.h"

/* $Header$ */

CC_dnam(str, val)
	char *str;
	arith val;
{
	COMMA();
	NOFF(str, val);
}