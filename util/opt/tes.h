/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 *
 * Author: Hans van Eck. 
 */
/* $Id$ */
#ifndef UTIL_OPT_TES_H
#define UTIL_OPT_TES_H

extern int state;
#define	KNOWN		1
#define	NOTREACHED	2

/* util/opt/tes.c */
void init_state(void);
void tes_pseudos(void);
void tes_instr(line_p lnp, line_p x, line_p y);
void assign_label(num_p label);
void do_inst_label(line_p lnp);

#endif /* UTIL_OPT_TES_H */
