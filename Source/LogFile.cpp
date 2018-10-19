#include "Core.h"

#include "LogFile.h"
#include "Timer.h"

FileLogger::FileLogger( const char* fname )
{
    logFile = fopen( fname, "wb" );
    startTick = Timer::FastTick();
}

FileLogger::~FileLogger()
{
    if( logFile )
    {
        fclose( logFile );
        logFile = NULL;
    }
}

void FileLogger::Write( const char* fmt, ... )
{
    if( logFile )
    {
        fprintf( logFile, "%10u) ", (Timer::FastTick() - startTick) / 1000 );
        va_list list;
        va_start( list, fmt );
        vfprintf( logFile, fmt, list );
        va_end( list );
    }
}
