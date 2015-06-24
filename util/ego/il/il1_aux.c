/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ A U X . C
 */

#include <stdio.h>
#include <em_spec.h>
#include "../share/types.h"
#include "il.h"
#include "../share/debug.h"
#include "../share/alloc.h"
#include "../share/global.h"
#include "../share/lset.h"
#include "il_aux.h"
#include "il1_aux.h"

#define USE_INDIR(p)	(p->p_use->u_flags & UF_INDIR)

#define IS_INSTR(c)	(c >= sp_fmnem && c <= sp_lmnem)


bool same_size(int t1, int t2)
{
	/* See if the two types have the same size */

	return tsize(t1) == tsize(t2);
}



static bool is_reg(offset off, int s)
{
	/* See if there is a register message
	 * for the local or parameter at offset off
	 * and size s.
	 */

	Lindex i;
	arg_p arg;

	for (i = Lfirst(mesregs); i != (Lindex) 0; i = Lnext(i,mesregs)) {
		arg = ((line_p) Lelem(i))->l_a.la_arg->a_next;
		if (arg->a_a.a_offset == off &&
		    arg->a_next->a_a.a_offset == s) {
			return TRUE;
		}
	}
	return FALSE;
}


void rem_actuals(actual_p acts)
{
	/* remove the actual-list */

	actual_p a,next;

	for (a = acts; a != (actual_p) 0; a = next) {
		next = a->ac_next;
		/* REMOVE CODE OF a->ac_exp HERE */
		oldactual(a);
	}
}



void remov_formals(proc_p p)
{
	/* Remove the list of formals of p */

	formal_p f, next;

	for (f = p->P_FORMALS; f != (formal_p) 0; f = next) {
		next = f->f_next;
		oldformal(f);
	}
	p->P_FORMALS = (formal_p) 0;
}



void rem_indir_acc(proc_p p)
{
	/* Formals that may be accessed indirectly
	 * cannot be expanded in line, so they are
	 * removed from the formals list.
	 */

	formal_p prev, f, next;

	if (!USE_INDIR(p) && !CHANGE_INDIR(p)) return;
	/* Any formal for which we don't have
	 * a register message is now doomed.
	 */
	prev = (formal_p) 0;
	for (f = p->P_FORMALS; f != (formal_p) 0; f = next) {
		next = f->f_next;
		if (!is_reg(f->f_offset,tsize(f->f_type))) {
			if (prev == (formal_p) 0) {
				p->P_FORMALS = next;
			} else {
				prev->f_next = next;
			}
			oldformal(f);
		}
	}
}



bool par_overlap(offset off1, int t1, offset off2, int t2)
{
	/* See if the parameter at offset off1 and type t1
	 * overlaps the paramete at offset off2 and type t2.
	 */

	if (off1 > off2) {
		return off2 + tsize(t2) > off1;
	} else {
		if (off2 > off1) {
			return off1 + tsize(t1) > off2;
		} else {
			return TRUE;
		}
	}
}



short looplevel(bblock_p b)
{
	/* determine the loop nesting level of basic block b;
	 * this is the highest nesting level of all blocks
	 * that b is part of.
	 * Note that the level of a loop is 0 for outer loops,
	 * so a block inside a loop with nesting level N has
	 * looplevel N+1.
	 */

	Lindex i;
	short max = 0;

	for (i = Lfirst(b->b_loops); i != (Lindex)0; i = Lnext(i,b->b_loops)) {
		if (((loop_p) Lelem(i))->lp_level >= max) {
			max = ((loop_p) Lelem(i))->lp_level + 1;
		}
	}
	return max;
}



int proclength(proc_p p)
{
	/* count the number of EM instructions of p */

	int cnt;
	bblock_p b;
	line_p l;

	cnt = 0;
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		for (l = b->b_start; l != (line_p) 0; l = l->l_next) {
			if (IS_INSTR(INSTR(l))) {
				/* skip pseudo instructions */
				cnt++;
			}
		}
	}
	return cnt;
}





line_p copy_code(line_p l1, line_p l2)
{
	/* copy the code between l1 and l2 */

	line_p head, tail, l, lnp;

	head = (line_p) 0;
	for (lnp = l1; ; lnp = lnp->l_next) {
		l = duplicate(lnp);
		if (head == (line_p) 0) {
			head = tail = l;
			PREV(l) = (line_p) 0;
		} else {
			tail->l_next = l;
			PREV(l) = tail;
			tail = l;
		}
		if (lnp == l2) break;
	}
	return head;
}
