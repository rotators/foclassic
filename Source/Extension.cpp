#include "Core.h"

#include "Extension.h"
#include "Log.h"

using namespace std;

bool                              Extension::Initialized = false;
unordered_map<string, Extension*> Extension::Map;
bool                              Extension::RunEventDebug = false;

#if defined (FOCLASSIC_CLIENT) && FOCLASSIC_EXTENSIONS_CLIENT > 0
extern void InitExtensionsClient();
extern void FinishExtensionsClient();
#elif defined (FOCLASSIC_MAPPER) && FOCLASSIC_EXTENSIONS_MAPPER > 0
extern void InitExtensionsMapper();
extern void FinishExtensionsMapper();
#elif defined (FOCLASSIC_SERVER) && FOCLASSIC_EXTENSIONS_SERVER > 0
extern void InitExtensionsServer();
extern void FinishExtensionsServer();
#endif

void Extension::Init()
{
    if( Initialized )
        return;

    #if defined (FOCLASSIC_CLIENT) && FOCLASSIC_EXTENSIONS_CLIENT > 0
    InitExtensionsClient();
    #elif defined (FOCLASSIC_MAPPER) && FOCLASSIC_EXTENSIONS_MAPPER > 0
    InitExtensionsMapper();
    #elif defined (FOCLASSIC_SERVER) && FOCLASSIC_EXTENSIONS_SERVER > 0
    InitExtensionsServer();
    #endif

    RunEvent( ExtensionEvent::INIT );
}

void Extension::Finish()
{
    if( !Initialized )
        return;

    RunEvent( ExtensionEvent::FINISH );

    #if defined (FOCLASSIC_CLIENT) && FOCLASSIC_EXTENSIONS_CLIENT > 0
    FinishExtensionsClient();
    #elif defined (FOCLASSIC_MAPPER) && FOCLASSIC_EXTENSIONS_MAPPER > 0
    FinishExtensionsMapper();
    #elif defined (FOCLASSIC_SERVER) && FOCLASSIC_EXTENSIONS_SERVER > 0
    FinishExtensionsServer();
    #endif

    Map.clear();
}

void Extension::RunEvent( const uint& id )
{
    if( RunEventDebug )
        WriteLogF( _FUNC_, "<%u>\n", id );

    for( auto it = Extension::Map.begin(), end = Extension::Map.end(); it != end; ++it )
    {
        it->second->Event( id );
    }
}

//

Extension::Extension( const Extension& other )
{
    throw runtime_error( "Extension::Extension(const Extension& other)" );
}

Extension& Extension::operator=( const Extension& other )
{
    throw runtime_error( "Extension& Extension::operator =(const Extension& other)" );
}

Extension::Extension()
{}

size_t Extension::GetFunctionAddress( const string& name )
{
    return 0;
}

void Extension::Event( const uint& id )
{}
