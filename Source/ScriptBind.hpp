#ifndef __SCRIPT_BIND__
#define __SCRIPT_BIND__

#include <angelscript.h>

#include "Types.h"

#define SCRIPT_BIND_CLIENT    (1)
#define SCRIPT_BIND_MAPPER    (2)
#define SCRIPT_BIND_SERVER    (3)

#if !defined (FOCLASSIC_SCRIPT_COMPILER)
namespace Script
{
    bool RegisterGlobalFunction( asIScriptEngine* engine, string declaration, const asSFuncPtr& function, asDWORD callConv, string name );
    bool RegisterObjectMethod( asIScriptEngine* engine, string object, string method, const asSFuncPtr& function, asDWORD callConv, string name );
    bool RegisterAll( asIScriptEngine* engine, uchar bind );
}
#endif

#if defined (FOCLASSIC_SCRIPT_COMPILER) || defined (FOCLASSIC_SERVER)
namespace ScriptDummy
{
    bool RegisterGlobalFunction( asIScriptEngine* engine, string declaration, const asSFuncPtr& function, asDWORD callConv, string name );
    bool RegisterObjectMethod( asIScriptEngine* engine, string object, string method, const asSFuncPtr& function, asDWORD callConv, string name );
    bool RegisterAll( asIScriptEngine* engine, uchar bind );
}
#endif

#endif // __SCRIPT_BIND__ //
