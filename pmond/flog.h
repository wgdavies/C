#ifndef FLOG_H
#define FLOG_H

#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

// These variable are set in the main process which uses the logger
extern int loglevel;
extern FILE* logfile;
extern int logfacility;

void flog(int level, const char *format, ...);

#endif