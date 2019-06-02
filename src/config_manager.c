#include <config_manager.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int parse_json_object(json_value*,int (*)(const char *,json_value*));

static int handle_manual_update_check_json_object(const char *name , json_value *value){
	if(value->type != json_string){
		return -1;
	}

	if(!strcmp(name , "qmenu-name")){
		printl(info , "QMenu QObject name: %s" , value->u.string.ptr);
	}else if(!strcmp(name , "qmenubar-name")){
		printl(info , "QMenuBar QObject name: %s" , value->u.string.ptr);
	}else if(!strcmp(name , "qpushbutton-name")){
		printl(info , "QPushButton QObject name: %s" , value->u.string.ptr);
	}else{
		return -1;
	}
	return 0;
}

static int handle_auto_update_check_json_object(const char *name , json_value *value){
	if(!strcmp(name , "interval")){
		printl(info , "auto update check interval: %s" , value->u.string.ptr);
		return 0;
	}
	
	if(value->type != json_boolean){
		return -1;
	}

	if(!strcmp(name , "startup")){
		printl(info , "auto update check on startup: %s" ,
		       (value->u.boolean) ? "true" : "false");
	}else if(!strcmp(name , "close")){
		printl(info , "auto update check on close: %s",
			(value->u.boolean) ? "true" : "false");
	}else if(!strcmp(name , "cyclic")){
		printl(info , "cyclic auto update check: %s",
			(value->u.boolean) ? "true" : "false");
	}else{
		return -1;
	}
	return 0;
}

static int handle_basic_info(const char *name , json_value *value){
	if(!strcmp(name , "name")){
		if(value->type != json_string){
				printl(warning , "cannot get the project name , ignoring");
				return 0;	
		}
		printl(info , "project name: %s" , value->u.string.ptr);
	}else if(!strcmp(name , "version")){
		if(value->type != json_string){
			printl(warning , "cannot get project version , ignoring");
			return 0;	
		}
		printl(info , "project version: %s" , value->u.string.ptr);
	}else if(!strcmp(name , "qt-version")){
		if(value->type != json_string){
			printl(fatal , "expected a string for qt-version in %d:%d" , value->line , value->col);
			return -1;
		}
		printl(info , "qt target version: %s" , value->u.string.ptr);
	}	
	else if(!strcmp(name , "auto-update-check")){
		if(value->type != json_object){
			return -1;
		}
		printl(info , "auto update check: true");
		parse_json_object(value , handle_auto_update_check_json_object);
	}
	else if(!strcmp(name , "manual-update-check")){
		if(value->type != json_object){
			return -1;
		}
		printl(info , "manual update check: true");
		parse_json_object(value , handle_manual_update_check_json_object);
	}
	else{
		return -1;
	}
	return 0;
}

static int parse_json_object(json_value *value , int (*handle_json_value)(const char * , json_value *)){
	int length = 0 , x =0;

	if(!value ||
	   value->type != json_object){
		return -1;
	}
	length = value->u.object.length;
	for(x = 0; x < length; ++x){
		if(handle_json_value(value->u.object.values[x].name , 
				  value->u.object.values[x].value) < 0){
			printl(fatal , "updatedeployqt.json:%d:%d:%s: unknown error in configuration file , please fix it" , 
				(value->u.object.values[x].value)->line ,
				(value->u.object.values[x].value)->col,
				value->u.object.values[x].name);
		}
	}
	return 0;
}

static int parse(const char *json_contents , size_t json_contents_len){
	json_value *value = NULL;
	if(!json_contents || !json_contents_len){
		return -1;
	}
       
	value = json_parse(json_contents , json_contents_len);
	if(!value ||
	    value->type != json_object){
		return -1;
	}
	parse_json_object(value , handle_basic_info); 	
	json_value_free(value);
	return 0;
}

config_manager_t *config_manager_create(const char *config){
	config_manager_t *obj = NULL;
	if(!config ||
	   !(obj = calloc(1 , sizeof(*obj)))){
		return NULL;
	}

	if(access(config , F_OK)){
		printl(fatal , "cannot find %s" , config);
		return NULL;
	}
	if(access(config , R_OK )){
		printl(fatal , "you do not have permission to read %s" , config);
		return NULL;
	}
	obj->config_file = calloc(1 , (sizeof(*(obj->config_file)) * strlen(config)) + 1);
	if(!obj->config_file){
		printl(fatal , "not enough memory");
		config_manager_destroy(obj);
		return NULL;
	}
	strcpy(obj->config_file , config);
	return obj;
}

void config_manager_destroy(config_manager_t *obj){
	if(!obj){
		return;
	}

	if(obj->config_file){
		free(obj->config_file);
	}
	if(obj->project_name){
		free(obj->project_name);
	}
	if(obj->project_version){
		free(obj->project_version);
	}
	if(obj->qobject_name){
		free(obj->qobject_name);
	}
	free(obj);
}


int config_manager_run(config_manager_t *obj){
	int r = 0;
	int c = 0;
	size_t pos = 0;
	char *contents = NULL,
	     *guard = NULL;
	FILE *fp = NULL;
	if(!obj || !obj->config_file){
		return -1;
	}


	if(!(fp = fopen(obj->config_file , "r"))){
		printl(fatal , "cannot open %s" , obj->config_file);
		return -1;
	}

	contents = calloc(1 , sizeof(*contents) * 2);
	if(!contents){
		printl(fatal , "not enough memory");
		fclose(fp);
		return -1;
	}

	while((c = getc(fp)) != EOF){
		*(contents + pos) = c;
		++pos;
		guard = realloc(contents , sizeof(*contents) * (pos + 4));
		if(!guard){
			printl(fatal , "memory reallocation failed");
			fclose(fp);
			free(contents);
			return -1;
		}
		contents = guard;
	}
	fclose(fp); 
	
	r = parse(contents , pos);	
	free(contents);
	return r;
}

