/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_MACH_DEP_H
#define MACH_PROTO_NCG_MACH_DEP_H

/* Function prototypes that must be exported by specific machines */
void regreturn(void);
void mes(word type);
void prolog(full nlocals);
void con_part(int sz, word w);
void con_mult(word sz);
void con_float(void);
int regscore(long off, int size, int typ, int score, int totyp);
void i_regsave();
void f_regsave();
void regsave(char *regstr, long off, int size);
void regreturn();

#endif /* MACH_PROTO_NCG_MACH_DEP_H */
