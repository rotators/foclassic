#include "StdAfx.h"
#include "CMake.h"
#include "Client.h"
#include "Exception.h"
#include "Version.h"
#include "Keyboard.h"
#include <locale.h>
#ifndef FO_WINDOWS
# include <signal.h>
#endif

FOWindow* MainWindow = NULL;
FOClient* FOEngine = NULL;

int main( int argc, char** argv )
{
    RestoreMainDirectory();

    // Threading
    #ifdef FO_WINDOWS
    pthread_win32_process_attach_np();
    #endif
    Thread::SetCurrentName( "Main" );

    // Disable SIGPIPE signal
    #ifndef FO_WINDOWS
    signal( SIGPIPE, SIG_IGN );
    #endif

    // Exceptions
    #if defined (FO_D3D)
    CatchExceptions( "ClientDX" );
    #else
    CatchExceptions( "ClientGL" );
    #endif


    // Make command line
    SetCommandLine( argc, argv );

    // Stuff
    Timer::Init();
    LogToFile( "FOClassic.log" );

    // Singleplayer mode initialization
    #ifdef FO_WINDOWS
    char full_path[MAX_FOPATH] = { 0 };
    char path[MAX_FOPATH] = { 0 };
    char name[MAX_FOPATH] = { 0 };
    GetModuleFileName( NULL, full_path, MAX_FOPATH );
    FileManager::ExtractPath( full_path, path );
    FileManager::ExtractFileName( full_path, name );
    if( Str::Substring( name, "Singleplayer" ) || Str::Substring( CommandLine, "Singleplayer" ) )
    {
        WriteLog( "Singleplayer mode.\n" );
        Singleplayer = true;
        Timer::SetGamePause( true );

        // Create interprocess shared data
        HANDLE map_file = SingleplayerData.Init();
        if( !map_file )
        {
            WriteLog( "Can't map shared data to memory.\n" );
            return 0;
        }

        // Fill interprocess initial data
        if( SingleplayerData.Lock() )
        {
            // Initialize other data
            SingleplayerData.NetPort = 0;
            SingleplayerData.Pause = true;

            SingleplayerData.Unlock();
        }
        else
        {
            WriteLog( "Can't lock mapped file.\n" );
            return 0;
        }

        // Config parsing
        IniParser cfg;
        char      server_exe[MAX_FOPATH] = { 0 };
        char      server_path[MAX_FOPATH] = { 0 };
        char      server_cmdline[MAX_FOPATH] = { 0 };
        cfg.LoadFile( GetConfigFileName(), PT_ROOT );
        cfg.GetStr( CLIENT_CONFIG_APP, "ServerAppName", "Server.exe", server_exe );
        cfg.GetStr( CLIENT_CONFIG_APP, "ServerPath", "..\\server\\", server_path );
        cfg.GetStr( CLIENT_CONFIG_APP, "ServerCommandLine", "", server_cmdline );

        // Process attributes
        PROCESS_INFORMATION server;
        memzero( &server, sizeof(server) );
        STARTUPINFOA        sui;
        memzero( &sui, sizeof(sui) );
        sui.cb = sizeof(sui);
        HANDLE client_process = OpenProcess( SYNCHRONIZE, TRUE, GetCurrentProcessId() );
        char   command_line[2048];

        // Start server
        Str::Format( command_line, "\"%s%s\" -singleplayer %p %p %s -logpath %s", server_path, server_exe, map_file, client_process, server_cmdline, path );
        if( !CreateProcess( NULL, command_line, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, server_path, &sui, &server ) )
        {
            WriteLog( "Can't start server process, error<%u>.\n", GetLastError() );
            return 0;
        }
        CloseHandle( server.hProcess );
        CloseHandle( server.hThread );
    }
    #endif

    // Check for already runned window
    #ifndef DEV_VERSION
    # ifdef FO_WINDOWS
    HANDLE h = CreateEvent( NULL, FALSE, FALSE, "foclassic_instance" ); // last change 20.09.2018
    if( !h || h == INVALID_HANDLE_VALUE || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        ShowMessage( "FOClassic engine instance is already running." );
        return 0;
    }
    # else
    // Todo: Linux
    # endif
    #endif

    // Options
    GetClientOptions();

    // Create window
    MainWindow = new FOWindow();

    // Start message
    WriteLog( "Starting FOClassic (version %u)...\n", FOCLASSIC_VERSION );

    // Create engine
    FOEngine = new FOClient();
    if( !FOEngine || !FOEngine->Init() )
    {
        WriteLog( "FOClassic engine initialization fail.\n" );
        GameOpt.Quit = true;
        return -1;
    }

    // Loop
    while( !GameOpt.Quit && Fl::check() )
    {
        if( !FOEngine->MainLoop() )
            Thread::Sleep( 100 );
    }
    GameOpt.Quit = true;

    // Destroy engine
    FOEngine->Finish();
    SAFEDEL( FOEngine );

    // Finish
    #ifdef FO_WINDOWS
    if( Singleplayer )
        SingleplayerData.Finish();
    #endif
    WriteLog( "FOClassic finished.\n" );
    LogFinish();
    Timer::Finish();

    return 0;
}
