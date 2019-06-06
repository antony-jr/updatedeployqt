#include <injector.h>
#include <bridge_deployer.h>
#include <logger.h>
#include <qt_version_info.h>
#include <md5.h>
#include <utils.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* placeholders to replace in binary. */
const char *plugin_to_load_md5sum_placeholder = "8cfaddf5b1a24d1fd31cab97b01f1f87";
const char *slot_to_call_placeholder = "f80b03178d4080a30c14e71bbbe6e31b";


/* internal configuration */
const char *slot_to_call = "initAutoUpdate(void)";

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
	qt_version_info_t *qt_ver_info = NULL;
	if(!obj ||
	   !deploy_info_qxcb_plugin_path(obj->bridge->info)){
		return -1;
	}

	/* Get Qt version that is targeted. */
	if(!(qt_version = config_manager_get_qt_version(obj->bridge->manager))){
		/* Qt version is not given in updatedeployqt.json , 
		 * lets get it from libQt5Core in the deploy dir */

		char *path = calloc(1 , sizeof(*path) * (strlen(deploy_info_library_directory(obj->bridge->info)) + 20));
		sprintf(path , "%s/libQt5Core.so.5" , deploy_info_library_directory(obj->bridge->info));

		qt_ver_info = qt_version_info_create(path);
		if(!qt_ver_info){
			printl(fatal , "cannot determine qt version to target");
			return -1;
		}

		qt_version = qt_version_info_get_version(qt_ver_info);
	}

	/* Now lets download the modified qxcb plugin from upstream. */
	printl(info , "downloading modified qxcb plugin for qt version %s" , qt_version);

	if(qt_version[0] != '5' ||
	   ((qt_version[2] - '0') < 6 && qt_version[3] == '.')){
		printl(fatal , "your qt version is yet not supported , aborting.");
		qt_version_info_destroy(qt_ver_info);
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
	qt_version_info_destroy(qt_ver_info);

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

