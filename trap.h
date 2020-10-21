#ifndef __trap_h
#define __trap_h

#ifdef __cplusplus
extern "C" {
#endif

	// Trapping macros
	// ----------------

	// Abort the program. Write a message detailing the source file and line no. 
	// at the point of abortion onto the trap log. 
	// In addition, if there are variadic arguments, the first one must be a format, 
	// like in printf(), and the rest are format dependent arguments. If format is
	// present and not empty, a formatted message is written to the trap log as well.
#ifdef __GNUC__
	#define fatal_error(...) TRAP_dont_call_explicitly(__FILE__,__LINE__, #__VA_ARGS__ "" __VA_OPT__(,) __VA_ARGS__)
#endif
#ifdef _MSC_VER
	#define fatal_error(...) TRAP_dont_call_explicitly(__FILE__,__LINE__, #__VA_ARGS__ "", __VA_ARGS__)
#endif

#ifdef _DEBUG
// If x not zero, abort the program by calling fatal_error(...) with the variadic arguments
	#define fail_on(x, ...) if (x) fatal_error(__VA_ARGS__); else
	#define if_fail_on(x, t, f, ...) ((x) ? (fatal_error(__VA_ARGS__), t) : f)
#else
	#define fail_on(x, ...) 
	#define if_fail_on(x, t, f, ...) (f)
#endif


// ------------------------------------------------------------------------
// Set the path-name of the trap file onto which trap messages are written.
// By default the trap file is empty and thus, nothing is written onto it.
// The trap file is opened and closed for each write operation. To write to
// the trap file, call TRAP_write().
// ------------------------------------------------------------------------
	void TRAP_file_set(const char *pathName);


	// --------------------------------------------------------------------------------
	// This function aborts the caller either by: calling abort(), access violation or
	// a devision by zero. The actual termination method is implementation dependent. 
	// Unless your program is prepared to handle the implemented exception, this 
	// is a call of no return.
	//
	// Before aborting, the functions tries to write a record detailing the 
	// 'sourceName' and 'lineNo' onto the trap log.  
	// In addition it can write a formatted message, like in printf(). If 'format' is
	// not NULL and not empty the message is formatted and written to the trap log
	// as well. If the trap log is not specified, nothing is written. see TRAP_file_set().
	// --------------------------------------------------------------------------------
	void TRAP_noreturn(const char *sourceName, int lineNumber, const char *format, ...);


	// ----------------------------------------------------------------------------------------------
	// Write a formatted message onto the trap log. All it does is prepend the date and then 
	// write the message. If the trap log is not specified, (the default), nothing is written. 
	// To specify the trap log call TRAP_file_set(...);
	// ----------------------------------------------------------------------------------------------
	void TRAP_write(char *format, ...);


	// This functions is of no use if called explicitly . Its purpose is to facilitate the 
	// expansion of the variadic macros fail_on(x,...); fatal_error(...)
	void TRAP_dont_call_explicitly(const char *sourceName, int lineNumber, const char *formatExist, ...);

//#define TRAP_ARGS , const char *__file__, int __line__
//#define TRAP_PUSH , __file__, __line__
//#define TRAP_FNGR , __FILE__, __LINE__
//#define TRAP_ON(x) fail_on(x) 


#ifdef __cplusplus
}
#endif

#endif // __trap_h multiple inclusion






