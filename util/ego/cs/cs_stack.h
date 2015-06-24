/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
void Push(token_p tkp);
			/* Push the token in tkp on the fake-stack.
			 */

void Pop(token_p tkp, offset size);
			/* Pop a token of size bytes from the fake-stack
			 * into tkp. If such a token is not there
			 * we put a dummy in tkp and adjust the fake-stack.
			 */

void Dup(line_p lnp);
			/* Reflect the changes made by the dup-instruction
			 * in lnp to the EM-stack into the fake-stack.
			 */

void clr_stack();
			/* Clear the fake-stack.
			 */
