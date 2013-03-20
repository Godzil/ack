/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_IDF_H
#define LANG_CEM_CEMCOM_ANSI_IDF_H

#include "declar.h"
#include "decspecs.h"
#include "def.h"

/* lang/cem/cemcom.ansi/idf.c */
void init_idf(void);
struct idf *str2idf(char tg[], int cpy);
struct idf *gen_idf(void);
int is_anon_idf(struct idf *idf);
int declare_idf(struct decspecs *ds, struct declarator *dc, int lvl);
int actual_declaration(int sc, struct type *tp);
int global_redecl(register struct idf *idf, int new_sc, struct type *tp);
int good_formal(register struct def *def, register struct idf *idf);
int declare_params(struct declarator *dc);
int idf_initialized(register struct idf *idf);
int declare_parameter(struct idf *idf);
int declare_enum(struct type *tp, struct idf *idf, arith l);
int check_formals(struct idf *idf, struct declarator *dc);
int declare_formals(struct idf *idf, arith *fp);
int regtype(struct type *tp);
int add_def(struct idf *idf, int sc, struct type *tp, int lvl);
int update_ahead(register struct idf *idf);
int free_formals(register struct formal *fm);

#endif /* LANG_CEM_CEMCOM_ANSI_IDF_H */

