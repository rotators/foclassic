#include "Core.h"

#include <locale.h>

#include "FL/Fl.H"

#include "CommandLine.h"
#include "ConfigFile.h"
#include "Exception.h"
#include "GameOptions.h"
#include "Ini.h"
#include "Log.h"
#include "Mapper.h"
#include "Script.h"
#include "SinglePlayer.h"
#include "Thread.h"
#include "Timer.h"
#include "Version.h"
#include "Window.h"

FOWindow* MainWindow = NULL;
FOMapper* Mapper = NULL;

int main( int argc, char** argv )
{
    setlocale( LC_ALL, "English" );

    // Threading
    #ifdef FO_WINDOWS
    pthread_win32_process_attach_np();
    #endif
    Thread::SetCurrentName( "GUI" );

    // Exceptions
    #if defined (FO_D3D)
    GameOpt.OpenGLRendering = false;
    CatchExceptions( "MapperDX" );
    #else
    GameOpt.OpenGLRendering = true;
    CatchExceptions( "MapperGL" );
    #endif

    // Logging
    LogToFile( "Mapper.log" );
    WriteLog( "Starting Mapper (%s)...\n", FOCLASSIC_VERSION_LOGSTRING );

    // Command line
    CommandLine = new CmdLine( argc, argv );
    GAME_OPTION_EXT( CommandLine ) = CommandLine;
    if( !CommandLine->IsOption( "no-restore-directory" ) )
        RestoreMainDirectory();

    // Options
    LoadConfigFile( FileManager::GetFullPath( GetConfigFileName(), PATH_MAPPER_ROOT ), SECTION_MAIN, SECTION_DETAIL, SECTION_UNUSED );
    GAME_OPTION_EXT( ConfigFile ) = ConfigFile;
    GetMapperOptions();
    GetClientOptions();
    GetServerOptions();
    Script::SetRunTimeout( 0, 0 );

    // Timer
    Timer::Init();

    // Create window
    MainWindow = new FOWindow();

    // Create engine
    Mapper = new FOMapper();
    if( !Mapper || !Mapper->Init() )
    {
        WriteLog( "FOnline engine initialization fail.\n" );
        GameOpt.Quit = true;
        return 0;
    }

    // Loop
    while( !GameOpt.Quit && Fl::check() )
        Mapper->MainLoop();
    GameOpt.Quit = true;

    if( Script::PrepareContext( MapperFunctions.Finish, _FUNC_, "Game" ) )
        Script::RunPrepared();

    // Destroy engine
    Mapper->Finish();
    SAFEDEL( Mapper );

    // Finish
    #ifdef FO_WINDOWS
    if( Singleplayer )
        SingleplayerData.Finish();
    #endif
    WriteLog( "FOnline finished.\n" );
    LogFinish();

    delete CommandLine;
    UnloadConfigFile();

    return 0;
}
