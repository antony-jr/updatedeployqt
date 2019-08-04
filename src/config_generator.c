#include <config_generator.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <json_builder.h>

static void strlower(char *buffer){
	while(*buffer){
		if(isalpha(*buffer) && isupper(*buffer)){
			*buffer += 32;
		}
		++buffer;
	}
	return;
}

static int istrue(char *option){
	if(!option){
		return 0;
	}
	strlower(option);
	if(!strcmp(option , "y") ||
	   !strcmp(option , "yes")){
		return 1;
	}
	return 0;
}

static int is_string_empty(const char *s){
	if(!s || s[0] == '\0' || s[0] == '\n'){
		return 1;
	}
	return 0;
}

static char *get_string(){
	int c = 0;
	size_t pos = 0;
	char *buf = calloc(1 , sizeof(*buf) * 2);
	char *guard = NULL;

	while((c = getchar()) != EOF){
		if(c == '\n'){
			break;
		}
		*(buf + pos) = c;
		guard = realloc(buf , sizeof(*buf) * (pos + 4));
		if(!guard){
			free(buf);
			return NULL;
		}
		buf = guard;
		++pos;
	}
	*(buf + pos) = '\0';
	return buf;
}

config_generator_t *config_generator_create(){
	char *option = NULL;
	config_generator_t *obj = calloc(1 , sizeof(*obj));
	if(!obj){
		return NULL;
	}

	obj->option = calloc(1 , sizeof(*option) * 50);
	if(!obj->option){
		free(obj);
		return NULL;
	}
	/* Lets try opening the file. */
	if(!access("./updatedeployqt.json" , F_OK)){
		printf("The configuration file already exists , do you want to overwrite ?(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(!istrue(obj->option)){
			free(obj->option);
			free(obj);
			return NULL;
		}
	}

	obj->fp = fopen("./updatedeployqt.json" , "w");
	if(!obj->fp){
		free(obj->option);
		free(obj);
		return NULL;
	}
	return obj;
}

void config_generator_destroy(config_generator_t *obj){
	if(!obj){
		return;
	}

	if(obj->fp){
		fclose(obj->fp);
	}
	if(obj->option){
		free(obj->option);
	}
	if(obj->name){
		free(obj->name);
	}
	if(obj->ver){
		free(obj->ver);
	}
	if(obj->qtver){
		free(obj->qtver);
	}
	if(obj->bridge){
		free(obj->bridge);
	}
	if(obj->interval){
		free(obj->interval);
	}
	if(obj->qmenu_qobject){
		free(obj->qmenu_qobject);
	}
	if(obj->qmenu_text){
		free(obj->qmenu_text);
	}
	if(obj->qmenubar_qobject){
		free(obj->qmenubar_qobject);
	}
	if(obj->qpushbutton_qobject){
		free(obj->qpushbutton_qobject);
	}
	if(obj->qpushbutton_text){
		free(obj->qpushbutton_text);
	}	
	if(obj->qaction_qobject){
		free(obj->qaction_qobject);
	}
	if(obj->qaction_text){
		free(obj->qaction_text);
	}
}

