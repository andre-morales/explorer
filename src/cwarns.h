#pragma once
#if defined(_MSC_VER)
	#define IGNORE_WARN_PUSH    __pragma(warning( push ))
	#define IGNORE_WARN_POP     __pragma(warning( pop )) 
	#define IGNORE_WARN(number) __pragma(warning( disable : number ))

	#define IGNORE_WARN_UNSCOPED_ENUM      IGNORE_WARN(26812)
	#define IGNORE_WARN_DANGLING_TEMP_PNTR IGNORE_WARN(26815)

#elif defined(__GNUC__)
	#define DO_PRAGMA(X) _Pragma(#X)
	#define IGNORE_WARN_PUSH           DO_PRAGMA(GCC diagnostic push)
	#define IGNORE_WARN_POP            DO_PRAGMA(GCC diagnostic pop) 
	#define IGNORE_WARN(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

	#define IGNORE_WARN_UNSCOPED_ENUM
	#define IGNORE_WARN_DANGLING_TEMP_PNTR
#else
	#define IGNORE_WARN_PUSH
	#define IGNORE_WARN_POP
	
	#define IGNORE_WARN_UNSCOPED_ENUM
	#define IGNORE_WARN_DANGLING_TEMP_PNTR
#endif

