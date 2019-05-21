#ifndef DOWNLOADER_H_INCLUDED
#define DOWNLOADER_H_INCLUDED
#include <curl/curl.h>
typedef struct {
	CURL *handle;
	char *url;
	char *output;
} downloader_t;

downloader_t *downloader_create();
void downloader_destroy(downloader_t*);

void downloader_set_url(downloader_t*,const char*);
void downloader_set_destination(downloader_t*,const char*);
const char *downloader_get_url(downloader_t*);
int downloader_exec(downloader_t*);
#endif /* DOWNLOADER_H_INCLUDED */
