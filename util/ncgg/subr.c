/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include <stdlib.h>
#include <string.h>
#include "param.h"
#include "reg.h"
#include "lookup.h"
#include "property.h"
#include "expr.h"
#include "set.h"
#include "varinfo.h"
#include "instruct.h"
#include "token.h"
#include "regvar.h"
#include "error.h"
#include "subr.h"
#include <cgg_cg.h>
#include "extern.h"
#include "strlookup.h"
#include "hall.h"

void n_proc(char *name)
{
	symbol *sy_p;
	extern int npatbytes;

	sy_p = lookup(name,symproc,newsymbol);
	sy_p->sy_value.syv_procoff = npatbytes + 1;
}

struct varinfo *make_erase(char *name)
{
	expr_t e;
	struct varinfo *result;

	e = ident_expr(name);
	if (e.ex_typ != TYPREG)
		error("Register name required here");
	NEW(result,struct varinfo);
	result->vi_next = VI_NULL;
	result->vi_int[0] = e.ex_index;
	return(result);
}

void n_instr(char *name, char *asname, operand *oplist, struct varinfo *eraselist, struct varinfo *cost)
{
	int instrno;
	int cc_count;
	instr_p ip;

	instrno = NEXT(ninstr,MAXINSTR,"Instructions");
	ip = &l_instr[instrno];
	ip->i_name = name;
	ip->i_asname = strlookup(asname!=0 ? asname : name);
	ip->i_nops = 0;
	ip->i_oplist = oplist;
	ip->i_erases = eraselist;
	if (cost==0) {
		ip->i_cost.ct_space = 0;
		ip->i_cost.ct_time = 0;
	} else {
		ip->i_cost.ct_space = cost->vi_int[0];
		ip->i_cost.ct_space = cost->vi_int[1];
	}
	for (cc_count=0; oplist!=0; oplist = oplist->o_next) {
		ip->i_nops++;
		if(oplist->o_adorn&AD_CC)
			cc_count++;
	}
	while (eraselist!=VI_NULL) {
		if (eraselist->vi_int[0] == -1 && cc_count)
			error("Instruction can't both set and break the condition codes");
		eraselist=eraselist->vi_next;
	}
	if (cc_count>1)
		error("No instruction can set condition codes more than once");
}

void n_set(char *name, int number)
{
	symbol *sy_p;

	sy_p = lookup(name,symset,newsymbol);
	sy_p->sy_value.syv_setno = number;
}

void n_tok(char *name,struct varinfo *atts, int size, struct varinfo *cost, struct varinfo *format)
{
	symbol *sy_p;
	token_p tp;
	struct varinfo *vip;
	int i;
	int tokno;
	int thistokensize;
	char formstr[50],smallstr[2];

	sy_p = lookup(name,symtok,newsymbol);
	NEW(tp,token_t);
	tokno = NEXT(ntokens,MAXTOKENS,"Tokens");
	sy_p->sy_value.syv_tokno = tokno;
	l_tokens[tokno] = tp;
	tp->tk_name = sy_p->sy_name;
	tp->tk_size = size;
	if (cost != 0) {
		tp->tk_cost.ct_space = cost->vi_int[0];
		tp->tk_cost.ct_time  = cost->vi_int[1];
	} else {
		tp->tk_cost.ct_space = 0;
		tp->tk_cost.ct_time  = 0;
	}
	for(i=0,vip=atts;i<MAXATT && vip!=0;i++,vip=vip->vi_next) {
		tp->tk_att[i].ta_type = vip->vi_int[0];
		tp->tk_att[i].ta_name = vip->vi_str[0];
		vip->vi_str[0]=0;
	}
	thistokensize=i;
	if (i>maxtokensize)
		maxtokensize=i;
	if (vip!=0)
		error("More then %d attributes, rest discarded",MAXATT);
	for(;i<MAXATT;i++)
		tp->tk_att[i].ta_type= -3;
	if (format!=0) {
		formstr[0] = 0;
		for (vip=format;vip!=0;vip=vip->vi_next) {
			if (vip->vi_int[0]==0)
				strcat(formstr,vip->vi_str[0]);
			else {
				for(i=0;i<thistokensize;i++) {
					if (strcmp(vip->vi_str[0],tp->tk_att[i].ta_name)==0) {
						smallstr[0] = i+1;
						smallstr[1] = 0;
						strcat(formstr,smallstr);
						break;
					}
				}
				if (i==thistokensize)
					error("%s not a known attribute",
						vip->vi_str[0]);
			}
		}
		tp->tk_format = strlookup(formstr);
	} else
		tp->tk_format = -1;
}

void checkprintformat(int n)
{
	short *s;
	int i;
	extern set_t l_sets[];
	
	s= l_sets[n].set_val;
	for(i=nregs;i<nregs+ntokens;i++)
		if (BIT(s,i) && l_tokens[i-nregs]->tk_format<0)
			error("Token %s in set does not have printformat",
				l_tokens[i-nregs]->tk_name);
}

void n_prop(char *name, int size)
{
	int propno;
	symbol *sp;

	propno = NEXT(nprops,MAXPROPS,"Properties");
	sp = lookup(name,symprop,newsymbol);
	sp->sy_value.syv_propno = propno;
	if (size <= 0) {
		error("Size of property must be >0");
		size = wordsize;
	}
	l_props[propno].pr_size = size;
}

