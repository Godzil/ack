/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */
/*	A R I T H M E T I C   C O N V E R S I O N S	 */

/*	This file contains the routines for the various conversions that
	may befall operands in C. It is structurally a mess, but I haven't
	decided yet whether I can't find the right structure or the
	semantics of C is a mess.
*/

#include	<alloc.h>
#include	"lint.h"
#include	"nobitfield.h"
#include	"idf.h"
#include	<flt_arith.h>
#include	"arith.h"
#include	"sizes.h"
#include	"type.h"
#include	"proto.h"
#include	"label.h"
#include	"expr.h"
#include	"Lpars.h"
#include	"field.h"
#include	"mes.h"
#include	"assert.h"

extern char *symbol2str();
extern char options[];

arithbalance(e1p, oper, e2p)	/* 3.1.2.5 */
	register struct expr **e1p, **e2p;
	int oper;
{
	/*	The expressions *e1p and *e2p are balanced to be operands
		of the arithmetic operator oper.
	*/
	register int t1, t2, u1, u2;
	int shifting = (oper == LEFT || oper == RIGHT
			|| oper == LEFTAB || oper == RIGHTAB);

	t1 = any2arith(e1p, oper);
	t2 = any2arith(e2p, oper);
	/* Now t1 and t2 are either INT, LONG, FLOAT, DOUBLE, or LNGDBL */

	/*	If any operand has the type long double, the other operand
		is converted to long double.
	*/
	/* ??? t1 == LNGDBL, t2 == DOUBLE */
	if (t1 == LNGDBL) {
		if (t2 != LNGDBL)
			int2float(e2p, lngdbl_type);
		return;
	} else if (t2 == LNGDBL) {
		if (t1 != LNGDBL)
			int2float(e1p, lngdbl_type);
		return;
	}

	/*	If any operand has the type double, the other operand
		is converted to double.
	*/
	if (t1 == DOUBLE) {
		if (t2 != DOUBLE)
			int2float(e2p, double_type);
		return;
	} else if (t2 == DOUBLE) {
		if (t1 != DOUBLE)
			int2float(e1p, double_type);
		return;
	}

	/*	If any operand has the type float, the other operand
		is converted to float.
	*/
	if (t1 == FLOAT) {
		if (t2 != FLOAT)
			int2float(e2p, float_type);
		return;
	} else if (t2 == FLOAT) {
		if (t1 != FLOAT)
			int2float(e1p, float_type);
		return;
	}

	/* Now they are INT or LONG */
	u1 = (*e1p)->ex_type->tp_unsigned;
	u2 = (*e2p)->ex_type->tp_unsigned;

	/*	If either operand has type unsigned long int, the other
		operand is converted to unsigned long int.
	*/
	if (t1 == LONG && u1 && (t2 != LONG || !u2))
		t2 = int2int(e2p, ulong_type);
	else if (t2 == LONG && u2 && (t1 != LONG || !u1)
			&& !shifting)	/* ??? */
		t1 = int2int(e1p, ulong_type);

	/*	If one operand has type long int and the other has type unsigned
		int, if a long int can represent all values of an unsigned int,
		the operand of type unsigned int is converted to long int; if
		a long int cannot represent all values of an unsigned int,
		both operands are converted to unsigned long int.
	*/
	if (t1 == LONG && t2 == INT && u2)
		t2 = int2int(e2p, (int_size<long_size)? long_type : ulong_type);
	else if (t2 == LONG && t1 == INT && u1 && !shifting)	/* ??? */
		t1 = int2int(e1p, (int_size<long_size)? long_type : ulong_type);
	if (int_size > long_size) /* sanity check */
		crash("size of int exceeds size of long");

	/*	If either operand has type long int, the other operand is con-
		verted to long int.
	*/
	if (t1 == LONG && t2 != LONG)
		t2 = int2int(e2p, long_type);
	else
	if (t2 == LONG && t1 != LONG && !shifting)	/* ??? */
		t1 = int2int(e1p, long_type);

	/*	If either operand has type unsigned int, the other operand
		is converted to unsigned int.
		Otherwise, both operands have type int.
	*/
	if (u1 && !u2 && !shifting)
		t2 = int2int(e2p, (t1 == LONG) ? ulong_type : uint_type);
	else
	if (!u1 && u2 && !shifting)
		t1 = int2int(e1p, (t2 == LONG) ? ulong_type : uint_type);
}

