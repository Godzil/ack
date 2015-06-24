/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* P U T . C */

#include <stdio.h>
#include <stdlib.h>

#include <em_pseu.h>
#include <em_spec.h>
#include "types.h"
#include "global.h"
#include "debug.h"
#include "def.h"
#include "map.h"
#include "lset.h"
#include "alloc.h"
#include "put.h"
#include "cset.h"

FILE *curoutp;


/* The output can be either 'typed' or 'untyped'. Typed data
 * consists of a value preceded by a byte specifying what kind
 * of value it is (e.g. 2 bytes constant, 4 bytes constant,
 * proc-id, lab-id, string etc.). Untyped data consists
 * of the value only. We use typed data for the EM text and
 * untyped data for all other files.
 */

/* putlines */

static void putstr(argb_p abp);
static void outlab(lab_id lid);
static void outobject(obj_p obj);
static void putstr(argb_p abp);
static void putstr(argb_p abp);

static void putargs(arg_p ap)
{
	while (ap != (arg_p) 0) {
		outbyte((byte) ap->a_type & BMASK);
		switch(ap->a_type) {
			case ARGOFF:
				outoff(ap->a_a.a_offset);
				break;
			case ARGINSTRLAB:
				outlab(ap->a_a.a_instrlab);
				break;
			case ARGOBJECT:
				outobject(ap->a_a.a_obj);
				break;
			case ARGPROC:
				outproc(ap->a_a.a_proc);
				break;
			case ARGSTRING:
				putstr(&ap->a_a.a_string);
				break;
			case ARGICN:
			case ARGUCN:
			case ARGFCN:
				outshort(ap->a_a.a_con.ac_length);
				putstr(&ap->a_a.a_con.ac_con);
				break;
		}
		ap = ap->a_next;
	}
	outbyte((byte) ARGCEND);
}



static void putstr(argb_p abp)
{
	argb_p tbp;
	int length;

	length = 0;
	tbp = abp;
	while (tbp!= (argb_p) 0) {
		length += tbp->ab_index;
		tbp = tbp->ab_next;
	}
	outshort(length);
	while (abp != (argb_p) 0) {
		for (length=0;length<abp->ab_index;length++)
			outbyte( (byte) abp->ab_contents[length] );
		abp = abp->ab_next;
	}
}


void outoff(offset off)
{
	outshort( (short) (off&0177777L) );
	outshort( (short) (off>>16) );
}


void outshort(short i)
{
	outbyte( (byte) (i&BMASK) );
	outbyte( (byte) (i>>8) );
}


static void outint(int i)
{
	/* Write an integer to the output file. This routine is
	 * only used when outputting a bitvector-set. We expect  an
	 * integer to be either a short or a long.
	 */

	if (sizeof(int) == sizeof(short)) {
		outshort(i);
	} else {
		assert (sizeof(int) == sizeof(offset));
		outoff((offset) i);
	}
}

static void outlab(lab_id lid)
{
	outshort((short) lid);
}


static void outobject(obj_p obj)
{
	outshort((short) obj->o_id);
}


void outproc(proc_p p)
{
	outshort((short) p->p_id);
}


short putlines(line_p l, FILE *lf)
{
	/* Output the list of em instructions headed by l.
	 * Return the number of instruction written.
	 */

	line_p lnp;
	line_p next;
	short instr;
	short count= 0;

	curoutp = lf;	/* Set f to the EM-text output file */
	for (lnp = l; lnp != (line_p) 0; lnp = next) {
		VL(lnp);
		count++;
		next = lnp->l_next;
		instr = INSTR(lnp);
		outbyte((byte) instr);
		outbyte((byte) TYPE(lnp));
		switch(TYPE(lnp)) {
			case OPSHORT:
				outshort(SHORT(lnp));
				break;
			case OPOFFSET:
				outoff(OFFSET(lnp));
				break;
			case OPINSTRLAB:
				outlab(INSTRLAB(lnp));
				break;
			case OPOBJECT:
				outobject(OBJ(lnp));
				break;
			case OPPROC:
				outproc(PROC(lnp));
				break;
			case OPLIST:
				putargs(ARG(lnp));
				break;
		}
		oldline(lnp);
	}
	return count;
}


/* putdtable */

#define outmark(m)	outbyte((byte) m)


static void putobjects(obj_p obj)
{
	while (obj != (obj_p) 0) {
		outmark(MARK_OBJ);
		outshort(obj->o_id);
		outoff(obj->o_size);
		outoff(obj->o_off);
		obj = obj->o_next;
	}
}



static void putvalues(arg_p arg)
{
	while (arg != (arg_p) 0) {
		assert(arg->a_type == ARGOFF);
		outmark(MARK_ARG);
		outoff(arg->a_a.a_offset);
		arg = arg->a_next;
	}
}

void putdtable(dblock_p head, FILE *df)
{
	/* Write the datablock table to the data block file df. */

	dblock_p dbl;
	obj_p obj;
	dblock_p next;
	short n = 0;

	curoutp = df;	    /* set f to the data block output file */
	/* Count the number of objects */
	for (dbl = head; dbl != (dblock_p) 0; dbl = dbl->d_next) {
		for (obj = dbl->d_objlist; obj != (obj_p) 0;
						obj = obj->o_next) {
			n++;
		}
	}
	outshort(n);  /* The table is preceded by #objects . */
	for (dbl = head; dbl != (dblock_p) 0; dbl = next) {
		next = dbl->d_next;
		outmark(MARK_DBLOCK);
		outshort(dbl->d_id);
		outbyte(dbl->d_pseudo);
		outoff(dbl->d_size);
		outshort(dbl->d_fragmnr);
		outbyte(dbl->d_flags1);
		putobjects(dbl->d_objlist);
		putvalues(dbl->d_values);
		olddblock(dbl);
	}
	fclose(curoutp);
	if (omap != (obj_p *) 0) {
		oldmap((short **) omap,olength);  /* release memory for omap */
	}
}



