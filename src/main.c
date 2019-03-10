#include <stdio.h>
#include <qmake_process.h>

int main(int argc , char **argv){
	qmake_process_t *qp = qmake_process_create("qmake");
	qmake_query_result_t *qr = NULL;
	qr = qmake_process_query(qp , "QT_INSTALL_LIBS");
	printf("QT_INSTALL_LIBS = %s\n" , qmake_query_result_value(qr));
	qmake_process_destroy(qp);
}

