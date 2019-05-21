#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <qmake_process.h>
#include <config_writer.h>
#include <downloader.h>
#include <logger.h>
#include <md5.h>


#define APPIMAGE_UPDATER_BRIDGE 1
#define QINSTALLER_BRIDGE 2
#define GHRELEASES_BRIDGE 3

static char *get_bridge_path(const char *, const char *);
static int deploy_appimage_updater_bridge(const char * , const char *);

int main(int argc , char **argv){
	printf("updatedeployqt version 1-alpha (commit %s) , built on %s\n" , 
#ifdef GIT_COMMIT_STR
			GIT_COMMIT_STR
#else
			"none"
#endif
			,
			__TIMESTAMP__
	      );
	printf("copyright (C) 2019 the future shell laboratory , antony jr.\n");	
	if(argc < 3){
		printf("\nUsage: %s [BRIDGE] [PATH to libqxcb.so] [OPTIONS]\n\n" , *argv);
		printf("BRIDGES: \n");
		printf("    AppImage         deploy auto updater for AppImages\n");
		printf("    QtInstaller      deploy auto updater for Qt Installer packaged Application\n");
		printf("    GithubReleases   deploy auto updater for Qt Application released via Github\n\n");
		printf("OPTIONS: \n");
		printf("    -q,--qmake       path to qmake binary to use to query qt version.\n");
		printf("   -qv,--qt-version  assume this as the qt version.\n");
		printf("    -l,--lib-path    path where libraries are deployed.\n");
		return 0;
	}

	putchar('\n');

	int ret = 0;	
	short bridge = 0;
	char *qmake_path = NULL;
	char *given_qtver = NULL;
	char *qtxcb_so_path = NULL;
	char *dep_lib_path = NULL;
	qmake_process_t *qmakep = NULL;

	/* get the bridge to use */
	if(!strcmp(*(++argv) , "AppImage")){
		bridge = APPIMAGE_UPDATER_BRIDGE;
	}else if(!strcmp(*argv , "QtInstaller")){
		bridge = QINSTALLER_BRIDGE;
		printl(info , "not implemented yet");
		goto cleanup;
	}else if(!strcmp(*argv , "GithubReleases")){
		bridge = GHRELEASES_BRIDGE;
		printl(info , "not implemented yet");
		goto cleanup;
	}else{
		printl(fatal , "unknown bridge %s , exiting" , *argv);
		goto cleanup;
	}

	if(access(*(++argv) , F_OK)){
		printl(fatal , "cannot find %s" , *argv);	
		goto cleanup;
	}
	else if(access(*argv , R_OK | W_OK)){
		printl(fatal , "you don't have read and write access for %s" , *argv);
		goto cleanup;
	}else{
		qtxcb_so_path = strdup(*argv);
	}

	while(*(++argv)){
		if(!strcmp(*argv , "-q") ||
		   !strcmp(*argv , "--qmake")){
			if(*(++argv)){
				qmake_path = strdup(*argv);
				continue;
			}
			printl(fatal , "expected qmake binary path");
			ret = -1;
			goto cleanup;
		}else
		if(!strcmp(*argv , "-qv") ||
		   !strcmp(*argv , "--qt-version")){
			if(*(++argv)){
				given_qtver = strdup(*argv);
				continue;
			}
			printl(fatal , "expected qt version");
			ret = -1;
			goto cleanup;
		}else
		if(!strcmp(*argv , "-l") ||
		   !strcmp(*argv , "--lib-path")){
			if(*(++argv)){
				dep_lib_path = strdup(*argv);
				continue;
			}
			printl(fatal , "expected library path");
			ret = -1;
			goto cleanup;
		}
		else{ 
			printl(fatal , "unknown or invalid argument '%s'" , *argv);
			ret = -1;
			goto cleanup;
		}
	}

	const char *qt_version = NULL,
                   *qt_libs = NULL;	      
	if(!(qmakep = 
	        qmake_process_create((!qmake_path) ? "qmake" : qmake_path))){
		printl(warning , "cannot create qmake process");
	}else{
	qt_version = qmake_query_result_value(
					qmake_process_query(qmakep , "QT_VERSION")
				 );
	qt_libs =    qmake_query_result_value(
					qmake_process_query(qmakep , "QT_INSTALL_LIBS")
		     );
	}


	if(!qt_version  && !given_qtver){
		printl(fatal , "no information on the qt version can be retrived , giving up");
		ret = -1;
		goto cleanup;
	}

	if(!qt_libs){
		printl(warning , "cannot determine qt library path , assume you have deployed the network module");
	}else if(!dep_lib_path){
		printl(warning , "deployed library path not given , assume you have deployed the network module");
	}

	if(given_qtver){
		qt_version = given_qtver;
	}

	printl(info , "Qt version to use %s" , qt_version);
	printl(warning , "file '%s' will be replaced" , qtxcb_so_path);

	switch(bridge){
		case APPIMAGE_UPDATER_BRIDGE:
			ret = deploy_appimage_updater_bridge(qtxcb_so_path , qt_version);
			if(0 > ret){
				printl(fatal , "deploy failed");
				goto cleanup;
			}
			break;
		default:
			break;
	}

	/* copy network module if needed and possible. */
	if(qt_libs && dep_lib_path){
		printl(info , "checking if qt network module is deployed");
		do{
			char buf[200];
			sprintf(buf , "%s/libQt5Network.so.5" , dep_lib_path);
			if(!access(buf , F_OK)){ /* exists. */
				printl(info , "qt network module is deployed already");
				goto cleanup;
			}
			printl(warning , "qt network module is not deployed");
			printl(info , "copying qt network module... ");
		}while(0);

		do{
			char host_lib[200];
			char dep_lib[200];
			FILE *dest;
			FILE *src;
			int c = 0;

			sprintf(host_lib , "%s/libQt5Network.so.5" , qt_libs);
			sprintf(dep_lib  , "%s/libQt5Network.so.5" , dep_lib_path);

			if(!(src=fopen(host_lib ,"rb"))){
				printl(fatal , "cannot open '%s' for reading" , host_lib);
				continue;
			}

			if(!(dest=fopen(dep_lib , "wb"))){
				printl(fatal , "cannot open '%s' for writing" , dep_lib);
				fclose(src);
				continue;
			}	

			while((c = getc(src)) != EOF){
				putc(c , dest);
			}
			fclose(src);
			fclose(dest);

			printl(info , "copying finished successfully");
		}while(0);
	}else{
	     printl(warning , "skipping check for qt network module");
	}

cleanup:
	printl(info , "cleaning up resources");
	qmake_process_destroy(qmakep);
	if(qmake_path)
		free(qmake_path);
	if(qtxcb_so_path)
		free(qtxcb_so_path);
	if(given_qtver)
		free(given_qtver);

	return ret;
}

