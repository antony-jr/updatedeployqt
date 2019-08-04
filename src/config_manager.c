#include <config_manager.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define true 1
#define false 0

static int isnum(char c){
	c -= '0';
	if(c >= 0 && c <= 9){
		return 1;
	}
	return 0;
}

/* function prototype of a function to parse all json values from a json object 
 * in which these json values will be passed to the other function given by the function pointer.*/
static int parse_json_object(json_value*,int (*)(const char *,json_value*,config_manager_t*) , config_manager_t *);

static int handle_qaction_to_override(const char *name, json_value *value, config_manager_t *obj){
	if(value->type != json_string){
		printl(fatal, "updatedeployqt.json:%d:%d: expected a string", value->line, value->col);
		return -1;
	}

	if(!strcmp(name , "qobject-name")){
		printl(info , "QAction QObject name(%s) is given, this QAction will be overrided" , 
		       value->u.string.ptr);
		obj->booleans[CONFIG_MANAGER_QACTION_QOBJECT_NAME_GIVEN] = true;
		obj->qaction_qobject_name = calloc(1 ,sizeof(*(obj->qaction_qobject_name)) * value->u.string.length);
		strncpy(obj->qaction_qobject_name , value->u.string.ptr , value->u.string.length);
	}else if(!strcmp(name, "text")){
		printl(info , "QAction title(%s) is given, this QAction will be overrided" , 
		       value->u.string.ptr);
		obj->booleans[CONFIG_MANAGER_QACTION_TEXT_GIVEN] = true;
		obj->qaction_text = calloc(1 ,sizeof(*(obj->qaction_text)) * value->u.string.length);
		strncpy(obj->qaction_text , value->u.string.ptr , value->u.string.length);	
	}else{
		return -1;
	}
	return 0;
}

static int handle_qpushbutton(const char *name, json_value *value, config_manager_t *obj){
	if(value->type != json_string){
		printl(fatal, "updatedeployqt.json:%d:%d: expected a string", value->line, value->col);
		return -1;
	}

	if(!strcmp(name , "qobject-name")){
		printl(info , "QPushButton QObject name(%s) is given, it will be connected to update check init" , 
		       value->u.string.ptr);
		obj->booleans[CONFIG_MANAGER_QPUSHBUTTON_QOBJECT_NAME_GIVEN] = true;
		obj->qpushbutton_qobject_name = calloc(1 ,sizeof(*(obj->qpushbutton_qobject_name)) * value->u.string.length);
		strncpy(obj->qpushbutton_qobject_name , value->u.string.ptr , value->u.string.length);
	}else if(!strcmp(name, "text")){
		printl(info , "QPushButton text(%s) is given, it will be connection to update check init" , 
		       value->u.string.ptr);
		obj->booleans[CONFIG_MANAGER_QPUSHBUTTON_TEXT_GIVEN] = true;
		obj->qpushbutton_text = calloc(1 ,sizeof(*(obj->qpushbutton_text)) * value->u.string.length);
		strncpy(obj->qpushbutton_text , value->u.string.ptr , value->u.string.length);	
	}else{
		return -1;
	}
	return 0;
}

static int handle_qmenu(const char *name, json_value *value, config_manager_t *obj){
	if(value->type != json_string){
		printl(fatal, "updatedeployqt.json:%d:%d: expected a string", value->line, value->col);
		return -1;
	}

	if(!strcmp(name , "qobject-name")){
		printl(info , "QMenu QObject name(%s) is given, 'Check for Update' option will be appended to it" , 
		       value->u.string.ptr);
		obj->booleans[CONFIG_MANAGER_QMENU_QOBJECT_NAME_GIVEN] = true;
		obj->qmenu_qobject_name = calloc(1 ,sizeof(*(obj->qmenu_qobject_name)) * value->u.string.length);
		strncpy(obj->qmenu_qobject_name , value->u.string.ptr , value->u.string.length);
	}else if(!strcmp(name, "text")){
		printl(info , "QMenu title(%s) is given, 'Check for Update' option will be appended to it" , 
		       value->u.string.ptr);
		obj->booleans[CONFIG_MANAGER_QMENU_TEXT_GIVEN] = true;
		obj->qmenu_text = calloc(1 ,sizeof(*(obj->qmenu_text)) * value->u.string.length);
		strncpy(obj->qmenu_text , value->u.string.ptr , value->u.string.length);	
	}else{
		return -1;
	}
	return 0;
}

