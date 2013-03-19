/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_NCGG_ERROR_H
#define LANG_NCGG_ERROR_H

void yyerror(char *s);
void goodbye();

#if __STDC__
void error(char *fmt, ...);
void fatal(char *fmt, ...);
#else /* __STDC__ */
/*VARARGS1*/
fatal(s,a,b,c,d);
error(s,a,b,c,d);
#endif

#ifndef NDEBUG
void badassertion(char *string, char *file, int line);
#endif

int tabovf(char *string);

#endif /* LANG_NCGG_ERROR_H */