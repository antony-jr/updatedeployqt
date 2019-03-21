#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <qmake_process.h>
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
	char *appdir = NULL,
	     *qmake_path = NULL;
	qmake_process_t *qmakep = NULL;

	while(*(++argv)){
		if(!strcmp(*argv , "-v") ||
		   !strcmp(*argv , "--version")){
			goto cleanup;
		}else 
		if(!strcmp(*argv , "-a") ||
		   !strcmp(*argv , "--appdir")){
			if(*(++argv)){
				appdir = strdup(*argv);
				continue;
			}
			printl(fatal , "expected appdir path");
			goto cleanup;
		}else
		if(!strcmp(*argv , "-q") ||
		   !strcmp(*argv , "--qmake")){
			if(*(++argv)){
				qmake_path = strdup(*argv);
				continue;
			}
			printl(fatal , "expected qmake binary path");
			goto cleanup;
		}else{ 
			printl(fatal , "unknown or invalid argument '%s'" , *argv);
			goto cleanup;
		}
	}

	if(!appdir){
		printl(fatal , "no appdir path is given");
		goto cleanup;
	}

	if(!(qmakep = qmake_process_create((!qmake_path) ? "qmake" : qmake_path))){
		printl(fatal , "cannot create qmake process");
		goto cleanup;
	}

	const char *qt_version = qmake_query_result_value(
					qmake_process_query(qmakep , "QT_VERSION")
				 );

	printl(info , "Qt version is %s" , qt_version);

cleanup:
	if(qmakep)
		qmake_process_destroy(qmakep);
	if(appdir)
		free(appdir);
	if(qmake_path)
		free(qmake_path);
	return 0;
}

