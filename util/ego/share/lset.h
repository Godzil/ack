/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  O P E R A T I O N S    F O R
 *	L O N G   S E T S
 */


lset	Lempty_set();
bool Lis_elem(Lelem_t x, lset s);
void Ladd(Lelem_t x, lset *s_p);
void Lremove(Lelem_t x, lset *s_p);
Lindex Lfirst(lset s);
Lindex Lnext(Lindex i, lset s);
Lelem_t Lelem(Lindex i);
void Ljoin(lset s1, lset *s2_p);
void Ldeleteset(lset s);
bool Lis_subset(lset s1, lset s2);
short Lnrelems(lset s);
