#define __str_util_c

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdarg.h>

#include "c_linkage.h"
//#include "mem_util.h"
#include "trap.h"
#include "str_util.h"

#ifdef _WIN32
#pragma warning(disable : 4996)
#include <Windows.h>
#include <stringapiset.h>
#endif


#define __FALSE 0
#define __TRUE 1

#if defined(_OS_LINUX) || defined(_OS_TANDEM)
#include <stdio.h>
#endif

#if defined(_OS_TANDEM)
#include <spthread.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#define str_len (int)strlen


 /* ===================================================================== *\
|					make a string from bytes 								|
 \* ===================================================================== */
char *str_make (char *s, int zS, const char *t, int zT)
{
	int	l;

	fail_on(zS < 0 || zT < 0);
	l = MIN(zS-1, zT);
	if (l >= 0) {
		memcpy(s, t, l);
		s[l] = 0;
	}
	return s;
}

char *str_make_prefix(char *S, int zS, const char *T, int zT)
{
	fail_on(zS < 0 || zT < 0);

	zS--;
	if (zS >= zT) {
		memcpy(S, T, zT);
		S[zT] = '\0';
		return S;
	}

	int nEllipsis = zS;
	for (int i = 0; zS > 0 && i < 3; i++, zS--)
		;
	nEllipsis = nEllipsis - zS;

	if (zS >= 0) {
		memcpy(S, T, zS);
		memcpy(S + zS, "...", nEllipsis);
		S[zS+nEllipsis] = '\0';
	}

	return S;
}


char *str_make_suffix(char *S, int zS, const char *T, int zT)
{
	fail_on(zS < 0 || zT < 0);

	zS--;
	if (zS >= zT) {
		memcpy(S, T, zT);
		S[zT] = '\0';
		return S;
	}

	int nEllipsis = zS;
	for (int i = 0; zS > 0 && i < 3; i++, zS--)
		;
	nEllipsis = nEllipsis - zS;

	if (zS >= 0) {
		memcpy(S, "...", nEllipsis);
		memcpy(S + nEllipsis, T + zT - zS, zS);
		S[zS + nEllipsis] = '\0';
	}


	return S;
}




// ------------------------------------------------------------------------
// Concatenate list of strings, return address of the zero terminating byte
// ------------------------------------------------------------------------
char *vstr_cat2(char *s, int z, va_list list)
{
	int	  k;
	int	  n;
	char *t;

	if (z <= 1)
		return (s);

	k = 0;
	z--;

	while (t = va_arg(list, char *)) {
		if (n = str_len(t)) {
			if (n > z - k)
				n = z - k;
			memcpy(s + k, t, n);
			k = k + n;
		}
	}

	s[k] = 0;
	return s + k;
}


// ---------------------------------------------------------
// Concatenate list of strings, return address of first byte
// ---------------------------------------------------------
char *vstr_cat (char *s, int z, va_list list)
{
	vstr_cat2(s, z, list);
	return s;
}




 /* ======================================================================= *\
|							Append strings to an exsiting string							|
 \* ======================================================================= */
char *str_append1(int *i, char *s, int z, ...)
{
	char *ret = s;

	s += *i;
	z -= *i;

	int	n;
	va_list list;

	n = str_len(s);
	va_start(list, z);
	vstr_cat(s + n, z - n, list);

	*i = *i + str_len(s + n);
	return ret;
}
char *str_append (char *s, int z, ...)
{
	int	n;
	va_list list;

	n = str_len (s);
	va_start (list, z);
	return vstr_cat (s+n, z-n, list);
}

char *str_append2(char *s, const char *t)
{
	int m = str_len(s);
	int n = str_len(t);
	memcpy(s + m, t, n + 1);
	return s + m + n;
}



char *str_cat (char *s, int z, ...)
{
	va_list list;

	va_start(list, z);
	vstr_cat2 (s, z, list);
	return s;
}

char *str_cat2(char *s, int *z, ...)
{
	va_list list;
	va_start(list, z);
	char *t = vstr_cat2(s, *z, list);
	*z -= (int)(t - s);
	return t;
}




 /* ======================================================================= *\
|			Concatenate an argv list of strings into a single string					|
 \* ======================================================================= */
char *str_cat_argv (char *Str, int Len, char *argv[], int doSpace)
{
	int	i;
   int	k;
   int	l;
   char	*s;

   if (Len <= 1)
      return (Str);

	i = 0;
   k = 0;
   Len--;

   while (s = argv[i]) {

		if (i && doSpace) {
			if (Len - k < 1) 
				break;
			Str[k++] = ' ';
		}

		if (l = str_len (s)) {
         if (l > Len - k)
				break;
         memcpy (Str + k, s, l);
         k = k + l;
      }
		i++;
   }

   Str[k] = 0;
   return (Str);
}


char *str_snprintf(char *s, int *z, const char *frmt, ...)
{
	va_list list;

	if (z <= 0)
		return (s);

	va_start(list, frmt);
#if defined(__linux__)
	vsnprintf(s, *z, frmt, list);
#else
	int j = _vsnprintf(s, *z, frmt, list);

	if (j < 0) {
		j = *z - 1;
		s[j] = '\0';
	}

	*z -= j + 1;
	return s + j;
#endif
}


