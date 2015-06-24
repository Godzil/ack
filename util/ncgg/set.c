/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "param.h"
#include "property.h"
#include "set.h"
#include "token.h"
#include "lookup.h"
#include "reg.h"
#include "error.h"
#include "subr.h"
#include "extern.h"

extern set_t l_sets[];

int setlookup(set_t s)
{
	set_p p;
	int i;
	int setno;

	for(p=l_sets;p<&l_sets[nsets];p++) {
		if (p->set_size != s.set_size)
			continue;
		for (i=0;i<SETSIZE;i++)
			if (p->set_val[i] != s.set_val[i])
				goto cont;
		return(p-l_sets);
	    cont:;
	}
	setno = NEXT(nsets,MAXSETS,"Sets");
	l_sets[setno] = s;
	return(setno);
}

void make_std_sets()
{
	set_t s;
	int i;

	for(i=0;i<SETSIZE;i++)
		s.set_val[i]=0;
	for(i=0;i<nregs+ntokens;i++)
		BIS(s.set_val,i);
	s.set_size = 0;
	allsetno = setlookup(s);
	n_set("ALL",allsetno);
}

set_t emptyset;

set_t ident_to_set(char *name)
{
	symbol *sy_p;
	int i;
	struct propinfo *pp;
	int bitno;
	set_t result;

	sy_p = lookup(name,symany,mustexist);
	switch(sy_p->sy_type) {
	default:
		error("%s is wrong kind of symbol",name);
		return(emptyset);
	case symprop:
		pp = &l_props[sy_p->sy_value.syv_propno];
		result.set_size = pp->pr_size;
		for (i=0;i<SZOFSET(MAXREGS);i++)
			result.set_val[i] = pp->pr_regset[i];
		BIS(result.set_val,0);
		for (;i<SETSIZE;i++)
			result.set_val[i] = 0;
		break;
	case symtok:
		bitno = sy_p->sy_value.syv_tokno+nregs;
		for (i=0;i<SETSIZE;i++)
			result.set_val[i] = 0;
		BIS(result.set_val,bitno);
		result.set_size = l_tokens[sy_p->sy_value.syv_tokno]->tk_size;
		break;
	case symset:
		return(l_sets[sy_p->sy_value.syv_setno]);
	}
	return(result);
}

static void checksize(set_p s)
{
	int i;
	int size = -1;

	s->set_size = 0;
	for (i = 1; i <= nregs; i++) {
		if (BIT(s->set_val, i)) {
			int sz = l_regs[i].ri_size;

			if (size == -1) size = sz;
			else if (size != sz) return;
		}
	}
	for (i = 1; i <= ntokens; i++) {
		if (BIT(s->set_val, i+nregs)) {
			int sz = l_tokens[i]->tk_size;
			if (size == -1) size = sz;
			else if (size != sz) return;
		}
	}
	if (size != -1) s->set_size = size;
}

set_t setproduct(set_t s1, set_t s2)
{
	set_t result;
	int i;

	for(i=0;i<SETSIZE;i++)
		result.set_val[i] = s1.set_val[i] & s2.set_val[i];
	checksize(&result);
	return(result);
}

set_t setsum(set_t s1, set_t s2)
{
	set_t result;
	int i;

	if (s1.set_size == s2.set_size)
		result.set_size = s1.set_size;
	else
		result.set_size = 0;
	for(i=0;i<SETSIZE;i++)
		result.set_val[i] = s1.set_val[i] | s2.set_val[i];
	return(result);
}

set_t setdiff(set_t s1, set_t s2)
{
	set_t result;
	int i;

	for(i=0;i<SETSIZE;i++)
		result.set_val[i] = s1.set_val[i] & ~ s2.set_val[i];
	/* make sure that we don't loose the lowest bit of the set, which
	   indicates that it contains registers
	*/
	for (i=1;i <= nregs; i++) {
		if (BIT(result.set_val, i)) {
			BIS(result.set_val,0);
			break;
		}
	}
	checksize(&result);
	return(result);
}
