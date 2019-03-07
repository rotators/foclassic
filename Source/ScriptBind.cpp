#include "Core.h"

#include <angelscript.h>

#include "App.h"
#include "DynamicLibrary.h"
#include "GameOptions.h"
#include "Log.h"
#include "Random.h"
#include "ScriptBind.h"
#include "ScriptUtils.h"

using namespace std;

// "copy" macros related to registration
// redefined during dummy registration

#define focFUNCTION( func )                        asFUNCTION( func )
#define focMETHOD( clas, member )                  asMETHOD( clas, member )
#define focMETHODPR( clas, member, decl, ret )     asMETHODPR( clas, member, decl, ret )
#define focSIZEOF( obj )                           sizeof(obj)
#define focOFFSET( clas, member )                  asOFFSET( clas, member )

#if defined (FOCLASSIC_CLIENT)
# include "Client.h"
# define BIND_CLIENT
# define BIND_CLASS                                FOClient::SScriptFunc::
#elif defined (FOCLASSIC_MAPPER)
# include "Mapper.h"
# define BIND_MAPPER
# define BIND_CLASS                                FOMapper::SScriptFunc::
#elif defined (FOCLASSIC_SERVER)
# include "Map.h"
# include "Server.h"
# define BIND_SERVER
# define BIND_CLASS                                FOServer::SScriptFunc::
#elif defined (FOCLASSIC_SCRIPT_COMPILER)
# define BIND_DUMMY

// change the meaning of selected AS macros
// allows dummy registration without preparing dummy structures

static const asSFuncPtr DummyFunction = asFUNCTION( 0 );
# undef focFUNCTION
# undef focMETHOD
# undef focMETHODPR
# define focFUNCTION( func )                       DummyFunction
# define focMETHOD( clas, method )                 DummyFunction
# define focMETHODPR( clas, method, decl, ret )    DummyFunction

struct DummyClass
{
    uint DummyMember;
};
static const size_t DummyClassSize = sizeof(DummyClass);
static DummyClass   DummyObject;
# undef focSIZEOF
# undef focOFFSET
# define focSIZEOF( obj )                          DummyClassSize
# define focOFFSET( clas, method )                 ++ DummyObject.DummyMember
#endif

// keep dummy registration in own namespace
// allows real and dummy registration to co-exists within same application using "same" functions

#if !defined (BIND_DUMMY)
# define BIND_NAMESPACE_BEGIN                      namespace Script
# define BIND_NAMESPACE_END
namespace ScriptBind = Script;
#else
# define BIND_NAMESPACE_BEGIN                      namespace Script { namespace BindDummy
# define BIND_NAMESPACE_END                        }
namespace ScriptBind = Script::BindDummy;
#endif

BIND_NAMESPACE_BEGIN
{
    bool BindSuccess = true;

    bool BindChecks( asIScriptEngine* engine )
    {
        if( BindSuccess && !engine )
        {
            BindSuccess = false;
            WriteLog( "BIND ERROR : no engine\n" );
        }

        return BindSuccess;
    }
}
BIND_NAMESPACE_END

bool ScriptBind::SetEngineProperty( asIScriptEngine* engine, asEEngineProp property, asPWORD value )
{
    if( !BindChecks( engine ) )
        return false;

    int result = engine->SetEngineProperty( property, value );
    if( result == asSUCCESS )
    {
        // TODO ScriptBind postprocess
    }
    else
    {
        WriteLog( "BIND ERROR : %s : %u : %s\n", _FUNC_, property, Script::GetASReturnCode( result ) );
        BindSuccess = false;
    }

    return BindSuccess;
}

bool ScriptBind::RegisterGlobalProperty( asIScriptEngine* engine, const string& declaration, void* pointer )
{
    if( !BindChecks( engine ) )
        return false;

    int result = engine->RegisterGlobalProperty( declaration.c_str(), pointer );
    if( result == asSUCCESS )
    {
        // TODO ScriptBind postprocess
    }
    else
    {
        WriteLog( "BIND ERROR : %s : %s : %s\n", _FUNC_, declaration.c_str(), Script::GetASReturnCode( result ) );
        BindSuccess = false;
    }

    return BindSuccess;
}

/* asCALL_CDECL */
bool ScriptBind::RegisterGlobalFunction( asIScriptEngine* engine, const string& declaration, const asSFuncPtr& function, asDWORD callConv, void* auxiliary /* = 0 */ )
{
    if( !BindChecks( engine ) )
        return false;

    int result = engine->RegisterGlobalFunction( declaration.c_str(), function, callConv, auxiliary );
    if( result ) // function id
    {
        // TODO ScriptBind postprocess
    }
    else
    {
        WriteLog( "BIND ERROR : %s : %s : %s\n", _FUNC_, declaration.c_str(), Script::GetASReturnCode( result ) );
        BindSuccess = false;
    }

    return BindSuccess;
}

bool ScriptBind::RegisterObjectType( asIScriptEngine* engine, const string& object, int byteSize, asDWORD flags )
{
    if( !BindChecks( engine ) )
        return false;

    int result = engine->RegisterObjectType( object.c_str(), byteSize, flags );
    if( result ) // type id
    {
        // TODO ScriptBind postprocess
    }
    else
    {
        WriteLog( "BIND ERROR : %s : %s : %s\n", _FUNC_, object.c_str(), Script::GetASReturnCode( result ) );
        BindSuccess = false;
    }

    return BindSuccess;
}

bool ScriptBind::RegisterObjectBehaviour( asIScriptEngine* engine, const string& object, asEBehaviours behaviour, const string& declaration, const asSFuncPtr& function, asDWORD callConv, void* objForThisCall /* = 0 */  )
{
    if( !BindChecks( engine ) )
        return false;

    int result = engine->RegisterObjectBehaviour( object.c_str(), behaviour, declaration.c_str(), function, callConv, objForThisCall );
    if( result ) // function id
    {
        // TODO ScriptBind postprocess
    }
    else
    {
        WriteLog( "BIND ERROR : %s : %s %s : %s\n", _FUNC_, object.c_str(), declaration.c_str(), Script::GetASReturnCode( result ) );
        BindSuccess = false;
    }

    return BindSuccess;
}

bool ScriptBind::RegisterObjectProperty( asIScriptEngine* engine, const string& object, const string& declaration, int byteOffset )
{
    if( !BindChecks( engine ) )
        return false;

    int result = engine->RegisterObjectProperty( object.c_str(), declaration.c_str(), byteOffset );
    if( result == asSUCCESS )
    {
        // TODO ScriptBind postprocess
    }
    else
    {
        WriteLog( "BIND ERROR : %s : %s %s : %s\n", _FUNC_, object.c_str(), declaration.c_str(), Script::GetASReturnCode( result ) );
        BindSuccess = false;
    }

    return BindSuccess;
}

/* asCALL_CDECL_OBJFIRST */
bool ScriptBind::RegisterObjectMethod( asIScriptEngine* engine, const string& object, const string& declaration, const asSFuncPtr& function, asDWORD callConv )
{
    if( !BindChecks( engine ) )
        return false;

    int result = engine->RegisterObjectMethod( object.c_str(), declaration.c_str(), function, callConv );
    if( result ) // function id
    {
        // TODO ScriptBind postprocess
    }
    else
    {
        WriteLog( "BIND ERROR : %s : %s %s : %s\n", _FUNC_, object.c_str(), declaration.c_str(), Script::GetASReturnCode( result ) );
        BindSuccess = false;
    }

    return BindSuccess;
}