char *str_frmt (char *Str, int Len, char *Frmt, ...)
{
   va_list Arg;

   if (Len <= 0)
      return (Str);

   va_start(Arg, Frmt);
#if defined(__linux__)
   vsnprintf(Str, Len, Frmt, Arg);
#else
   _vsnprintf(Str, Len, Frmt, Arg);
#endif

   va_end(Arg); 

   return Str;
}	

#if 0
char *str_dup (const char *Str)
{
   int	l;
   char	*s;

   if (Str) {
      l = str_len (Str) + 1;
      s = mem_get (l);
      memcpy (s, Str,	 l);
      return (s);
   }
   return ((char *)0);
}

#endif

char *str_2_str (char *Out, int zOut, const char *Inp)
{
   int    l;
//	printf ("inp=%d, out=%d, zout=%d\n", Inp, Out, zOut);

   if (Inp)
		l = str_len (Inp);
	else
		l = 0;
   if (l >= zOut)
      l = zOut - 1;
   memcpy (Out, Inp, l);
   Out[l] = 0;
   return (Out);
}



// ================================================================================================
// Calculate the net length of a string. i.e. excluding leadings and trainling whites
// ================================================================================================
int str_net_len(const char *s)
{
	if (!s)
		return 0;

	while (isspace(*s))
		s++;
	int len = str_len(s);

	while (len > 0)
		if (!isspace(s[--len])) {
			len++;
			break;
		}
	return len;
}



char	*str_trim (char *s)
{
   int	l;
   char	*p;

   if (!s)
      return (char *)s;

   l = str_len ((char *)s);
   p  = s + l - 1;

   while (l) {
      if (!isspace(*p))
         break;
      p--;
      l--;
   }

   s[l] = 0;
   return (char *)s;
}


char *str_trim_c_comment (char *s)
{
	char	*t;

	if (t = s)
		while (*t)	{
			if (*t == '/' && *(t+1) == '/') {
				*t = '\0';
				break;
			}
			t++;
		}
	return s;
}


char	*str_ltrim (char	*s)
{
   char *p;

   if (!s)
      return s;

   p = s;
   while (isspace(*p)) 
      p++;
   strcpy (s, p);
   return s;
}

#ifdef WIN32

char *str_heb_dos_2_win (unsigned char *s)
{
   unsigned char	*s0;

   s0 = s;
   while (*s) {
      if (*s >= 128 && *s <= 154)
#ifdef	__BC5
         *s = (unsigned char)(*s + (224 - 128));
#else
         *s = *s + (224 - 128);
#endif
      s++;
   }
   return ((char *)s0);
}

#endif

char *str_unique (char *Str, char c)
{
   char	*s;
   char	*t;

   s = Str;

   while (*s) {
      if (*s == c && *(s + 1) == c) {
         t = s + 2;
         while (*t == c)
            t++;
         strcpy (s + 1, t);
      }
      s++;
   }

   return (Str);
}

const char *str_i_str (const char *Src, const char *Sub)
{
   const char *s1;
   const char *s2;


   while (*Src) {
      s1 = Src;
      s2 = Sub;
      while (*s1 && *s2 && (toupper (*s1) == toupper (*s2))) {
         s1++;
         s2++;
      }
      if (!*s2)
         return Src;				// Return address of occurrence
      Src++;
   }
   return 0;
}

int str_prefix (const char *s, const char *t)
{
   int	n;
   int	m;

   n = str_len (s);
   m = str_len (t);
   if (n && m)
      return memcmp (s, t, MIN(m, n));
   return 1;
}

#ifdef _WIN32
int str_i_prefix (const char *s, const char *t)
{
   int	n;
   int	m;

   n = str_len (s);
   m = str_len (t);
   if (n && m)
      return memicmp (s, t, MIN(m, n));
   return 1;
}
#endif

#ifdef _WIN32

wchar_t	*str_2_wstr(wchar_t *wstr, int zWstr, const char *str)
{

   zWstr /= sizeof(wchar_t); 

   if (str) 
      MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, zWstr);
   	else 
	   wstr[0]=0L;

   return wstr;
}


char	*wstr_2_str(char *Str, int zStr, const wchar_t *Wstr, unsigned int CodePage)
{

   if (Wstr){
      WideCharToMultiByte (CodePage, 0, Wstr, -1, Str, zStr, NULL, NULL);
   }
   else 	Str[0]=0;
   return Str;
}

#endif


void str_uppercase(char* Str, int len)
{

   int i;

   for (i = 0; i< len; i++){
      if((Str[i]<='z')&&(Str[i]>='a'))
         Str[i]-=32;
   }
   return;
}


char *str_tok(char *s1, const char *s2, char **s3)
{
#ifdef _WIN32
	return strtok_s (s1, s2, s3);
#elif defined (__linux__)
	return strtok_r (s1, s2, s3);
#else
	#error str_tok;
#endif
}



