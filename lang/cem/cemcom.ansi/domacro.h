/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_DOMACRO_H
#define LANG_CEM_CEMCOM_ANSI_DOMACRO_H

/* lang/cem/cemcom.ansi/domacro.c */
struct idf *GetIdentifier(int skiponerr);
int domacro(void);
int skip_block(int to_endif);
int ifexpr(void);
int do_include(void);
int do_define(void);
int push_if(void);
int do_elif(void);
int do_else(void);
int do_endif(void);
int do_if(void);
int do_ifdef(int how);
int do_undef(struct idf *argidf);
int do_error(void);
int getparams(char *buf[], char parbuf[]);
int macro_def(register struct idf *id, char *text, int nformals, int length, int flags);
int find_name(char *nm, char *index[]);
char *get_text(char *formals[], int *length);
int macroeq(register char *s, register char *t);
int do_line(unsigned int l);

#endif /* LANG_CEM_CEMCOM_ANSI_DOMACRO_H */

