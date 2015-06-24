/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* F I L E   I N F O R M A T I O N   S T R U C T U R E */

#ifndef UTIL_CPP_FILE_INFO_H
#define UTIL_CPP_FILE_INFO_H

struct file_info {
	unsigned int	fil_lino;
	char		*fil_name;
	char		*fil_wdir;
};

#define LineNumber	finfo.fil_lino
#define FileName	finfo.fil_name
#define WorkingDir	finfo.fil_wdir

extern struct file_info finfo;	/* input.c */

#endif /* UTIL_CPP_FILE_INFO_H */