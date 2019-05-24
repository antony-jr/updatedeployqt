#include <qmake_process.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *strlower(char *buf, const char *s) {
    while(*s) {
        *buf++ = tolower(*s++);
    }
    *buf = '\0';
    return buf;
}

static qmake_query_result_t *qmake_query_result_create(char *value) {
    qmake_query_result_t *qr = calloc(1, sizeof(qmake_query_result_t));
    if(!qr) {
        return NULL;
    }
    qr->next = NULL;
    qr->value = value;
    return qr;
}

static void qmake_query_result_destroy(qmake_query_result_t **p) {
    qmake_query_result_t *to_free = NULL;
    while(*p) {
        if((*p)->value) {
            free((*p)->value);
        }
        /*
         * using p = &((to_free = *p)->next);
         * throws some context errors in valgrind , don't
         * know what I'm doing wrong.
        */
        *p = (to_free = *p)->next;
        free(to_free);
    }
}

static short check_prog(const char *prog) {
    FILE *fp = NULL;
    char buf[50];
#ifdef __linux__
    if(!(fp = popen(prog, "r"))) {
#else
    if(1) {
#endif
        return -1;
    }
    if(feof(fp)) {
        pclose(fp);
        return -1;
    }
    fread(buf, (sizeof buf) - 1, sizeof *buf, fp);
    buf[(sizeof buf) - 1] = '\0';
    strlower(buf, buf);
    if(strstr(buf, "command not found") ||
            !strstr(buf,  "qmake")) {
        pclose(fp);
        return -1;
    }

    pclose(fp);
    return 0;
}

qmake_process_t *qmake_process_create(const char *prog) {
    if(!prog || check_prog(prog))
        return NULL;
    size_t len = strlen(prog);
    qmake_process_t *qp = calloc(1, sizeof(qmake_process_t));
    if(!qp)
        return NULL;
    qp->results_head = NULL;
    qp->program_path = calloc(len + 100, sizeof(char));
    if(!(qp->program_path)) {
        free(qp);
        return NULL;
    }
    strcpy(qp->program_path, prog);
    *(qp->program_path + len) = ' ';
    qp->program_path_offset = len;
    return qp;
}

void qmake_process_destroy(qmake_process_t *qp) {
    if(!qp)
        return;
    qmake_query_result_destroy(&qp->results_head);
    if(qp->program_path) {
        free(qp->program_path);
    }
    free(qp);
}

int qmake_process_build_project(qmake_process_t *qp , const char *project_file){
	if(!qp || !project_file){
		return -1;
	}
	FILE *fp = NULL;

	/* Set the command */	
	sprintf(qp->program_path + qp->program_path_offset , " %s" , project_file);


	/* execute command. */
	if(!(fp = popen(qp->program_path, "r"))) {
		return -1;
	}

	while(!feof(fp)){
		(void)getc(fp);
	}
	pclose(fp); /* We don't need the output. */
	
	if(!(fp = popen("make -j4 && make -j$(nproc)" , "r"))){
		return -1;
	}
	while(!feof(fp)){
		(void)getc(fp);
	}
	pclose(fp);
	return 0;
}

qmake_query_result_t *qmake_process_query(qmake_process_t *qp, const char *qry) {
    if(!qp || !qry){
    	return NULL;
    }
    FILE *fp = NULL;
    qmake_query_result_t **p = NULL;
    qmake_query_result_t *qr = NULL;
    int c = 0;
    int pos = 0;
    char *output_guard = NULL;
    char *output = calloc(1, sizeof(char));

    /* append query to the program. */
    sprintf(qp->program_path + qp->program_path_offset , " -query %s" , qry);

    /* execute command. */
#ifdef __linux__
    if(!(fp = popen(qp->program_path, "r"))) {
#else
    if(1) {
#endif
        free(output);
        return NULL;
    }

    while(!feof(fp)) {
        c = getc(fp);
        *(output + pos) = c;
        output_guard = realloc(output,  sizeof(*output) * (pos + 2));
        if(!output_guard) {
            free(output);
            return NULL;
        }
        output = output_guard;
        ++pos;
    }
    *(output + pos - 2) = '\0'; /* remove newline and terminate. */
    if(!(qr = qmake_query_result_create(output))) {
        free(output);
        return NULL;
    }

    p = &qp->results_head;
    while(*p) {
        p = &((*p)->next);
    }
    *p = qr;
    pclose(fp);
    return qr;
}

const char *qmake_query_result_value(qmake_query_result_t *qr) {
    if(!qr)
        return NULL;
    return qr->value;
}
