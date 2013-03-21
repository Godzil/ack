/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
#ifndef MACH_PROTO_NCG_REGVAR_H
#define MACH_PROTO_NCG_REGVAR_H

struct regvar {
	struct regvar  *rv_next;
	long		rv_off;
	int		rv_size;
	int		rv_type;
	int		rv_score;
	int		rv_reg;
};

struct regassigned {
	struct regvar  *ra_rv;
	int		ra_score;
};

extern struct regvar *rvlist;
extern int nregvar[];
extern struct regassigned *regassigned[];

struct regvar *linkreg(long of, int sz, int tp, int sc);
void tryreg(struct regvar *rvlp, int typ);
void fixregvars(int saveall);
int isregvar(long off);
int isregtyp(long off);
void unlinkregs();

#endif /* MACH_PROTO_NCG_REGVAR_H */