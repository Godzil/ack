/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef NORCSID
static char rcsid[]= "$Id$";
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "param.h"
#include "set.h"
#include "reg.h"
#include "lookup.h"
#include "token.h"
#include "property.h"
#include "expr.h"
#include "regvar.h"
#include <cgg_cg.h>
#include "extern.h"

extern set_t l_sets[];

void subregset(short *sp, int subreg, short *regset);

int i_expr(expr_t e)
{
	if (e.ex_typ != TYPINT)
		error("Expression should be integer");
	return(e.ex_index);
}

int b_expr(expr_t e)
{
	if (e.ex_typ != TYPBOOL)
		error("Expression should be boolean");
	return(e.ex_index);
}

expr_t make_expr(int type, int operator, int op1, int op2)
{
	expr_t result;

	result.ex_typ=type;
	result.ex_index=ex_lookup(operator,op1,op2);
	return(result);
}

expr_t regno_expr(int regno)
{
	expr_t result;
	int i;

	result.ex_typ = TYPREG;
	result.ex_index = ex_lookup(EX_REG,regno,0);
	for (i=0;i<SZOFSET(MAXREGS);i++)
		result.ex_regset[i] = 0;
	BIS(result.ex_regset,regno);
	return(result);
}

expr_t ident_expr(char *name)
{
	symbol *sy_p;

	sy_p = lookup(name,symany,mustexist);
	if (sy_p->sy_type==symconst)
		return(make_expr(TYPINT,EX_CON,
			   (int) (sy_p->sy_value.syv_cstval&0xFFFF),
			   (int) (sy_p->sy_value.syv_cstval>>16)));
	else if (sy_p->sy_type==symsconst)
		return(make_expr(TYPADDR,EX_STRING,sy_p->sy_value.syv_stringno,0));
	else if (sy_p->sy_type!=symreg)
		error("Wrong type of identifier %s",name);
	return(regno_expr(sy_p->sy_value.syv_regno));
}

expr_t subreg_expr(int tokarg, int subreg)
{
	expr_t result;

	result.ex_typ = TYPREG;
	subregset(l_sets[tokpatset[tokarg > 0 ? tokarg-1 : tokarg]].set_val,
		  subreg,result.ex_regset);
	result.ex_index = ex_lookup(EX_SUBREG,tokarg,subreg);
	return(result);
}

void subregset(short *sp, int subreg, short *regset)
{
	int i;
	reginfo *rp;

	for (i=0;i<SZOFSET(MAXREGS);i++)
		regset[i]=0;
	for (i=1;i<nregs;i++)	if (BIT(sp,i)) {
		if(subreg) {
			rp = &l_regs[i];
			if (rp->ri_memb[subreg-1]==0)
				error("Register %s in set has no member %d",
					rp->ri_name,subreg);
			BIS(regset,rp->ri_memb[subreg-1]);
		} else
			BIS(regset,i);
	}
	for(;i<nregs+MAXTOKENS;i++) if(BIT(sp,i))
		error("Set contains %s, which is not a register",
			l_tokens[i-nregs]->tk_name);
}

int membset(int setno, char *name, short *regset, char *appearance, int restyp, int *typp)
{
	register short *sp;
	register token_p tp;
	register i,j,k;
	int thistyp;
	int typesdiffer=0;
	int res_j= -1;

	if (setno < 0) return 0;
	sp = l_sets[setno].set_val;
	for (i=1;i<nregs;i++) if (BIT(sp,i)) {
		error("Set in %s contains %s, which is not a token",
			appearance,l_regs[i].ri_name);
		break;
	}
	for (i=0;i<SZOFSET(MAXREGS);i++)
		regset[i] = 0;
	for (i=nregs;i<nregs+MAXTOKENS;i++) if (BIT(sp,i)) {
		tp = l_tokens[i-nregs];
		for(j=0;j<MAXATT &&
		        (tp->tk_att[j].ta_type == -3 ||
			 strcmp(tp->tk_att[j].ta_name,name));j++)
			;
		if (j==MAXATT)
			error("Token %s does not contain %s",tp->tk_name,name);
		else if (j!=res_j && res_j != -1)
			typesdiffer=1;
		else {
			res_j = j;
			thistyp = tp->tk_att[j].ta_type;
			if (thistyp== -2) {
				if (restyp!=TYPADDR && restyp!=0)
					typesdiffer=1;
				else
					restyp=TYPADDR;
			} else if (thistyp== -1) {
				if (restyp!=TYPINT && restyp!=0)
					typesdiffer=1;
				else
					restyp=TYPINT;
			} else {
				if (restyp!=TYPREG && restyp!=0)
					typesdiffer=1;
				else {
					restyp=TYPREG;
					for(k=0;k<SZOFSET(MAXREGS);k++)
						regset[k] |=
						    l_props[tp->tk_att[j].ta_type].pr_regset[k];
				}
			}
		}
	}
	if (typesdiffer)
		error("%s is not a valid expression; types differ in the set",
			appearance);
	*typp = restyp==0 ? TYPINT : restyp;
	return(res_j == -1 ? 0 : res_j);
}

