/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_GENCODE_H
#define MACH_PROTO_NCG_GENCODE_H

/* mach/proto/ncg/gencode.c */
void out_init(char *filename);
void out_finish(void);
void tstoutput(void);
void genstr(int stringno);
string ad2str(addr_t ad);
void praddr(addr_t ad);
void gennl(void);
void prtoken(token_p tp, int leadingchar);
void printlabel(int labnum);

#endif /* MACH_PROTO_NCG_GENCODE_H */

