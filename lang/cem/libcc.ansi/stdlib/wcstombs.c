/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Header$ */

#include	<stdlib.h>
#include	<locale.h>
#include	<limits.h>

size_t
wcstombs(register char *s, register const wchar_t *pwcs, size_t n)
{
	register int i = 0;

	while (i < n) {
		i++;
		*s++ = *pwcs++;
		if (*s == '\0')
			return i;
	}
	return n;
}