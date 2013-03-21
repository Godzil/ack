/*
 * This file contains the structures that hold information about the labels.
 * The height and the fallthrough functions of the table. 
 *
 * Author: Hans van Eck. 
 */
#ifndef MACH_PROTO_NCG_LABEL_H
#define MACH_PROTO_NCG_LABEL_H

typedef struct label *label_p;

struct label {
	label_p	lb_next;
	word	lb_number;
	int	lb_height;
	short	lb_fallthrough;
};

void add_label(int num, int height, int flth);
label_p get_label(word num);
void kill_labels();

#endif /* MACH_PROTO_NCG_LABEL_H */