expr_t memb_expr(int setno, char *name, char *appearance, int tokarg)
{
	expr_t result;
	int res_j;

	res_j = membset(setno,name,result.ex_regset,appearance,0,&result.ex_typ);
	result.ex_index = ex_lookup(EX_TOKFIELD,tokarg,res_j+1);
	return(result);
}

expr_t tokm_expr(int tokarg, char *name)
{
	char app[100];
	int tokarg1 = tokarg > 0 ? tokarg : 1;

	sprintf(app,"%%%d.%s",tokarg1,name);
	return(memb_expr(tokpatset[tokarg1-1],name,app,tokarg));
}

expr_t perc_ident_expr(char *name)
{
	char app[100];

	sprintf(app,"%%%s",name);
	return(memb_expr(cursetno,name,app,0));
}

expr_t all_expr(int all_no, int subreg)
{
	set_t localset;
	int i;
	short *sp;
	expr_t result;

	sp = l_props[allreg[all_no]].pr_regset;
	for (i=0;i<SETSIZE;i++)
		localset.set_val[i] = i<SZOFSET(MAXREGS) ? sp[i] : 0;
	subregset(localset.set_val,subreg,result.ex_regset);
	result.ex_typ = TYPREG;
	result.ex_index = ex_lookup(EX_ALLREG,all_no+1,subreg);
	return(result);
}

int eq2expr(expr_t e1, expr_t e2)
{
	if (e1.ex_typ != e2.ex_typ)
		error("Expressions around == should have equal type");
	switch (e1.ex_typ) {
	default: assert(0);
	case TYPBOOL:
		error("== on booleans not implemented");
	case TYPINT:
		return(EX_NCPEQ);
	case TYPADDR:
		return(EX_SCPEQ);
	case TYPREG:
		return(EX_RCPEQ);
	}
}

int ne2expr(expr_t e1, expr_t e2)
{
	if (e1.ex_typ != e2.ex_typ)
		error("Expressions around != should have equal type");
	switch (e1.ex_typ) {
	default: assert(0);
	case TYPBOOL:
		error("!= on booleans not implemented");
	case TYPINT:
		return(EX_NCPNE);
	case TYPADDR:
		return(EX_SCPNE);
	case TYPREG:
		return(EX_RCPNE);
	}
}

expr_t sum_expr(expr_t e1, expr_t e2)
{
	int operator,op1,op2;
	expr_t result;

	operator = EX_CAT; op1 = e1.ex_index; op2 = e2.ex_index;
	if (e1.ex_typ==e2.ex_typ) {
		result.ex_typ = e1.ex_typ;
		if (e1.ex_typ == TYPINT)
			operator = EX_PLUS;
		else if (e1.ex_typ != TYPADDR)
			error("+ is not implemented on this type");
	} else {
		result.ex_typ = TYPADDR;
		if (e1.ex_typ != TYPADDR) {
			if (e1.ex_typ!=TYPINT)
				error("Wrong left operand of +");
			op1 = ex_lookup(EX_TOSTRING,op1,0);
		}
		if (e2.ex_typ != TYPADDR) {
			if (e2.ex_typ!=TYPINT)
				error("Wrong right operand of +");
			op2 = ex_lookup(EX_TOSTRING,op2,0);
		}
	}
	result.ex_index=ex_lookup(operator,op1,op2);
	return(result);
}

expr_t iextoaddr(expr_t e)
{
	expr_t result;

	result.ex_typ = TYPADDR;
	result.ex_index = ex_lookup(EX_TOSTRING,e.ex_index,0);
	return(result);
}

expr_t regvar_expr(expr_t e, int regtyp)
{
	expr_t result;
	int i;
	
	result = make_expr(TYPREG,EX_REGVAR,i_expr(e),0);
	for(i=0;i<SZOFSET(MAXREGS);i++)
		result.ex_regset[i]=0;
	for(i=0;i<nregvar[regtyp];i++)
		BIS(result.ex_regset,rvnumbers[regtyp][i]);
	return(result);
}

/*
 * Node table lookup part
 */

node_t nodes[MAXNODES];
int nnodes=0;

void initnodes()
{
	nodes[0].ex_operator = EX_CON;
	nodes[0].ex_lnode = 0;
	nodes[0].ex_rnode = 0;
	nnodes++;
}

int ex_lookup(int operator, int lnode, int rnode)
{
	register node_p p;

	for(p=nodes+1;p< &nodes[nnodes];p++) {
		if (p->ex_operator != operator)
			continue;
		if (p->ex_lnode != lnode)
			continue;
		if (p->ex_rnode != rnode)
			continue;
		return(p-nodes);
	}
	NEXT(nnodes,MAXNODES,"Node");
	p->ex_operator = operator;
	p->ex_lnode = lnode;
	p->ex_rnode = rnode;
	return(p-nodes);
}