static int deploy_appimage_updater_bridge(const char *qxcb , const char *qtver){
	if(!qxcb || !qtver){
		return -1;
	}

	printl(info , "deploying AppImage updater bridge");
	printl(info , "preparing for download");


	const char *bridge_path = get_bridge_path(qxcb , "libAppImageUpdaterBridge.so");	
	downloader_t *downloader = downloader_create();
	if(qtver[0] == '5' && 
	   qtver[2] == '6' &&
	   qtver[3] == '.'){ /* Qt5.6.xx */
		downloader_set_url(
				downloader,
				"https://github.com/TheFutureShell/"
				"QtPluginInjector/releases/"
				"download/continuous-Qt5.6.0/"
				"libqxcb.so");
	}else if(qtver[0] == '5' && 
	   qtver[2] == '7' &&
	   qtver[3] == '.'){ /* Qt5.7.xx */
		downloader_set_url(
				downloader,
				"https://github.com/TheFutureShell/"
				"QtPluginInjector/releases/"
				"download/continuous-Qt5.7.0/"
				"libqxcb.so");
	}else if(qtver[0] == '5' && 
	   qtver[2] == '8' &&
	   qtver[3] == '.'){ /* Qt5.8.xx */
		downloader_set_url(
				downloader,
				"https://github.com/TheFutureShell/"
				"QtPluginInjector/releases/"
				"download/continuous-Qt5.8.0/"
				"libqxcb.so");
	
	}else if(qtver[0] == '5' && 
	   qtver[2] == '9' &&
	   qtver[3] == '.'){ /* Qt5.9.xx */
		downloader_set_url(
				downloader,
				"https://github.com/TheFutureShell/"
				"QtPluginInjector/releases/"
				"download/continuous-Qt5.9.0/"
				"libqxcb.so");
	
	}else if(qtver[0] == '5' && 
	   qtver[2] == '1' &&
	   qtver[3] != '.'){ /* Qt5.1y.xx */
	   /* For Qt version 5.10.0 and higher , we use 
	    * only Qt 5.10.0 version of qxcb plugin. */
		downloader_set_url(
				downloader,
				"https://github.com/TheFutureShell/"
				"QtPluginInjector/releases/"
				"download/continuous-Qt5.10.0/"
				"libqxcb.so");
	
	}else{
		printl(fatal , "your Qt%s is not supported , please try Qt5.6 and higher" , qtver);
		downloader_destroy(downloader);
		return -1;
	}

	downloader_set_destination(downloader , qxcb); /* overwrite */
	
	/* Download the modified libqxcb.so plugin from the master repo 
	 * at github. Also show progress. */
	printl(info , "downloading modified qxcb plugin for Qt version %s..." , qtver);
	if( 0 > downloader_exec(downloader)){
		printl(fatal , "download failed for unknown reason");
		downloader_destroy(downloader);
		return -1;
	}


	/* Now downlaod the AppImageUpdagerBridge which is compiled with
	 * Qt5.6.0
	 *
	 * We need the oldest still supported version since , Old version can 
	 * work with higher versions and thus we don't need to compile for other
	 * version. Qt5.6.0 is the oldest supported version for code injection 
	 * through qxcb plugin. */

	downloader_set_url(downloader,
			"https://github.com/TheFutureShell/"
			"QtUpdateBridges/releases/download/"
			"continuous/"
			"libAppImageUpdaterBridge.so");
	downloader_set_destination(downloader , bridge_path);

	printl(info , "downloading bridge... ");

	if(0 > downloader_exec(downloader)){
		printl(fatal , "download failed for unknown reason");
		downloader_destroy(downloader);
		return -1;
	}
	downloader_destroy(downloader);	

	/* Now we need to write configuration. */	
	do{
		char md5sum[33];
		memset(md5sum , 0 , sizeof(md5sum[0]) * 33);	
		
		/* Get MD5 sum of the bridge. */
		printl(info , "computing md5 sum for '%s'" , bridge_path);
		if(0 > MD5File(bridge_path , md5sum)){
			printl(fatal , "cannot compute md5 sum for %s" , bridge_path);
			continue;
		}
		printl(info , "md5 sum for '%s' is '%s'" , bridge_path , md5sum);
			
		config_writer_t *writer = config_writer_create(qxcb);
		if( !config_writer_set_plugin_md5sum(writer,md5sum) ||
		    !config_writer_set_slot_to_call(writer,"initAutoUpdate(void)")){
			printl(warning , "cannot write configuration , maybe already written");
		}
		config_writer_destroy(writer);
	}while(0);

	printl(info , "successfully deployed bridge");
	return 0;
}

static char *get_bridge_path(const char *qxcb , const char *bridge){
	static char buf[200];
	memset(buf , 0 , sizeof(buf[0]) * 200);
	const char *p = qxcb;
	const char *ins = strstr(p , "plugins");
	strncpy(buf , p , ins - p + 7);
	strcpy(buf + strlen(buf) , "/");
	strcpy(buf + strlen(buf) , bridge);
	return buf;
}
