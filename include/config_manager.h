#ifndef CONFIG_MANAGER_H_INCLUDED
#define CONFIG_MANAGER_H_INCLUDED
#include <json.h>

/* Boolean index for specific options. */
#define CONFIG_MANAGER_AUTO_UPDATE_CHECK 0
#define CONFIG_MANAGER_AUTO_UPDATE_CHECK_ON_STARTUP 1
#define CONFIG_MANAGER_AUTO_UPDATE_CHECK_ON_CLOSE 2
#define CONFIG_MANAGER_AUTO_UPDATE_CHECK_CYCLIC 3
#define CONFIG_MANAGER_MANUAL_UPDATE_CHECK 4
#define CONFIG_MANAGER_QMENU_GIVEN 5
#define CONFIG_MANAGER_QMENUBAR_GIVEN 6
#define CONFIG_MANAGER_QPUSHBUTTON_GIVEN 7
#define CONFIG_MANAGER_INTERVAL_GIVEN 8

typedef struct {
	char *config_file;
	char *qtversion;
	char *bridge_name;
	char *qmenu_name;
	char *qmenubar_name;
	char *qpushbutton_name;
	int   interval; /* in miliseconds */

	char booleans[8];
        /* 
	 * booleans[0] - If set then auto update check is enabled
	 * booleans[1] - If set then auto update check should occur on startup of the 
	 *               application.
	 * booleans[2] - If set then auto update check should occur on close of the
	 *               application.
	 * booleans[3] - If set then auto update check should be cyclic.
	 * booleans[4] - If set then manual update check is enabled. (Both manual and auto can enabled).
	 * booleans[5] - If set then QMenu QObject name is available.
	 * booleans[6] - If set then QMenuBar QObject name is available.
	 * booleans[7] - If set then QPushButton QObject name is available.
	 * booleans[8] - If set then interval for startup auto update check and cyclic auto update check 
	 *               is available.
	 */
} config_manager_t;

config_manager_t *config_manager_create(const char *);
void config_manager_destroy(config_manager_t*);

int config_manager_run(config_manager_t*);

/* gives a boolean string that has a series of boolean values 
 * for specific options supported by updatedeployqt.
 * This table is mentioned above. */
const char *config_manager_get_boolean_string(config_manager_t*);

const char *config_manager_get_qmenu_name(config_manager_t*);
const char *config_manager_get_qmenubar_name(config_manager_t*);
const char *config_manager_get_qpushbutton_name(config_manager_t*);
const char *config_manager_get_qt_version(config_manager_t*);

/* returns time in miliseconds. */
int config_manager_get_interval(config_manager_t*);


#endif /*CONFIG_MANAGER_H_INCLUDED*/
