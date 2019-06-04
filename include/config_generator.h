#ifndef CONFIG_GENERATOR_H_INCLUDED
#define CONFIG_GENERATOR_H_INCLUDED
#include <stdio.h>

typedef struct {
	FILE *fp;
	char *option;
	char *name;
	char *ver;
	char *qtver;
	char *bridge;
	char *interval;
	char *qmenu;
	char *qmenubar;
	char *qpushbutton;
	int update_check_on_startup : 1;
	int update_check_on_close : 1;
	int cyclic_update_check : 1;
	int interval_given : 1;
	int qmenu_given : 1;
	int qmenubar_given : 1;
	int qpushbutton_given : 1;
} config_generator_t;

config_generator_t *config_generator_create();
void config_generator_destroy(config_generator_t*);

int config_generator_run(config_generator_t*);

#endif /* CONFIG_GENERATOR_H_INCLUDED */
