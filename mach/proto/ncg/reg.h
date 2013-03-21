/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_REG_H
#define MACH_PROTO_NCG_REG_H

/* mach/proto/ncg/reg.c */
void chrefcount(int regno, int amount, int tflag);
int getrefcount(int regno, int tflag);
void erasereg(int regno);
void cleanregs(void);
void inctcount(int regno);
void chkregs(void);

#endif /* MACH_PROTO_NCG_REG_H */

