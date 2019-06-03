#include <stdio.h>

const char *interval = "75629552e6e8286442676be60e7da67d";
const char *qmenu_name = "871abbc22416bb25429594dec45caf1f";
const char *qmenubar_name = "bfa40825ef36e05bbc2c561595829a92";
const char *qpushbutton_name = "930b29debfb164461b39342d59e2565c";
const char *boolean_string = "4c6160c2d6bfeba1";

#define BVAL(s , i) (s[i] == 1) ? "true" : "false"

int main(){
	return printf("interval: %s\n" , interval) + 
	       printf("QMenu name: %s\n" , qmenu_name) +
	       printf("QMenuBar name: %s\n" , qmenubar_name) +
	       printf("QPushButton name: %s\n" , qpushbutton_name) + 
	       printf("auto update check: %s\n" , BVAL(boolean_string , 0)) + 
	       printf("auto update check on startup: %s\n" , BVAL(boolean_string , 1)) +
	       printf("auto update check on close: %s\n" , BVAL(boolean_string , 2)) + 
	       printf("cyclic auto update check: %s\n"  , BVAL(boolean_string , 3)) + 
	       printf("manual update check: %s\n" , BVAL(boolean_string , 4)) +
	       printf("QMenu is given: %s\n" , BVAL(boolean_string , 5)) +
	       printf("QMenuBar is given: %s\n" , BVAL(boolean_string , 6)) +
	       printf("QPushButton is given: %s\n" , BVAL(boolean_string , 7)) + 
	       printf("interval is given: %s\n" , BVAL(boolean_string , 8));
}
