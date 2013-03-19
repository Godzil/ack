/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef UTIL_OPT_ALLOC_H
#define UTIL_OPT_ALLOC_H

#define USEMALLOC	/* if defined malloc() and free() are used */

#undef COREDEBUG	/* keep records and print statistics */

/*
 * The next define gives if defined the number of pseudo's outside
 * procedures that are collected without processing.
 * If undefined all pseudo's will be collected but that may
 * give trouble on small machines, because of lack of room.
 */
#define PSEUBETWEEN 200 

#ifndef USEMALLOC
/*
 * Now the real bitsqueezing starts.
 * When running on a machine where code and data live in
 * separate address-spaces it is worth putting in some extra
 * code to save on probably less data.
 */
#define SEPID		/* code and data in separate spaces */
/*
 * If the stack segment and the data are separate as on a PDP11 under UNIX
 * it is worth squeezing some shorts out of the stack page.
 */
#define STACKROOM 3200	/* number of shorts space in stack */

#else

#define STACKROOM 1	/* 0 gives problems */

#endif	/* USEMALLOC */

/* util/opt/alloc.c */
line_p newline(int optyp);
void oldline(line_p lnp);
arg_p newarg(int kind);
void oldargs(arg_p ap);
void oldargb(argb_p abp);
reg_p newreg(void);
void oldreg(reg_p rp);
num_p newnum(void);
void oldnum(num_p lp);
offset *newrom(void);
sym_p newsym(int len);
argb_p newargb(void);
void coreinit(short *p1, short *p2);
short *myalloc(int size);

#endif /* UTIL_OPT_ALLOC_H */