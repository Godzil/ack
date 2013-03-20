/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_DECLARATOR_H
#define LANG_CEM_CEMCOM_ANSI_DECLARATOR_H

/* lang/cem/cemcom.ansi/declarator.c */
struct type *declare_type(struct type *tp, struct declarator *dc);
void add_decl_unary(struct declarator *dc, int fund, int qual, arith count, struct formal *fm, struct proto *pl);
void remove_declarator(struct declarator *dc);
void reject_params(struct declarator *dc);
void check_array_subscript(struct expr *expr);

#endif /* LANG_CEM_CEMCOM_ANSI_DECLARATOR_H */

