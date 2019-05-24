#ifndef LDD_PROCESS_H_INCLUDED
#define LDD_PROCESS_H_INCLUDED
#include <stdio.h>

typedef struct _ldd_query_result_t {
    char *library;
    char *path;
    struct _ldd_query_result_t *next;
} ldd_query_result_t;

typedef struct {
    char *program_path;
    size_t program_path_offset;
    ldd_query_result_t *results_head;
} ldd_process_t;

ldd_process_t *ldd_process_create(const char*);
void ldd_process_destroy(ldd_process_t*);

ldd_query_result_t *ldd_process_get_required_libs(ldd_process_t *, const char *);
const char *ldd_query_result_library(ldd_query_result_t*);
const char *ldd_query_result_library_path(ldd_query_result_t*);
#endif /* LDD_PROCESS_H_INCLUDED */