void prophall(int n) {
	int i;
	short hallset[SETSIZE];
	
	if (n < 0) return;
	for(i=0;i<SETSIZE;i++)
		hallset[i] = i<SZOFSET(MAXREGS) ? l_props[n].pr_regset[i] : 0;
	nexthall(hallset);
}

int n_reg(char *name, char *printstring, int nmemb, int member1, int member2)
{
	symbol *sy_p;
	reginfo *ri_p;
	int regno;

	sy_p = lookup(name,symreg,newsymbol);
	sy_p->sy_value.syv_regno = regno = NEXT(nregs,MAXREGS,"Number of registers");
	ri_p = &l_regs[regno];
	ri_p->ri_name = mystrcpy(name);
	ri_p->ri_repr = printstring!=0 ? mystrcpy(printstring) : ri_p->ri_name;
	ri_p->ri_memb[0] = member1;
	ri_p->ri_memb[1] = member2;
	if (nmemb>maxmembers)
		maxmembers=nmemb;
	return(regno);
}

void make_const()
{
	wordsize = cmustbeset("EM_WSIZE");
	pointersize = cmustbeset("EM_PSIZE");
}

int cmustbeset(char *ident)
{
	return(lookup(ident,symconst,mustexist)->sy_value.syv_cstval);
}

void n_const(char *ident, int val)
{
	symbol *sy_p;

	sy_p = lookup(ident,symconst,newsymbol);
	sy_p->sy_value.syv_cstval = val;
}

void n_sconst(char *ident, char *val)
{
	symbol *sy_p;

	sy_p = lookup(ident,symsconst,newsymbol);
	sy_p->sy_value.syv_stringno = strlookup(val);
}

void regline(varinfo *rl, varinfo *pl, int rv)
{
	varinfo *rrl,*rpl;
	short *sp;
	reginfo *regp;
	int thissize;
	int propno;

	for(rrl=rl;rrl!=0;rrl=rrl->vi_next) {
		regp = &l_regs[rrl->vi_int[0]];
		thissize = 0;
		for(rpl=pl;rpl!=0;rpl=rpl->vi_next) {
			propno = rpl->vi_int[0];
			sp= l_props[propno].pr_regset;
			BIS(sp,rrl->vi_int[0]);
			if (thissize==0)
				thissize = l_props[propno].pr_size;
			else if (thissize!=-1 && thissize!=l_props[propno].pr_size)
				error("Register %s has no clear size",
					regp->ri_name);
		}
		regp->ri_size = thissize;
		regp->ri_class = regclass;
		regp->ri_rregvar = rv;
		if (rv>=0) {
			if (regp->ri_memb[0]!=0)
				error("Register variables may not have subregisters");
			rvused |= ANY_REGVAR;
			if (regp->ri_size == wordsize)
				rvused |= SL_REGVAR;
			else if (regp->ri_size == 2*wordsize)
				rvused |= DL_REGVAR;
			if (nregvar[rv]==0)
				rvsize[rv] = regp->ri_size;
			else if (rvsize[rv]!=regp->ri_size)
				error("All variables of one type must have the same size");
			NEXT(nregvar[rv],MAXREGVAR,"Register variable");
			rvnumbers[rv][nregvar[rv]-1] = rrl->vi_int[0];
		}
	}
	regclass++;
}

void setallreg(struct varinfo *vi)
{
	nallreg=0;
	for(;vi!=0;vi=vi->vi_next) {
		if (vi->vi_int[0]<0)
			continue;
		allreg[nallreg++] = vi->vi_int[0];
	}
}

void freevi(struct varinfo *vip)
{
	int i;

	if (vip==0)
		return;
	freevi(vip->vi_next);
	freevi(vip->vi_vi);
	for (i=0;i<VI_NSTR;i++)
		free((char *) vip->vi_str[i]);
	free(vip);
}

int myatoi(char *s)
{
	int base=10;
	int sum=0;

	if (*s=='0') {
		base = 8;
		s++;
		if (*s=='x') {
			base=16;
			s++;
		}
	}
	for (;;) {
		switch (*s) {
		default:	return(sum);
		case '8':
		case '9':
			if (base==8) error("Bad digit in octal number");
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			sum = sum*base + *s++ - '0';
			break;
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
			if (base!=16) error("Hexletter in number not expected");
			sum = sum*base + 10 + *s++ - 'a';
			break;
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			if (base!=16) error("Hexletter in number not expected");
			sum = sum*base + 10 + *s++ - 'A';
			break;
		}
	}
}

char *mystrcpy(char *s)
{
	char *p;

	p=myalloc(strlen(s)+1);
	strcpy(p,s);
	return(p);
}

char *myalloc(int n)
{
	char *p,*result;

	result=p=malloc(n);
	if (p== (char *) 0)
		fatal("Out of memory");
	do *p++=0; while (--n);
	return(result);
}

int chkincl(int value, int lwb, int upb)
{
	if (value<lwb || value>upb)
		error("Number %d should have been between %d and %d",
			value,lwb,upb);
	return(value);
}

int subset(short *sp1, short *sp2, int setsize)
{
	int i;

	for(i=0;i<setsize;i++)
		if ( (sp1[i] | sp2[i]) != sp2[i])
			return(0);
	return(1);
}

int vilength(struct varinfo *vip)
{
	int l=0;

	while(vip!=0) {
		vip=vip->vi_next;
		l++;
	}
	return(l);
}
