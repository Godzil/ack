/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef MACH_PROTO_NCG_COMPUTE_H
#define MACH_PROTO_NCG_COMPUTE_H

/* mach/proto/ncg/compute.c */
string mycat(string s1, string s2);
string mystrcpy(string s);
string tostring(long n);
void compute(node_p node, result_t *presult);

#endif /* MACH_PROTO_NCG_COMPUTE_H */

