#include "Core.h"

#include <stdarg.h>
#include <stdio.h>

#include "Command.h"
#include "CommandLine.h"
#include "Text.h"

// Commands
#include "ShowWorld.h"

using namespace std;

#define FOCMD_VERSION    "0.1"
static map<string, Command*> Commands;

int Exit( int return_code )
{
    for( auto it = Commands.begin(), end = Commands.end(); it != end; ++it )
    {
        Command* cmd = it->second;
        if( cmd )
            delete cmd;
    }

    Commands.clear();

    return return_code;
}

void WriteLogInternal( bool prefixes, const char* func, const char* frmt, va_list& list )
{
    char str[MAX_LOGTEXT] = { 0 };
    if( func )
        Str::Append( str, func );

    size_t len = Str::Length( str );
    vsnprintf( &str[len], MAX_LOGTEXT - len, frmt, list );
    str[MAX_LOGTEXT - 1] = 0;

    printf( "%s", str );
}

void WriteLog( const char* frmt, ... )
{
    va_list list;
    va_start( list, frmt );
    WriteLogInternal( true, NULL, frmt, list );
    va_end( list );
}

void WriteLogF( const char* func, const char* frmt, ... )
{
    va_list list;
    va_start( list, frmt );
    WriteLogInternal( true, func, frmt, list );
    va_end( list );
}

void WriteLogX( const char* frmt, ... )
{
    va_list list;
    va_start( list, frmt );
    WriteLogInternal( false, NULL, frmt, list );
    va_end( list );
}

int Usage( string msg = string(), int return_code = EXIT_FAILURE )
{
    WriteLog( "Usage: FOCmd [command] <options>\n" );
    WriteLog( "\n" );
    WriteLog( "COMMANDS:\n" );

    auto   end = Commands.end();

    size_t maxInfoLen = 0;
    for( auto it = Commands.begin(); it != end; ++it )
    {
        string info = it->first + " " + it->second->UsageArguments();

        if( info.size() > maxInfoLen )
            maxInfoLen = info.size();
    }

    for( auto it = Commands.begin(); it != end; ++it )
    {
        string   info = it->first + " " + it->second->UsageArguments();
        Command* command = it->second;
        string   spaces = "  ";

        for( size_t len = info.size(); len < maxInfoLen; len++ )
        {
            spaces += " ";
        }

        WriteLog( "    %s%s%s\n", info.c_str(), spaces.c_str(), command->UsageDescription().c_str() );
    }

    if( !msg.empty() )
    {
        WriteLog( "\n%s\n", msg.c_str() );
    }

    return Exit( return_code );
}

int main( int argc, char** argv )
{
    setvbuf( stdout, NULL, _IONBF, 0 );

    WriteLog( "FOCmd %s-%u\n", FOCMD_VERSION, FOCLASSIC_VERSION );

    Commands["ShowWorld"] = new ShowWorld();

    CommandLine = new CmdLine( argc, argv );
    string command = CommandLine->GetArg( 0 );

    if( command.empty() )
        return Usage();

    auto it = Commands.find( command );
    if( it == Commands.end() )
        return Usage( "ERROR: Unknown command '" + command + "'" );

    int return_code = it->second->Run();

    return Exit( return_code );
}
