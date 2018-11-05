#include "Core.h"

#include "CommandLine.h"
#include "ConfigFile.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "GameOptions.h"
#include "Ini.h"
#include "Log.h"
#include "Script.h"
#include "Text.h"

Ini* ConfigFile = NULL;

const char* GetConfigFileName()
{
    // Default config names
    static char config_name[MAX_FOPATH] =
    {
        #if defined (FOCLASSIC_SERVER)
        "FOnlineServer.cfg\0--default-server-config--"
        #elif defined (FOCLASSIC_MAPPER)
        "Mapper.cfg\0--default-mapper-config--"
        #else                 // FOCLASSIC_CLIENT and others
        "FOnline.cfg\0--default-client-config--"
        #endif
    };

    // Extract config name from current exe
    static bool processed = false;
    if( !processed )
    {
        // Call once
        processed = true;

        // Get full path
        char module_name[MAX_FOPATH];
        #ifdef FO_WINDOWS
        if( !GetModuleFileName( NULL, module_name, sizeof(module_name) ) )
            return config_name;
        #else
        // Todo: Linux CommandLineArgValues[0] ?
        #endif

// Change extension
        char* ext = (char*)FileManager::GetExtension( module_name );
        if( !ext )
            return config_name;
        Str::Copy( ext, 4, "cfg" );

        // Check availability
        if( !FileExist( module_name ) )
            return config_name;

        // Get file name
        const char* name = NULL;
        for( size_t i = 0, j = Str::Length( module_name ); i < j; i++ )
            if( module_name[i] == DIR_SLASH_C )
                name = &module_name[i + 1];
        if( !name )
            return config_name;

        // Set as main
        Str::Copy( config_name, name );
    }

    return config_name;
}

bool LoadConfigFile( const char* fname, const char* main /* = NULL */, const char* detail /* = NULL */, const char* unused /* = NULL */ )
{
    if( !ConfigFile )
        ConfigFile = new Ini();

    bool result = ConfigFile->LoadFile( fname );

    if( result && main && detail && unused )
    {
        ConfigFile->MergeSections( main, detail, true );
        ConfigFile->RemoveSection( unused );
    }

    return result;
}

void UnloadConfigFile()
{
    if( ConfigFile )
    {
        ConfigFile->Unload();

        delete ConfigFile;
        ConfigFile = NULL;
    }
}

//

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)

bool GetBool( const string& section, const string& key )
{
    bool result = false;

    if( section.size() && key.size() )
        result = ConfigFile->GetBool( section, key, result );

    if( !result && key.size() && CommandLine->IsOption( key ) )
        result = true;

    return result;
}

int GetInt( const string& section, const string& key, const int& min, const int& max, const int& default_value )
{
    int result = default_value;

    if( section.size() && key.size() )
        result = ConfigFile->GetInt( section, key, result );

    if( key.size() )
        result = CommandLine->GetInt( key, result );

    if( result < min || result > max )
        result = default_value;

    return result;
}

string GetStr( const string& section, const string& key, const string& default_value )
{
    string result = default_value;

    if( section.size() && key.size() )
        result = ConfigFile->GetStr( section, key, result );

    if( key.size() )
        result = CommandLine->GetStr( key, result );

    return result;
}

string GetStrPath( const string& section, const string& key, const string& default_value )
{
    string result = GetStr( section, key, default_value );

    char*  buf = Str::Duplicate( result.c_str() );
    FileManager::FormatPath( buf );
    result = string( buf );

    return result;
}

