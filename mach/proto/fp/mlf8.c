/*
  (c) copyright 1988 by the Vrije Universiteit, Amsterdam, The Netherlands.
  See the copyright notice in the ACK home directory, in the file "Copyright".
*/

/* $Header$ */

/*
 * Multiply Single Precision Float (MLF 8)
 */

#include	"FP_types.h"

_double
mlf8(s2,s1)
_double	s1,s2;
{
	EXTEND	e1,e2;

	extend(&s1,&e1,sizeof(_double));
	extend(&s2,&e2,sizeof(_double));
		/* do a multiply */
	mul_ext(&e1,&e2);
	compact(&e1,&s1,sizeof(_double));
	return(s1);
}