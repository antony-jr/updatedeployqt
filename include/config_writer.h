#ifndef CONFIG_WRITER_H_INCLUDED
#define CONFIG_WRITER_H_INCLUDED
#include <stdio.h>

typedef struct {
	FILE *fp;
	char plugin_md5_sum_placeholder[33];
	char slot_name_placeholder[33];
} config_writer_t;

config_writer_t *config_writer_create(const char*);
void config_writer_destroy(config_writer_t*);

int config_writer_set_plugin_md5sum(config_writer_t*,const char*);
int config_writer_set_slot_to_call(config_writer_t*,const char*);
#endif // CONFIG_WRITER_H_INCLUDED