void GetClientOptions()
{
    # ifdef FOCLASSIC_MAPPER
    // Load client config
    FileManager::SetDataPath( GameOpt.ClientPath.c_str() );

    string cfg = GetStr( SECTION_MAPPER, "ClientName", "FOnline" ) + ".cfg";
    ConfigFile->LoadFile( FileManager::GetFullPath( cfg.c_str(), PATH_ROOT ), false );
    #  ifdef FO_D3D
    ConfigFile->MergeSections( SECTION_CLIENT, SECTION_CLIENT_DX, true );
    ConfigFile->RemoveSection( SECTION_CLIENT_GL );
    #  else
    ConfigFile->MergeSections( SECTION_CLIENT, SECTION_CLIENT_GL, true );
    ConfigFile->RemoveSection( SECTION_CLIENT_DX );
    #  endif
    # endif

    // Language
    // cfg.GetStr( SECTION_CLIENT, "Language", "engl", buf );
    // GETOPTIONS_CMD_LINE_STR( buf, "Language" );

    // Int / Bool
    GameOpt.OpenGLDebug = GetBool( SECTION_CLIENT, "OpenGLDebug" );
    GameOpt.AssimpLogging = GetBool( SECTION_CLIENT, "AssimpLogging" );
    GameOpt.FullScreen = GetBool( SECTION_CLIENT, "FullScreen" );
    GameOpt.VSync = GetBool( SECTION_CLIENT, "VSync" );
    GameOpt.Light = GetInt( SECTION_CLIENT, "Light", 0, 100, 20 );
    GameOpt.ScrollDelay = GetInt( SECTION_CLIENT, "ScrollDelay", 0, 100, 10 );
    GameOpt.ScrollStep = GetInt( SECTION_CLIENT, "ScrollStep", 4, 32, 12 );
    GameOpt.TextDelay = GetInt( SECTION_CLIENT, "TextDelay", 1000, 3000, 30000 );
    GameOpt.DamageHitDelay = GetInt( SECTION_CLIENT, "DamageHitDelay", 0, 30000, 0 );
    GameOpt.ScreenWidth = GetInt( SECTION_CLIENT, "ScreenWidth", 100, 30000, 800 );
    GameOpt.ScreenHeight = GetInt( SECTION_CLIENT, "ScreenHeight", 100, 30000, 600 );
    GameOpt.MultiSampling = GetInt( SECTION_CLIENT, "MultiSampling", -1, 16, -1 );
    GameOpt.AlwaysOnTop = GetBool( SECTION_CLIENT, "AlwaysOnTop" );
    GameOpt.FlushVal = GetInt( SECTION_CLIENT, "FlushValue", 1, 1000, 50 );
    GameOpt.BaseTexture = GetInt( SECTION_CLIENT, "BaseTexture", 128, 8192, 1024 );
    GameOpt.FixedFPS = GetInt( SECTION_CLIENT, "FixedFPS", -10000, 10000, 100 );
    GameOpt.MsgboxInvert = GetBool( SECTION_CLIENT, "InvertMessBox" );
    GameOpt.MessNotify = GetBool( SECTION_CLIENT, "WinNotify" );
    GameOpt.SoundNotify = GetBool( SECTION_CLIENT, "SoundNotify" );
    GameOpt.Port = GetInt( SECTION_CLIENT, "RemotePort", 0, 0xFFFF, 4000 );
    GameOpt.ProxyType = GetInt( SECTION_CLIENT, "ProxyType", 0, 3, 0 );
    GameOpt.ProxyPort = GetInt( SECTION_CLIENT, "ProxyPort", 0, 0xFFFF, 1080 );
    GameOpt.AlwaysRun = GetBool( SECTION_CLIENT, "AlwaysRun" );
    GameOpt.DefaultCombatMode = GetInt( SECTION_CLIENT, "DefaultCombatMode", COMBAT_MODE_ANY, COMBAT_MODE_TURN_BASED, COMBAT_MODE_ANY );
    GameOpt.IndicatorType = GetInt( SECTION_CLIENT, "IndicatorType", INDICATOR_LINES, INDICATOR_BOTH, INDICATOR_LINES );
    GameOpt.DoubleClickTime = GetInt( SECTION_CLIENT, "DoubleClickTime", 0, 1000, 0 );
    GameOpt.CombatMessagesType = GetInt( SECTION_CLIENT, "CombatMessagesType", 0, 1, 0 );
    GameOpt.Animation3dFPS = GetInt( SECTION_CLIENT, "Animation3dFPS", 0, 1000, 10 );
    GameOpt.Animation3dSmoothTime = GetInt( SECTION_CLIENT, "Animation3dSmoothTime", 0, 10000, 250 );

    GameOpt.HelpInfo = CommandLine->IsOption( "HelpInfo" );
    GameOpt.DebugInfo = CommandLine->IsOption( "DebugInfo" );
    GameOpt.DebugNet = CommandLine->IsOption( "DebugNet" );
    GameOpt.DebugSprites = CommandLine->IsOption( "DebugSprites" );

    // Str
    GameOpt.FoDataPath = GetStrPath( SECTION_CLIENT, "DataPath", DIR_SLASH_SD "data" );
    GameOpt.Host = GetStr( SECTION_CLIENT, "RemoteHost", "localhost" );
    GameOpt.ProxyHost = GetStr( SECTION_CLIENT, "ProxyHost", "localhost" );
    GameOpt.ProxyUser = GetStr( SECTION_CLIENT, "ProxyUser", "" );
    GameOpt.ProxyPass = GetStr( SECTION_CLIENT, "ProxyPass", "" );
    GameOpt.Name = GetStr( SECTION_CLIENT, "UserName", "" );
    GameOpt.KeyboardRemap = GetStr( SECTION_CLIENT, "KeyboardRemap", "" );

    // Logging
    LogToDebugOutput( GetBool( SECTION_CLIENT, "LoggingDebugOutput" ) );
    LogWithTime( GetBool( SECTION_CLIENT, "LoggingTime" ) );
    LogWithThread( GetBool( SECTION_CLIENT, "LoggingThread" ) );
}
#endif // FOCLASSIC_CLIENT || FOCLASSIC_MAPPER

