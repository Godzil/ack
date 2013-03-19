/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTIL_OPT_PEEPHOLE_H
#define UTIL_OPT_PEEPHOLE_H

#include "types.h"
#include "pattern.h"
 
/* util/opt/peephole.c */
void opcheck(byte *bp);
void hashpatterns(void);
int peephole(void);
int optimize(void);
offset oabs(offset off);
line_p repline(eval_t ev, int patlen);
offset rotate(offset w, offset amount);
eval_t compute(expr_p pexp);
bool tryrepl(line_p *lpp, register byte *bp, int patlen);
bool trypat(line_p *lpp, register byte *bp, int len);
int basicblock(line_p *alpp);
int repl_mul(register line_p lp, line_p *b, line_p *e);

#endif /* UTIL_OPT_PEEPHOLE_H */