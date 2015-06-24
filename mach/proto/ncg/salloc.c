/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

/*
 * Package for string allocation and garbage collection.
 * Call salloc(size) to get room for string.
 * Every now and then call garbage_collect() from toplevel.
 */
#include <stdlib.h>
#include "assert.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "extern.h"
#include "utils.h"

#include "salloc.h"

#define MAXSTAB         1500
#define THRESHOLD       200

char *stab[MAXSTAB];
int nstab=0;

string myalloc(int size)
{
	string p;

	p = (string) malloc((unsigned)size);
	if (p==0)
		fatal("Out of memory");
	return(p);
}

void myfree(string p)
{
	free(p);
}

void popstr(int nnstab)
{
	int i;

	for (i=nnstab;i<nstab;i++)
		myfree(stab[i]);
	nstab = nnstab;
}

char *salloc(int size)
{
	char *p;

	if (nstab==MAXSTAB)
		fatal("String table overflow");
	p = myalloc(size+1);    /* extra room for terminating zero */
	stab[nstab++] = p;
	return(p);
}

int compar(const void *vp1, const void *vp2)
{
	char **p1 = (char **)vp1;
	char **p2 = (char **)vp2;
	assert(*p1 != *p2);
	if (*p1 < *p2)
		return(-1);
	return(1);
}

void garbage_collect()
{
	int i;
	struct emline *emlp;
	token_p tp;
	tkdef_p tdp;
	struct reginfo *rp;
	char **fillp,**scanp;
	char used[MAXSTAB];     /* could be bitarray */

	if (nstab<THRESHOLD)
		return;
	qsort((char *)stab,nstab,sizeof (char *),compar);
	for (i=0;i<nstab;i++)
		used[i]= FALSE;
	for(emlp=emlines;emlp<emlines+nemlines;emlp++)
		chkstr(emlp->em_soper,used);
	for (tp= fakestack;tp<&fakestack[stackheight];tp++) {
		if (tp->t_token== -1)
			continue;
		tdp = &tokens[tp->t_token];
		for (i=0;i<TOKENSIZE;i++)
			if (tdp->t_type[i] == EV_ADDR)
				chkstr(tp->t_att[i].aa.ea_str,used);
	}
	for (rp= machregs+1; rp<machregs+NREGS; rp++) {
		tp = &rp->r_contents;
		assert(tp->t_token != -1);
		tdp= &tokens[tp->t_token];
		for (i=0;i<TOKENSIZE;i++)
			if (tdp->t_type[i] == EV_ADDR)
				chkstr(tp->t_att[i].aa.ea_str,used);
	}
	for (i=0;i<nstab;i++)
		if (!used[i]) {
			myfree(stab[i]);
			stab[i]=0;
		}
	fillp=stab;
	for (scanp=stab;scanp<stab+nstab;scanp++)
		if (*scanp != 0)
			*fillp++ = *scanp;
	nstab = fillp-stab;
}

void chkstr(string str, char used[])
{
	int low,middle,high;

	low=0; high=nstab-1;
	while (high>low) {
		middle= (low+high)>>1;
		if (str==stab[middle]) {
			used[middle]=1;
			return;
		}
		if (str<stab[middle])
			high = middle-1;
		else
			low = middle+1;
	}
	if (low==high) {
		if (str==stab[low]) {
			used[low]=1;
		}
		return;
	}
}
