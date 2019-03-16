#include "Core.h"
#include "CMake.AngelScript.h"
#include "App.h"

#include <cstdlib>

#include "Ini.h"
#include "ScriptUtils.h"
#include "Text.h"

using namespace std;

const char* Script::GetASReturnCode( int return_code )
{
    if( return_code > 0 || abs( return_code ) >= ASRetCodeLen )
        return Str::FormatBuf( "%d", return_code );

    return Str::Duplicate( ASRetCode[abs( return_code )] );
}

bool Script::LoadConfigFile( Ini* scripts_cfg, Ini* config, const string& section_modules, const string& section_binds )
{
    // <scripts_cfg> must be loaded with KeepKeysOrder=true

    if( !scripts_cfg || !config )
        return false;

    // Cache keys order settings
    bool keep_keys_order = config->KeepKeysOrder;
    config->KeepKeysOrder = true;

    // [APP_TYPE scripts] -- optional
    // module_name = load [options]
    // module_name = skip [options]
    //
    // can be empty

    StrVec cfg_modules;
    scripts_cfg->GetSectionKeys( section_modules, cfg_modules, true );

    for( auto it = cfg_modules.begin(), end = cfg_modules.end(); it != end; it++ )
    {
        const string& module_name = *it;

        // Ignore empty keys

        if( scripts_cfg->IsSectionKeyEmpty( section_modules, module_name ) )
            continue;

        StrVec module_options = scripts_cfg->GetStrVec( section_modules, module_name );
        if( !module_options.size() )
            continue;

        // Ignore modules with 'skip'/'SKIP' option

        if( find( module_options.begin(), module_options.end(), "skip" ) != module_options.end() ||
            find( module_options.begin(), module_options.end(), "SKIP" ) != module_options.end() )
            continue;

        // Ignore modules without 'load'/'LOAD' option

        if( find( module_options.begin(), module_options.end(), "load" ) == module_options.end() &&
            find( module_options.begin(), module_options.end(), "LOAD" ) == module_options.end() )
            continue;

        // TODO remove unknown options

        config->SetStr( section_modules, module_name, scripts_cfg->GetStr( section_modules, module_name ) );
    }

    // [APP_TYPE binds] -- required
    // reserved_function_name = [target] [type]
    // all                    = [target] [type]

    StrVec cfg_binds;
    if( !scripts_cfg->IsSection( section_binds ) || !scripts_cfg->GetSectionKeys( section_binds, cfg_binds, true ) )
    {
        App.WriteLogF( _FUNC_, " - Scripts config section<%s> is missing or empty\n", section_binds.c_str() );
        config->KeepKeysOrder = keep_keys_order;

        return false;
    }

    for( auto it = cfg_binds.begin(), end = cfg_binds.end(); it != end; it++ )
    {
        const string& bind_name = *it;

        // Ignore empty keys
        if( scripts_cfg->IsSectionKeyEmpty( section_binds, bind_name ) )
            continue;

        config->SetStr( section_binds, bind_name, scripts_cfg->GetStr( section_binds, bind_name ) );
    }

    // Restore keys order settings
    config->KeepKeysOrder = keep_keys_order;

    return true;
}
