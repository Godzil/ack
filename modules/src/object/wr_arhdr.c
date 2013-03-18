/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#include "obj.h"

void wr_arhdr(int fd, struct ar_hdr *arhdr)
{
	char buf[AR_TOTAL];
	char *c = buf;
	char *p = arhdr->ar_name;
	int i = 14;

	while (i--) {
		*c++ = *p++;
	}
	put2((int)(arhdr->ar_date>>16),c); c += 2;
	put2((int)(arhdr->ar_date),c); c += 2;
	*c++ = arhdr->ar_uid;
	*c++ = arhdr->ar_gid;
	put2(arhdr->ar_mode,c); c += 2;
	put2((int)(arhdr->ar_size>>16),c); c += 2;
	put2((int)(arhdr->ar_size),c);
	wr_bytes(fd, buf, (long) AR_TOTAL);
}
