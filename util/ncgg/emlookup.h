/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_NCGG_EMLOOKUP_H
#define LANG_NCGG_EMLOOKUP_H

/* util/ncgg/emlookup.c */
void initemhash(void);
unsigned int emhash(char *name);
void enter(char *name, int value);
int mlookup(char *name);
int argtyp(int mn);


#endif /* LANG_NCGG_EMLOOKUP_H */