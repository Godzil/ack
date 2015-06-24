#ifndef NORCSID
static char rcsid[] = "$Id$";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "param.h"
#include "types.h"
#include "tes.h"
#include "assert.h"
#include "alloc.h"
#include "line.h"
#include "lookup.h"
#include "proinf.h"

/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Staveren
 */

#ifdef USEMALLOC

short *myalloc(int size);

#define newcore(size) myalloc(size)
#define oldcore(p,size) free(p)

#else

#undef CORECHECK	/* if defined tests are made to insure
			   each block occurs at most once */

#define CCHUNK	1024	/* number of shorts asked from system */

short *newcore(int size);
//short *freshcore();

#ifdef COREDEBUG
int shortsasked=0;
#endif

#endif

/*
 * The following two sizetables contain the sizes of the various kinds
 * of line and argument structures.
 * Care has been taken to make this table implementation independent,
 * but if you think very hard you might find a compiler failing the
 * assumptions made.
 * A wasteful but safe approach is to replace every line of them by
 *  sizeof(line_t)
 * and
 *  sizeof(arg_t)
 * respectively.
 */

#define LBASE (sizeof(line_t)-sizeof(un_l_a))

int lsizetab[] = {
	LBASE,
	LBASE+sizeof(short),
	LBASE+sizeof(offset),
	LBASE+sizeof(num_p),
	LBASE+sizeof(sym_p),
	LBASE+sizeof(s_la_sval),
	LBASE+sizeof(s_la_lval),
	LBASE+sizeof(arg_p),
	LBASE
};

#define ABASE (sizeof(arg_t)-sizeof(un_a_a))

int asizetab[] = {
	ABASE+sizeof(offset),
	ABASE+sizeof(num_p),
	ABASE+sizeof(sym_p),
	ABASE+sizeof(s_a_val),
	ABASE+sizeof(argb_t),
	ABASE+sizeof(s_a_con),
	ABASE+sizeof(s_a_con),
	ABASE+sizeof(s_a_con),
};

void oldline(line_p lnp);
void oldargb(argb_p abp);
void oldreg(reg_p rp);
void oldargs(arg_p ap);

/*
 * alloc routines:
 * Two parts:
 *   1)	typed alloc and free routines
 *   2) untyped raw core allocation
 */

/*
 * PART 1
 */

line_p newline(int optyp)
{
	line_p lnp;
	int kind=optyp;

	if (kind>OPMINI)
		kind = OPMINI;
	lnp = (line_p) newcore(lsizetab[kind]);
	lnp->l_optyp = optyp;
	return(lnp);
}

void oldline(line_p lnp)
{
	int kind=lnp->l_optyp&BMASK;

	if (kind>OPMINI)
		kind = OPMINI;
	if (kind == OPLIST)
		oldargs(lnp->l_a.la_arg);
	oldcore((short *) lnp,lsizetab[kind]);
}

arg_p newarg(int kind)
{
	arg_p ap;

	ap = (arg_p) newcore(asizetab[kind]);
	ap->a_typ = kind;
	return(ap);
}

void oldargs(arg_p ap)
{
	arg_p	next;

	while (ap != (arg_p) 0) {
		next = ap->a_next;
		switch(ap->a_typ) {
		case ARGSTR:
			oldargb(ap->a_a.a_string.ab_next);
			break;
		case ARGICN:
		case ARGUCN:
		case ARGFCN:
			oldargb(ap->a_a.a_con.ac_con.ab_next);
			break;
		}
		oldcore((short *) ap,asizetab[ap->a_typ]);
		ap = next;
	}
}

void oldargb(argb_p abp)
{
	argb_p next;

	while (abp != (argb_p) 0) {
		next = abp->ab_next;
		oldcore((short *) abp,sizeof (argb_t));
		abp = next;
	}
}

reg_p newreg()
{
	return((reg_p) newcore(sizeof(reg_t)));
}

void oldreg(reg_p rp)
{
	oldcore((short *) rp,sizeof(reg_t));
}

num_p newnum() {

	return((num_p) newcore(sizeof(num_t)));
}

void oldnum(num_p lp)
{
	oldcore((short *) lp,sizeof(num_t));
}

offset *newrom()
{
	return((offset *) newcore(MAXROM*sizeof(offset)));
}

sym_p newsym(int len)
{
	/*
	 * sym_t includes a 2 character s_name at the end
	 * extend this structure with len-2 characters
	 */
	return((sym_p) newcore(sizeof(sym_t) - 2 + len));
}

argb_p newargb()
{
	return((argb_p) newcore(sizeof(argb_t)));
}

#ifndef USEMALLOC

/******************************************************************/
/******   Start of raw core management package    *****************/
/******************************************************************/

#define MAXSHORT 30	/* Maximum number of shorts one can ask for */

short *freelist[MAXSHORT];

typedef struct coreblock {
	struct coreblock *co_next;
	short co_size;
} core_t,*core_p;

#define SINC	(sizeof(core_t)/sizeof(short))
#ifdef COREDEBUG
void coreverbose()
{
	int size;
	short *p;
	int sum;

	sum = 0;
	for(size=1;size<MAXSHORT;size++)
		for (p=freelist[size];p!=0;p = *(short **) p)
			sum += size;
	fprintf(stderr,"Used core %u\n",(shortsasked-sum)*sizeof(short));
}
#endif

#ifdef SEPID

