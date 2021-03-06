#ifndef INJECTOR_H_INCLUDED
#define INJECTOR_H_INCLUDED
#include <bridge_deployer.h>

typedef struct {
	bridge_deployer_t *bridge;
} injector_t;

injector_t *injector_create(bridge_deployer_t*);
void injector_destroy(injector_t*);
int injector_run(injector_t*);

#endif /* INJECTOR_H_INCLUDED */
