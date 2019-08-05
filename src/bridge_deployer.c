#include <bridge_deployer.h>
#include <config_manager.h>
#include <deploy_info.h>
#include <logger.h>
#include <utils.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef BINARIES_PACKAGED_HEADER_AVAL
#include <aiub.h> /* AppImageUpdaterBridge binary encoded as a header */
#endif

/*
 * Placeholders present in all bridges.
 *
 * interval - The interval to init the update check on startup of the 
 *            application if auto update check is enabled given 
 *            in the booleans string.
 * 
 * qmenu_name - The QObject name of the QMenu to integrate the update check.
 *
 * qmenubar_name - The QObject name of the QMenuBar to integrate the update check.
 *
 * qpushbutton_name - The QObject name of the QPushButton to connect the clicked signal
 *                    to the update check.
 *
 * qpushbutton_text - The text of any QPushButton with this text will have its clicked signal 
 *                    connected to the update check.
 *
 * qaction_to_override_qobject_name - The QObject name of the QAction to integrate the update check.
 *
 * qaction_to_override_text - The text of any QAction with this text will be integrated with the update check.
 *
*/
/*------------------------------------------------------------------------------------------*/
/* | */const char *interval = "75629552e6e8286442676be60e7da67d";                        /*|*/
/* | */const char *qmenu_name = "871abbc22416bb25429594dec45caf1f";                      /*|*/
/* | */const char *qmenu_text = "97393fe3f5e452adfc36db9dfaff5628";                      /*|*/
/* | */const char *qmenubar_name = "bfa40825ef36e05bbc2c561595829a92";                   /*|*/
/* | */const char *qpushbutton_name = "930b29debfb164461b39342d59e2565c";                /*|*/
/* | */const char *qpushbutton_text = "130047834d253af84d40d1c0fb52f02d";                /*|*/
/* | */const char *qaction_to_override_qobject_name = "0b6e66aa3800ad9cad94fe41984b9b56";/*|*/
/* | */const char *qaction_to_override_text = "78c9a822db28277212b8be5d73764c7f";        /*|*/
/* | */const char *booleans = "4c6160c2d6bfeba1";                                        /*|*/
/*------------------------------------------------------------------------------------------*/

#ifndef BINARIES_PACKAGED_HEADER_AVAL
static char *get_bridge_source(const char *bridge_name){
	char *r = NULL;
	char *p = calloc(1 , sizeof(*p) * (strlen(bridge_name) + 10));
	if(!p){
		return NULL;
	}
	
	sprintf(p , "lib%sBridge.so" , bridge_name);

	r = get_bundled_data_file(p);
	free(p);
	return r;
}
#endif

bridge_deployer_t *bridge_deployer_create(config_manager_t *manager,
					  deploy_info_t *info){
	bridge_deployer_t *obj = NULL;
	if(!manager || !info){
		printl(fatal , "invalid configuration manager or deploy information");
		return NULL;
	}

	if(!(obj = calloc(1 , sizeof(*obj)))){
		printl(fatal , "not enough memory");
		return NULL;
	}
	obj->manager = manager;
	obj->info = info;
	return obj;
}

