/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Ceriel J.H. Jacobs
 */

/* C O D E   G E N E R A T I O N   R O U T I N E S */

/* $Header$ */

/*	Code generation for expressions and coercions
*/

#include	"debug.h"

#include	<em_arith.h>
#include	<em_label.h>
#include	<em_code.h>
#include	<em_abs.h>
#include	<assert.h>
#include	<alloc.h>

#include	"type.h"
#include	"LLlex.h"
#include	"def.h"
#include	"scope.h"
#include	"desig.h"
#include	"node.h"
#include	"Lpars.h"
#include	"standards.h"
#include	"walk.h"
#include	"bigresult.h"

extern char	*long2str();
extern char	*symbol2str();
extern int	proclevel;
extern char	options[];
int		fp_used;

STATIC char *
NameOfProc(df)
	register t_def *df;
{

	assert(df->df_kind & (D_PROCHEAD|D_PROCEDURE));

	if (df->df_kind == D_PROCEDURE) {
		return df->prc_vis->sc_scope->sc_name;
	}
	return df->for_name;
}

CodeConst(cst, size)
	arith cst;
	int size;
{
	/*	Generate code to push constant "cst" with size "size"
	*/

	if (size <= (int) word_size) {
		C_loc(cst);
	}
	else if (size == (int) dword_size) {
		C_ldc(cst);
	}
	else {
		crash("(CodeConst)");
/*
		C_df_dlb(++data_label);
		C_rom_icon(long2str((long) cst), (arith) size);
		c_lae_dlb(data_label);
		C_loi((arith) size);
*/
	}
}

CodeString(nd)
	register t_node *nd;
{
	if (nd->nd_type->tp_fund != T_STRING) {
		/* Character constant */
		C_loc(nd->nd_INT);
		return;
	}
	C_df_dlb(++data_label);
	C_rom_scon(nd->nd_STR, WA((arith)(nd->nd_SLE + 1)));
	c_lae_dlb(data_label);
}

