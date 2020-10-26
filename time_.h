/*
				This file contains the variants of the <time.h> declarations depending of the operating system and
				common functions to deal with date/time

*/



#ifndef time__h
#define time__h



#include <time.h>
#include <sys/timeb.h>

#ifdef  __cplusplus
extern "C" {
#endif

// Convert epoch time into an ISO date string. 
// The zero terminated string is placed in 'buf'.
// no overrun checks!! Returns length of string
// ---------------------------------------------
int time_format_iso(char buf[], time_t time);


// convert epoch time ino a short string: YYMMDDHHMMSS.
// The return value is always 12. 
// 'buf' should be 13 chars long at least.
// -----------------------------------------------------
int time_format_compact(char buf[], time_t time);


// Format for a file name: YYYYMMDDHHMMSS. Buf must be 15 bytes log at least
// --------------------------------------------------------------------------
char *time_format_for_file_name(char buf[], time_t t);


// Format according to 'format' parameter
// ---------------------------------------
char *time_format(char buf[], int zBuf, const char *format, time_t time);


struct tm *time_localtime (struct tm *, time_t t);
struct tm *time_gmtime (struct tm*, time_t t);


#ifdef  __cplusplus
}
#endif

#endif


