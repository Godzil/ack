/* $Header$ */
/*
 * (c) copyright 1983 by the Vrije Universiteit, Amsterdam, The Netherlands.
 *
 *          This product is part of the Amsterdam Compiler Kit.
 *
 * Permission to use, sell, duplicate or disclose this software must be
 * obtained in writing. Requests for such permissions may be sent to
 *
 *      Dr. Andrew S. Tanenbaum
 *      Wiskundig Seminarium
 *      Vrije Universiteit
 *      Postbox 7161
 *      1007 MC Amsterdam
 *      The Netherlands
 *
 */

/* Author: J.W. Stevenson */

/* function clock:integer; extern; */

extern int	times();

struct tbuf {
	long	utime;
	long	stime;
	long	cutime;
	long	cstime;
};

int clock() {
	struct tbuf t;

	times(&t);
	return( (int)(t.utime + t.stime) &
#if EM_WSIZE <= 2
	077777
#else
	0x7fffffffL
#endif
	);
}
