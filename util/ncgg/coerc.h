/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_NCGG_COERC_H
#define LANG_NCGG_COERC_H

#include "iocc.h"
#include "varinfo.h"

/* util/ncgg/coerc.c */
void n_move(int s1, int e1, int s2, int e2, struct varinfo *vi);
int existmove(iocc_t from, short *sp);
int existalmove(iocc_t from, int prpno);
struct varinfo *gen_move(iocc_t from, iocc_t to);
void n_test(int s, int e, struct varinfo *vi);
struct varinfo *gen_test(iocc_t from);
struct varinfo *gen_label(int arg);
struct varinfo *gen_preturn(void);
struct varinfo *gen_tlab(int n);
void n_stack(int s, int e, int p, struct varinfo *vi);
void checkstacking(short *sp);
void n_coerc(int ti, int be, struct varinfo *al, struct varinfo *ge, struct varinfo *rp, iocc_t in);
void checkunstacking(int setno);
void n_split(int ti, int be, struct varinfo *al, struct varinfo *ge, struct varinfo *rp, int n);

#endif /* LANG_NCGG_COERC_H */