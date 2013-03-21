/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_FILLEM_H
#define MACH_PROTO_NCG_FILLEM_H

/* mach/proto/ncg/fillem.c */
long our_atol(char *s);
void in_init(char *filename);
void in_start(void);
void in_finish(void);
void fillemlines(void);
void dopseudo(void);
int getarg(int typset);
int table1(void);
int table2(void);
int table3(int i);
int get16(void);
long get32(void);
void getstring(void);
char *strarg(int t);
void bss(int n, int t, int b);
long con(int t);
void swtxt(void);
void switchseg(int s);
void savelab(void);
void dumplab(void);
void xdumplab(void);
void part_flush(void);
string holstr(long n);

#endif /* MACH_PROTO_NCG_FILLEM_H */

