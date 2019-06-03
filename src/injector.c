#include <injector.h>
#include <bridge_deployer.h>
#include <logger.h>
#include <qmake_process.h>
#include <md5.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* placeholders to replace in binary. */
const char *plugin_to_load_md5sum_placeholder = "8cfaddf5b1a24d1fd31cab97b01f1f87";
const char *slot_to_call_placeholder = "f80b03178d4080a30c14e71bbbe6e31b";


/* internal configuration */
const char *slot_to_call = "initAutoUpdate(void)";

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



injector_t *injector_create(const char *qmake , bridge_deployer_t *bridge){
	injector_t *obj = NULL;
	if(!bridge){
		printl(fatal , "invalid bridge deployer or args parser given , internal error");
		return NULL;
	}

	if(!(obj = calloc(1 , sizeof(*obj)))){
		printl(fatal , "not enough memory");
		return NULL;
	}
	obj->qmake = qmake;
	obj->bridge = bridge;
	return obj;
}

void injector_destroy(injector_t *obj){
	if(!obj){
		return;
	}
	free(obj);
}

int injector_run(injector_t *obj){
	FILE *fp = NULL;
	int tries = 0,
	    r = 0;
	char *p = NULL;
	const char *qt_version = NULL;
	const char *url_template = "https://github.com/TheFutureShell/QtPluginInjector/"
		                   "releases/download/continuous-Qt%s/libqxcb.so";
	qmake_process_t *qmake = NULL;
	if(!obj ||
	   !deploy_info_qxcb_plugin_path(obj->bridge->info)){
		return -1;
	}

	/* Get Qt version that is targeted. */
	if(!(qt_version = config_manager_get_qt_version(obj->bridge->manager))){
		/* Qt version is not given in updatedeployqt.json , 
		 * lets try guessing the Qt version from the qmake installed 
		 * in the system or the one given by the user. */

		if(!(qmake = qmake_process_create(!obj->qmake ? "qmake" : obj->qmake))){
			printl(fatal , "cannot determine the qt version");
			return -1;
		}
		qt_version =  qmake_query_result_value(qmake_process_query(qmake , "QT_VERSION"));
		printl(info , "targeting qt version: %s" , qt_version);
	}

	/* Now lets download the modified qxcb plugin from upstream. */
	printl(info , "downloading modified qxcb plugin for qt version %s" , qt_version);

	if(qt_version[0] != '5' ||
	   (('0' - qt_version[2]) < 6 && qt_version[3] == '.')){
		printl(fatal , "your qt version is yet not supported , aborting.");
		qmake_process_destroy(qmake);
		return -1;
	}

	if(qt_version[0] == '5' &&
           qt_version[2] == '1' &&
	   qt_version[3] != '.'){ /* Qt 5.1y.xx */
	   /* for now we only use Qt 5.10.0 */
	   p = (char*)qt_version;
	   p[3] = '0';
	   p[5] = '0';
	   p[6] = '\0';	   
	}else{ /* Qt 5.y.xx */
	   p = (char*)qt_version;
	   p[4] = '0';
	   p[5] = '\0';
	}

	p = calloc( 1, sizeof(*p) * (strlen(url_template) + strlen(qt_version) + 20));
	sprintf(p , url_template , qt_version);

	downloader_set_url(obj->bridge->downloader , p);
	free(p);
	qmake_process_destroy(qmake);

	downloader_set_destination(obj->bridge->downloader , deploy_info_qxcb_plugin_path(obj->bridge->info));

	/* download the file */
	while(1){
		printl(info , "downloading qt plugin injector from upstream..");
		if(downloader_exec(obj->bridge->downloader) < 0){
			if(tries > 4){
				printl(info , "downloading qt plugin injector from upstream failed");
				return -1;
			}
			printl(info , "download failed for unknown reason , retrying..");
			++tries;
			continue;
		}
		break;
	}

	/* Now write the configuration into the qxcb plugin. */
	/* Lets first open the file. */
	if(!(fp = fopen(deploy_info_qxcb_plugin_path(obj->bridge->info) , "r+"))){
		printl(fatal , "cannot open %s for reading and writing" , deploy_info_qxcb_plugin_path(obj->bridge->info));
		return -1;
	}

	/* Lets compute MD5 sum for the bridge we are deploying. */
	p = calloc(1 , sizeof(*p) * 33); /* MD5 will not exceed 33 bytes to save. */
	if(!p){
		printl(fatal , "not enough memory");
		return -1;
	}
	if(0 > MD5File(bridge_deployer_get_bridge_path(obj->bridge) , p)) {
		printl(fatal, "cannot compute md5sum for %s", 
		       bridge_deployer_get_bridge_path(obj->bridge));
		free(p);
		return -1;
	}

	r = find_offset_and_write(fp , plugin_to_load_md5sum_placeholder , p , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	if(r <= 0){
		printl(fatal , "cannot write md5sum of the bridge into qxcb plugin");
		free(p);
		return -1;
	}
	free(p); /* we don't need this anymore */

	r = find_offset_and_write(fp , slot_to_call_placeholder , slot_to_call , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	if(r <= 0){
		printl(fatal , "cannot write internal configuration to qxcb plugin");
		return -1;
	}
	return 0;
}

