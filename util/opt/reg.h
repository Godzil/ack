/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTIL_OPT_REG_H
#define UTIL_OPT_REG_H

#include "types.h"

/* util/opt/reg.c */
void regvar(arg_p ap);
int inreg(offset off);
void outregs(void);
void outtes(void);
void incregusage(offset off);

#endif /* UTIL_OPT_REG_H */