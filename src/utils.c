#include <utils.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int read_bytes(FILE *fp , char **buffer , size_t n){
	long int pos = 0;
	int r = 0;
	pos = ftell(fp); /* Make it as if one byte is only read. */
	r = fread(*buffer , sizeof(**buffer) , n , fp);
	fseek(fp , pos , SEEK_SET);
	if(getc(fp) == EOF){
		return 0; 
	}
	return (r<=0) ? 0 : r;
}

int find_offset_and_write(FILE *fp, const char *to_find, const char *replace, size_t to_write){
    char *buffer = NULL;
    int r = 0;
    buffer = calloc(1 , sizeof(*buffer) * to_write);
    if(!buffer){
	    return -1;
    }
    if(!replace || !to_find || !fp) {
        free(buffer);
	return -1;
    }

    rewind(fp);

    /* Check if we have an empty file. */
    if(feof(fp)) {
	free(buffer);
        return -1;
    }

    while(1) {
        memset(buffer, 0, sizeof(*buffer) * to_write);
        if(read_bytes(fp , &buffer , to_write) == 0){
		break;
	}
        if(!strncmp(buffer, to_find , to_write)) {
            fseek(fp, (ftell(fp) - 1), SEEK_SET);
            fwrite(replace, sizeof *replace, to_write, fp);
            ++r;
        }
    }
    free(buffer);
    return r;
}
