#include "Core.h"

#include "CommandLine.h"
#include "ConfigFile.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "GameOptions.h"
#include "IniParser.h"
#include "Log.h"
#include "Script.h"
#include "Text.h"

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
void GetClientOptions()
{
    // Defines
    # define GETOPTIONS_CMD_LINE_INT( opt, str_id )                       \
        do { char* str = Str::Substring( CommandLine, str_id ); if( str ) \
                 opt = atoi( str + Str::Length( str_id ) + 1 ); } while( 0 )
    # define GETOPTIONS_CMD_LINE_BOOL( opt, str_id )                      \
        do { char* str = Str::Substring( CommandLine, str_id ); if( str ) \
                 opt = atoi( str + Str::Length( str_id ) + 1 ) != 0; } while( 0 )
    # define GETOPTIONS_CMD_LINE_BOOL_ON( opt, str_id )                   \
        do { char* str = Str::Substring( CommandLine, str_id ); if( str ) \
                 opt = true; } while( 0 )
    # define GETOPTIONS_CMD_LINE_STR( opt, str_id )                       \
        do { char* str = Str::Substring( CommandLine, str_id ); if( str ) \
                 sscanf( str + Str::Length( str_id ) + 1, "%s", opt ); } while( 0 )
    # define GETOPTIONS_CHECK( val_, min_, max_, def_ )                \
        do { int val__ = (int)val_; if( val__ < min_ || val__ > max_ ) \
                 val_ = def_; } while( 0 )

    char buf[MAX_FOTEXT];

    // Load config file
    # ifdef FOCLASSIC_MAPPER
    IniParser cfg_mapper;
    cfg_mapper.LoadFile( GetConfigFileName(), PATH_MAPPER_ROOT );

    cfg_mapper.GetStr( "ClientPath", "", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-ClientPath" );
    FileManager::FormatPath( buf );
    GameOpt.ClientPath = buf;
    if( GameOpt.ClientPath.length() && GameOpt.ClientPath.c_str()[GameOpt.ClientPath.length() - 1] != DIR_SLASH_C )
        GameOpt.ClientPath += DIR_SLASH_S;
    cfg_mapper.GetStr( "ServerPath", "", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-ServerPath" );
    FileManager::FormatPath( buf );
    GameOpt.ServerPath = buf;
    if( GameOpt.ServerPath.length() && GameOpt.ServerPath.c_str()[GameOpt.ServerPath.length() - 1] != DIR_SLASH_C )
        GameOpt.ServerPath += DIR_SLASH_S;

    FileManager::SetDataPath( GameOpt.ClientPath.c_str() );

    // Client config
    IniParser cfg;
    cfg_mapper.GetStr( "ClientName", "FOnline", buf );
    Str::Append( buf, ".cfg" );
    cfg.LoadFile( buf, PATH_ROOT );
    # else
    IniParser cfg;
    cfg.LoadFile( GetConfigFileName(), PATH_ROOT );
    # endif

    // Language
    cfg.GetStr( CLIENT_CONFIG_APP, "Language", "engl", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "Language" );

    // Int / Bool
    GameOpt.OpenGLDebug = cfg.GetInt( CLIENT_CONFIG_APP, "OpenGLDebug", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.OpenGLDebug, "-OpenGLDebug" );
    GameOpt.AssimpLogging = cfg.GetInt( CLIENT_CONFIG_APP, "AssimpLogging", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.AssimpLogging, "-AssimpLogging" );
    GameOpt.FullScreen = cfg.GetInt( CLIENT_CONFIG_APP, "FullScreen", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.FullScreen, "-FullScreen" );
    GameOpt.VSync = cfg.GetInt( CLIENT_CONFIG_APP, "VSync", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.VSync, "-VSync" );
    GameOpt.Light = cfg.GetInt( CLIENT_CONFIG_APP, "Light", 20 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.Light, "-Light" );
    GETOPTIONS_CHECK( GameOpt.Light, 0, 100, 20 );
    GameOpt.ScrollDelay = cfg.GetInt( CLIENT_CONFIG_APP, "ScrollDelay", 10 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.ScrollDelay, "-ScrollDelay" );
    GETOPTIONS_CHECK( GameOpt.ScrollDelay, 0, 100, 10 );
    GameOpt.ScrollStep = cfg.GetInt( CLIENT_CONFIG_APP, "ScrollStep", 12 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.ScrollStep, "-ScrollStep" );
    GETOPTIONS_CHECK( GameOpt.ScrollStep, 4, 32, 12 );
    GameOpt.TextDelay = cfg.GetInt( CLIENT_CONFIG_APP, "TextDelay", 3000 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.TextDelay, "-TextDelay" );
    GETOPTIONS_CHECK( GameOpt.TextDelay, 1000, 3000, 30000 );
    GameOpt.DamageHitDelay = cfg.GetInt( CLIENT_CONFIG_APP, "DamageHitDelay", 0 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.DamageHitDelay, "-OptDamageHitDelay" );
    GETOPTIONS_CHECK( GameOpt.DamageHitDelay, 0, 30000, 0 );
    GameOpt.ScreenWidth = cfg.GetInt( CLIENT_CONFIG_APP, "ScreenWidth", 0 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.ScreenWidth, "-ScreenWidth" );
    GETOPTIONS_CHECK( GameOpt.ScreenWidth, 100, 30000, 800 );
    GameOpt.ScreenHeight = cfg.GetInt( CLIENT_CONFIG_APP, "ScreenHeight", 0 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.ScreenHeight, "-ScreenHeight" );
    GETOPTIONS_CHECK( GameOpt.ScreenHeight, 100, 30000, 600 );
    GameOpt.MultiSampling = cfg.GetInt( CLIENT_CONFIG_APP, "MultiSampling", 0 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.MultiSampling, "-MultiSampling" );
    GETOPTIONS_CHECK( GameOpt.MultiSampling, -1, 16, -1 );
    GameOpt.AlwaysOnTop = cfg.GetInt( CLIENT_CONFIG_APP, "AlwaysOnTop", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.AlwaysOnTop, "-AlwaysOnTop" );
    GameOpt.FlushVal = cfg.GetInt( CLIENT_CONFIG_APP, "FlushValue", 50 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.FlushVal, "-FlushValue" );
    GETOPTIONS_CHECK( GameOpt.FlushVal, 1, 1000, 50 );
    GameOpt.BaseTexture = cfg.GetInt( CLIENT_CONFIG_APP, "BaseTexture", 1024 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.BaseTexture, "-BaseTexture" );
    GETOPTIONS_CHECK( GameOpt.BaseTexture, 128, 8192, 1024 );
    GameOpt.FixedFPS = cfg.GetInt( CLIENT_CONFIG_APP, "FixedFPS", 100 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.FixedFPS, "-FixedFPS" );
    GETOPTIONS_CHECK( GameOpt.FixedFPS, -10000, 10000, 100 );
    GameOpt.MsgboxInvert = cfg.GetInt( CLIENT_CONFIG_APP, "InvertMessBox", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.MsgboxInvert, "-InvertMessBox" );
    GameOpt.MessNotify = cfg.GetInt( CLIENT_CONFIG_APP, "WinNotify", true ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.MessNotify, "-WinNotify" );
    GameOpt.SoundNotify = cfg.GetInt( CLIENT_CONFIG_APP, "SoundNotify", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.SoundNotify, "-SoundNotify" );
    GameOpt.Port = cfg.GetInt( CLIENT_CONFIG_APP, "RemotePort", 4000 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.Port, "-RemotePort" );
    GETOPTIONS_CHECK( GameOpt.Port, 0, 0xFFFF, 4000 );
    GameOpt.ProxyType = cfg.GetInt( CLIENT_CONFIG_APP, "ProxyType", 0 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.ProxyType, "-ProxyType" );
    GETOPTIONS_CHECK( GameOpt.ProxyType, 0, 3, 0 );
    GameOpt.ProxyPort = cfg.GetInt( CLIENT_CONFIG_APP, "ProxyPort", 8080 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.ProxyPort, "-ProxyPort" );
    GETOPTIONS_CHECK( GameOpt.ProxyPort, 0, 0xFFFF, 1080 );
    GameOpt.AlwaysRun = cfg.GetInt( CLIENT_CONFIG_APP, "AlwaysRun", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( GameOpt.AlwaysRun, "-AlwaysRun" );
    GameOpt.DefaultCombatMode = cfg.GetInt( CLIENT_CONFIG_APP, "DefaultCombatMode", COMBAT_MODE_ANY );
    GETOPTIONS_CMD_LINE_INT( GameOpt.DefaultCombatMode, "-DefaultCombatMode" );
    GETOPTIONS_CHECK( GameOpt.DefaultCombatMode, COMBAT_MODE_ANY, COMBAT_MODE_TURN_BASED, COMBAT_MODE_ANY );
    GameOpt.IndicatorType = cfg.GetInt( CLIENT_CONFIG_APP, "IndicatorType", COMBAT_MODE_ANY );
    GETOPTIONS_CMD_LINE_INT( GameOpt.IndicatorType, "-IndicatorType" );
    GETOPTIONS_CHECK( GameOpt.IndicatorType, INDICATOR_LINES, INDICATOR_BOTH, INDICATOR_LINES );
    GameOpt.DoubleClickTime = cfg.GetInt( CLIENT_CONFIG_APP, "DoubleClickTime", COMBAT_MODE_ANY );
    GETOPTIONS_CMD_LINE_INT( GameOpt.DoubleClickTime, "-DoubleClickTime" );
    GETOPTIONS_CHECK( GameOpt.DoubleClickTime, 0, 1000, 0 );
    GameOpt.CombatMessagesType = cfg.GetInt( CLIENT_CONFIG_APP, "CombatMessagesType", 0 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.CombatMessagesType, "-CombatMessagesType" );
    GETOPTIONS_CHECK( GameOpt.CombatMessagesType, 0, 1, 0 );
    GameOpt.Animation3dFPS = cfg.GetInt( CLIENT_CONFIG_APP, "Animation3dFPS", 10 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.Animation3dFPS, "-Animation3dFPS" );
    GETOPTIONS_CHECK( GameOpt.Animation3dFPS, 0, 1000, 10 );
    GameOpt.Animation3dSmoothTime = cfg.GetInt( CLIENT_CONFIG_APP, "Animation3dSmoothTime", 0 );
    GETOPTIONS_CMD_LINE_INT( GameOpt.Animation3dSmoothTime, "-Animation3dSmoothTime" );
    GETOPTIONS_CHECK( GameOpt.Animation3dSmoothTime, 0, 10000, 250 );

    GETOPTIONS_CMD_LINE_BOOL_ON( GameOpt.HelpInfo, "-HelpInfo" );
    GETOPTIONS_CMD_LINE_BOOL_ON( GameOpt.DebugInfo, "-DebugInfo" );
    GETOPTIONS_CMD_LINE_BOOL_ON( GameOpt.DebugNet, "-DebugNet" );
    GETOPTIONS_CMD_LINE_BOOL_ON( GameOpt.DebugSprites, "-DebugSprites" );

    // Str
    cfg.GetStr( CLIENT_CONFIG_APP, "FonlineDataPath", DIR_SLASH_SD "data", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-FonlineDataPath" );
    FileManager::FormatPath( buf );
    GameOpt.FoDataPath = buf;
    cfg.GetStr( CLIENT_CONFIG_APP, "RemoteHost", "localhost", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-RemoteHost" );
    GameOpt.Host = buf;
    cfg.GetStr( CLIENT_CONFIG_APP, "ProxyHost", "localhost", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-ProxyHost" );
    GameOpt.ProxyHost = buf;
    cfg.GetStr( CLIENT_CONFIG_APP, "ProxyUser", "", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-ProxyUser" );
    GameOpt.ProxyUser = buf;
    cfg.GetStr( CLIENT_CONFIG_APP, "ProxyPass", "", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-ProxyPass" );
    GameOpt.ProxyPass = buf;
    cfg.GetStr( CLIENT_CONFIG_APP, "UserName", "", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-UserName" );
    GameOpt.Name = buf;
    cfg.GetStr( CLIENT_CONFIG_APP, "KeyboardRemap", "", buf );
    GETOPTIONS_CMD_LINE_STR( buf, "-KeyboardRemap" );
    GameOpt.KeyboardRemap = buf;

    // Logging
    bool logging = cfg.GetInt( CLIENT_CONFIG_APP, "Logging", 1 ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( logging, "-Logging" );
    if( !logging )
    {
        WriteLog( "File logging off.\n" );
        LogToFile( NULL );
    }

    logging = cfg.GetInt( CLIENT_CONFIG_APP, "LoggingDebugOutput", 0 ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( logging, "-LoggingDebugOutput" );
    if( logging )
        LogToDebugOutput( true );

    logging = cfg.GetInt( CLIENT_CONFIG_APP, "LoggingTime", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( logging, "-LoggingTime" );
    LogWithTime( logging );
    logging = cfg.GetInt( CLIENT_CONFIG_APP, "LoggingThread", false ) != 0;
    GETOPTIONS_CMD_LINE_BOOL( logging, "-LoggingThread" );
    LogWithThread( logging );

    # ifdef FOCLASSIC_MAPPER
    Script::SetRunTimeout( 0, 0 );
    # endif
}
#endif

#ifdef FOCLASSIC_SERVER
bool FOQuit = false;
int  ServerGameSleep = 10;
uint VarsGarbageTime = 3600000;
bool WorldSaveManager = true;
bool LogicMT = false;

void GetServerOptions()
{
    IniParser cfg;
    cfg.LoadFile( GetConfigFileName(), PATH_SERVER_ROOT );
    ServerGameSleep = cfg.GetInt( "GameSleep", 10 );
    Script::SetConcurrentExecution( cfg.GetInt( "ScriptConcurrentExecution", 0 ) != 0 );
    WorldSaveManager = (cfg.GetInt( "WorldSaveManager", 1 ) == 1);
}
#endif
