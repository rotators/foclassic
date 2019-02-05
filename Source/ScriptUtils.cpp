#include "Core.h"
#include "CMake.AngelScript.h"

#include <stdlib.h>

#include "ScriptUtils.h"
#include "Text.h"

const char* Script::GetASReturnCode( int return_code )
{
    if( return_code > 0 || abs( return_code ) >= ASRetCodeLen )
        return Str::FormatBuf( "%d", return_code );

    return Str::Duplicate( ASRetCode[abs( return_code )] );
}
