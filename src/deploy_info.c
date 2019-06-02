#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <deploy_info.h>
#include <logger.h>

static size_t get_line(char **ptr ,FILE *fp){
    if(!fp){
	    return 0;
    }
    int c = 0;
    size_t pos = 0;
    char *guard = NULL;
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
        guard = realloc(output,  sizeof(char) * (pos + 2));
        if(!guard) {
            free(output);
            return 0;
        }
        output = guard;
        ++pos;
    }
    return pos;
}

static char *get_system_library_path(){
	size_t len = 0;
	char *path = NULL;
	int skip = 5;
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

		char *buf = calloc( 1, sizeof(*buf) * (line + len - p + 2));
		char *bp = buf;
		while(*p && !isspace(*p)){
			*bp++ = *p++;	
		}
		path = buf;
		break;	
	}

	*(strrchr(path , '/')) = '\0';
	pclose(fp);
	return path;
}	

static int searchdir(char **store , char *dir , const char *pattern){
	struct dirent *de;
	char *dirp = NULL;
	DIR *dr = opendir(dir);
	if(dr == NULL){
		return -1;
	}
	while ((de = readdir(dr)) != NULL){
		if(!strcmp(de->d_name , ".") ||
		   !strcmp(de->d_name , "..")){
			continue;
		}

		if(de->d_type == DT_DIR){
			dirp = calloc(1 ,sizeof(*dirp) * (strlen(dir) + strlen(de->d_name) + 5));
			sprintf(dirp , "%s/%s" , dir , de->d_name);
			if(searchdir(store , dirp , pattern)){
				free(dirp);
				dirp = NULL;
				continue;
			}
			closedir(dr);
			return 0;
		}
		
		if(strstr(de->d_name , pattern)){
			*store = dir;
			closedir(dr);
			return 0;	
		}
	}
	closedir(dr);
	return -1;
}

static char *get_deploy_library_dir(char *deploy_dir){
	char *dir = NULL;
	searchdir(&dir , deploy_dir ,"libQt5Core");
	if(!dir){
		printl(warning , "cannot find deploy library path , might end up in error");
	}
	return dir;
}

static char *get_qxcb_plugin_path(char *deploy_dir){
	char *dir = NULL;
	size_t pos = 0;
	char *g = NULL;
	searchdir(&dir , deploy_dir , "libqxcb.so");
	if(!dir){
		printl(warning , "cannot file QXcb plugin in the deploy directory");
	}else{
		pos = strlen(dir);
		g = realloc(dir , sizeof(*dir) * (strlen(dir) + 20));
		if(!g){
			printl(fatal , "not enough memory");
			return NULL;
		}
		dir = g;
		sprintf(dir + pos , "/libqxcb.so" );	
	}
	return dir;
}

static char *get_deploy_plugins_path(char *libqxcb_path){
	const char *ins = NULL;
	char *buf = calloc(1 , sizeof(*buf) * strlen(libqxcb_path));
	if(!buf){
		return NULL;
	}
	ins = strstr(libqxcb_path, "plugins");
	strncpy(buf, libqxcb_path, ins - libqxcb_path + 7);
	return buf;
}

deploy_info_t *deploy_info_create(const char *deploy_dir){
	size_t t = 0;
	deploy_info_t *obj = NULL;
	if(!deploy_dir){
		return NULL;
	}

	if(!(obj = calloc(1 , sizeof(*obj)))){
		printl(fatal , "cannot allocate space for getting deploy information");
		return NULL;
	}

	obj->deploy_dir = calloc(1 , sizeof(*(obj->deploy_dir)) * strlen(deploy_dir));
	if(!obj->deploy_dir){
		printl(fatal , "not enough space");
		deploy_info_destroy(obj);
		return NULL;
	}
	strcpy(obj->deploy_dir , deploy_dir);

	/* Remove '/' or '\' at the end if found. */
	t = strlen(obj->deploy_dir) - 1;
	if((obj->deploy_dir)[t] == '/' || 
	   (obj->deploy_dir)[t] == '\\'){
		(obj->deploy_dir)[t] = '\0';
	}
	return obj;
}

void deploy_info_destroy(deploy_info_t *obj){
	if(!obj){
		return;
	}
	if(obj->deploy_dir){
		free(obj->deploy_dir);
	}
	if(obj->system_lib_dir){
		free(obj->system_lib_dir);
	}
	if(obj->deploy_lib_dir){
		free(obj->deploy_lib_dir);
	}
	if(obj->deploy_plugins_dir){
		free(obj->deploy_plugins_dir);
	}
	if(obj->deployed_libqxcb_path){
		free(obj->deployed_libqxcb_path);
	}
	free(obj);
	return;
}


int deploy_info_gather(deploy_info_t *obj){
	if(!obj){
		printl(fatal , "invalid deploy info instance given");
		return -1;
	}

	/* TODO:
	 *  Also check for qt.conf file first , if found then read it 
	 *  to guess the required directories and files. 
	 *  If that fails then search the entire directory. */

	/* Check if the deploy directory exists. */
	if(access(obj->deploy_dir , F_OK)){
		printl(fatal , "%s directory does not exists , giving up" , obj->deploy_dir);
		return -1;
	}
	if(access(obj->deploy_dir , W_OK | R_OK)){ /* also check permissions */
		printl(fatal , "you have no permission to read or write in %s" , obj->deploy_dir);
		return -1;
	}

	/* lets get the system library directory. */
	obj->system_lib_dir = get_system_library_path();
	if(!obj->system_lib_dir){
		printl(fatal , "cannot get system library directory");
		return -1;
	}

	/* lets get the deploy library dir */
	obj->deploy_lib_dir = get_deploy_library_dir(obj->deploy_dir);
	if(!obj->deploy_lib_dir){
		printl(fatal , "cannot get deploy library directory");
		return -1;
	}

	obj->deployed_libqxcb_path = get_qxcb_plugin_path(obj->deploy_dir);
	if(!obj->deployed_libqxcb_path){
		printl(fatal , "cannot get path to libqxcb.so");
		return -1;
	}

	/* lets get the deploy plugins dir */
	obj->deploy_plugins_dir = get_deploy_plugins_path(obj->deployed_libqxcb_path);
	if(!obj->deploy_plugins_dir){
		printl(fatal , "cannot get deploy plugins directory");
		return -1;
	}
	return 0;
}

const char *deploy_info_system_library_directory(deploy_info_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->system_lib_dir;
}

const char *deploy_info_library_directory(deploy_info_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->deploy_lib_dir;
}

const char *deploy_info_plugins_directory(deploy_info_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->deploy_plugins_dir;
}

const char *deploy_info_qxcb_plugin_path(deploy_info_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->deployed_libqxcb_path;
}
