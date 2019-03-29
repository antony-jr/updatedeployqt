#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <logger.h>
#include <appdir_context.h>

appdir_context_t *appdir_context_create(const char *path){
	appdir_context_t *p = NULL;
	struct stat *ds = NULL;
	if(!path)
		return NULL;


	/* - Check if the file exists
	 * - Check if we have permissions to read and write to it.
	**/
	if(access(path , F_OK)){
		printl(fatal , "the given appdir '%s' , doest not exists" , path);
		return NULL;
	}
	if(access(path , R_OK | W_OK)){
		printl(fatal , "you don't have read or write permissions to '%s'" , path);
		return NULL;
	}

	ds = calloc(1 , sizeof *ds);
	if(!ds || stat(path , ds)){
		printl(fatal , "cannot get stat for '%s'" , path);
		return  NULL;
	}

	/* Check file stats */

	free(ds);

	p = calloc(1 , sizeof *p);
	if(!p){
		printl(warning , "memory allocation error");
		return NULL;
	}
	p->dir_path = calloc(strlen(path) + 1 , sizeof *path);
	if(!p->dir_path){
		free(p);
		printl(warning , "memory allocation error");
		return NULL;
	}
	strcpy(p->dir_path , path);
	return p;
}

void appdir_context_destroy(appdir_context_t *p){
	if(!p)
		return;
	free(p->dir_path);
	free(p);
}
