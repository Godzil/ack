#include "em_code.h"
#include "em_private.h"

/* $Id$ */

void CC_op(int opcode)
{
	/*	Instruction that never has an argument
		Argument types: -
	*/
	OP(opcode);
	NL();
}