relbalance(e1p, oper, e2p)
	register struct expr **e1p, **e2p;
{
	/*	The expressions *e1p and *e2p are balanced to be operands
		of the relational operator oper.
	*/
	if ((*e1p)->ex_type->tp_fund == FUNCTION)
		function2pointer(*e1p);
	if ((*e2p)->ex_type->tp_fund == FUNCTION)
		function2pointer(*e2p);
	if ((*e1p)->ex_type->tp_fund == POINTER)
		ch3pointer(e2p, oper, (*e1p)->ex_type);
	else if ((*e2p)->ex_type->tp_fund == POINTER)
		ch3pointer(e1p, oper, (*e2p)->ex_type);
	else if ((*e1p)->ex_type == (*e2p)->ex_type
		&& (*e1p)->ex_type->tp_fund == ENUM) {}
	else if (oper == ':'
		    && (*e1p)->ex_type->tp_fund == VOID
		    && (*e2p)->ex_type->tp_fund == VOID) {}
	else
		arithbalance(e1p, oper, e2p);
}

ch3pointer(expp, oper, tp)
	struct expr **expp;
	register struct type *tp;
{
	/*	Checks whether *expp may be compared to tp using oper,
		as described in chapter 3.3.8 and 3.3.9.
		tp is known to be a pointer.
	*/
	register struct expr *exp = *expp;

	if (exp->ex_type->tp_fund == POINTER)	{
		if (exp->ex_type != tp)
			ch3cast(expp, oper, tp);
	}
	else
	if (is_integral_type(exp->ex_type)) {
		if ((oper != EQUAL && oper != NOTEQUAL && oper != ':')
		    || !(is_cp_cst(exp) && exp->VL_VALUE == 0)) {
			expr_error(exp,"%s on %s and pointer",
					symbol2str(oper),
					symbol2str(exp->ex_type->tp_fund));
		}
		ch3cast(expp, CAST, tp);
	}
	else	{
		expr_error(exp, "%s on %s and pointer",
				symbol2str(oper),
				symbol2str(exp->ex_type->tp_fund)
			);
		ch3cast(expp, oper, tp);
	}
}

int
any2arith(expp, oper)
	register struct expr **expp;
	register int oper;
{
	/*	Turns any expression into int_type, long_type,
		float_type, double_type or lngdbl_type.
	*/
	int fund;

	switch (fund = (*expp)->ex_type->tp_fund)	{
	case CHAR:
	case SHORT:
	case GENERIC:
		ASSERT((*expp)->ex_type->tp_size <= int_type->tp_size);

		if ((*expp)->ex_type->tp_unsigned
		    && (*expp)->ex_type->tp_size == int_type->tp_size) {
			int2int(expp, uint_type);
		} else {
			int2int(expp, int_type);
		}
		break;
	case INT:
	case LONG:
		break;
	case ENUM:
#ifndef	LINT
		int2int(expp, int_type);
#endif	LINT
		break;
	case FLOAT:
/*	only when it is a parameter and the default promotion should
	occur. Hence this code is moved to any2parameter().
		float2float(expp, double_type);
		break;
*/
	case DOUBLE:
	case LNGDBL:
		break;
#ifndef NOBITFIELD
	case FIELD:
		field2arith(expp);
		break;
#endif NOBITFIELD
	default:
		expr_error(*expp, "operator %s on non-numerical operand (%s)",
			symbol2str(oper), symbol2str(fund));
	case ERRONEOUS:
		erroneous2int(expp);
		break;
	}

	return (*expp)->ex_type->tp_fund;
}

erroneous2int(expp)
	struct expr **expp;
{
	/*	the (erroneous) expression *expp is replaced by an
		int expression
	*/
	register struct expr *exp = *expp;
	int flags = exp->ex_flags;
	
	free_expression(exp);
	exp = intexpr((arith)0, INT);
	exp->ex_flags = (flags | EX_ERROR);
	*expp = exp;
}