void compactcore()
{
	core_p corelist=0,tp,cl;
	int size;

#ifdef COREDEBUG
	fprintf(stderr,"Almost out of core\n");
#endif
	for(size=SINC;size<MAXSHORT;size++) {
		while ((tp = (core_p) freelist[size]) != (core_p) 0) {
			freelist[size] = (short *) tp->co_next;
			tp->co_size = size;
			if (corelist==0 || tp<corelist) {
				tp->co_next = corelist;
				corelist = tp;
			} else {
				for(cl=corelist;cl->co_next != 0 && tp>cl->co_next;
							cl = cl->co_next)
					;
				tp->co_next = cl->co_next;
				cl->co_next = tp;
			}
		}
	}
	while (corelist != 0) {
		while ((short *) corelist->co_next ==
		    (short *) corelist + corelist->co_size) {
			corelist->co_size += corelist->co_next->co_size;
			corelist->co_next =  corelist->co_next->co_next;
		}
		assert(corelist->co_next==0 ||
			(short *) corelist->co_next >
			    (short *) corelist + corelist->co_size);
		while (corelist->co_size >= MAXSHORT+SINC) {
			oldcore((short *) corelist + corelist->co_size-(MAXSHORT-1),
				sizeof(short)*(MAXSHORT-1));
			corelist->co_size -= MAXSHORT;
		}
		if (corelist->co_size >= MAXSHORT) {
			oldcore((short *) corelist + corelist->co_size-SINC,
				sizeof(short)*SINC);
			corelist->co_size -= SINC;
		}
		cl = corelist->co_next;
		oldcore((short *) corelist, sizeof(short)*corelist->co_size);
		corelist = cl;
	}
}

short *grabcore(int size)
{
	short *p;
	int trysize;

	/*
	 * Desperate situation, can't get more core from system.
	 * Postpone giving up just a little bit by splitting up
	 * larger free blocks if possible.
	 * Algorithm is worst fit.
	 */

	assert(size<2*MAXSHORT);
	for(trysize=2*MAXSHORT-2; trysize>size; trysize -= 2) {
		p = freelist[trysize/sizeof(short)];
		if ( p != (short *) 0) {
			freelist[trysize/sizeof(short)] = *(short **) p;
			oldcore(p+size/sizeof(short),trysize-size);
			return(p);
		}
	}

	/*
	 * Can't get more core from the biggies, try to combine the
	 * little ones. This is expensive but probably better than
	 * giving up.
	 */

	compactcore();
	if ((p=freelist[size/sizeof(short)]) != 0) {
		freelist[size/sizeof(short)] = * (short **) p;
		return(p);
	}
	for(trysize=2*MAXSHORT-2; trysize>size; trysize -= 2) {
		p = freelist[trysize/sizeof(short)];
		if ( p != (short *) 0) {
			freelist[trysize/sizeof(short)] = *(short **) p;
			oldcore(p+size/sizeof(short),trysize-size);
			return(p);
		}
	}

	/*
	 * That's it then. Finished.
	 */

	return(0);
}
#endif	/* SEPID */

short *newcore(int size)
{
	register short *p,*q;

	size = (size + sizeof(int) - 1) & ~(sizeof(int) - 1);
	if( size < 2*MAXSHORT ) {
		if ((p=freelist[size/sizeof(short)]) != (short *) 0)
			freelist[size/sizeof(short)] = *(short **) p;
		else {
			p = freshcore(size);
#ifdef SEPID
			if (p == (short *) 0)
				p = grabcore(size);
#endif
		}
	} else
		p = freshcore(size);
	if (p == 0)
		error("out of memory");
	for (q=p; size > 0 ; size -= sizeof(short))
		*q++ = 0;
	return(p);
}

#ifndef USEMALLOC

/*
 * stdio uses malloc and free.
 * you can use these as substitutes
 */

char *malloc(int size) 
{
	/*
	 * malloc(III) is called by stdio,
	 * this routine is a substitute.
	 */

	return( (char *) newcore(size));
}

void free(void *ptr)
{

}
#endif

void oldcore(short *p, int size)
{
#ifdef CORECHECK
	short *cp;
#endif

	assert(size<2*MAXSHORT);
#ifdef CORECHECK
	for (cp=freelist[size/sizeof(short)]; cp != (short *) 0;
	    cp = *(short **) cp)
		assert(cp != p);
#endif
	*(short **) p = freelist[size/sizeof(short)];
	freelist[size/sizeof(short)] = p;
}

short *ccur,*cend;

void coreinit(short *p1, short *p2)
{
	/*
	 * coreinit is called with the boundaries of a piece of
	 * memory that can be used for starters.
	 */

	ccur = p1;
	cend = p2;
}

short *freshcore(int size)
{
	short *temp;
	static int cchunk=CCHUNK;
	
	while(&ccur[size/sizeof(short)] >= cend && cchunk>0) {
		do {
			temp = (short *) sbrk(cchunk*sizeof(short));
			if (temp == (short *) -1)
				cchunk >>= 1;
			else if (temp != cend)
				ccur = cend = temp;
		} while (temp == (short *) -1 && cchunk>0);
		cend += cchunk;
#ifdef COREDEBUG
		shortsasked += cchunk;
#endif
	}
	if (cchunk==0)
		return(0);
	temp = ccur;
	ccur = &ccur[size/sizeof(short)];
	return(temp);
}

#else	/* USEMALLOC */

void coreinit() {

	/*
	 * Empty function, no initialization needed
	 */
}

short *myalloc(int size)
{
	register short *p,*q;

	p = (short *)malloc(size);
	if (p == 0)
		error("out of memory", NULL);
	for(q=p;size>0;size -= sizeof(short))
		*q++ = 0;
	return(p);
}
#endif
