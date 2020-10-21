/*
		common time-related calls per operating system
*/



#include <time.h>


#include "time_.h"
#include "trap.h"


struct tm *time_localtime (struct tm *_tm, time_t t)
{
	fail_on(localtime_s(_tm, &t));
#if 0
#ifdef WIN32
	fail_on(localtime_s (_tm, &t));
#elif defined(_OS_LINUX) || defined (_OS_TANDEM)
	fail_on(!localtime_r (&t, _tm));
#else
	compilation error
#endif
#endif
	return _tm;
}
struct tm *time_gmtime (struct tm *_tm, time_t t)
{
	fail_on(gmtime_s(_tm, &t));
#if 0
#ifdef WIN32
	fail_on(gmtime_s (_tm, &t));
#elif defined(_OS_LINUX) || defined (_OS_TANDEM)
	fail_on(!gmtime_r (&t, _tm));
#else
	compilation error
#endif
#endif
	return _tm;
}

#define _2digits(val)	{tmp = (val); *buf++ = (tmp / 10) + '0'; *buf++ = (tmp % 10) + '0';}


int time_format_compact(char buf[], time_t time)
{
	char *save = buf;
	int tmp;
	struct tm tm;

	time_localtime(&tm, time);

	_2digits((tm.tm_year + 1900) % 100);
	_2digits(tm.tm_mon + 1); // month
	_2digits(tm.tm_mday); // day in month
	_2digits(tm.tm_hour); // hours
	_2digits(tm.tm_min); // minutes
	_2digits(tm.tm_sec); // seconds
	*buf = '\0';
	return (int)(buf - save);
}


// -------------------------------------
// Format according to parameter 'format'
// -------------------------------------
char *time_format(char buf[], int zBuf, const char *format, time_t time)
{
	struct tm tm;
	time_localtime(&tm, time);
	strftime(buf, zBuf, format, &tm);
	return buf;
}


// ================================================================================================
// ================================================================================================
int time_format_iso(char buf[], time_t time)
{
	int i;
	int tmp;
	struct tm tm;

	time_localtime(&tm, time);

	tmp = tm.tm_year + 1900;	// Year
	buf[4] = '-';
	buf[3] = (tmp % 10) + '0'; tmp /= 10;
	buf[2] = (tmp % 10) + '0'; tmp /= 10;
	buf[1] = (tmp % 10) + '0'; tmp /= 10;
	buf[0] = tmp + '0';
	i = 5;


	#define _2DIG(buf, idx, val, suffix)	{tmp = val; buf[i++] = (tmp / 10) + '0'; buf[i++] = (tmp % 10) + '0'; buf[i++]  = suffix;}
	_2DIG(buf, i, tm.tm_mon + 1, '-') // month
	_2DIG(buf, i, tm.tm_mday,    'T') // day in month
	_2DIG(buf, i, tm.tm_hour,    ':') // hours
	_2DIG(buf, i, tm.tm_min,     ':') // minutes
	_2DIG(buf, i, tm.tm_sec,     'Z') // seconds

	buf[i] = '\0';
	return i;
}


char *time_format_for_file_name(char buf[], time_t t)
{
	int i;
	int year;
	struct tm tm;

	if (!t)
		t = time(0);

	time_localtime(&tm, t);

	#undef _2DIG
	#define _2DIG(buf, idx, val)	{buf[i++] = ((val) / 10) + '0'; buf[i++] = ((val) % 10) + '0';}

	year = tm.tm_year + 1900;	// Year

	i = 0;
	_2DIG(buf, i, year / 100);
	_2DIG(buf, i, year % 100);
	_2DIG(buf, i, tm.tm_mon + 1) // month
	_2DIG(buf, i, tm.tm_mday) // day in month
	_2DIG(buf, i, tm.tm_hour) // hours
	_2DIG(buf, i, tm.tm_min) // minutes
	_2DIG(buf, i, tm.tm_sec) // seconds

	buf[i] = '\0';
	return buf;
}




