#ifndef LANG_CEM_CEMCOM_ANSI_ARITH_H
#define LANG_CEM_CEMCOM_ANSI_ARITH_H
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* COMPILER ARITHMETIC */

/*	Normally the compiler does its internal arithmetics in longs
	native to the source machine, which is always good for local
	compilations, and generally OK too for cross compilations
	downwards and sidewards.  For upwards cross compilation and
	to save storage on small machines, SPECIAL_ARITHMETICS will
	be handy.
*/

#include	"spec_arith.h"

#ifndef	SPECIAL_ARITHMETICS

#include <em_arith.h>		/* obtain definition of "arith"	*/

#else	/* SPECIAL_ARITHMETICS */

/*	All preprocessor arithmetic should be done in longs.
*/
#include <stdint.h>

typedef int32_t arith; /* dummy */

#endif	/* SPECIAL_ARITHMETICS */

#define	arith_size	(sizeof(arith))
#define	arith_sign	((arith) 1 << (arith_size * 8 - 1))
#define	max_arith	(~arith_sign)

struct expr;
struct type;	

/* lang/cem/cemcom.ansi/arith.c */
void arithbalance(struct expr **e1p, int oper, struct expr **e2p);
void relbalance(struct expr **e1p, int oper, struct expr **e2p);
void ch3pointer(struct expr **expp, int oper, struct type *tp);
int any2arith(struct expr **expp, int oper);
void erroneous2int(struct expr **expp);
struct expr *arith2arith(struct type *tp, int oper, struct expr *expr);
int int2int(struct expr **expp, struct type *tp);
void int2float(struct expr **expp, struct type *tp);
void float2int(struct expr **expp, struct type *tp);
void float2float(struct expr **expp, struct type *tp);
void array2pointer(struct expr *exp);
void function2pointer(struct expr *exp);
void string2pointer(struct expr *ex);
void opnd2integral(struct expr **expp, int oper);
void opnd2logical(struct expr **expp, int oper);
void opnd2test(struct expr **expp, int oper);
void any2opnd(struct expr **expp, int oper);
void any2parameter(struct expr **expp);
void field2arith(struct expr **expp);
void switch_sign_fp(struct expr *expr);

#endif /* LANG_CEM_CEMCOM_ANSI_ARITH_H */

