/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
/* $Id$ */
/* D E F I N I T I O N S   F O R   T H E   L E X I C A L   A N A L Y Z E R */

/*	A token from the input stream is represented by an integer,
	called a "symbol", but it may have other information associated
	to it.
*/

/* the structure of a token:	*/
struct token	{
	int tok_symb;		/* the token itself */
	union	{
		char *tok_str;
		struct {
			int tok_unsigned;
			arith tok_val;		/* for INTEGER */
		} tok_int;
	} tok_data;
};

#include "file_info.h"
#include <symbol2str.h>

#define tk_symb	tok_symb
#define tk_str	tok_data.tok_str
#define tk_val	tok_data.tok_int.tok_val
#define tk_unsigned tok_data.tok_int.tok_unsigned

extern struct token dot;

extern int ReplaceMacros;	/* "LLlex.c"	*/
extern int AccDefined;		/* "LLlex.c"	*/
extern int Unstacked;		/* "LLlex.c"	*/
extern int UnknownIdIsZero;	/* "LLlex.c"	*/
extern int AccFileSpecifier;    /* "LLlex.c"    */

extern int NoUnstack;		/* buffer.c	*/

extern int err_occurred;	/* "error.c"	*/

struct idf; 
struct repl;

void skipcomment();
void fatal(char *fmt, ...);
void error(char *fmt, ...);
void warning(char *fmt, ...);
void strict(char *fmt, ...);
void crash(char *fmt, ...);
int GetToken(struct token *ptok);
void EnableMacros();
int replace(struct idf *idf);
int skipspaces(int ch, int skipnl);
int SkipToNewLine();
void do_pragma();
int ifexpr();
void If_expr(void);
void add_dependency(char *s);
int getparams(char *buf[], char parbuf[]);
int macroeq(char *s, char *t);
void add2repl(struct repl *repl, int ch);
char *GetIdentifier(int skiponerr);
void macro_def(struct idf *id, char *text, int nformals, int length, int flags);
void unstackrepl();
void init_pp();
void do_option(char *text);
void preprocess(char *fn);
void do_undef(char *argstr);
void domacro();
int actual(struct repl *repl);
void ch3mon(int oper, arith *pval, int *puns);
int rank_of(int oper);
void ch3bin(arith *pval, int *pis_uns, int oper, arith val, int is_uns);
/* External */
int InsertFile(char *, char **, char **);

#define	DOT	dot.tk_symb

#define EOF	(-1)
