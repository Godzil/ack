/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_SALLOC_H
#define MACH_PROTO_NCG_SALLOC_H

/* mach/proto/ncg/salloc.c */
string myalloc(int size);
void myfree(string p);
void popstr(int nnstab);
char *salloc(int size);
int compar(const void *vp1, const void *vp2);
void garbage_collect(void);
void chkstr(string str, char used[]);

#endif /* MACH_PROTO_NCG_SALLOC_H */

