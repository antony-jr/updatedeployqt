#ifndef ARGS_PARSER_H_InCLUDED
#define ARGS_PARSER_H_INCLUDED

/* error codes */
#define ARGS_PARSER_INVALID_OBJECT -1
#define ARGS_PARSER_NO_ARGS -2
#define ARGS_PARSER_CLEAN_EXIT -3
#define ARGS_PARSER_FATAL_ERROR -4

typedef struct {
	int args_len;
	short flags;
	char **args;
	char *qmake;
	char *config;
	char *deploy_dir;
	void (*header)();
	void (*help)(const char*);
} args_parser_t;

args_parser_t *args_parser_create(int , char** , void (*)() , void (*)(const char*));
void args_parser_destroy(args_parser_t*);

int args_parser_run(args_parser_t*);
const char *args_parser_get_config_file_path(args_parser_t*);
const char *args_parser_get_deploy_dir_path(args_parser_t*);
const char *args_parser_get_qmake(args_parser_t*);
short args_parser_is_generate_config(args_parser_t*);
short args_parser_is_quiet(args_parser_t*);

#endif /* ARGS_PARSER_H_INCLUDED */
