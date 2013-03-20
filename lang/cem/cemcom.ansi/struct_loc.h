/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_STRUCT_H
#define LANG_CEM_CEMCOM_ANSI_STRUCT_H

/* lang/cem/cemcom.ansi/struct.c */
int add_sel(register struct type *stp, struct type *tp, register struct idf *idf, struct sdef ***sdefpp, arith *szp, struct field *fd);
int check_selector(register struct idf *idf, struct type *stp);
int declare_struct(int fund, register struct idf *idf, struct type **tpp);
int apply_struct(int fund, register struct idf *idf, struct type **tpp);
struct sdef *idf2sdef(register struct idf *idf, struct type *tp);
arith add_field(arith *szp, register struct field *fd, register struct type **fdtpp, struct idf *idf, register struct type *stp);
int is_struct_or_union(register int fund);
int gcd(register int m, register int n);
int lcm(register int m, register int n);

#endif /* LANG_CEM_CEMCOM_ANSI_STRUCT_H */

