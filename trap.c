#define __trap_c
/*
	functions to abort caller
*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trap.h"
#include "str_util.h"
#include "time_.h"

static const int _byZeroDev = 1;
#ifndef PATH_MAX
#define PATH_MAX 256
#endif
static char _log[PATH_MAX] = "";

static void _no_return(const char *source, int lineNo, const char *format, va_list va);
static void _write(const char *fname, const char *frmt, va_list list);



// ------------------------------------------------------------------------
// Set the path-name of the trap log. See header
// ------------------------------------------------------------------------
void TRAP_file_set(const char *pathName)
{
	str_2_str(_log, sizeof(_log), pathName);
}



// ------------------------------------
// Abort caller. See comments at header
// ------------------------------------
void TRAP_noreturn(const char *sourceName, int lineNumber, const char *format, ...)
{
	va_list va;
	va_start(va, format);
	_no_return(sourceName, lineNumber, format, va);
}


// ---------------------------------------------------------------------------------
// Abort caller by means of the variadic macro expansion of fail_on(), fatal_error()
// ---------------------------------------------------------------------------------
void TRAP_dont_call_explicitly(const char *source, int lineNo, const char *formatExists, ...)
{
	va_list va;
	va_start(va, formatExists);
//	if (formatExists && *formatExists) {
		const char *format = va_arg(va, const char *);
		_no_return(source, lineNo, format, va);
//	} 
//	else
		_no_return(source, lineNo, (const char *)0, NULL);
}


// ----------------------------------------------------------------------------------------------
// Write a message onto the trap file. All it does is prepend the date and then write the message
// ----------------------------------------------------------------------------------------------
void TRAP_write(char *frmt, ...)
{
	va_list list;
	va_start(list, frmt);
	_write(_log, frmt, list);
}


#ifdef _WIN32
#pragma warning(disable : 4996) 
#endif

static void _no_return(const char *source, int lineNo, const char *format, va_list va)
{
	int	lenSource;
	int one;
	volatile int zero;
	char *s = 0;

	char bufNum[20];
	char buf[512 + sizeof(bufNum) + 20];


	lenSource = (int)strlen(source);
	if (lenSource > 512)
		source += lenSource - 512;

	snprintf(bufNum, sizeof(buf), "%d", lineNo);
	str_cat(buf, sizeof(buf), "Trapped at ", source, "[", bufNum, "]\n", (char *)0);

	TRAP_write(buf);
	fprintf(stderr, "%s\n", buf);

	if (format && *format) {
		_write(_log, format, va);
		vfprintf(stderr, format, va);
	}

#ifndef _DEBIG
	fprintf(stderr, "\npanic>>"); fgetc(stdin);
#endif


	if (_byZeroDev) {
		one = 1;
		zero = 0;
		one /= zero;
	}
	else {
		s = 0;
		strcpy(s, "fault");
	}
	exit(161);
}


// ----------------------------------------------------------------------------------------------
// implement TRAP_write
// ----------------------------------------------------------------------------------------------
static void _write(const char *fname, const char *frmt, va_list list)
{
	if (!fname || !fname[0])
		return;
	FILE *f = fopen(fname, "a");
	if (!f)
		return;

	time_t	t = time(NULL);
	struct tm _tm;
	time_localtime(&_tm, t);

	char buf[128];
	strftime(buf, sizeof(buf), "%Y/%m/%d %H:%M:%S", &_tm);
	fprintf(f, "%s ", buf);

	vfprintf(f, frmt, list);

	fclose(f);
}


