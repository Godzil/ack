/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_UTIL_H
#define LANG_CEM_CEMCOM_ANSI_UTIL_H

/* lang/cem/cemcom.ansi/util.c */
void LocalInit(void);
arith LocalSpace(arith sz, int al);
arith NewLocal(arith sz, int al, int regtype, int sc);
void FreeLocal(arith off);
void LocalFinish(void);
void RegisterAccount(arith offset, arith size, int regtype, int sc);
void LoadLocal(arith off, arith sz);
void StoreLocal(arith off, arith sz);
void AddrLocal(arith off);

#endif /* LANG_CEM_CEMCOM_ANSI_UTIL_H */

