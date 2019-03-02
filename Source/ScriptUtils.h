#ifndef __SCRIPT_UTILS__
#define __SCRIPT_UTILS__

#include "Types.h"

class Ini;

namespace Script
{
    // Get asERetCodes (from angelscript.h) as text
    const char* GetASReturnCode( int return_code );
};

#endif // __SCRIPT_UTILS__ //
