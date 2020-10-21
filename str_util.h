#ifndef	__str_util_h
#define	__str_util_h


#ifdef __cplusplus
extern "C" {
#endif

#define remove_white_space(str){str = str_trim(str); str = str_ltrim(str);}

// ======================================
// Append string(s) to an exsiting string							|
// ======================================

// Append strings to an existing string with overrun control. 'z' is the maximal 
// allowed output string length. Returns the address of the output string
// -----------------------------------------------------------------------------
char *str_append(char *output, int z, ...);		

// Append strings to an existing string with overrun control. 'z' is the maximal 
// allowed output string length. Returns the address of the output string.
// In addition returns in 'newLen' the new length of the string after appending
// -----------------------------------------------------------------------------
char *str_append1(int *newLen, char *output, int z, ...);


// Append onto the 'output' string the 'toAppend' WITHOUT overrun control
// Returns the address of the output string after appending.
// ----------------------------------------------------------------------------------
char *str_append2(char *output, const char *toAppend);


char *str_cat(char *str, int len, ...);
char *str_cat2(char *str, int *len, ...);


 /* ======================================================================= *\
|							Concatenate argv into a single string							|
 \* ======================================================================= */
char *str_cat_argv (char *Str, int Len, char *argv[], int doSpace);

/**
 * str_unique()
 * Reduce consecutive occurrence oc a given character in a string 
 *
 * @param str -
 * @param c -
 *
 * @return -
 */
char *str_unique (char *str, char c);

/**
 * str_2_str()
 * copy a string 
 *
 * @param out -
 * @param zout -
 * @param inp -
 *
 * @return -
 */
char *str_2_str(char *out, int zout, const char *inp);


// ================================================================================================
// Calculate the net length of a string. i.e. excluding leadings and trainling whites
// ================================================================================================
int str_net_len(const char *s);



/**
 * str_trim()
 * trim trailing white spaces from a string 
 *
 * @param str -
 *
 * @return -
 */
char *str_trim(char *str);

// Trim a C comment '//' from the string
char *str_trim_c_comment (char *s);


 // trim leading white spaces from a string 
char *str_ltrim(char *str);

// duplicate a String 
 char *str_dup(const char *str);

/**
 * str_i_str()
 * Scan a string(src) for an occurrence of (sub) 
 *
 * @param src -
 * @param sub -
 *
 * @return -
 */
const char *str_i_str(const char *src, const char *sub);

/*
The str_tok() function is a reentrant version of strtok(). It gets the next token from string s1, where tokens are strings separated by characters from s2. To get the first token from s1, strtok_r() is called with s1 as its first parameter. Remaining tokens from s1 are obtained by calling strtok_r() with a null pointer for the first parameter. The string of delimiters, s2, can differ from call to call.

Note that the function strtok_r() writes null characters into the buffer pointed to by s1.

If the character buffer s1 is in dynamic memory, strtok_r() is thread-safe.
*/

char *str_tok(char *s1, const char *s2, char **s3);


/**
 * str_prefix()
 * check if two prefixes match.
 * case sensitive
 *
 * @param s -
 * @param t -
 *
 * @return -
 */
int str_prefix(const char *s, const char *t);

/**
 * str_i_prefix()
 * check if two prefixes match.
 * case insensitive
 *
 * @param s -
 * @param t -
 *
 * @return -
 */
int str_i_prefix(const char *s, const char *t);

// * convert the string  to upper case 
void str_uppercase(char* s, int l);

// like in snprintf, but also returns the formatted string as return value 
char *str_frmt(char *str, int l, char *frmt, ...);
char *str_snprintf(char *s, int *z, const char *frmt, ...);


#ifdef _WIN32
#ifdef UNICODE

/**
 * str_heb_dos_2_win()
 * convert Hebrew chars from DOS to Ansi 
 *
 * @param s -
 *
 * @return -
 */
char *str_heb_dos_2_win(unsigned char *s);

/**
 * str_2_wstr()
 * convert multibyte to wide String  
 *
 * @param wstr - translated string
 * @param zwstr - size in bytes of wstr
 * @param str - string to be converted
 *
 * @return - wstr
 */
wchar_t *str_2_wstr(wchar_t *wstr, int zwstr, const char *str);

/**
 * wstr_2_str()
 * convert wide string to multibyte	(char*) 
 *
 * @param str - string to be converted
 * @param zstr -
 * @param wstr -
 * @param codepage -
 *
 * @return -
 */
char *wstr_2_str(char *str, int zstr, const wchar_t *wstr, unsigned int codepage);
#endif  // UNICODE
#endif  // WIN32


 
// ================================================================================================
//	Convert a string into an integer value specified in octal/hexadecimal/decimal base.
//
// str_2_int() shall detect the base according to C notation of literals. So this is the
// most useful function.
//
// If you now the base already, you could call str_2_int8(), str_2_int10() or str_2_int16()
// according to the base at hand.
//
// Irrespective of the base, the number may be prefixed or suffixed by white spaces and in the
// case of a decimal notation prefixed whit a sign (+/-). 
// The function(s) return true of the number was formatted accordingly. 
// ================================================================================================
int str_2_int (int *val, const char *s);
int str_2_int8 (int *val8, const char *s);
int str_2_int10 (int *val10, const char *s);
int str_2_int16 (int *val16, const char *s);


// Convert a string into a boolean value and place it in the 'ret' return argument
// Return if string is a valid boolean string. If it is not, 'ret' is not updated.
// -------------------------------------------------------------------------------
int str_2_bool(int *ret, const char *s);


 /* ======================================================================= *\
|			Convert a string into argc-argv representation								|
 \* ======================================================================= */
int str_2_argv (char *argv[], int zArgv, char *s);



 /* ======================================================================= *\
|					Check if string is a valid C identifier								|
 \* ======================================================================= */
int str_is_id (const char *s);


 /* ===================================================================== *\
|					make a string from bytes 								|
 \* ===================================================================== */
char *str_make (char *s, int zS, const char *t, int zT);


 /* ===================================================================== *\
|				make a string from a bytes.									|
| If there are two many bytes, take the suffix and prepend ellipsis			|
 \* ===================================================================== */
char *str_make_suffix(char *s, int zS, const char *t, int zT);


 /* ===================================================================== *\
|				make a string from a bytes.									|
| If there are two many bytes, take the prefix and append ellipsis			|
 \* ===================================================================== */
char *str_make_prefix(char *s, int zS, const char *t, int zT);


// -----------------------------------------------------------------------------------------
// Find a subtring - CaSE InSenSiTiVe and rreturn ist address in 'str' or null if not found
// -----------------------------------------------------------------------------------------
const char *strstri(const char *str, const char *sub);


#ifdef __cplusplus
}
#endif


#endif // multiple inclusion

