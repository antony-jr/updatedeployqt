#ifndef APPDIR_CONTEXT_H_INCLUDED
#define APPDIR_CONTEXT_H_INCLUDED

typedef struct {
	char *dir_path;
	int flags;
} appdir_context_t;

appdir_context_t *appdir_context_create(const char*);
void appdir_context_destroy(appdir_context_t*); 

#endif // APPDIR_CONTEX_H_INCLUDED