void bridge_deployer_destroy(bridge_deployer_t *obj){
	if(!obj){
		return;
	}

	if(obj->bridge){
		free(obj->bridge);
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

const char *bridge_deployer_get_bridge(bridge_deployer_t *obj){
	if(!obj ||
	   !obj->bridge){
		return NULL;
	}
	return obj->bridge;
}

int bridge_deployer_run(bridge_deployer_t *obj){
	if(!obj){
		return -1;
	}
	FILE *fp = NULL;
	int iter = 0,
	    r = 0,
	    btype = 0;
	char *buffer = NULL;
	const char *p = NULL;
	const char *plugins_dir = deploy_info_plugins_directory(obj->info);
	const char *bridge_name = config_manager_get_bridge_name(obj->manager);
	char *bridge_path = NULL;
	char *bridge_src = NULL;
	if(!plugins_dir || 
	   !bridge_name){
		return -1;
	}
	
	bridge_path = calloc(1 ,sizeof(*bridge_path) * (strlen(plugins_dir) + 100));

	if(!bridge_path){
		printl(fatal , "not enough memory");
		return -1;
	}

	/* Set the bridge. */
	if(!strcmp(bridge_name , "AppImageUpdater")){
		btype = 0;	
	}else if(!strcmp(bridge_name , "QInstaller")){
		btype = 1;
	}else if(!strcmp(bridge_name , "GHReleases")){
		btype = 2;
	}else{
		/* All possible bridge name mismatches. */
		free(bridge_path);
		printl(fatal , "unknown updater bridge, please fix it");
		return -1;
	}

	obj->bridge = calloc(33 , sizeof(*(obj->bridge)));
	strcpy(obj->bridge , bridge_name);

	/* Determine the destination to save and source to copy from. */
	sprintf(bridge_path , "%s/lib%sBridge.so" , plugins_dir , bridge_name);
#ifndef BINARIES_PACKAGED_HEADER_AVAL
	bridge_src = get_bridge_source(bridge_name);

	printl(info , "copying required bridge.");
	if(copy_file(bridge_path , bridge_src) < 0){
		free(bridge_path);
		free(bridge_src);
		printl(fatal , "failed to copy required bridge");
		return -1;
	}
	free(bridge_src);
#else
	switch(btype){
		case 0:
			r = write_string_as_file(aiub_binary, 
					aiub_binary_filesize , 
					bridge_path);
			break;
		case 1:
			r = -1;
			break;
		case 2:
			r = -1;
			break;
		default:
			r = -1;
			break;
	}
	
	if(0 > r){
		free(bridge_path);
		printl(fatal , "failed to copy required bridge");
		return -1;
	}
#endif
	printl(info , "successfully copied %s bridge to deploy directory" , bridge_name);	

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
	if(!(p = config_manager_get_boolean_string(obj->manager))){
		printl(fatal , "cannot get the options from updatedeployqt.json");
		fclose(fp);
		return -1;
	}

	buffer = calloc(1 , sizeof(*buffer) * 17); /* TODO: change this once the boolean string has constant length. */	
	for(iter = 0; iter <= CONFIG_MANAGER_BOOLEAN_STRING_LEN ;++iter){
		buffer[iter] = *p++;
	}

	r = find_offset_and_write(fp , booleans , buffer , 17);
	if(r <= 0){
		printl(fatal , "failed to write boolean string to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

	/* Now write QMenuBar QObject name if available. */
	if(!(p = config_manager_get_qmenubar_qobject_name(obj->manager))){
		goto qmenu_name_write;
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	for(iter = 0; iter < CONFIG_MANAGER_OBJECT_STRING_LEN ; ++iter){
		buffer[iter] = *p++;
	}

	r = find_offset_and_write(fp , qmenubar_name , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	if(r <= 0){
		printl(fatal , "failed to write QMenuBar QObject name to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

qmenu_name_write:
	/* Now write QMenu QObject name or text if available. */
	if(!(p = config_manager_get_qmenu_qobject_name(obj->manager)) && 
	   !(p = config_manager_get_qmenu_text(obj->manager))){
		goto qpushbutton_name_write;	
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	for(iter = 0; iter < CONFIG_MANAGER_OBJECT_STRING_LEN ; ++iter){
		buffer[iter] = *p++;
	}

	if(p == config_manager_get_qmenu_qobject_name(obj->manager)){
		r = find_offset_and_write(fp , qmenu_name , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	}else{
		r = find_offset_and_write(fp , qmenu_text , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);	
	}
	if(r <= 0){
		printl(fatal , "failed to write QMenu QObject name or title to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

qpushbutton_name_write:
	/* Now write QPushButton QObject name if available. */
	if(!(p = config_manager_get_qpushbutton_qobject_name(obj->manager)) && 
	   !(p = config_manager_get_qpushbutton_text(obj->manager))){
		goto qaction_to_remove_write;	
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	for(iter = 0; iter < CONFIG_MANAGER_OBJECT_STRING_LEN ; ++iter){
		buffer[iter] = *p++;
	}

	if(p == config_manager_get_qpushbutton_qobject_name(obj->manager)){
		r = find_offset_and_write(fp , qpushbutton_name , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	}else{
		r = find_offset_and_write(fp , qpushbutton_text , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);	
	}
	if(r <= 0){
		printl(fatal , "failed to write QPushButton QObject name or text to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);

qaction_to_remove_write:
	/* Now write the text of the QAction to remove if found on runtime, if available. */
	if(!(p = config_manager_get_qaction_qobject_name(obj->manager)) && 
	   !(p = config_manager_get_qaction_text(obj->manager))){
		goto interval_write;	
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	for(iter = 0; iter < CONFIG_MANAGER_OBJECT_STRING_LEN ; ++iter){
		buffer[iter] = *p++;
	}

	if(p == config_manager_get_qaction_qobject_name(obj->manager)){
		r = find_offset_and_write(fp , qaction_to_override_qobject_name , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
	}else{
		r = find_offset_and_write(fp , qaction_to_override_text , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);	
	}
	if(r <= 0){
		printl(fatal , "failed to write QAction QObject name or text to binary");
		free(buffer);
		fclose(fp);
		return -1;
	}
	free(buffer);


interval_write:
	if((r = config_manager_get_interval(obj->manager)) <= 0){
		r = 10 * 1000; /* 10 seconds interval */
	}

	buffer = calloc(1 , sizeof(*buffer) * (CONFIG_MANAGER_OBJECT_STRING_LEN + 1));
	sprintf(buffer , "%i" , r);

	r = find_offset_and_write(fp , interval , buffer , CONFIG_MANAGER_OBJECT_STRING_LEN + 1);
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
