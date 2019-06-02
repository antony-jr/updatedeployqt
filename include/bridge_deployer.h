#ifndef BRIDGE_DEPLOYER_H_INCLUDED
#define BRIDGE_DEPLOYER_H_INCLUDED
#include <config_manager.h>
#include <downloader.h>
#include <deploy_info.h>

typedef struct {
	char *deploy_dir;
	downloader_t *downloader;
	config_manager_t *manager;
	deploy_info_t *info;
} bridge_deployer_t;

bridge_deployer_t *bridge_deployer_create(const char *,
		                          downloader_t*,
					  config_manager_t*,
					  deploy_info_t*);
void bridge_deployer_destroy(bridge_deployer_t*);

int bridge_deployer_run(bridge_deployer_t*);

#endif /* BRIDGE_DEPLOYER_H_INCLUDED */
