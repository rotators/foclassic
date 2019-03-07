#include "Core.h"

#include <locale.h>
#ifndef FO_WINDOWS
# include <signal.h>
#endif

#include "FL/Fl.H"

#include "CommandLine.h"
#include "ConfigFile.h"
#include "Client.h"
#include "Exception.h"
#include "GameOptions.h"
#include "Ini.h"
#include "Keyboard.h"
#include "Log.h"
#include "Script.h"
#include "ScriptReservedFunctions.h"
#include "SinglePlayer.h"
#include "Timer.h"
#include "Thread.h"
#include "Version.h"
#include "Window.h"

using namespace std;

FOWindow* MainWindow = NULL;
FOClient* FOEngine = NULL;

int main( int argc, char** argv )
{
    setlocale( LC_ALL, "English" );

    // Threading
    #ifdef FO_WINDOWS
    pthread_win32_process_attach_np();
    #endif
    Thread::SetCurrentName( "Main" );

    // Exceptions
    #if defined (FO_D3D)
    GameOpt.OpenGLRendering = false;
    CatchExceptions( "ClientDX" );
    #else
    GameOpt.OpenGLRendering = true;
    CatchExceptions( "ClientGL" );
    #endif

    // Logging
    Timer::Init();
    LogToFile( "Client.log" );
    WriteLog( "Starting Client (%s)...\n", FOCLASSIC_VERSION_LOGSTRING );

    // Command line
    CommandLine = new CmdLine( argc, argv );
    GAME_OPTION_EXT( CommandLine ) = CommandLine;
    if( !CommandLine->IsOption( "no-restore-directory" ) )
        RestoreMainDirectory();

    // Options
    LoadConfigFile( FileManager::GetFullPath( GetConfigFileName(), PATH_ROOT ), SECTION_MAIN, SECTION_DETAIL, SECTION_UNUSED );
    GAME_OPTION_EXT( ConfigFile ) = ConfigFile;
    GetClientOptions();

    // Disable SIGPIPE signal
    #ifndef FO_WINDOWS
    signal( SIGPIPE, SIG_IGN );
    #endif

    // Singleplayer mode initialization
    #ifdef FO_WINDOWS
    char full_path[MAX_FOPATH] = { 0 };
    char path[MAX_FOPATH] = { 0 };
    char name[MAX_FOPATH] = { 0 };
    GetModuleFileName( NULL, full_path, MAX_FOPATH );
    FileManager::ExtractPath( full_path, path );
    FileManager::ExtractFileName( full_path, name );
    if( Str::Substring( name, "SinglePlayer" ) || CommandLine->IsOption( "SinglePlayer" ) )
    {
        WriteLog( "SinglePlayer mode.\n" );
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
        string server_exe = ConfigFile->GetStr( SECTION_CLIENT, "ServerAppName", "Server.exe" );
        string server_path = ConfigFile->GetStr( SECTION_CLIENT, "ServerPath", "..\\Server\\" );
        string server_cmdline = ConfigFile->GetStr( SECTION_CLIENT, "ServerCommandLine" );

        // Process attributes
        PROCESS_INFORMATION server;
        memzero( &server, sizeof(server) );
        STARTUPINFOA        sui;
        memzero( &sui, sizeof(sui) );
        sui.cb = sizeof(sui);
        HANDLE client_process = OpenProcess( SYNCHRONIZE, TRUE, GetCurrentProcessId() );
        char   command_line[2048];

        // Start server
        Str::Format( command_line, "\"%s%s\" --SinglePlayer %p:%p --LogPath %s %s", server_path.c_str(), server_exe.c_str(), map_file, client_process, path, server_cmdline.c_str() );
        if( !CreateProcess( NULL, command_line, NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, server_path.c_str(), &sui, &server ) )
        {
            WriteLog( "Can't start server process, error<%u>.\n", GetLastError() );
            return 0;
        }
        CloseHandle( server.hProcess );
        CloseHandle( server.hThread );
    }
    #endif

    // Check for already opened window
    #ifndef DEV_VERSION
    # ifdef FO_WINDOWS
    HANDLE h = CreateEvent( NULL, FALSE, FALSE, "foclassic_instance" ); // last change 20.09.2018
    if( !h || h == INVALID_HANDLE_VALUE || GetLastError() == ERROR_ALREADY_EXISTS )
    {
        ShowMessage( "FOClassic client is already running." );
        return 0;
    }
    # else
    // Todo: Linux
    # endif
    #endif

    // Create window
    MainWindow = new FOWindow();


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

    if( Script::PrepareContext( ClientFunctions.Finish, _FUNC_, "Game" ) )
        Script::RunPrepared();

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
    delete CommandLine;
    UnloadConfigFile();

    return 0;
}
