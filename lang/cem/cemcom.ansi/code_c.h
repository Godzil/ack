/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_CODE_H
#define LANG_CEM_CEMCOM_ANSI_CODE_H

struct decspecs;

void init_code(char *dst_file);
label code_string(char *val, int len);
void flush_strings();
void code_scope(char *text, struct def *def);
void begin_proc(struct decspecs *ds, struct idf *idf);
void end_proc(arith fbytes);
void do_return();
void do_return_expr(struct expr *expr);
void code_declaration(struct idf *idf, struct expr *expr, int lvl, int sc);
void loc_init(struct expr *expr, struct idf *id);
void bss(struct idf *idf);
void code_expr(struct expr *expr, int val, int code, label tlbl, label flbl);
void code_break();
void code_continue();
void stack_stmt(label break_label, label cont_label);
void unstack_stmt();

#ifdef DBSYMTAB
void db_line(char *file, unsigned int line);
#endif

#ifdef	PREPEND_SCOPES
void prepend_scopes();
#endif

#ifndef	LINT
void end_code();
#endif

#endif