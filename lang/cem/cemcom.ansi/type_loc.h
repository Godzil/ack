/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_TYPE_H
#define LANG_CEM_CEMCOM_ANSI_TYPE_H

/* lang/cem/cemcom.ansi/type.c */
struct type *create_type(int fund);
struct type *promoted_type(struct type *tp);
struct type *construct_type(int fund, struct type *tp, int qual, arith count, struct proto *pl);
struct type *function_of(struct type *tp, struct proto *pl, int qual);
struct type *pointer_to(struct type *tp, int qual);
struct type *array_of(struct type *tp, arith count, int qual);
struct type *field_of(struct type *tp, int qual);
arith size_of_type(struct type *tp, char nm[]);
void idf2type(struct idf *idf, struct type **tpp);
arith align(arith pos, int al);
struct type *standard_type(int fund, int sgn, int algn, arith sz);
void completed(struct type *tp);

#endif /* LANG_CEM_CEMCOM_ANSI_TYPE_H */

