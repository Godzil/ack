/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef LANG_CEM_CEMCOM_ANSI_CH3_H
#define LANG_CEM_CEMCOM_ANSI_CH3_H

void ch3cast(struct expr **expp, int oper, struct type *tp);
void ch3sel(struct expr **expp, int oper, struct idf *idf);
void ch3incr(struct expr **expp, int oper);
int equal_type(struct type *tp, struct type *otp, int qual_lev, int diag);
int recurqual(struct type *tp, int qual);
void ch3asgn(struct expr **expp, int oper, struct expr *expr);
int is_integral_type(struct type *tp);

#endif