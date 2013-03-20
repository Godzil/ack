/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_EXPR_H
#define LANG_CEM_CEMCOM_ANSI_EXPR_H

/* lang/cem/cemcom.ansi/expr.c */
int rank_of(int oper);
int dot2expr(struct expr **expp);
int idf2expr(register struct expr *expr);
int string2expr(register struct expr **expp, char *str, int len);
int int2expr(struct expr *expr);
int float2expr(register struct expr *expr);
struct expr *intexpr(arith ivalue, int fund);
int fill_int_expr(register struct expr *ex, arith ivalue, int fund);
struct expr *new_oper(struct type *tp, register struct expr *e1, int oper, register struct expr *e2);
int chk_cst_expr(struct expr **expp);
int init_expression(register struct expr ***eppp, struct expr *expr);
int is_ld_cst(register struct expr *expr);
int is_cp_cst(struct expr *expr);
int is_fp_cst(struct expr *expr);
int is_zero_cst(register struct expr *expr);
int free_expression(register struct expr *expr);

#endif /* LANG_CEM_CEMCOM_ANSI_EXPR_H */