#ifdef FOCLASSIC_MAPPER

void GetMapperOptions()
{
    // Read config file
    GameOpt.ClientPath = GetStrPath( SECTION_MAIN, "ClientPath", "" );
    GameOpt.ServerPath = GetStrPath( SECTION_MAIN, "ServerPath", "" );

    if( GameOpt.ClientPath.length() && GameOpt.ClientPath.c_str()[GameOpt.ClientPath.length() - 1] != DIR_SLASH_C )
        GameOpt.ClientPath += DIR_SLASH_S;

    if( GameOpt.ServerPath.length() && GameOpt.ServerPath.c_str()[GameOpt.ServerPath.length() - 1] != DIR_SLASH_C )
        GameOpt.ServerPath += DIR_SLASH_S;
}
#endif

#ifdef FOCLASSIC_SERVER
bool FOQuit = false;
int  ServerGameSleep = 10;
uint VarsGarbageTime = 3600000;
bool WorldSaveManager = true;
bool LogicMT = false;
#endif

#if defined (FOCLASSIC_SERVER) || defined (FOCLASSIC_MAPPER)
void GetServerOptions()
{
    # ifdef FOCLASSIC_SERVER
    ServerGameSleep = ConfigFile->GetInt( SECTION_SERVER, "GameSleep", 10 );
    Script::SetConcurrentExecution( ConfigFile->GetBool( SECTION_SERVER, "ScriptConcurrentExecution", false ) );
    WorldSaveManager = ConfigFile->GetInt( SECTION_SERVER, "WorldSaveManager", 1 ) == 1;
    # else
    // Load server config
    FileManager::SetDataPath( GameOpt.ServerPath.c_str() );

    string cfg = GetStr( SECTION_MAIN, "ServerName", "Server" ) + ".cfg";
    ConfigFile->LoadFile( FileManager::GetFullPath( cfg.c_str(), PATH_SERVER_ROOT ), false );
    # endif
}
#endif
