/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_STRUCT_H
#define LANG_CEM_CEMCOM_ANSI_STRUCT_H

/* lang/cem/cemcom.ansi/struct.c */
void add_sel(struct type *stp, struct type *tp, struct idf *idf, struct sdef ***sdefpp, arith *szp, struct field *fd);
void check_selector(struct idf *idf, struct type *stp);
void declare_struct(int fund, struct idf *idf, struct type **tpp);
void apply_struct(int fund, struct idf *idf, struct type **tpp);
struct sdef *idf2sdef(struct idf *idf, struct type *tp);
arith add_field(arith *szp, struct field *fd, struct type **fdtpp, struct idf *idf, struct type *stp);
int is_struct_or_union(register int fund);
int gcd(register int m, register int n);
int lcm(register int m, register int n);

#endif /* LANG_CEM_CEMCOM_ANSI_STRUCT_H */

