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
    CatchExceptions( "MapperDX" );
    #else
    CatchExceptions( "MapperGL" );
    #endif

    // Command line
    CommandLine = new CmdLine( argc, argv );
    if( !CommandLine->IsOption( "no-restore-directory" ) )
        RestoreMainDirectory();

    // Options
    LoadConfigFile( FileManager::GetFullPath( GetConfigFileName(), PATH_MAPPER_ROOT ), SECTION_MAIN, SECTION_DETAIL, SECTION_UNUSED );
    GetMapperOptions();
    GetClientOptions();
    GetServerOptions();
    ConfigFile->Lock = true;
    Script::SetRunTimeout( 0, 0 );

    // Timer
    Timer::Init();

    LogToFile( "Mapper.log" );
    WriteLog( "Starting Mapper (version %u)...\n", FOCLASSIC_VERSION );



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