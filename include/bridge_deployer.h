#ifndef BRIDGE_DEPLOYER_H_INCLUDED
#define BRIDGE_DEPLOYER_H_INCLUDED
#include <config_manager.h>
#include <downloader.h>
#include <deploy_info.h>

typedef struct {
	char *bridge_path;
	downloader_t *downloader;
	config_manager_t *manager;
	deploy_info_t *info;
} bridge_deployer_t;

bridge_deployer_t *bridge_deployer_create(config_manager_t*,
		                          downloader_t*,
					  deploy_info_t*);
void bridge_deployer_destroy(bridge_deployer_t*);

const char *bridge_deployer_get_bridge_path(bridge_deployer_t*);
int bridge_deployer_run(bridge_deployer_t*);

#endif /* BRIDGE_DEPLOYER_H_INCLUDED */