int str_2_bool(int *val, const char *s)
{
	static const char *yes[] = { "yes", "y", "true", "t" };
	static const char *no[]  = { "no", "n", "false", "f" };

	for (int i=0; i < NELM(yes); i++)
#ifdef _WIN32
		if (!stricmp(s, yes[i])) {
#elif __linux__
		if (!strcasecmp(s, yes[i])) {
#endif
			*val = 1;
			return 1;
		}

	for (int i = 0; i < NELM(yes); i++)
#ifdef _WIN32
		if (!stricmp(s, no[i])) {
#elif __linux__
		if (!strcasecmp(s, no[i])) {
#endif
			*val = 0;
			return 1;
		}

	return 0;
}

 /* ======================================================================= *\
|	Convert a string into an integer value specified as octal/hexa/decimal notation |
 \* ======================================================================= */
int str_2_int (int *val, const char *s)
{
	while (isspace(*s)) 
		s++;

	if (*s == '0') 
		if (*(s+1) == 'x' || *(s+1) == 'X')
			return str_2_int16 (val, s);
		else
			return str_2_int8 (val, s);
	else
		if (*s == '-' || *s == '+' || isdigit(*s))
			return str_2_int10 (val, s);
	return __FALSE;
}


 /* ======================================================================= *\
|				Convert a string into an octal value						  |
 \* ======================================================================= */
int str_2_int8 (int *val8, const char *s)
{
	unsigned int	octVal;

	while (isspace (*s))
		s++;
	if (*s < '0' || *s > '7')
		return __FALSE;


	octVal = 0;
	while (*s >= '0' && *s <= '7') {
		octVal = octVal * 8 + (*s - '0');
		s++;
	}

	while (isspace (*s))
		s++;

	if (!(*s)) {
		if (*val8)
			*val8 = (int)octVal;
		return __TRUE;
	}
	return __FALSE;
}



 /* ======================================================================= *\
|							Convert a string into an hex value				  |
 \* ======================================================================= */
int str_2_int16 (int *val16, const char *s)
{
	char	a, f;
	unsigned int	hexVal;

	while (isspace (*s))
		s++;

	if (*s !='0')
		return __FALSE;
	s++;

	if (*s !='x' && *s != 'X')
		return __FALSE;
	if (*s == 'x') {
		a = 'a';
		f = 'f';
	} else {
		a = 'A';
		f = 'F';
	}
	s++;

	if ((*s < '0' || *s > '9') && (*s < a || *s > f))
		return __FALSE;

	hexVal = 0;
	while ((*s >= '0' && *s <= '9') || (*s >= a && *s <= f)) {
		if (*s >= '0' && *s <= '9')
			hexVal = hexVal * 16 + (*s - '0');
		else
			hexVal = hexVal * 16 + (*s - a + 10);
		s++;
	}

	while (isspace (*s))
		s++;

	if (!(*s)) {
		if (*val16)
			*val16 = (int)hexVal;
		return __TRUE;
	}
	return __FALSE;
}


 /* ======================================================================= *\
|						Convert a string into a decimal integer				  |
 \* ======================================================================= */
int str_2_int10 (int *val10, const char *s)
{
	int	val;
	int	sign;

	while (isspace (*s))
		s++;

	if (*s == '-') {
		sign = -1;
		s++;
	} else {
		sign = 1;
		if (*s == '+')
			s++;
	}

	if (*s < '0' || *s > '9')
		return __FALSE;

	val = 0;
	while (*s >= '0' && *s <= '9') {
		val = val * 10 + (*s - '0');
		s++;
	}

	while (isspace (*s))
		s++;

	if (!(*s)) {
		if (val10)
			*val10 = val * sign;
		return __TRUE;
	}
	return __FALSE;
}


 /* ======================================================================= *\
|			Convert a string into argc-argv representation								|
 \* ======================================================================= */
int str_2_argv (char *argv[], int zArgv, char *s)
{
	int	n;

	if (zArgv <= 0)
		return 0;
	argv[n=0] = (char *)0;

	while (*s) {
		while (isspace(*s))
			s++;
		if (*s) {
			if (n >= zArgv - 1) {
				argv[0] = (char *)0;
				return 0;
			}
			argv[n++] = s;
			argv[n] = (char *)0;
			while (*s && !isspace(*s))
				s++;
			if (isspace(*s))
				*s++ = '\0';
		}
	}
	return n;
}


 /* ======================================================================= *\
|					Check if string is a valid C identifier					  |
 \* ======================================================================= */
int str_is_id (const char *s)
{
	if (!s)
		return __FALSE;
	while (*s == '_')
		s++;
	if (!isalpha (*s))
		return __FALSE;
	while (isalnum (*s) || *s == '_')
		s++;
	if (*s == ':' && *(s+1) == ':')
		return str_is_id (s + 2);
	return !(*s);
}


const char *strstri(const char *str, const char *sub)
{
	while (*str) {
		const char *t = sub;
		const char *s = str;
		while (*s && *t && toupper(*s) == toupper(*t)) {
			s++; 
			t++;
		}
		if (!(*t))
			return str;
		str++;
	}

	return NULL;
}
