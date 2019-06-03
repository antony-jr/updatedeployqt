#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED
#include <stdio.h>
int copy_file(const char *,const char*);
int find_offset_and_write(FILE *, const char *, const char *, size_t);
#endif /* UTILS_H_INCLUDED */
