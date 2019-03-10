#include <logger.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

int printl(log_class_t class, const char *fmt , ...){
	va_list ap;
	time_t current_time;
	struct tm * time_info;
	char time_string[9];
	va_start(ap , NULL);
	time(&current_time);
	time_info = localtime(&current_time);
	strftime(time_string, sizeof(time_string), "%H:%M:%S", time_info);
	printf("[ %s ] " , time_string);
	if(class == info){
		printf("INFO   ");
	}else if(class == warning){
		printf("WARNING");
	}else{
		printf("FATAL  ");
	}
	printf(" : ");
	vprintf(fmt , ap);
	putchar('\n');
	va_end(ap);
}