struct expr *
arith2arith(tp, oper, expr)
	struct type *tp;
	int oper;
	register struct expr *expr;
{
	/*	arith2arith constructs a new expression containing a
		run-time conversion between some arithmetic types.
	*/
	register struct expr *new = new_expr();
	
	new->ex_file = expr->ex_file;
	new->ex_line = expr->ex_line;
	new->ex_type = tp;
	new->ex_class = Type;
	return new_oper(tp, new, oper, expr);
}

int
int2int(expp, tp)
	struct expr **expp;
	register struct type *tp;
{
	/*	The expression *expp, which is of some integral type, is
		converted to the integral type tp.
	*/
	register struct expr *exp = *expp;
	
	if (is_cp_cst(exp))	{
		register struct type *tp1 = exp->ex_type;

		exp->ex_type = tp;
		if (! tp1->tp_unsigned && tp->tp_unsigned) {
			/*	Avoid "unreal" overflow warnings, such as
				caused by f.i.:
					unsigned int x = ~0;
					unsigned int y = -1;
			*/
			extern long full_mask[];
			long remainder = exp->VL_VALUE &
						~full_mask[(int)(tp->tp_size)];

			if (remainder == 0 ||
			    remainder == ~full_mask[(int)(tp->tp_size)]) {
				exp->VL_VALUE &= ~remainder;
			}
		}
		cut_size(exp);
	}
	else	{
		exp = arith2arith(tp, INT2INT, exp);
	}
	*expp = exp;
	return exp->ex_type->tp_fund;
}

int2float(expp, tp)
	register struct expr **expp;
	struct type *tp;
{
	/*	The expression *expp, which is of some integral type, is
		converted to the floating type tp.
	*/
	register struct expr *exp = *expp;
	
	fp_used = 1;
	if (is_cp_cst(exp)) {
		*expp = new_expr();
		**expp = *exp;
		/* sprint(buf+1, "%ld", (long)(exp->VL_VALUE));
		/* buf[0] = '-';
		*/
		exp = *expp;	/* ??? */
		
		exp->ex_type = tp;
		exp->ex_class = Float;
		/* exp->FL_VALUE = 0 /* Salloc(buf, (unsigned)strlen(buf)+1) */ ;
		flt_arith2flt(exp->VL_VALUE, &(exp->FL_ARITH));
		/* exp->FL_DATLAB = 0; */
	}
	else	*expp = arith2arith(tp, INT2FLOAT, *expp);
}

float2int(expp, tp)
	struct expr **expp;
	struct type *tp;
{
	/*	The expression *expp, which is of some floating type, is
		converted to the integral type tp.
	*/
	
	fp_used = 1;
	*expp = arith2arith(tp, FLOAT2INT, *expp);
}

float2float(expp, tp)
	register struct expr **expp;
	struct type *tp;
{
	/*	The expression *expp, which is of some floating type, is
		converted to the floating type tp.
		There is no need for an explicit conversion operator
		if the expression is a constant.
	*/
	
	fp_used = 1;
	if (is_fp_cst(*expp))
		(*expp)->ex_type = tp;
	else
		*expp = arith2arith(tp, FLOAT2FLOAT, *expp);
}

array2pointer(exp)
	register struct expr *exp;
{
	/*	The expression, which must be an array, is converted
		to a pointer.
	*/
	exp->ex_type = construct_type(POINTER, exp->ex_type->tp_up
				    , /* exp->ex_type->tp_typequal */ 0
				    , (arith)0, NO_PROTO);
}

function2pointer(exp)
	register struct expr *exp;
{
	/*	The expression, which must be a function, is converted
		to a pointer to the function.
	*/
	exp->ex_type = construct_type(POINTER, exp->ex_type, 0,
				     (arith)0, NO_PROTO);
}

string2pointer(ex)
	register struct expr *ex;
{
	/*	The expression, which must be a string constant, is converted
		to a pointer to the string-containing area.
	*/
	label lbl = data_label();

	code_string(ex->SG_VALUE, ex->SG_LEN, lbl);
	ex->ex_class = Value;
	ex->VL_CLASS = Label;
	ex->VL_LBL = lbl;
	ex->VL_VALUE = (arith)0;
}

