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
void declare_idf(struct decspecs *ds, struct declarator *dc, int lvl);
int actual_declaration(int sc, struct type *tp);
void global_redecl(struct idf *idf, int new_sc, struct type *tp);
int good_formal(struct def *def, struct idf *idf);
void declare_params(struct declarator *dc);
void idf_initialized(struct idf *idf);
void declare_parameter(struct idf *idf);
void declare_enum(struct type *tp, struct idf *idf, arith l);
void check_formals(struct idf *idf, struct declarator *dc);
void declare_formals(struct idf *idf, arith *fp);
int regtype(struct type *tp);
void add_def(struct idf *idf, int sc, struct type *tp, int lvl);
void update_ahead(struct idf *idf);
void free_formals(struct formal *fm);

#endif /* LANG_CEM_CEMCOM_ANSI_IDF_H */

