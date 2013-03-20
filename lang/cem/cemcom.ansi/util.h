/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_UTIL_H
#define LANG_CEM_CEMCOM_ANSI_UTIL_H

/* lang/cem/cemcom.ansi/util.c */
int LocalInit(void);
arith LocalSpace(arith sz, int al);
arith NewLocal(arith sz, int al, int regtype, int sc);
int FreeLocal(arith off);
int LocalFinish(void);
int RegisterAccount(arith offset, arith size, int regtype, int sc);
int LoadLocal(arith off, arith sz);
int StoreLocal(arith off, arith sz);
int AddrLocal(arith off);

#endif /* LANG_CEM_CEMCOM_ANSI_UTIL_H */

