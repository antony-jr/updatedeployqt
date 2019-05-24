#ifndef QMAKE_PROCESS_H_INCLUDED
#define QMAKE_PROCESS_H_INCLUDED
#include <stdio.h>

typedef struct _qmake_query_result_t {
    char *value;
    struct _qmake_query_result_t *next;
} qmake_query_result_t;

typedef struct {
    char *program_path;
    size_t program_path_offset;
    qmake_query_result_t *results_head;
} qmake_process_t;

qmake_process_t *qmake_process_create(const char*);
void qmake_process_destroy(qmake_process_t*);

qmake_query_result_t *qmake_process_query(qmake_process_t *, const char *);
int qmake_process_build_project(qmake_process_t*,const char*);
const char *qmake_query_result_value(qmake_query_result_t*);

#endif /* QMAKE_PROCESS_H_INCLUDED */
