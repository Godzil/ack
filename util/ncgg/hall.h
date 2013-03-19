/*
 * (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */
#ifndef LANG_NCGG_HALL_H
#define LANG_NCGG_HALL_H

/* util/ncgg/hall.c */
void hallverbose(void);
void inithall(void);
void nexthall(short *sp);
int card(short *sp);
void checkhall(void);
int hall(void);
int recurhall(int nhallsets, short hallsets[][(((40 +100)+15)/16)]);
void unite(short *sp, short *into);

#endif /* LANG_NCGG_HALL_H */