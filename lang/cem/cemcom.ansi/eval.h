/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_EVAL_H
#define LANG_CEM_CEMCOM_ANSI_EVAL_H

/* lang/cem/cemcom.ansi/eval.c */
void EVAL(struct expr *expr, int val, int code, label true_label, label false_label);
void compare(int relop, label lbl);
void truthvalue(int relop);
void assop(struct type *type, int oper);
void ptr_add(arith size);
void store_val(struct value *vl, struct type *tp);
void load_val(struct expr *expr, int rlval);
void load_cst(arith val, arith siz);
void operands(struct expr *expr, int gencode);

#endif /* LANG_CEM_CEMCOM_ANSI_EVAL_H */