/* putptable */
static void outcset(cset s)
{
	/* A 'compact' set is represented externally as a row of words
	 * (its bitvector) preceded by its length.
	 */

	short i;

	outshort(s->v_size);
	for (i = 0; i <= DIVWL(s->v_size - 1); i++) {
		outint(s->v_bits[i]);
	}
}



void putptable(proc_p head, FILE *pf, bool all)
{
	proc_p p;
	proc_p next;
	short n = 0;
	/* Write the proc table */

	curoutp = pf;
	/* Determine the number of procs */
	for (p = head; p != (proc_p) 0; p = p->p_next) {
		n++;
	}
	outshort(n);  /* The table is preceded by its length. */
	outshort ((all?1:0)); /* if all=false, only some of the attributes
			         are written. */
	for (p = head; p != (proc_p) 0; p = next) {
		next = p->p_next;
		outshort(p->p_id);
		outbyte(p->p_flags1);
		if (p->p_flags1 & PF_BODYSEEN) {
			/* If we have no access to the EM text of the
			 * body of a procedure, we have no information
			 * about it whatsoever, so there is nothing
			 * to output in that case.
			 */
			outshort(p->p_nrlabels);
			outoff(p->p_localbytes);
			outoff(p->p_nrformals);
			if (all) {
				outcset(p->p_change->c_ext);
				outshort(p->p_change->c_flags);
				outshort(p->p_use->u_flags);
				outcset(p->p_calling);
				Cdeleteset(p->p_change->c_ext);
				oldchange(p->p_change);
				olduse(p->p_use);
				Cdeleteset(p->p_calling);
			}
		}
		oldproc(p);
	}
	fclose(curoutp);
	if (pmap != (proc_p *) 0) {
		oldmap((short **) pmap,plength);  /* release memory for pmap */
	}
}



/* putunit */

static void outloop(Lelem_t param)
{
	loop_p l = (loop_p)param;
	outshort((short) l->lp_id);
}


static void outblock(Lelem_t param)
{
	bblock_p b = (bblock_p)param;

	if (b == (bblock_p) 0) {
		outshort((short) 0);
	} else {
		outshort((short) b->b_id);
	}
}


static void outid(Lelem_t e, void (*p)(Lelem_t))
{
	/* Auxiliary routine used by outlset. */

	/* NOSTRICT */
	(*p) (e);
}


static void outlset(lset s, void (*p)(Lelem_t))
{
	/* A 'long' set is represented externally as a
	 * a sequence of elements terminated by a 0 word.
	 * The procedural parameter p is a routine that
	 * prints an id (proc_id, obj_id etc.).
	 */

	Lindex i;

	for (i = Lfirst(s); i != (Lindex) 0; i = Lnext(i,s)) {
		outid(Lelem(i),p);
	}
	outshort((short) 0);
}



void putunit(short kind, proc_p p, line_p l, FILE *gf, FILE *lf)
{
	bblock_p b;
	short n = 0;
	Lindex   pi;
	loop_p   lp;

	curoutp = gf;
	if (kind == LDATA) {
		outshort(0); /* No basic blocks */
		n = putlines(l,lf);
		curoutp = gf;
		outshort(n);
		return;
	}
	/* Determine the number of basic blocks */
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		n++;
	}
	outshort(n); /* # basic blocks */
	outshort(Lnrelems(p->p_loops));  /* # loops */
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		n = putlines(b->b_start,lf);
		curoutp = gf;
		outblock(b);  /* put its block_id */
		outshort(n);  /* #instructions of the block */
		outlset(b->b_succ, outblock); /* put succ set */
		outlset(b->b_pred, outblock); /* put pred set */
		outblock(b->b_idom); /* put id of immediate dominator */
		outlset(b->b_loops, outloop); /* put loop set */
		outshort(b->b_flags);
	}
	/* The Control Flow Graph of every procedure is followed
	 * by a description of the loops of the procedure.
	 * Every loop contains an id, an entry block and a level.
	 */
	for (pi = Lfirst(p->p_loops); pi != (Lindex) 0;
					 pi = Lnext(pi,p->p_loops)) {
		lp = (loop_p) Lelem(pi);
		outloop(lp);	/* id */
		outshort(lp->lp_level); /* nesting level */
		outblock(lp->lp_entry); /* loop entry block */
		outblock(lp->lp_end);
		oldloop(lp);
	}
	Ldeleteset(p->p_loops);
	/* We will now release the memory of the basic blocks.
	 * Note that it would be incorrect to release a basic block
	 * after it has been written, because there may be references
	 * to it from other (later) blocks.
	 */
	for (b = p->p_start; b != (bblock_p) 0; b = b->b_next) {
		Ldeleteset(b->b_loops);
		Ldeleteset(b->b_succ);
		Ldeleteset(b->b_pred);
		oldbblock(b);
	}
	/* Release the memory for the lmap, lbmap, bmap, lpmap tables */
	if (lmap != (line_p *) 0) oldmap((short **) lmap,llength);
	if (lbmap != (bblock_p *) 0) oldmap((short **) lbmap,llength);
	if (bmap != (bblock_p *) 0)  oldmap((short **) bmap,blength);
	if (lpmap != (loop_p *) 0) oldmap((short **) lpmap,lplength);
	curoutp = lf;
}
