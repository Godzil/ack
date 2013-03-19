/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_NCGG_OUTPUT_H
#define LANG_NCGG_OUTPUT_H

/* util/ncgg/output.c */
void opnfile(FILE **f, char *s);
void unlfile(FILE *f, char *s);
void initio(void);
void finishcode(void);
void errorexit(void);
void code8(int x);
void code53(int x, int y);
void codeint(int x);
void outpatterns(void);
void pat(int n);
void patshort(int n);
void patbyte(int n);
void hashpatterns(void);
void outincludes(void);
void outregs(void);
void outregvars(void);
int typeconv(int n);
void outfmt(char *p);
void outtokens(void);
void outenodes(void);
void outstrings(void);
void outsets(void);
void outinstances(void);
void outmoves(void);
void outtests(void);
void outstacks(void);
void outsplits(void);
void outcoercs(void);
void outproplists(void);
void outconsts(void);
void cdef(char *s, int n);
void passon(char *s);
void outdefs(void);
void outars(void);
void finishio(void);
void codecoco(int cocono);
void dopattern(int stackcoerc, varinfo *kills, varinfo *allocates, varinfo *generates, varinfo *yields, varinfo *leaving);
void used(char *resource, int use, int max);
void statistics(void);

#endif /* LANG_NCGG_OUTPUT_H */