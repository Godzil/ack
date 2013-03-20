/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_REPLACE_H
#define LANG_CEM_CEMCOM_ANSI_REPLACE_H

struct args;
struct repl;

/* lang/cem/cemcom.ansi/replace.c */
int replace(register struct idf *idf);
void unstackrepl(void);
void freeargs(struct args *args);
void EnableMacros(void);
int expand_macro(register struct repl *repl, register struct idf *idf);
void expand_defined(register struct repl *repl);
void newarg(struct args *args);
void getactuals(struct repl *repl, register struct idf *idf);
void saveraw(struct repl *repl);
int actual(struct repl *repl);
void macro_func(register struct idf *idef);
void macro2buffer(register struct repl *repl, register struct idf *idf, register struct args *args);
char *stringify(register struct repl *repl, register char *ptr, register struct args *args);
void add2repl(register struct repl *repl, int ch);
void stash(struct repl *repl, register int ch, int stashraw);

#endif /* LANG_CEM_CEMCOM_ANSI_REPLACE_H */

