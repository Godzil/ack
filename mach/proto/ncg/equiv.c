/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */
#include "assert.h"
#include "param.h"
#include "tables.h"
#include "types.h"
#include <cgg_cg.h>
#include "data.h"
#include "result.h"
#include "extern.h"
#include "salloc.h"
#include "reg.h"
#include "subr.h"
#include "utils.h"
#include "equiv.h"

int rar[MAXCREG];
rl_p *lar;
int maxindex;
int regclass[NREGS];
struct perm *perms;

struct perm *tuples(rl_p *regls, int nregneeded)
{
	int class=0;
	int i,j;
	struct reginfo *rp;

	/*
	 * First compute equivalence classes of registers.
	 */

	for (i=NREGS, rp = &machregs[NREGS-1];--i>=0;rp--) {
		regclass[i] = class++;
		if (getrefcount(i, FALSE) == 0) {
			for (j=NREGS;--j>i;) {
				if (eqregclass(i,j) &&
				    eqtoken(&rp->r_contents,
					    &machregs[j].r_contents)) {
					regclass[i] = regclass[j];
					break;
				}
			}
		}
	}

	/*
	 * Now create tuples through a recursive function
	 */

	maxindex = nregneeded;
	lar = regls;
	perms = 0;
	permute(0);
	return(perms);
}

void permute(int index)
{
	struct perm *pp;
	rl_p rlp;
	int i,j;

	if (index == maxindex) {
		for (pp=perms; pp != 0; pp=pp->p_next) {
			for (i=0; i<maxindex; i++)
				if (regclass[rar[i]] != regclass[pp->p_rar[i]])
					goto diff;
			for (i=0; i<maxindex; i++) {
				int rari = rar[i], p_rari = pp->p_rar[i];
				for (j=0; j<i; j++)
					if (clash(rari,rar[j]) !=
					    clash(p_rari,pp->p_rar[j]))
						goto diff;
			}
			return;
		    diff: ;
		}
		pp = (struct perm *) myalloc(sizeof ( *pp ));
		pp->p_next = perms;
		for (i=0; i<maxindex; i++)
			pp->p_rar[i] = rar[i];
		perms = pp;
	} else {
		rlp=lar[index];
		for (i=rlp->rl_n; i>0; i--) {
			rar[index] = rlp->rl_list[rlp->rl_n-i];
			permute(index+1);
		}
	}
}
