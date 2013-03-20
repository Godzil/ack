/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* TOKENNAME DEFINITION */
#ifndef LANG_CEM_CEMCOM_ANSI_TOCKENNAME_H
#define LANG_CEM_CEMCOM_ANSI_TOCKENNAME_H

struct tokenname	{	/*	Used for defining the name of a
					token as identified by its symbol
				*/
	int tn_symbol;
	char *tn_name;
};

void reserve(struct tokenname resv[]);

#endif /* LANG_CEM_CEMCOM_ANSI_TOCKENNAME_H */
