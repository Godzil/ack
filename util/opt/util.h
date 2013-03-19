/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTIL_OPT_UTIL_H
#define UTIL_OPT_UTIL_H

/* util/opt/util.c */
void error(char *s, ...);

#ifndef NDEBUG
void badassertion(char *file, unsigned int line);
#endif

#ifdef DIAGOPT
void optim(int n);
#endif
#endif /* UTIL_OPT_UTIL_H */