bool ScriptBind::RegisterAll( asIScriptEngine* engine, const uint8& app )
{
    if( !BindChecks( engine ) )
        return false;

    //
    // - both checks must pass before using any registration function:
    //   #ifdef - filters real registration
    //   if()   - filters dummy registration
    //
    // - for real registration, 'app' argument is always related to current application
    //   for dummy registration, it defines which application is being simulated
    //
    // - keep each type inside its own #ifdef+if() block
    //
    // - registration order
    //   forward registration of all types, alphabetically
    //   global properties, available for all applications
    //   global properties, available for selected applications
    //   global functions, available for all applications (including ASCompiler)
    //   global functions, available for all applications (excluding ASCompiler)
    //   global functions, available for selected applications
    //   types, alphabetically
    //     global functions related to type
    //     type properties
    //     type methods
    //
    // - registration macros
    //   if type/function/property/method will be available in ASCompiler: use asFUNCTION, asOFFSET, etc.
    //   in other case: use focFUNCTION, focOFFSET, etc.
    //

    //
    // Configuration
    //

    SetEngineProperty( engine, asEP_ALLOW_UNSAFE_REFERENCES, true );
    SetEngineProperty( engine, asEP_OPTIMIZE_BYTECODE, true );
    SetEngineProperty( engine, asEP_USE_CHARACTER_LITERALS, true );
    SetEngineProperty( engine, asEP_SCRIPT_SCANNER, 1 );    // 0 = ASCII, 1 = UTF8
    SetEngineProperty( engine, asEP_AUTO_GARBAGE_COLLECT, false );
    SetEngineProperty( engine, asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT, true  );
    SetEngineProperty( engine, asEP_COMPILER_WARNINGS, 1 ); // 0 = dismiss, 1 = emit, 2 = treat as error

    #if ANGELSCRIPT_VERSION >= 22900
    # pragma TODO("asCALL_THISCALL_OBJFIRST")
    #endif

    #if ANGELSCRIPT_VERSION >= 23000
    # pragma TODO("asEP_DISALLOW_EMPTY_LIST_ELEMENTS")
    #endif

    #if ANGELSCRIPT_VERSION >= 23200
    # pragma TODO("asEP_MAX_NESTED_CALLS")
    #endif

    #if defined (BIND_DUMMY)
    DummyObject.DummyMember = 0;
    // AS docs recommends to skip global vars initialization after script is built
    SetEngineProperty( engine, asEP_INIT_GLOBAL_VARS_AFTER_BUILD, false );
    #endif

    //
    // Forward registration
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "CraftItem", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "CraftItem", asBEHAVE_ADDREF, "void f()", focMETHOD( CraftItem, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "CraftItem", asBEHAVE_RELEASE, "void f()", focMETHOD( CraftItem, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "Critter", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "Critter", asBEHAVE_ADDREF, "void f()", focMETHOD( Critter, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "Critter", asBEHAVE_RELEASE, "void f()", focMETHOD( Critter, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( app == APP_TYPE_CLIENT )
    {
        RegisterObjectType( engine, "CritterCl", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "CritterCl", asBEHAVE_ADDREF, "void f()", focMETHOD( CritterCl, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "CritterCl", asBEHAVE_RELEASE, "void f()", focMETHOD( CritterCl, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "DataRef", 0, asOBJ_REF | asOBJ_NOHANDLE );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "DataVal", 0, asOBJ_REF | asOBJ_NOHANDLE );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "GameVar", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "GameVar", asBEHAVE_ADDREF, "void f()", focMETHOD( GameVar, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "GameVar", asBEHAVE_RELEASE, "void f()", focMETHOD( GameVar, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "Item", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "Item", asBEHAVE_ADDREF, "void f()", focMETHOD( Item, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "Item", asBEHAVE_RELEASE, "void f()", focMETHOD( Item, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( app == APP_TYPE_CLIENT )
    {
        RegisterObjectType( engine, "ItemCl", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "ItemCl", asBEHAVE_ADDREF, "void f()", focMETHOD( Item, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "ItemCl", asBEHAVE_RELEASE, "void f()", focMETHOD( Item, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "Location", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "Location", asBEHAVE_ADDREF, "void f()", focMETHOD( Location, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "Location", asBEHAVE_RELEASE, "void f()", focMETHOD( Location, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "Map", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "Map", asBEHAVE_ADDREF, "void f()", focMETHOD( Map, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "Map", asBEHAVE_RELEASE, "void f()", focMETHOD( Map, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( app == APP_TYPE_MAPPER )
    {
        RegisterObjectType( engine, "MapperMap", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "MapperMap", asBEHAVE_ADDREF, "void f()", focMETHOD( ProtoMap, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "MapperMap", asBEHAVE_RELEASE, "void f()", focMETHOD( ProtoMap, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( app == APP_TYPE_MAPPER )
    {
        RegisterObjectType( engine, "MapperObject", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "MapperObject", asBEHAVE_ADDREF, "void f()", focMETHOD( MapObject, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "MapperObject", asBEHAVE_RELEASE, "void f()", focMETHOD( MapObject, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "NpcPlane", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "NpcPlane", asBEHAVE_ADDREF, "void f()", focMETHOD( AIDataPlane, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "NpcPlane", asBEHAVE_RELEASE, "void f()", focMETHOD( AIDataPlane, Release ), asCALL_THISCALL );
    }
    #endif

    RegisterObjectType( engine, "ProtoItem", 0, asOBJ_REF );
    RegisterObjectBehaviour( engine, "ProtoItem", asBEHAVE_ADDREF, "void f()", focMETHOD( ProtoItem, AddRef ), asCALL_THISCALL );
    RegisterObjectBehaviour( engine, "ProtoItem", asBEHAVE_RELEASE, "void f()", focMETHOD( ProtoItem, Release ), asCALL_THISCALL );

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "Scenery", 0, asOBJ_REF );
        RegisterObjectBehaviour( engine, "Scenery", asBEHAVE_ADDREF, "void f()", focMETHOD( MapObject, AddRef ), asCALL_THISCALL );
        RegisterObjectBehaviour( engine, "Scenery", asBEHAVE_RELEASE, "void f()", focMETHOD( MapObject, Release ), asCALL_THISCALL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectType( engine, "Synchronizer", focSIZEOF( SyncObject ), asOBJ_VALUE );
        RegisterObjectBehaviour( engine, "Synchronizer", asBEHAVE_CONSTRUCT, "void f()", focFUNCTION( BIND_CLASS Synchronizer_Constructor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectBehaviour( engine, "Synchronizer", asBEHAVE_DESTRUCT, "void f()", focFUNCTION( BIND_CLASS Synchronizer_Destructor ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // Global properties
    //

    RegisterGlobalProperty( engine, "bool __MapHexagonal", &GameOpt.MapHexagonal );
    RegisterGlobalProperty( engine, "int __MapHexWidth", &GameOpt.MapHexWidth );
    RegisterGlobalProperty( engine, "int __MapHexHeight", &GameOpt.MapHexHeight );
    RegisterGlobalProperty( engine, "int __MapHexLineHeight", &GameOpt.MapHexLineHeight );
    RegisterGlobalProperty( engine, "int __MapTileOffsX", &GameOpt.MapTileOffsX );
    RegisterGlobalProperty( engine, "int __MapTileOffsY", &GameOpt.MapTileOffsY );
    RegisterGlobalProperty( engine, "int __MapRoofOffsX", &GameOpt.MapRoofOffsX );
    RegisterGlobalProperty( engine, "int __MapRoofOffsY", &GameOpt.MapRoofOffsY );
    RegisterGlobalProperty( engine, "int __MapRoofSkipSize", &GameOpt.MapRoofSkipSize );
    RegisterGlobalProperty( engine, "float __MapCameraAngle", &GameOpt.MapCameraAngle );
    RegisterGlobalProperty( engine, "bool __MapSmoothPath", &GameOpt.MapSmoothPath );
    RegisterGlobalProperty( engine, "string __MapDataPrefix", &GameOpt.MapDataPrefix );

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( app == APP_TYPE_CLIENT )
    {
        RegisterGlobalFunction( engine, "bool get___ConsoleActive()", focFUNCTION( BIND_CLASS Global_get___ConsoleActive ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool get___WorldmapActive()", focFUNCTION( BIND_CLASS Global_get___WorldmapActive ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool get___WorldmapWait()", focFUNCTION( BIND_CLASS Global_get___WorldmapWait ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "float get___WorldmapZoom()", focFUNCTION( BIND_CLASS Global_get___WorldmapZoom ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int get___WorldmapOffsetX()", focFUNCTION( BIND_CLASS Global_get___WorldmapOffsetX ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int get___WorldmapOffsetY()", focFUNCTION( BIND_CLASS Global_get___WorldmapOffsetY ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int get___WorldmapGroupCurX()", focFUNCTION( BIND_CLASS Global_get___WorldmapGroupCurX ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int get___WorldmapGroupCurY()", focFUNCTION( BIND_CLASS Global_get___WorldmapGroupCurY ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int get___WorldmapGroupToX()", focFUNCTION( BIND_CLASS Global_get___WorldmapGroupToX ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int get___WorldmapGroupToY()", focFUNCTION( BIND_CLASS Global_get___WorldmapGroupToY ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "float get___WorldmapGroupSpeed()", focFUNCTION( BIND_CLASS Global_get___WorldmapGroupSpeed ), asCALL_CDECL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( app == APP_TYPE_MAPPER )
    {
        RegisterGlobalProperty( engine, "string __ClientPath", &GameOpt.ClientPath );
        RegisterGlobalProperty( engine, "string __ServerPath", &GameOpt.ServerPath );
        RegisterGlobalProperty( engine, "bool __ShowCorners", &GameOpt.ShowCorners );
        RegisterGlobalProperty( engine, "bool __ShowSpriteCuts", &GameOpt.ShowSpriteCuts );
        RegisterGlobalProperty( engine, "bool __ShowDrawOrder", &GameOpt.ShowDrawOrder );
        RegisterGlobalProperty( engine, "bool __SplitTilesCollection", &GameOpt.SplitTilesCollection );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_MAPPER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_MAPPER )
    {
        RegisterGlobalProperty( engine, "bool __Quit", &GameOpt.Quit );
        RegisterGlobalProperty( engine, "bool __OpenGLRendering", &GameOpt.OpenGLRendering );
        RegisterGlobalProperty( engine, "bool __OpenGLDebug", &GameOpt.OpenGLDebug );
        RegisterGlobalProperty( engine, "bool __AssimpLogging", &GameOpt.AssimpLogging );
        RegisterGlobalProperty( engine, "int __MouseX", &GameOpt.MouseX );
        RegisterGlobalProperty( engine, "int __MouseY", &GameOpt.MouseY );
        RegisterGlobalProperty( engine, "uint8 __RoofAlpha", &GameOpt.RoofAlpha );
        RegisterGlobalProperty( engine, "uint8 __WallAlpha", &GameOptExt.WallAlpha );
        RegisterGlobalProperty( engine, "bool __HideCursor", &GameOpt.HideCursor );
        RegisterGlobalProperty( engine, "const int __ScreenWidth", &GameOpt.ScreenWidth );
        RegisterGlobalProperty( engine, "const int __ScreenHeight", &GameOpt.ScreenHeight );
        RegisterGlobalProperty( engine, "const int __MultiSampling", &GameOpt.MultiSampling );
        RegisterGlobalProperty( engine, "bool __DisableLMenu", &GameOpt.DisableLMenu );
        RegisterGlobalProperty( engine, "bool __DisableMouseEvents", &GameOpt.DisableMouseEvents );
        RegisterGlobalProperty( engine, "bool __DisableKeyboardEvents", &GameOpt.DisableKeyboardEvents );
        RegisterGlobalProperty( engine, "bool __HidePassword", &GameOpt.HidePassword );
        RegisterGlobalProperty( engine, "string __PlayerOffAppendix", &GameOpt.PlayerOffAppendix );
        RegisterGlobalProperty( engine, "uint __DamageHitDelay", &GameOpt.DamageHitDelay );
        RegisterGlobalProperty( engine, "int __CombatMessagesType", &GameOpt.CombatMessagesType );
        RegisterGlobalProperty( engine, "bool __ShowTile", &GameOpt.ShowTile );
        RegisterGlobalProperty( engine, "bool __ShowRoof", &GameOpt.ShowRoof );
        RegisterGlobalProperty( engine, "bool __ShowItem", &GameOpt.ShowItem );
        RegisterGlobalProperty( engine, "bool __ShowScen", &GameOpt.ShowScen );
        RegisterGlobalProperty( engine, "bool __ShowWall", &GameOpt.ShowWall );
        RegisterGlobalProperty( engine, "bool __ShowCrit", &GameOpt.ShowCrit );
        RegisterGlobalProperty( engine, "bool __ShowFast", &GameOpt.ShowFast );
        RegisterGlobalProperty( engine, "bool __ShowPlayerNames", &GameOpt.ShowPlayerNames );
        RegisterGlobalProperty( engine, "bool __ShowNpcNames", &GameOpt.ShowNpcNames );
        RegisterGlobalProperty( engine, "bool __ShowCritId", &GameOpt.ShowCritId );
        RegisterGlobalProperty( engine, "bool __ScrollKeybLeft", &GameOpt.ScrollKeybLeft );
        RegisterGlobalProperty( engine, "bool __ScrollKeybRight", &GameOpt.ScrollKeybRight );
        RegisterGlobalProperty( engine, "bool __ScrollKeybUp", &GameOpt.ScrollKeybUp );
        RegisterGlobalProperty( engine, "bool __ScrollKeybDown", &GameOpt.ScrollKeybDown );
        RegisterGlobalProperty( engine, "bool __ScrollMouseLeft", &GameOpt.ScrollMouseLeft );
        RegisterGlobalProperty( engine, "bool __ScrollMouseRight", &GameOpt.ScrollMouseRight );
        RegisterGlobalProperty( engine, "bool __ScrollMouseUp", &GameOpt.ScrollMouseUp );
        RegisterGlobalProperty( engine, "bool __ScrollMouseDown", &GameOpt.ScrollMouseDown );
        RegisterGlobalProperty( engine, "bool __ShowGroups", &GameOpt.ShowGroups );
        RegisterGlobalProperty( engine, "bool __HelpInfo", &GameOpt.HelpInfo );
        RegisterGlobalProperty( engine, "bool __FullScr", &GameOpt.FullScreen );
        RegisterGlobalProperty( engine, "bool __VSync", &GameOpt.VSync );
        RegisterGlobalProperty( engine, "int __FlushVal", &GameOpt.FlushVal );
        RegisterGlobalProperty( engine, "int __BaseTex", &GameOpt.BaseTexture );
        RegisterGlobalProperty( engine, "int __Light", &GameOpt.Light );
        RegisterGlobalProperty( engine, "uint __ScrollDelay", &GameOpt.ScrollDelay );
        RegisterGlobalProperty( engine, "int __ScrollStep", &GameOpt.ScrollStep );
        RegisterGlobalProperty( engine, "bool __MouseScroll", &GameOpt.MouseScroll );
        RegisterGlobalProperty( engine, "bool __ScrollCheck", &GameOpt.ScrollCheck );
        RegisterGlobalProperty( engine, "string __DataPath", &GameOpt.DataPath );
        RegisterGlobalProperty( engine, "string __Host", &GameOpt.Host );
        RegisterGlobalProperty( engine, "uint __Port", &GameOpt.Port );
        RegisterGlobalProperty( engine, "uint __ProxyType", &GameOpt.ProxyType );
        RegisterGlobalProperty( engine, "string __ProxyHost", &GameOpt.ProxyHost );
        RegisterGlobalProperty( engine, "uint __ProxyPort", &GameOpt.ProxyPort );
        RegisterGlobalProperty( engine, "string __ProxyUser", &GameOpt.ProxyUser );
        RegisterGlobalProperty( engine, "string __ProxyPass", &GameOpt.ProxyPass );
        RegisterGlobalProperty( engine, "string __Name", &GameOpt.Name );
        RegisterGlobalProperty( engine, "uint __TextDelay", &GameOpt.TextDelay );
        RegisterGlobalProperty( engine, "bool __AlwaysOnTop", &GameOpt.AlwaysOnTop );
        RegisterGlobalProperty( engine, "int __FixedFPS", &GameOpt.FixedFPS );
        RegisterGlobalProperty( engine, "uint __FPS", &GameOpt.FPS );
        RegisterGlobalProperty( engine, "uint __PingPeriod", &GameOpt.PingPeriod );
        RegisterGlobalProperty( engine, "uint __Ping", &GameOpt.Ping );
        RegisterGlobalProperty( engine, "bool __MsgboxInvert", &GameOpt.MsgboxInvert );
        RegisterGlobalProperty( engine, "uint8 __DefaultCombatMode", &GameOpt.DefaultCombatMode );
        RegisterGlobalProperty( engine, "bool __MessNotify", &GameOpt.MessNotify );
        RegisterGlobalProperty( engine, "bool __SoundNotify", &GameOpt.SoundNotify );
        RegisterGlobalProperty( engine, "int __IndicatorType", &GameOpt.IndicatorType );
        RegisterGlobalProperty( engine, "uint __DoubleClickTime", &GameOpt.DoubleClickTime );
        RegisterGlobalProperty( engine, "int __RunModMul", &GameOpt.RunModMul );
        RegisterGlobalProperty( engine, "int __RunModDiv", &GameOpt.RunModDiv );
        RegisterGlobalProperty( engine, "int __RunModAdd", &GameOpt.RunModAdd );
        RegisterGlobalProperty( engine, "uint __Animation3dSmoothTime", &GameOpt.Animation3dSmoothTime );
        RegisterGlobalProperty( engine, "uint __Animation3dFPS", &GameOpt.Animation3dFPS );
        RegisterGlobalProperty( engine, "bool __MapZooming", &GameOpt.MapZooming );
        RegisterGlobalProperty( engine, "float __SpritesZoom", &GameOpt.SpritesZoom );
        RegisterGlobalProperty( engine, "float __SpritesZoomMin", &GameOpt.SpritesZoomMin );
        RegisterGlobalProperty( engine, "float __SpritesZoomMax", &GameOpt.SpritesZoomMax );
        RegisterGlobalProperty( engine, "float __EffectValue0", &GameOpt.EffectValues[0] );
        RegisterGlobalProperty( engine, "float __EffectValue1", &GameOpt.EffectValues[1] );
        RegisterGlobalProperty( engine, "float __EffectValue2", &GameOpt.EffectValues[2] );
        RegisterGlobalProperty( engine, "float __EffectValue3", &GameOpt.EffectValues[3] );
        RegisterGlobalProperty( engine, "float __EffectValue4", &GameOpt.EffectValues[4] );
        RegisterGlobalProperty( engine, "float __EffectValue5", &GameOpt.EffectValues[5] );
        RegisterGlobalProperty( engine, "float __EffectValue6", &GameOpt.EffectValues[6] );
        RegisterGlobalProperty( engine, "float __EffectValue7", &GameOpt.EffectValues[7] );
        RegisterGlobalProperty( engine, "float __EffectValue8", &GameOpt.EffectValues[8] );
        RegisterGlobalProperty( engine, "float __EffectValue9", &GameOpt.EffectValues[9] );
        RegisterGlobalProperty( engine, "bool __AlwaysRun", &GameOpt.AlwaysRun );
        RegisterGlobalProperty( engine, "uint __AlwaysRunMoveDist", &GameOpt.AlwaysRunMoveDist );
        RegisterGlobalProperty( engine, "uint __AlwaysRunUseDist", &GameOpt.AlwaysRunUseDist );
        RegisterGlobalProperty( engine, "uint __CritterFidgetTime", &GameOpt.CritterFidgetTime );
        RegisterGlobalProperty( engine, "uint __Anim2CombatBegin", &GameOpt.Anim2CombatBegin );
        RegisterGlobalProperty( engine, "uint __Anim2CombatIdle", &GameOpt.Anim2CombatIdle );
        RegisterGlobalProperty( engine, "uint __Anim2CombatEnd", &GameOpt.Anim2CombatEnd );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_SERVER )
    {
        RegisterGlobalProperty( engine, "const uint16 __Year", &GameOpt.Year );
        RegisterGlobalProperty( engine, "const uint16 __Month", &GameOpt.Month );
        RegisterGlobalProperty( engine, "const uint16 __Day", &GameOpt.Day );
        RegisterGlobalProperty( engine, "const uint16 __Hour", &GameOpt.Hour );
        RegisterGlobalProperty( engine, "const uint16 __Minute", &GameOpt.Minute );
        RegisterGlobalProperty( engine, "const uint16 __Second", &GameOpt.Second );
        RegisterGlobalProperty( engine, "const uint16 __TimeMultiplier", &GameOpt.TimeMultiplier );
        RegisterGlobalProperty( engine, "const uint __FullSecond", &GameOpt.FullSecond );

        RegisterGlobalProperty( engine, "bool __DisableTcpNagle", &GameOpt.DisableTcpNagle );
        RegisterGlobalProperty( engine, "bool __DisableZlibCompression", &GameOpt.DisableZlibCompression );
        RegisterGlobalProperty( engine, "uint __FloodSize", &GameOpt.FloodSize );
        RegisterGlobalProperty( engine, "bool __NoAnswerShuffle", &GameOpt.NoAnswerShuffle );
        RegisterGlobalProperty( engine, "bool __DialogDemandRecheck", &GameOpt.DialogDemandRecheck );
        RegisterGlobalProperty( engine, "uint __FixBoyDefaultExperience", &GameOpt.FixBoyDefaultExperience );
        RegisterGlobalProperty( engine, "uint __SneakDivider", &GameOpt.SneakDivider );
        RegisterGlobalProperty( engine, "uint __LevelCap", &GameOpt.LevelCap );
        RegisterGlobalProperty( engine, "bool __LevelCapAddExperience", &GameOpt.LevelCapAddExperience );
        RegisterGlobalProperty( engine, "uint __LookNormal", &GameOpt.LookNormal );
        RegisterGlobalProperty( engine, "uint __LookMinimum", &GameOpt.LookMinimum );
        RegisterGlobalProperty( engine, "uint __WorldmapMaxGroupCount", &GameOpt.WorldmapMaxGroupCount );
        RegisterGlobalProperty( engine, "uint __CritterIdleTick", &GameOpt.CritterIdleTick );
        RegisterGlobalProperty( engine, "uint __TurnBasedTick", &GameOpt.TurnBasedTick );
        RegisterGlobalProperty( engine, "int __DeadHitPoints", &GameOpt.DeadHitPoints );
        RegisterGlobalProperty( engine, "uint __Breaktime", &GameOpt.Breaktime );
        RegisterGlobalProperty( engine, "uint __TimeoutTransfer", &GameOpt.TimeoutTransfer );
        RegisterGlobalProperty( engine, "uint __TimeoutBattle", &GameOpt.TimeoutBattle );
        RegisterGlobalProperty( engine, "uint __ApRegeneration", &GameOpt.ApRegeneration );
        RegisterGlobalProperty( engine, "uint __RtApCostCritterWalk", &GameOpt.RtApCostCritterWalk );
        RegisterGlobalProperty( engine, "uint __RtApCostCritterRun", &GameOpt.RtApCostCritterRun );
        RegisterGlobalProperty( engine, "uint __RtApCostMoveItemContainer", &GameOpt.RtApCostMoveItemContainer );
        RegisterGlobalProperty( engine, "uint __RtApCostMoveItemInventory", &GameOpt.RtApCostMoveItemInventory );
        RegisterGlobalProperty( engine, "uint __RtApCostPickItem", &GameOpt.RtApCostPickItem );
        RegisterGlobalProperty( engine, "uint __RtApCostDropItem", &GameOpt.RtApCostDropItem );
        RegisterGlobalProperty( engine, "uint __RtApCostReloadWeapon", &GameOpt.RtApCostReloadWeapon );
        RegisterGlobalProperty( engine, "uint __RtApCostPickCritter", &GameOpt.RtApCostPickCritter );
        RegisterGlobalProperty( engine, "uint __RtApCostUseItem", &GameOpt.RtApCostUseItem );
        RegisterGlobalProperty( engine, "uint __RtApCostUseSkill", &GameOpt.RtApCostUseSkill );
        RegisterGlobalProperty( engine, "bool __RtAlwaysRun", &GameOpt.RtAlwaysRun );
        RegisterGlobalProperty( engine, "uint __TbApCostCritterMove", &GameOpt.TbApCostCritterMove );
        RegisterGlobalProperty( engine, "uint __TbApCostMoveItemContainer", &GameOpt.TbApCostMoveItemContainer );
        RegisterGlobalProperty( engine, "uint __TbApCostMoveItemInventory", &GameOpt.TbApCostMoveItemInventory );
        RegisterGlobalProperty( engine, "uint __TbApCostPickItem", &GameOpt.TbApCostPickItem );
        RegisterGlobalProperty( engine, "uint __TbApCostDropItem", &GameOpt.TbApCostDropItem );
        RegisterGlobalProperty( engine, "uint __TbApCostReloadWeapon", &GameOpt.TbApCostReloadWeapon );
        RegisterGlobalProperty( engine, "uint __TbApCostPickCritter", &GameOpt.TbApCostPickCritter );
        RegisterGlobalProperty( engine, "uint __TbApCostUseItem", &GameOpt.TbApCostUseItem );
        RegisterGlobalProperty( engine, "uint __TbApCostUseSkill", &GameOpt.TbApCostUseSkill );
        RegisterGlobalProperty( engine, "uint __ApCostAimEyes", &GameOpt.ApCostAimEyes );
        RegisterGlobalProperty( engine, "uint __ApCostAimHead", &GameOpt.ApCostAimHead );
        RegisterGlobalProperty( engine, "uint __ApCostAimGroin", &GameOpt.ApCostAimGroin );
        RegisterGlobalProperty( engine, "uint __ApCostAimTorso", &GameOpt.ApCostAimTorso );
        RegisterGlobalProperty( engine, "uint __ApCostAimArms", &GameOpt.ApCostAimArms );
        RegisterGlobalProperty( engine, "uint __ApCostAimLegs", &GameOpt.ApCostAimLegs );
        RegisterGlobalProperty( engine, "bool __TbAlwaysRun", &GameOpt.TbAlwaysRun );
        RegisterGlobalProperty( engine, "bool __RunOnCombat", &GameOpt.RunOnCombat );
        RegisterGlobalProperty( engine, "bool __RunOnTransfer", &GameOpt.RunOnTransfer );
        RegisterGlobalProperty( engine, "uint __GlobalMapWidth", &GameOpt.GlobalMapWidth );
        RegisterGlobalProperty( engine, "uint __GlobalMapHeight", &GameOpt.GlobalMapHeight );
        RegisterGlobalProperty( engine, "uint __GlobalMapZoneLength", &GameOpt.GlobalMapZoneLength );
        RegisterGlobalProperty( engine, "uint __GlobalMapMoveTime", &GameOpt.GlobalMapMoveTime );
        RegisterGlobalProperty( engine, "uint __BagRefreshTime", &GameOpt.BagRefreshTime );
        RegisterGlobalProperty( engine, "uint __AttackAnimationsMinDist", &GameOpt.AttackAnimationsMinDist );
        RegisterGlobalProperty( engine, "uint __WisperDist", &GameOpt.WhisperDist );
        RegisterGlobalProperty( engine, "uint __ShoutDist", &GameOpt.ShoutDist );
        RegisterGlobalProperty( engine, "int __LookChecks", &GameOpt.LookChecks );
        RegisterGlobalProperty( engine, "uint __LookDir0", &GameOpt.LookDir[0] );
        RegisterGlobalProperty( engine, "uint __LookDir1", &GameOpt.LookDir[1] );
        RegisterGlobalProperty( engine, "uint __LookDir2", &GameOpt.LookDir[2] );
        RegisterGlobalProperty( engine, "uint __LookDir3", &GameOpt.LookDir[3] );
        RegisterGlobalProperty( engine, "uint __LookDir4", &GameOpt.LookDir[4] );
        RegisterGlobalProperty( engine, "uint __LookSneakDir0", &GameOpt.LookSneakDir[0] );
        RegisterGlobalProperty( engine, "uint __LookSneakDir1", &GameOpt.LookSneakDir[1] );
        RegisterGlobalProperty( engine, "uint __LookSneakDir2", &GameOpt.LookSneakDir[2] );
        RegisterGlobalProperty( engine, "uint __LookSneakDir3", &GameOpt.LookSneakDir[3] );
        RegisterGlobalProperty( engine, "uint __LookSneakDir4", &GameOpt.LookSneakDir[4] );
        RegisterGlobalProperty( engine, "uint __LookWeight", &GameOpt.LookWeight );
        RegisterGlobalProperty( engine, "bool __CustomItemCost", &GameOpt.CustomItemCost );
        RegisterGlobalProperty( engine, "uint __RegistrationTimeout", &GameOpt.RegistrationTimeout );
        RegisterGlobalProperty( engine, "uint __AccountPlayTime", &GameOpt.AccountPlayTime );
        RegisterGlobalProperty( engine, "bool __LoggingVars", &GameOpt.LoggingVars );
        RegisterGlobalProperty( engine, "uint __ScriptRunSuspendTimeout", &GameOpt.ScriptRunSuspendTimeout );
        RegisterGlobalProperty( engine, "uint __ScriptRunMessageTimeout", &GameOpt.ScriptRunMessageTimeout );
        RegisterGlobalProperty( engine, "uint __TalkDistance", &GameOpt.TalkDistance );
        RegisterGlobalProperty( engine, "uint __NpcMaxTalkers", &GameOpt.NpcMaxTalkers );
        RegisterGlobalProperty( engine, "uint __MinNameLength", &GameOpt.MinNameLength );
        RegisterGlobalProperty( engine, "uint __MaxNameLength", &GameOpt.MaxNameLength );
        RegisterGlobalProperty( engine, "uint __DlgTalkMinTime", &GameOpt.DlgTalkMinTime );
        RegisterGlobalProperty( engine, "uint __DlgBarterMinTime", &GameOpt.DlgBarterMinTime );
        RegisterGlobalProperty( engine, "uint __MinimumOfflineTime", &GameOpt.MinimumOfflineTime );

        RegisterGlobalProperty( engine, "int __StartSpecialPoints", &GameOpt.StartSpecialPoints );
        RegisterGlobalProperty( engine, "int __StartTagSkillPoints", &GameOpt.StartTagSkillPoints );
        RegisterGlobalProperty( engine, "int __SkillMaxValue", &GameOpt.SkillMaxValue );
        RegisterGlobalProperty( engine, "int __SkillModAdd2", &GameOpt.SkillModAdd2 );
        RegisterGlobalProperty( engine, "int __SkillModAdd3", &GameOpt.SkillModAdd3 );
        RegisterGlobalProperty( engine, "int __SkillModAdd4", &GameOpt.SkillModAdd4 );
        RegisterGlobalProperty( engine, "int __SkillModAdd5", &GameOpt.SkillModAdd5 );
        RegisterGlobalProperty( engine, "int __SkillModAdd6", &GameOpt.SkillModAdd6 );

        RegisterGlobalProperty( engine, "bool __AbsoluteOffsets", &GameOpt.AbsoluteOffsets );
        RegisterGlobalProperty( engine, "uint __SkillBegin", &GameOpt.SkillBegin );
        RegisterGlobalProperty( engine, "uint __SkillEnd", &GameOpt.SkillEnd );
        RegisterGlobalProperty( engine, "uint __TimeoutBegin", &GameOpt.TimeoutBegin );
        RegisterGlobalProperty( engine, "uint __TimeoutEnd", &GameOpt.TimeoutEnd );
        RegisterGlobalProperty( engine, "uint __KillBegin", &GameOpt.KillBegin );
        RegisterGlobalProperty( engine, "uint __KillEnd", &GameOpt.KillEnd );
        RegisterGlobalProperty( engine, "uint __PerkBegin", &GameOpt.PerkBegin );
        RegisterGlobalProperty( engine, "uint __PerkEnd", &GameOpt.PerkEnd );
        RegisterGlobalProperty( engine, "uint __AddictionBegin", &GameOpt.AddictionBegin );
        RegisterGlobalProperty( engine, "uint __AddictionEnd", &GameOpt.AddictionEnd );
        RegisterGlobalProperty( engine, "uint __KarmaBegin", &GameOpt.KarmaBegin );
        RegisterGlobalProperty( engine, "uint __KarmaEnd", &GameOpt.KarmaEnd );
        RegisterGlobalProperty( engine, "uint __DamageBegin", &GameOpt.DamageBegin );
        RegisterGlobalProperty( engine, "uint __DamageEnd", &GameOpt.DamageEnd );
        RegisterGlobalProperty( engine, "uint __TraitBegin", &GameOpt.TraitBegin );
        RegisterGlobalProperty( engine, "uint __TraitEnd", &GameOpt.TraitEnd );
        RegisterGlobalProperty( engine, "uint __ReputationBegin", &GameOpt.ReputationBegin );
        RegisterGlobalProperty( engine, "uint __ReputationEnd", &GameOpt.ReputationEnd );

        RegisterGlobalProperty( engine, "int __ReputationLoved", &GameOpt.ReputationLoved );
        RegisterGlobalProperty( engine, "int __ReputationLiked", &GameOpt.ReputationLiked );
        RegisterGlobalProperty( engine, "int __ReputationAccepted", &GameOpt.ReputationAccepted );
        RegisterGlobalProperty( engine, "int __ReputationNeutral", &GameOpt.ReputationNeutral );
        RegisterGlobalProperty( engine, "int __ReputationAntipathy", &GameOpt.ReputationAntipathy );
        RegisterGlobalProperty( engine, "int __ReputationHated", &GameOpt.ReputationHated );
    }
    #endif

    //
    // Global functions
    //

    RegisterGlobalFunction( engine, "int Random(int minimum, int maximum)", asFUNCTION( Random ), asCALL_CDECL );

    RegisterGlobalFunction( engine, "void Log(string& text)", focFUNCTION( BIND_CLASS Global_Log ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "string@ GetLastError()", focFUNCTION( BIND_CLASS Global_GetLastError ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "ProtoItem@+ GetProtoItem(uint16 protoId)", focFUNCTION( BIND_CLASS Global_GetProtoItem ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "bool StrToInt(string@+ text, int& result)", focFUNCTION( BIND_CLASS Global_StrToInt ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "bool StrToFloat(string@+ text, float& result)", focFUNCTION( BIND_CLASS Global_StrToFloat ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "uint GetDistantion(uint16 hexX1, uint16 hexY1, uint16 hexX2, uint16 hexY2)", focFUNCTION( BIND_CLASS Global_GetDistantion ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "uint8 GetDirection(uint16 fromHexX, uint16 fromHexY, uint16 toHexX, uint16 toHexY)", focFUNCTION( BIND_CLASS Global_GetDirection ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "uint8 GetOffsetDir(uint16 fromHexX, uint16 fromHexY, uint16 toHexX, uint16 toHexY, float offset)", focFUNCTION( BIND_CLASS Global_GetOffsetDir ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "uint GetTick()", focFUNCTION( BIND_CLASS Global_GetTick ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "uint GetAngelScriptProperty(int property)", focFUNCTION( BIND_CLASS Global_GetAngelScriptProperty ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "void SetAngelScriptProperty(int property, uint value)", focFUNCTION( BIND_CLASS Global_SetAngelScriptProperty ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "uint GetStrHash(string@+ str)", focFUNCTION( BIND_CLASS Global_GetStrHash ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "bool LoadDataFile(string& dataFileName)", focFUNCTION( BIND_CLASS Global_LoadDataFile ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "int GetConstantValue(int constCollection, string@+ name)", focFUNCTION( BIND_CLASS Global_GetConstantValue ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "string@ GetConstantName(int constCollection, int value)", focFUNCTION( BIND_CLASS Global_GetConstantName ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "void AddConstant(int constCollection, string@+ name, int value)", focFUNCTION( BIND_CLASS Global_AddConstant ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "bool LoadConstants(int constCollection, string@+ fileName, int pathType)", focFUNCTION( BIND_CLASS Global_LoadConstants ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "void AllowSlot(uint8 index, string& slotName)", focFUNCTION( BIND_CLASS Global_AllowSlot ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "uint DecodeUTF8(const string& text, uint& length)", focFUNCTION( BIND_CLASS Global_DecodeUTF8 ), asCALL_CDECL );
    RegisterGlobalFunction( engine, "string@ EncodeUTF8(uint ucs)", focFUNCTION( BIND_CLASS Global_EncodeUTF8 ), asCALL_CDECL );

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( app == APP_TYPE_CLIENT )
    {
        RegisterGlobalFunction( engine, "uint GetPathLength(CritterCl& cr, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthCr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void FlushScreen(uint fromColor, uint toColor, uint timeMs)", focFUNCTION( BIND_CLASS Global_FlushScreen ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void QuakeScreen(uint noise, uint timeMs)", focFUNCTION( BIND_CLASS Global_QuakeScreen ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool PlaySound(string& soundName)", focFUNCTION( BIND_CLASS Global_PlaySound ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool PlaySound(uint8 soundType, uint8 soundTypeExt, uint8 soundId, uint8 soundIdExt)", focFUNCTION( BIND_CLASS Global_PlaySoundType ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool PlayMusic(string& musicName, uint pos, uint repeat)", focFUNCTION( BIND_CLASS Global_PlayMusic ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void PlayVideo(string& videoName, bool canStop)", focFUNCTION( BIND_CLASS Global_PlayVideo ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsTurnBased()", focFUNCTION( BIND_CLASS Global_IsTurnBased ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetTurnBasedTime()", focFUNCTION( BIND_CLASS Global_GetTurnBasedTime ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint16 GetCurrentMapPid()", focFUNCTION( BIND_CLASS Global_GetCurrentMapPid ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMessageFilters(int[]@+ filters)", focFUNCTION( BIND_CLASS Global_GetMessageFilters ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetMessageFilters(int[]@+ filters)", focFUNCTION( BIND_CLASS Global_SetMessageFilters ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void Message(string& text, int type)", focFUNCTION( BIND_CLASS Global_MessageType ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void Message(int textMsg, uint strNum, int type)", focFUNCTION( BIND_CLASS Global_MessageMsgType ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetMsgStr(int textMsg, uint strNum, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMsgStrSkip ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMsgStrNumUpper(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumUpper ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMsgStrNumLower(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumLower ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMsgStrCount(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrCount ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_IsMsgStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ ReplaceText(const string& text, const string& replace, const string& str)", focFUNCTION( BIND_CLASS Global_ReplaceTextStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ ReplaceText(const string& text, const string& replace, int i)", focFUNCTION( BIND_CLASS Global_ReplaceTextInt ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ FormatTags(const string& text, const string@+ lexems)", focFUNCTION( BIND_CLASS Global_FormatTags ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetSomeValue(int var)", focFUNCTION( BIND_CLASS Global_GetSomeValue ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveScreen(uint16 hexX, uint16 hexY, uint speed)", focFUNCTION( BIND_CLASS Global_MoveScreen ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void LockScreenScroll(CritterCl@+ cr)", focFUNCTION( BIND_CLASS Global_LockScreenScroll ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetFog(uint16 zoneX, uint16 zoneY)", focFUNCTION( BIND_CLASS Global_GetFog ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void RefreshItemsCollection(int collection)", focFUNCTION( BIND_CLASS Global_RefreshItemsCollection ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetScroll(int scrollElement)", focFUNCTION( BIND_CLASS Global_GetScroll ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetScroll(int scrollElement, int value)", focFUNCTION( BIND_CLASS Global_SetScroll ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetDayTime(uint dayPart)", focFUNCTION( BIND_CLASS Global_GetDayTime ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b)", focFUNCTION( BIND_CLASS Global_GetDayColor ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void RunServerScript(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Global_RunServerScript ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void RunServerScriptUnsafe(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Global_RunServerScriptUnsafe ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint LoadSprite(string& name, int pathIndex)", focFUNCTION( BIND_CLASS Global_LoadSprite ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint LoadSprite(uint nameHash, uint8 dir)", focFUNCTION( BIND_CLASS Global_LoadSpriteHash ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetSpriteWidth(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteWidth ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetSpriteHeight(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteHeight ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetSpriteCount(uint sprId)", focFUNCTION( BIND_CLASS Global_GetSpriteCount ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void GetTextInfo(string& text, int w, int h, int font, int flags, int& tw, int& th, int& lines)", focFUNCTION( BIND_CLASS Global_GetTextInfo ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color)", focFUNCTION( BIND_CLASS Global_DrawSprite ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteOffs ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpriteSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteSizeOffs ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSpritePattern(uint sprId, int sprIndex, int x, int y, int w, int h, int sprWidth, int sprHeight, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpritePattern ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawText(string& text, int x, int y, int w, int h, uint color, int font, int flags)", focFUNCTION( BIND_CLASS Global_DrawText ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawPrimitive(int primitiveType, int[]& data)", focFUNCTION( BIND_CLASS Global_DrawPrimitive ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawMapSprite(uint16 hx, uint16 hy, uint16 effectPid, uint sprId, int sprIndex, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_DrawMapSprite ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawCritter2d(uint crType, uint anim1, uint anim2, uint8 dir, int l, int t, int r, int b, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter2d ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawCritter3d(uint instance, uint crType, uint anim1, uint anim2, int[]@+ layers, float[]@+ position, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter3d ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void ShowScreen(int screen, int p0, int p1, int p2)", focFUNCTION( BIND_CLASS Global_ShowScreen ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void HideScreen(int screen, int p0, int p1, int p2)", focFUNCTION( BIND_CLASS Global_HideScreen ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void GetHardcodedScreenPos(int screen, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHardcodedScreenPos ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawHardcodedScreen(int screen)", focFUNCTION( BIND_CLASS Global_DrawHardcodedScreen ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetHexPos(uint16 hx, uint16 hy, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHexPos ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetMonitorHex(int x, int y, uint16& hx, uint16& hy, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorHex ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint16 GetMapWidth()", focFUNCTION( BIND_CLASS Global_GetMapWidth ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint16 GetMapHeight()", focFUNCTION( BIND_CLASS Global_GetMapHeight ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetCurrentCursor()", focFUNCTION( BIND_CLASS Global_GetCurrentCursor ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetLastCursor()", focFUNCTION( BIND_CLASS Global_GetLastCursor ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void ChangeCursor(int cursor)", focFUNCTION( BIND_CLASS Global_ChangeCursor ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps)", focFUNCTION( BIND_CLASS Global_MoveHexByDir ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool AppendIfaceIni(string& iniName)", focFUNCTION( BIND_CLASS Global_AppendIfaceIni ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetIfaceIniStr(string& key)", focFUNCTION( BIND_CLASS Global_GetIfaceIniStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool Load3dFile(string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_Load3dFile ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void WaitPing()", focFUNCTION( BIND_CLASS Global_WaitPing ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool LoadFont(int font, string& fontFileName)", focFUNCTION( BIND_CLASS Global_LoadFont ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetDefaultFont(int font, uint color)", focFUNCTION( BIND_CLASS Global_SetDefaultFont ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetEffect(int effectType, int effectSubtype, string@+ effectName, string@+ effectDefines = null)", focFUNCTION( BIND_CLASS Global_SetEffect ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void RefreshMap(bool onlyTiles, bool onlyRoof, bool onlyLight)", focFUNCTION( BIND_CLASS Global_RefreshMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MouseClick(int x, int y, int button, int cursor)", focFUNCTION( BIND_CLASS Global_MouseClick ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void KeyboardPress(uint8 key1, uint8 key2, string@+ key1Text = null, string@+ key2Text = null)", focFUNCTION( BIND_CLASS Global_KeyboardPress ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SaveScreenshot()", focFUNCTION( BIND_CLASS Global_SaveScreenshot ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SaveLogFile()", focFUNCTION( BIND_CLASS Global_SaveLogFile ), asCALL_CDECL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( app == APP_TYPE_MAPPER )
    {
        RegisterGlobalFunction( engine, "void SetDefaultCritterParam(uint index, int param)", focFUNCTION( BIND_CLASS Global_SetDefaultCritterParam ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMapFileNames(string@+ dir, string@[]@+ names)", focFUNCTION( BIND_CLASS Global_GetMapFileNames ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "uint TabGetTileDirs(int tab, string@[]@+ dirNames, bool[]@+ includeSubdirs)", focFUNCTION( BIND_CLASS Global_TabGetTileDirs ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint TabGetItemPids(int tab, string@+ subTab, uint16[]@+ itemPids)", focFUNCTION( BIND_CLASS Global_TabGetItemPids ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint TabGetCritterPids(int tab, string@+ subTab, uint16[]@+ critterPids)", focFUNCTION( BIND_CLASS Global_TabGetCritterPids ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void TabSetTileDirs(int tab, string@[]@+ dirNames, bool[]@+ includeSubdirs)", focFUNCTION( BIND_CLASS Global_TabSetTileDirs ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void TabSetItemPids(int tab, string@+ subTab, uint16[]@+ itemPids)", focFUNCTION( BIND_CLASS Global_TabSetItemPids ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void TabSetCritterPids(int tab, string@+ subTab, uint16[]@+ critterPids)", focFUNCTION( BIND_CLASS Global_TabSetCritterPids ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void TabDelete(int tab)", focFUNCTION( BIND_CLASS Global_TabDelete ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void TabSelect(int tab, string@+ subTab, bool show = false)", focFUNCTION( BIND_CLASS Global_TabSelect ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void TabSetName(int tab, string@+ tabName)", focFUNCTION( BIND_CLASS Global_TabSetName ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "bool IsCritterCanWalk(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanWalk ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanRun(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRun ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanRotate(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRotate ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanAim(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanAim ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanArmor(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanArmor ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterAnim1(uint crType, uint anim1)", focFUNCTION( BIND_CLASS Global_IsCritterAnim1 ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetCritterAnimType(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAnimType ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetCritterAlias(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAlias ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetCritterTypeName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterTypeName ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetCritterSoundName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterSoundName ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "bool IsMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_IsMsgStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetMsgStr(int textMsg, uint strNum, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMsgStrSkip ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMsgStrNumUpper(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumUpper ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMsgStrNumLower(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumLower ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetMsgStrCount(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrCount ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "string@ ReplaceText(const string& text, const string& replace, const string& str)", focFUNCTION( BIND_CLASS Global_ReplaceTextStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ ReplaceText(const string& text, const string& replace, int i)", focFUNCTION( BIND_CLASS Global_ReplaceTextInt ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "void MoveScreen(uint16 hexX, uint16 hexY, uint speed)", focFUNCTION( BIND_CLASS Global_MoveScreen ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "uint LoadSprite(string& name, int pathIndex)", focFUNCTION( BIND_CLASS Global_LoadSprite ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint LoadSprite(uint nameHash, uint8 dir)", focFUNCTION( BIND_CLASS Global_LoadSpriteHash ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetSpriteWidth(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteWidth ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetSpriteHeight(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteHeight ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetSpriteCount(uint sprId)", focFUNCTION( BIND_CLASS Global_GetSpriteCount ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void GetTextInfo(string& text, int w, int h, int font, int flags, int& tw, int& th, int& lines)", focFUNCTION( BIND_CLASS Global_GetTextInfo ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color)", focFUNCTION( BIND_CLASS Global_DrawSprite ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteOffs ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpriteSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteSizeOffs ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawSpritePattern(uint sprId, int sprIndex, int x, int y, int w, int h, int sprWidth, int sprHeight, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpritePattern ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawText(string& text, int x, int y, int w, int h, uint color, int font, int flags)", focFUNCTION( BIND_CLASS Global_DrawText ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawPrimitive(int primitiveType, int[]& data)", focFUNCTION( BIND_CLASS Global_DrawPrimitive ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawMapSprite(uint16 hx, uint16 hy, uint16 effectPid, uint sprId, int sprIndex, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_DrawMapSprite ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawCritter2d(uint crType, uint anim1, uint anim2, uint8 dir, int l, int t, int r, int b, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter2d ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DrawCritter3d(uint instance, uint crType, uint anim1, uint anim2, int[]@+ layers, float[]@+ position, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter3d ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetHexPos(uint16 hx, uint16 hy, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHexPos ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetMonitorHex(int x, int y, uint16& hx, uint16& hy, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorHex ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps)", focFUNCTION( BIND_CLASS Global_MoveHexByDir ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetIfaceIniStr(string& key)", focFUNCTION( BIND_CLASS Global_GetIfaceIniStr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool LoadFont(int font, string& fontFileName)", focFUNCTION( BIND_CLASS Global_LoadFont ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetDefaultFont(int font, uint color)", focFUNCTION( BIND_CLASS Global_SetDefaultFont ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MouseClick(int x, int y, int button, int cursor)", focFUNCTION( BIND_CLASS Global_MouseClick ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void KeyboardPress(uint8 key1, uint8 key2, string@+ key1Text = null, string@+ key2Text = null)", focFUNCTION( BIND_CLASS Global_KeyboardPress ), asCALL_CDECL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        // AnyData
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int64[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int32[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int16[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int8[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint64[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint32[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint16[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint8[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int64[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int32[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int16[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, int8[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint64[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint32[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint16[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetAnyData(string& name, uint8[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, int64[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, int32[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, int16[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, int8[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, uint64[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, uint32[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, uint16[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetAnyData(string& name, uint8[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsAnyData(string& name)", focFUNCTION( BIND_CLASS Global_IsAnyData ), asCALL_CDECL );
        // RegisterGlobalFunction( engine, "bool AnyDataClass(?& storedClass, ?[]& array)", focFUNCTION( BIND_CLASS Global_AnyDataClass ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void EraseAnyData(string& name)", focFUNCTION( BIND_CLASS Global_EraseAnyData ), asCALL_CDECL );
        // TimeEvent
        RegisterGlobalFunction( engine, "uint CreateTimeEvent(uint beginSecond, string& funcName, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventEmpty ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint CreateTimeEvent(uint beginSecond, string& funcName, uint value, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValue ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint CreateTimeEvent(uint beginSecond, string& funcName, int value, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValue ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint CreateTimeEvent(uint beginSecond, string& funcName, uint[]& values, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValues ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint CreateTimeEvent(uint beginSecond, string& funcName, int[]& values, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValues ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool EraseTimeEvent(uint num)", focFUNCTION( BIND_CLASS Global_EraseTimeEvent ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetTimeEvent(uint num, uint& duration, uint[]@+ values)", focFUNCTION( BIND_CLASS Global_GetTimeEvent ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool GetTimeEvent(uint num, uint& duration, int[]@+ values)", focFUNCTION( BIND_CLASS Global_GetTimeEvent ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetTimeEvent(uint num, uint duration, uint[]@+ values)", focFUNCTION( BIND_CLASS Global_SetTimeEvent ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetTimeEvent(uint num, uint duration, int[]@+ values)", focFUNCTION( BIND_CLASS Global_SetTimeEvent ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "void RadioMessage(uint16 channel, string& text)", focFUNCTION( BIND_CLASS Global_RadioMessage ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void RadioMessageMsg(uint16 channel, uint16 textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_RadioMessageMsg ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void RadioMessageMsg(uint16 channel, uint16 textMsg, uint strNum, string@+ lexems)", focFUNCTION( BIND_CLASS Global_RadioMessageMsgLex ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool RunDialog(Critter& player, Critter& npc, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogNpc ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool RunDialog(Critter& player, Critter& npc, uint dialogPack, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogNpcDlgPack ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool RunDialog(Critter& player, uint dialogPack, uint16 hexX, uint16 hexY, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogHex ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int64 WorldItemCount(uint16 protoId)", focFUNCTION( BIND_CLASS Global_WorldItemCount ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetBestScore(int score, Critter@+ player, string& name)", focFUNCTION( BIND_CLASS Global_SetBestScore ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool AddTextListener(int sayType, string& firstStr, uint16 parameter, string& scriptName)", focFUNCTION( BIND_CLASS Global_AddTextListener ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void EraseTextListener(int sayType, string& firstStr, uint16 parameter)", focFUNCTION( BIND_CLASS Global_EraseTextListener ), asCALL_CDECL );
        // RegisterGlobalFunction( engine, "uint8 ReverseDir(uint8 dir)", focFUNCTION( BIND_CLASS Global_ReverseDir ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetBagItems(uint bagId, uint16[]@+ pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots)", focFUNCTION( BIND_CLASS Global_GetBagItems ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetChosenSendParameter(int index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetChosenSendParameter ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetSendParameter(int index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetSendParameter ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetSendParameter(int index, bool enabled, string@+ allowFunc)", focFUNCTION( BIND_CLASS Global_SetSendParameterFunc ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetAllMaps(uint16 pid, Map@[]@+ maps)", focFUNCTION( BIND_CLASS Global_GetAllMaps ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetAllLocations(uint16 pid, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetAllLocations ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetScriptId(string& scriptName, string& funcDeclaration)", focFUNCTION( BIND_CLASS Global_GetScriptId ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetScriptName(uint scriptId)", focFUNCTION( BIND_CLASS Global_GetScriptName ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int8[]@ GetItemDataMask(int maskType)", focFUNCTION( BIND_CLASS Global_GetItemDataMask ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetItemDataMask(int maskType, int8[]& mask)", focFUNCTION( BIND_CLASS Global_SetItemDataMask ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool LoadImage(uint index, string@+ imageName, uint imageDepth, int pathType)", focFUNCTION( BIND_CLASS Global_LoadImage ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetImageColor(uint index, uint x, uint y)", focFUNCTION( BIND_CLASS Global_GetImageColor ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetParameterDialogGetBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterDialogGetBehaviour ), asCALL_CDECL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_MAPPER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_MAPPER )
    {
        RegisterGlobalFunction( engine, "void Message(string& text)", focFUNCTION( BIND_CLASS Global_Message ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void Message(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_MessageMsg ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MapMessage(string& text, uint16 hx, uint16 hy, uint timeMs, uint color, bool fade, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_MapMessage ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist)", focFUNCTION( BIND_CLASS Global_GetHexInPath ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthHex ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "void SetRainAnimation(string@+ fallAnimName, string@+ dropAnimName)", focFUNCTION( BIND_CLASS Global_SetRainAnimation ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetZoom(float zoom)", focFUNCTION( BIND_CLASS Global_SetZoom ), asCALL_CDECL );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "uint GetFullSecond(uint16 year, uint16 month, uint16 day, uint16 hour, uint16 minute, uint16 second)", focFUNCTION( BIND_CLASS Global_GetFullSecond ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void GetTime(uint16& year, uint16& month, uint16& day, uint16& dayOfWeek, uint16& hour, uint16& minute, uint16& second, uint16& milliseconds)", focFUNCTION( BIND_CLASS Global_GetTime ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void GetGameTime(uint fullSecond, uint16& year, uint16& month, uint16& day, uint16& dayOfWeek, uint16& hour, uint16& minute, uint16& second)", focFUNCTION( BIND_CLASS Global_GetGameTime ), asCALL_CDECL );
        // RegisterGlobalFunction( engine, "void GetVersion(uint& server, uint& client, uint& net)", focFUNCTION( BIND_CLASS Global_GetVersion ), asCALL_CDECL );
        // RegisterGlobalFunction( engine, "void Sort(int[]& array, string& func)", focFUNCTION( BIND_CLASS Global_Sort ), asCALL_CDECL );
        // RegisterGlobalFunction( engine, "void Sort(uint[]& array, string& func)", focFUNCTION( BIND_CLASS Global_Sort ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetParameterGetBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterGetBehaviour ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SetParameterChangeBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterChangeBehaviour ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetRegistrationParameter(uint index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetRegistrationParam ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanWalk(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanWalk ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanRun(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRun ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanRotate(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRotate ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanAim(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanAim ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterCanArmor(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanArmor ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool IsCritterAnim1(uint crType, uint anim1)", focFUNCTION( BIND_CLASS Global_IsCritterAnim1 ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetCritterAnimType(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAnimType ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetCritterAlias(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAlias ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetCritterTypeName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterTypeName ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetCritterSoundName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterSoundName ), asCALL_CDECL );
    }
    #endif


    /*******************************************************************************************************************************/


    //
    //   CraftItem
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "CraftItem@ GetCraftItem(uint num)", focFUNCTION( BIND_CLASS Global_GetCraftItem ), asCALL_CDECL );

        RegisterObjectProperty( engine, "CraftItem", "const uint Num", focOFFSET( CraftItem, Num ) );
        RegisterObjectProperty( engine, "CraftItem", "const string Name", focOFFSET( CraftItem, Name ) );
        RegisterObjectProperty( engine, "CraftItem", "const string Info", focOFFSET( CraftItem, Info ) );
        RegisterObjectProperty( engine, "CraftItem", "const uint Experience", focOFFSET( CraftItem, Experience ) );
        RegisterObjectProperty( engine, "CraftItem", "const string Script", focOFFSET( CraftItem, Script ) );

        RegisterObjectMethod( engine, "CraftItem", "uint GetShowParams(array<uint>@+ nums, array<int>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetShowParams ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CraftItem", "uint GetNeedParams(array<uint>@+ nums, array<int>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedParams ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CraftItem", "uint GetNeedTools(array<uint16>@+ pids, array<uint>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedTools ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CraftItem", "uint GetNeedItems(array<uint16>@+ pids, array<uint>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedItems ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CraftItem", "uint GetOutItems(array<uint16>@+ pids, array<uint>@+ values)", focFUNCTION( BIND_CLASS CraftItem_GetOutItems ), asCALL_CDECL_OBJFIRST );
    }
    #endif


    //
    // Critter
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        // Critter (any)
        RegisterGlobalFunction( engine, "uint GetCrittersDistantion(Critter& cr1, Critter& cr2)", focFUNCTION( BIND_CLASS Global_GetCrittersDistantion ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetGlobalMapCritters(uint16 worldX, uint16 worldY, uint radius, int findType, Critter@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetGlobalMapCritters ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "Critter@+ GetCritter(uint critterId)", focFUNCTION( BIND_CLASS Global_GetCritter ), asCALL_CDECL );

        // Npc
        RegisterGlobalFunction( engine, "void GetProtoCritter(uint16 protoId, int[]& data)", focFUNCTION( BIND_CLASS Global_GetProtoCritter ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DeleteNpc(Critter& npc)", focFUNCTION( BIND_CLASS Global_DeleteNpc ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetAllNpc(uint16 pid, Critter@[]@+ npc)", focFUNCTION( BIND_CLASS Global_GetAllNpc ), asCALL_CDECL );
        // Player
        RegisterGlobalFunction( engine, "Critter@+ GetPlayer(string& name)", focFUNCTION( BIND_CLASS Global_GetPlayer ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetPlayerId(string& name)", focFUNCTION( BIND_CLASS Global_GetPlayerId ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "string@ GetPlayerName(uint playerId)", focFUNCTION( BIND_CLASS Global_GetPlayerName ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetAllPlayers(Critter@[]@+ players)", focFUNCTION( BIND_CLASS Global_GetAllPlayers ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetRegisteredPlayers(uint[]& ids, string@[]& names)", focFUNCTION( BIND_CLASS Global_GetRegisteredPlayers ), asCALL_CDECL );

        // Parameters
        RegisterObjectProperty( engine, "Critter", "const uint Id", focOFFSET( Critter, Data ) + focOFFSET( CritData, Id ) );
        RegisterObjectProperty( engine, "Critter", "const uint CrType", focOFFSET( Critter, Data ) + focOFFSET( CritData, BaseType ) );
        RegisterObjectProperty( engine, "Critter", "const uint16 HexX", focOFFSET( Critter, Data ) + focOFFSET( CritData, HexX ) );
        RegisterObjectProperty( engine, "Critter", "const uint16 HexY", focOFFSET( Critter, Data ) + focOFFSET( CritData, HexY ) );
        RegisterObjectProperty( engine, "Critter", "const uint16 WorldX", focOFFSET( Critter, Data ) + focOFFSET( CritData, WorldX ) );
        RegisterObjectProperty( engine, "Critter", "const uint16 WorldY", focOFFSET( Critter, Data ) + focOFFSET( CritData, WorldY ) );
        RegisterObjectProperty( engine, "Critter", "const uint8 Dir", focOFFSET( Critter, Data ) + focOFFSET( CritData, Dir ) );
        RegisterObjectProperty( engine, "Critter", "const uint8 Cond", focOFFSET( Critter, Data ) + focOFFSET( CritData, Cond ) );
        RegisterObjectProperty( engine, "Critter", "const uint Anim1Life", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim1Life ) );
        RegisterObjectProperty( engine, "Critter", "const uint Anim1Knockout", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim1Knockout ) );
        RegisterObjectProperty( engine, "Critter", "const uint Anim1Dead", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim1Dead ) );
        RegisterObjectProperty( engine, "Critter", "const uint Anim2Life", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim2Life ) );
        RegisterObjectProperty( engine, "Critter", "const uint Anim2Knockout", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim2Knockout ) );
        RegisterObjectProperty( engine, "Critter", "const uint Anim2Dead", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim2Dead ) );
        RegisterObjectProperty( engine, "Critter", "const uint Flags", focOFFSET( Critter, Flags ) );
        RegisterObjectProperty( engine, "Critter", "const string Name", focOFFSET( Critter, NameStr ) );
        RegisterObjectProperty( engine, "Critter", "uint ShowCritterDist1", focOFFSET( Critter, Data ) + focOFFSET( CritData, ShowCritterDist1 ) );
        RegisterObjectProperty( engine, "Critter", "uint ShowCritterDist2", focOFFSET( Critter, Data ) + focOFFSET( CritData, ShowCritterDist2 ) );
        RegisterObjectProperty( engine, "Critter", "uint ShowCritterDist3", focOFFSET( Critter, Data ) + focOFFSET( CritData, ShowCritterDist3 ) );
        RegisterObjectProperty( engine, "Critter", "bool IsRuning", focOFFSET( Critter, IsRuning ) );
        RegisterObjectProperty( engine, "Critter", "const bool IsNotValid", focOFFSET( Critter, IsNotValid ) );
        RegisterObjectProperty( engine, "Critter", "const int Ref", focOFFSET( Critter, RefCounter ) );
        RegisterObjectProperty( engine, "Critter", "DataVal Param", focOFFSET( Critter, ThisPtr[0] ) );
        RegisterObjectProperty( engine, "Critter", "DataRef ParamBase", focOFFSET( Critter, ThisPtr[0] ) );

        RegisterObjectMethod( engine, "Critter", "bool IsPlayer() const", focFUNCTION( BIND_CLASS Crit_IsPlayer ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsNpc() const", focFUNCTION( BIND_CLASS Crit_IsNpc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsCanWalk() const", focFUNCTION( BIND_CLASS Crit_IsCanWalk ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsCanRun() const", focFUNCTION( BIND_CLASS Crit_IsCanRun ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsCanRotate() const", focFUNCTION( BIND_CLASS Crit_IsCanRotate ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsCanAim() const", focFUNCTION( BIND_CLASS Crit_IsCanAim ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsAnim1(uint index) const", focFUNCTION( BIND_CLASS Crit_IsAnim1 ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "uint8 GetAccess() const", focFUNCTION( BIND_CLASS Cl_GetAccess ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetAccess(uint8 access) const", focFUNCTION( BIND_CLASS Cl_SetAccess ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetUID(uint8 index)", focFUNCTION( BIND_CLASS Cl_GetUID ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetIp()", focFUNCTION( BIND_CLASS Cl_GetIp ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "string@ GetIpString()", focFUNCTION( BIND_CLASS Cl_GetIpString ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool get_IsDisconnected() const", focFUNCTION( BIND_CLASS Cl_get_IsDisconnected ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Crit_SetEvent ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetLexems(string@+ lexems)", focFUNCTION( BIND_CLASS Crit_SetLexems ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "Map@+ GetMap() const", focFUNCTION( BIND_CLASS Crit_GetMap ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetMapId() const", focFUNCTION( BIND_CLASS Crit_GetMapId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint16 GetMapProtoId() const", focFUNCTION( BIND_CLASS Crit_GetMapProtoId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetHomePos(uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Crit_SetHomePos ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void GetHomePos(uint& mapId, uint16& hexX, uint16& hexY, uint8& dir)", focFUNCTION( BIND_CLASS Crit_GetHomePos ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool ChangeCrType(uint newType)", focFUNCTION( BIND_CLASS Crit_ChangeCrType ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void DropTimers()", focFUNCTION( BIND_CLASS Cl_DropTimers ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "bool MoveRandom()", focFUNCTION( BIND_CLASS Crit_MoveRandom ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool MoveToDir(uint8 dir)", focFUNCTION( BIND_CLASS Crit_MoveToDir ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool TransitToHex(uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Crit_TransitToHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool TransitToMap(uint mapId, uint16 hexX, uint16 hexY, uint8 dir, bool withGroup = false)", focFUNCTION( BIND_CLASS Crit_TransitToMapHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool TransitToMap(uint mapId, int entireNum, bool withGroup = false)", focFUNCTION( BIND_CLASS Crit_TransitToMapEntire ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool TransitToGlobal(bool requestGroup)", focFUNCTION( BIND_CLASS Crit_TransitToGlobal ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool TransitToGlobal(Critter@[]& group)", focFUNCTION( BIND_CLASS Crit_TransitToGlobalWithGroup ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool TransitToGlobalGroup(uint critterId)", focFUNCTION( BIND_CLASS Crit_TransitToGlobalGroup ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void AddScore(uint score, int val)", focFUNCTION( BIND_CLASS Crit_AddScore ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "int GetScore(uint score)", focFUNCTION( BIND_CLASS Crit_GetScore ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void AddHolodiskInfo(uint holodiskNum)", focFUNCTION( BIND_CLASS Crit_AddHolodiskInfo ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EraseHolodiskInfo(uint holodiskNum)", focFUNCTION( BIND_CLASS Crit_EraseHolodiskInfo ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsHolodiskInfo(uint holodiskNum) const", focFUNCTION( BIND_CLASS Crit_IsHolodiskInfo ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "bool IsLife() const", focFUNCTION( BIND_CLASS Crit_IsLife ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsKnockout() const", focFUNCTION( BIND_CLASS Crit_IsKnockout ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsDead() const", focFUNCTION( BIND_CLASS Crit_IsDead ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsFree() const", focFUNCTION( BIND_CLASS Crit_IsFree ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsBusy() const", focFUNCTION( BIND_CLASS Crit_IsBusy ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void Wait(uint ms)", focFUNCTION( BIND_CLASS Crit_Wait ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void ToDead(uint anim2, Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_ToDead ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool ToLife()", focFUNCTION( BIND_CLASS Crit_ToLife ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool ToKnockout(uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint16 knockHx, uint16 knockHy)", focFUNCTION( BIND_CLASS Crit_ToKnockout ), asCALL_CDECL_OBJFIRST );
        // RegisterObjectMethod( engine, "Critter", "void GetProtoData(int[]& data)", focFUNCTION( BIND_CLASS Npc_GetProtoData ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void RefreshVisible()", focFUNCTION( BIND_CLASS Crit_RefreshVisible ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void ViewMap(Map& map, uint look, uint16 hx, uint16 hy, uint8 dir)", focFUNCTION( BIND_CLASS Crit_ViewMap ), asCALL_CDECL_OBJFIRST );
        // RegisterObjectMethod( engine, "Critter", "void Mute(uint ms)", focFUNCTION( BIND_CLASS Crit_Mute ), asCALL_CDECL_OBJFIRST );
        // RegisterObjectMethod( engine, "Critter", "void Ban(uint ms)", focFUNCTION( BIND_CLASS Crit_Mute ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "Item@+ AddItem(uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Crit_AddItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool DeleteItem(uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Crit_DeleteItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint ItemsCount() const", focFUNCTION( BIND_CLASS Crit_ItemsCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint ItemsWeight() const", focFUNCTION( BIND_CLASS Crit_ItemsWeight ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint ItemsVolume() const", focFUNCTION( BIND_CLASS Crit_ItemsVolume ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint CountItem(uint16 protoId) const", focFUNCTION( BIND_CLASS Crit_CountItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "Item@+ GetItem(uint16 protoId, int slot) const", focFUNCTION( BIND_CLASS Crit_GetItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "Item@+ GetItemById(uint itemId) const", focFUNCTION( BIND_CLASS Crit_GetItemById ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetItems(int slot, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItems ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetItemsByType(int type, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItemsByType ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "ProtoItem@+ GetSlotProto(int slot, uint8& mode) const", focFUNCTION( BIND_CLASS Crit_GetSlotProto ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool MoveItem(uint itemId, uint count, uint8 toSlot)", focFUNCTION( BIND_CLASS Crit_MoveItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool PickItem(uint16 hexX, uint16 hexY, uint16 protoId)", focFUNCTION( BIND_CLASS Crit_PickItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetFavoriteItem(int slot, uint16 pid)", focFUNCTION( BIND_CLASS Crit_SetFavoriteItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint16 GetFavoriteItem(int slot)", focFUNCTION( BIND_CLASS Crit_GetFavoriteItem ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "uint GetCritters(bool lookOnMe, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Crit_GetCritters ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetFollowGroup(int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Crit_GetFollowGroup ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "Critter@+ GetFollowLeader() const", focFUNCTION( BIND_CLASS Crit_GetFollowLeader ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "Critter@[]@ GetGlobalGroup() const", focFUNCTION( BIND_CLASS Crit_GetGlobalGroup ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsGlobalGroupLeader()", focFUNCTION( BIND_CLASS Crit_IsGlobalGroupLeader ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void LeaveGlobalGroup()", focFUNCTION( BIND_CLASS Crit_LeaveGlobalGroup ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void GiveGlobalGroupLead(Critter& toCr)", focFUNCTION( BIND_CLASS Crit_GiveGlobalGroupLead ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetTalkedPlayers(Critter@[]@+ players) const", focFUNCTION( BIND_CLASS Npc_GetTalkedPlayers ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsSee(Critter& cr) const", focFUNCTION( BIND_CLASS Crit_IsSeeCr ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsSeenBy(Critter& cr) const", focFUNCTION( BIND_CLASS Crit_IsSeenByCr ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsSee(Item& item) const", focFUNCTION( BIND_CLASS Crit_IsSeeItem ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void Say(uint8 howSay, string& text)", focFUNCTION( BIND_CLASS Crit_Say ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SayMsg(uint8 howSay, uint16 textMsg, uint strNum)", focFUNCTION( BIND_CLASS Crit_SayMsg ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SayMsg(uint8 howSay, uint16 textMsg, uint strNum, string& lexems)", focFUNCTION( BIND_CLASS Crit_SayMsgLex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetDir(uint8 dir)", focFUNCTION( BIND_CLASS Crit_SetDir ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "uint ErasePlane(int planeType, bool all)", focFUNCTION( BIND_CLASS Npc_ErasePlane ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool ErasePlane(uint index)", focFUNCTION( BIND_CLASS Npc_ErasePlaneIndex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void DropPlanes()", focFUNCTION( BIND_CLASS Npc_DropPlanes ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsNoPlanes() const", focFUNCTION( BIND_CLASS Npc_IsNoPlanes ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsCurPlane(int planeType) const", focFUNCTION( BIND_CLASS Npc_IsCurPlane ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "NpcPlane@+ GetCurPlane() const", focFUNCTION( BIND_CLASS Npc_GetCurPlane ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetPlanes(NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanes ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetPlanes(int identifier, NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanesIdentifier ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetPlanes(int identifier, uint identifierExt, NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanesIdentifier2 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool AddPlane(NpcPlane& plane)", focFUNCTION( BIND_CLASS Npc_AddPlane ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void SendMessage(int num, int val, int to)", focFUNCTION( BIND_CLASS Crit_SendMessage ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void Action(int action, int actionExt, Item@+ item)", focFUNCTION( BIND_CLASS Crit_Action ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void Animate(uint anim1, uint anim2, Item@+ item, bool clearSequence, bool delayPlay)", focFUNCTION( BIND_CLASS Crit_Animate ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetAnims(int cond, uint anim1, uint anim2)", focFUNCTION( BIND_CLASS Crit_SetAnims ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void PlaySound(string& soundName, bool sendSelf)", focFUNCTION( BIND_CLASS Crit_PlaySound ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void PlaySound(uint8 soundType, uint8 soundTypeExt, uint8 soundId, uint8 soundIdExt, bool sendSelf)", focFUNCTION( BIND_CLASS Crit_PlaySoundType ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void SendCombatResult(uint[]& combatResult)", focFUNCTION( BIND_CLASS Crit_SendCombatResult ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool IsKnownLoc(bool byId, uint locNum) const", focFUNCTION( BIND_CLASS Cl_IsKnownLoc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool SetKnownLoc(bool byId, uint locNum)", focFUNCTION( BIND_CLASS Cl_SetKnownLoc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool UnsetKnownLoc(bool byId, uint locNum)", focFUNCTION( BIND_CLASS Cl_UnsetKnownLoc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetFog(uint16 zoneX, uint16 zoneY, int fog)", focFUNCTION( BIND_CLASS Cl_SetFog ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "int GetFog(uint16 zoneX, uint16 zoneY)", focFUNCTION( BIND_CLASS Cl_GetFog ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void ShowContainer(Critter@+ contCr, Item@+ contItem, uint8 transferType)", focFUNCTION( BIND_CLASS Cl_ShowContainer ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void ShowScreen(int screenType, uint param, string@+ funcName)", focFUNCTION( BIND_CLASS Cl_ShowScreen ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void RunClientScript(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Cl_RunClientScript ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void Disconnect()", focFUNCTION( BIND_CLASS Cl_Disconnect ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Crit_SetScript ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Crit_GetScriptId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetBagRefreshTime(uint realMinutes)", focFUNCTION( BIND_CLASS Crit_SetBagRefreshTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetBagRefreshTime() const", focFUNCTION( BIND_CLASS Crit_GetBagRefreshTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetInternalBag(uint16[]& pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots)", focFUNCTION( BIND_CLASS Crit_SetInternalBag ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetInternalBag(uint16[]@+ pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots) const", focFUNCTION( BIND_CLASS Crit_GetInternalBag ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Crit_GetProtoId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetMultihex() const", focFUNCTION( BIND_CLASS Crit_GetMultihex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void SetMultihex(int value)", focFUNCTION( BIND_CLASS Crit_SetMultihex ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void AddEnemyInStack(uint critterId)", focFUNCTION( BIND_CLASS Crit_AddEnemyInStack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool CheckEnemyInStack(uint critterId) const", focFUNCTION( BIND_CLASS Crit_CheckEnemyInStack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EraseEnemyFromStack(uint critterId)", focFUNCTION( BIND_CLASS Crit_EraseEnemyFromStack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void ChangeEnemyStackSize(uint newSize)", focFUNCTION( BIND_CLASS Crit_ChangeEnemyStackSize ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void GetEnemyStack(uint[]& enemyStack) const", focFUNCTION( BIND_CLASS Crit_GetEnemyStack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void ClearEnemyStack()", focFUNCTION( BIND_CLASS Crit_ClearEnemyStack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void ClearEnemyStackNpc()", focFUNCTION( BIND_CLASS Crit_ClearEnemyStackNpc ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "bool AddTimeEvent(string& funcName, uint duration, int identifier)", focFUNCTION( BIND_CLASS Crit_AddTimeEvent ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool AddTimeEvent(string& funcName, uint duration, int identifier, uint rate)", focFUNCTION( BIND_CLASS Crit_AddTimeEventRate ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetTimeEvents(int identifier, uint[]@+ indexes, uint[]@+ durations, uint[]@+ rates) const", focFUNCTION( BIND_CLASS Crit_GetTimeEvents ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint GetTimeEvents(int[]& findIdentifiers, int[]@+ identifiers, uint[]@+ indexes, uint[]@+ durations, uint[]@+ rates) const", focFUNCTION( BIND_CLASS Crit_GetTimeEventsArr ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void ChangeTimeEvent(uint index, uint newDuration, uint newRate)", focFUNCTION( BIND_CLASS Crit_ChangeTimeEvent ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EraseTimeEvent(uint index)", focFUNCTION( BIND_CLASS Crit_EraseTimeEvent ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint EraseTimeEvents(int identifier)", focFUNCTION( BIND_CLASS Crit_EraseTimeEvents ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "uint EraseTimeEvents(int[]& identifiers)", focFUNCTION( BIND_CLASS Crit_EraseTimeEventsArr ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Critter", "void EventIdle()", focFUNCTION( BIND_CLASS Crit_EventIdle ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Crit_EventFinish ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventDead(Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_EventDead ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventRespawn()", focFUNCTION( BIND_CLASS Crit_EventRespawn ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventShowCritter(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventShowCritter1(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter1 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventShowCritter2(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter2 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventShowCritter3(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter3 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventHideCritter(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventHideCritter1(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter1 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventHideCritter2(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter2 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventHideCritter3(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter3 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventShowItemOnMap(Item& showItem, bool added, Critter@+ dropper)", focFUNCTION( BIND_CLASS Crit_EventShowItemOnMap ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventChangeItemOnMap(Item& item)", focFUNCTION( BIND_CLASS Crit_EventChangeItemOnMap ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventHideItemOnMap(Item& hideItem, bool removed, Critter@+ picker)", focFUNCTION( BIND_CLASS Crit_EventHideItemOnMap ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventAttack(Critter& target)", focFUNCTION( BIND_CLASS Crit_EventAttack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventAttacked(Critter@+ attacker)", focFUNCTION( BIND_CLASS Crit_EventAttacked ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventStealing(Critter& thief, Item& item, uint count)", focFUNCTION( BIND_CLASS Crit_EventStealing ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventMessage(Critter& fromCr, int message, int value)", focFUNCTION( BIND_CLASS Crit_EventMessage ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventUseItem(Item& item, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventUseItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventUseItemOnMe(Critter& whoUse, Item& item)", focFUNCTION( BIND_CLASS Crit_EventUseItemOnMe ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventUseSkill(int skill, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventUseSkill ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventUseSkillOnMe(Critter& whoUse, int skill)", focFUNCTION( BIND_CLASS Crit_EventUseSkillOnMe ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventDropItem(Item& item)", focFUNCTION( BIND_CLASS Crit_EventDropItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventMoveItem(Item& item, uint8 fromSlot)", focFUNCTION( BIND_CLASS Crit_EventMoveItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventKnockout(uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint knockDist)", focFUNCTION( BIND_CLASS Crit_EventKnockout ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthDead(Critter& fromCr, Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_EventSmthDead ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthStealing(Critter& fromCr, Critter& thief, bool success, Item& item, uint count)", focFUNCTION( BIND_CLASS Crit_EventSmthStealing ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthAttack(Critter& fromCr, Critter& target)", focFUNCTION( BIND_CLASS Crit_EventSmthAttack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthAttacked(Critter& fromCr, Critter@+ attacker)", focFUNCTION( BIND_CLASS Crit_EventSmthAttacked ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthUseItem(Critter& fromCr, Item& item, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventSmthUseItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthUseSkill(Critter& fromCr, int skill, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventSmthUseSkill ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthDropItem(Critter& fromCr, Item& item)", focFUNCTION( BIND_CLASS Crit_EventSmthDropItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthMoveItem(Critter& fromCr, Item& item, uint8 fromSlot)", focFUNCTION( BIND_CLASS Crit_EventSmthMoveItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthKnockout(Critter& fromCr, uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint knockDist)", focFUNCTION( BIND_CLASS Crit_EventSmthKnockout ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "int EventPlaneBegin(NpcPlane& plane, int reason, Critter@+ someCr, Item@+ someItem)", focFUNCTION( BIND_CLASS Crit_EventPlaneBegin ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "int EventPlaneEnd(NpcPlane& plane, int reason, Critter@+ someCr, Item@+ someItem)", focFUNCTION( BIND_CLASS Crit_EventPlaneEnd ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "int EventPlaneRun(NpcPlane& plane, int reason, uint& p0, uint& p1, uint& p2)", focFUNCTION( BIND_CLASS Crit_EventPlaneRun ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventBarter(Critter& barterCr, bool attach, uint barterCount)", focFUNCTION( BIND_CLASS Crit_EventBarter ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventTalk(Critter& talkCr, bool attach, uint talkCount)", focFUNCTION( BIND_CLASS Crit_EventTalk ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventGlobalProcess(int type, Item@ car, float& x, float& y, float& toX, float& toY, float& speed, uint& encounterDescriptor, bool& waitForAnswer)", focFUNCTION( BIND_CLASS Crit_EventGlobalProcess ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "bool EventGlobalInvite(Item@ car, uint encounterDescriptor, int combatMode, uint& mapId, uint16& hexX, uint16& hexY, uint8& dir)", focFUNCTION( BIND_CLASS Crit_EventGlobalInvite ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventTurnBasedProcess(Map& map, bool beginTurn)", focFUNCTION( BIND_CLASS Crit_EventTurnBasedProcess ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Critter", "void EventSmthTurnBasedProcess(Critter& fromCr, Map& map, bool beginTurn)", focFUNCTION( BIND_CLASS Crit_EventSmthTurnBasedProcess ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // CritterCl
    //

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( app == APP_TYPE_CLIENT )
    {
        RegisterGlobalFunction( engine, "CritterCl@+ GetChosen()", focFUNCTION( BIND_CLASS Global_GetChosen ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetChosenActions(uint[]@+ actions)", focFUNCTION( BIND_CLASS Global_GetChosenActions ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SetChosenActions(uint[]@+ actions)", focFUNCTION( BIND_CLASS Global_SetChosenActions ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "CritterCl@+ GetCritter(uint critterId)", focFUNCTION( BIND_CLASS Global_GetCritter ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetCrittersHex(uint16 hexX, uint16 hexY, uint radius, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCritters ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetCritters(uint16 pid, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCrittersByPids ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetCrittersDistantion(CritterCl& cr1, CritterCl& cr2)", focFUNCTION( BIND_CLASS Global_GetCrittersDistantion ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCrittersInPath ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, CritterCl@[]@+ critters, uint16& preBlockHx, uint16& preBlockHy, uint16& blockHx, uint16& blockHy)", focFUNCTION( BIND_CLASS Global_GetCrittersInPathBlock ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "CritterCl@+ GetMonitorCritter(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorCritter ), asCALL_CDECL );

        RegisterObjectProperty( engine, "CritterCl", "const uint Id", focOFFSET( CritterCl, Id ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint16 Pid", focOFFSET( CritterCl, Pid ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint CrType", focOFFSET( CritterCl, BaseType ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint CrTypeAlias", focOFFSET( CritterCl, BaseTypeAlias ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint16 HexX", focOFFSET( CritterCl, HexX ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint16 HexY", focOFFSET( CritterCl, HexY ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint8 Dir", focOFFSET( CritterCl, CrDir ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint8 Cond", focOFFSET( CritterCl, Cond ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint Anim1Life", focOFFSET( CritterCl, Anim1Life ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint Anim1Knockout", focOFFSET( CritterCl, Anim1Knockout ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint Anim1Dead", focOFFSET( CritterCl, Anim1Dead ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint Anim2Life", focOFFSET( CritterCl, Anim2Life ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint Anim2Knockout", focOFFSET( CritterCl, Anim2Knockout ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint Anim2Dead", focOFFSET( CritterCl, Anim2Dead ) );
        RegisterObjectProperty( engine, "CritterCl", "const uint Flags", focOFFSET( CritterCl, Flags ) );
        RegisterObjectProperty( engine, "CritterCl", "string Name", focOFFSET( CritterCl, Name ) );
        RegisterObjectProperty( engine, "CritterCl", "string NameOnHead", focOFFSET( CritterCl, NameOnHead ) );
        RegisterObjectProperty( engine, "CritterCl", "string Lexems", focOFFSET( CritterCl, Lexems ) );
        RegisterObjectProperty( engine, "CritterCl", "uint NameColor", focOFFSET( CritterCl, NameColor ) );
        RegisterObjectProperty( engine, "CritterCl", "const int16 Ref", focOFFSET( CritterCl, RefCounter ) );
        RegisterObjectProperty( engine, "CritterCl", "DataVal Param", focOFFSET( CritterCl, ThisPtr[0] ) );
        RegisterObjectProperty( engine, "CritterCl", "DataRef ParamBase", focOFFSET( CritterCl, ThisPtr[0] ) );
        RegisterObjectProperty( engine, "CritterCl", "int[]@ Anim3dLayer", focOFFSET( CritterCl, Layers3d ) );
        //
        RegisterObjectProperty( engine, "CritterCl", "uint8 Alpha", focOFFSET( CritterCl, Alpha ) );
        RegisterObjectProperty( engine, "CritterCl", "const bool FadingEnable", focOFFSET( CritterCl, fadingEnable ) );

        RegisterObjectMethod( engine, "CritterCl", "bool IsChosen() const", focFUNCTION( BIND_CLASS Crit_IsChosen ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsPlayer() const", focFUNCTION( BIND_CLASS Crit_IsPlayer ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsNpc() const", focFUNCTION( BIND_CLASS Crit_IsNpc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsLife() const", focFUNCTION( BIND_CLASS Crit_IsLife ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsKnockout() const", focFUNCTION( BIND_CLASS Crit_IsKnockout ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsDead() const", focFUNCTION( BIND_CLASS Crit_IsDead ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsFree() const", focFUNCTION( BIND_CLASS Crit_IsFree ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsBusy() const", focFUNCTION( BIND_CLASS Crit_IsBusy ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsAnim3d() const", focFUNCTION( BIND_CLASS Crit_IsAnim3d ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsAnimAviable(uint anim1, uint anim2) const", focFUNCTION( BIND_CLASS Crit_IsAnimAviable ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsAnimPlaying() const", focFUNCTION( BIND_CLASS Crit_IsAnimPlaying ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint GetAnim1() const", focFUNCTION( BIND_CLASS Crit_GetAnim1 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "void Animate(uint anim1, uint anim2)", focFUNCTION( BIND_CLASS Crit_Animate ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "void Animate(uint anim1, uint anim2, ItemCl@+ item)", focFUNCTION( BIND_CLASS Crit_AnimateEx ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "void ClearAnim()", focFUNCTION( BIND_CLASS Crit_ClearAnim ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "void Wait(uint ms)", focFUNCTION( BIND_CLASS Crit_Wait ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint ItemsCount() const", focFUNCTION( BIND_CLASS Crit_ItemsCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint ItemsWeight() const", focFUNCTION( BIND_CLASS Crit_ItemsWeight ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint ItemsVolume() const", focFUNCTION( BIND_CLASS Crit_ItemsVolume ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint CountItem(uint16 protoId) const", focFUNCTION( BIND_CLASS Crit_CountItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "ItemCl@+ GetItem(uint16 protoId, int slot) const", focFUNCTION( BIND_CLASS Crit_GetItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint GetItems(int slot, ItemCl@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItems ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint GetItemsByType(int type, ItemCl@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItemsByType ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "ProtoItem@+ GetSlotProto(int slot, uint8& mode) const", focFUNCTION( BIND_CLASS Crit_GetSlotProto ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "void SetVisible(bool visible)", focFUNCTION( BIND_CLASS Crit_SetVisible ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool GetVisible() const", focFUNCTION( BIND_CLASS Crit_GetVisible ), asCALL_CDECL_OBJFIRST );
        // RegisterObjectMethod( engine, "CritterCl", "bool IsMyTurn() const", focFUNCTION( BIND_CLASS Crit_CheckKey ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "void set_ContourColor(uint value)", focFUNCTION( BIND_CLASS Crit_set_ContourColor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint get_ContourColor() const", focFUNCTION( BIND_CLASS Crit_get_ContourColor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "uint GetMultihex() const", focFUNCTION( BIND_CLASS Crit_GetMultihex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "bool IsTurnBasedTurn() const", focFUNCTION( BIND_CLASS Crit_IsTurnBasedTurn ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "CritterCl", "void GetNameTextInfo(bool& nameVisible, int& x, int& y, int& w, int& h, int& lines) const", focFUNCTION( BIND_CLASS Crit_GetNameTextInfo ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // DataRef
    // Reference value
    //

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_SERVER )
    {
        RegisterObjectMethod( engine, "DataRef", "const int& opIndex(uint) const", focFUNCTION( BIND_CLASS DataRef_Index ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectMethod( engine, "DataRef", "int& opIndex(uint)", focFUNCTION( BIND_CLASS DataRef_Index ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // DataVal
    // Computed value
    //
    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( app == APP_TYPE_CLIENT || app == APP_TYPE_SERVER )
    {
        RegisterObjectMethod( engine, "DataVal", "const int opIndex(uint) const", focFUNCTION( BIND_CLASS DataVal_Index ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // GameVar
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "GameVar@+ GetGlobalVar(uint16 varId)", focFUNCTION( BIND_CLASS Global_GetGlobalVar ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "GameVar@+ GetLocalVar(uint16 varId, uint masterId)", focFUNCTION( BIND_CLASS Global_GetLocalVar ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "GameVar@+ GetUnicumVar(uint16 varId, uint masterId, uint slaveId)", focFUNCTION( BIND_CLASS Global_GetUnicumVar ), asCALL_CDECL );

        RegisterObjectMethod( engine, "GameVar", "int GetValue() const", focMETHOD( GameVar, GetValue ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "int GetMin() const", focMETHOD( GameVar, GetMin ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "int GetMax() const", focMETHOD( GameVar, GetMax ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "bool IsQuest() const", focMETHOD( GameVar, IsQuest ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "uint GetQuestStr() const", focMETHOD( GameVar, GetQuestStr ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opAddAssign(const int)", focMETHODPR( GameVar, operator+=, (const int), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opSubAssign(const int)", focMETHODPR( GameVar, operator-=, (const int), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opMulAssign(const int)", focMETHODPR( GameVar, operator*=, (const int), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opDivAssign(const int)", focMETHODPR( GameVar, operator/=, (const int), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opAssign(const int)", focMETHODPR( GameVar, operator=, (const int), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opAddAssign(const GameVar&)", focMETHODPR( GameVar, operator+=, (const GameVar &), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opSubAssign(const GameVar&)", focMETHODPR( GameVar, operator-=, (const GameVar &), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opMulAssign(const GameVar&)", focMETHODPR( GameVar, operator*=, (const GameVar &), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opDivAssign(const GameVar&)", focMETHODPR( GameVar, operator/=, (const GameVar &), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "GameVar& opAssign(const GameVar&)", focMETHODPR( GameVar, operator=, (const GameVar &), GameVar& ), asCALL_THISCALL );
        RegisterObjectMethod( engine, "GameVar", "int opAdd(const int)", focFUNCTION( GameVarAddInt ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opSub(const int)", focFUNCTION( GameVarSubInt ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opMul(const int)", focFUNCTION( GameVarMulInt ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opDiv(const int)", focFUNCTION( GameVarDivInt ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opAdd(const GameVar&)", focFUNCTION( GameVarAddGameVar ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opSub(const GameVar&)", focFUNCTION( GameVarSubGameVar ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opMul(const GameVar&)", focFUNCTION( GameVarMulGameVar ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opDiv(const GameVar&)", focFUNCTION( GameVarDivGameVar ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "bool opEquals(const int)", focFUNCTION( GameVarEqualInt ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opCmp(const int)", focFUNCTION( GameVarCmpInt ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "bool opEquals(const GameVar&)", focFUNCTION( GameVarEqualGameVar ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "GameVar", "int opCmp(const GameVar&)", focFUNCTION( GameVarCmpGameVar ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // Item
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "uint GetAllItems(uint16 pid, Item@[]@+ items)", focFUNCTION( BIND_CLASS Global_GetAllItems ), asCALL_CDECL );

        RegisterGlobalFunction( engine, "Item@+ GetItem(uint itemId)", focFUNCTION( BIND_CLASS Global_GetItem ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveItem(Item& item, uint count, Critter& toCr)", focFUNCTION( BIND_CLASS Global_MoveItemCr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveItem(Item& item, uint count, Item& toCont, uint stackId)", focFUNCTION( BIND_CLASS Global_MoveItemCont ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveItem(Item& item, uint count, Map& toMap, uint16 toHx, uint16 toHy)", focFUNCTION( BIND_CLASS Global_MoveItemMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveItems(Item@[]& items, Critter& toCr)", focFUNCTION( BIND_CLASS Global_MoveItemsCr ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveItems(Item@[]& items, Item& toCont, uint stackId)", focFUNCTION( BIND_CLASS Global_MoveItemsCont ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void MoveItems(Item@[]& items, Map& toMap, uint16 toHx, uint16 toHy)", focFUNCTION( BIND_CLASS Global_MoveItemsMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DeleteItem(Item& item)", focFUNCTION( BIND_CLASS Global_DeleteItem ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DeleteItems(Item@[]& items)", focFUNCTION( BIND_CLASS Global_DeleteItems ), asCALL_CDECL );

        RegisterObjectProperty( engine, "Item", "const uint Id", focOFFSET( Item, Id ) );
        RegisterObjectProperty( engine, "Item", "const ProtoItem@ Proto", focOFFSET( Item, Proto ) );
        RegisterObjectProperty( engine, "Item", "const uint8 Accessory", focOFFSET( Item, Accessory ) );
        RegisterObjectProperty( engine, "Item", "const uint MapId", focOFFSET( Item, AccHex.MapId ) );
        RegisterObjectProperty( engine, "Item", "const uint16 HexX", focOFFSET( Item, AccHex.HexX ) );
        RegisterObjectProperty( engine, "Item", "const uint16 HexY", focOFFSET( Item, AccHex.HexY ) );
        RegisterObjectProperty( engine, "Item", "const uint CritId", focOFFSET( Item, AccCritter.Id ) );
        RegisterObjectProperty( engine, "Item", "const uint8 CritSlot", focOFFSET( Item, AccCritter.Slot ) );
        RegisterObjectProperty( engine, "Item", "const uint ContainerId", focOFFSET( Item, AccContainer.ContainerId ) );
        RegisterObjectProperty( engine, "Item", "const uint StackId", focOFFSET( Item, AccContainer.StackId ) );
        RegisterObjectProperty( engine, "Item", "const bool IsNotValid", focOFFSET( Item, IsNotValid ) );
        RegisterObjectProperty( engine, "Item", "const uint8 Mode", focOFFSET( Item, Data.Mode ) );

        RegisterObjectProperty( engine, "Item", "uint16 SortValue", focOFFSET( Item, Data.SortValue ) );
        RegisterObjectProperty( engine, "Item", "uint8 Info", focOFFSET( Item, Data.Info ) );
        RegisterObjectProperty( engine, "Item", "uint PicMap", focOFFSET( Item, Data.PicMapHash ) );
        RegisterObjectProperty( engine, "Item", "uint PicInv", focOFFSET( Item, Data.PicInvHash ) );
        RegisterObjectProperty( engine, "Item", "uint16 AnimWaitBase", focOFFSET( Item, Data.AnimWaitBase ) );
        RegisterObjectProperty( engine, "Item", "uint8 AnimStayBegin", focOFFSET( Item, Data.AnimStay[0] ) );
        RegisterObjectProperty( engine, "Item", "uint8 AnimStayEnd", focOFFSET( Item, Data.AnimStay[1] ) );
        RegisterObjectProperty( engine, "Item", "uint8 AnimShowBegin", focOFFSET( Item, Data.AnimShow[0] ) );
        RegisterObjectProperty( engine, "Item", "uint8 AnimShowEnd", focOFFSET( Item, Data.AnimShow[1] ) );
        RegisterObjectProperty( engine, "Item", "uint8 AnimHideBegin", focOFFSET( Item, Data.AnimHide[0] ) );
        RegisterObjectProperty( engine, "Item", "uint8 AnimHideEnd", focOFFSET( Item, Data.AnimHide[1] ) );
        RegisterObjectProperty( engine, "Item", "uint Cost", focOFFSET( Item, Data.Cost ) );
        RegisterObjectProperty( engine, "Item", "int Val0", focOFFSET( Item, Data.ScriptValues[0] ) );
        RegisterObjectProperty( engine, "Item", "int Val1", focOFFSET( Item, Data.ScriptValues[1] ) );
        RegisterObjectProperty( engine, "Item", "int Val2", focOFFSET( Item, Data.ScriptValues[2] ) );
        RegisterObjectProperty( engine, "Item", "int Val3", focOFFSET( Item, Data.ScriptValues[3] ) );
        RegisterObjectProperty( engine, "Item", "int Val4", focOFFSET( Item, Data.ScriptValues[4] ) );
        RegisterObjectProperty( engine, "Item", "int Val5", focOFFSET( Item, Data.ScriptValues[5] ) );
        RegisterObjectProperty( engine, "Item", "int Val6", focOFFSET( Item, Data.ScriptValues[6] ) );
        RegisterObjectProperty( engine, "Item", "int Val7", focOFFSET( Item, Data.ScriptValues[7] ) );
        RegisterObjectProperty( engine, "Item", "int Val8", focOFFSET( Item, Data.ScriptValues[8] ) );
        RegisterObjectProperty( engine, "Item", "int Val9", focOFFSET( Item, Data.ScriptValues[9] ) );
        RegisterObjectProperty( engine, "Item", "int8 LightIntensity", focOFFSET( Item, Data.LightIntensity ) );
        RegisterObjectProperty( engine, "Item", "uint8 LightDistance", focOFFSET( Item, Data.LightDistance ) );
        RegisterObjectProperty( engine, "Item", "uint8 LightFlags", focOFFSET( Item, Data.LightFlags ) );
        RegisterObjectProperty( engine, "Item", "uint LightColor", focOFFSET( Item, Data.LightColor ) );
        RegisterObjectProperty( engine, "Item", "uint8 Indicator", focOFFSET( Item, Data.Indicator ) );
        RegisterObjectProperty( engine, "Item", "uint8 BrokenFlags", focOFFSET( Item, Data.BrokenFlags ) );
        RegisterObjectProperty( engine, "Item", "uint8 BrokenCount", focOFFSET( Item, Data.BrokenCount ) );
        RegisterObjectProperty( engine, "Item", "uint16 Deterioration", focOFFSET( Item, Data.Deterioration ) );
        RegisterObjectProperty( engine, "Item", "uint16 AmmoPid", focOFFSET( Item, Data.AmmoPid ) );
        RegisterObjectProperty( engine, "Item", "uint16 AmmoCount", focOFFSET( Item, Data.AmmoCount ) );
        RegisterObjectProperty( engine, "Item", "uint LockerId", focOFFSET( Item, Data.LockerId ) );
        RegisterObjectProperty( engine, "Item", "uint16 LockerCondition", focOFFSET( Item, Data.LockerCondition ) );
        RegisterObjectProperty( engine, "Item", "uint16 LockerComplexity", focOFFSET( Item, Data.LockerComplexity ) );
        RegisterObjectProperty( engine, "Item", "uint16 Charge", focOFFSET( Item, Data.Charge ) );
        RegisterObjectProperty( engine, "Item", "uint16 RadioChannel", focOFFSET( Item, Data.RadioChannel ) );
        RegisterObjectProperty( engine, "Item", "uint16 RadioFlags", focOFFSET( Item, Data.RadioFlags ) );
        RegisterObjectProperty( engine, "Item", "uint8 RadioBroadcastSend", focOFFSET( Item, Data.RadioBroadcastSend ) );
        RegisterObjectProperty( engine, "Item", "uint8 RadioBroadcastRecv", focOFFSET( Item, Data.RadioBroadcastRecv ) );
        RegisterObjectProperty( engine, "Item", "uint HolodiskNumber", focOFFSET( Item, Data.HolodiskNumber ) );
        RegisterObjectProperty( engine, "Item", "int16 OffsetX", focOFFSET( Item, Data.OffsetX ) );
        RegisterObjectProperty( engine, "Item", "int16 OffsetY", focOFFSET( Item, Data.OffsetY ) );
        RegisterObjectProperty( engine, "Item", "int16 Dir", focOFFSET( Item, Data.Dir ) );

        RegisterObjectMethod( engine, "Item", "void set_Flags(uint value)", focFUNCTION( BIND_CLASS Item_set_Flags ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "uint get_Flags() const", focFUNCTION( BIND_CLASS Item_get_Flags ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void set_TrapValue(int16 val)", focFUNCTION( BIND_CLASS Item_set_TrapValue ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "int16 get_TrapValue() const", focFUNCTION( BIND_CLASS Item_get_TrapValue ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Item", "bool LockerOpen()", focFUNCTION( BIND_CLASS Item_LockerOpen ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool LockerClose()", focFUNCTION( BIND_CLASS Item_LockerClose ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Item", "bool IsStackable() const", focFUNCTION( BIND_CLASS Item_IsStackable ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool IsDeteriorable() const", focFUNCTION( BIND_CLASS Item_IsDeteriorable ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Item_SetScript ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Item_GetScriptId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Item_SetEvent ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "uint8 GetType() const", focFUNCTION( BIND_CLASS Item_GetType ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Item_GetProtoId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "uint GetCount() const", focFUNCTION( BIND_CLASS Item_GetCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void SetCount(uint count)", focFUNCTION( BIND_CLASS Item_SetCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "uint GetCost() const", focFUNCTION( BIND_CLASS Item_GetCost ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "Item@+ AddItem(uint16 protoId, uint count, uint specialId)", focFUNCTION( BIND_CLASS Container_AddItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "Item@+ GetItem(uint16 protoId, uint specialId) const", focFUNCTION( BIND_CLASS Container_GetItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "uint GetItems(uint specialId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Container_GetItems ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "Map@+ GetMapPosition(uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Item_GetMapPosition ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool ChangeProto(uint16 protoId) const", focFUNCTION( BIND_CLASS Item_ChangeProto ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void Update() const", focFUNCTION( BIND_CLASS Item_Update ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void Animate(uint8 fromFrame, uint8 toFrame)", focFUNCTION( BIND_CLASS Item_Animate ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void SetLexems(string@+ lexems)", focFUNCTION( BIND_CLASS Item_SetLexems ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "Item@+ GetChild(uint childIndex) const", focFUNCTION( BIND_CLASS Item_GetChild ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Item", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Item_EventFinish ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool EventAttack(Critter& attacker, Critter& target)", focFUNCTION( BIND_CLASS Item_EventAttack ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool EventUse(Critter& cr, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Item_EventUse ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool EventUseOnMe(Critter& cr, Item@+ usedItem)", focFUNCTION( BIND_CLASS Item_EventUseOnMe ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "bool EventSkill(Critter& cr, int skill)", focFUNCTION( BIND_CLASS Item_EventSkill ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void EventDrop(Critter& cr)", focFUNCTION( BIND_CLASS Item_EventDrop ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void EventMove(Critter& cr, uint8 fromSlot)", focFUNCTION( BIND_CLASS Item_EventMove ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Item", "void EventWalk(Critter& cr, bool entered, uint8 dir)", focFUNCTION( BIND_CLASS Item_EventWalk ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // ItemCl
    //

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( app == APP_TYPE_CLIENT )
    {
        RegisterGlobalFunction( engine, "ItemCl@+ GetItem(uint itemId)", focFUNCTION( BIND_CLASS Global_GetItem ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "ItemCl@+ GetMonitorItem(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorItem ), asCALL_CDECL );

        RegisterObjectProperty( engine, "ItemCl", "const uint Id", focOFFSET( Item, Id ) );
        RegisterObjectProperty( engine, "ItemCl", "const ProtoItem@ Proto", focOFFSET( Item, Proto ) );
        RegisterObjectProperty( engine, "ItemCl", "const string Lexems", focOFFSET( Item, Lexems ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 Accessory", focOFFSET( Item, Accessory ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint MapId", focOFFSET( Item, AccHex.MapId ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 HexX", focOFFSET( Item, AccHex.HexX ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 HexY", focOFFSET( Item, AccHex.HexY ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint CritId", focOFFSET( Item, AccCritter.Id ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 CritSlot", focOFFSET( Item, AccCritter.Slot ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint ContainerId", focOFFSET( Item, AccContainer.ContainerId ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint StackId", focOFFSET( Item, AccContainer.StackId ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 SortValue", focOFFSET( Item, Data.SortValue ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 Info", focOFFSET( Item, Data.Info ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint PicMap", focOFFSET( Item, Data.PicMapHash ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint PicInv", focOFFSET( Item, Data.PicInvHash ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 AnimWaitBase", focOFFSET( Item, Data.AnimWaitBase ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 AnimStayBegin", focOFFSET( Item, Data.AnimStay[0] ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 AnimStayEnd", focOFFSET( Item, Data.AnimStay[1] ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 AnimShowBegin", focOFFSET( Item, Data.AnimShow[0] ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 AnimShowEnd", focOFFSET( Item, Data.AnimShow[1] ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 AnimHideBegin", focOFFSET( Item, Data.AnimHide[0] ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 AnimHideEnd", focOFFSET( Item, Data.AnimHide[1] ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint Flags", focOFFSET( Item, Data.Flags ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 Mode", focOFFSET( Item, Data.Mode ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val0", focOFFSET( Item, Data.ScriptValues[0] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val1", focOFFSET( Item, Data.ScriptValues[1] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val2", focOFFSET( Item, Data.ScriptValues[2] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val3", focOFFSET( Item, Data.ScriptValues[3] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val4", focOFFSET( Item, Data.ScriptValues[4] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val5", focOFFSET( Item, Data.ScriptValues[5] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val6", focOFFSET( Item, Data.ScriptValues[6] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val7", focOFFSET( Item, Data.ScriptValues[7] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val8", focOFFSET( Item, Data.ScriptValues[8] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int Val9", focOFFSET( Item, Data.ScriptValues[9] ) );
        RegisterObjectProperty( engine, "ItemCl", "const int8 LightIntensity", focOFFSET( Item, Data.LightIntensity ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 LightDistance", focOFFSET( Item, Data.LightDistance ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 LightFlags", focOFFSET( Item, Data.LightFlags ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint LightColor", focOFFSET( Item, Data.LightColor ) );
        RegisterObjectProperty( engine, "ItemCl", "const int16 TrapValue", focOFFSET( Item, Data.TrapValue ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 Indicator", focOFFSET( Item, Data.Indicator ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 BrokenFlags", focOFFSET( Item, Data.BrokenFlags ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 BrokenCount", focOFFSET( Item, Data.BrokenCount ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 Deterioration", focOFFSET( Item, Data.Deterioration ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 AmmoPid", focOFFSET( Item, Data.AmmoPid ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 AmmoCount", focOFFSET( Item, Data.AmmoCount ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint LockerId", focOFFSET( Item, Data.LockerId ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 LockerCondition", focOFFSET( Item, Data.LockerCondition ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 LockerComplexity", focOFFSET( Item, Data.LockerComplexity ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 Charge", focOFFSET( Item, Data.Charge ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 RadioChannel", focOFFSET( Item, Data.RadioChannel ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint16 RadioFlags", focOFFSET( Item, Data.RadioFlags ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 RadioBroadcastSend", focOFFSET( Item, Data.RadioBroadcastSend ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint8 RadioBroadcastRecv", focOFFSET( Item, Data.RadioBroadcastRecv ) );
        RegisterObjectProperty( engine, "ItemCl", "const uint HolodiskNumber", focOFFSET( Item, Data.HolodiskNumber ) );
        RegisterObjectProperty( engine, "ItemCl", "const int16 OffsetX", focOFFSET( Item, Data.OffsetX ) );
        RegisterObjectProperty( engine, "ItemCl", "const int16 OffsetY", focOFFSET( Item, Data.OffsetY ) );
        RegisterObjectProperty( engine, "ItemCl", "const int16 Dir", focOFFSET( Item, Data.Dir ) );

        RegisterObjectMethod( engine, "ItemCl", "bool IsStackable() const", focFUNCTION( BIND_CLASS Item_IsStackable ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "ItemCl", "bool IsDeteriorable() const", focFUNCTION( BIND_CLASS Item_IsDeteriorable ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "ItemCl", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Item_GetScriptId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "ItemCl", "uint8 GetType() const", focFUNCTION( BIND_CLASS Item_GetType ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "ItemCl", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Item_GetProtoId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "ItemCl", "uint GetCount() const", focFUNCTION( BIND_CLASS Item_GetCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "ItemCl", "bool GetMapPosition(uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Item_GetMapPosition ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "ItemCl", "void Animate(uint8 fromFrame, uint8 toFrame)", focFUNCTION( BIND_CLASS Item_Animate ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // Location
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "Location@+ GetLocation(uint locId)", focFUNCTION( BIND_CLASS Global_GetLocation ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "Location@+ GetLocationByPid(uint16 locPid, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetLocationByPid ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetLocations(uint16 worldX, uint16 worldY, uint radius, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetLocations ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetVisibleLocations(uint16 worldX, uint16 worldY, uint radius, Critter@+ visibleBy, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetVisibleLocations ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetZoneLocationIds(uint16 zoneX, uint16 zoneY, uint zoneRadius, uint[]@+ locationIds)", focFUNCTION( BIND_CLASS Global_GetZoneLocationIds ), asCALL_CDECL );

        RegisterObjectProperty( engine, "Location", "const uint Id", focOFFSET( Location, Data.LocId ) );
        RegisterObjectProperty( engine, "Location", "uint16 WorldX", focOFFSET( Location, Data.WX ) );
        RegisterObjectProperty( engine, "Location", "uint16 WorldY", focOFFSET( Location, Data.WY ) );
        RegisterObjectProperty( engine, "Location", "bool Visible", focOFFSET( Location, Data.Visible ) );
        RegisterObjectProperty( engine, "Location", "bool GeckVisible", focOFFSET( Location, Data.GeckVisible ) );
        RegisterObjectProperty( engine, "Location", "bool AutoGarbage", focOFFSET( Location, Data.AutoGarbage ) );
        RegisterObjectProperty( engine, "Location", "int GeckCount", focOFFSET( Location, GeckCount ) );
        RegisterObjectProperty( engine, "Location", "uint16 Radius", focOFFSET( Location, Data.Radius ) );
        RegisterObjectProperty( engine, "Location", "uint Color", focOFFSET( Location, Data.Color ) );
        RegisterObjectProperty( engine, "Location", "const bool IsNotValid", focOFFSET( Location, IsNotValid ) );

        RegisterGlobalFunction( engine, "uint CreateLocation(uint16 locPid, uint16 worldX, uint16 worldY, Critter@[]@+ critters)", focFUNCTION( BIND_CLASS Global_CreateLocation ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DeleteLocation(uint locId)", focFUNCTION( BIND_CLASS Global_DeleteLocation ), asCALL_CDECL );

        RegisterObjectMethod( engine, "Location", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Location_GetProtoId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Location_SetEvent ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "uint GetMapCount() const", focFUNCTION( BIND_CLASS Location_GetMapCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "Map@+ GetMap(uint16 mapPid) const", focFUNCTION( BIND_CLASS Location_GetMap ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "Map@+ GetMapByIndex(uint index) const", focFUNCTION( BIND_CLASS Location_GetMapByIndex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "uint GetMaps(Map@[]@+ maps) const", focFUNCTION( BIND_CLASS Location_GetMaps ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "bool Reload()", focFUNCTION( BIND_CLASS Location_Reload ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "void Update()", focFUNCTION( BIND_CLASS Location_Update ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Location", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Location_EventFinish ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Location", "bool EventEnter(Critter@[]& group, uint8 entrance)", focFUNCTION( BIND_CLASS Location_EventEnter ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    ///
    // Map
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "Map@+ GetMap(uint mapId)", focFUNCTION( BIND_CLASS Global_GetMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "Map@+ GetMapByPid(uint16 mapPid, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMapByPid ), asCALL_CDECL );

        RegisterObjectProperty( engine, "Map", "const bool IsNotValid", focOFFSET( Map, IsNotValid ) );
        RegisterObjectProperty( engine, "Map", "const uint Id", focOFFSET( Map, Data.MapId ) );
        // RegisterObjectProperty( engine, "Map", "bool CombatRunning", focOFFSET( Map, MapId ) );
        RegisterObjectProperty( engine, "Map", "const uint TurnBasedRound", focOFFSET( Map, TurnBasedRound ) );
        RegisterObjectProperty( engine, "Map", "const uint TurnBasedTurn", focOFFSET( Map, TurnBasedTurn ) );
        RegisterObjectProperty( engine, "Map", "const uint TurnBasedWholeTurn", focOFFSET( Map, TurnBasedWholeTurn ) );

        RegisterObjectMethod( engine, "Map", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Map_GetProtoId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Location@+ GetLocation() const", focFUNCTION( BIND_CLASS Map_GetLocation ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Map_SetScript ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Map_GetScriptId ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Map_SetEvent ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetLoopTime(uint numLoop, uint ms)", focFUNCTION( BIND_CLASS Map_SetLoopTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint8 GetRain() const", focFUNCTION( BIND_CLASS Map_GetRain ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetRain(uint8 capacity)", focFUNCTION( BIND_CLASS Map_SetRain ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "int GetTime() const", focFUNCTION( BIND_CLASS Map_GetTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetTime(int time)", focFUNCTION( BIND_CLASS Map_SetTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetDayTime(uint dayPart) const", focFUNCTION( BIND_CLASS Map_GetDayTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetDayTime(uint dayPart, uint time)", focFUNCTION( BIND_CLASS Map_SetDayTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b) const", focFUNCTION( BIND_CLASS Map_GetDayColor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetDayColor(uint dayPart, uint8 r, uint8 g, uint8 b)", focFUNCTION( BIND_CLASS Map_SetDayColor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetTurnBasedAvailability(bool value)", focFUNCTION( BIND_CLASS Map_SetTurnBasedAvailability ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool IsTurnBasedAvailability() const", focFUNCTION( BIND_CLASS Map_IsTurnBasedAvailability ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void BeginTurnBased(Critter@+ firstTurnCrit)", focFUNCTION( BIND_CLASS Map_BeginTurnBased ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool IsTurnBased() const", focFUNCTION( BIND_CLASS Map_IsTurnBased ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EndTurnBased()", focFUNCTION( BIND_CLASS Map_EndTurnBased ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "int GetTurnBasedSequence(uint[]& crittersIds) const", focFUNCTION( BIND_CLASS Map_GetTurnBasedSequence ), asCALL_CDECL_OBJFIRST );
        // RegisterObjectMethod( engine, "Map", "void SetTurnBasedSequence(uint[]& crittersIds)", focFUNCTION( BIND_CLASS Map_SetTurnBasedSequence ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetData(uint index, int value)", focFUNCTION( BIND_CLASS Map_SetData ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "int GetData(uint index) const", focFUNCTION( BIND_CLASS Map_GetData ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Item@+ AddItem(uint16 hexX, uint16 hexY, uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Map_AddItem ), asCALL_CDECL_OBJFIRST );
        // RegisterObjectMethod( engine, "Map", "Item@+ AddItemFromScenery(uint16 hexX, uint16 hexY, uint16 protoId)", focFUNCTION( BIND_CLASS Map_AddItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Item@+ GetItem(uint itemId) const", focFUNCTION( BIND_CLASS Map_GetItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Item@+ GetItem(uint16 hexX, uint16 hexY, uint16 protoId) const", focFUNCTION( BIND_CLASS Map_GetItemHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetItems(uint16 hexX, uint16 hexY, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetItems(uint16 hexX, uint16 hexY, uint radius, uint16 protoId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsHexEx ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetItems(uint16 protoId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsByPid ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetItemsByType(int type, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsByType ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Item@+ GetDoor(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetDoor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Item@+ GetCar(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetCar ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Scenery@+ GetScenery(uint16 hexX, uint16 hexY, uint16 protoId) const", focFUNCTION( BIND_CLASS Map_GetSceneryHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetSceneries(uint16 hexX, uint16 hexY, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetSceneries(uint16 hexX, uint16 hexY, uint radius, uint16 protoId, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesHexEx ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetSceneries(uint16 protoId, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesByPid ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Critter@+ GetCritter(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetCritterHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Critter@+ GetCritter(uint critterId) const", focFUNCTION( BIND_CLASS Map_GetCritterById ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetCrittersHex(uint16 hexX, uint16 hexY, uint radius, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCritters ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetCritters(uint16 pid, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersByPids ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersInPath ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, Critter@[]@+ critters, uint16& preBlockHx, uint16& preBlockHy, uint16& blockHx, uint16& blockHy) const", focFUNCTION( BIND_CLASS Map_GetCrittersInPathBlock ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetCrittersWhoViewPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersWhoViewPath ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetCrittersSeeing(Critter@[]& critters, bool lookOnThem, int find_type, Critter@[]@+ crittersResult) const", focFUNCTION( BIND_CLASS Map_GetCrittersSeeing ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist) const", focFUNCTION( BIND_CLASS Map_GetHexInPath ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void GetHexCoordWall(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist) const", focFUNCTION( BIND_CLASS Map_GetHexInPathWall ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut) const", focFUNCTION( BIND_CLASS Map_GetPathLengthHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetPathLength(Critter& cr, uint16 toHx, uint16 toHy, uint cut) const", focFUNCTION( BIND_CLASS Map_GetPathLengthCr ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool VerifyTrigger(Critter& cr, uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Map_VerifyTrigger ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Critter@+ AddNpc(uint16 protoId, uint16 hexX, uint16 hexY, uint8 dir, int[]@+ params, int[]@+ items, string@+ script)", focFUNCTION( BIND_CLASS Map_AddNpc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetNpcCount(int npcRole, int findType) const", focFUNCTION( BIND_CLASS Map_GetNpcCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "Critter@+ GetNpc(int npcRole, int findType, uint skipCount) const", focFUNCTION( BIND_CLASS Map_GetNpc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint CountEntire(int entire) const", focFUNCTION( BIND_CLASS Map_CountEntire ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint GetEntires(int entire, uint[]@+ entires, uint16[]@+ hexX, uint16[]@+ hexY) const", focFUNCTION( BIND_CLASS Map_GetEntires ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool GetEntireCoords(int entire, uint skip, uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Map_GetEntireCoords ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool GetEntireCoords(int entire, uint skip, uint16& hexX, uint16& hexY, uint8& dir) const", focFUNCTION( BIND_CLASS Map_GetEntireCoordsDir ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool GetNearEntireCoords(int& entire, uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Map_GetNearEntireCoords ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool GetNearEntireCoords(int& entire, uint16& hexX, uint16& hexY, uint8& dir) const", focFUNCTION( BIND_CLASS Map_GetNearEntireCoordsDir ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool IsHexPassed(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_IsHexPassed ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool IsHexRaked(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_IsHexRaked ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetText(uint16 hexX, uint16 hexY, uint color, string& text) const", focFUNCTION( BIND_CLASS Map_SetText ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetTextMsg(uint16 hexX, uint16 hexY, uint color, uint16 textMsg, uint strNum) const", focFUNCTION( BIND_CLASS Map_SetTextMsg ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void SetTextMsg(uint16 hexX, uint16 hexY, uint color, uint16 textMsg, uint strNum, string& lexems) const", focFUNCTION( BIND_CLASS Map_SetTextMsgLex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void RunEffect(uint16 effectPid, uint16 hexX, uint16 hexY, uint16 radius) const", focFUNCTION( BIND_CLASS Map_RunEffect ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void RunFlyEffect(uint16 effectPid, Critter@+ fromCr, Critter@+ toCr, uint16 fromX, uint16 fromY, uint16 toX, uint16 toY) const", focFUNCTION( BIND_CLASS Map_RunFlyEffect ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool CheckPlaceForItem(uint16 hexX, uint16 hexY, uint16 pid) const", focFUNCTION( BIND_CLASS Map_CheckPlaceForItem ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void BlockHex(uint16 hexX, uint16 hexY, bool full)", focFUNCTION( BIND_CLASS Map_BlockHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void UnblockHex(uint16 hexX, uint16 hexY)", focFUNCTION( BIND_CLASS Map_UnblockHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void PlaySound(string& soundName) const", focFUNCTION( BIND_CLASS Map_PlaySound ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void PlaySound(string& soundName, uint16 hexX, uint16 hexY, uint radius) const", focFUNCTION( BIND_CLASS Map_PlaySoundRadius ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "bool Reload()", focFUNCTION( BIND_CLASS Map_Reload ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint16 GetWidth() const", focFUNCTION( BIND_CLASS Map_GetWidth ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "uint16 GetHeight() const", focFUNCTION( BIND_CLASS Map_GetHeight ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps) const", focFUNCTION( BIND_CLASS Map_MoveHexByDir ), asCALL_CDECL_OBJFIRST );

        RegisterObjectMethod( engine, "Map", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Map_EventFinish ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventLoop0()", focFUNCTION( BIND_CLASS Map_EventLoop0 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventLoop1()", focFUNCTION( BIND_CLASS Map_EventLoop1 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventLoop2()", focFUNCTION( BIND_CLASS Map_EventLoop2 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventLoop3()", focFUNCTION( BIND_CLASS Map_EventLoop3 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventLoop4()", focFUNCTION( BIND_CLASS Map_EventLoop4 ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventInCritter(Critter& cr)", focFUNCTION( BIND_CLASS Map_EventInCritter ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventOutCritter(Critter& cr)", focFUNCTION( BIND_CLASS Map_EventOutCritter ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventCritterDead(Critter& cr, Critter@+ killer)", focFUNCTION( BIND_CLASS Map_EventCritterDead ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventTurnBasedBegin()", focFUNCTION( BIND_CLASS Map_EventTurnBasedBegin ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventTurnBasedEnd()", focFUNCTION( BIND_CLASS Map_EventTurnBasedEnd ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "Map", "void EventTurnBasedProcess(Critter& cr, bool beginTurn)", focFUNCTION( BIND_CLASS Map_EventTurnBasedProcess ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // MapperMap
    //

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( app == APP_TYPE_MAPPER )
    {
        RegisterGlobalFunction( engine, "MapperMap@+ LoadMap(string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_LoadMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void UnloadMap(MapperMap@+ map)", focFUNCTION( BIND_CLASS Global_UnloadMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool SaveMap(MapperMap@+ map, string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_SaveMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "bool ShowMap(MapperMap@+ map)", focFUNCTION( BIND_CLASS Global_ShowMap ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "int GetLoadedMaps(MapperMap@[]@+ maps)", focFUNCTION( BIND_CLASS Global_GetLoadedMaps ), asCALL_CDECL );

        RegisterObjectProperty( engine, "MapperMap", "const uint16 Width", focOFFSET( ProtoMap, Header.MaxHexX ) );
        RegisterObjectProperty( engine, "MapperMap", "const uint16 Height", focOFFSET( ProtoMap, Header.MaxHexY ) );
        RegisterObjectProperty( engine, "MapperMap", "const int WorkHexX", focOFFSET( ProtoMap, Header.WorkHexX ) );
        RegisterObjectProperty( engine, "MapperMap", "const int WorkHexY", focOFFSET( ProtoMap, Header.WorkHexY ) );
        RegisterObjectProperty( engine, "MapperMap", "int Time", focOFFSET( ProtoMap, Header.Time ) );
        RegisterObjectProperty( engine, "MapperMap", "bool NoLogOut", focOFFSET( ProtoMap, Header.NoLogOut ) );

        RegisterObjectMethod( engine, "MapperMap", "MapperObject@+ AddObject(uint16 hexX, uint16 hexY, int mapObjType, uint16 pid)", focFUNCTION( BIND_CLASS MapperMap_AddObject ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "MapperObject@+ GetObject(uint16 hexX, uint16 hexY, int mapObjType, uint16 pid, uint skip) const", focFUNCTION( BIND_CLASS MapperMap_GetObject ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "uint GetObjects(uint16 hexX, uint16 hexY, uint radius, int mapObjType, uint16 pid, MapperObject@[]@+ objects) const", focFUNCTION( BIND_CLASS MapperMap_GetObjects ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void UpdateObjects() const", focFUNCTION( BIND_CLASS MapperMap_UpdateObjects ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void Resize(uint16 width, uint16 height)", focFUNCTION( BIND_CLASS MapperMap_Resize ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "uint GetTilesCount(uint16 hexX, uint16 hexY, bool roof) const", focFUNCTION( BIND_CLASS MapperMap_GetTilesCount ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void DeleteTile(uint16 hexX, uint16 hexY, bool roof, uint index)", focFUNCTION( BIND_CLASS MapperMap_DeleteTile ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "uint GetTile(uint16 hexX, uint16 hexY, bool roof, uint index) const", focFUNCTION( BIND_CLASS MapperMap_GetTileHash ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void AddTile(uint16 hexX, uint16 hexY, int offsX, int offsY, int layer, bool roof, uint picHash)", focFUNCTION( BIND_CLASS MapperMap_AddTileHash ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "string@ GetTileName(uint16 hexX, uint16 hexY, bool roof, uint index) const", focFUNCTION( BIND_CLASS MapperMap_GetTileName ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void AddTileName(uint16 hexX, uint16 hexY, int offsX, int offsY, int layer, bool roof, string@+ picName)", focFUNCTION( BIND_CLASS MapperMap_AddTileName ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "uint GetDayTime(uint dayPart) const", focFUNCTION( BIND_CLASS MapperMap_GetDayTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void SetDayTime(uint dayPart, uint time)", focFUNCTION( BIND_CLASS MapperMap_SetDayTime ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b) const", focFUNCTION( BIND_CLASS MapperMap_GetDayColor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void SetDayColor(uint dayPart, uint8 r, uint8 g, uint8 b)", focFUNCTION( BIND_CLASS MapperMap_SetDayColor ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "string@ get_ScriptModule() const", focFUNCTION( BIND_CLASS MapperMap_get_ScriptModule ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void set_ScriptModule(const string& name)", focFUNCTION( BIND_CLASS MapperMap_set_ScriptModule ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "string@ get_ScriptFunc() const", focFUNCTION( BIND_CLASS MapperMap_get_ScriptFunc ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperMap", "void set_ScriptFunc(const string& name)", focFUNCTION( BIND_CLASS MapperMap_set_ScriptFunc ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // MapperObject
    //

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( app == APP_TYPE_MAPPER )
    {
        RegisterGlobalFunction( engine, "void DeleteObject(MapperObject@+ obj)", focFUNCTION( BIND_CLASS Global_DeleteObject ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void DeleteObjects(MapperObject@[]& objects)", focFUNCTION( BIND_CLASS Global_DeleteObjects ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SelectObject(MapperObject@+ obj, bool set)", focFUNCTION( BIND_CLASS Global_SelectObject ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void SelectObjects(MapperObject@[]& objects, bool set)", focFUNCTION( BIND_CLASS Global_SelectObjects ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "MapperObject@+ GetSelectedObject()", focFUNCTION( BIND_CLASS Global_GetSelectedObject ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "uint GetSelectedObjects(MapperObject@[]@+ objects)", focFUNCTION( BIND_CLASS Global_GetSelectedObjects ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "MapperObject@+ GetMonitorObject(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorObject ), asCALL_CDECL );

        // Generic
        RegisterObjectProperty( engine, "MapperObject", "const uint8 MapObjType", focOFFSET( MapObject, MapObjType ) );
        RegisterObjectProperty( engine, "MapperObject", "const uint16 ProtoId", focOFFSET( MapObject, ProtoId ) );
        RegisterObjectProperty( engine, "MapperObject", "const uint16 MapX", focOFFSET( MapObject, MapX ) );
        RegisterObjectProperty( engine, "MapperObject", "const uint16 MapY", focOFFSET( MapObject, MapY ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Dir", focOFFSET( MapObject, Dir ) );
        RegisterObjectProperty( engine, "MapperObject", "const uint UID", focOFFSET( MapObject, UID ) );
        RegisterObjectProperty( engine, "MapperObject", "const uint ContainerUID", focOFFSET( MapObject, ContainerUID ) );
        RegisterObjectProperty( engine, "MapperObject", "const uint ParentUID", focOFFSET( MapObject, ParentUID ) );
        RegisterObjectProperty( engine, "MapperObject", "const uint ParentChildIndex", focOFFSET( MapObject, ParentChildIndex ) );
        RegisterObjectProperty( engine, "MapperObject", "uint LightColor", focOFFSET( MapObject, LightColor ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 LightDay", focOFFSET( MapObject, LightDay ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 LightDirOff", focOFFSET( MapObject, LightDirOff ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 LightDistance", focOFFSET( MapObject, LightDistance ) );
        RegisterObjectProperty( engine, "MapperObject", "int8 LightIntensity", focOFFSET( MapObject, LightIntensity ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData0", focOFFSET( MapObject, UserData[0] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData1", focOFFSET( MapObject, UserData[1] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData2", focOFFSET( MapObject, UserData[2] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData3", focOFFSET( MapObject, UserData[3] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData4", focOFFSET( MapObject, UserData[4] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData5", focOFFSET( MapObject, UserData[5] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData6", focOFFSET( MapObject, UserData[6] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData7", focOFFSET( MapObject, UserData[7] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData8", focOFFSET( MapObject, UserData[8] ) );
        RegisterObjectProperty( engine, "MapperObject", "int UserData9", focOFFSET( MapObject, UserData[9] ) );

        // Critter
        RegisterObjectProperty( engine, "MapperObject", "uint8 Critter_Anim1", focOFFSET( MapObject, MCritter.Anim1 ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Critter_Anim2", focOFFSET( MapObject, MCritter.Anim2 ) );
        for( int i = 0; i < 40 /*MAPOBJ_CRITTER_PARAMS*/; i++ )
        {
            char str[256];
            sprintf( str, "int16 Critter_ParamIndex%d", i );
            RegisterObjectProperty( engine, "MapperObject", str, focOFFSET( MapObject, MCritter.ParamIndex[0] ) + sizeof(short) * i );

            sprintf( str, "int Critter_ParamValue%d", i );
            RegisterObjectProperty( engine, "MapperObject", str, focOFFSET( MapObject, MCritter.ParamValue[0] ) + sizeof(int) * i );

        }

        // Item/Critter shared parameters
        RegisterObjectProperty( engine, "MapperObject", "int16 OffsetX", focOFFSET( MapObject, MItem.OffsetX ) );
        RegisterObjectProperty( engine, "MapperObject", "int16 OffsetY", focOFFSET( MapObject, MItem.OffsetY ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 AnimStayBegin", focOFFSET( MapObject, MItem.AnimStayBegin ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 AnimStayEnd", focOFFSET( MapObject, MItem.AnimStayEnd ) );
        RegisterObjectProperty( engine, "MapperObject", "uint16 AnimWait", focOFFSET( MapObject, MItem.AnimWait ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 InfoOffset", focOFFSET( MapObject, MItem.InfoOffset ) );
        RegisterObjectMethod( engine, "MapperObject", "string@ get_PicMap() const", focFUNCTION( BIND_CLASS MapperObject_get_PicMap ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void set_PicMap(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_PicMap ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "string@ get_PicInv() const", focFUNCTION( BIND_CLASS MapperObject_get_PicInv ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void set_PicInv(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_PicInv ), asCALL_CDECL_OBJFIRST );

        // Item
        RegisterObjectProperty( engine, "MapperObject", "uint Item_Count", focOFFSET( MapObject, MItem.Count ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Item_BrokenFlags", focOFFSET( MapObject, MItem.BrokenFlags ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Item_BrokenCount", focOFFSET( MapObject, MItem.BrokenCount ) );
        RegisterObjectProperty( engine, "MapperObject", "uint16 Item_Deterioration", focOFFSET( MapObject, MItem.Deterioration ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Item_ItemSlot", focOFFSET( MapObject, MItem.ItemSlot ) );
        RegisterObjectProperty( engine, "MapperObject", "uint16 Item_AmmoPid", focOFFSET( MapObject, MItem.AmmoPid ) );
        RegisterObjectProperty( engine, "MapperObject", "uint Item_AmmoCount", focOFFSET( MapObject, MItem.AmmoCount ) );
        RegisterObjectProperty( engine, "MapperObject", "uint Item_LockerDoorId", focOFFSET( MapObject, MItem.LockerDoorId ) );
        RegisterObjectProperty( engine, "MapperObject", "uint16 Item_LockerCondition", focOFFSET( MapObject, MItem.LockerCondition ) );
        RegisterObjectProperty( engine, "MapperObject", "uint16 Item_LockerComplexity", focOFFSET( MapObject, MItem.LockerComplexity ) );
        RegisterObjectProperty( engine, "MapperObject", "int16 Item_TrapValue", focOFFSET( MapObject, MItem.TrapValue ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val0", focOFFSET( MapObject, MItem.Val[0] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val1", focOFFSET( MapObject, MItem.Val[1] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val2", focOFFSET( MapObject, MItem.Val[2] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val3", focOFFSET( MapObject, MItem.Val[3] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val4", focOFFSET( MapObject, MItem.Val[4] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val5", focOFFSET( MapObject, MItem.Val[5] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val6", focOFFSET( MapObject, MItem.Val[6] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val7", focOFFSET( MapObject, MItem.Val[7] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val8", focOFFSET( MapObject, MItem.Val[8] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Item_Val9", focOFFSET( MapObject, MItem.Val[9] ) );

        // Scenery
        RegisterObjectProperty( engine, "MapperObject", "bool Scenery_CanUse", focOFFSET( MapObject, MScenery.CanUse ) );
        RegisterObjectProperty( engine, "MapperObject", "bool Scenery_CanTalk", focOFFSET( MapObject, MScenery.CanTalk ) );
        RegisterObjectProperty( engine, "MapperObject", "uint Scenery_TriggerNum", focOFFSET( MapObject, MScenery.TriggerNum ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Scenery_ParamsCount", focOFFSET( MapObject, MScenery.ParamsCount ) );
        RegisterObjectProperty( engine, "MapperObject", "int Scenery_Param0", focOFFSET( MapObject, MScenery.Param[0] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Scenery_Param1", focOFFSET( MapObject, MScenery.Param[1] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Scenery_Param2", focOFFSET( MapObject, MScenery.Param[2] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Scenery_Param3", focOFFSET( MapObject, MScenery.Param[3] ) );
        RegisterObjectProperty( engine, "MapperObject", "int Scenery_Param4", focOFFSET( MapObject, MScenery.Param[4] ) );
        RegisterObjectProperty( engine, "MapperObject", "uint16 Scenery_ToMapPid", focOFFSET( MapObject, MScenery.ToMapPid ) );
        RegisterObjectProperty( engine, "MapperObject", "uint Scenery_ToEntire", focOFFSET( MapObject, MScenery.ToEntire ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Scenery_ToDir", focOFFSET( MapObject, MScenery.ToDir ) );
        RegisterObjectProperty( engine, "MapperObject", "uint8 Scenery_SpriteCut", focOFFSET( MapObject, MScenery.SpriteCut ) );

        //

        RegisterObjectMethod( engine, "MapperObject", "void Update() const", focFUNCTION( BIND_CLASS MapperObject_Update ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "MapperObject@+ AddChild(uint16 pid)", focFUNCTION( BIND_CLASS MapperObject_AddChild ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "uint GetChilds(MapperObject@[]@+ objects) const", focFUNCTION( BIND_CLASS MapperObject_GetChilds ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "string@ get_ScriptName() const", focFUNCTION( BIND_CLASS MapperObject_get_ScriptName ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void set_ScriptName(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_ScriptName ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "string@ get_FuncName() const", focFUNCTION( BIND_CLASS MapperObject_get_FuncName ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void set_FuncName(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_FuncName ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "uint8 get_Critter_Cond() const", focFUNCTION( BIND_CLASS MapperObject_get_Critter_Cond ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void set_Critter_Cond(uint8 value)", focFUNCTION( BIND_CLASS MapperObject_set_Critter_Cond ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void MoveToHex(uint16 hexX, uint16 hexY)", focFUNCTION( BIND_CLASS MapperObject_MoveToHex ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void MoveToHexOffset(int x, int y)", focFUNCTION( BIND_CLASS MapperObject_MoveToHexOffset ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "MapperObject", "void MoveToDir(uint8 dir)", focFUNCTION( BIND_CLASS MapperObject_MoveToDir ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // NpcPlane
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "NpcPlane@ CreatePlane()", focFUNCTION( BIND_CLASS Global_CreatePlane ), asCALL_CDECL );

        RegisterObjectProperty( engine, "NpcPlane", "int Type", focOFFSET( AIDataPlane, Type ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint Priority", focOFFSET( AIDataPlane, Priority ) );
        // RegisterObjectProperty( engine, "NpcPlane", "NpcPlane@ Child", focOFFSET( AIDataPlane, Type ) );
        RegisterObjectProperty( engine, "NpcPlane", "int Identifier", focOFFSET( AIDataPlane, Identifier ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint IdentifierExt", focOFFSET( AIDataPlane, IdentifierExt ) );
        RegisterObjectProperty( engine, "NpcPlane", "bool Run", focOFFSET( AIDataPlane, Pick.IsRun ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint Misc_WaitSecond", focOFFSET( AIDataPlane, Misc.WaitSecond ) );
        RegisterObjectProperty( engine, "NpcPlane", "int Misc_ScriptId", focOFFSET( AIDataPlane, Misc.ScriptBindId ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint Attack_TargId", focOFFSET( AIDataPlane, Attack.TargId ) );
        RegisterObjectProperty( engine, "NpcPlane", "int Attack_MinHp", focOFFSET( AIDataPlane, Attack.MinHp ) );
        RegisterObjectProperty( engine, "NpcPlane", "bool Attack_IsGag", focOFFSET( AIDataPlane, Attack.IsGag ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Attack_GagHexX", focOFFSET( AIDataPlane, Attack.GagHexX ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Attack_GagHexY", focOFFSET( AIDataPlane, Attack.GagHexY ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Attack_LastHexX", focOFFSET( AIDataPlane, Attack.LastHexX ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Attack_LastHexY", focOFFSET( AIDataPlane, Attack.LastHexY ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Walk_HexX", focOFFSET( AIDataPlane, Walk.HexX ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Walk_HexY", focOFFSET( AIDataPlane, Walk.HexY ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint8 Walk_Dir", focOFFSET( AIDataPlane, Walk.Dir ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint Walk_Cut", focOFFSET( AIDataPlane, Walk.Cut ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Pick_HexX", focOFFSET( AIDataPlane, Pick.HexX ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Pick_HexY", focOFFSET( AIDataPlane, Pick.HexY ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint16 Pick_Pid", focOFFSET( AIDataPlane, Pick.Pid ) );
        RegisterObjectProperty( engine, "NpcPlane", "uint Pick_UseItemId", focOFFSET( AIDataPlane, Pick.UseItemId ) );
        RegisterObjectProperty( engine, "NpcPlane", "bool Pick_ToOpen", focOFFSET( AIDataPlane, Pick.ToOpen ) );

        RegisterObjectMethod( engine, "NpcPlane", "NpcPlane@ GetCopy() const", focFUNCTION( BIND_CLASS NpcPlane_GetCopy ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "NpcPlane", "NpcPlane@+ SetChild(NpcPlane& child)", focFUNCTION( BIND_CLASS NpcPlane_SetChild ), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "NpcPlane", "NpcPlane@+ GetChild(uint index) const", focFUNCTION( BIND_CLASS NpcPlane_GetChild ), asCALL_CDECL_OBJFIRST );

        // RegisterObjectMethod( engine, "NpcPlane", "uint GetIndex() const", focFUNCTION(BIND_CLASS NpcPlane_GetIndex), asCALL_CDECL_OBJFIRST );
        RegisterObjectMethod( engine, "NpcPlane", "bool Misc_SetScript(string& funcName)", focFUNCTION( BIND_CLASS NpcPlane_Misc_SetScript ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // ProtoItem
    //

    RegisterObjectProperty( engine, "ProtoItem", "const uint16 ProtoId", focOFFSET( ProtoItem, ProtoId ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Type", focOFFSET( ProtoItem, Type ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint PicMap", focOFFSET( ProtoItem, PicMap ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint PicInv", focOFFSET( ProtoItem, PicInv ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Flags", focOFFSET( ProtoItem, Flags ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Stackable", focOFFSET( ProtoItem, Stackable ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Deteriorable", focOFFSET( ProtoItem, Deteriorable ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool GroundLevel", focOFFSET( ProtoItem, GroundLevel ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Corner", focOFFSET( ProtoItem, Corner ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Dir", focOFFSET( ProtoItem, Dir ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 Slot", focOFFSET( ProtoItem, Slot ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weight", focOFFSET( ProtoItem, Weight ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Volume", focOFFSET( ProtoItem, Volume ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Cost", focOFFSET( ProtoItem, Cost ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint StartCount", focOFFSET( ProtoItem, StartCount ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 SoundId", focOFFSET( ProtoItem, SoundId ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 Material", focOFFSET( ProtoItem, Material ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 LightFlags", focOFFSET( ProtoItem, LightFlags ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 LightDistance", focOFFSET( ProtoItem, LightDistance ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int8 LightIntensity", focOFFSET( ProtoItem, LightIntensity ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint LightColor", focOFFSET( ProtoItem, LightColor ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool DisableEgg", focOFFSET( ProtoItem, DisableEgg ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 AnimWaitBase", focOFFSET( ProtoItem, AnimWaitBase ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 AnimWaitRndMin", focOFFSET( ProtoItem, AnimWaitRndMin ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 AnimWaitRndMax", focOFFSET( ProtoItem, AnimWaitRndMax ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 AnimStay_0", focOFFSET( ProtoItem, AnimStay[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 AnimStay_1", focOFFSET( ProtoItem, AnimStay[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 AnimShow_0", focOFFSET( ProtoItem, AnimShow[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 AnimShow_1", focOFFSET( ProtoItem, AnimShow[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 AnimHide_0", focOFFSET( ProtoItem, AnimHide[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 AnimHide_1", focOFFSET( ProtoItem, AnimHide[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int16 OffsetX", focOFFSET( ProtoItem, OffsetX ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int16 OffsetY", focOFFSET( ProtoItem, OffsetY ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 SpriteCut", focOFFSET( ProtoItem, SpriteCut ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int8 DrawOrderOffsetHexY", focOFFSET( ProtoItem, DrawOrderOffsetHexY ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 RadioChannel", focOFFSET( ProtoItem, RadioChannel ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 RadioFlags", focOFFSET( ProtoItem, RadioFlags ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 RadioBroadcastSend", focOFFSET( ProtoItem, RadioBroadcastSend ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 RadioBroadcastRecv", focOFFSET( ProtoItem, RadioBroadcastRecv ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 IndicatorStart", focOFFSET( ProtoItem, IndicatorStart ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 IndicatorMax", focOFFSET( ProtoItem, IndicatorMax ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint HolodiskNum", focOFFSET( ProtoItem, HolodiskNum ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_0", focOFFSET( ProtoItem, StartValue[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_1", focOFFSET( ProtoItem, StartValue[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_2", focOFFSET( ProtoItem, StartValue[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_3", focOFFSET( ProtoItem, StartValue[3] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_4", focOFFSET( ProtoItem, StartValue[4] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_5", focOFFSET( ProtoItem, StartValue[5] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_6", focOFFSET( ProtoItem, StartValue[6] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_7", focOFFSET( ProtoItem, StartValue[7] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_8", focOFFSET( ProtoItem, StartValue[8] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int StartValue_9", focOFFSET( ProtoItem, StartValue[9] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 BlockLines", focOFFSET( ProtoItem, BlockLines ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 ChildPid_0", focOFFSET( ProtoItem, ChildPid[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 ChildPid_1", focOFFSET( ProtoItem, ChildPid[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 ChildPid_2", focOFFSET( ProtoItem, ChildPid[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 ChildPid_3", focOFFSET( ProtoItem, ChildPid[3] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 ChildPid_4", focOFFSET( ProtoItem, ChildPid[4] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 ChildLines_0", focOFFSET( ProtoItem, ChildLines[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 ChildLines_1", focOFFSET( ProtoItem, ChildLines[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 ChildLines_2", focOFFSET( ProtoItem, ChildLines[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 ChildLines_3", focOFFSET( ProtoItem, ChildLines[3] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 ChildLines_4", focOFFSET( ProtoItem, ChildLines[4] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Weapon_IsUnarmed", focOFFSET( ProtoItem, Weapon_IsUnarmed ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_UnarmedTree", focOFFSET( ProtoItem, Weapon_UnarmedTree ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_UnarmedPriority", focOFFSET( ProtoItem, Weapon_UnarmedPriority ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_UnarmedMinAgility", focOFFSET( ProtoItem, Weapon_UnarmedMinAgility ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_UnarmedMinUnarmed", focOFFSET( ProtoItem, Weapon_UnarmedMinUnarmed ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_UnarmedMinLevel", focOFFSET( ProtoItem, Weapon_UnarmedMinLevel ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_Anim1", focOFFSET( ProtoItem, Weapon_Anim1 ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_MaxAmmoCount", focOFFSET( ProtoItem, Weapon_MaxAmmoCount ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_Caliber", focOFFSET( ProtoItem, Weapon_Caliber ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 Weapon_DefaultAmmoPid", focOFFSET( ProtoItem, Weapon_DefaultAmmoPid ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_MinStrength", focOFFSET( ProtoItem, Weapon_MinStrength ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_Perk", focOFFSET( ProtoItem, Weapon_Perk ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_ActiveUses", focOFFSET( ProtoItem, Weapon_ActiveUses ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_Skill_0", focOFFSET( ProtoItem, Weapon_Skill[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_Skill_1", focOFFSET( ProtoItem, Weapon_Skill[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Weapon_Skill_2", focOFFSET( ProtoItem, Weapon_Skill[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_PicUse_0", focOFFSET( ProtoItem, Weapon_PicUse[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_PicUse_1", focOFFSET( ProtoItem, Weapon_PicUse[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_PicUse_2", focOFFSET( ProtoItem, Weapon_PicUse[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_MaxDist_0", focOFFSET( ProtoItem, Weapon_MaxDist[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_MaxDist_1", focOFFSET( ProtoItem, Weapon_MaxDist[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_MaxDist_2", focOFFSET( ProtoItem, Weapon_MaxDist[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_Round_0", focOFFSET( ProtoItem, Weapon_Round[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_Round_1", focOFFSET( ProtoItem, Weapon_Round[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_Round_2", focOFFSET( ProtoItem, Weapon_Round[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_ApCost_0", focOFFSET( ProtoItem, Weapon_ApCost[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_ApCost_1", focOFFSET( ProtoItem, Weapon_ApCost[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Weapon_ApCost_2", focOFFSET( ProtoItem, Weapon_ApCost[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Weapon_Aim_0", focOFFSET( ProtoItem, Weapon_Aim[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Weapon_Aim_1", focOFFSET( ProtoItem, Weapon_Aim[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Weapon_Aim_2", focOFFSET( ProtoItem, Weapon_Aim[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 Weapon_SoundId_0", focOFFSET( ProtoItem, Weapon_SoundId[0] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 Weapon_SoundId_1", focOFFSET( ProtoItem, Weapon_SoundId[1] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint8 Weapon_SoundId_2", focOFFSET( ProtoItem, Weapon_SoundId[2] ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Ammo_Caliber", focOFFSET( ProtoItem, Ammo_Caliber ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Door_NoBlockMove", focOFFSET( ProtoItem, Door_NoBlockMove ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Door_NoBlockShoot", focOFFSET( ProtoItem, Door_NoBlockShoot ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Door_NoBlockLight", focOFFSET( ProtoItem, Door_NoBlockLight ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Container_Volume", focOFFSET( ProtoItem, Container_Volume ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Container_Changeble", focOFFSET( ProtoItem, Container_Changeble ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Container_CannotPickUp", focOFFSET( ProtoItem, Container_CannotPickUp ) );
    RegisterObjectProperty( engine, "ProtoItem", "const bool Container_MagicHandsGrnd", focOFFSET( ProtoItem, Container_MagicHandsGrnd ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint16 Locker_Condition", focOFFSET( ProtoItem, Locker_Condition ) );
    RegisterObjectProperty( engine, "ProtoItem", "const int Grid_Type", focOFFSET( ProtoItem, Grid_Type ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_Speed", focOFFSET( ProtoItem, Car_Speed ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_Passability", focOFFSET( ProtoItem, Car_Passability ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_DeteriorationRate", focOFFSET( ProtoItem, Car_DeteriorationRate ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_CrittersCapacity", focOFFSET( ProtoItem, Car_CrittersCapacity ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_TankVolume", focOFFSET( ProtoItem, Car_TankVolume ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_MaxDeterioration", focOFFSET( ProtoItem, Car_MaxDeterioration ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_FuelConsumption", focOFFSET( ProtoItem, Car_FuelConsumption ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_Entrance", focOFFSET( ProtoItem, Car_Entrance ) );
    RegisterObjectProperty( engine, "ProtoItem", "const uint Car_MovementType", focOFFSET( ProtoItem, Car_MovementType ) );

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectMethod( engine, "ProtoItem", "string@ GetScriptName() const", focFUNCTION( BIND_CLASS ProtoItem_GetScriptName ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // Scenery
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterObjectProperty( engine, "Scenery", "const uint16 ProtoId", focOFFSET( MapObject, ProtoId ) );
        RegisterObjectProperty( engine, "Scenery", "const uint16 HexX", focOFFSET( MapObject, MapX ) );
        RegisterObjectProperty( engine, "Scenery", "const uint16 HexY", focOFFSET( MapObject, MapY ) );

        RegisterObjectMethod( engine, "Scenery", "bool CallSceneryFunction(Critter& cr, int skill, Item@+ item)", focFUNCTION( BIND_CLASS Scen_CallSceneryFunction ), asCALL_CDECL_OBJFIRST );
    }
    #endif

    //
    // Synchronizer
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( app == APP_TYPE_SERVER )
    {
        RegisterGlobalFunction( engine, "void Synchronize()", focFUNCTION( BIND_CLASS Global_Synchronize ), asCALL_CDECL );
        RegisterGlobalFunction( engine, "void Resynchronize()", focFUNCTION( BIND_CLASS Global_Resynchronize ), asCALL_CDECL );

        RegisterObjectMethod( engine, "Synchronizer", "void Lock()", focMETHOD( SyncObject, Lock ), asCALL_THISCALL );
    }
    #endif

    return BindSuccess;
}
