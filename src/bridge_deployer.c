#include <bridge_deployer.h>
#include <config_manager.h>
#include <downloader.h>
#include <deploy_info.h>
#include <logger.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* placeholders present in the binary. */
const char *interval_placeholder = "75629552e6e8286442676be60e7da67d";
const char *qmenu_name_placeholder = "871abbc22416bb25429594dec45caf1f";
const char *qmenubar_name_placeholder = "bfa40825ef36e05bbc2c561595829a92";
const char *qpushbutton_name_placeholder = "930b29debfb164461b39342d59e2565c";
const char *boolean_string_placeholder = "4c6160c2d6bfeba1";

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

static int find_offset_and_write(FILE *fp, const char *to_find, const char *replace, size_t to_write){
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



bridge_deployer_t *bridge_deployer_create(config_manager_t *manager,
					  downloader_t *downloader,
					  deploy_info_t *info){
	bridge_deployer_t *obj = NULL;
	if(!manager || !downloader || !info){
		printl(fatal , "invalid configuration manager or downloader or deploy information");
		return NULL;
	}

	if(!(obj = calloc(1 , sizeof(*obj)))){
		printl(fatal , "not enough memory");
		return NULL;
	}
	obj->manager = manager;
	obj->downloader = downloader;
	obj->info = info;
	return obj;
}

void bridge_deployer_destroy(bridge_deployer_t *obj){
	if(!obj){
		return;
	}
	
	if(obj->bridge_path){
		free(obj->bridge_path);
	}
	free(obj);
}

const char *bridge_deployer_get_bridge_path(bridge_deployer_t *obj){
	if(!obj ||
	   !obj->bridge_path){
		return NULL;
	}
	return obj->bridge_path;
}

int bridge_deployer_run(bridge_deployer_t *obj){
	if(!obj){
		return -1;
	}
	FILE *fp = NULL;
	int tries = 0,
	    iter = 0,
	    r = 0;
	char *buffer = NULL;
	const char *p = NULL;
	const char *url_template = "https://github.com/TheFutureShell/QtUpdateBridges/"
		                   "releases/download/continuous/lib%sBridge.so";
	const char *plugins_dir = deploy_info_plugins_directory(obj->info);
	const char *bridge_name = config_manager_get_bridge_name(obj->manager);
	char *bridge_path = NULL;
	char *bridge_url = NULL;
	if(!plugins_dir || 
	   !bridge_name){
		return -1;
	}
	
	bridge_path = calloc(1 ,sizeof(*bridge_path) * (strlen(plugins_dir) + 100));
	bridge_url = calloc(1 , sizeof(*bridge_url) * (strlen(url_template) + 100));

	if(!bridge_path ||
	   !bridge_url){
		printl(fatal , "not enough memory");
		return -1;
	}

	/* Check if its a valid bridge. */
	if(strcmp(bridge_name , "AppImageUpdater") &&
	   strcmp(bridge_name , "QInstaller") &&
	   strcmp(bridge_name , "GHReleases")){
		/* All possible bridge name mismatches. */
		free(bridge_path);
		free(bridge_url);
		printl(fatal , "unknown updater bridge , please fix it");
		return -1;
	}

	/* Determine the path to save and url. */
	sprintf(bridge_path , "./%s/lib%sBridge.so" , plugins_dir , bridge_name);
	sprintf(bridge_url , url_template , bridge_name);
	

	/* Now download the files and save it. */
	downloader_set_url(obj->downloader , bridge_url);
	downloader_set_destination(obj->downloader , bridge_path);

	/* We don't need the bridge_url , since 
	 * the downloader will copy them. */
	free(bridge_url);

#if 0
	/* run the downloader , if fails try four times. */
	while(1){
		printl(info , "downloading %s bridge from upstream.." , bridge_name);
		if(downloader_exec(obj->downloader) < 0){
			if(tries > 4){
				printl(info , "downloading bridge from upstream failed");
				free(bridge_path);
				return -1;
			}
			printl(info , "download failed for unknown reason , retrying..");
			++tries;
			continue;
		}
		break;
	}
#endif 
	/* Now write configuration to the bridge machine code directly.
	 * We will use predefined placeholders in the binary to replace them 
	 * with our valid configuration. */

	/* Lets first open the file. */
	if(!(fp = fopen(bridge_path , "r+"))){
		printl(fatal , "cannot open %s for reading and writing" , bridge_path);
		free(bridge_path);
		return -1;
	}

	/* Save bridge path for later use. */
	if(obj->bridge_path){
		free(obj->bridge_path);
	}
	obj->bridge_path = bridge_path;

	/* First lets write the boolean_string into the binary. */
	if(!config_manager_get_boolean_string(obj->manager)){
		printl(fatal , "cannot get the options from updatedeployqt.json");
		fclose(fp);
		return -1;
	}

	p = config_manager_get_boolean_string(obj->manager);
	buffer = calloc(1 , sizeof(*buffer) * 17); /* TODO: change this once the boolean string has constant length. */	
	for(iter = 0; iter < CONFIG_MANAGER_BOOLEAN_STRING_LEN ;++iter){
		buffer[iter] = *p++;
	}

	r = find_offset_and_write(fp , boolean_string_placeholder , buffer , 17);
	if(r <= 0){
		printl(fatal , "failed to write boolean string to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

	/* Now write QMenuBar QObject name if available. */
	if(!(p = config_manager_get_qmenubar_name(obj->manager))){
		goto qmenu_name_write;
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	for(iter = 0; iter < CONFIG_MANAGER_OBJECT_STRING_LEN ; ++iter){
		buffer[iter] = *p++;
	}

	r = find_offset_and_write(fp , qmenubar_name_placeholder , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	if(r <= 0){
		printl(fatal , "failed to write QMenuBar QObject name to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

qmenu_name_write:
	/* Now write QMenu QObject name if available. */
	if(!(p = config_manager_get_qmenu_name(obj->manager))){
		goto qpushbutton_name_write;
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	for(iter = 0; iter < CONFIG_MANAGER_OBJECT_STRING_LEN ; ++iter){
		buffer[iter] = *p++;
	}

	r = find_offset_and_write(fp , qmenu_name_placeholder , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	if(r <= 0){
		printl(fatal , "failed to write QMenu QObject name to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

qpushbutton_name_write:
	/* Now write QPushButton QObject name if available. */
	if(!(p = config_manager_get_qpushbutton_name(obj->manager))){
		goto interval_write;		
	}
	
	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	for(iter = 0; iter < CONFIG_MANAGER_OBJECT_STRING_LEN ; ++iter){
		buffer[iter] = *p++;
	}

	r = find_offset_and_write(fp , qmenu_name_placeholder , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	if(r <= 0){
		printl(fatal , "failed to write QMenu QObject name to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

interval_write:
	if((r = config_manager_get_interval(obj->manager)) <= 0){
		r = 1 * 24 * 3600 * 1000; /* 1 day interval */
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	sprintf(buffer , "%i" , r);

	r = find_offset_and_write(fp , interval_placeholder , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	if(r <= 0){
		printl(fatal , "failed to write update check interval to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);
	fclose(fp);
	return 0;
}
