#include "Core.h"
#include "App.h"

#include "Log.h"

AppData App;

AppData::AppData() :

#if defined (FOCLASSIC_CLIENT)
    Type( APP_TYPE_CLIENT ),
#elif defined (FOCLASSIC_MAPPER)
    Type( APP_TYPE_MAPPER ),
#elif defined (FOCLASSIC_SERVER)
    Type( APP_TYPE_SERVER ),
#elif defined (FOCLASSIC_SCRIPT_COMPILER)
    Type( APP_TYPE_COMPILER ),
#else
    Type( APP_TYPE_UNKNOWN ),
#endif

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
# if defined (FO_D3D)
    Render( APP_RENDER_DX ),
# else
    Render( APP_RENDER_GL ),
# endif
#else
    Render( APP_RENDER_NONE ),
#endif

#if defined (FO_LINUX)
    OS( APP_OS_LINUX ),
#elif defined (FO_WINDOWS)
    OS( APP_OS_WINDOWS ),
#else
    OS( APP_OS_UNKNOWN ),
#endif

#if defined (FO_X86)
    Bits( 32 ),
#elif defined (FO_X64)
    Bits( 64 ),
#endif

    WriteLog( &::WriteLog ), WriteLogF( &::WriteLogF ), WriteLogX( &::WriteLogX )
{}
