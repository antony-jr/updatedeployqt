#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <config_writer.h>

static int find_offset_and_write(FILE *fp , const char *to_find , const char *replace , int avoid_first){
	char buffer[33];
        int r = 0;
	memset(buffer , 0 , sizeof(buffer[0]) * 33);	
	if(!replace || !to_find || !fp){
		return -1;
	}

	rewind(fp);

	/* Check if we have an empty file. */
	if(feof(fp)){
		return -1;
	}

	while(!feof(fp)){
		if(ftell(fp)){
			fseek(fp , ftell(fp) - 32 , SEEK_SET);
		}
		memset(buffer , 0 , sizeof(buffer[0]) * 33);
		fread(buffer , sizeof(buffer[0]) , 33 , fp);	
		if(!strcmp(buffer , to_find)){
			if(avoid_first && !r){
				avoid_first = 0;
				++r;
				continue;
			}
			fseek(fp , ftell(fp) - 33 , SEEK_SET);
			fwrite(replace , sizeof *replace , 33 , fp);
			++r;
		}
	}
	return r;
}	

config_writer_t *config_writer_create(const char *filename){
	config_writer_t *obj = NULL;	
	if(!(obj = calloc(1 , sizeof *obj)) || 
	   !filename || 
	   access(filename , R_OK | W_OK) ||
	   !(obj->fp = fopen(filename , "r+"))){
		if(obj){
			free(obj);
		}
		return NULL;
	}
	
	strcpy(obj->plugin_md5_sum_placeholder , "8cfaddf5b1a24d1fd31cab97b01f1f87");
	strcpy(obj->slot_name_placeholder , "f80b03178d4080a30c14e71bbbe6e31b");
	return obj;
}

void config_writer_destroy(config_writer_t *obj){
	if(!obj){
		return;
	}
	fclose(obj->fp);
	free(obj);
}

int config_writer_set_plugin_md5sum(config_writer_t *obj , const char *md5sum){
	char md5sum_buffer[33];
	memset(md5sum_buffer , 0 , sizeof(md5sum_buffer[0]) * 33);
	strcpy(md5sum_buffer , md5sum);
	return find_offset_and_write(obj->fp , obj->plugin_md5_sum_placeholder , md5sum_buffer , 0);
}

int config_writer_set_slot_to_call(config_writer_t *obj , const char *slot_name){
	char slot_buffer[33];
	memset(slot_buffer , 0 , sizeof(slot_buffer[0]) * 33);
	strcpy(slot_buffer , slot_name);
	return find_offset_and_write(obj->fp , obj->slot_name_placeholder , slot_buffer , 1);
}
