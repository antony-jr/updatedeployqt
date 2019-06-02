#ifndef CONFIG_MANAGER_H_INCLUDED
#define CONFIG_MANAGER_H_INCLUDED
#include <json.h>

typedef struct {
	char *config_file;
	char *project_name;
	char *project_version;
	char *qobject_name;
	int update_interval;

	/* We know that C89 states to have int data type atleast 16 bits ,
	 * So it is safe to assume that we can use 16 bit fields. */
	int flags;
} config_manager_t;

config_manager_t *config_manager_create(const char *);
void config_manager_destroy(config_manager_t*);

int config_manager_run(config_manager_t*);
const char *config_manager_get_project_name(config_manager_t*);
const char *config_manager_get_project_version(config_manager_t*);
const char *config_manager_get_qmenu_name(config_manager_t*);
int config_manager_get_update_cycle_interval(config_manager_t*);

#endif /*CONFIG_MANAGER_H_INCLUDED*/
