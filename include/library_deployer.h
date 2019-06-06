#ifndef LIBRARY_DEPLOYER_H_INCLUDED
#define LIBRARY_DEPLOYER_H_INCLUDED
#include <deploy_info.h>

typedef struct {
	deploy_info_t *info;
} library_deployer_t;

library_deployer_t *library_deployer_create(deploy_info_t*);
void library_deployer_destroy(library_deployer_t*);

int library_deployer_run(library_deployer_t*);

#endif /* LIBRARY_DEPLOYER_H_INCLUDED */
