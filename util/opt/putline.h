/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTIL_OPT_PUTLINE_H
#define UTIL_OPT_PUTLINE_H

#include "proinf.h"
#include "types.h"

/* util/opt/putline.c */
void putlines(line_p lnp);
void putargs(arg_p ap);
void putstr(argb_p abp);
void outdef(sym_p sp);
void outocc(sym_p sp);
void outpro(void);
void outend(void);
void outinst(int m);
void outoff(offset off);
void outint(short i);
void outshort(short i);
void numlab(num_p np);
void outnum(num_p np);
void outsym(sym_p sp);

#endif /* UTIL_OPT_PUTLINE_H */