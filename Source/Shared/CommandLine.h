#ifndef __COMMAND_LINE__
#define __COMMAND_LINE__

#include "Defines.h"
#include "Types.h"

extern char CommandLine[MAX_FOTEXT];
extern uint CommandLineArgCount;

extern void SetCommandLine( uint argc, char** argv );

#endif // __COMMAND_LINE__ //