static int handle_manual_update_check_json_object(const char *name , json_value *value , config_manager_t *obj){
	if(value->type != json_string){
		printl(fatal , "updatedeployqt.json:%d:%d: expected a string" , value->line , value->col);
		return -1;
	}

	if(!strcmp(name , "qmenu")){
		if(value->type != json_object){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a json object" , value->line , value->col);
			return -1;
		}
		return parse_json_object(value , handle_qmenu , obj);
	}
	else if(!strcmp(name , "qpushbutton")){
		if(value->type != json_object){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a json object" , value->line , value->col);
			return -1;
		}
		return parse_json_object(value , handle_qpushbutton , obj);
	}
	else if(!strcmp(name , "qaction-to-override")){
		if(value->type != json_object){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a json object" , value->line , value->col);
			return -1;
		}
		return parse_json_object(value , handle_qaction_to_override , obj);	
	}
	else if(!strcmp(name , "qmenubar-name")){
		printl(info , "QMenuBar QObject name(%s) is given, 'Check for Update' QMenu object will be created" , 
			value->u.string.ptr);
		obj->booleans[CONFIG_MANAGER_QMENUBAR_QOBJECT_NAME_GIVEN] = true;
		obj->qmenubar_qobject_name = calloc(1 ,sizeof(*(obj->qmenubar_qobject_name)) * value->u.string.length);
		strncpy(obj->qmenubar_qobject_name , value->u.string.ptr , value->u.string.length);
	}
	else{
		return -1;
	}
	return 0;
}

static int handle_auto_update_check_json_object(const char *name , json_value *value , config_manager_t *obj){
	if(!strcmp(name , "interval")){
		if(value->type != json_integer){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a integer" , 
				value->line , 
				value->col);
			return -1;
		}
		printl(info , "auto update check interval of the application: %i miliseconds" , value->u.integer);
		obj->booleans[CONFIG_MANAGER_INTERVAL_GIVEN] = true;
		obj->interval = value->u.integer;
		return 0;
	}
	
	if(value->type != json_boolean){
		printl(fatal , "updatedeployqt.json:%d:%d: expected boolean" , value->line , value->col);
		return -1;
	}

	if(!strcmp(name , "startup")){
		printl(info , "auto update check on startup of the application: %s" ,
		       (value->u.boolean) ? "true" : "false");
		obj->booleans[CONFIG_MANAGER_AUTO_UPDATE_CHECK_ON_STARTUP] = true;
	}else{
		return -1;
	}
	return 0;
}

static int handle_basic_info(const char *name , json_value *value , config_manager_t *obj){
	if(!strcmp(name , "name")){
		if(value->type != json_string){
			return 0;	
		}
		printl(info , "deploying updater for project %s" , value->u.string.ptr);
	}else if(!strcmp(name , "version")){
		if(value->type != json_string){
			return 0;	
		}
		printl(info , "deploying updater for project version %s" , value->u.string.ptr);
	}else if(!strcmp(name , "qt-version")){
		if(value->type != json_string){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a string" , value->line , value->col);
			return -1;
		}
		printl(info , "will be deploying plugins for qt version %s" , value->u.string.ptr);
		obj->qtversion = calloc(1 , sizeof(*(obj->qtversion)) * value->u.string.length);
		if(!obj->qtversion){
			printl(warning , "cannot allocate memory for storing qt version");
			obj->qtversion = NULL;
			return 0;
		}
		strncpy(obj->qtversion , value->u.string.ptr , value->u.string.length);

		if(strlen(obj->qtversion) < 3){
			free(obj->qtversion);
			obj->qtversion = NULL;
			printl(warning , "invalid qt version given, please fix that.");
			return 0;
		}

		if(strlen(obj->qtversion) < 4){
			obj->qtversion = realloc(obj->qtversion , sizeof(*(obj->qtversion)) * (value->u.string.length + 5));
			if(*(obj->qtversion + 3) == '.' || 
			   *(obj->qtversion + 3) == '\0'){
				*(obj->qtversion + 3) = '.';
				*(obj->qtversion + 4) = '0';
			}
		}else if(strlen(obj->qtversion) > 5){
			if(isnum(*(obj->qtversion + 3)) &&
			   (*(obj->qtversion + 4) == '.' || *(obj->qtversion + 4) == '\0')){
				strcpy(obj->qtversion , "5.10.0"); 
				/* Qxb plugin version 5.10.0 is used for Qt version 5.10.0 and above. */
			}else{
				free(obj->qtversion);
				obj->qtversion = NULL;
				printl(warning , "invalid qt version given, please fix that.");
				return 0;	
			}
		}
	}else if(!strcmp(name , "bridge")){
		if(value->type != json_string){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a string" , 
				value->line , value->col);
			return -1;
		}
		printl(info , "will be deploying %s bridge" , value->u.string.ptr);
		obj->bridge_name = calloc(1 , sizeof(*(obj->bridge_name)) * value->u.string.length);
		if(!obj->bridge_name){
			printl(warning , "cannot allocate memory for storing bridge name");
			obj->bridge_name = NULL;
			return 0;
		}
		strncpy(obj->bridge_name , value->u.string.ptr , value->u.string.length);
	}	
	else if(!strcmp(name , "auto-update-check")){
		if(value->type != json_object){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a json object" , value->line , value->col);
			return -1;
		}
		obj->booleans[CONFIG_MANAGER_AUTO_UPDATE_CHECK] = true;
		printl(info , "adding auto update check initialization for this application");
		return parse_json_object(value , handle_auto_update_check_json_object , obj);
	}
	else if(!strcmp(name , "manual-update-check")){
		if(value->type != json_object){
			printl(fatal , "updatedeployqt.json:%d:%d: expected a json object" , value->line , value->col);
			return -1;
		}
		obj->booleans[CONFIG_MANAGER_MANUAL_UPDATE_CHECK] = true;
		printl(info , "adding manual update check initialization for this application");
		return parse_json_object(value , handle_manual_update_check_json_object , obj);
	}
	else{
		return -1;
	}
	return 0;
}

