/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* These routines partition the huge set of EM-instructions in
 * "manageable chunks.
 */

int instrgroup(line_p lnp);
				/* Return the group into which the instruction
				 * in lnp belongs to.
				 */

bool stack_group(int instr);
				/* Return whether instr is an instruction that
				 * only changes the state of the stack, i.e.
				 * is a "true" operator.
				 */

offset op11size(line_p lnp);
				/* Return the size of the operand of the unary
				 * operator in lnp.
				 */

offset op12size(line_p lnp);
				/* Return the size of the first operand of the
				 * binary operator in lnp.
				 */

offset op22size(line_p lnp);
				/* Return the size of the second operand of the
				 * binary operator in lnp.
				 */

offset op13size(line_p lnp);
				/* Return the size of the first operand of the
				 * ternary operator in lnp.
				 */

offset op23size(line_p lnp);
				/* Return the size of the second operand of the
				 * ternary operator in lnp.
				 */

offset op33size(line_p lnp);
				/* Return the size of the third operand of the
				 * ternary operator in lnp.
				 */

offset avsize(line_p lnp);
				/* Return the size of the result of the
				 * operator in lnp.
				 */

int regtype(byte instr);
				/* Return in what kind of machine-register
				 * the result of instr should be stored:
				 * pointer, float, or any.
				 */