int config_generator_run(config_generator_t *obj){
	int opt = 0;
	char *buf = NULL;
	json_serialize_opts options;
	options.mode = json_serialize_mode_multiline;

	json_value *main_object = json_object_new(0);
	json_value *auto_update_check_object  = json_object_new(0);
	json_value *manual_update_check_object = json_object_new(0);
	json_value *qmenu_json_object = NULL;
	json_value *qpushbutton_json_object = NULL;
	json_value *qaction_json_object = NULL;

	if(!obj){
		return -1;
	}

	printf("Enter the name for the application(optional): ");
	obj->name = get_string();
	if(!is_string_empty(obj->name)){
		json_object_push(main_object , "name" , json_string_new(obj->name));
	}

	printf("\nEnter the version for the application(optional): ");
	obj->ver = get_string();
	if(!is_string_empty(obj->ver)){
		json_object_push(main_object , "version" , json_string_new(obj->ver));
	}
	
	printf("\nEnter the Qt version to target(optional): ");
	obj->qtver = get_string();
	if(!is_string_empty(obj->qtver)){
		json_object_push(main_object , "qt-version" , json_string_new(obj->qtver));
	}
	
	printf("\nFrom the below options , select the bridge you want to use:\n");
	printf("1. AppImageUpdater - Used to deploy updater for Qt Application packaged with AppImage.\n");
	printf("2. QInstaller      - Used to deploy updater for Qt Installer Framework.\n");
	printf("3. Github Releases - Used to deploy updater for Qt Application released on Github.\n");
	printf("Enter you option(1/2/3): ");
	scanf("%d" , &opt);

	if(opt < 1 || opt > 3){
		printl(fatal , "invalid option given");
		return -1;
	}

	obj->bridge = calloc(1 , sizeof(*(obj->bridge)) * 20);
	switch(opt){
		case 1:
			strcpy(obj->bridge , "AppImageUpdater");
			break;
		case 2:
			strcpy(obj->bridge , "QInstaller");
			break;
		case 3:
			strcpy(obj->bridge , "GHReleases");
			break;
		default:
			break;
	}
	
	if(!is_string_empty(obj->bridge)){
		json_object_push(main_object , "bridge" , json_string_new(obj->bridge));	
	}

	printf("\nSelect the type of update you prefer:\n");
	printf("1. Auto Update Check Initialization - Automatically check for upates without concern from user.\n");
	printf("2. Manual Update Check Initialization - Use this to follow GDPR.\n");
	printf("Enter your option(1/2): ");
	scanf("%d" , &opt);

	if(opt == 1){
		printf("\nDo you want to check update on startup(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->update_check_on_startup = 1;
			obj->interval_given = 1;
			printf("Please enter an interval to start the update check(in miliseconds)(Default=Press Enter): ");
			obj->interval = get_string();
			if(is_string_empty(obj->interval)){
				free(obj->interval);
				obj->interval_given = 0;
				obj->interval = NULL;
			}
		}

		if(obj->update_check_on_startup){
			if(obj->interval_given){
				json_object_push(auto_update_check_object , "interval" , json_string_new(obj->interval));
			}
			json_object_push(auto_update_check_object, "startup" , json_boolean_new(1));
			json_object_push(main_object , "auto-update-check" , auto_update_check_object);	
		}
	}else if(opt == 2){
		printf("\nDo you want to integrate 'Check for Update' option in one of your QMenu Object(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->qmenu_qobject_given = 1;
			obj->qmenu_text_given = 1;
			printf("Please enter the QObject name of the QMenu you want to integrate: ");
			obj->qmenu_qobject = get_string();
			printf("Please enter the title of the QMenu you want to integrate: ");
			obj->qmenu_text = get_string();
			
			if(is_string_empty(obj->qmenu_qobject)){
				obj->qmenu_qobject_given = 0;
			}
			if(is_string_empty(obj->qmenu_text)){
				obj->qmenu_text_given = 0;
			}

			qmenu_json_object = json_object_new(0);
			if(obj->qmenu_qobject_given){
				json_object_push(qmenu_json_object , "qobject-name" , json_string_new(obj->qmenu_qobject));
			}
			if(obj->qmenu_text_given){
				json_object_push(qmenu_json_object , "text" , json_string_new(obj->qmenu_text));
			}
		}
		printf("Do you want to integrate 'Check for Update' option in one of your QMenuBar Object(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->qmenubar_qobject_given = 1;
			printf("Please enter the QObject name of the QMenuBar you want to integrate: ");
			obj->qmenubar_qobject = get_string();

			if(is_string_empty(obj->qmenubar_qobject)){
				obj->qmenubar_qobject_given = 0;
			}else{
				json_object_push(manual_update_check_object , "qmenubar-name" , 
						 json_string_new(obj->qmenubar_qobject));
			}
		}
		printf("Do you want to connect update check initialization with one of your QPushButton Object(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->qpushbutton_qobject_given = 1;
			obj->qpushbutton_text_given = 1;
			printf("Please enter the QObject name of the QPushButton you want to connect: ");
			obj->qpushbutton_qobject = get_string();
			printf("Please enter the text of the QPushButton you want to connect: ");
			obj->qpushbutton_text = get_string();	

			qpushbutton_json_object = json_object_new(0);

			if(is_string_empty(obj->qpushbutton_qobject)){
				obj->qpushbutton_qobject_given = 0;
			}else{
				json_object_push(qpushbutton_json_object , 
				                 "qobject-name",
					         json_string_new(obj->qpushbutton_qobject));	 
			}

			if(is_string_empty(obj->qpushbutton_text)){
				obj->qpushbutton_text_given = 0;
			}else{
				json_object_push(qpushbutton_json_object ,
						 "text",
						 json_string_new(obj->qpushbutton_text));
			}
		}
		printf("Do you want to override a QAction to update check initialization(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->qaction_qobject_given = 1;
			obj->qaction_text_given = 1;
			printf("Please enter the QObject name of the QAction you want to override: ");
			obj->qaction_qobject = get_string();
			printf("Please enter the text of the QAction you want to override: ");
			obj->qaction_text = get_string();

			qaction_json_object = json_object_new(0);

			if(is_string_empty(obj->qaction_qobject)){
				obj->qaction_qobject_given = 0;
			}else{
				json_object_push(qaction_json_object ,
					         "qobject-name" ,
						 json_string_new(obj->qaction_qobject));
			}
			if(is_string_empty(obj->qaction_text)){
				obj->qaction_text_given = 0;
			}else{
				json_object_push(qaction_json_object ,
						 "text",
						 json_string_new(obj->qaction_text));
			}
		}

	/* Add all json objects to manual update check object */
	if(obj->qmenu_qobject_given || obj->qmenu_text_given){
		json_object_push(manual_update_check_object ,
				 "qmenu",
				 qmenu_json_object);
	}

	if(obj->qpushbutton_qobject_given || obj->qpushbutton_text_given){
		json_object_push(manual_update_check_object ,
				 "qpushbutton",
				 qpushbutton_json_object);	
	}
	if(obj->qaction_qobject_given || obj->qaction_text_given){
		json_object_push(manual_update_check_object ,
				 "qaction-to-override",
				 qaction_json_object);
	}
	json_object_push(main_object, "manual-update-check" , manual_update_check_object);

	}else{
		printl(fatal , "invalid option given");
		return -1;
	}

	printl(info , "wrting configuration file..");
        buf = calloc(json_measure(main_object)/sizeof(*buf) , sizeof(*buf));
	json_serialize_ex(buf, main_object, options);
	fwrite(buf , strlen(buf) , sizeof *buf , obj->fp);
	free(buf);
	json_builder_free(main_object);
	return 0;
}