CodeExpr(nd, ds, true_label, false_label)
	register t_node *nd;
	register t_desig *ds;
	label true_label, false_label;
{
	register t_type *tp = nd->nd_type;

	DoLineno(nd);
	if (tp->tp_fund == T_REAL) fp_used = 1;
	switch(nd->nd_class) {
	case Def:
		if (nd->nd_def->df_kind & (D_PROCEDURE|D_PROCHEAD)) {
			C_lpi(NameOfProc(nd->nd_def));
			ds->dsg_kind = DSG_LOADED;
			break;
		}
		/* Fall through */

	case Link:
	case Arrsel:
	case Arrow:
		CodeDesig(nd, ds);
		break;

	case Oper:
		CodeOper(nd, true_label, false_label);
		ds->dsg_kind = DSG_LOADED;
		true_label = NO_LABEL;
		break;

	case Uoper:
		CodeUoper(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Value:
		switch(nd->nd_symb) {
		case REAL:
			C_df_dlb(++data_label);
			C_rom_fcon(nd->nd_REL, tp->tp_size);
			c_lae_dlb(data_label);
			C_loi(tp->tp_size);
			break;
		case STRING:
			CodeString(nd);
			break;
		case INTEGER:
			CodeConst(nd->nd_INT, (int) (tp->tp_size));
			break;
		default:
			crash("Value error");
		}
		ds->dsg_kind = DSG_LOADED;
		break;

	case Call:
		CodeCall(nd);
		ds->dsg_kind = DSG_LOADED;
		break;

	case Set: {
		register unsigned i = (unsigned) (tp->tp_size) / (int) word_size;
		register arith *st = nd->nd_set + i;

		ds->dsg_kind = DSG_LOADED;
		for (; i; i--) { 
			C_loc(*--st);
		}
		FreeSet(nd->nd_set);
		CodeSet(nd);
		}
		break;

	default:
		crash("(CodeExpr) bad node type");
	}

	if (true_label != NO_LABEL) {
		/* Only for boolean expressions
		*/
		CodeValue(ds, tp);
		C_zne(true_label);
		C_bra(false_label);
	}
}

CodeCoercion(t1, t2)
	register t_type *t1, *t2;
{
	register int fund1, fund2;
	arith sz1 = t1->tp_size;
	arith sz2;

	t1 = BaseType(t1);
	t2 = BaseType(t2);
	sz2 = t2->tp_size;
	switch(fund1 = t1->tp_fund) {
	case T_WORD:
		fund1 = T_INTEGER;
		break;
	case T_CHAR:
	case T_ENUMERATION:
	case T_CARDINAL:
	case T_INTORCARD:
		if (sz1 < word_size) sz1 = word_size;
		/* fall through */
	case T_EQUAL:
	case T_POINTER:
		fund1 = T_CARDINAL;
		break;
	}
	switch(fund2 = t2->tp_fund) {
	case T_WORD:
		fund2 = T_INTEGER;
		break;
	case T_CHAR:
	case T_ENUMERATION:
		sz2 = word_size;
		/* fall through */
	case T_EQUAL:
	case T_POINTER:
		fund2 = T_CARDINAL;
		break;
	}

	switch(fund1) {
	case T_INTEGER:
		if (sz1 < word_size) {
			c_loc((int)sz1);
			c_loc((int) word_size);
			C_cii();
			sz1 = word_size;
		}
		if (fund2 == T_REAL) {
			c_loc((int)sz1);
			c_loc((int)sz2);
			C_cif();
			break;
		}
		if (sz2 != sz1) {
			c_loc((int)sz1);
			c_loc((int)sz2);
			switch(fund2) {
			case T_INTEGER:
				C_cii();
				break;
			case T_CARDINAL:
				C_ciu();
				break;
			default:
				crash("Funny integer conversion");
			}
		}
		break;

	case T_CARDINAL:
	case T_INTORCARD:
		if (fund2 == T_REAL) {
			c_loc((int)sz1);
			c_loc((int)sz2);
			C_cuf();
			break;
		}
		if (sz1 != sz2) {
			c_loc((int)sz1);
			c_loc((int)sz2);
			switch(fund2) {
			case T_CARDINAL:
			case T_INTORCARD:
				C_cuu();
				break;
			case T_INTEGER:
				C_cui();
				break;
			default:
				crash("Funny cardinal conversion");
			}
		}
		break;

	case T_REAL:
		switch(fund2) {
		case T_REAL:
			if (sz1 != sz2) {
				c_loc((int)sz1);
				c_loc((int)sz2);
				C_cff();
			}
			break;
		case T_INTEGER:
			c_loc((int)sz1);
			c_loc((int)sz2);
			C_cfi();
			break;
		case T_CARDINAL:
			if (! options['R']) {
				label lb = ++text_label;

				C_dup(sz1);
				C_zrf(sz1);
				C_cmf(sz1);
				C_zge(lb);
				c_loc(ECONV);
				C_trp();
				def_ilb(lb);
			}
			c_loc((int)sz1);
			c_loc((int)sz2);
			C_cfu();
			break;
		default:
			crash("Funny REAL conversion");
		}
		break;
	}
}

CodeCall(nd)
	register t_node *nd;
{
	/*	Generate code for a procedure call. Checking of parameters
		and result is already done.
	*/
	register t_node *left = nd->nd_left;
	t_type *result_tp;
	int needs_fn;

	if (left->nd_type == std_type) {
		CodeStd(nd);
		return;
	}	

	assert(IsProc(left));

	result_tp = ResultType(left->nd_type);
#ifdef BIG_RESULT_ON_STACK
	if (result_tp && TooBigForReturnArea(result_tp)) {
		C_asp(-WA(result_tp->tp_size));
	}
#endif

	if (nd->nd_right) {
		CodeParameters(ParamList(left->nd_type), nd->nd_right);
	}

	switch(left->nd_class) {
	case Def: {
		register t_def *df = left->nd_def;

		if (df->df_kind == D_CONST) {
			df = df->con_const.tk_data.tk_def;
		}
		if (df->df_kind & (D_PROCEDURE|D_PROCHEAD)) {
			int level = df->df_scope->sc_level;

			if (level > 0) {
				C_lxl((arith) (proclevel - level));
			}
			needs_fn = df->df_scope->sc_defmodule;
			C_cal(NameOfProc(df));
			break;
		}}
		/* Fall through */
	default:
		needs_fn = 1;
		CodePExpr(left);
		C_cai();
	}
	C_asp(left->nd_type->prc_nbpar);
	if (result_tp) {
		arith sz = WA(result_tp->tp_size);
		if (TooBigForReturnArea(result_tp)) {
#ifndef BIG_RESULT_ON_STACK
			C_lfr(pointer_size);
			C_loi(sz);
#endif
		}
		else	C_lfr(sz);
	}
	DoFilename(needs_fn);
	DoLineno(nd);
}

CodeParameters(param, arg)
	t_param *param;
	t_node *arg;
{
	register t_type *tp;
	register t_node *left;
	register t_type *left_type;

	assert(param != 0 && arg != 0);

	if (param->par_next) {
		CodeParameters(param->par_next, arg->nd_right);
	}

	tp = TypeOfParam(param);
	left = arg->nd_left;
	left_type = left->nd_type;
	if (IsConformantArray(tp)) {
		register t_type *elem = tp->arr_elem;

		C_loc(tp->arr_elsize);
		if (IsConformantArray(left_type)) {
			DoHIGH(left->nd_def);
			if (elem->tp_size != left_type->arr_elem->tp_size) {
				/* This can only happen if the formal type is
				   ARRAY OF (WORD|BYTE)
				*/
				C_loc(left_type->arr_elem->tp_size);
				C_mli(word_size);
				if (elem == word_type) {
					c_loc((int) word_size - 1);
					C_adi(word_size);
					c_loc((int) word_size);
					C_dvi(word_size);
				}
				else {
					assert(elem == byte_type);
				}
			}
		}
		else if (left->nd_symb == STRING) {
			C_loc((arith)(left->nd_SLE - 1));
		}
		else if (elem == word_type) {
			C_loc((left_type->tp_size+word_size-1) / word_size - 1);
		}
		else if (elem == byte_type) {
			C_loc(left_type->tp_size - 1);
		}
		else {
			C_loc(left_type->arr_high - left_type->arr_low);
		}
		c_loc(0);
	}
	if (IsConformantArray(tp) || IsVarParam(param) || IsBigParamTp(tp)) {
		if (left->nd_symb == STRING) {
			CodeString(left);
		}
		else switch(left->nd_class) {
		case Arrsel:
		case Arrow:
		case Def:
			CodeDAddress(left, IsVarParam(param));
			break;
		default:{
			arith tmp, TmpSpace();

			CodePExpr(left);
			tmp = TmpSpace(left->nd_type->tp_size, left->nd_type->tp_align);
			STL(tmp, WA(left->nd_type->tp_size));
			C_lal(tmp);
			}
			break;
		}
		return;
	}
	if (left_type->tp_fund == T_STRING) {
		CodePString(left, tp);
		return;
	}
	CodePExpr(left);
}

CodePString(nd, tp)
	t_node *nd;
	t_type *tp;
{
	arith szarg = WA(nd->nd_type->tp_size);
	register arith zersz = WA(tp->tp_size) - szarg;

	if (zersz) {
		/* null padding required */
		assert(zersz > 0);
		C_zer(zersz);
	}
	CodeString(nd);	/* push address of string */
	C_loi(szarg);
}

static
subu(sz)
	arith sz;
{
	if (options['R']) C_sbu(sz);
	else {
		CAL((int) sz == (int) word_size ? "subu" : "subul", (int) sz);
	}
}

static
addu(sz)
	arith sz;
{
	if (options['R']) C_adu(sz);
	else {
		CAL((int) sz == (int) word_size ? "addu" : "addul", (int) sz);
	}
}

CodeStd(nd)
	t_node *nd;
{
	register t_node *arg = nd->nd_right;
	register t_node *left = 0;
	register t_type *tp = 0;
	int std = nd->nd_left->nd_def->df_value.df_stdname;

	if (arg) {
		left = arg->nd_left;
		tp = BaseType(left->nd_type);
		arg = arg->nd_right;
	}

	switch(std) {
	case S_ORD:
	case S_VAL:
		CodePExpr(left);
		break;

	case S_ABS:
		CodePExpr(left);
		if (tp->tp_fund == T_INTEGER) {
			CAL((int)(tp->tp_size) == (int)int_size ? "absi" : "absl", (int)(tp->tp_size));
		}
		else if (tp->tp_fund == T_REAL) {
			CAL((int)(tp->tp_size) == (int)float_size ? "absf" : "absd", (int)(tp->tp_size));
		}
		C_lfr(tp->tp_size);
		break;

	case S_CAP:
		CodePExpr(left);
		C_cal("cap");
		break;

	case S_HIGH:
		assert(IsConformantArray(tp));
		DoHIGH(left->nd_def);
		break;

	case S_SIZE:
	case S_TSIZE:
		assert(IsConformantArray(tp));
		DoHIGH(left->nd_def);
		C_inc();
		C_loc(tp->arr_elem->tp_size);
		C_mlu(word_size);
		break;

	case S_ODD:
		CodePExpr(left);
		if (tp->tp_size == word_size) {
			c_loc(1);
			C_and(word_size);
		}
		else {
			assert(tp->tp_size == dword_size);
			C_ldc((arith) 1);
			C_and(dword_size);
			C_ior(word_size);
		}
		break;

	case S_ADR:
		CodeDAddress(left, 1);
		break;

	case S_DEC:
	case S_INC: {
		register arith size;

		size = left->nd_type->tp_size;
		if (size < word_size) size = word_size;
		CodePExpr(left);
		CodeCoercion(left->nd_type, tp);
		if (arg) {
			CodePExpr(arg->nd_left);
			CodeCoercion(arg->nd_left->nd_type, tp);
		}
		else	{
			c_loc(1);
			CodeCoercion(intorcard_type, tp);
		}
		if (std == S_DEC) {
			if (tp->tp_fund == T_INTEGER) C_sbi(size);
			else	subu(size);
		}
		else {
			if (tp->tp_fund == T_INTEGER) C_adi(size);
			else	addu(size);
		}
		if (size == word_size) {
			RangeCheck(left->nd_type, tp->tp_fund == T_INTEGER ?
						int_type : card_type);
		}
		CodeDStore(left);
		break;
		}

	case S_HALT:
		C_cal("halt");
		break;

	case S_INCL:
	case S_EXCL:
		CodePExpr(left);
		CodePExpr(arg->nd_left);
		C_loc(tp->set_low);
		C_sbi(word_size);
		C_set(tp->tp_size);
		if (std == S_INCL) {
			C_ior(tp->tp_size);
		}
		else {
			C_com(tp->tp_size);
			C_and(tp->tp_size);
		}
		CodeDStore(left);
		break;

	default:
		crash("(CodeStd)");
	}
}

RangeCheck(tpl, tpr)
	register t_type *tpl, *tpr;
{
	/*	Generate a range check if neccessary
	*/

	arith rlo, rhi;

	if (options['R']) return;

	if (bounded(tpl)) {
		/* In this case we might need a range check.
		   If both types are restricted. check the bounds
		   to see wether we need a range check.
		   We don't need one if the range of values of the
		   right hand side is a subset of the range of values
		   of the left hand side.
		*/
		if (bounded(tpr)) {
			getbounds(tpr, &rlo, &rhi);
			if (in_range(rlo, tpl) && in_range(rhi, tpl)) {
				return;
			}
		}
		genrck(tpl);
		return;
	}
	tpr = BaseType(tpr);
	if ((tpl->tp_fund == T_INTEGER && tpr == card_type) ||
	     (tpr->tp_fund == T_INTEGER && tpl == card_type)) {
		label lb = ++text_label;

		C_dup(tpr->tp_size);
		C_zer(tpr->tp_size);
		C_cmi(tpr->tp_size);
		C_zge(lb);
		c_loc(ECONV);
		C_trp();
		def_ilb(lb);
	}
}

Operands(nd)
	register t_node *nd;
{

	CodePExpr(nd->nd_left);
	CodePExpr(nd->nd_right);
	DoLineno(nd);
}

CodeOper(expr, true_label, false_label)
	register t_node *expr;	/* the expression tree itself	*/
	label true_label;
	label false_label;	/* labels to jump to in logical expr's	*/
{
	register t_node *leftop = expr->nd_left;
	register t_node *rightop = expr->nd_right;
	register t_type *tp = expr->nd_type;

	switch (expr->nd_symb)	{
	case '+':
		Operands(expr);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_adi(tp->tp_size);
			break;
		case T_REAL:
			C_adf(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
			C_ads(rightop->nd_type->tp_size);
			break;
		case T_CARDINAL:
		case T_INTORCARD:
			addu(tp->tp_size);
			break;
		case T_SET:
			C_ior(tp->tp_size);
			break;
		default:
			crash("bad type +");
		}
		break;
	case '-':
		Operands(expr);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_sbi(tp->tp_size);
			break;
		case T_REAL:
			C_sbf(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
			if (rightop->nd_type == address_type) {
				C_sbs(tp->tp_size);
				break;
			}
			C_ngi(rightop->nd_type->tp_size);
			C_ads(rightop->nd_type->tp_size);
			break;
		case T_INTORCARD:
		case T_CARDINAL:
			subu(tp->tp_size);
			break;
		case T_SET:
			C_com(tp->tp_size);
			C_and(tp->tp_size);
			break;
		default:
			crash("bad type -");
		}
		break;
	case '*':
		Operands(expr);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_mli(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			if (options['R']) {
				C_mlu(tp->tp_size);
			}
			else {
				CAL((int)(tp->tp_size) <= (int)word_size ? "mulu" : "mulul",
				    (int)(tp->tp_size));
			}
			break;
		case T_REAL:
			C_mlf(tp->tp_size);
			break;
		case T_SET:
			C_and(tp->tp_size);
			break;
		default:
			crash("bad type *");
		}
		break;
	case '/':
		Operands(expr);
		switch (tp->tp_fund)	{
		case T_REAL:
			C_dvf(tp->tp_size);
			break;
		case T_SET:
			C_xor(tp->tp_size);
			break;
		default:
			crash("bad type /");
		}
		break;
	case DIV:
		Operands(expr);
		switch(tp->tp_fund)	{
		case T_INTEGER:
			C_dvi(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_dvu(tp->tp_size);
			break;
		default:
			crash("bad type DIV");
		}
		break;
	case MOD:
		Operands(expr);
		switch(tp->tp_fund)	{
		case T_INTEGER:
			C_rmi(tp->tp_size);
			break;
		case T_POINTER:
		case T_EQUAL:
		case T_CARDINAL:
		case T_INTORCARD:
			C_rmu(tp->tp_size);
			break;
		default:
			crash("bad type MOD");
		}
		break;
	case '<':
	case LESSEQUAL:
	case '>':
	case GREATEREQUAL:
	case '=':
	case '#':
		Operands(expr);
		tp = BaseType(leftop->nd_type);
		if (tp == intorcard_type) tp = BaseType(rightop->nd_type);
		switch (tp->tp_fund)	{
		case T_INTEGER:
			C_cmi(tp->tp_size);
			break;
		case T_POINTER:
		case T_HIDDEN:
		case T_EQUAL:
			C_cmp();
			break;
		case T_CARDINAL:
		case T_INTORCARD:
			C_cmu(tp->tp_size);
			break;
		case T_ENUMERATION:
		case T_CHAR:
			C_cmu(word_size);
			break;
		case T_REAL:
			C_cmf(tp->tp_size);
			break;
		case T_SET:
			if (expr->nd_symb == GREATEREQUAL) {
				/* A >= B is the same as A equals A + B
				*/
				C_dup(tp->tp_size << 1);
				C_asp(tp->tp_size);
				C_ior(tp->tp_size);
				expr->nd_symb = '=';
			}
			else if (expr->nd_symb == LESSEQUAL) {
				/* A <= B is the same as A - B = {}
				*/
				C_com(tp->tp_size);
				C_and(tp->tp_size);
				C_zer(tp->tp_size);
				expr->nd_symb = '=';
			}
			C_cms(tp->tp_size);
			break;
		default:
			crash("bad type COMPARE");
		}
		if (true_label != NO_LABEL)	{
			compare(expr->nd_symb, true_label);
			C_bra(false_label);
			break;
		}
		truthvalue(expr->nd_symb);
		break;

	case IN:
		/* In this case, evaluate right hand side first! The
		   INN instruction expects the bit number on top of the
		   stack
		*/
		CodePExpr(rightop);
		CodePExpr(leftop);
		C_loc(rightop->nd_type->set_low);
		C_sbu(word_size);
		C_inn(rightop->nd_type->tp_size);
		if (true_label != NO_LABEL) {
			C_zne(true_label);
			C_bra(false_label);
		}
		break;
	case OR:
	case AND: {
		label  l_maybe = ++text_label, l_end = NO_LABEL;
		t_desig *Des = new_desig();

		if (true_label == NO_LABEL)	{
			true_label = ++text_label;
			false_label = ++text_label;
			l_end = ++text_label;
		}

		if (expr->nd_symb == OR) {
			CodeExpr(leftop, Des, true_label, l_maybe);
		}
		else	CodeExpr(leftop, Des, l_maybe, false_label);
		def_ilb(l_maybe);
		clear((char *) Des, sizeof(t_desig));
		CodeExpr(rightop, Des, true_label, false_label);
		if (l_end != NO_LABEL) {
			def_ilb(true_label);
			c_loc(1);
			C_bra(l_end);
			def_ilb(false_label);
			c_loc(0);
			def_ilb(l_end);
		}
		free_desig(Des);
		break;
		}
	default:
		crash("(CodeOper) Bad operator %s\n",symbol2str(expr->nd_symb));
	}
}

/*	compare() serves as an auxiliary function of CodeOper	*/
compare(relop, lbl)
	int relop;
	register label lbl;
{
	switch (relop)	{
	case '<':
		C_zlt(lbl);
		break;
	case LESSEQUAL:
		C_zle(lbl);
		break;
	case '>':
		C_zgt(lbl);
		break;
	case GREATEREQUAL:
		C_zge(lbl);
		break;
	case '=':
		C_zeq(lbl);
		break;
	case '#':
		C_zne(lbl);
		break;
	default:
		crash("(compare)");
	}
}

/*	truthvalue() serves as an auxiliary function of CodeOper	*/
truthvalue(relop)
	int relop;
{
	switch (relop)	{
	case '<':
		C_tlt();
		break;
	case LESSEQUAL:
		C_tle();
		break;
	case '>':
		C_tgt();
		break;
	case GREATEREQUAL:
		C_tge();
		break;
	case '=':
		C_teq();
		break;
	case '#':
		C_tne();
		break;
	default:
		crash("(truthvalue)");
	}
}

CodeUoper(nd)
	register t_node *nd;
{
	register t_type *tp = nd->nd_type;

	CodePExpr(nd->nd_right);
	switch(nd->nd_symb) {
	case NOT:
		C_teq();
		break;
	case '-':
		switch(tp->tp_fund) {
		case T_INTEGER:
		case T_INTORCARD:
			C_ngi(tp->tp_size);
			break;
		case T_REAL:
			C_ngf(tp->tp_size);
			break;
		default:
			crash("Bad operand to unary -");
		}
		break;
	case COERCION:
		CodeCoercion(nd->nd_right->nd_type, tp);
		RangeCheck(tp, nd->nd_right->nd_type);
		break;
	case CAST:
		break;
	default:
		crash("Bad unary operator");
	}
}

CodeSet(nd)
	register t_node *nd;
{
	register t_type *tp = nd->nd_type;

	nd = nd->nd_right;
	while (nd) {
		assert(nd->nd_class == Link && nd->nd_symb == ',');

		if (nd->nd_left) CodeEl(nd->nd_left, tp);
		nd = nd->nd_right;
	}
}

CodeEl(nd, tp)
	register t_node *nd;
	register t_type *tp;
{
	register t_type *eltype = ElementType(tp);

	if (nd->nd_class == Link && nd->nd_symb == UPTO) {
		C_loc(tp->set_low);
		C_loc(tp->tp_size);	/* push size */
		if (eltype->tp_fund == T_SUBRANGE) {
			C_loc(eltype->sub_ub);
		}
		else	C_loc((arith) (eltype->enm_ncst - 1));
		Operands(nd);
		CAL("LtoUset", 5 * (int) word_size);
		/* library routine to fill set */
	}
	else {
		CodePExpr(nd);
		C_loc(tp->set_low);
		C_sbi(word_size);
		C_set(tp->tp_size);
		C_ior(tp->tp_size);
	}
}

CodePExpr(nd)
	register t_node *nd;
{
	/*	Generate code to push the value of the expression "nd"
		on the stack.
	*/
	register t_desig *designator = new_desig();

	CodeExpr(nd, designator, NO_LABEL, NO_LABEL);
	CodeValue(designator, nd->nd_type);
	free_desig(designator);
}

CodeDAddress(nd, chk_controlvar)
	t_node *nd;
{
	/*	Generate code to push the address of the designator "nd"
		on the stack.
	*/

	register t_desig *designator = new_desig();
	int chkptr;

	if (chk_controlvar) ChkForFOR(nd);
	CodeDesig(nd, designator);
	chkptr = designator->dsg_kind==DSG_PLOADED ||
		 designator->dsg_kind==DSG_PFIXED;
	CodeAddress(designator);

	/*	Generate dummy use of pointer, to get possible error message
		as soon as possible
	*/
	if (chkptr && ! options['R']) {
		C_dup(pointer_size);
		C_loi((arith) 1);
		C_asp(word_size);
	}
	free_desig(designator);
}

CodeDStore(nd)
	register t_node *nd;
{
	/*	Generate code to store the expression on the stack into the
		designator "nd".
	*/

	register t_desig *designator = new_desig();

	ChkForFOR(nd);
	CodeDesig(nd, designator);
	CodeStore(designator, nd->nd_type);
	free_desig(designator);
}

DoHIGH(df)
	register t_def *df;
{
	/*	Get the high index of a conformant array, indicated by "nd".
		The high index is the second field in the descriptor of
		the array, so it is easily found.
	*/
	register arith highoff;

	assert(df->df_kind == D_VARIABLE);
	assert(IsConformantArray(df->df_type));

	highoff = df->var_off		/* base address and descriptor */
		  + word_size + pointer_size;
					/* skip base and first field of
					   descriptor
					*/
	if (df->df_scope->sc_level < proclevel) {
		C_lxa((arith) (proclevel - df->df_scope->sc_level));
		C_lof(highoff);
	}
	else	C_lol(highoff);
}

#ifdef SQUEEZE
c_loc(n)
{
	C_loc((arith) n);
}

c_lae_dlb(l)
	label l;
{
	C_lae_dlb(l, (arith) 0);
}

CAL(name, ssp)
	char *name;
	int ssp;
{
	C_cal(name);
	C_asp((arith) ssp);
}
#endif