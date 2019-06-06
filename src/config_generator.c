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
	if(s[0] == '\0' || s[0] == '\n'){
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
	if(obj->qmenu){
		free(obj->qmenu);
	}
	if(obj->qmenubar){
		free(obj->qmenubar);
	}
	if(obj->qpushbutton){
		free(obj->qpushbutton);
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
	printf("Enter you option: ");
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
	printf("Enter your option: ");
	scanf("%d" , &opt);

	if(opt == 1){
		printf("\nDo you want to check update on startup(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->update_check_on_startup = 1;
		}

		printf("Do you want to check update on close(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->update_check_on_close = 1;
		}

		printf("Do you want to check for update in a loop(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->cyclic_update_check = 1;
			printf("Please enter the interval of a single iteration of the update check: ");
			obj->interval = get_string();
		}
	}else if(opt == 2){
		printf("\nDo you want to integrate 'Check for Update' option in one of your QMenu Object(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->qmenu_given = 1;
			printf("Please enter the QObject name of the QMenu you want to integrate: ");
			obj->qmenu = get_string();
		}
		printf("Do you want to integrate 'Check for Update' option in one of your QMenuBar Object(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->qmenubar_given = 1;
			printf("Please enter the QObject name of the QMenuBar you want to integrate: ");
			obj->qmenubar = get_string();
		}
		printf("Do you want to connect update check initialization with one of your QPushButton Object(Y/n): ");
		scanf("%s" , obj->option);
		free(get_string());
		if(istrue(obj->option)){
			obj->qpushbutton_given = 1;
			printf("Please enter the QObject name of the QPushButton you want to connect: ");
			obj->qpushbutton = get_string();
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

	if(obj->update_check_on_close){
		write_string("   ,\"close\" : true\n" , obj->fp);
	}

	if(obj->cyclic_update_check){
		write_string("   ,\"cyclic\" : true\n" , obj->fp);
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
	
	if(obj->qmenu_given){
		write_string("    \"qmenu-name\" : \"" , obj->fp);
		write_string(obj->qmenu , obj->fp);
		write_string("\"" , obj->fp);
	}
	
	if(obj->qmenubar_given){
		if(obj->qmenu_given){
			write_string(",\n" , obj->fp);
		}
		write_string("    \"qmenubar-name\" : \"" , obj->fp);
		write_string(obj->qmenubar , obj->fp);
		write_string("\"" , obj->fp);
	}
		
	if(obj->qpushbutton_given){
		if(obj->qmenu_given || obj->qmenubar_given){
			write_string(",\n" , obj->fp);
		}
		write_string("    \"qpushbutton-name\" : \"" , obj->fp);
		write_string(obj->qpushbutton , obj->fp);
		write_string("\"\n" , obj->fp);
	}
	
	if(opt == 2){
		write_string("}\n" , obj->fp);	
	}

	write_string("}" , obj->fp);

	printl(info , "wrote configuration file successfully");
	return 0;
}
