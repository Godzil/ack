/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_DOMACRO_H
#define LANG_CEM_CEMCOM_ANSI_DOMACRO_H

/* lang/cem/cemcom.ansi/domacro.c */
struct idf *GetIdentifier(int skiponerr);
void domacro(void);
void skip_block(int to_endif);
int ifexpr(void);
void do_include(void);
void do_define(void);
void push_if(void);
void do_elif(void);
void do_else(void);
void do_endif(void);
void do_if(void);
void do_ifdef(int how);
void do_undef(struct idf *argidf);
void do_error(void);
int getparams(char *buf[], char parbuf[]);
void macro_def(struct idf *id, char *text, int nformals, int length, int flags);
int find_name(char *nm, char *index[]);
char *get_text(char *formals[], int *length);
int macroeq(char *s, char *t);
void do_line(unsigned int l);

#endif /* LANG_CEM_CEMCOM_ANSI_DOMACRO_H */
