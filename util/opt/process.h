/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTIL_OPT_PROCESS_H
#define UTIL_OPT_PROCESS_H

#include "types.h"

/* util/opt/process.c */
void process(void);
void relabel(void);
void symknown(void);
void cleanlocals(void);
void checklocs(void);
offset align(offset count, offset alignment);
void symvalue(void);
void do_tes(void);

#endif /* UTIL_OPT_PROCESS_H */