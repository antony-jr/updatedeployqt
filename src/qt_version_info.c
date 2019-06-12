#include <qt_version_info.h>
#include <logger.h>
#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

int find_qt_minor_version(FILE *fp){
    const char *pattern = "qt_version_tag_5_";
    const size_t atmost_read = strlen(pattern) + 2;
    const size_t tomove = strlen(pattern);
    int minor = 0;
    int temp = 0;
    char *tag = NULL;
    char *buffer = NULL;
    tag = calloc(1 , sizeof(*tag) * (atmost_read + 2));
    buffer = calloc(1 , sizeof(*buffer) * (atmost_read + 2) );
    if(!buffer){
	    return NULL;
    }
    if(!fp) {
        free(buffer);
	return NULL;
    }

    rewind(fp);

    /* Check if we have an empty file. */
    if(feof(fp)) {
	free(buffer);
        return NULL;
    }

    while(1) {
        memset(buffer, 0, sizeof(*buffer) * (atmost_read + 2));
        if(read_bytes(fp , &buffer , atmost_read) == 0){
		break;
	}
        if(strstr(buffer, pattern)) {
	    size_t iter = 0;
	    while(iter < atmost_read){
		    if(!isascii(buffer[iter])){
			    buffer[iter] = '\0';
		    }
		    ++iter;
	    }
	    if(strlen(buffer) > 14){
		    strcpy(tag , buffer);
		    if(*(tag + tomove) - '0' > 6 || 
		       *(tag + tomove + 1) != '\0'){
			    temp = *(tag + tomove + 1) != '\0' ? 10 :
				   *(tag + tomove) - '0'; 
			    
			    if(temp > minor){
				    minor = temp;
			    }
			    break;
		    }
	    }
	}
    }

    free(tag);
    free(buffer);
    return minor;
}


qt_version_info_t *qt_version_info_create(const char *qtcore_path){
	FILE *fp = NULL;
	qt_version_info_t *obj = NULL;
	if(!qtcore_path){
		return NULL;
	}

	if(!(obj = calloc(1 , sizeof(*obj)))){
		printl(fatal , "not enough memory");
		return NULL;
	}

	if(access(qtcore_path , F_OK)){
		printl(fatal , "%s does not exists" , qtcore_path);
		qt_version_info_destroy(obj);
		return NULL;
	}

	if(access(qtcore_path , R_OK)){
		printl(fatal , "you don't have the permissions to read %s" , qtcore_path);
		qt_version_info_destroy(obj);
		return NULL;
	}

	if(!(fp = fopen(qtcore_path , "r"))){
		printl(fatal , "cannot open %s for reading" , qtcore_path);
		qt_version_info_destroy(obj);
		return NULL;
	}

	obj->minor = find_qt_minor_version(fp);
	fclose(fp);

	if(!obj->minor){
		printl(fatal , "unable to find suitable qt version");
		printl(fatal , "please try explicitly mentioning it in updatedeployqt.json");
		qt_version_info_destroy(obj);
		return NULL;
	}
	
	obj->version_str = calloc(1 , sizeof(*(obj->version_str)) * 10);
	sprintf(obj->version_str, "5.%d.0" , obj->minor);
	return obj;

}

void qt_version_info_destroy(qt_version_info_t *obj){
	if(!obj){
		return;
	}

	if(obj->version_str){
		free(obj->version_str);
	}
	return;
}

const char *qt_version_info_get_version(qt_version_info_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->version_str;
}

int qt_version_info_get_minor(qt_version_info_t *obj){
	if(!obj){
		return -1;
	}
	return obj->minor;
}


