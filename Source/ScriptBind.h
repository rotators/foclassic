#ifndef __SCRIPT_BIND__
#define __SCRIPT_BIND__

#include <angelscript.h>

#include "Types.h"

namespace Script
{
    #if !defined (FOCLASSIC_SCRIPT_COMPILER)
    bool SetEngineProperty( asIScriptEngine* engine, asEEngineProp property, asPWORD value );

    bool RegisterGlobalProperty( asIScriptEngine* engine, const std::string& declaration, void* pointer );
    bool RegisterGlobalFunction( asIScriptEngine* engine, const std::string& declaration, const asSFuncPtr& function, asDWORD callConv, void* auxiliary = 0 );

    bool RegisterObjectType( asIScriptEngine* engine, const std::string& object, int byteSize, asDWORD flags );
    bool RegisterObjectBehaviour( asIScriptEngine* engine, const std::string& object, asEBehaviours behaviour, const std::string& declaration, const asSFuncPtr& function, asDWORD callConv, void* objForThisCall = 0 );
    bool RegisterObjectProperty( asIScriptEngine* engine, const std::string& object, const std::string& declaration, int byteOffset );
    bool RegisterObjectMethod( asIScriptEngine* engine, const std::string& object, const std::string& declaration, const asSFuncPtr& function, asDWORD callConv );

    bool RegisterAll( asIScriptEngine* engine, const uint8& app );
    #endif

    #if defined (FOCLASSIC_SCRIPT_COMPILER) || defined (FOCLASSIC_SERVER)
    namespace BindDummy
    {
        bool SetEngineProperty( asIScriptEngine* engine, asEEngineProp property, asPWORD value );

        bool RegisterGlobalProperty( asIScriptEngine* engine, const std::string& declaration, void* pointer );
        bool RegisterGlobalFunction( asIScriptEngine* engine, const std::string& declaration, const asSFuncPtr& function, asDWORD callConv, void* auxiliary = 0 );

        bool RegisterObjectType( asIScriptEngine* engine, const std::string& object, int byteSize, asDWORD flags );
        bool RegisterObjectBehaviour( asIScriptEngine* engine, const std::string& object, asEBehaviours behaviour, const std::string& declaration, const asSFuncPtr& function, asDWORD callConv, void* objForThisCall = 0 );
        bool RegisterObjectProperty( asIScriptEngine* engine, const std::string& object, const std::string& declaration, int byteOffset );
        bool RegisterObjectMethod( asIScriptEngine* engine, const std::string& object, const std::string& declaration, const asSFuncPtr& function, asDWORD callConv );

        bool RegisterAll( asIScriptEngine* engine, const uint8& app );
    }
    #endif
}

#endif // __SCRIPT_BIND__ //
