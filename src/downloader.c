#include <downloader.h>
#include <stdlib.h>
#include <string.h>
#include <logger.h>

downloader_t *downloader_create(){
	downloader_t *obj = calloc(1 , sizeof *obj);
	if(!obj){
		return NULL;
	}

	obj->handle = curl_easy_init();
	if(!obj->handle){
		free(obj);
		return NULL;
	}
	return obj;
}

void downloader_destroy(downloader_t *obj){
	if(!obj){
		return;
	}

	if(obj->output){
		free(obj->output);
	}
	if(obj->url){
		free(obj->url);
	}
	curl_easy_cleanup(obj->handle);
	free(obj);
}

void downloader_set_url(downloader_t *obj, const char *url){
	if(!obj || !url){
		return;
	}

	if(obj->url){
		free(obj->url);
		obj->url = NULL;
	}

	size_t len = strlen(url);
	obj->url = calloc(1 , (sizeof(*url) * len) + 1);
	if(!obj->url){
		return;
	}
	strcpy(obj->url , url);
}


void downloader_set_destination(downloader_t *obj,const char *dest){
	if(!obj || !dest){
		return;
	}

	/* Free previously allocated strings. */
	if(obj->output){
		free(obj->output);
		obj->output = NULL;
	}

	size_t len = strlen(dest);
	obj->output = calloc(1 , (sizeof(*dest) * len) + 1);
	if(!obj->output){
		return;
	}
	strcpy(obj->output , dest);
}


const char *downloader_get_url(downloader_t *obj){
	return (!obj) ? NULL : obj->url;
}

int downloader_exec(downloader_t *obj){
	if(!obj || !obj->url || !obj->output){
		return -1;
	}
	
	CURLcode res;
        FILE *fp = fopen(obj->output, "wb");
        if(!fp){
		printl(fatal , "cannot open '%s'" , obj->output);
		return -1;
	}
	
	
	curl_easy_setopt(obj->handle, CURLOPT_URL, obj->url);
        curl_easy_setopt(obj->handle, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(obj->handle, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(obj->handle, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(obj->handle);
        fclose(fp);

	if(res != CURLE_OK){
		printl(fatal , "download failed because , %s" , curl_easy_strerror(res));
		return -1;
	}
	return 0;
}

