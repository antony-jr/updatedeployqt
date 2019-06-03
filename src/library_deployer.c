#include <library_deployer.h>
#include <deploy_info.h>
#include <qmake_process.h>
#include <logger.h>
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

library_deployer_t *library_deployer_create(const char *qmake ,
		                            deploy_info_t *info){
	library_deployer_t *obj = NULL;
	if(!info){
		printl(fatal , "invalid deploy information given");
		return NULL;
	}

	if(!(obj = calloc(1 , sizeof(*obj)))){
		printl(fatal , "not enough memory");
		return NULL;
	}
	
	obj->qmake = qmake;
	obj->info = info;
	return obj;
}

void library_deployer_destroy(library_deployer_t *obj){
	if(!obj){
		return;
	}
	free(obj);
}

int library_deployer_run(library_deployer_t *obj){
	int qt_network_dep = 0;
	char *p = NULL;
	char *p2 = NULL;
	const char *qt_lib_install_path = NULL;
	struct dirent *de;
	DIR *dr = NULL;
	qmake_process_t *qmake = NULL;
	
	if(!obj ||
	   !deploy_info_library_directory(obj->info) ||
	   !deploy_info_system_library_directory(obj->info)){
		return -1;
	}


	p = calloc(1 , sizeof(*p) * (strlen(deploy_info_library_directory(obj->info)) + 100));
	sprintf(p , "%s/libQt5Network.so.5" , deploy_info_library_directory(obj->info));

	if(access(p , F_OK)){
		printl(warning , "qt network module does not exist in your deploy directory");
		printl(info , "trying to deploy it");
		if(!(qmake = qmake_process_create(!obj->qmake ? "qmake" : obj->qmake))){
			printl(fatal , "cannot determine the qt install path , giving up");
			free(p);
			goto deploy_openssl_libs;
		}
		qt_lib_install_path =  
			qmake_query_result_value(qmake_process_query(qmake , "QT_INSTALL_LIBS"));
		printl(info , "qt libraries install path: %s" , qt_lib_install_path);
		p2 = calloc(1 , sizeof(*p2) * (strlen(qt_lib_install_path) + 100));
		sprintf(p2 , "%s/libQt5Network.so.5" , qt_lib_install_path);

		if(copy_file(p , p2) < 0){
			printl(warning , "cannot copy qt network module , giving up");
			free(p);
			free(p2);
			goto deploy_openssl_libs;
		}
		qt_network_dep = 1;

		qmake_process_destroy(qmake);
		free(p);
		free(p2);
	}else{
		printl(info , "qt network module already deployed");
		qt_network_dep = 1;
		free(p);
	}

deploy_openssl_libs:
	if(!qt_network_dep){
		printl(info , "will search for libQt5Network.so.5 in system library directory");
	}
	printl(info , "searching for openssl libraries in host system library path");
	
	dr = opendir(deploy_info_system_library_directory(obj->info));
	if (!dr){
		printl(fatal , "cannot open directory , giving up");
		    return -1;
	} 
	while ((de = readdir(dr)) != NULL){
		if((!strstr(de->d_name , "libcrypt") &&
		   !strstr(de->d_name , "libssl") ) ||  
		   (strcmp(de->d_name , "libQt5Network.so.5") || qt_network_dep)){
			continue;
		}
		
		char *source = calloc(1 , sizeof(*source) * (strlen(deploy_info_system_library_directory(obj->info)) + strlen(de->d_name) + 2));
		sprintf(source , "%s/%s" , deploy_info_system_library_directory(obj->info) , de->d_name);

		char *destination = calloc(1 , sizeof(*destination) * (strlen(deploy_info_library_directory(obj->info))+strlen(de->d_name) + 3));
		sprintf(destination , "%s/%s" , deploy_info_library_directory(obj->info) , de->d_name);
			
		if(copy_file(destination ,source) < 0){
			printl(warning , "copy failed , ignoring");
		}
		free(source);
		free(destination);
	}
    	closedir(dr);
    	printl(info , "all required libraries are deployed");
   	return 0;
}
