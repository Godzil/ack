/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  O P E R A T I O N S    F O R
 *      C O M P A C T   S E T S
 */

cset Cempty_set(short n);
bool Cis_elem(Celem_t x, cset s);
void Cadd(Celem_t x, cset *s_p);
void Cremove(Celem_t x, cset *s_p);
Cindex Cfirst(cset s);
Cindex Cnext(Cindex i, cset s);
Celem_t Celem(Cindex i);
void Cjoin(cset s1, cset *s2_p);
void Cintersect(cset s1, cset *s2_p);
void Cdeleteset(cset s);
bool Cis_subset(cset s1, cset s2);void Cclear_set(cset *s_p);
void Ccopy_set(cset s1, cset *s2_p);
void Csubtract(cset s1, cset *s2_p);
bool Cequal(cset s1, cset s2);
short Cnrelems(cset s);