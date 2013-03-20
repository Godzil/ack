/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_FIELD_H
#define LANG_CEM_CEMCOM_ANSI_FIELD_H

/* lang/cem/cemcom.ansi/field.c */
void eval_field(struct expr *expr, int code);
void store_field(struct field *fd, int uns, int code, struct expr *leftop, arith tmpvar);

#endif /* LANG_CEM_CEMCOM_ANSI_FIELD_H */

