#include "Core.h"
#include "App.h"

#include "CommandLine.h"
#include "FileSystem.h"
#include "ShowWorld.h"
#include "ShowWorld/WorldSaveDump.h"

using namespace std;

ShowWorld::ShowWorld()
{}

string ShowWorld::UsageArguments()
{
    return "[worldsave.fo]";
}
string ShowWorld::UsageDescription()
{
    return "???";
}

int ShowWorld::Run()
{
    string name = CommandLine->GetArg( 1 );
    string name_short = name;

    if( name.empty() )
    {
        App.WriteLog( "ERROR: Missing filename\n" );
        return EXIT_FAILURE;
    }
    else
    {
        replace( name_short.begin(), name_short.end(), '\\', '/' );
        size_t pos = name_short.find_last_of( '/' );
        if( pos != string::npos )
            name_short = name_short.substr( pos + 1 );
    }

    if( !FileExist( name.c_str() ) )
    {
        App.WriteLog( "ERROR: file<%s> does not exists\n", name.c_str() );
        return EXIT_FAILURE;
    }

    void* file = FileOpen( name.c_str(), false );
    if( !file )
    {
        App.WriteLog( "ERROR: file<%s> cannot be opened\n", name.c_str() );
        return EXIT_FAILURE;
    }

    WorldSave::Object::Signature signature;
    if( !signature.Load( file, name ) )
        return EXIT_FAILURE;

    WorldSave* world = WorldSaveDump::NewWorld( signature, file, name_short );

    if( !world )
    {
        App.WriteLog( "ERROR: unknown version<%u>\n", signature.Version );
        return EXIT_FAILURE;
    }

    bool success = world->LoadWorld();

    delete world;

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
