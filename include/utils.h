#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED
#include <stdio.h>
char *get_bundled_data_file(const char*);
int copy_file(const char *,const char*);
int read_bytes(FILE * , char ** , size_t);
int find_offset_and_write(FILE *, const char *, const char *, size_t);
int write_string_as_file(const char *, size_t , const char*);
#endif /* UTILS_H_INCLUDED */
