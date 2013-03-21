/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_MOVE_H
#define MACH_PROTO_NCG_MOVE_H

/* mach/proto/ncg/move.c */
int move(token_p tp1, token_p tp2, int ply, int toplevel, unsigned int maxcost);
void setcc(token_p tp);
int test(token_p tp, int ply, int toplevel, unsigned int maxcost);

#endif /* MACH_PROTO_NCG_MOVE_H */

