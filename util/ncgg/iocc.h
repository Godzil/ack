/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef LANG_NCGG_IOCC_H
#define LANG_NCGG_IOCC_H

typedef struct iocc {
	short in_set[SETSIZE];
	int   in_index;
} iocc_t,*iocc_p;

/* util/ncgg/iocc.c */
iocc_t subr_iocc(int tokarg, int subreg);
iocc_t tokm_iocc(int tokarg, char *ident);
iocc_t percident_iocc(char *ident);
iocc_t ident_iocc(char *ident);
iocc_t all_iocc(int all_no, int subreg);
iocc_t descr_iocc(char *ident);
int instalookup(inst_t insta, int filled);

#endif /* LANG_NCGG_IOCC_H */