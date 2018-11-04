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

bool LoadConfigFile( const char* fname )
{
    if( !ConfigFile )
        ConfigFile = new Ini();

    bool result = ConfigFile->LoadFile( fname );

    // Merge "(Client|Mapper)(DX|GL)" into "(Client|Mapper)"
    #if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
    if( result )
    {
        ConfigFile->MergeSections( APP_SECTION, APP_SECTION_DETAIL, true );
        ConfigFile->RemoveSection( APP_SECTION_UNUSED );
    }
    #endif

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

const char* GetConfigFileName()
{
    // Default config names
    static char config_name[MAX_FOPATH] =
    {
        #if defined (FOCLASSIC_SERVER)
        "FOnlineServer.cfg\0--default-server-config--"
        #elif defined (FOCLASSIC_MAPPER)
        "Mapper.cfg\0--default-mapper-config--"
        #else         // FOCLASSIC_CLIENT and others
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

    string cfg = GetStr( MAPPER_SECTION, "ClientName", "FOnline" ) + ".cfg";
    ConfigFile->LoadFile( FileManager::GetFullPath( cfg.c_str(), PATH_ROOT ), false );
    #  ifdef FO_D3D
    ConfigFile->MergeSections( CLIENT_SECTION, CLIENT_DX_SECTION, true );
    ConfigFile->RemoveSection( CLIENT_GL_SECTION );
    #  else
    ConfigFile->MergeSections( CLIENT_SECTION, CLIENT_GL_SECTION, true );
    ConfigFile->RemoveSection( CLIENT_DX_SECTION );
    #  endif
    # endif

    // Language
    // cfg.GetStr( CLIENT_SECTION, "Language", "engl", buf );
    // GETOPTIONS_CMD_LINE_STR( buf, "Language" );

    // Int / Bool
    GameOpt.OpenGLDebug = GetBool( CLIENT_SECTION, "OpenGLDebug" );
    GameOpt.AssimpLogging = GetBool( CLIENT_SECTION, "AssimpLogging" );
    GameOpt.FullScreen = GetBool( CLIENT_SECTION, "FullScreen" );
    GameOpt.VSync = GetBool( CLIENT_SECTION, "VSync" );
    GameOpt.Light = GetInt( CLIENT_SECTION, "Light", 0, 100, 20 );
    GameOpt.ScrollDelay = GetInt( CLIENT_SECTION, "ScrollDelay", 0, 100, 10 );
    GameOpt.ScrollStep = GetInt( CLIENT_SECTION, "ScrollStep", 4, 32, 12 );
    GameOpt.TextDelay = GetInt( CLIENT_SECTION, "TextDelay", 1000, 3000, 30000 );
    GameOpt.DamageHitDelay = GetInt( CLIENT_SECTION, "DamageHitDelay", 0, 30000, 0 );
    GameOpt.ScreenWidth = GetInt( CLIENT_SECTION, "ScreenWidth", 100, 30000, 800 );
    GameOpt.ScreenHeight = GetInt( CLIENT_SECTION, "ScreenHeight", 100, 30000, 600 );
    GameOpt.MultiSampling = GetInt( CLIENT_SECTION, "MultiSampling", -1, 16, -1 );
    GameOpt.AlwaysOnTop = GetBool( CLIENT_SECTION, "AlwaysOnTop" );
    GameOpt.FlushVal = GetInt( CLIENT_SECTION, "FlushValue", 1, 1000, 50 );
    GameOpt.BaseTexture = GetInt( CLIENT_SECTION, "BaseTexture", 128, 8192, 1024 );
    GameOpt.FixedFPS = GetInt( CLIENT_SECTION, "FixedFPS", -10000, 10000, 100 );
    GameOpt.MsgboxInvert = GetBool( CLIENT_SECTION, "InvertMessBox" );
    GameOpt.MessNotify = GetBool( CLIENT_SECTION, "WinNotify" );
    GameOpt.SoundNotify = GetBool( CLIENT_SECTION, "SoundNotify" );
    GameOpt.Port = GetInt( CLIENT_SECTION, "RemotePort", 0, 0xFFFF, 4000 );
    GameOpt.ProxyType = GetInt( CLIENT_SECTION, "ProxyType", 0, 3, 0 );
    GameOpt.ProxyPort = GetInt( CLIENT_SECTION, "ProxyPort", 0, 0xFFFF, 1080 );
    GameOpt.AlwaysRun = GetBool( CLIENT_SECTION, "AlwaysRun" );
    GameOpt.DefaultCombatMode = GetInt( CLIENT_SECTION, "DefaultCombatMode", COMBAT_MODE_ANY, COMBAT_MODE_TURN_BASED, COMBAT_MODE_ANY );
    GameOpt.IndicatorType = GetInt( CLIENT_SECTION, "IndicatorType", INDICATOR_LINES, INDICATOR_BOTH, INDICATOR_LINES );
    GameOpt.DoubleClickTime = GetInt( CLIENT_SECTION, "DoubleClickTime", 0, 1000, 0 );
    GameOpt.CombatMessagesType = GetInt( CLIENT_SECTION, "CombatMessagesType", 0, 1, 0 );
    GameOpt.Animation3dFPS = GetInt( CLIENT_SECTION, "Animation3dFPS", 0, 1000, 10 );
    GameOpt.Animation3dSmoothTime = GetInt( CLIENT_SECTION, "Animation3dSmoothTime", 0, 10000, 250 );

    GameOpt.HelpInfo = CommandLine->IsOption( "HelpInfo" );
    GameOpt.DebugInfo = CommandLine->IsOption( "DebugInfo" );
    GameOpt.DebugNet = CommandLine->IsOption( "DebugNet" );
    GameOpt.DebugSprites = CommandLine->IsOption( "DebugSprites" );

    // Str
    GameOpt.FoDataPath = GetStrPath( CLIENT_SECTION, "DataPath", DIR_SLASH_SD "data" );
    GameOpt.Host = GetStr( CLIENT_SECTION, "RemoteHost", "localhost" );
    GameOpt.ProxyHost = GetStr( CLIENT_SECTION, "ProxyHost", "localhost" );
    GameOpt.ProxyUser = GetStr( CLIENT_SECTION, "ProxyUser", "" );
    GameOpt.ProxyPass = GetStr( CLIENT_SECTION, "ProxyPass", "" );
    GameOpt.Name = GetStr( CLIENT_SECTION, "UserName", "" );
    GameOpt.KeyboardRemap = GetStr( CLIENT_SECTION, "KeyboardRemap", "" );

    // Logging
    LogToDebugOutput( GetBool( CLIENT_SECTION, "LoggingDebugOutput" ) );
    LogWithTime( GetBool( CLIENT_SECTION, "LoggingTime" ) );
    LogWithThread( GetBool( CLIENT_SECTION, "LoggingThread" ) );
}
#endif // FOCLASSIC_CLIENT || FOCLASSIC_MAPPER

#ifdef FOCLASSIC_MAPPER

void GetMapperOptions()
{
    // Read config file
    GameOpt.ClientPath = GetStrPath( MAPPER_SECTION, "ClientPath", "" );
    GameOpt.ServerPath = GetStrPath( MAPPER_SECTION, "ServerPath", "" );

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
    ServerGameSleep = ConfigFile->GetInt( SERVER_SECTION, "GameSleep", 10 );
    Script::SetConcurrentExecution( ConfigFile->GetBool( SERVER_SECTION, "ScriptConcurrentExecution", false ) );
    WorldSaveManager = ConfigFile->GetInt( SERVER_SECTION, "WorldSaveManager", 1 ) == 1;
    # else
    // Load server config
    FileManager::SetDataPath( GameOpt.ServerPath.c_str() );

    string cfg = GetStr( MAPPER_SECTION, "ServerName", "Server" ) + ".cfg";
    ConfigFile->LoadFile( FileManager::GetFullPath( cfg.c_str(), PATH_SERVER_ROOT ), false );
    # endif
}
#endif
