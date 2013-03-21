/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_CODEGEN_H
#define MACH_PROTO_NCG_CODEGEN_H

/* mach/proto/ncg/codegen.c */
unsigned codegen(byte *codep, int ply, int toplevel, unsigned int costlimit, int forced);
void readcodebytes(void);

#ifdef TABLEDEBUG
void initlset(char *f);
void termlset();
#endif

#endif /* MACH_PROTO_NCG_CODEGEN_H */

