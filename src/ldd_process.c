#include <ldd_process.h>
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

static ldd_query_result_t *ldd_query_result_create(char *library , char *path) {
    ldd_query_result_t *qr = calloc(1, sizeof(ldd_query_result_t));
    if(!qr) {
        return NULL;
    }
    qr->next = NULL;
    qr->path = path;
    qr->library = library;
    return qr;
}

static void ldd_query_result_destroy(ldd_query_result_t **p) {
    ldd_query_result_t *to_free = NULL;
    while(*p) {
        if((*p)->path) {
            free((*p)->path);
        }
	if((*p)->library){
	    free((*p)->library);
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

ldd_process_t *ldd_process_create(const char *prog) {
    if(!prog)
        return NULL;
    size_t len = strlen(prog);
    ldd_process_t *qp = calloc(1, sizeof(*qp));
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

void ldd_process_destroy(ldd_process_t *qp) {
    if(!qp)
        return;
    ldd_query_result_destroy(&qp->results_head);
    if(qp->program_path) {
        free(qp->program_path);
    }
    free(qp);
}

ldd_query_result_t *ldd_process_get_required_libs(ldd_process_t *qp, const char *path) {
    if(!qp || !path){
    	return NULL;
    }
    FILE *fp = NULL;
    ldd_query_result_t **p = NULL;
    ldd_query_result_t *qr = NULL;
    int c = 0;
    int pos = 0;
    char *output_guard = NULL;
    char *output = calloc(1, sizeof(char));

    /* append query to the program. */
    sprintf(qp->program_path + qp->program_path_offset , " %s" , path);

    /* execute command. */
#ifdef __linux__
    if(!(fp = popen(qp->program_path, "r"))) {
#else
    if(1) {
#endif
        free(output);
        return NULL;
    }

    p = &qp->results_head;
    while(*p) {
        p = &((*p)->next);
    }

    while((c = getc(fp)) != EOF) {	    
	if(c == '\n'){
		*(output + pos) = '\0'; /* remove newline and terminate. */
		output_guard = output;

		char *lib = calloc(1 , sizeof(*lib) * (pos/2));
		char *libPath = calloc(1 , sizeof(*lib)*pos);
		char *libp = lib;
		char *libPathp = libPath;

		/* Skip whitespaces. */
		while(*output_guard && isspace(*output_guard)){
			++output_guard;
		}

		/* Read till whitespace. */
		while(*output_guard && !isspace(*output_guard)){
			*libp++ = *output_guard++;
		}


		/* Skip whitespaces and => */
		while(*output_guard && (isspace(*output_guard) || *output_guard == '>' || *output_guard == '=')){
			++output_guard;
		}

		while(*output_guard && !isspace(*output_guard)){
			*libPathp++ = *output_guard++;
		}
	
		
		free(output);	
		if(!(qr = ldd_query_result_create(lib , libPath))) {
			free(lib);
			free(libPath);
			return NULL;
		}

		*p = qr;
		p = &((*p)->next);
	
		pos = 0;
		output_guard = NULL;
		output = calloc(1, sizeof(char));
		continue;
	}

        *(output + pos) = c;
        output_guard = realloc(output,  sizeof(*output) * (pos + 2));
        if(!output_guard) {
            free(output);
            return NULL;
        }
        output = output_guard;
        ++pos;
    }
    pclose(fp); 
    return qp->results_head;
}

const char *ldd_query_result_library(ldd_query_result_t *qr) {
    if(!qr)
        return NULL;
    return qr->library;
}

const char *ldd_query_result_library_path(ldd_query_result_t *qr){
    if(!qr)
        return NULL;
    return qr->path;
}
