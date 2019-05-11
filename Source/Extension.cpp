#include "Core.h"

#include "Extension.h"
#include "Log.h"
#include "Script.h"

using namespace std;

bool                              Extension::Initialized = false;
unordered_map<string, Extension*> Extension::Map;

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
}

void Extension::Finish()
{
    if( !Initialized )
        return;

    #if defined (FOCLASSIC_CLIENT) && FOCLASSIC_EXTENSIONS_CLIENT > 0
    FinishExtensionsClient();
    #elif defined (FOCLASSIC_MAPPER) && FOCLASSIC_EXTENSIONS_MAPPER > 0
    FinishExtensionsMapper();
    #elif defined (FOCLASSIC_SERVER) && FOCLASSIC_EXTENSIONS_SERVER > 0
    FinishExtensionsServer();
    #endif

    Map.clear();
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
