#ifndef LOG_H
#define LOG_H


#include <stdio.h>
#include <stdarg.h>


static FILE* new_logger ()
{
	FILE *fptr = fopen("nwm-log.txt", "a");	
	return fptr;
}


static void write_log (FILE *l, const char *message, ...)
{
	if (!l) return;
	va_list args; 

	va_start (args, message);
	vfprintf(l, message, args);
	va_end(args);


	fprintf(l, "\n");

	fflush(l);
}


static void close_logger (FILE *l) 
{
	if (l != NULL) fclose(l);
	remove("nwm-logger.txt");
}

#endif
