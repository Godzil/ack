/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */

#ifdef NDEBUG

#define debug(s)

#else
extern int DEB;

#define debug(s...)	(DEB && printf(s))

#endif

extern int Verbose;
#define verbose(s...)	(Verbose && do_verbose(s))
