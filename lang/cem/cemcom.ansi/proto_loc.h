/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_CEM_CEMCOM_ANSI_PROTO_H
#define LANG_CEM_CEMCOM_ANSI_PROTO_H

/* lang/cem/cemcom.ansi/proto.c */
void check_for_void(register struct proto *pl);
void add_proto(struct proto *pl, struct decspecs *ds, struct declarator *dc, int lvl);
struct tag *gettag(struct type *tp, struct idf **idpp);
void declare_protos(register struct declarator *dc);
void update_proto(register struct type *tp, register struct type *otp);
void remove_proto_tag(struct type *tp);
void remove_proto_idfs(register struct proto *pl);
void call_proto(register struct expr **expp);

#endif /* LANG_CEM_CEMCOM_ANSI_PROTO_H */

