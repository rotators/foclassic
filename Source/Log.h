#ifndef __LOG__
#define __LOG__

#include <string>

#define _LOG_    __FUNCTION__ ":" __LINE__ " - "

typedef void ( * LogFuncPtr )( const char* str );

// Write formatted text
extern void WriteLog( const char* frmt, ... );
extern void WriteLogF( const char* func, const char* frmt, ... );
extern void WriteLogX( const char* frmt, ... );

// Append logging to
extern void LogToFile( const char* fname );                // File
extern void LogToFunc( LogFuncPtr func_ptr, bool enable ); // Extern function
extern void LogToTextBox( void* text_box );                // Text box (Fl_Text_Display)
extern void LogToBuffer( bool enable );                    // Buffer, to get value use LogGetBuffer
extern void LogToDebugOutput( bool enable );               // OutputDebugString / printf

// Other stuff
extern void LogFinish();                                   // Finish all logging
extern void LogWithTime( bool enable );                    // Logging with time
extern void LogWithThread( bool enable );                  // Logging with thread name
extern void LogGetBuffer( std::string& buf );              // Get buffer, if used LogBuffer

#endif // __LOG__
