/* $Id$ */
/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* D E F I N I T I O N S   F O R   T H E   L E X I C A L   A N A L Y Z E R */

/*	A token from the input stream is represented by an integer,
	called a "symbol", but it may have other information associated
	to it.
*/

#include <em_arith.h>
#include "system.h"
#include "print.h"

/* the structure of a token:	*/
struct token	{
	int tok_symb;		/* the token itself */
	union {
		arith tok_val;		/* numeric values */
		char *tok_str;		/* string/filespecifier */
	} tok_data;
};

#include "file_info.h"

#define tk_symb	tok_symb
#define tk_val	tok_data.tok_val
#define tk_str	tok_data.tok_str

extern struct token dot;

extern int ReplaceMacros;	/* "LLlex.c"	*/
extern int AccFileSpecifier;	/* "LLlex.c"	*/
extern int AccDefined;		/* "LLlex.c"	*/
extern int UnknownIdIsZero;	/* "LLlex.c"	*/

extern int NoUnstack;		/* "input.c"	*/
extern int Unstacked;		/* "input.c"	*/

extern int err_occurred;	/* "error.c"	*/

#define	DOT	dot.tk_symb

#define EOF	(-1)

#include <symbol2str.h>
struct idf;
struct token;

char *string_token(char *nm, int stop_char);
void skipcomment();
int GetToken(struct token *ptok);
void fatal(char *fmt, ...);
void EnableMacros();
int replace(struct idf *idef);
void error(char *fmt, ...);
int quoted(int c);
int val_in_base(int c, int base);
void crash(char *fmt, ...);
void skipline();
void warning(char *fmt, ...);
void PushLex();
void If_expr(void);
void PopLex();
void add_dependency(char *s);
int skipspaces(int ch, int skipnl);
void macro_def(struct idf *id, char *text, int nformals, int length, int flags);
void DoUnstack();
void init_pp();
void do_option(char *text);
void preprocess(char *fn);
void domacro();
int rank_of(int oper);
char *GetIdentifier();
char **getactuals(struct idf *idef);

/* External */
int InsertFile(char *, char **, char **);