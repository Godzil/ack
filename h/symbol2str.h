/*
 * The Amsterdam Compiler Kit
 * See the copyright notice in the ACK home directory, in the file "Copyright".
 */

#ifndef H_SYMBOL2STR_H
#define H_SYMBOL2STR_H

/* A lot of component use a version of this function, there is currently no header file
 * that contain it, and all use the same prototype as they are all generated from the same
 * generator.
 */
char *symbol2str(int tok);

#endif