/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_SUBR_H
#define MACH_PROTO_NCG_SUBR_H

/* mach/proto/ncg/subr.c */
int match(token_p tp, set_p tep, int optexp);
void instance(int instno, token_p token);
void cinstance(int instno, token_p token, token_p tp, int regno);
int eqtoken(token_p tp1, token_p tp2);
int distance(int cindex);
int from_stack(set_p s1);
unsigned int costcalc(cost_t cost);
int ssize(int tokexpno);
int tsize(token_p p);
void tref(token_p tp, int amount);
void save_stack(token_p tp);
int in_stack(int reg);
void rest_stack(void);
unsigned int docoerc(token_p tp, c3_p cp, int ply, int toplevel, int forced);
unsigned int stackupto(token_p limit, int ply, int toplevel);
c3_p findcoerc(token_p tp, set_p tep);
void itokcost(void);

#endif /* MACH_PROTO_NCG_SUBR_H */

