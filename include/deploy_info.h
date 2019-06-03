#ifndef DEPLOY_INFO_H_INCLUDED
#define DEPLOY_INFO_H_INCLUDED

typedef struct {
	char *deploy_dir;
	char *system_lib_dir;
	char *deploy_lib_dir;
	char *deploy_plugins_dir;
	char *deployed_libqxcb_path;
} deploy_info_t;

deploy_info_t *deploy_info_create(const char *);
void deploy_info_destroy(deploy_info_t*);

int deploy_info_gather(deploy_info_t*);

const char *deploy_info_directory(deploy_info_t*);
const char *deploy_info_system_library_directory(deploy_info_t*);
const char *deploy_info_library_directory(deploy_info_t*);
const char *deploy_info_plugins_directory(deploy_info_t*);
const char *deploy_info_qxcb_plugin_path(deploy_info_t*);

#endif /* DEPLOY_INFO_H_INCLUDED */
