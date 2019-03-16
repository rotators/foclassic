#ifndef __SCRIPT_UTILS__
#define __SCRIPT_UTILS__

#include "Types.h"

class Ini;

namespace Script
{
    // Get asERetCodes (from angelscript.h) as text
    const char* GetASReturnCode( int return_code );

    // Parse already loaded [scripts_cfg] and copy valid entries to [config]
    // In most cases, scripts_cfg should be loaded with KeepKeysOrder=true
    bool LoadConfigFile( Ini* scripts_cfg, Ini* config, const std::string& section_modules, const std::string& section_binds );
};

#endif // __SCRIPT_UTILS__ //
