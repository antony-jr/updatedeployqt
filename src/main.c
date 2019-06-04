#include <stdio.h>
#include <string.h>
#include <config_manager.h>
#include <downloader.h>
#include <args_parser.h>
#include <deploy_info.h>
#include <bridge_deployer.h>
#include <injector.h>
#include <library_deployer.h>
#include <config_generator.h>
#include <logger.h>

static void print_header();
static void print_help();
static void print_conclusion(int);

static char *get_config_file(const char *);

int main(int argc, char **argv) {
	int r = 0;
	int arg_parse_ret = 0;
	char *rbuf = NULL;
	const char *config = NULL;
	const char *deploy_dir = NULL;
	const char *qmake_path = NULL;
	deploy_info_t *dinfo = NULL;
	downloader_t *downloader = NULL;
	config_manager_t *cmanager = NULL;
	config_generator_t *generator = NULL;
	bridge_deployer_t *bdeployer = NULL;
	library_deployer_t *ldeployer = NULL;
	injector_t *injector = NULL;
	args_parser_t *ap = args_parser_create(argc,argv,
			                       print_header , print_help);

	/* parse args from command line arguments of the program. */	
	arg_parse_ret = args_parser_run(ap);
	if(arg_parse_ret == ARGS_PARSER_INVALID_OBJECT ||
	   arg_parse_ret == ARGS_PARSER_FATAL_ERROR){
		/* if invalid arguments given , go to cleanup with errors. */
		r = -1;
		goto cleanup;
	}

	if(arg_parse_ret == ARGS_PARSER_NO_ARGS ||
	   arg_parse_ret == ARGS_PARSER_CLEAN_EXIT){
		/* cleanup and exit with no errors if no args is given or -v
		 * is given or -g is given.*/

		if(args_parser_is_generate_config(ap)){
			/* If -g is given then generate and exit peacefully */
			putchar('\n'); /* newline after the header */
			if(!(generator = config_generator_create())){
				r = -1;
				goto cleanup;
			}
			if(config_generator_run(generator) < 0){
				printl(fatal , "there was an error generating configuration file");
				r = -1;
				goto cleanup;
			}
		}

		r = 0;
		goto cleanup;
	}

	if(args_parser_is_quiet(ap)){
		/* activate quiet mode */
		freopen(NULL , "r" , stdout);
	}

	/* get config file path and deploy directory path */
	config = args_parser_get_config_file_path(ap);
	deploy_dir = args_parser_get_deploy_dir_path(ap);
	qmake_path = args_parser_get_qmake(ap);

	/* construct configuration manager from the config file.
	 * If config file path is not given then it is assumed to be './updatedeployqt.json'.
	 * else the given path is appended with '/updatedeployqt.json'.
	*/
	cmanager = config_manager_create(!config ? "./updatedeployqt.json" : 
			                 (rbuf = get_config_file(config)));
	/* no memory to allocate or some other error. */
	if(!cmanager){
	r = -1;
		goto cleanup;
	}

	/* run the configuration manager to parse the json file and store 
	 * required information. */
	if(config_manager_run(cmanager) < 0){
		/* if parsing fails then cleanup and exit with errors */
		r = -1;
		goto cleanup;
	}

	/* allocate downloader. */
	if(!(downloader = downloader_create())){
		/* cleanup and exit with errors */
		printl(fatal , "cannot allocate space for downloader");
		r = -1;
		goto cleanup;
	}

	/* Now lets get the deploy information before we download anything. */
	if(!(dinfo = deploy_info_create(deploy_dir))){
		r = -1;
		goto cleanup;
	}

	if(deploy_info_gather(dinfo) < 0){
		printl(fatal , "cannot get all deploy information");
		r = - 1;
		goto cleanup;
	}

	printl(info , "system library path: %s" , deploy_info_system_library_directory(dinfo));
	printl(info , "deploy library path: %s" , deploy_info_library_directory(dinfo));
	printl(info , "deploy plugins path: %s" , deploy_info_plugins_directory(dinfo));
	printl(info , "QXcb plugin path: %s" , deploy_info_qxcb_plugin_path(dinfo));


	/* Now lets download , and write configuration on the required bridge 
	 * in the plugins directory. */
	if(!(bdeployer = bridge_deployer_create(cmanager , downloader , dinfo))){
		r = -1;
		goto cleanup;
	}

	if(bridge_deployer_run(bdeployer) < 0){
		r = -1;
		goto cleanup;
	}
	
	printl(info , "deployed bridge successfully!");

	/* Now lets download , and write configuration on the qxcb plugin 
	 * in the plugins directory. */
	if(!(injector = injector_create(qmake_path , bdeployer))){
		r = -1;
		goto cleanup;
	}

	if(injector_run(injector) < 0){
		printl(fatal , "qt plugin injection failed");
		r = -1;
		goto cleanup;
	}

	printl(info , "qt plugin injections was successful");


	/* finally deploy required shared libraries */
	printl(info , "deploying required shared libraries..");
	if(!(ldeployer = library_deployer_create(qmake_path , dinfo))){
		r = -1;
		goto cleanup;
	}

	if(library_deployer_run(ldeployer) < 0){
		printl(fatal , "cannot deploy required libraries");
		r = -1;
		goto cleanup;
	}

	r = 1; /* Mark that this deploy is successful. */

cleanup:
	print_conclusion(r);
	deploy_info_destroy(dinfo);
	args_parser_destroy(ap);
	config_manager_destroy(cmanager);
	config_generator_destroy(generator);
	downloader_destroy(downloader);
	bridge_deployer_destroy(bdeployer);
	library_deployer_destroy(ldeployer);
	injector_destroy(injector);
	if(rbuf){
		free(rbuf);
	}
	return (r==1) ? 0 : r; /* if success then return 0 , else return the error code. */
}

