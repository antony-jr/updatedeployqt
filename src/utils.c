#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <utils.h>
#include <ctype.h>
#include <logger.h>

size_t get_line(char **ptr ,FILE *fp){
    if(!fp){
	    return 0;
    }
    int c = 0;
    size_t pos = 0;
    char *output_guard = NULL;
    char *output = calloc(1, sizeof(*output) * 2);
    if(!output){
	    return 0;
    }

    while((c=getc(fp)) != EOF) {
        if(c == '\n'){
		*ptr = output;
		break;
	}
	*(output + pos) = c;
        output_guard = realloc(output,  sizeof(char) * (pos + 2));
        if(!output_guard) {
            free(output);
            return 0;
        }
        output = output_guard;
        ++pos;
    }
    return pos;
}

char *get_system_library_path(){
	size_t len = 0;
	size_t path_len = 0;
	int skip = 5;
	char *path = NULL;
	char *line = NULL;
	FILE *fp = popen("ldconfig -p" , "r");
	if(!fp){
		return NULL;
	}

	while((len = get_line(&line , fp))){
		if(skip){
			--skip;
			free(line);
			continue;
		}

		char *p = strstr(line , "=>");
		p += 2; /* get past '=>' */

		while(*p && isspace(*p)){
			++p;
		}

		path_len = line + len - p + 2;
		char *buf = calloc( 1, sizeof(*buf) * path_len);
		char *bp = buf;
		while(*p && !isspace(*p)){
			*bp++ = *p++;	
		}
		path = buf;
		break;	
	}

	while(path_len){
		if(*(path + path_len) == '/'){
			*(path + path_len) = '\0';
			break;
		}
		*(path + path_len) = '\0';
		--path_len;
	}
	pclose(fp);
	return path;
}	

