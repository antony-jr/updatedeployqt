#ifndef CONFIG_MANAGER_H_INCLUDED
#define CONFIG_MANAGER_H_INCLUDED
#include <json.h>

/* Boolean index for specific options. */
#define CONFIG_MANAGER_AUTO_UPDATE_CHECK 0
#define CONFIG_MANAGER_AUTO_UPDATE_CHECK_ON_STARTUP 1
#define CONFIG_MANAGER_MANUAL_UPDATE_CHECK 2
#define CONFIG_MANAGER_QMENU_QOBJECT_NAME_GIVEN 3
#define CONFIG_MANAGER_QMENUBAR_QOBJECT_NAME_GIVEN 4
#define CONFIG_MANAGER_QPUSHBUTTON_QOBJECT_NAME_GIVEN 5
#define CONFIG_MANAGER_QPUSHBUTTON_TEXT_GIVEN 6
#define CONFIG_MANAGER_QACTION_QOBJECT_NAME_GIVEN 7
#define CONFIG_MANAGER_QACTION_TEXT_GIVEN 8
#define CONFIG_MANAGER_INTERVAL_GIVEN 9
#define CONFIG_MANAGER_QMENU_TEXT_GIVEN 10

/* Some hardcoded values. */
#define CONFIG_MANAGER_BOOLEAN_STRING_LEN 10
#define CONFIG_MANAGER_OBJECT_STRING_LEN 32

typedef struct {
	char *config_file;
	char *qtversion;
	char *bridge_name;
	char *qmenu_qobject_name;
	char *qmenu_text;
	char *qmenubar_qobject_name;
	char *qpushbutton_qobject_name;
	char *qpushbutton_text;
	char *qaction_qobject_name;
	char *qaction_text;
	int   interval; /* in miliseconds */

	char booleans[11];
	/*
	 * Representation of the boolean string.
	 * ------------------------------------------------------------------------------------*
	 * booleans[0] - If set then auto update check is enabled
	 * booleans[1] - If set then auto update check should occur on startup of the 
	 *               application.
	 * booleans[2] - If set then manual update check is enabled.
	 * booleans[3] - If set then QMenu Object name is available
	 * booleans[4] - If set then QMenuBar QObject name is available.
	 * booleans[5] - If set then QPushButton QObject name is available.
	 * booleans[6] - If set then QPushButton Substring text is available.
	 * booleans[7] - If set then QAction QObject name is available.
	 * booleans[8] - If set then QAction Substring text is available.
	 * booleans[9] - If set then Interval is given.
	 * booleans[10] - If set then QMenu title is available. 
	 * -------------------------------------------------------------------------------------*
	*/

	/*
	 * Note to self:
	 * 	To set a boolean true in the boolean string, set the character at i-th
	 * 	position to 1(not ascii) and everything else is false.
	*/
} config_manager_t;

config_manager_t *config_manager_create(const char *);
void config_manager_destroy(config_manager_t*);

int config_manager_run(config_manager_t*);

/* gives a boolean string that has a series of boolean values 
 * for specific options supported by updatedeployqt.
 * This table is mentioned above. */
const char *config_manager_get_boolean_string(config_manager_t*);

const char *config_manager_get_bridge_name(config_manager_t*);
const char *config_manager_get_qmenu_qobject_name(config_manager_t*);
const char *config_manager_get_qmenu_text(config_manager_t*);
const char *config_manager_get_qmenubar_qobject_name(config_manager_t*);
const char *config_manager_get_qpushbutton_qobject_name(config_manager_t*);
const char *config_manager_get_qpushbutton_text(config_manager_t*);
const char *config_manager_get_qaction_qobject_name(config_manager_t*);
const char *config_manager_get_qaction_text(config_manager_t*);
const char *config_manager_get_qt_version(config_manager_t*);

/* returns time in miliseconds. */
int config_manager_get_interval(config_manager_t*);


#endif /*CONFIG_MANAGER_H_INCLUDED*/
