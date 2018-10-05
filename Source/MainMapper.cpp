#include <locale.h>

#include "FL/Fl.H"

#include "Core.h"

#include "Exception.h"
#include "Mapper.h"
#include "Thread.h"
#include "Version.h"
#include "Window.h"

FOWindow* MainWindow = NULL;
FOMapper* Mapper = NULL;

int main( int argc, char** argv )
{
    setlocale( LC_ALL, "English" );
    RestoreMainDirectory();

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

    // Make command line
    SetCommandLine( argc, argv );

    // Timer
    Timer::Init();

    LogToFile( "FOMapper.log" );

    GetClientOptions();

    WriteLog( "Starting Mapper (%s)...\n", MAPPER_VERSION_STR );

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

    return 0;
}
