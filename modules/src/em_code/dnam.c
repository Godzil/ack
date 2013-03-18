#include "em_code.h"
#include "em_private.h"

/* $Id$ */

void CC_dnam(char *str, arith val)
{
	COMMA();
	NOFF(str, val);
}