static void print_conclusion(int result){
	switch(result){
		case 0: /* no-op */
			break;
		case 1:
			printl(info , "successfully deployed updater");
			break;
		default:
			printl(fatal , "deploy failed , exiting with errors");
			break;
	}
	return;
}

/*
 * @desc    : this function simply constructs the path to configuration file with the
 *            given path.
 * @param   : const char * which is assumed to be the path of the configuration file.
 * @returns : char * which is the path to configuration file.
 *
 * @note    : you have to free the output of this function.
*/
static char *get_config_file(const char *path){
	char *buf = NULL;
	if(!path ||
	   !(buf = calloc(1 , (sizeof(*buf) * strlen(path)) + 30 ))){
		return NULL;
       	}
	sprintf(buf , "%s/updatedeployqt.json" , path);
	return buf;
}

/*
 * @desc   : this prints the version and other things about the program.
 * @param  : nothing.
 * @returns: nothing.
*/
static void print_header(){
	printf("updatedeployqt git-commit %s , built on %s\n",
#ifdef GIT_COMMIT_STR
           GIT_COMMIT_STR
#else
           "none"
#endif
           ,
           __TIMESTAMP__
          );
    printf("Copyright (C) 2019 The Future Shell Laboratory.\n");
}

/* 
 * @desc   : this prints the usage to standard output.
 * @param  : program name as char*
 * @returns: nothing.
*/
static void print_help(char *program_name){
    printf("\nUsage: %s [OPTIONS] [PATH TO DEPLOY DIR]\n\n", program_name);
    printf("OPTIONS: \n");
    printf("    -c,--config           configuration file to use for the deploy.(default=.)\n");
    printf("    -g,--generate-config  create configuration file interactively.\n");
    printf("    -q,--quiet            do not print anything to stdout.\n");
    printf("    -v,--version          show version and exit.\n");
    printf("    -p,--qmake            use this qmake to query about qt installation.\n");
}
