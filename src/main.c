#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <qmake_process.h>
#include <appdir_context.h>
#include <logger.h>

int main(int argc , char **argv){
	printf("updatedeployqt version 1-alpha (commit %s) , build %d built on %s\n" , 
#ifdef GIT_COMMIT_STR
			GIT_COMMIT_STR
#else
			"none"
#endif
			,
#ifdef GIT_BUILD_NO
			GIT_BUILD_NO
#else 
			0
#endif
			,
#ifdef BUILD_TIME_STR
			BUILD_TIME_STR
#else
			"Unknown time"
#endif
	      );
	printf("Copyright (C) 2019 The Future Shell Laboratory , Antony Jr.\n");	
	if(argc == 1){
		printf("\nUsage: %s [OPTIONS]\n\n" , *argv);
		printf("OPTIONS: \n");
		printf("    -a,--appdir     set path to appdir.\n");
		printf("    -v,--version    print version and exit.\n");
		printf("    -q,--qmake      path to qmake binary to use.\n");
		return 0;
	}

	putchar('\n');

	int ret = 0;	
	char *appdir = NULL,
	     *qmake_path = NULL;
	qmake_process_t *qmakep = NULL;
	appdir_context_t *appdir_ctx = NULL;

	while(*(++argv)){
		if(!strcmp(*argv , "-v") ||
		   !strcmp(*argv , "--version")){
			ret = -1;
			goto cleanup;
		}else 
		if(!strcmp(*argv , "-a") ||
		   !strcmp(*argv , "--appdir")){
			if(*(++argv)){
				appdir = strdup(*argv);
				continue;
			}
			printl(fatal , "expected appdir path");
			ret = -2;
			goto cleanup;
		}else
		if(!strcmp(*argv , "-q") ||
		   !strcmp(*argv , "--qmake")){
			if(*(++argv)){
				qmake_path = strdup(*argv);
				continue;
			}
			printl(fatal , "expected qmake binary path");
			ret = -3;
			goto cleanup;
		}else{ 
			printl(fatal , "unknown or invalid argument '%s'" , *argv);
			ret = -4;
			goto cleanup;
		}
	}

	if(!appdir){
		printl(fatal , "no appdir path is given");
		ret = -5;
		goto cleanup;
	}
	
	if(!(appdir_ctx = appdir_context_create(appdir))){
		printl(fatal , "cannot create a appdir context");
		ret = -6;
		goto cleanup;
	}

	if(!(qmakep = qmake_process_create((!qmake_path) ? "qmake" : qmake_path))){
		printl(fatal , "cannot create qmake process");
		ret = -7;
		goto cleanup;
	}

	const char *qt_version = qmake_query_result_value(
					qmake_process_query(qmakep , "QT_VERSION")
				 );
	const char *qt_libs    = qmake_query_result_value(
					qmake_process_query(qmakep , "QT_INSTALL_LIBS")
				 );
	const char *qt_plugins = qmake_query_result_value(
				        qmake_process_query(qmakep , 
						            "QT_INSTALL_PLUGINS")
				 );
	printl(info , "Qt version is %s" , qt_version);
	printl(info , "Qt library install path %s" , qt_libs);
	printl(info , "Qt plugins path %s" , qt_plugins);

cleanup:
	if(qmakep)
		qmake_process_destroy(qmakep);
	if(appdir){
		free(appdir);
		appdir_context_destroy(appdir_ctx);
	}
	if(qmake_path)
		free(qmake_path);
	return ret;
}

