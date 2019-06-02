#include <stdlib.h>
#include <string.h>
#include <args_parser.h>
#include <logger.h>

/* 
 * @desc: constructs arg parser.
 * @param:
 *    argc   - number of arguments given to the program.
 *    args   - the actual arguments given to the program.
 *    header - function pointer to the function which should be called when
 *             needed to print the header of the said program.
 *    help   - callback for printing help of the program.
 * @returns: pointer to args_parser_t structure if the given data is valid.
 *           else returns NULL.
*/
args_parser_t *args_parser_create(int argc ,
		                  char **args ,
		                  void (*header)() , 
				  void (*help)(const char*)){
	args_parser_t *obj = calloc(1 , sizeof(*obj));
	if(!obj){
		return NULL;
	}
	obj->args_len = argc;
	obj->args = args;
	obj->header = header;
	obj->help = help;
	return obj;
}

void args_parser_destroy(args_parser_t *obj){
	if(!obj){
		return;
	}

	if(obj->config){
		free(obj->config);
	}
	if(obj->deploy_dir){
		free(obj->deploy_dir);
	}
	free(obj);
	return;
}

int args_parser_run(args_parser_t *obj){
	int noarg = 0;
	if(!obj || obj->flags & 4){
		return ARGS_PARSER_INVALID_OBJECT;
	}

	if(obj->header){
		obj->header();
	}

	if(obj->args_len == 1 || !obj->args){
		if(obj->help){
			obj->help(*(obj->args));
		}
		return ARGS_PARSER_NO_ARGS;
	}

	obj->flags |= 4; /* to remember that we parsed */
	char **argv = obj->args;
	while(*(++argv)) {
        if(!strcmp(*argv, "-q") ||
                !strcmp(*argv, "--quiet")) {
		obj->flags |= 1; /* Set the last bit. */ 
		continue;
	} else if(!strcmp(*argv, "-v") ||
                  !strcmp(*argv, "--version")) {
		return ARGS_PARSER_CLEAN_EXIT;	
       }else if(!strcmp(*argv, "-g") ||
                  !strcmp(*argv, "--generate-config")) {
	      obj->flags |= 2; /* Set the second last bit. */
	      continue; 	
       }
       else if(strstr(*argv, "-c") ||
                  strstr(*argv, "--config")) {
	    if(obj->config){
		    printl(fatal , "two configuration file passed");
		    return ARGS_PARSER_FATAL_ERROR;
	    }
            if((*argv)[0] == '-' && (*argv)[1] == 'c'){
		if((*argv)[2] == '\0'){ /* The next arg has the string.*/
			if(*(++argv)){
				obj->config = calloc(1 , 
						    (sizeof(*(obj->config))
						    * strlen(*argv)) + 1);
				strcpy(obj->config , *argv);
				continue;
			}
			printl(fatal , "expected the configuration file");
			return ARGS_PARSER_FATAL_ERROR;
		}
      		char *config_file = *argv + 2; /* go past -c */
		obj->config = calloc(1 , (sizeof(*(obj->config)) * 
				     strlen(config_file)) + 1);
		strcpy(obj->config , config_file);
		continue;
            }
	    char *config_file = *argv + 8; /* move past --config*/
	    if(!(*config_file)){
		    /* next arg has the config file. */
	   	    if(*(++argv)){
			obj->config = calloc(1 , 
						(sizeof(*(obj->config))
						    * strlen(*argv)) + 1);
				strcpy(obj->config , *argv);
				continue;
			}
			printl(fatal , "expected the configuration file");
			return ARGS_PARSER_FATAL_ERROR;

	    }
	    obj->config = calloc(1 , (sizeof(*(obj->config)) * 
				     strlen(config_file)) + 1);
	    strcpy(obj->config , config_file);
	    continue; 
       } else {
	    /* If its not an option then lets take it as the
	     * deploy directory. 
	     * IP comes here twice , we raise a error. */
	    if(!noarg){
		    obj->deploy_dir = calloc(1 , (sizeof(*(obj->config)) 
					    * strlen(*argv)) + 1);
		    strcpy(obj->deploy_dir , *argv);
		    ++noarg;
		    continue;
	    }
	    printl(fatal , "unexpected argument %s given" , *argv);
	    return ARGS_PARSER_FATAL_ERROR;
	}
    }

    if(!noarg){
	    printl(fatal , "no deploy directory is given");
	    return ARGS_PARSER_FATAL_ERROR;
    }
    return 0;	
}

const char *args_parser_get_config_file_path(args_parser_t *obj){
	if(!obj || !obj->config){
		return NULL;
	}
	return obj->config;
}

const char *args_parser_get_deploy_dir_path(args_parser_t *obj){
	if(!obj || !obj->deploy_dir){
		return NULL;
	}
	return obj->deploy_dir;
}

short args_parser_is_generate_config(args_parser_t *obj){
	if(!obj){
		return 0;
	}
	return (obj->flags & 2) >> 1;
}

short args_parser_is_quiet(args_parser_t *obj){
	if(!obj){
		return 0;
	}
	return (obj->flags & 1);
}