static int parse_json_object(json_value *value , int (*handle_json_value)(const char * , json_value * , config_manager_t*) ,
		             config_manager_t *obj){
	int length = 0 , x =0;

	if(!value ||
	   value->type != json_object){
		return -1;
	}
	length = value->u.object.length;
	for(x = 0; x < length; ++x){
		if(handle_json_value(value->u.object.values[x].name , 
				  value->u.object.values[x].value ,
				  obj) < 0){
			printl(fatal , "updatedeployqt.json:%d:%d:%s: unknown error in configuration file, please fix it" , 
				(value->u.object.values[x].value)->line ,
				(value->u.object.values[x].value)->col,
				value->u.object.values[x].name);
			return -1;
		}
	}
	return 0;
}

static int parse(const char *json_contents , size_t json_contents_len , config_manager_t *obj){
	json_value *value = NULL;
	int r = 0;
	if(!json_contents || !json_contents_len){
		return (r=-1);
	}
       
	json_settings settings = { 0 };
	settings.settings |= json_enable_comments;
	char error[512];
	value = json_parse_ex(&settings , json_contents , json_contents_len , error);
	if(!value){
		printl(fatal , "updatedeployqt.json:%s" , error);
		return (r=-1);
	}
	if(value->type != json_object){
		printl(fatal , "updatedeployqt.json:0:0: expected a json object");
		return (r=-1);
	}
	r = parse_json_object(value , handle_basic_info , obj); 	
	json_value_free(value);
	return r;
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

	/* clear the boolean string. */	
	memset(obj->booleans , 0 , sizeof(obj->booleans));
	return obj;
}

void config_manager_destroy(config_manager_t *obj){
	if(!obj){
		return;
	}

	if(obj->config_file){
		free(obj->config_file);
	}
	if(obj->qtversion){
		free(obj->qtversion);
	}
	if(obj->bridge_name){
		free(obj->bridge_name);
	}
	if(obj->qmenu_qobject_name){
		free(obj->qmenu_name);
	}
	if(obj->qmenu_text){
		free(obj->qmenu_text);
	}
	if(obj->qmenubar_qobject_name){
		free(obj->qmenubar_qobject_name);
	}
	if(obj->qpushbutton_qobject_name){
		free(obj->qpushbutton_qobject_name);
	}
	if(obj->qaction_qobject_name){
		free(obj->qaction_qobject_name);
	}
	if(obj->qaction_text){
		free(obj->qaction_text);
	}
	free(obj);
}


int config_manager_run(config_manager_t *obj){
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

	if(parse(contents , pos , obj) < 0){
		printl(fatal , "cannot parse %s, giving up" , obj->config_file);
		free(contents);
		return -1;
	}
	free(contents);
	return 0;
}

const char *config_manager_get_boolean_string(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->booleans;
}

const char *config_manager_get_bridge_name(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->bridge_name;
}

const char *config_manager_get_qmenu_qobject_name(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qmenu_qobject_name;
}

const char *config_manager_get_qmenu_text(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qmenu_text;
}

const char *config_manager_get_qmenubar_qobject_name(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qmenubar_qobject_name;
}

const char *config_manager_get_qpushbutton_qobject_name(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qpushbutton_qobject_name;
}

const char *config_manager_get_qpushbutton_text(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qpushbutton_text;
}

const char *config_manager_get_qaction_qobject_name(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qaction_qobject_name;
}

const char *config_manager_get_qaction_text(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qaction_text;
}

const char *config_manager_get_qt_version(config_manager_t *obj){
	if(!obj){
		return NULL;
	}
	return obj->qtversion;
}

int config_manager_get_interval(config_manager_t *obj){
	if(!obj || obj->interval < 0){
		return 0;
	}
	return obj->interval;
}



