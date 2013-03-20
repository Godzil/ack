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
int unstackrepl(void);
int freeargs(struct args *args);
int EnableMacros(void);
int expand_macro(register struct repl *repl, register struct idf *idf);
int expand_defined(register struct repl *repl);
int newarg(struct args *args);
int getactuals(struct repl *repl, register struct idf *idf);
int saveraw(struct repl *repl);
int actual(struct repl *repl);
int macro_func(register struct idf *idef);
int macro2buffer(register struct repl *repl, register struct idf *idf, register struct args *args);
char *stringify(register struct repl *repl, register char *ptr, register struct args *args);
int add2repl(register struct repl *repl, int ch);
int stash(struct repl *repl, register int ch, int stashraw);

#endif /* LANG_CEM_CEMCOM_ANSI_REPLACE_H */