opnd2integral(expp, oper)
	register struct expr **expp;
	int oper;
{
	register int fund = (*expp)->ex_type->tp_fund;

	if (fund != INT && fund != LONG)	{
		expr_error(*expp, "%s operand to %s",
				symbol2str(fund), symbol2str(oper));
		erroneous2int(expp);
		/* fund = INT; */
	}
}

opnd2logical(expp, oper)
	register struct expr **expp;
	int oper;
{
	int fund = (*expp)->ex_type->tp_fund;

	if (fund == FUNCTION || fund == ARRAY) {
		expr_warning(*expp, "%s operand to %s",
			symbol2str(fund),
			symbol2str(oper));
		if (fund == FUNCTION) {
			function2pointer(*expp);
		}
		else	array2pointer(*expp);
	}
#ifndef NOBITFIELD
	else
	if (fund == FIELD)
		field2arith(expp);
#endif NOBITFIELD
	switch (fund = (*expp)->ex_type->tp_fund) {
	case CHAR:
	case SHORT:
	case INT:
	case LONG:
	case ENUM:
	case POINTER:
	case FLOAT:
	case DOUBLE:
	case LNGDBL:
		break;
	default:
		expr_error(*expp, "%s operand to %s",
			symbol2str(fund), symbol2str(oper));
	case ERRONEOUS:
		erroneous2int(expp);
		break;
	}
}

opnd2test(expp, oper)
	register struct expr **expp;
{
	opnd2logical(expp, oper);
	if ((*expp)->ex_class == Oper) {
		switch((*expp)->OP_OPER) {
		case '<':
		case '>':
		case LESSEQ:
		case GREATEREQ:
		case EQUAL:
		case NOTEQUAL:
		case '!':
		case AND:
		case OR:	/* && and || also impose a test	*/
			/* It is already a test */
			return;
		case ',':
			opnd2test(&((*expp)->OP_RIGHT), oper);
			return;
		}
	}
	ch3bin(expp, NOTEQUAL, intexpr((arith)0, INT));
}

any2opnd(expp, oper)
	register struct expr **expp;
{
	if (!*expp)
		return;
	switch ((*expp)->ex_type->tp_fund)	{
	case CHAR:
	case SHORT:
	case ENUM:
	/* case FLOAT:	/* not necessary anymore */
		any2arith(expp, oper);
		break;
	case ARRAY:
		array2pointer(*expp);
		break;
	case POINTER:
		if ((*expp)->ex_class == String)
			string2pointer(*expp);
		break;
#ifndef NOBITFIELD
	case FIELD:
		field2arith(expp);
		break;
#endif NOBITFIELD
	}
}

any2parameter(expp)
	register struct expr **expp;
{
	/*	To handle default argument promotions
	*/
	any2opnd(expp, '(');
	if ((*expp)->ex_type->tp_fund == FLOAT)
		float2float(expp, double_type);
}

#ifndef NOBITFIELD
field2arith(expp)
	register struct expr **expp;
{
	/*	The expression to extract the bitfield value from the
		memory word is put in the tree.
	*/
	register struct type *tp = (*expp)->ex_type->tp_up;
	register struct field *fd = (*expp)->ex_type->tp_field;
	register struct type *atype = tp->tp_unsigned ? uword_type : word_type;

	(*expp)->ex_type = atype;

	if (atype->tp_unsigned)	{	/* don't worry about the sign bit */
		ch3bin(expp, RIGHT, intexpr((arith)fd->fd_shift, INT));
		ch3bin(expp, '&', intexpr(fd->fd_mask, INT));
	}
	else	{	/* take care of the sign bit: sign extend if needed */
		arith bits_in_type = atype->tp_size * 8;

		ch3bin(expp, LEFT,
			intexpr(bits_in_type - fd->fd_width - fd->fd_shift,
						INT)
		);
		ch3bin(expp, RIGHT, intexpr(bits_in_type - fd->fd_width, INT));
	}
	ch3cast(expp, CAST, tp);	/* restore its original type */
}
#endif NOBITFIELD

/*	switch_sign_fp() negates the given floating constant expression,
 *	and frees the string representing the old value.
 */
switch_sign_fp(expr)
	register struct expr *expr;
{
	flt_umin(&(expr->FL_ARITH));
	if (expr->FL_VALUE) free(expr->FL_VALUE);
	expr->FL_VALUE = 0;
}