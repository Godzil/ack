/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTIL_OPT_GETLINE_H
#define UTIL_OPT_GETLINE_H

/* util/opt/getline.c */
short readshort(void);
offset readoffset(void);
void draininput(void);
short getint(void);
sym_p getsym(int status);
offset getoff(void);
void make_string(int n);
void inident(void);
int table3(int n);
int table1(void);
int table2(void);
void getlines(void);
void argstring(offset length, argb_p abp);
line_p arglist(int n);
offset aoff(arg_p ap, int n);
int inpseudo(short n);
void tstinpro(void);


#endif /* UTIL_OPT_GETLINE_H */