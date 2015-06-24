/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/*  I N L I N E   S U B S T I T U T I O N
 *
 *  I L 1 _ A U X . H
 */

bool same_size(int t1, int t2);
				/* See if the two types t1 and t2 have
				 * the same size.
				 */
void rem_actuals(actual_p acts);
				/* remove an actual-list from core.
				 */
void remov_formals(proc_p p);
				/* Remove the formals-list of p from core.
				 */
void rem_indir_acc(proc_p p);
				/* Remove formal that may be accessed
				 * indirectly from formal lists of p
				 */
bool par_overlap(offset off1, int t1, offset off2, int t2);
				/* See if the formal at offset off1 and type t1
				 * overlaps the formal at offset off2
				 * and type t2.
				 */
short looplevel(bblock_p b);
				/* Determine the loop nesting level of b.
				 */
int proclength(proc_p p);
				/* Determine the number of EM instructions
				 * in p. Do not count pseudos.
				 */

line_p copy_code(line_p l1, line_p l2);
				/* copy the code between l1 and l2.
				 * Pseudos may not be contained in
				 * the list of instructions. If l1==l2
				 * the result is only one instruction.
				 */
