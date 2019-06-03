#include <injector.h>
#include <bridge_deployer.h>
#include <logger.h>
#include <qmake_process.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* placeholders to replace in binary. */
const char *plugin_to_load_md5sum_placeholder = "8cfaddf5b1a24d1fd31cab97b01f1f87";
const char *slot_to_call_placeholder = "f80b03178d4080a30c14e71bbbe6e31b";


/* internal configuration */
const char *slot_to_call = "initAutoUpdate(void)";

injector_t *injector_create(const char *qmake , bridge_deployer_t *bridge){
	injector_t *obj = NULL;
	if(!bridge){
		printl(fatal , "invalid bridge deployer or args parser given , internal error");
		return NULL;
	}

	if(!(obj = calloc(1 , sizeof(*obj)))){
		printl(fatal , "not enough memory");
		return NULL;
	}
	obj->qmake = qmake;
	obj->bridge = bridge;
	return obj;
}

void injector_destroy(injector_t *obj){
	if(!obj){
		return;
	}
	free(obj);
}

int injector_run(injector_t *obj){
	const char *p = NULL;
	const char *qt_version = NULL;
	qmake_process_t *qmake = NULL;
	if(!obj ||
	   !deploy_info_qxcb_plugin_path(obj->bridge->info)){
		return -1;
	}

	/* Get Qt version that is targeted. */
	if(!(p = config_manager_get_qt_version(obj->bridge->manager))){
		/* Qt version is not given in updatedeployqt.json , 
		 * lets try guessing the Qt version from the qmake installed 
		 * in the system or the one given by the user. */

		if(!(qmake = qmake_process_create(!obj->qmake ? "qmake" : obj->qmake))){
			printl(fatal , "cannot determine the qt version");
			return -1;
		}
		qt_version =  qmake_query_result_value(qmake_process_query(qmake , "QT_VERSION"));
		printl(info , "targeting qt version: %s" , qt_version);
		qmake_process_destroy(qmake);
	}
	printl(info , "targeting qt version: %s" , p);
	return 0;
}

