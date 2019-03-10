#include <qmake_process.h>
#include <logger.h>

int main(int argc , char **argv){
	if(argc == 1){
		printl(fatal , "no arguments given");
		return -1;
	}
	while(*argv){
		printl(info , "argument at %p = %s" , argv , *argv);
		++argv;
	}
	printl(info , "graceful exit.");
}

