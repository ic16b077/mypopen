
#ifndef MYPOPEN_H
#define MYPOPEN_H


/* includes */
#include <stdio.h>

/* prototypes */

extern FILE *mypopen(const char *command, const char *type);
extern int mypclose(FILE *stream);

#endif

/* EOF */