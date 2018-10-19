#include "Core.h"

#include "CommandLine.h"
#include "Text.h"

char   CommandLine[MAX_FOTEXT] = { 0 };
char** CommandLineArgValues = NULL;
uint   CommandLineArgCount = 0;

void SetCommandLine( uint argc, char** argv )
{
    CommandLineArgCount = argc;
    CommandLineArgValues = new char*[CommandLineArgCount];
    for( uint i = 0; i < argc; i++ )
    {
        Str::Append( CommandLine, argv[i] );
        Str::Append( CommandLine, " " );
        CommandLineArgValues[i] = Str::Duplicate( argv[i] );
    }
}
