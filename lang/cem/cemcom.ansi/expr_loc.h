/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_EXPR_H
#define LANG_CEM_CEMCOM_ANSI_EXPR_H

/* lang/cem/cemcom.ansi/expr.c */
int rank_of(int oper);
void dot2expr(struct expr **expp);
void idf2expr(struct expr *expr);
void string2expr(struct expr **expp, char *str, int len);
void int2expr(struct expr *expr);
void float2expr(struct expr *expr);
struct expr *intexpr(arith ivalue, int fund);
void fill_int_expr(struct expr *ex, arith ivalue, int fund);
struct expr *new_oper(struct type *tp, struct expr *e1, int oper, struct expr *e2);
void chk_cst_expr(struct expr **expp);
void init_expression(struct expr ***eppp, struct expr *expr);
int is_ld_cst(struct expr *expr);
int is_cp_cst(struct expr *expr);
int is_fp_cst(struct expr *expr);
int is_zero_cst(struct expr *expr);
void free_expression(struct expr *expr);

#endif /* LANG_CEM_CEMCOM_ANSI_EXPR_H */

