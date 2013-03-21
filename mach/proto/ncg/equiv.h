/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef MACH_PROTO_NCG_EQUIV_H
#define MACH_PROTO_NCG_EQUIV_H

#define MAXCREG 4

struct perm {
	struct perm *p_next;
	int p_rar[MAXCREG];
};

struct perm *tuples(rl_p *regls, int nregneeded);
void permute(int index);

#endif /* MACH_PROTO_NCG_EQUIV_H */