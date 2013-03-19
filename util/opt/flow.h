/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef UTIL_OPT_FLOW_H
#define UTIL_OPT_FLOW_H

/* util/opt/flow.c */
void flow(void);
void findreach(void);
void reach(line_p lnp);
void cleaninstrs(void);

#endif /* UTIL_OPT_FLOW_H */