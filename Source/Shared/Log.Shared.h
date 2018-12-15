#ifndef __LOG_SHARED__
#define __LOG_SHARED__

typedef void (* LogFuncPtr)( const char* str );
extern void LogToFunc( LogFuncPtr func_ptr, bool enable );

extern void WriteLog( const char* frmt, ... );
extern void WriteLogF( const char* func, const char* frmt, ... );

#endif // __LOG_SHARED__ //
