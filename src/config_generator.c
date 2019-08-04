#include <config_generator.h>
#include <logger.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

static int write_string(const char *s , FILE *fp){
	return fwrite(s , sizeof(*s) , strlen(s) , fp);
}

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
	if(!obj){
		return -1;
	}

	printf("Enter the name for the application(optional): ");
	obj->name = get_string();

	printf("\nEnter the version for the application(optional): ");
	obj->ver = get_string();


	printf("\nEnter the Qt version to target(optional): ");
	obj->qtver = get_string();

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

			if(is_string_empty(obj->qpushbutton_qobject)){
				obj->qpushbutton_qobject_given = 0;
			}
			if(is_string_empty(obj->qpushbutton_text)){
				obj->qpushbutton_text_given = 0;
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
			obj->qpushbutton_text = get_string();

			if(is_string_empty(obj->qaction_qobject)){
				obj->qaction_qobject_given = 0;
			}
			if(is_string_empty(obj->qaction_text)){
				obj->qaction_text_given = 0;
			}
		}
	}else{
		printl(fatal , "invalid option given");
		return -1;
	}

	printl(info , "wrting configuration file..");
	write_string("{\n" , obj->fp);
	if(!is_string_empty(obj->name)){
		write_string("   \"name\" : \"" , obj->fp);
		write_string(obj->name , obj->fp);
		write_string("\",\n" , obj->fp);
	}
	if(!is_string_empty(obj->ver)){
		write_string("    \"version\" : \"" , obj->fp);
		write_string(obj->ver , obj->fp);
		write_string("\",\n" , obj->fp);
	}
	if(!is_string_empty(obj->qtver)){
		write_string("    \"qt-version\" : \"" , obj->fp);
		write_string(obj->qtver , obj->fp);
		write_string("\",\n" , obj->fp);
	}

	if(is_string_empty(obj->bridge)){
		printl(fatal , "no valid bridge is given , internal error");
		return -1;
	}

	write_string("   \"bridge\" : \"" , obj->fp);
	write_string(obj->bridge , obj->fp);
	write_string("\"\n" , obj->fp);

	if(opt == 1){
		write_string("  ,\"auto-update-check\" : {\n" , obj->fp);
	}

	if(obj->update_check_on_startup){
		write_string("    \"startup\" : true\n" , obj->fp);
	}

	if(obj->interval_given){
		write_string("   ,\"interval\" : \"" , obj->fp);
		write_string(obj->interval , obj->fp);
		write_string("\"\n" , obj->fp);
	}

	if(opt == 1){
		write_string("}\n" , obj->fp);	
	}


	if(opt == 2){
		write_string("  ,\"manual-update-check\" : {\n" , obj->fp);
	}
	
	if(obj->qmenu_qobject_given || obj->qmenu_text_given){
		write_string("\"qmenu\" : {\n" , obj->fp);
		if(obj->qmenu_qobject_given){
		write_string("    \"qobject-name\" : \"" , obj->fp);
		write_string(obj->qmenu_qobject , obj->fp);
		if(obj->qmenu_text_given){
			write_string("\"," , obj->fp);
		}else{
			write_string("\"" , obj->fp);	
		}
		}
		if(obj->qmenu_text_given){
		write_string("    \"text\" : \"" , obj->fp);
		write_string(obj->qmenu_text , obj->fp);
		write_string("\"" , obj->fp);	
		}
		write_string("}\n" , obj->fp);		

	}
	
	if(obj->qmenubar_qobject_given){
		if(obj->qmenu_qobject_given || obj->qmenu_text_given){
			write_string(",\n" , obj->fp);
		}
		write_string("    \"qmenubar-name\" : \"" , obj->fp);
		write_string(obj->qmenubar_qobject , obj->fp);
		write_string("\"" , obj->fp);
	}
		
	if(obj->qpushbutton_qobject_given || obj->qpushbutton_text_given){
		if(obj->qmenu_qobject_given || obj->qmenu_text_given || obj->qmenubar_qobject_given){
			write_string(",\n" , obj->fp);
		}
		write_string("\"qpushbutton\" : {\n" , obj->fp);
		if(obj->qpushbutton_qobject_given){
		write_string("    \"qobject-name\" : \"" , obj->fp);
		write_string(obj->qpushbutton_qobject , obj->fp);
		if(obj->qpushbutton_text_given){
			write_string("\"," , obj->fp);
		}else{
			write_string("\"" , obj->fp);	
		}
		}
		if(obj->qpushbutton_text_given){
		write_string("    \"text\" : \"" , obj->fp);
		write_string(obj->qpushbutton_text , obj->fp);
		write_string("\"" , obj->fp);	
		}
		write_string("}\n" , obj->fp);		
	}

	if(obj->qaction_qobject_given || obj->qaction_text_given){
		if(obj->qmenu_qobject_given || obj->qmenu_text_given || obj->qmenubar_qobject_given 
		   || obj->qpushbutton_qobject_given || obj->qpushbutton_text_given){
			write_string(",\n" , obj->fp);
		}
		write_string("\"qaction-to-override\" : {\n" , obj->fp);
		if(obj->qaction_qobject_given){
		write_string("    \"qobject-name\" : \"" , obj->fp);
		write_string(obj->qaction_qobject , obj->fp);
		if(obj->qaction_text_given){
			write_string("\"," , obj->fp);
		}else{
			write_string("\"" , obj->fp);	
		}
		}
		if(obj->qaction_text_given){
		write_string("    \"text\" : \"" , obj->fp);
		write_string(obj->qaction_text , obj->fp);
		write_string("\"" , obj->fp);	
		}
		write_string("}\n" , obj->fp);		
	}
	
	if(opt == 2){
		write_string("}\n" , obj->fp);	
	}

	write_string("}" , obj->fp);

	printl(info , "wrote configuration file successfully");
	return 0;
}
