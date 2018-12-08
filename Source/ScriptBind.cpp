#include "Core.h"

#include <angelscript.h>

#include "DynamicLibrary.h"
#include "GameOptions.h"
#include "Log.h"
#include "Random.h"
#include "Script.h"

// copy macros related to registration
// redefined during dummy registration

#define focFUNCTION( func )                        asFUNCTION( func )
#define focMETHOD( clas, member )                  asMETHOD( clas, member )
#define focMETHODPR( clas, member, decl, ret )     asMETHODPR( clas, member, decl, ret )
#define focOFFSET( clas, member )                  asOFFSET( clas, member )
#define focSIZEOF( obj )                           sizeof(obj)

#if defined (FOCLASSIC_CLIENT)
# include "Client.h"
# define BIND_CLIENT
# define BIND_CLASS                                FOClient::SScriptFunc::
# define BIND_CLASS_DERAIL( name )                 & FOClient::SScriptFunc::name
# define BIND_ASSERT( x )                          if( (x) < 0 ) { WriteLog( "Bind error, line<%d>.\n", __LINE__ ); return false; }
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
// change the meaning of selected AS macros
// allows ASCompiler to compile without issues or preparing dummy structures
# undef focFUNCTION
# undef focMETHOD
# undef focMETHODPR
# undef focOFFSET
# undef focSIZEOF
# define focFUNCTION( func )                       asFUNCTION( 0 )
# define focMETHOD( clas, method )                 asFUNCTION( 0 )
# define focMETHODPR( clas, method, decl, ret )    asFUNCTION( 0 )
# define focOFFSET( clas, method )                 ++ DummyObject.DummyMember
# define focSIZEOF( obj )                          sizeof(DummyClass)
# define BIND_DUMMY
# define BIND_CLASS_DERAIL( name )                 & DummyObject.DummyMember
struct DummyClass
{
    uint DummyMember;
} DummyObject;
#endif

// keep dummy registration in own namespace
// allows real and dummy registration to co-exists within same function

#if !defined (BIND_DUMMY)
namespace ScriptBind = Script;
#else
namespace ScriptBind = ScriptDummy;
#endif

#if !defined (BIND_ASSERT)
# define BIND_ASSERT( x )                          if( (x) < 0 ) { WriteLogF( _FUNC_, " - Bind error, line<%d>.\n", __LINE__ ); return false; }
#endif

#pragma TODO("Generate BindError automagically")
static const char* BindError[] =
{
    "SUCCESS",                                  //  0
    "ERROR",                                    // -1
    "CONTEXT_ACTIVE",                           // -2
    "CONTEXT_NOT_FINISHED",                     // -3
    "CONTEXT_NOT_PREPARED",                     // -4
    "INVALID_ARG",                              // -5
    "NO_FUNCTION",                              // -6
    "NOT_SUPPORTED",                            // -7
    "INVALID_NAME",                             // -8
    "NAME_TAKEN",                               // -9
    "INVALID_DECLARATION",                      // -10
    "INVALID_OBJECT",                           // -11
    "INVALID_TYPE",                             // -12
    "ALREADY_REGISTERED",                       // -13
    "MULTIPLE_FUNCTIONS",                       // -14
    "NO_MODULE",                                // -15
    "NO_GLOBAL_VAR",                            // -16
    "INVALID_CONFIGURATION",                    // -17
    "INVALID_INTERFACE",                        // -18
    "CANT_BIND_ALL_FUNCTIONS",                  // -19
    "LOWER_ARRAY_DIMENSION_NOT_REGISTERED",     // -20
    "WRONG_CONFIG_GROUP",                       // -21
    "CONFIG_GROUP_IS_IN_USE",                   // -22
    "ILLEGAL_BEHAVIOUR_FOR_TYPE",               // -23
    "WRONG_CALLING_CONV",                       // -24
    "BUILD_IN_PROGRESS",                        // -25
    "INIT_GLOBAL_VARS_FAILED",                  // -26
    "OUT_OF_MEMORY",                            // -27
};

/* asCALL_CDECL */
bool ScriptBind::RegisterGlobalFunction( asIScriptEngine* engine, string declaration, const asSFuncPtr& function, asDWORD callConv, string name )
{
    if( !engine )
    {
        WriteLog( "BIND ERROR : no engine\n" );

        return false;
    }

    int result = engine->RegisterGlobalFunction( declaration.c_str(), function, callConv );
    if( result )
    {
        // TODO
        return true;
    }

    WriteLog( "BIND ERROR : %s : %s\n", BindError[abs( result )], declaration.c_str() );
    return false;
}

/* asCALL_CDECL_OBJFIRST */
bool ScriptBind::RegisterObjectMethod( asIScriptEngine* engine, string object, string method, const asSFuncPtr& function, asDWORD callConv, string name )
{
    if( !engine )
    {
        WriteLog( "BIND ERROR : no engine\n" );

        return false;
    }

    int result = engine->RegisterObjectMethod( object.c_str(), method.c_str(), function, callConv );
    if( result )
    {
        // TODO
        return true;
    }

    WriteLog( "BIND ERROR : %s : %s %s\n", BindError[abs( result )], object.c_str(), method.c_str() );
    return false;
}

bool ScriptBind::RegisterAll( asIScriptEngine* engine, uchar bind )
{
    //
    // - both checks must pass before using any registration function:
    //   #ifdef - filters real registration
    //   if()   - filters dummy registration
    //
    // - for real registration, 'bind' argument is always related to current application
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

    BIND_ASSERT( engine->SetEngineProperty( asEP_ALLOW_UNSAFE_REFERENCES, true ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_OPTIMIZE_BYTECODE, true ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_SCRIPT_SCANNER, 1 ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_USE_CHARACTER_LITERALS, 1 ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_AUTO_GARBAGE_COLLECT, false ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT, true ) );

    #if defined (BIND_DUMMY)
    DummyObject.DummyMember = 0;
    // AS docs recommends to skip global vars initialization after script is built
    BIND_ASSERT( engine->SetEngineProperty( asEP_INIT_GLOBAL_VARS_AFTER_BUILD, false ) );
    #endif

    //
    // Forward registration
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "CraftItem", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "CraftItem", asBEHAVE_ADDREF, "void f()", focMETHOD( CraftItem, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "CraftItem", asBEHAVE_RELEASE, "void f()", focMETHOD( CraftItem, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "Critter", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Critter", asBEHAVE_ADDREF, "void f()", focMETHOD( Critter, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Critter", asBEHAVE_RELEASE, "void f()", focMETHOD( Critter, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( bind == SCRIPT_BIND_CLIENT )
    {
        BIND_ASSERT( engine->RegisterObjectType( "CritterCl", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "CritterCl", asBEHAVE_ADDREF, "void f()", focMETHOD( CritterCl, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "CritterCl", asBEHAVE_RELEASE, "void f()", focMETHOD( CritterCl, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_CLIENT || bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "DataRef", 0, asOBJ_REF | asOBJ_NOHANDLE ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_CLIENT || bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "DataVal", 0, asOBJ_REF | asOBJ_NOHANDLE ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "GameVar", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "GameVar", asBEHAVE_ADDREF, "void f()", focMETHOD( GameVar, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "GameVar", asBEHAVE_RELEASE, "void f()", focMETHOD( GameVar, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "Item", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Item", asBEHAVE_ADDREF, "void f()", focMETHOD( Item, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Item", asBEHAVE_RELEASE, "void f()", focMETHOD( Item, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( bind == SCRIPT_BIND_CLIENT )
    {
        BIND_ASSERT( engine->RegisterObjectType( "ItemCl", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "ItemCl", asBEHAVE_ADDREF, "void f()", focMETHOD( Item, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "ItemCl", asBEHAVE_RELEASE, "void f()", focMETHOD( Item, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "Location", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Location", asBEHAVE_ADDREF, "void f()", focMETHOD( Location, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Location", asBEHAVE_RELEASE, "void f()", focMETHOD( Location, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "Map", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Map", asBEHAVE_ADDREF, "void f()", focMETHOD( Map, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Map", asBEHAVE_RELEASE, "void f()", focMETHOD( Map, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( bind == SCRIPT_BIND_MAPPER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "MapperMap", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperMap", asBEHAVE_ADDREF, "void f()", focMETHOD( ProtoMap, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperMap", asBEHAVE_RELEASE, "void f()", focMETHOD( ProtoMap, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    {
        BIND_ASSERT( engine->RegisterObjectType( "MapperObject", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperObject", asBEHAVE_ADDREF, "void f()", focMETHOD( MapObject, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperObject", asBEHAVE_RELEASE, "void f()", focMETHOD( MapObject, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "NpcPlane", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "NpcPlane", asBEHAVE_ADDREF, "void f()", focMETHOD( AIDataPlane, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "NpcPlane", asBEHAVE_RELEASE, "void f()", focMETHOD( AIDataPlane, Release ), asCALL_THISCALL ) );
    }
    #endif

    BIND_ASSERT( engine->RegisterObjectType( "ProtoItem", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "ProtoItem", asBEHAVE_ADDREF, "void f()", focMETHOD( ProtoItem, AddRef ), asCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "ProtoItem", asBEHAVE_RELEASE, "void f()", focMETHOD( ProtoItem, Release ), asCALL_THISCALL ) );

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "Scenery", 0, asOBJ_REF ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Scenery", asBEHAVE_ADDREF, "void f()", focMETHOD( MapObject, AddRef ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Scenery", asBEHAVE_RELEASE, "void f()", focMETHOD( MapObject, Release ), asCALL_THISCALL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectType( "Synchronizer", focSIZEOF( SyncObject ), asOBJ_VALUE ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Synchronizer", asBEHAVE_CONSTRUCT, "void f()", focFUNCTION( BIND_CLASS Synchronizer_Constructor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectBehaviour( "Synchronizer", asBEHAVE_DESTRUCT, "void f()", focFUNCTION( BIND_CLASS Synchronizer_Destructor ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // Global properties
    //

    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MapHexagonal", &GameOpt.MapHexagonal ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapHexWidth", &GameOpt.MapHexWidth ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapHexHeight", &GameOpt.MapHexHeight ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapHexLineHeight", &GameOpt.MapHexLineHeight ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapTileOffsX", &GameOpt.MapTileOffsX ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapTileOffsY", &GameOpt.MapTileOffsY ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapRoofOffsX", &GameOpt.MapRoofOffsX ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapRoofOffsY", &GameOpt.MapRoofOffsY ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapRoofSkipSize", &GameOpt.MapRoofSkipSize ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "float __MapCameraAngle", &GameOpt.MapCameraAngle ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MapSmoothPath", &GameOpt.MapSmoothPath ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "string __MapDataPrefix", &GameOpt.MapDataPrefix ) );

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( bind == SCRIPT_BIND_CLIENT )
    {
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ConsoleActive", BIND_CLASS_DERAIL( ConsoleActive ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __GmapActive", BIND_CLASS_DERAIL( GmapActive ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __GmapWait", BIND_CLASS_DERAIL( GmapWait ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __GmapZoom", BIND_CLASS_DERAIL( GmapZoom ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapOffsetX", BIND_CLASS_DERAIL( GmapOffsetX ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapOffsetY", BIND_CLASS_DERAIL( GmapOffsetY ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupCurX", BIND_CLASS_DERAIL( GmapGroupCurX ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupCurY", BIND_CLASS_DERAIL( GmapGroupCurY ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupToX", BIND_CLASS_DERAIL( GmapGroupToX ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupToY", BIND_CLASS_DERAIL( GmapGroupToY ) ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __GmapGroupSpeed", BIND_CLASS_DERAIL( GmapGroupSpeed ) ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( bind == SCRIPT_BIND_MAPPER )
    {
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __ClientPath", &GameOpt.ClientPath ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __ServerPath", &GameOpt.ServerPath ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowCorners", &GameOpt.ShowCorners ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowSpriteCuts", &GameOpt.ShowSpriteCuts ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowDrawOrder", &GameOpt.ShowDrawOrder ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __SplitTilesCollection", &GameOpt.SplitTilesCollection ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_MAPPER)
    if( bind == SCRIPT_BIND_CLIENT || bind == SCRIPT_BIND_MAPPER )
    {
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __Quit", &GameOpt.Quit ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __OpenGLRendering", &GameOpt.OpenGLRendering ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __OpenGLDebug", &GameOpt.OpenGLDebug ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __AssimpLogging", &GameOpt.AssimpLogging ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __MouseX", &GameOpt.MouseX ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __MouseY", &GameOpt.MouseY ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint8 __RoofAlpha", &GameOpt.RoofAlpha ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __HideCursor", &GameOpt.HideCursor ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const int __ScreenWidth", &GameOpt.ScreenWidth ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const int __ScreenHeight", &GameOpt.ScreenHeight ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const int __MultiSampling", &GameOpt.MultiSampling ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __DisableLMenu", &GameOpt.DisableLMenu ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __DisableMouseEvents", &GameOpt.DisableMouseEvents ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __DisableKeyboardEvents", &GameOpt.DisableKeyboardEvents ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __HidePassword", &GameOpt.HidePassword ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __PlayerOffAppendix", &GameOpt.PlayerOffAppendix ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __DamageHitDelay", &GameOpt.DamageHitDelay ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __CombatMessagesType", &GameOpt.CombatMessagesType ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowTile", &GameOpt.ShowTile ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowRoof", &GameOpt.ShowRoof ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowItem", &GameOpt.ShowItem ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowScen", &GameOpt.ShowScen ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowWall", &GameOpt.ShowWall ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowCrit", &GameOpt.ShowCrit ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowFast", &GameOpt.ShowFast ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowPlayerNames", &GameOpt.ShowPlayerNames ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowNpcNames", &GameOpt.ShowNpcNames ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowCritId", &GameOpt.ShowCritId ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollKeybLeft", &GameOpt.ScrollKeybLeft ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollKeybRight", &GameOpt.ScrollKeybRight ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollKeybUp", &GameOpt.ScrollKeybUp ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollKeybDown", &GameOpt.ScrollKeybDown ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollMouseLeft", &GameOpt.ScrollMouseLeft ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollMouseRight", &GameOpt.ScrollMouseRight ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollMouseUp", &GameOpt.ScrollMouseUp ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollMouseDown", &GameOpt.ScrollMouseDown ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowGroups", &GameOpt.ShowGroups ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __HelpInfo", &GameOpt.HelpInfo ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __FullScr", &GameOpt.FullScreen ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __VSync", &GameOpt.VSync ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __FlushVal", &GameOpt.FlushVal ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __BaseTex", &GameOpt.BaseTexture ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __Light", &GameOpt.Light ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ScrollDelay", &GameOpt.ScrollDelay ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __ScrollStep", &GameOpt.ScrollStep ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MouseScroll", &GameOpt.MouseScroll ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ScrollCheck", &GameOpt.ScrollCheck ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __FoDataPath", &GameOpt.FoDataPath ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __Host", &GameOpt.Host ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Port", &GameOpt.Port ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ProxyType", &GameOpt.ProxyType ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __ProxyHost", &GameOpt.ProxyHost ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ProxyPort", &GameOpt.ProxyPort ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __ProxyUser", &GameOpt.ProxyUser ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __ProxyPass", &GameOpt.ProxyPass ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "string __Name", &GameOpt.Name ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TextDelay", &GameOpt.TextDelay ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __AlwaysOnTop", &GameOpt.AlwaysOnTop ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __FixedFPS", &GameOpt.FixedFPS ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __FPS", &GameOpt.FPS ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __PingPeriod", &GameOpt.PingPeriod ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Ping", &GameOpt.Ping ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MsgboxInvert", &GameOpt.MsgboxInvert ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint8 __DefaultCombatMode", &GameOpt.DefaultCombatMode ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MessNotify", &GameOpt.MessNotify ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __SoundNotify", &GameOpt.SoundNotify ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __IndicatorType", &GameOpt.IndicatorType ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __DoubleClickTime", &GameOpt.DoubleClickTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __RunModMul", &GameOpt.RunModMul ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __RunModDiv", &GameOpt.RunModDiv ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __RunModAdd", &GameOpt.RunModAdd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Animation3dSmoothTime", &GameOpt.Animation3dSmoothTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Animation3dFPS", &GameOpt.Animation3dFPS ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MapZooming", &GameOpt.MapZooming ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __SpritesZoom", &GameOpt.SpritesZoom ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __SpritesZoomMin", &GameOpt.SpritesZoomMin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __SpritesZoomMax", &GameOpt.SpritesZoomMax ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue0", &GameOpt.EffectValues[0] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue1", &GameOpt.EffectValues[1] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue2", &GameOpt.EffectValues[2] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue3", &GameOpt.EffectValues[3] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue4", &GameOpt.EffectValues[4] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue5", &GameOpt.EffectValues[5] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue6", &GameOpt.EffectValues[6] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue7", &GameOpt.EffectValues[7] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue8", &GameOpt.EffectValues[8] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "float __EffectValue9", &GameOpt.EffectValues[9] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __AlwaysRun", &GameOpt.AlwaysRun ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __AlwaysRunMoveDist", &GameOpt.AlwaysRunMoveDist ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __AlwaysRunUseDist", &GameOpt.AlwaysRunUseDist ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __CritterFidgetTime", &GameOpt.CritterFidgetTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Anim2CombatBegin", &GameOpt.Anim2CombatBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Anim2CombatIdle", &GameOpt.Anim2CombatIdle ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Anim2CombatEnd", &GameOpt.Anim2CombatEnd ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_CLIENT || bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint16 __Year", &GameOpt.Year ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint16 __Month", &GameOpt.Month ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint16 __Day", &GameOpt.Day ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint16 __Hour", &GameOpt.Hour ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint16 __Minute", &GameOpt.Minute ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint16 __Second", &GameOpt.Second ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint16 __TimeMultiplier", &GameOpt.TimeMultiplier ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "const uint __FullSecond", &GameOpt.FullSecond ) );

        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __DisableTcpNagle", &GameOpt.DisableTcpNagle ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __DisableZlibCompression", &GameOpt.DisableZlibCompression ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __FloodSize", &GameOpt.FloodSize ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __NoAnswerShuffle", &GameOpt.NoAnswerShuffle ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __DialogDemandRecheck", &GameOpt.DialogDemandRecheck ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __FixBoyDefaultExperience", &GameOpt.FixBoyDefaultExperience ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __SneakDivider", &GameOpt.SneakDivider ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LevelCap", &GameOpt.LevelCap ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __LevelCapAddExperience", &GameOpt.LevelCapAddExperience ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookNormal", &GameOpt.LookNormal ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookMinimum", &GameOpt.LookMinimum ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __GlobalMapMaxGroupCount", &GameOpt.GlobalMapMaxGroupCount ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __CritterIdleTick", &GameOpt.CritterIdleTick ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TurnBasedTick", &GameOpt.TurnBasedTick ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __DeadHitPoints", &GameOpt.DeadHitPoints ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __Breaktime", &GameOpt.Breaktime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TimeoutTransfer", &GameOpt.TimeoutTransfer ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TimeoutBattle", &GameOpt.TimeoutBattle ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ApRegeneration", &GameOpt.ApRegeneration ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostCritterWalk", &GameOpt.RtApCostCritterWalk ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostCritterRun", &GameOpt.RtApCostCritterRun ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostMoveItemContainer", &GameOpt.RtApCostMoveItemContainer ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostMoveItemInventory", &GameOpt.RtApCostMoveItemInventory ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostPickItem", &GameOpt.RtApCostPickItem ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostDropItem", &GameOpt.RtApCostDropItem ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostReloadWeapon", &GameOpt.RtApCostReloadWeapon ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostPickCritter", &GameOpt.RtApCostPickCritter ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostUseItem", &GameOpt.RtApCostUseItem ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RtApCostUseSkill", &GameOpt.RtApCostUseSkill ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __RtAlwaysRun", &GameOpt.RtAlwaysRun ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostCritterMove", &GameOpt.TbApCostCritterMove ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostMoveItemContainer", &GameOpt.TbApCostMoveItemContainer ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostMoveItemInventory", &GameOpt.TbApCostMoveItemInventory ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostPickItem", &GameOpt.TbApCostPickItem ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostDropItem", &GameOpt.TbApCostDropItem ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostReloadWeapon", &GameOpt.TbApCostReloadWeapon ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostPickCritter", &GameOpt.TbApCostPickCritter ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostUseItem", &GameOpt.TbApCostUseItem ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TbApCostUseSkill", &GameOpt.TbApCostUseSkill ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ApCostAimEyes", &GameOpt.ApCostAimEyes ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ApCostAimHead", &GameOpt.ApCostAimHead ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ApCostAimGroin", &GameOpt.ApCostAimGroin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ApCostAimTorso", &GameOpt.ApCostAimTorso ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ApCostAimArms", &GameOpt.ApCostAimArms ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ApCostAimLegs", &GameOpt.ApCostAimLegs ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __TbAlwaysRun", &GameOpt.TbAlwaysRun ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __RunOnCombat", &GameOpt.RunOnCombat ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __RunOnTransfer", &GameOpt.RunOnTransfer ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __GlobalMapWidth", &GameOpt.GlobalMapWidth ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __GlobalMapHeight", &GameOpt.GlobalMapHeight ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __GlobalMapZoneLength", &GameOpt.GlobalMapZoneLength ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __GlobalMapMoveTime", &GameOpt.GlobalMapMoveTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __BagRefreshTime", &GameOpt.BagRefreshTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __AttackAnimationsMinDist", &GameOpt.AttackAnimationsMinDist ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __WisperDist", &GameOpt.WhisperDist ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ShoutDist", &GameOpt.ShoutDist ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __LookChecks", &GameOpt.LookChecks ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookDir0", &GameOpt.LookDir[0] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookDir1", &GameOpt.LookDir[1] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookDir2", &GameOpt.LookDir[2] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookDir3", &GameOpt.LookDir[3] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookDir4", &GameOpt.LookDir[4] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookSneakDir0", &GameOpt.LookSneakDir[0] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookSneakDir1", &GameOpt.LookSneakDir[1] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookSneakDir2", &GameOpt.LookSneakDir[2] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookSneakDir3", &GameOpt.LookSneakDir[3] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookSneakDir4", &GameOpt.LookSneakDir[4] ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __LookWeight", &GameOpt.LookWeight ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __CustomItemCost", &GameOpt.CustomItemCost ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __RegistrationTimeout", &GameOpt.RegistrationTimeout ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __AccountPlayTime", &GameOpt.AccountPlayTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __LoggingVars", &GameOpt.LoggingVars ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ScriptRunSuspendTimeout", &GameOpt.ScriptRunSuspendTimeout ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ScriptRunMessageTimeout", &GameOpt.ScriptRunMessageTimeout ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TalkDistance", &GameOpt.TalkDistance ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __NpcMaxTalkers", &GameOpt.NpcMaxTalkers ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __MinNameLength", &GameOpt.MinNameLength ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __MaxNameLength", &GameOpt.MaxNameLength ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __DlgTalkMinTime", &GameOpt.DlgTalkMinTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __DlgBarterMinTime", &GameOpt.DlgBarterMinTime ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __MinimumOfflineTime", &GameOpt.MinimumOfflineTime ) );

        BIND_ASSERT( engine->RegisterGlobalProperty( "int __StartSpecialPoints", &GameOpt.StartSpecialPoints ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __StartTagSkillPoints", &GameOpt.StartTagSkillPoints ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __SkillMaxValue", &GameOpt.SkillMaxValue ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __SkillModAdd2", &GameOpt.SkillModAdd2 ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __SkillModAdd3", &GameOpt.SkillModAdd3 ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __SkillModAdd4", &GameOpt.SkillModAdd4 ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __SkillModAdd5", &GameOpt.SkillModAdd5 ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __SkillModAdd6", &GameOpt.SkillModAdd6 ) );

        BIND_ASSERT( engine->RegisterGlobalProperty( "bool __AbsoluteOffsets", &GameOpt.AbsoluteOffsets ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __SkillBegin", &GameOpt.SkillBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __SkillEnd", &GameOpt.SkillEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TimeoutBegin", &GameOpt.TimeoutBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TimeoutEnd", &GameOpt.TimeoutEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __KillBegin", &GameOpt.KillBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __KillEnd", &GameOpt.KillEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __PerkBegin", &GameOpt.PerkBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __PerkEnd", &GameOpt.PerkEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __AddictionBegin", &GameOpt.AddictionBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __AddictionEnd", &GameOpt.AddictionEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __KarmaBegin", &GameOpt.KarmaBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __KarmaEnd", &GameOpt.KarmaEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __DamageBegin", &GameOpt.DamageBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __DamageEnd", &GameOpt.DamageEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TraitBegin", &GameOpt.TraitBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __TraitEnd", &GameOpt.TraitEnd ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ReputationBegin", &GameOpt.ReputationBegin ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "uint __ReputationEnd", &GameOpt.ReputationEnd ) );

        BIND_ASSERT( engine->RegisterGlobalProperty( "int __ReputationLoved", &GameOpt.ReputationLoved ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __ReputationLiked", &GameOpt.ReputationLiked ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __ReputationAccepted", &GameOpt.ReputationAccepted ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __ReputationNeutral", &GameOpt.ReputationNeutral ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __ReputationAntipathy", &GameOpt.ReputationAntipathy ) );
        BIND_ASSERT( engine->RegisterGlobalProperty( "int __ReputationHated", &GameOpt.ReputationHated ) );
    }
    #endif

    //
    // Global functions
    //

    BIND_ASSERT( engine->RegisterGlobalFunction( "int Random(int minimum, int maximum)", asFUNCTION( Random ), asCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "void Log(string& text)", focFUNCTION( BIND_CLASS Global_Log ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetLastError()", focFUNCTION( BIND_CLASS Global_GetLastError ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "ProtoItem@+ GetProtoItem(uint16 protoId)", focFUNCTION( BIND_CLASS Global_GetProtoItem ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool StrToInt(string@+ text, int& result)", focFUNCTION( BIND_CLASS Global_StrToInt ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool StrToFloat(string@+ text, float& result)", focFUNCTION( BIND_CLASS Global_StrToFloat ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetDistantion(uint16 hexX1, uint16 hexY1, uint16 hexX2, uint16 hexY2)", focFUNCTION( BIND_CLASS Global_GetDistantion ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint8 GetDirection(uint16 fromHexX, uint16 fromHexY, uint16 toHexX, uint16 toHexY)", focFUNCTION( BIND_CLASS Global_GetDirection ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint8 GetOffsetDir(uint16 fromHexX, uint16 fromHexY, uint16 toHexX, uint16 toHexY, float offset)", focFUNCTION( BIND_CLASS Global_GetOffsetDir ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetTick()", focFUNCTION( BIND_CLASS Global_GetTick ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAngelScriptProperty(int property)", focFUNCTION( BIND_CLASS Global_GetAngelScriptProperty ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetAngelScriptProperty(int property, uint value)", focFUNCTION( BIND_CLASS Global_SetAngelScriptProperty ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetStrHash(string@+ str)", focFUNCTION( BIND_CLASS Global_GetStrHash ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadDataFile(string& dataFileName)", focFUNCTION( BIND_CLASS Global_LoadDataFile ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetConstantValue(int constCollection, string@+ name)", focFUNCTION( BIND_CLASS Global_GetConstantValue ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetConstantName(int constCollection, int value)", focFUNCTION( BIND_CLASS Global_GetConstantName ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void AddConstant(int constCollection, string@+ name, int value)", focFUNCTION( BIND_CLASS Global_AddConstant ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadConstants(int constCollection, string@+ fileName, int pathType)", focFUNCTION( BIND_CLASS Global_LoadConstants ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void AllowSlot(uint8 index, string& slotName)", focFUNCTION( BIND_CLASS Global_AllowSlot ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint DecodeUTF8(const string& text, uint& length)", focFUNCTION( BIND_CLASS Global_DecodeUTF8 ), asCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ EncodeUTF8(uint ucs)", focFUNCTION( BIND_CLASS Global_EncodeUTF8 ), asCALL_CDECL ) );

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( bind == SCRIPT_BIND_CLIENT )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist)", focFUNCTION( BIND_CLASS Global_GetHexInPath ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthHex ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPathLength(CritterCl& cr, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthCr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void FlushScreen(uint fromColor, uint toColor, uint timeMs)", focFUNCTION( BIND_CLASS Global_FlushScreen ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void QuakeScreen(uint noise, uint timeMs)", focFUNCTION( BIND_CLASS Global_QuakeScreen ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool PlaySound(string& soundName)", focFUNCTION( BIND_CLASS Global_PlaySound ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool PlaySound(uint8 soundType, uint8 soundTypeExt, uint8 soundId, uint8 soundIdExt)", focFUNCTION( BIND_CLASS Global_PlaySoundType ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool PlayMusic(string& musicName, uint pos, uint repeat)", focFUNCTION( BIND_CLASS Global_PlayMusic ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void PlayVideo(string& videoName, bool canStop)", focFUNCTION( BIND_CLASS Global_PlayVideo ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsTurnBased()", focFUNCTION( BIND_CLASS Global_IsTurnBased ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetTurnBasedTime()", focFUNCTION( BIND_CLASS Global_GetTurnBasedTime ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint16 GetCurrentMapPid()", focFUNCTION( BIND_CLASS Global_GetCurrentMapPid ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMessageFilters(int[]@+ filters)", focFUNCTION( BIND_CLASS Global_GetMessageFilters ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetMessageFilters(int[]@+ filters)", focFUNCTION( BIND_CLASS Global_SetMessageFilters ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(string& text)", focFUNCTION( BIND_CLASS Global_Message ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(string& text, int type)", focFUNCTION( BIND_CLASS Global_MessageType ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_MessageMsg ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(int textMsg, uint strNum, int type)", focFUNCTION( BIND_CLASS Global_MessageMsgType ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MapMessage(string& text, uint16 hx, uint16 hy, uint timeMs, uint color, bool fade, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_MapMessage ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMsgStrSkip ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumUpper(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumUpper ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumLower(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumLower ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrCount(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrCount ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_IsMsgStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, const string& str)", focFUNCTION( BIND_CLASS Global_ReplaceTextStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, int i)", focFUNCTION( BIND_CLASS Global_ReplaceTextInt ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ FormatTags(const string& text, const string@+ lexems)", focFUNCTION( BIND_CLASS Global_FormatTags ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSomeValue(int var)", focFUNCTION( BIND_CLASS Global_GetSomeValue ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveScreen(uint16 hexX, uint16 hexY, uint speed)", focFUNCTION( BIND_CLASS Global_MoveScreen ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void LockScreenScroll(CritterCl@+ cr)", focFUNCTION( BIND_CLASS Global_LockScreenScroll ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetFog(uint16 zoneX, uint16 zoneY)", focFUNCTION( BIND_CLASS Global_GetFog ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void RefreshItemsCollection(int collection)", focFUNCTION( BIND_CLASS Global_RefreshItemsCollection ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetScroll(int scrollElement)", focFUNCTION( BIND_CLASS Global_GetScroll ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetScroll(int scrollElement, int value)", focFUNCTION( BIND_CLASS Global_SetScroll ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetDayTime(uint dayPart)", focFUNCTION( BIND_CLASS Global_GetDayTime ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b)", focFUNCTION( BIND_CLASS Global_GetDayColor ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void RunServerScript(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Global_RunServerScript ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void RunServerScriptUnsafe(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Global_RunServerScriptUnsafe ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(string& name, int pathIndex)", focFUNCTION( BIND_CLASS Global_LoadSprite ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(uint nameHash, uint8 dir)", focFUNCTION( BIND_CLASS Global_LoadSpriteHash ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteWidth(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteWidth ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteHeight(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteHeight ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetSpriteCount(uint sprId)", focFUNCTION( BIND_CLASS Global_GetSpriteCount ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetTextInfo(string& text, int w, int h, int font, int flags, int& tw, int& th, int& lines)", focFUNCTION( BIND_CLASS Global_GetTextInfo ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color)", focFUNCTION( BIND_CLASS Global_DrawSprite ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteOffs ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpriteSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteSizeOffs ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSpritePattern(uint sprId, int sprIndex, int x, int y, int w, int h, int sprWidth, int sprHeight, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpritePattern ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawText(string& text, int x, int y, int w, int h, uint color, int font, int flags)", focFUNCTION( BIND_CLASS Global_DrawText ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawPrimitive(int primitiveType, int[]& data)", focFUNCTION( BIND_CLASS Global_DrawPrimitive ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawMapSprite(uint16 hx, uint16 hy, uint16 effectPid, uint sprId, int sprIndex, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_DrawMapSprite ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter2d(uint crType, uint anim1, uint anim2, uint8 dir, int l, int t, int r, int b, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter2d ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter3d(uint instance, uint crType, uint anim1, uint anim2, int[]@+ layers, float[]@+ position, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter3d ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void ShowScreen(int screen, int p0, int p1, int p2)", focFUNCTION( BIND_CLASS Global_ShowScreen ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void HideScreen(int screen, int p0, int p1, int p2)", focFUNCTION( BIND_CLASS Global_HideScreen ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetHardcodedScreenPos(int screen, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHardcodedScreenPos ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawHardcodedScreen(int screen)", focFUNCTION( BIND_CLASS Global_DrawHardcodedScreen ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetHexPos(uint16 hx, uint16 hy, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHexPos ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetMonitorHex(int x, int y, uint16& hx, uint16& hy, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorHex ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint16 GetMapWidth()", focFUNCTION( BIND_CLASS Global_GetMapWidth ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint16 GetMapHeight()", focFUNCTION( BIND_CLASS Global_GetMapHeight ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetCurrentCursor()", focFUNCTION( BIND_CLASS Global_GetCurrentCursor ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetLastCursor()", focFUNCTION( BIND_CLASS Global_GetLastCursor ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void ChangeCursor(int cursor)", focFUNCTION( BIND_CLASS Global_ChangeCursor ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps)", focFUNCTION( BIND_CLASS Global_MoveHexByDir ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool AppendIfaceIni(string& iniName)", focFUNCTION( BIND_CLASS Global_AppendIfaceIni ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetIfaceIniStr(string& key)", focFUNCTION( BIND_CLASS Global_GetIfaceIniStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool Load3dFile(string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_Load3dFile ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void WaitPing()", focFUNCTION( BIND_CLASS Global_WaitPing ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadFont(int font, string& fontFileName)", focFUNCTION( BIND_CLASS Global_LoadFont ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetDefaultFont(int font, uint color)", focFUNCTION( BIND_CLASS Global_SetDefaultFont ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetEffect(int effectType, int effectSubtype, string@+ effectName, string@+ effectDefines = null)", focFUNCTION( BIND_CLASS Global_SetEffect ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void RefreshMap(bool onlyTiles, bool onlyRoof, bool onlyLight)", focFUNCTION( BIND_CLASS Global_RefreshMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MouseClick(int x, int y, int button, int cursor)", focFUNCTION( BIND_CLASS Global_MouseClick ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void KeyboardPress(uint8 key1, uint8 key2, string@+ key1Text = null, string@+ key2Text = null)", focFUNCTION( BIND_CLASS Global_KeyboardPress ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetRainAnimation(string@+ fallAnimName, string@+ dropAnimName)", focFUNCTION( BIND_CLASS Global_SetRainAnimation ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SaveScreenshot()", focFUNCTION( BIND_CLASS Global_SaveScreenshot ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SaveLogFile()", focFUNCTION( BIND_CLASS Global_SaveLogFile ), asCALL_CDECL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( bind == SCRIPT_BIND_MAPPER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetDefaultCritterParam(uint index, int param)", focFUNCTION( BIND_CLASS Global_SetDefaultCritterParam ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMapFileNames(string@+ dir, string@[]@+ names)", focFUNCTION( BIND_CLASS Global_GetMapFileNames ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "uint TabGetTileDirs(int tab, string@[]@+ dirNames, bool[]@+ includeSubdirs)", focFUNCTION( BIND_CLASS Global_TabGetTileDirs ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint TabGetItemPids(int tab, string@+ subTab, uint16[]@+ itemPids)", focFUNCTION( BIND_CLASS Global_TabGetItemPids ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint TabGetCritterPids(int tab, string@+ subTab, uint16[]@+ critterPids)", focFUNCTION( BIND_CLASS Global_TabGetCritterPids ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetTileDirs(int tab, string@[]@+ dirNames, bool[]@+ includeSubdirs)", focFUNCTION( BIND_CLASS Global_TabSetTileDirs ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetItemPids(int tab, string@+ subTab, uint16[]@+ itemPids)", focFUNCTION( BIND_CLASS Global_TabSetItemPids ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetCritterPids(int tab, string@+ subTab, uint16[]@+ critterPids)", focFUNCTION( BIND_CLASS Global_TabSetCritterPids ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void TabDelete(int tab)", focFUNCTION( BIND_CLASS Global_TabDelete ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSelect(int tab, string@+ subTab, bool show = false)", focFUNCTION( BIND_CLASS Global_TabSelect ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetName(int tab, string@+ tabName)", focFUNCTION( BIND_CLASS Global_TabSetName ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanWalk(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanWalk ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRun(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRun ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRotate(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRotate ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanAim(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanAim ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanArmor(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanArmor ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterAnim1(uint crType, uint anim1)", focFUNCTION( BIND_CLASS Global_IsCritterAnim1 ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetCritterAnimType(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAnimType ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCritterAlias(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAlias ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterTypeName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterTypeName ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterSoundName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterSoundName ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist)", focFUNCTION( BIND_CLASS Global_GetHexInPath ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthHex ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(string& text)", focFUNCTION( BIND_CLASS Global_Message ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_MessageMsg ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MapMessage(string& text, uint16 hx, uint16 hy, uint timeMs, uint color, bool fade, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_MapMessage ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_IsMsgStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMsgStrSkip ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumUpper(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumUpper ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumLower(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumLower ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrCount(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrCount ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, const string& str)", focFUNCTION( BIND_CLASS Global_ReplaceTextStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, int i)", focFUNCTION( BIND_CLASS Global_ReplaceTextInt ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveScreen(uint16 hexX, uint16 hexY, uint speed)", focFUNCTION( BIND_CLASS Global_MoveScreen ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(string& name, int pathIndex)", focFUNCTION( BIND_CLASS Global_LoadSprite ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(uint nameHash, uint8 dir)", focFUNCTION( BIND_CLASS Global_LoadSpriteHash ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteWidth(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteWidth ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteHeight(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteHeight ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetSpriteCount(uint sprId)", focFUNCTION( BIND_CLASS Global_GetSpriteCount ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetTextInfo(string& text, int w, int h, int font, int flags, int& tw, int& th, int& lines)", focFUNCTION( BIND_CLASS Global_GetTextInfo ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color)", focFUNCTION( BIND_CLASS Global_DrawSprite ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteOffs ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpriteSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteSizeOffs ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSpritePattern(uint sprId, int sprIndex, int x, int y, int w, int h, int sprWidth, int sprHeight, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpritePattern ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawText(string& text, int x, int y, int w, int h, uint color, int font, int flags)", focFUNCTION( BIND_CLASS Global_DrawText ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawPrimitive(int primitiveType, int[]& data)", focFUNCTION( BIND_CLASS Global_DrawPrimitive ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawMapSprite(uint16 hx, uint16 hy, uint16 effectPid, uint sprId, int sprIndex, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_DrawMapSprite ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter2d(uint crType, uint anim1, uint anim2, uint8 dir, int l, int t, int r, int b, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter2d ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter3d(uint instance, uint crType, uint anim1, uint anim2, int[]@+ layers, float[]@+ position, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter3d ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetHexPos(uint16 hx, uint16 hy, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHexPos ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetMonitorHex(int x, int y, uint16& hx, uint16& hy, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorHex ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps)", focFUNCTION( BIND_CLASS Global_MoveHexByDir ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetIfaceIniStr(string& key)", focFUNCTION( BIND_CLASS Global_GetIfaceIniStr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadFont(int font, string& fontFileName)", focFUNCTION( BIND_CLASS Global_LoadFont ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetDefaultFont(int font, uint color)", focFUNCTION( BIND_CLASS Global_SetDefaultFont ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MouseClick(int x, int y, int button, int cursor)", focFUNCTION( BIND_CLASS Global_MouseClick ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void KeyboardPress(uint8 key1, uint8 key2, string@+ key1Text = null, string@+ key2Text = null)", focFUNCTION( BIND_CLASS Global_KeyboardPress ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetRainAnimation(string@+ fallAnimName, string@+ dropAnimName)", focFUNCTION( BIND_CLASS Global_SetRainAnimation ), asCALL_CDECL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        // AnyData
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int64[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int32[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int16[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int8[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint64[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint32[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint16[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint8[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int64[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int32[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int16[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int8[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint64[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint32[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint16[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint8[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int64[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int32[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int16[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int8[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint64[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint32[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint16[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint8[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsAnyData(string& name)", focFUNCTION( BIND_CLASS Global_IsAnyData ), asCALL_CDECL ) );
        // BIND_ASSERT( engine->RegisterGlobalFunction( "bool AnyDataClass(?& storedClass, ?[]& array)", focFUNCTION( BIND_CLASS Global_AnyDataClass ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void EraseAnyData(string& name)", focFUNCTION( BIND_CLASS Global_EraseAnyData ), asCALL_CDECL ) );
        // TimeEvent
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventEmpty ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, uint value, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValue ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, int value, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValue ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, uint[]& values, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValues ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, int[]& values, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValues ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool EraseTimeEvent(uint num)", focFUNCTION( BIND_CLASS Global_EraseTimeEvent ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetTimeEvent(uint num, uint& duration, uint[]@+ values)", focFUNCTION( BIND_CLASS Global_GetTimeEvent ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetTimeEvent(uint num, uint& duration, int[]@+ values)", focFUNCTION( BIND_CLASS Global_GetTimeEvent ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetTimeEvent(uint num, uint duration, uint[]@+ values)", focFUNCTION( BIND_CLASS Global_SetTimeEvent ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetTimeEvent(uint num, uint duration, int[]@+ values)", focFUNCTION( BIND_CLASS Global_SetTimeEvent ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "void RadioMessage(uint16 channel, string& text)", focFUNCTION( BIND_CLASS Global_RadioMessage ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void RadioMessageMsg(uint16 channel, uint16 textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_RadioMessageMsg ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void RadioMessageMsg(uint16 channel, uint16 textMsg, uint strNum, string@+ lexems)", focFUNCTION( BIND_CLASS Global_RadioMessageMsgLex ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool RunDialog(Critter& player, Critter& npc, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogNpc ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool RunDialog(Critter& player, Critter& npc, uint dialogPack, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogNpcDlgPack ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool RunDialog(Critter& player, uint dialogPack, uint16 hexX, uint16 hexY, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogHex ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int64 WorldItemCount(uint16 protoId)", focFUNCTION( BIND_CLASS Global_WorldItemCount ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetBestScore(int score, Critter@+ player, string& name)", focFUNCTION( BIND_CLASS Global_SetBestScore ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool AddTextListener(int sayType, string& firstStr, uint16 parameter, string& scriptName)", focFUNCTION( BIND_CLASS Global_AddTextListener ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void EraseTextListener(int sayType, string& firstStr, uint16 parameter)", focFUNCTION( BIND_CLASS Global_EraseTextListener ), asCALL_CDECL ) );
        // BIND_ASSERT( engine->RegisterGlobalFunction( "uint8 ReverseDir(uint8 dir)", focFUNCTION( BIND_CLASS Global_ReverseDir ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetBagItems(uint bagId, uint16[]@+ pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots)", focFUNCTION( BIND_CLASS Global_GetBagItems ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetChosenSendParameter(int index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetChosenSendParameter ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetSendParameter(int index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetSendParameter ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetSendParameter(int index, bool enabled, string@+ allowFunc)", focFUNCTION( BIND_CLASS Global_SetSendParameterFunc ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SwapCritters(Critter& cr1, Critter& cr2, bool withInventory, bool withVars)", focFUNCTION( BIND_CLASS Global_SwapCritters ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllMaps(uint16 pid, Map@[]@+ maps)", focFUNCTION( BIND_CLASS Global_GetAllMaps ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllLocations(uint16 pid, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetAllLocations ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetScriptId(string& scriptName, string& funcDeclaration)", focFUNCTION( BIND_CLASS Global_GetScriptId ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetScriptName(uint scriptId)", focFUNCTION( BIND_CLASS Global_GetScriptName ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int8[]@ GetItemDataMask(int maskType)", focFUNCTION( BIND_CLASS Global_GetItemDataMask ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetItemDataMask(int maskType, int8[]& mask)", focFUNCTION( BIND_CLASS Global_SetItemDataMask ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadImage(uint index, string@+ imageName, uint imageDepth, int pathType)", focFUNCTION( BIND_CLASS Global_LoadImage ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetImageColor(uint index, uint x, uint y)", focFUNCTION( BIND_CLASS Global_GetImageColor ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetParameterDialogGetBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterDialogGetBehaviour ), asCALL_CDECL ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_CLIENT || bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetFullSecond(uint16 year, uint16 month, uint16 day, uint16 hour, uint16 minute, uint16 second)", focFUNCTION( BIND_CLASS Global_GetFullSecond ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetTime(uint16& year, uint16& month, uint16& day, uint16& dayOfWeek, uint16& hour, uint16& minute, uint16& second, uint16& milliseconds)", focFUNCTION( BIND_CLASS Global_GetTime ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetGameTime(uint fullSecond, uint16& year, uint16& month, uint16& day, uint16& dayOfWeek, uint16& hour, uint16& minute, uint16& second)", focFUNCTION( BIND_CLASS Global_GetGameTime ), asCALL_CDECL ) );
        // BIND_ASSERT( engine->RegisterGlobalFunction( "void GetVersion(uint& server, uint& client, uint& net)", focFUNCTION( BIND_CLASS Global_GetVersion ), asCALL_CDECL ) );
        // BIND_ASSERT( engine->RegisterGlobalFunction( "void Sort(int[]& array, string& func)", focFUNCTION( BIND_CLASS Global_Sort ), asCALL_CDECL ) );
        // BIND_ASSERT( engine->RegisterGlobalFunction( "void Sort(uint[]& array, string& func)", focFUNCTION( BIND_CLASS Global_Sort ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetParameterGetBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterGetBehaviour ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetParameterChangeBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterChangeBehaviour ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetRegistrationParameter(uint index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetRegistrationParam ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanWalk(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanWalk ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRun(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRun ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRotate(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRotate ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanAim(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanAim ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanArmor(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanArmor ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterAnim1(uint crType, uint anim1)", focFUNCTION( BIND_CLASS Global_IsCritterAnim1 ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetCritterAnimType(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAnimType ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCritterAlias(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAlias ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterTypeName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterTypeName ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterSoundName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterSoundName ), asCALL_CDECL ) );
    }
    #endif


    /*******************************************************************************************************************************/

    //
    //   CraftItem
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "CraftItem@ GetCraftItem(uint num)", focFUNCTION( BIND_CLASS Global_GetCraftItem ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const uint Num", focOFFSET( CraftItem, Num ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const string Name", focOFFSET( CraftItem, Name ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const string Info", focOFFSET( CraftItem, Info ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const uint Experience", focOFFSET( CraftItem, Experience ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const string Script", focOFFSET( CraftItem, Script ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetShowParams(array<uint>@+ nums, array<int>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetShowParams ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetNeedParams(array<uint>@+ nums, array<int>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedParams ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetNeedTools(array<uint16>@+ pids, array<uint>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedTools ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetNeedItems(array<uint16>@+ pids, array<uint>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedItems ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetOutItems(array<uint16>@+ pids, array<uint>@+ values)", focFUNCTION( BIND_CLASS CraftItem_GetOutItems ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif


    //
    // Critter
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        // Critter (any)
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersDistantion(Critter& cr1, Critter& cr2)", focFUNCTION( BIND_CLASS Global_GetCrittersDistantion ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetGlobalMapCritters(uint16 worldX, uint16 worldY, uint radius, int findType, Critter@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetGlobalMapCritters ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "Critter@+ GetCritter(uint critterId)", focFUNCTION( BIND_CLASS Global_GetCritter ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsPlayer() const", focFUNCTION( BIND_CLASS Crit_IsPlayer ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsNpc() const", focFUNCTION( BIND_CLASS Crit_IsNpc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanWalk() const", focFUNCTION( BIND_CLASS Crit_IsCanWalk ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanRun() const", focFUNCTION( BIND_CLASS Crit_IsCanRun ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanRotate() const", focFUNCTION( BIND_CLASS Crit_IsCanRotate ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanAim() const", focFUNCTION( BIND_CLASS Crit_IsCanAim ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsAnim1(uint index) const", focFUNCTION( BIND_CLASS Crit_IsAnim1 ), asCALL_CDECL_OBJFIRST ) );

        // Npc
        BIND_ASSERT( engine->RegisterGlobalFunction( "void GetProtoCritter(uint16 protoId, int[]& data)", focFUNCTION( BIND_CLASS Global_GetProtoCritter ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteNpc(Critter& npc)", focFUNCTION( BIND_CLASS Global_DeleteNpc ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllNpc(uint16 pid, Critter@[]@+ npc)", focFUNCTION( BIND_CLASS Global_GetAllNpc ), asCALL_CDECL ) );
        // Player
        BIND_ASSERT( engine->RegisterGlobalFunction( "Critter@+ GetPlayer(string& name)", focFUNCTION( BIND_CLASS Global_GetPlayer ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPlayerId(string& name)", focFUNCTION( BIND_CLASS Global_GetPlayerId ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetPlayerName(uint playerId)", focFUNCTION( BIND_CLASS Global_GetPlayerName ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllPlayers(Critter@[]@+ players)", focFUNCTION( BIND_CLASS Global_GetAllPlayers ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetRegisteredPlayers(uint[]& ids, string@[]& names)", focFUNCTION( BIND_CLASS Global_GetRegisteredPlayers ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint8 GetAccess() const", focFUNCTION( BIND_CLASS Cl_GetAccess ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetAccess(uint8 access) const", focFUNCTION( BIND_CLASS Cl_SetAccess ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Crit_SetEvent ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetLexems(string@+ lexems)", focFUNCTION( BIND_CLASS Crit_SetLexems ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Map@+ GetMap() const", focFUNCTION( BIND_CLASS Crit_GetMap ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetMapId() const", focFUNCTION( BIND_CLASS Crit_GetMapId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint16 GetMapProtoId() const", focFUNCTION( BIND_CLASS Crit_GetMapProtoId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetHomePos(uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Crit_SetHomePos ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GetHomePos(uint& mapId, uint16& hexX, uint16& hexY, uint8& dir)", focFUNCTION( BIND_CLASS Crit_GetHomePos ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ChangeCrType(uint newType)", focFUNCTION( BIND_CLASS Crit_ChangeCrType ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void DropTimers()", focFUNCTION( BIND_CLASS Cl_DropTimers ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool MoveRandom()", focFUNCTION( BIND_CLASS Crit_MoveRandom ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool MoveToDir(uint8 dir)", focFUNCTION( BIND_CLASS Crit_MoveToDir ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToHex(uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Crit_TransitToHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToMap(uint mapId, uint16 hexX, uint16 hexY, uint8 dir, bool withGroup = false)", focFUNCTION( BIND_CLASS Crit_TransitToMapHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToMap(uint mapId, int entireNum, bool withGroup = false)", focFUNCTION( BIND_CLASS Crit_TransitToMapEntire ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToGlobal(bool requestGroup)", focFUNCTION( BIND_CLASS Crit_TransitToGlobal ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToGlobal(Critter@[]& group)", focFUNCTION( BIND_CLASS Crit_TransitToGlobalWithGroup ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToGlobalGroup(uint critterId)", focFUNCTION( BIND_CLASS Crit_TransitToGlobalGroup ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void AddScore(uint score, int val)", focFUNCTION( BIND_CLASS Crit_AddScore ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int GetScore(uint score)", focFUNCTION( BIND_CLASS Crit_GetScore ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void AddHolodiskInfo(uint holodiskNum)", focFUNCTION( BIND_CLASS Crit_AddHolodiskInfo ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EraseHolodiskInfo(uint holodiskNum)", focFUNCTION( BIND_CLASS Crit_EraseHolodiskInfo ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsHolodiskInfo(uint holodiskNum) const", focFUNCTION( BIND_CLASS Crit_IsHolodiskInfo ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsLife() const", focFUNCTION( BIND_CLASS Crit_IsLife ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsKnockout() const", focFUNCTION( BIND_CLASS Crit_IsKnockout ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsDead() const", focFUNCTION( BIND_CLASS Crit_IsDead ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsFree() const", focFUNCTION( BIND_CLASS Crit_IsFree ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsBusy() const", focFUNCTION( BIND_CLASS Crit_IsBusy ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Wait(uint ms)", focFUNCTION( BIND_CLASS Crit_Wait ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ToDead(uint anim2, Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_ToDead ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ToLife()", focFUNCTION( BIND_CLASS Crit_ToLife ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ToKnockout(uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint16 knockHx, uint16 knockHy)", focFUNCTION( BIND_CLASS Crit_ToKnockout ), asCALL_CDECL_OBJFIRST ) );
        // BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GetProtoData(int[]& data)", focFUNCTION( BIND_CLASS Npc_GetProtoData ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void RefreshVisible()", focFUNCTION( BIND_CLASS Crit_RefreshVisible ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ViewMap(Map& map, uint look, uint16 hx, uint16 hy, uint8 dir)", focFUNCTION( BIND_CLASS Crit_ViewMap ), asCALL_CDECL_OBJFIRST ) );
        // BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Mute(uint ms)", focFUNCTION( BIND_CLASS Crit_Mute ), asCALL_CDECL_OBJFIRST ) );
        // BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Ban(uint ms)", focFUNCTION( BIND_CLASS Crit_Mute ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Item@+ AddItem(uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Crit_AddItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool DeleteItem(uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Crit_DeleteItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ItemsCount() const", focFUNCTION( BIND_CLASS Crit_ItemsCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ItemsWeight() const", focFUNCTION( BIND_CLASS Crit_ItemsWeight ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ItemsVolume() const", focFUNCTION( BIND_CLASS Crit_ItemsVolume ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint CountItem(uint16 protoId) const", focFUNCTION( BIND_CLASS Crit_CountItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Item@+ GetItem(uint16 protoId, int slot) const", focFUNCTION( BIND_CLASS Crit_GetItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Item@+ GetItemById(uint itemId) const", focFUNCTION( BIND_CLASS Crit_GetItemById ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetItems(int slot, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItems ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetItemsByType(int type, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItemsByType ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "ProtoItem@+ GetSlotProto(int slot, uint8& mode) const", focFUNCTION( BIND_CLASS Crit_GetSlotProto ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool MoveItem(uint itemId, uint count, uint8 toSlot)", focFUNCTION( BIND_CLASS Crit_MoveItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool PickItem(uint16 hexX, uint16 hexY, uint16 protoId)", focFUNCTION( BIND_CLASS Crit_PickItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetFavoriteItem(int slot, uint16 pid)", focFUNCTION( BIND_CLASS Crit_SetFavoriteItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint16 GetFavoriteItem(int slot)", focFUNCTION( BIND_CLASS Crit_GetFavoriteItem ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetCritters(bool lookOnMe, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Crit_GetCritters ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetFollowGroup(int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Crit_GetFollowGroup ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Critter@+ GetFollowLeader() const", focFUNCTION( BIND_CLASS Crit_GetFollowLeader ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Critter@[]@ GetGlobalGroup() const", focFUNCTION( BIND_CLASS Crit_GetGlobalGroup ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsGlobalGroupLeader()", focFUNCTION( BIND_CLASS Crit_IsGlobalGroupLeader ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void LeaveGlobalGroup()", focFUNCTION( BIND_CLASS Crit_LeaveGlobalGroup ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GiveGlobalGroupLead(Critter& toCr)", focFUNCTION( BIND_CLASS Crit_GiveGlobalGroupLead ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetTalkedPlayers(Critter@[]@+ players) const", focFUNCTION( BIND_CLASS Npc_GetTalkedPlayers ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsSee(Critter& cr) const", focFUNCTION( BIND_CLASS Crit_IsSeeCr ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsSeenBy(Critter& cr) const", focFUNCTION( BIND_CLASS Crit_IsSeenByCr ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsSee(Item& item) const", focFUNCTION( BIND_CLASS Crit_IsSeeItem ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Say(uint8 howSay, string& text)", focFUNCTION( BIND_CLASS Crit_Say ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SayMsg(uint8 howSay, uint16 textMsg, uint strNum)", focFUNCTION( BIND_CLASS Crit_SayMsg ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SayMsg(uint8 howSay, uint16 textMsg, uint strNum, string& lexems)", focFUNCTION( BIND_CLASS Crit_SayMsgLex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetDir(uint8 dir)", focFUNCTION( BIND_CLASS Crit_SetDir ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ErasePlane(int planeType, bool all)", focFUNCTION( BIND_CLASS Npc_ErasePlane ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ErasePlane(uint index)", focFUNCTION( BIND_CLASS Npc_ErasePlaneIndex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void DropPlanes()", focFUNCTION( BIND_CLASS Npc_DropPlanes ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsNoPlanes() const", focFUNCTION( BIND_CLASS Npc_IsNoPlanes ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCurPlane(int planeType) const", focFUNCTION( BIND_CLASS Npc_IsCurPlane ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "NpcPlane@+ GetCurPlane() const", focFUNCTION( BIND_CLASS Npc_GetCurPlane ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetPlanes(NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanes ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetPlanes(int identifier, NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanesIdentifier ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetPlanes(int identifier, uint identifierExt, NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanesIdentifier2 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool AddPlane(NpcPlane& plane)", focFUNCTION( BIND_CLASS Npc_AddPlane ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SendMessage(int num, int val, int to)", focFUNCTION( BIND_CLASS Crit_SendMessage ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Action(int action, int actionExt, Item@+ item)", focFUNCTION( BIND_CLASS Crit_Action ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Animate(uint anim1, uint anim2, Item@+ item, bool clearSequence, bool delayPlay)", focFUNCTION( BIND_CLASS Crit_Animate ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetAnims(int cond, uint anim1, uint anim2)", focFUNCTION( BIND_CLASS Crit_SetAnims ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void PlaySound(string& soundName, bool sendSelf)", focFUNCTION( BIND_CLASS Crit_PlaySound ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void PlaySound(uint8 soundType, uint8 soundTypeExt, uint8 soundId, uint8 soundIdExt, bool sendSelf)", focFUNCTION( BIND_CLASS Crit_PlaySoundType ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SendCombatResult(uint[]& combatResult)", focFUNCTION( BIND_CLASS Crit_SendCombatResult ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsKnownLoc(bool byId, uint locNum) const", focFUNCTION( BIND_CLASS Cl_IsKnownLoc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool SetKnownLoc(bool byId, uint locNum)", focFUNCTION( BIND_CLASS Cl_SetKnownLoc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool UnsetKnownLoc(bool byId, uint locNum)", focFUNCTION( BIND_CLASS Cl_UnsetKnownLoc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetFog(uint16 zoneX, uint16 zoneY, int fog)", focFUNCTION( BIND_CLASS Cl_SetFog ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int GetFog(uint16 zoneX, uint16 zoneY)", focFUNCTION( BIND_CLASS Cl_GetFog ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ShowContainer(Critter@+ contCr, Item@+ contItem, uint8 transferType)", focFUNCTION( BIND_CLASS Cl_ShowContainer ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ShowScreen(int screenType, uint param, string@+ funcName)", focFUNCTION( BIND_CLASS Cl_ShowScreen ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void RunClientScript(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Cl_RunClientScript ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Disconnect()", focFUNCTION( BIND_CLASS Cl_Disconnect ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Crit_SetScript ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Crit_GetScriptId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetBagRefreshTime(uint realMinutes)", focFUNCTION( BIND_CLASS Crit_SetBagRefreshTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetBagRefreshTime() const", focFUNCTION( BIND_CLASS Crit_GetBagRefreshTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetInternalBag(uint16[]& pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots)", focFUNCTION( BIND_CLASS Crit_SetInternalBag ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetInternalBag(uint16[]@+ pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots) const", focFUNCTION( BIND_CLASS Crit_GetInternalBag ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Crit_GetProtoId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetMultihex() const", focFUNCTION( BIND_CLASS Crit_GetMultihex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetMultihex(int value)", focFUNCTION( BIND_CLASS Crit_SetMultihex ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void AddEnemyInStack(uint critterId)", focFUNCTION( BIND_CLASS Crit_AddEnemyInStack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool CheckEnemyInStack(uint critterId) const", focFUNCTION( BIND_CLASS Crit_CheckEnemyInStack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EraseEnemyFromStack(uint critterId)", focFUNCTION( BIND_CLASS Crit_EraseEnemyFromStack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ChangeEnemyStackSize(uint newSize)", focFUNCTION( BIND_CLASS Crit_ChangeEnemyStackSize ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GetEnemyStack(uint[]& enemyStack) const", focFUNCTION( BIND_CLASS Crit_GetEnemyStack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ClearEnemyStack()", focFUNCTION( BIND_CLASS Crit_ClearEnemyStack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ClearEnemyStackNpc()", focFUNCTION( BIND_CLASS Crit_ClearEnemyStackNpc ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool AddTimeEvent(string& funcName, uint duration, int identifier)", focFUNCTION( BIND_CLASS Crit_AddTimeEvent ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool AddTimeEvent(string& funcName, uint duration, int identifier, uint rate)", focFUNCTION( BIND_CLASS Crit_AddTimeEventRate ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetTimeEvents(int identifier, uint[]@+ indexes, uint[]@+ durations, uint[]@+ rates) const", focFUNCTION( BIND_CLASS Crit_GetTimeEvents ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetTimeEvents(int[]& findIdentifiers, int[]@+ identifiers, uint[]@+ indexes, uint[]@+ durations, uint[]@+ rates) const", focFUNCTION( BIND_CLASS Crit_GetTimeEventsArr ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ChangeTimeEvent(uint index, uint newDuration, uint newRate)", focFUNCTION( BIND_CLASS Crit_ChangeTimeEvent ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EraseTimeEvent(uint index)", focFUNCTION( BIND_CLASS Crit_EraseTimeEvent ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint EraseTimeEvents(int identifier)", focFUNCTION( BIND_CLASS Crit_EraseTimeEvents ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint EraseTimeEvents(int[]& identifiers)", focFUNCTION( BIND_CLASS Crit_EraseTimeEventsArr ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventIdle()", focFUNCTION( BIND_CLASS Crit_EventIdle ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Crit_EventFinish ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventDead(Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_EventDead ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventRespawn()", focFUNCTION( BIND_CLASS Crit_EventRespawn ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter1(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter1 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter2(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter2 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter3(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter3 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter1(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter1 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter2(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter2 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter3(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter3 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowItemOnMap(Item& showItem, bool added, Critter@+ dropper)", focFUNCTION( BIND_CLASS Crit_EventShowItemOnMap ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventChangeItemOnMap(Item& item)", focFUNCTION( BIND_CLASS Crit_EventChangeItemOnMap ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideItemOnMap(Item& hideItem, bool removed, Critter@+ picker)", focFUNCTION( BIND_CLASS Crit_EventHideItemOnMap ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventAttack(Critter& target)", focFUNCTION( BIND_CLASS Crit_EventAttack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventAttacked(Critter@+ attacker)", focFUNCTION( BIND_CLASS Crit_EventAttacked ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventStealing(Critter& thief, Item& item, uint count)", focFUNCTION( BIND_CLASS Crit_EventStealing ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventMessage(Critter& fromCr, int message, int value)", focFUNCTION( BIND_CLASS Crit_EventMessage ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseItem(Item& item, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventUseItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseItemOnMe(Critter& whoUse, Item& item)", focFUNCTION( BIND_CLASS Crit_EventUseItemOnMe ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseSkill(int skill, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventUseSkill ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseSkillOnMe(Critter& whoUse, int skill)", focFUNCTION( BIND_CLASS Crit_EventUseSkillOnMe ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventDropItem(Item& item)", focFUNCTION( BIND_CLASS Crit_EventDropItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventMoveItem(Item& item, uint8 fromSlot)", focFUNCTION( BIND_CLASS Crit_EventMoveItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventKnockout(uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint knockDist)", focFUNCTION( BIND_CLASS Crit_EventKnockout ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthDead(Critter& fromCr, Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_EventSmthDead ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthStealing(Critter& fromCr, Critter& thief, bool success, Item& item, uint count)", focFUNCTION( BIND_CLASS Crit_EventSmthStealing ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthAttack(Critter& fromCr, Critter& target)", focFUNCTION( BIND_CLASS Crit_EventSmthAttack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthAttacked(Critter& fromCr, Critter@+ attacker)", focFUNCTION( BIND_CLASS Crit_EventSmthAttacked ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthUseItem(Critter& fromCr, Item& item, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventSmthUseItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthUseSkill(Critter& fromCr, int skill, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventSmthUseSkill ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthDropItem(Critter& fromCr, Item& item)", focFUNCTION( BIND_CLASS Crit_EventSmthDropItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthMoveItem(Critter& fromCr, Item& item, uint8 fromSlot)", focFUNCTION( BIND_CLASS Crit_EventSmthMoveItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthKnockout(Critter& fromCr, uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint knockDist)", focFUNCTION( BIND_CLASS Crit_EventSmthKnockout ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int EventPlaneBegin(NpcPlane& plane, int reason, Critter@+ someCr, Item@+ someItem)", focFUNCTION( BIND_CLASS Crit_EventPlaneBegin ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int EventPlaneEnd(NpcPlane& plane, int reason, Critter@+ someCr, Item@+ someItem)", focFUNCTION( BIND_CLASS Crit_EventPlaneEnd ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int EventPlaneRun(NpcPlane& plane, int reason, uint& p0, uint& p1, uint& p2)", focFUNCTION( BIND_CLASS Crit_EventPlaneRun ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventBarter(Critter& barterCr, bool attach, uint barterCount)", focFUNCTION( BIND_CLASS Crit_EventBarter ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventTalk(Critter& talkCr, bool attach, uint talkCount)", focFUNCTION( BIND_CLASS Crit_EventTalk ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventGlobalProcess(int type, Item@ car, float& x, float& y, float& toX, float& toY, float& speed, uint& encounterDescriptor, bool& waitForAnswer)", focFUNCTION( BIND_CLASS Crit_EventGlobalProcess ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventGlobalInvite(Item@ car, uint encounterDescriptor, int combatMode, uint& mapId, uint16& hexX, uint16& hexY, uint8& dir)", focFUNCTION( BIND_CLASS Crit_EventGlobalInvite ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventTurnBasedProcess(Map& map, bool beginTurn)", focFUNCTION( BIND_CLASS Crit_EventTurnBasedProcess ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthTurnBasedProcess(Critter& fromCr, Map& map, bool beginTurn)", focFUNCTION( BIND_CLASS Crit_EventSmthTurnBasedProcess ), asCALL_CDECL_OBJFIRST ) );

        // Parameters
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Id", focOFFSET( Critter, Data ) + focOFFSET( CritData, Id ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint CrType", focOFFSET( Critter, Data ) + focOFFSET( CritData, BaseType ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint16 HexX", focOFFSET( Critter, Data ) + focOFFSET( CritData, HexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint16 HexY", focOFFSET( Critter, Data ) + focOFFSET( CritData, HexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint16 WorldX", focOFFSET( Critter, Data ) + focOFFSET( CritData, WorldX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint16 WorldY", focOFFSET( Critter, Data ) + focOFFSET( CritData, WorldY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint8 Dir", focOFFSET( Critter, Data ) + focOFFSET( CritData, Dir ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint8 Cond", focOFFSET( Critter, Data ) + focOFFSET( CritData, Cond ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Anim1Life", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim1Life ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Anim1Knockout", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim1Knockout ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Anim1Dead", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim1Dead ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Anim2Life", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim2Life ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Anim2Knockout", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim2Knockout ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Anim2Dead", focOFFSET( Critter, Data ) + focOFFSET( CritData, Anim2Dead ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const uint Flags", focOFFSET( Critter, Flags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const string Name", focOFFSET( Critter, NameStr ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "uint ShowCritterDist1", focOFFSET( Critter, Data ) + focOFFSET( CritData, ShowCritterDist1 ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "uint ShowCritterDist2", focOFFSET( Critter, Data ) + focOFFSET( CritData, ShowCritterDist2 ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "uint ShowCritterDist3", focOFFSET( Critter, Data ) + focOFFSET( CritData, ShowCritterDist3 ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "bool IsRuning", focOFFSET( Critter, IsRuning ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const bool IsNotValid", focOFFSET( Critter, IsNotValid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "const int Ref", focOFFSET( Critter, RefCounter ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "DataVal Param", focOFFSET( Critter, ThisPtr[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Critter", "DataRef ParamBase", focOFFSET( Critter, ThisPtr[0] ) ) );
    }
    #endif

    //
    // CritterCl
    //

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( bind == SCRIPT_BIND_CLIENT )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "CritterCl@+ GetChosen()", focFUNCTION( BIND_CLASS Global_GetChosen ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetChosenActions(uint[]@+ actions)", focFUNCTION( BIND_CLASS Global_GetChosenActions ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SetChosenActions(uint[]@+ actions)", focFUNCTION( BIND_CLASS Global_SetChosenActions ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "CritterCl@+ GetCritter(uint critterId)", focFUNCTION( BIND_CLASS Global_GetCritter ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersHex(uint16 hexX, uint16 hexY, uint radius, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCritters ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCritters(uint16 pid, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCrittersByPids ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersDistantion(CritterCl& cr1, CritterCl& cr2)", focFUNCTION( BIND_CLASS Global_GetCrittersDistantion ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCrittersInPath ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, CritterCl@[]@+ critters, uint16& preBlockHx, uint16& preBlockHy, uint16& blockHx, uint16& blockHy)", focFUNCTION( BIND_CLASS Global_GetCrittersInPathBlock ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "CritterCl@+ GetMonitorCritter(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorCritter ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Id", focOFFSET( CritterCl, Id ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint16 Pid", focOFFSET( CritterCl, Pid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint CrType", focOFFSET( CritterCl, BaseType ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint CrTypeAlias", focOFFSET( CritterCl, BaseTypeAlias ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint16 HexX", focOFFSET( CritterCl, HexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint16 HexY", focOFFSET( CritterCl, HexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint8 Dir", focOFFSET( CritterCl, CrDir ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint8 Cond", focOFFSET( CritterCl, Cond ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Anim1Life", focOFFSET( CritterCl, Anim1Life ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Anim1Knockout", focOFFSET( CritterCl, Anim1Knockout ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Anim1Dead", focOFFSET( CritterCl, Anim1Dead ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Anim2Life", focOFFSET( CritterCl, Anim2Life ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Anim2Knockout", focOFFSET( CritterCl, Anim2Knockout ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Anim2Dead", focOFFSET( CritterCl, Anim2Dead ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const uint Flags", focOFFSET( CritterCl, Flags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "string Name", focOFFSET( CritterCl, Name ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "string NameOnHead", focOFFSET( CritterCl, NameOnHead ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "string Lexems", focOFFSET( CritterCl, Lexems ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "uint NameColor", focOFFSET( CritterCl, NameColor ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "const int16 Ref", focOFFSET( CritterCl, RefCounter ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "DataVal Param", focOFFSET( CritterCl, ThisPtr[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "DataRef ParamBase", focOFFSET( CritterCl, ThisPtr[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "CritterCl", "int[]@ Anim3dLayer", focOFFSET( CritterCl, Layers3d ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsChosen() const", focFUNCTION( BIND_CLASS Crit_IsChosen ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsPlayer() const", focFUNCTION( BIND_CLASS Crit_IsPlayer ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsNpc() const", focFUNCTION( BIND_CLASS Crit_IsNpc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsLife() const", focFUNCTION( BIND_CLASS Crit_IsLife ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsKnockout() const", focFUNCTION( BIND_CLASS Crit_IsKnockout ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsDead() const", focFUNCTION( BIND_CLASS Crit_IsDead ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsFree() const", focFUNCTION( BIND_CLASS Crit_IsFree ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsBusy() const", focFUNCTION( BIND_CLASS Crit_IsBusy ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsAnim3d() const", focFUNCTION( BIND_CLASS Crit_IsAnim3d ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsAnimAviable(uint anim1, uint anim2) const", focFUNCTION( BIND_CLASS Crit_IsAnimAviable ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsAnimPlaying() const", focFUNCTION( BIND_CLASS Crit_IsAnimPlaying ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetAnim1() const", focFUNCTION( BIND_CLASS Crit_GetAnim1 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void Animate(uint anim1, uint anim2)", focFUNCTION( BIND_CLASS Crit_Animate ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void Animate(uint anim1, uint anim2, ItemCl@+ item)", focFUNCTION( BIND_CLASS Crit_AnimateEx ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void ClearAnim()", focFUNCTION( BIND_CLASS Crit_ClearAnim ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void Wait(uint ms)", focFUNCTION( BIND_CLASS Crit_Wait ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint ItemsCount() const", focFUNCTION( BIND_CLASS Crit_ItemsCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint ItemsWeight() const", focFUNCTION( BIND_CLASS Crit_ItemsWeight ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint ItemsVolume() const", focFUNCTION( BIND_CLASS Crit_ItemsVolume ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint CountItem(uint16 protoId) const", focFUNCTION( BIND_CLASS Crit_CountItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "ItemCl@+ GetItem(uint16 protoId, int slot) const", focFUNCTION( BIND_CLASS Crit_GetItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetItems(int slot, ItemCl@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItems ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetItemsByType(int type, ItemCl@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItemsByType ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "ProtoItem@+ GetSlotProto(int slot, uint8& mode) const", focFUNCTION( BIND_CLASS Crit_GetSlotProto ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void SetVisible(bool visible)", focFUNCTION( BIND_CLASS Crit_SetVisible ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool GetVisible() const", focFUNCTION( BIND_CLASS Crit_GetVisible ), asCALL_CDECL_OBJFIRST ) );
        // BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsMyTurn() const", focFUNCTION( BIND_CLASS Crit_CheckKey ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void set_ContourColor(uint value)", focFUNCTION( BIND_CLASS Crit_set_ContourColor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint get_ContourColor() const", focFUNCTION( BIND_CLASS Crit_get_ContourColor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetMultihex() const", focFUNCTION( BIND_CLASS Crit_GetMultihex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsTurnBasedTurn() const", focFUNCTION( BIND_CLASS Crit_IsTurnBasedTurn ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // DataRef
    // Reference value
    //

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_CLIENT || bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectMethod( "DataRef", "const int& opIndex(uint) const", focFUNCTION( BIND_CLASS DataRef_Index ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectMethod( "DataRef", "int& opIndex(uint)", focFUNCTION( BIND_CLASS DataRef_Index ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // DataVal
    // Computed value
    //
    #if defined (BIND_DUMMY) || defined (BIND_CLIENT) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_CLIENT || bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectMethod( "DataVal", "const int opIndex(uint) const", focFUNCTION( BIND_CLASS DataVal_Index ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // GameVar
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "GameVar@+ GetGlobalVar(uint16 varId)", focFUNCTION( BIND_CLASS Global_GetGlobalVar ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "GameVar@+ GetLocalVar(uint16 varId, uint masterId)", focFUNCTION( BIND_CLASS Global_GetLocalVar ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "GameVar@+ GetUnicumVar(uint16 varId, uint masterId, uint slaveId)", focFUNCTION( BIND_CLASS Global_GetUnicumVar ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int GetValue() const", focMETHOD( GameVar, GetValue ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int GetMin() const", focMETHOD( GameVar, GetMin ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int GetMax() const", focMETHOD( GameVar, GetMax ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "bool IsQuest() const", focMETHOD( GameVar, IsQuest ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "uint GetQuestStr() const", focMETHOD( GameVar, GetQuestStr ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAddAssign(const int)", focMETHODPR( GameVar, operator+=, (const int), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opSubAssign(const int)", focMETHODPR( GameVar, operator-=, (const int), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opMulAssign(const int)", focMETHODPR( GameVar, operator*=, (const int), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opDivAssign(const int)", focMETHODPR( GameVar, operator/=, (const int), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAssign(const int)", focMETHODPR( GameVar, operator=, (const int), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAddAssign(const GameVar&)", focMETHODPR( GameVar, operator+=, (const GameVar &), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opSubAssign(const GameVar&)", focMETHODPR( GameVar, operator-=, (const GameVar &), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opMulAssign(const GameVar&)", focMETHODPR( GameVar, operator*=, (const GameVar &), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opDivAssign(const GameVar&)", focMETHODPR( GameVar, operator/=, (const GameVar &), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAssign(const GameVar&)", focMETHODPR( GameVar, operator=, (const GameVar &), GameVar& ), asCALL_THISCALL ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opAdd(const int)", focFUNCTION( GameVarAddInt ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opSub(const int)", focFUNCTION( GameVarSubInt ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opMul(const int)", focFUNCTION( GameVarMulInt ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opDiv(const int)", focFUNCTION( GameVarDivInt ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opAdd(const GameVar&)", focFUNCTION( GameVarAddGameVar ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opSub(const GameVar&)", focFUNCTION( GameVarSubGameVar ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opMul(const GameVar&)", focFUNCTION( GameVarMulGameVar ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opDiv(const GameVar&)", focFUNCTION( GameVarDivGameVar ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "bool opEquals(const int)", focFUNCTION( GameVarEqualInt ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opCmp(const int)", focFUNCTION( GameVarCmpInt ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "bool opEquals(const GameVar&)", focFUNCTION( GameVarEqualGameVar ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opCmp(const GameVar&)", focFUNCTION( GameVarCmpGameVar ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // Item
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllItems(uint16 pid, Item@[]@+ items)", focFUNCTION( BIND_CLASS Global_GetAllItems ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "Item@+ GetItem(uint itemId)", focFUNCTION( BIND_CLASS Global_GetItem ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItem(Item& item, uint count, Critter& toCr)", focFUNCTION( BIND_CLASS Global_MoveItemCr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItem(Item& item, uint count, Item& toCont, uint stackId)", focFUNCTION( BIND_CLASS Global_MoveItemCont ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItem(Item& item, uint count, Map& toMap, uint16 toHx, uint16 toHy)", focFUNCTION( BIND_CLASS Global_MoveItemMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItems(Item@[]& items, Critter& toCr)", focFUNCTION( BIND_CLASS Global_MoveItemsCr ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItems(Item@[]& items, Item& toCont, uint stackId)", focFUNCTION( BIND_CLASS Global_MoveItemsCont ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItems(Item@[]& items, Map& toMap, uint16 toHx, uint16 toHy)", focFUNCTION( BIND_CLASS Global_MoveItemsMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteItem(Item& item)", focFUNCTION( BIND_CLASS Global_DeleteItem ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteItems(Item@[]& items)", focFUNCTION( BIND_CLASS Global_DeleteItems ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint Id", focOFFSET( Item, Id ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const ProtoItem@ Proto", focOFFSET( Item, Proto ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint8 Accessory", focOFFSET( Item, Accessory ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint MapId", focOFFSET( Item, AccHex.MapId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint16 HexX", focOFFSET( Item, AccHex.HexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint16 HexY", focOFFSET( Item, AccHex.HexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint CritId", focOFFSET( Item, AccCritter.Id ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint8 CritSlot", focOFFSET( Item, AccCritter.Slot ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint ContainerId", focOFFSET( Item, AccContainer.ContainerId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint StackId", focOFFSET( Item, AccContainer.StackId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const bool IsNotValid", focOFFSET( Item, IsNotValid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "const uint8 Mode", focOFFSET( Item, Data.Mode ) ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 SortValue", focOFFSET( Item, Data.SortValue ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 Info", focOFFSET( Item, Data.Info ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint PicMap", focOFFSET( Item, Data.PicMapHash ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint PicInv", focOFFSET( Item, Data.PicInvHash ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 AnimWaitBase", focOFFSET( Item, Data.AnimWaitBase ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 AnimStayBegin", focOFFSET( Item, Data.AnimStay[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 AnimStayEnd", focOFFSET( Item, Data.AnimStay[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 AnimShowBegin", focOFFSET( Item, Data.AnimShow[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 AnimShowEnd", focOFFSET( Item, Data.AnimShow[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 AnimHideBegin", focOFFSET( Item, Data.AnimHide[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 AnimHideEnd", focOFFSET( Item, Data.AnimHide[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint Cost", focOFFSET( Item, Data.Cost ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val0", focOFFSET( Item, Data.ScriptValues[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val1", focOFFSET( Item, Data.ScriptValues[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val2", focOFFSET( Item, Data.ScriptValues[2] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val3", focOFFSET( Item, Data.ScriptValues[3] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val4", focOFFSET( Item, Data.ScriptValues[4] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val5", focOFFSET( Item, Data.ScriptValues[5] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val6", focOFFSET( Item, Data.ScriptValues[6] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val7", focOFFSET( Item, Data.ScriptValues[7] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val8", focOFFSET( Item, Data.ScriptValues[8] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int Val9", focOFFSET( Item, Data.ScriptValues[9] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int8 LightIntensity", focOFFSET( Item, Data.LightIntensity ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 LightDistance", focOFFSET( Item, Data.LightDistance ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 LightFlags", focOFFSET( Item, Data.LightFlags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint LightColor", focOFFSET( Item, Data.LightColor ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 Indicator", focOFFSET( Item, Data.Indicator ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 BrokenFlags", focOFFSET( Item, Data.BrokenFlags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 BrokenCount", focOFFSET( Item, Data.BrokenCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 Deterioration", focOFFSET( Item, Data.Deterioration ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 AmmoPid", focOFFSET( Item, Data.AmmoPid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 AmmoCount", focOFFSET( Item, Data.AmmoCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint LockerId", focOFFSET( Item, Data.LockerId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 LockerCondition", focOFFSET( Item, Data.LockerCondition ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 LockerComplexity", focOFFSET( Item, Data.LockerComplexity ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 Charge", focOFFSET( Item, Data.Charge ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 RadioChannel", focOFFSET( Item, Data.RadioChannel ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint16 RadioFlags", focOFFSET( Item, Data.RadioFlags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 RadioBroadcastSend", focOFFSET( Item, Data.RadioBroadcastSend ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint8 RadioBroadcastRecv", focOFFSET( Item, Data.RadioBroadcastRecv ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "uint HolodiskNumber", focOFFSET( Item, Data.HolodiskNumber ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int16 OffsetX", focOFFSET( Item, Data.OffsetX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int16 OffsetY", focOFFSET( Item, Data.OffsetY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Item", "int16 Dir", focOFFSET( Item, Data.Dir ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void set_Flags(uint value)", focFUNCTION( BIND_CLASS Item_set_Flags ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint get_Flags() const", focFUNCTION( BIND_CLASS Item_get_Flags ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void set_TrapValue(int16 val)", focFUNCTION( BIND_CLASS Item_set_TrapValue ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "int16 get_TrapValue() const", focFUNCTION( BIND_CLASS Item_get_TrapValue ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool LockerOpen()", focFUNCTION( BIND_CLASS Item_LockerOpen ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool LockerClose()", focFUNCTION( BIND_CLASS Item_LockerClose ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool IsStackable() const", focFUNCTION( BIND_CLASS Item_IsStackable ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool IsDeteriorable() const", focFUNCTION( BIND_CLASS Item_IsDeteriorable ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Item_SetScript ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Item_GetScriptId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Item_SetEvent ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint8 GetType() const", focFUNCTION( BIND_CLASS Item_GetType ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Item_GetProtoId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetCount() const", focFUNCTION( BIND_CLASS Item_GetCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void SetCount(uint count)", focFUNCTION( BIND_CLASS Item_SetCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetCost() const", focFUNCTION( BIND_CLASS Item_GetCost ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Item@+ AddItem(uint16 protoId, uint count, uint specialId)", focFUNCTION( BIND_CLASS Container_AddItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Item@+ GetItem(uint16 protoId, uint specialId) const", focFUNCTION( BIND_CLASS Container_GetItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetItems(uint specialId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Container_GetItems ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Map@+ GetMapPosition(uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Item_GetMapPosition ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool ChangeProto(uint16 protoId) const", focFUNCTION( BIND_CLASS Item_ChangeProto ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void Update() const", focFUNCTION( BIND_CLASS Item_Update ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void Animate(uint8 fromFrame, uint8 toFrame)", focFUNCTION( BIND_CLASS Item_Animate ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void SetLexems(string@+ lexems)", focFUNCTION( BIND_CLASS Item_SetLexems ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Item@+ GetChild(uint childIndex) const", focFUNCTION( BIND_CLASS Item_GetChild ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Item_EventFinish ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventAttack(Critter& attacker, Critter& target)", focFUNCTION( BIND_CLASS Item_EventAttack ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventUse(Critter& cr, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Item_EventUse ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventUseOnMe(Critter& cr, Item@+ usedItem)", focFUNCTION( BIND_CLASS Item_EventUseOnMe ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventSkill(Critter& cr, int skill)", focFUNCTION( BIND_CLASS Item_EventSkill ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventDrop(Critter& cr)", focFUNCTION( BIND_CLASS Item_EventDrop ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventMove(Critter& cr, uint8 fromSlot)", focFUNCTION( BIND_CLASS Item_EventMove ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventWalk(Critter& cr, bool entered, uint8 dir)", focFUNCTION( BIND_CLASS Item_EventWalk ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // ItemCl
    //

    #if defined (BIND_DUMMY) || defined (BIND_CLIENT)
    if( bind == SCRIPT_BIND_CLIENT )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "ItemCl@+ GetItem(uint itemId)", focFUNCTION( BIND_CLASS Global_GetItem ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "ItemCl@+ GetMonitorItem(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorItem ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint Id", focOFFSET( Item, Id ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const ProtoItem@ Proto", focOFFSET( Item, Proto ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const string Lexems", focOFFSET( Item, Lexems ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 Accessory", focOFFSET( Item, Accessory ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint MapId", focOFFSET( Item, AccHex.MapId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 HexX", focOFFSET( Item, AccHex.HexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 HexY", focOFFSET( Item, AccHex.HexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint CritId", focOFFSET( Item, AccCritter.Id ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 CritSlot", focOFFSET( Item, AccCritter.Slot ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint ContainerId", focOFFSET( Item, AccContainer.ContainerId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint StackId", focOFFSET( Item, AccContainer.StackId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 SortValue", focOFFSET( Item, Data.SortValue ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 Info", focOFFSET( Item, Data.Info ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint PicMap", focOFFSET( Item, Data.PicMapHash ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint PicInv", focOFFSET( Item, Data.PicInvHash ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 AnimWaitBase", focOFFSET( Item, Data.AnimWaitBase ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 AnimStayBegin", focOFFSET( Item, Data.AnimStay[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 AnimStayEnd", focOFFSET( Item, Data.AnimStay[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 AnimShowBegin", focOFFSET( Item, Data.AnimShow[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 AnimShowEnd", focOFFSET( Item, Data.AnimShow[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 AnimHideBegin", focOFFSET( Item, Data.AnimHide[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 AnimHideEnd", focOFFSET( Item, Data.AnimHide[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint Flags", focOFFSET( Item, Data.Flags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 Mode", focOFFSET( Item, Data.Mode ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val0", focOFFSET( Item, Data.ScriptValues[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val1", focOFFSET( Item, Data.ScriptValues[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val2", focOFFSET( Item, Data.ScriptValues[2] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val3", focOFFSET( Item, Data.ScriptValues[3] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val4", focOFFSET( Item, Data.ScriptValues[4] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val5", focOFFSET( Item, Data.ScriptValues[5] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val6", focOFFSET( Item, Data.ScriptValues[6] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val7", focOFFSET( Item, Data.ScriptValues[7] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val8", focOFFSET( Item, Data.ScriptValues[8] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int Val9", focOFFSET( Item, Data.ScriptValues[9] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int8 LightIntensity", focOFFSET( Item, Data.LightIntensity ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 LightDistance", focOFFSET( Item, Data.LightDistance ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 LightFlags", focOFFSET( Item, Data.LightFlags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint LightColor", focOFFSET( Item, Data.LightColor ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int16 TrapValue", focOFFSET( Item, Data.TrapValue ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 Indicator", focOFFSET( Item, Data.Indicator ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 BrokenFlags", focOFFSET( Item, Data.BrokenFlags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 BrokenCount", focOFFSET( Item, Data.BrokenCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 Deterioration", focOFFSET( Item, Data.Deterioration ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 AmmoPid", focOFFSET( Item, Data.AmmoPid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 AmmoCount", focOFFSET( Item, Data.AmmoCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint LockerId", focOFFSET( Item, Data.LockerId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 LockerCondition", focOFFSET( Item, Data.LockerCondition ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 LockerComplexity", focOFFSET( Item, Data.LockerComplexity ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 Charge", focOFFSET( Item, Data.Charge ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 RadioChannel", focOFFSET( Item, Data.RadioChannel ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint16 RadioFlags", focOFFSET( Item, Data.RadioFlags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 RadioBroadcastSend", focOFFSET( Item, Data.RadioBroadcastSend ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint8 RadioBroadcastRecv", focOFFSET( Item, Data.RadioBroadcastRecv ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const uint HolodiskNumber", focOFFSET( Item, Data.HolodiskNumber ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int16 OffsetX", focOFFSET( Item, Data.OffsetX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int16 OffsetY", focOFFSET( Item, Data.OffsetY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "ItemCl", "const int16 Dir", focOFFSET( Item, Data.Dir ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "bool IsStackable() const", focFUNCTION( BIND_CLASS Item_IsStackable ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "bool IsDeteriorable() const", focFUNCTION( BIND_CLASS Item_IsDeteriorable ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Item_GetScriptId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint8 GetType() const", focFUNCTION( BIND_CLASS Item_GetType ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Item_GetProtoId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint GetCount() const", focFUNCTION( BIND_CLASS Item_GetCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "bool GetMapPosition(uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Item_GetMapPosition ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "void Animate(uint8 fromFrame, uint8 toFrame)", focFUNCTION( BIND_CLASS Item_Animate ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // Location
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "Location@+ GetLocation(uint locId)", focFUNCTION( BIND_CLASS Global_GetLocation ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "Location@+ GetLocationByPid(uint16 locPid, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetLocationByPid ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetLocations(uint16 worldX, uint16 worldY, uint radius, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetLocations ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetVisibleLocations(uint16 worldX, uint16 worldY, uint radius, Critter@+ visibleBy, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetVisibleLocations ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetZoneLocationIds(uint16 zoneX, uint16 zoneY, uint zoneRadius, uint[]@+ locationIds)", focFUNCTION( BIND_CLASS Global_GetZoneLocationIds ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "const uint Id", focOFFSET( Location, Data.LocId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "uint16 WorldX", focOFFSET( Location, Data.WX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "uint16 WorldY", focOFFSET( Location, Data.WY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "bool Visible", focOFFSET( Location, Data.Visible ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "bool GeckVisible", focOFFSET( Location, Data.GeckVisible ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "bool AutoGarbage", focOFFSET( Location, Data.AutoGarbage ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "int GeckCount", focOFFSET( Location, GeckCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "uint16 Radius", focOFFSET( Location, Data.Radius ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "uint Color", focOFFSET( Location, Data.Color ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Location", "const bool IsNotValid", focOFFSET( Location, IsNotValid ) ) );

        BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateLocation(uint16 locPid, uint16 worldX, uint16 worldY, Critter@[]@+ critters)", focFUNCTION( BIND_CLASS Global_CreateLocation ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteLocation(uint locId)", focFUNCTION( BIND_CLASS Global_DeleteLocation ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Location_GetProtoId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Location_SetEvent ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "uint GetMapCount() const", focFUNCTION( BIND_CLASS Location_GetMapCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "Map@+ GetMap(uint16 mapPid) const", focFUNCTION( BIND_CLASS Location_GetMap ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "Map@+ GetMapByIndex(uint index) const", focFUNCTION( BIND_CLASS Location_GetMapByIndex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "uint GetMaps(Map@[]@+ maps) const", focFUNCTION( BIND_CLASS Location_GetMaps ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "bool Reload()", focFUNCTION( BIND_CLASS Location_Reload ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "void Update()", focFUNCTION( BIND_CLASS Location_Update ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Location_EventFinish ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Location", "bool EventEnter(Critter@[]& group, uint8 entrance)", focFUNCTION( BIND_CLASS Location_EventEnter ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    ///
    // Map
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "Map@+ GetMap(uint mapId)", focFUNCTION( BIND_CLASS Global_GetMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "Map@+ GetMapByPid(uint16 mapPid, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMapByPid ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const bool IsNotValid", focOFFSET( Map, IsNotValid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint Id", focOFFSET( Map, Data.MapId ) ) );
        // BIND_ASSERT( engine->RegisterObjectProperty( "Map", "bool CombatRunning", focOFFSET( Map, MapId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint TurnBasedRound", focOFFSET( Map, TurnBasedRound ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint TurnBasedTurn", focOFFSET( Map, TurnBasedTurn ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint TurnBasedWholeTurn", focOFFSET( Map, TurnBasedWholeTurn ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Map_GetProtoId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Location@+ GetLocation() const", focFUNCTION( BIND_CLASS Map_GetLocation ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Map_SetScript ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Map_GetScriptId ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Map_SetEvent ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetLoopTime(uint numLoop, uint ms)", focFUNCTION( BIND_CLASS Map_SetLoopTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint8 GetRain() const", focFUNCTION( BIND_CLASS Map_GetRain ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetRain(uint8 capacity)", focFUNCTION( BIND_CLASS Map_SetRain ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "int GetTime() const", focFUNCTION( BIND_CLASS Map_GetTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTime(int time)", focFUNCTION( BIND_CLASS Map_SetTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetDayTime(uint dayPart) const", focFUNCTION( BIND_CLASS Map_GetDayTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetDayTime(uint dayPart, uint time)", focFUNCTION( BIND_CLASS Map_SetDayTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b) const", focFUNCTION( BIND_CLASS Map_GetDayColor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetDayColor(uint dayPart, uint8 r, uint8 g, uint8 b)", focFUNCTION( BIND_CLASS Map_SetDayColor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTurnBasedAvailability(bool value)", focFUNCTION( BIND_CLASS Map_SetTurnBasedAvailability ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsTurnBasedAvailability() const", focFUNCTION( BIND_CLASS Map_IsTurnBasedAvailability ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void BeginTurnBased(Critter@+ firstTurnCrit)", focFUNCTION( BIND_CLASS Map_BeginTurnBased ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsTurnBased() const", focFUNCTION( BIND_CLASS Map_IsTurnBased ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EndTurnBased()", focFUNCTION( BIND_CLASS Map_EndTurnBased ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "int GetTurnBasedSequence(uint[]& crittersIds) const", focFUNCTION( BIND_CLASS Map_GetTurnBasedSequence ), asCALL_CDECL_OBJFIRST ) );
        // BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTurnBasedSequence(uint[]& crittersIds)", focFUNCTION( BIND_CLASS Map_SetTurnBasedSequence ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetData(uint index, int value)", focFUNCTION( BIND_CLASS Map_SetData ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "int GetData(uint index) const", focFUNCTION( BIND_CLASS Map_GetData ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ AddItem(uint16 hexX, uint16 hexY, uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Map_AddItem ), asCALL_CDECL_OBJFIRST ) );
        // BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ AddItemFromScenery(uint16 hexX, uint16 hexY, uint16 protoId)", focFUNCTION( BIND_CLASS Map_AddItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetItem(uint itemId) const", focFUNCTION( BIND_CLASS Map_GetItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetItem(uint16 hexX, uint16 hexY, uint16 protoId) const", focFUNCTION( BIND_CLASS Map_GetItemHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItems(uint16 hexX, uint16 hexY, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItems(uint16 hexX, uint16 hexY, uint radius, uint16 protoId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsHexEx ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItems(uint16 protoId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsByPid ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItemsByType(int type, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsByType ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetDoor(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetDoor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetCar(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetCar ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Scenery@+ GetScenery(uint16 hexX, uint16 hexY, uint16 protoId) const", focFUNCTION( BIND_CLASS Map_GetSceneryHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetSceneries(uint16 hexX, uint16 hexY, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetSceneries(uint16 hexX, uint16 hexY, uint radius, uint16 protoId, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesHexEx ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetSceneries(uint16 protoId, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesByPid ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ GetCritter(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetCritterHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ GetCritter(uint critterId) const", focFUNCTION( BIND_CLASS Map_GetCritterById ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersHex(uint16 hexX, uint16 hexY, uint radius, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCritters ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCritters(uint16 pid, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersByPids ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersInPath ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, Critter@[]@+ critters, uint16& preBlockHx, uint16& preBlockHy, uint16& blockHx, uint16& blockHy) const", focFUNCTION( BIND_CLASS Map_GetCrittersInPathBlock ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersWhoViewPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersWhoViewPath ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersSeeing(Critter@[]& critters, bool lookOnThem, int find_type, Critter@[]@+ crittersResult) const", focFUNCTION( BIND_CLASS Map_GetCrittersSeeing ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist) const", focFUNCTION( BIND_CLASS Map_GetHexInPath ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void GetHexCoordWall(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist) const", focFUNCTION( BIND_CLASS Map_GetHexInPathWall ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut) const", focFUNCTION( BIND_CLASS Map_GetPathLengthHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetPathLength(Critter& cr, uint16 toHx, uint16 toHy, uint cut) const", focFUNCTION( BIND_CLASS Map_GetPathLengthCr ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool VerifyTrigger(Critter& cr, uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Map_VerifyTrigger ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ AddNpc(uint16 protoId, uint16 hexX, uint16 hexY, uint8 dir, int[]@+ params, int[]@+ items, string@+ script)", focFUNCTION( BIND_CLASS Map_AddNpc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetNpcCount(int npcRole, int findType) const", focFUNCTION( BIND_CLASS Map_GetNpcCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ GetNpc(int npcRole, int findType, uint skipCount) const", focFUNCTION( BIND_CLASS Map_GetNpc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint CountEntire(int entire) const", focFUNCTION( BIND_CLASS Map_CountEntire ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetEntires(int entire, uint[]@+ entires, uint16[]@+ hexX, uint16[]@+ hexY) const", focFUNCTION( BIND_CLASS Map_GetEntires ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetEntireCoords(int entire, uint skip, uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Map_GetEntireCoords ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetEntireCoords(int entire, uint skip, uint16& hexX, uint16& hexY, uint8& dir) const", focFUNCTION( BIND_CLASS Map_GetEntireCoordsDir ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetNearEntireCoords(int& entire, uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Map_GetNearEntireCoords ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetNearEntireCoords(int& entire, uint16& hexX, uint16& hexY, uint8& dir) const", focFUNCTION( BIND_CLASS Map_GetNearEntireCoordsDir ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsHexPassed(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_IsHexPassed ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsHexRaked(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_IsHexRaked ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetText(uint16 hexX, uint16 hexY, uint color, string& text) const", focFUNCTION( BIND_CLASS Map_SetText ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTextMsg(uint16 hexX, uint16 hexY, uint color, uint16 textMsg, uint strNum) const", focFUNCTION( BIND_CLASS Map_SetTextMsg ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTextMsg(uint16 hexX, uint16 hexY, uint color, uint16 textMsg, uint strNum, string& lexems) const", focFUNCTION( BIND_CLASS Map_SetTextMsgLex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void RunEffect(uint16 effectPid, uint16 hexX, uint16 hexY, uint16 radius) const", focFUNCTION( BIND_CLASS Map_RunEffect ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void RunFlyEffect(uint16 effectPid, Critter@+ fromCr, Critter@+ toCr, uint16 fromX, uint16 fromY, uint16 toX, uint16 toY) const", focFUNCTION( BIND_CLASS Map_RunFlyEffect ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool CheckPlaceForItem(uint16 hexX, uint16 hexY, uint16 pid) const", focFUNCTION( BIND_CLASS Map_CheckPlaceForItem ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void BlockHex(uint16 hexX, uint16 hexY, bool full)", focFUNCTION( BIND_CLASS Map_BlockHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void UnblockHex(uint16 hexX, uint16 hexY)", focFUNCTION( BIND_CLASS Map_UnblockHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void PlaySound(string& soundName) const", focFUNCTION( BIND_CLASS Map_PlaySound ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void PlaySound(string& soundName, uint16 hexX, uint16 hexY, uint radius) const", focFUNCTION( BIND_CLASS Map_PlaySoundRadius ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool Reload()", focFUNCTION( BIND_CLASS Map_Reload ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint16 GetWidth() const", focFUNCTION( BIND_CLASS Map_GetWidth ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint16 GetHeight() const", focFUNCTION( BIND_CLASS Map_GetHeight ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps) const", focFUNCTION( BIND_CLASS Map_MoveHexByDir ), asCALL_CDECL_OBJFIRST ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Map_EventFinish ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop0()", focFUNCTION( BIND_CLASS Map_EventLoop0 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop1()", focFUNCTION( BIND_CLASS Map_EventLoop1 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop2()", focFUNCTION( BIND_CLASS Map_EventLoop2 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop3()", focFUNCTION( BIND_CLASS Map_EventLoop3 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop4()", focFUNCTION( BIND_CLASS Map_EventLoop4 ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventInCritter(Critter& cr)", focFUNCTION( BIND_CLASS Map_EventInCritter ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventOutCritter(Critter& cr)", focFUNCTION( BIND_CLASS Map_EventOutCritter ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventCritterDead(Critter& cr, Critter@+ killer)", focFUNCTION( BIND_CLASS Map_EventCritterDead ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventTurnBasedBegin()", focFUNCTION( BIND_CLASS Map_EventTurnBasedBegin ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventTurnBasedEnd()", focFUNCTION( BIND_CLASS Map_EventTurnBasedEnd ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventTurnBasedProcess(Critter& cr, bool beginTurn)", focFUNCTION( BIND_CLASS Map_EventTurnBasedProcess ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // MapperMap
    //

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( bind == SCRIPT_BIND_MAPPER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "MapperMap@+ LoadMap(string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_LoadMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void UnloadMap(MapperMap@+ map)", focFUNCTION( BIND_CLASS Global_UnloadMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool SaveMap(MapperMap@+ map, string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_SaveMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "bool ShowMap(MapperMap@+ map)", focFUNCTION( BIND_CLASS Global_ShowMap ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "int GetLoadedMaps(MapperMap@[]@+ maps)", focFUNCTION( BIND_CLASS Global_GetLoadedMaps ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const uint16 Width", focOFFSET( ProtoMap, Header.MaxHexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const uint16 Height", focOFFSET( ProtoMap, Header.MaxHexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const int WorkHexX", focOFFSET( ProtoMap, Header.WorkHexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const int WorkHexY", focOFFSET( ProtoMap, Header.WorkHexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "int Time", focOFFSET( ProtoMap, Header.Time ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "bool NoLogOut", focOFFSET( ProtoMap, Header.NoLogOut ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "MapperObject@+ AddObject(uint16 hexX, uint16 hexY, int mapObjType, uint16 pid)", focFUNCTION( BIND_CLASS MapperMap_AddObject ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "MapperObject@+ GetObject(uint16 hexX, uint16 hexY, int mapObjType, uint16 pid, uint skip) const", focFUNCTION( BIND_CLASS MapperMap_GetObject ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetObjects(uint16 hexX, uint16 hexY, uint radius, int mapObjType, uint16 pid, MapperObject@[]@+ objects) const", focFUNCTION( BIND_CLASS MapperMap_GetObjects ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void UpdateObjects() const", focFUNCTION( BIND_CLASS MapperMap_UpdateObjects ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void Resize(uint16 width, uint16 height)", focFUNCTION( BIND_CLASS MapperMap_Resize ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetTilesCount(uint16 hexX, uint16 hexY, bool roof) const", focFUNCTION( BIND_CLASS MapperMap_GetTilesCount ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void DeleteTile(uint16 hexX, uint16 hexY, bool roof, uint index)", focFUNCTION( BIND_CLASS MapperMap_DeleteTile ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetTile(uint16 hexX, uint16 hexY, bool roof, uint index) const", focFUNCTION( BIND_CLASS MapperMap_GetTileHash ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void AddTile(uint16 hexX, uint16 hexY, int offsX, int offsY, int layer, bool roof, uint picHash)", focFUNCTION( BIND_CLASS MapperMap_AddTileHash ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "string@ GetTileName(uint16 hexX, uint16 hexY, bool roof, uint index) const", focFUNCTION( BIND_CLASS MapperMap_GetTileName ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void AddTileName(uint16 hexX, uint16 hexY, int offsX, int offsY, int layer, bool roof, string@+ picName)", focFUNCTION( BIND_CLASS MapperMap_AddTileName ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetDayTime(uint dayPart) const", focFUNCTION( BIND_CLASS MapperMap_GetDayTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void SetDayTime(uint dayPart, uint time)", focFUNCTION( BIND_CLASS MapperMap_SetDayTime ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b) const", focFUNCTION( BIND_CLASS MapperMap_GetDayColor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void SetDayColor(uint dayPart, uint8 r, uint8 g, uint8 b)", focFUNCTION( BIND_CLASS MapperMap_SetDayColor ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "string@ get_ScriptModule() const", focFUNCTION( BIND_CLASS MapperMap_get_ScriptModule ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void set_ScriptModule(const string& name)", focFUNCTION( BIND_CLASS MapperMap_set_ScriptModule ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "string@ get_ScriptFunc() const", focFUNCTION( BIND_CLASS MapperMap_get_ScriptFunc ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void set_ScriptFunc(const string& name)", focFUNCTION( BIND_CLASS MapperMap_set_ScriptFunc ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // MapperObject
    //

    #if defined (BIND_DUMMY) || defined (BIND_MAPPER)
    if( bind == SCRIPT_BIND_MAPPER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteObject(MapperObject@+ obj)", focFUNCTION( BIND_CLASS Global_DeleteObject ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteObjects(MapperObject@[]& objects)", focFUNCTION( BIND_CLASS Global_DeleteObjects ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SelectObject(MapperObject@+ obj, bool set)", focFUNCTION( BIND_CLASS Global_SelectObject ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void SelectObjects(MapperObject@[]& objects, bool set)", focFUNCTION( BIND_CLASS Global_SelectObjects ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "MapperObject@+ GetSelectedObject()", focFUNCTION( BIND_CLASS Global_GetSelectedObject ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetSelectedObjects(MapperObject@[]@+ objects)", focFUNCTION( BIND_CLASS Global_GetSelectedObjects ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "MapperObject@+ GetMonitorObject(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorObject ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void Update() const", focFUNCTION( BIND_CLASS MapperObject_Update ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "MapperObject@+ AddChild(uint16 pid)", focFUNCTION( BIND_CLASS MapperObject_AddChild ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "uint GetChilds(MapperObject@[]@+ objects) const", focFUNCTION( BIND_CLASS MapperObject_GetChilds ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_ScriptName() const", focFUNCTION( BIND_CLASS MapperObject_get_ScriptName ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_ScriptName(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_ScriptName ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_FuncName() const", focFUNCTION( BIND_CLASS MapperObject_get_FuncName ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_FuncName(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_FuncName ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "uint8 get_Critter_Cond() const", focFUNCTION( BIND_CLASS MapperObject_get_Critter_Cond ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_Critter_Cond(uint8 value)", focFUNCTION( BIND_CLASS MapperObject_set_Critter_Cond ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void MoveToHex(uint16 hexX, uint16 hexY)", focFUNCTION( BIND_CLASS MapperObject_MoveToHex ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void MoveToHexOffset(int x, int y)", focFUNCTION( BIND_CLASS MapperObject_MoveToHexOffset ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void MoveToDir(uint8 dir)", focFUNCTION( BIND_CLASS MapperObject_MoveToDir ), asCALL_CDECL_OBJFIRST ) );

        // Generic
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint8 MapObjType", focOFFSET( MapObject, MapObjType ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint16 ProtoId", focOFFSET( MapObject, ProtoId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint16 MapX", focOFFSET( MapObject, MapX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint16 MapY", focOFFSET( MapObject, MapY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Dir", focOFFSET( MapObject, Dir ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint UID", focOFFSET( MapObject, UID ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint ContainerUID", focOFFSET( MapObject, ContainerUID ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint ParentUID", focOFFSET( MapObject, ParentUID ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "const uint ParentChildIndex", focOFFSET( MapObject, ParentChildIndex ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint LightColor", focOFFSET( MapObject, LightColor ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 LightDay", focOFFSET( MapObject, LightDay ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 LightDirOff", focOFFSET( MapObject, LightDirOff ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 LightDistance", focOFFSET( MapObject, LightDistance ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int8 LightIntensity", focOFFSET( MapObject, LightIntensity ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData0", focOFFSET( MapObject, UserData[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData1", focOFFSET( MapObject, UserData[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData2", focOFFSET( MapObject, UserData[2] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData3", focOFFSET( MapObject, UserData[3] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData4", focOFFSET( MapObject, UserData[4] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData5", focOFFSET( MapObject, UserData[5] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData6", focOFFSET( MapObject, UserData[6] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData7", focOFFSET( MapObject, UserData[7] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData8", focOFFSET( MapObject, UserData[8] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int UserData9", focOFFSET( MapObject, UserData[9] ) ) );

        // Critter
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Critter_Anim1", focOFFSET( MapObject, MCritter.Anim1 ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Critter_Anim2", focOFFSET( MapObject, MCritter.Anim2 ) ) );
        for( int i = 0; i < 40 /*MAPOBJ_CRITTER_PARAMS*/; i++ )
        {
            char str[256];
            sprintf( str, "int16 Critter_ParamIndex%d", i );
            BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", str, focOFFSET( MapObject, MCritter.ParamIndex[0] ) + sizeof(short) * i ) );

            sprintf( str, "int Critter_ParamValue%d", i );
            BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", str, focOFFSET( MapObject, MCritter.ParamValue[0] ) + sizeof(int) * i ) );

        }

        // Item/Critter shared parameters
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int16 OffsetX", focOFFSET( MapObject, MItem.OffsetX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int16 OffsetY", focOFFSET( MapObject, MItem.OffsetY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 AnimStayBegin", focOFFSET( MapObject, MItem.AnimStayBegin ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 AnimStayEnd", focOFFSET( MapObject, MItem.AnimStayEnd ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint16 AnimWait", focOFFSET( MapObject, MItem.AnimWait ) ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_PicMap() const", focFUNCTION( BIND_CLASS MapperObject_get_PicMap ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_PicMap(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_PicMap ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_PicInv() const", focFUNCTION( BIND_CLASS MapperObject_get_PicInv ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_PicInv(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_PicInv ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 InfoOffset", focOFFSET( MapObject, MItem.InfoOffset ) ) );

        // Item
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint Item_Count", focOFFSET( MapObject, MItem.Count ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Item_BrokenFlags", focOFFSET( MapObject, MItem.BrokenFlags ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Item_BrokenCount", focOFFSET( MapObject, MItem.BrokenCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint16 Item_Deterioration", focOFFSET( MapObject, MItem.Deterioration ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Item_ItemSlot", focOFFSET( MapObject, MItem.ItemSlot ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint16 Item_AmmoPid", focOFFSET( MapObject, MItem.AmmoPid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint Item_AmmoCount", focOFFSET( MapObject, MItem.AmmoCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint Item_LockerDoorId", focOFFSET( MapObject, MItem.LockerDoorId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint16 Item_LockerCondition", focOFFSET( MapObject, MItem.LockerCondition ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint16 Item_LockerComplexity", focOFFSET( MapObject, MItem.LockerComplexity ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int16 Item_TrapValue", focOFFSET( MapObject, MItem.TrapValue ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val0", focOFFSET( MapObject, MItem.Val[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val1", focOFFSET( MapObject, MItem.Val[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val2", focOFFSET( MapObject, MItem.Val[2] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val3", focOFFSET( MapObject, MItem.Val[3] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val4", focOFFSET( MapObject, MItem.Val[4] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val5", focOFFSET( MapObject, MItem.Val[5] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val6", focOFFSET( MapObject, MItem.Val[6] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val7", focOFFSET( MapObject, MItem.Val[7] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val8", focOFFSET( MapObject, MItem.Val[8] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Item_Val9", focOFFSET( MapObject, MItem.Val[9] ) ) );

        // Scenery
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "bool Scenery_CanUse", focOFFSET( MapObject, MScenery.CanUse ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "bool Scenery_CanTalk", focOFFSET( MapObject, MScenery.CanTalk ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint Scenery_TriggerNum", focOFFSET( MapObject, MScenery.TriggerNum ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Scenery_ParamsCount", focOFFSET( MapObject, MScenery.ParamsCount ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Scenery_Param0", focOFFSET( MapObject, MScenery.Param[0] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Scenery_Param1", focOFFSET( MapObject, MScenery.Param[1] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Scenery_Param2", focOFFSET( MapObject, MScenery.Param[2] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Scenery_Param3", focOFFSET( MapObject, MScenery.Param[3] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int Scenery_Param4", focOFFSET( MapObject, MScenery.Param[4] ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint16 Scenery_ToMapPid", focOFFSET( MapObject, MScenery.ToMapPid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint Scenery_ToEntire", focOFFSET( MapObject, MScenery.ToEntire ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Scenery_ToDir", focOFFSET( MapObject, MScenery.ToDir ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 Scenery_SpriteCut", focOFFSET( MapObject, MScenery.SpriteCut ) ) );
    }
    #endif

    //
    // NpcPlane
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "NpcPlane@ CreatePlane()", focFUNCTION( BIND_CLASS Global_CreatePlane ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "int Type", focOFFSET( AIDataPlane, Type ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint Priority", focOFFSET( AIDataPlane, Priority ) ) );
        // BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "NpcPlane@ Child", focOFFSET( AIDataPlane, Type ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "int Identifier", focOFFSET( AIDataPlane, Identifier ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint IdentifierExt", focOFFSET( AIDataPlane, IdentifierExt ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "bool Run", focOFFSET( AIDataPlane, Pick.IsRun ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint Misc_WaitSecond", focOFFSET( AIDataPlane, Misc.WaitSecond ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "int Misc_ScriptId", focOFFSET( AIDataPlane, Misc.ScriptBindId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint Attack_TargId", focOFFSET( AIDataPlane, Attack.TargId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "int Attack_MinHp", focOFFSET( AIDataPlane, Attack.MinHp ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "bool Attack_IsGag", focOFFSET( AIDataPlane, Attack.IsGag ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Attack_GagHexX", focOFFSET( AIDataPlane, Attack.GagHexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Attack_GagHexY", focOFFSET( AIDataPlane, Attack.GagHexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Attack_LastHexX", focOFFSET( AIDataPlane, Attack.LastHexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Attack_LastHexY", focOFFSET( AIDataPlane, Attack.LastHexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Walk_HexX", focOFFSET( AIDataPlane, Walk.HexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Walk_HexY", focOFFSET( AIDataPlane, Walk.HexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint8 Walk_Dir", focOFFSET( AIDataPlane, Walk.Dir ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint Walk_Cut", focOFFSET( AIDataPlane, Walk.Cut ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Pick_HexX", focOFFSET( AIDataPlane, Pick.HexX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Pick_HexY", focOFFSET( AIDataPlane, Pick.HexY ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint16 Pick_Pid", focOFFSET( AIDataPlane, Pick.Pid ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "uint Pick_UseItemId", focOFFSET( AIDataPlane, Pick.UseItemId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "NpcPlane", "bool Pick_ToOpen", focOFFSET( AIDataPlane, Pick.ToOpen ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "NpcPlane@ GetCopy() const", focFUNCTION( BIND_CLASS NpcPlane_GetCopy ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "NpcPlane@+ SetChild(NpcPlane& child)", focFUNCTION( BIND_CLASS NpcPlane_SetChild ), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "NpcPlane@+ GetChild(uint index) const", focFUNCTION( BIND_CLASS NpcPlane_GetChild ), asCALL_CDECL_OBJFIRST ) );

        // BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "uint GetIndex() const", focFUNCTION(BIND_CLASS NpcPlane_GetIndex), asCALL_CDECL_OBJFIRST ) );
        BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "bool Misc_SetScript(string& funcName)", focFUNCTION( BIND_CLASS NpcPlane_Misc_SetScript ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // ProtoItem
    //

    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 ProtoId", focOFFSET( ProtoItem, ProtoId ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Type", focOFFSET( ProtoItem, Type ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint PicMap", focOFFSET( ProtoItem, PicMap ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint PicInv", focOFFSET( ProtoItem, PicInv ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Flags", focOFFSET( ProtoItem, Flags ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Stackable", focOFFSET( ProtoItem, Stackable ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Deteriorable", focOFFSET( ProtoItem, Deteriorable ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool GroundLevel", focOFFSET( ProtoItem, GroundLevel ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Corner", focOFFSET( ProtoItem, Corner ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Dir", focOFFSET( ProtoItem, Dir ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 Slot", focOFFSET( ProtoItem, Slot ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weight", focOFFSET( ProtoItem, Weight ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Volume", focOFFSET( ProtoItem, Volume ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Cost", focOFFSET( ProtoItem, Cost ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint StartCount", focOFFSET( ProtoItem, StartCount ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 SoundId", focOFFSET( ProtoItem, SoundId ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 Material", focOFFSET( ProtoItem, Material ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 LightFlags", focOFFSET( ProtoItem, LightFlags ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 LightDistance", focOFFSET( ProtoItem, LightDistance ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int8 LightIntensity", focOFFSET( ProtoItem, LightIntensity ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint LightColor", focOFFSET( ProtoItem, LightColor ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool DisableEgg", focOFFSET( ProtoItem, DisableEgg ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 AnimWaitBase", focOFFSET( ProtoItem, AnimWaitBase ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 AnimWaitRndMin", focOFFSET( ProtoItem, AnimWaitRndMin ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 AnimWaitRndMax", focOFFSET( ProtoItem, AnimWaitRndMax ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 AnimStay_0", focOFFSET( ProtoItem, AnimStay[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 AnimStay_1", focOFFSET( ProtoItem, AnimStay[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 AnimShow_0", focOFFSET( ProtoItem, AnimShow[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 AnimShow_1", focOFFSET( ProtoItem, AnimShow[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 AnimHide_0", focOFFSET( ProtoItem, AnimHide[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 AnimHide_1", focOFFSET( ProtoItem, AnimHide[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int16 OffsetX", focOFFSET( ProtoItem, OffsetX ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int16 OffsetY", focOFFSET( ProtoItem, OffsetY ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 SpriteCut", focOFFSET( ProtoItem, SpriteCut ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int8 DrawOrderOffsetHexY", focOFFSET( ProtoItem, DrawOrderOffsetHexY ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 RadioChannel", focOFFSET( ProtoItem, RadioChannel ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 RadioFlags", focOFFSET( ProtoItem, RadioFlags ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 RadioBroadcastSend", focOFFSET( ProtoItem, RadioBroadcastSend ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 RadioBroadcastRecv", focOFFSET( ProtoItem, RadioBroadcastRecv ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 IndicatorStart", focOFFSET( ProtoItem, IndicatorStart ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 IndicatorMax", focOFFSET( ProtoItem, IndicatorMax ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint HolodiskNum", focOFFSET( ProtoItem, HolodiskNum ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_0", focOFFSET( ProtoItem, StartValue[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_1", focOFFSET( ProtoItem, StartValue[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_2", focOFFSET( ProtoItem, StartValue[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_3", focOFFSET( ProtoItem, StartValue[3] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_4", focOFFSET( ProtoItem, StartValue[4] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_5", focOFFSET( ProtoItem, StartValue[5] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_6", focOFFSET( ProtoItem, StartValue[6] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_7", focOFFSET( ProtoItem, StartValue[7] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_8", focOFFSET( ProtoItem, StartValue[8] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int StartValue_9", focOFFSET( ProtoItem, StartValue[9] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 BlockLines", focOFFSET( ProtoItem, BlockLines ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 ChildPid_0", focOFFSET( ProtoItem, ChildPid[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 ChildPid_1", focOFFSET( ProtoItem, ChildPid[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 ChildPid_2", focOFFSET( ProtoItem, ChildPid[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 ChildPid_3", focOFFSET( ProtoItem, ChildPid[3] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 ChildPid_4", focOFFSET( ProtoItem, ChildPid[4] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 ChildLines_0", focOFFSET( ProtoItem, ChildLines[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 ChildLines_1", focOFFSET( ProtoItem, ChildLines[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 ChildLines_2", focOFFSET( ProtoItem, ChildLines[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 ChildLines_3", focOFFSET( ProtoItem, ChildLines[3] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 ChildLines_4", focOFFSET( ProtoItem, ChildLines[4] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Weapon_IsUnarmed", focOFFSET( ProtoItem, Weapon_IsUnarmed ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_UnarmedTree", focOFFSET( ProtoItem, Weapon_UnarmedTree ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_UnarmedPriority", focOFFSET( ProtoItem, Weapon_UnarmedPriority ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_UnarmedMinAgility", focOFFSET( ProtoItem, Weapon_UnarmedMinAgility ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_UnarmedMinUnarmed", focOFFSET( ProtoItem, Weapon_UnarmedMinUnarmed ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_UnarmedMinLevel", focOFFSET( ProtoItem, Weapon_UnarmedMinLevel ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_Anim1", focOFFSET( ProtoItem, Weapon_Anim1 ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_MaxAmmoCount", focOFFSET( ProtoItem, Weapon_MaxAmmoCount ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_Caliber", focOFFSET( ProtoItem, Weapon_Caliber ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 Weapon_DefaultAmmoPid", focOFFSET( ProtoItem, Weapon_DefaultAmmoPid ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_MinStrength", focOFFSET( ProtoItem, Weapon_MinStrength ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_Perk", focOFFSET( ProtoItem, Weapon_Perk ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_ActiveUses", focOFFSET( ProtoItem, Weapon_ActiveUses ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_Skill_0", focOFFSET( ProtoItem, Weapon_Skill[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_Skill_1", focOFFSET( ProtoItem, Weapon_Skill[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Weapon_Skill_2", focOFFSET( ProtoItem, Weapon_Skill[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_PicUse_0", focOFFSET( ProtoItem, Weapon_PicUse[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_PicUse_1", focOFFSET( ProtoItem, Weapon_PicUse[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_PicUse_2", focOFFSET( ProtoItem, Weapon_PicUse[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_MaxDist_0", focOFFSET( ProtoItem, Weapon_MaxDist[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_MaxDist_1", focOFFSET( ProtoItem, Weapon_MaxDist[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_MaxDist_2", focOFFSET( ProtoItem, Weapon_MaxDist[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_Round_0", focOFFSET( ProtoItem, Weapon_Round[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_Round_1", focOFFSET( ProtoItem, Weapon_Round[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_Round_2", focOFFSET( ProtoItem, Weapon_Round[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_ApCost_0", focOFFSET( ProtoItem, Weapon_ApCost[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_ApCost_1", focOFFSET( ProtoItem, Weapon_ApCost[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Weapon_ApCost_2", focOFFSET( ProtoItem, Weapon_ApCost[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Weapon_Aim_0", focOFFSET( ProtoItem, Weapon_Aim[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Weapon_Aim_1", focOFFSET( ProtoItem, Weapon_Aim[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Weapon_Aim_2", focOFFSET( ProtoItem, Weapon_Aim[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 Weapon_SoundId_0", focOFFSET( ProtoItem, Weapon_SoundId[0] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 Weapon_SoundId_1", focOFFSET( ProtoItem, Weapon_SoundId[1] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint8 Weapon_SoundId_2", focOFFSET( ProtoItem, Weapon_SoundId[2] ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Ammo_Caliber", focOFFSET( ProtoItem, Ammo_Caliber ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Door_NoBlockMove", focOFFSET( ProtoItem, Door_NoBlockMove ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Door_NoBlockShoot", focOFFSET( ProtoItem, Door_NoBlockShoot ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Door_NoBlockLight", focOFFSET( ProtoItem, Door_NoBlockLight ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Container_Volume", focOFFSET( ProtoItem, Container_Volume ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Container_Changeble", focOFFSET( ProtoItem, Container_Changeble ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Container_CannotPickUp", focOFFSET( ProtoItem, Container_CannotPickUp ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const bool Container_MagicHandsGrnd", focOFFSET( ProtoItem, Container_MagicHandsGrnd ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint16 Locker_Condition", focOFFSET( ProtoItem, Locker_Condition ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const int Grid_Type", focOFFSET( ProtoItem, Grid_Type ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_Speed", focOFFSET( ProtoItem, Car_Speed ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_Passability", focOFFSET( ProtoItem, Car_Passability ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_DeteriorationRate", focOFFSET( ProtoItem, Car_DeteriorationRate ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_CrittersCapacity", focOFFSET( ProtoItem, Car_CrittersCapacity ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_TankVolume", focOFFSET( ProtoItem, Car_TankVolume ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_MaxDeterioration", focOFFSET( ProtoItem, Car_MaxDeterioration ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_FuelConsumption", focOFFSET( ProtoItem, Car_FuelConsumption ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_Entrance", focOFFSET( ProtoItem, Car_Entrance ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "ProtoItem", "const uint Car_MovementType", focOFFSET( ProtoItem, Car_MovementType ) ) );

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectMethod( "ProtoItem", "string@ GetScriptName() const", focFUNCTION( BIND_CLASS ProtoItem_GetScriptName ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // Scenery
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterObjectProperty( "Scenery", "const uint16 ProtoId", focOFFSET( MapObject, ProtoId ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Scenery", "const uint16 HexX", focOFFSET( MapObject, MapX ) ) );
        BIND_ASSERT( engine->RegisterObjectProperty( "Scenery", "const uint16 HexY", focOFFSET( MapObject, MapY ) ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Scenery", "bool CallSceneryFunction(Critter& cr, int skill, Item@+ item)", focFUNCTION( BIND_CLASS Scen_CallSceneryFunction ), asCALL_CDECL_OBJFIRST ) );
    }
    #endif

    //
    // Synchronizer
    //

    #if defined (BIND_DUMMY) || defined (BIND_SERVER)
    if( bind == SCRIPT_BIND_SERVER )
    {
        BIND_ASSERT( engine->RegisterGlobalFunction( "void Synchronize()", focFUNCTION( BIND_CLASS Global_Synchronize ), asCALL_CDECL ) );
        BIND_ASSERT( engine->RegisterGlobalFunction( "void Resynchronize()", focFUNCTION( BIND_CLASS Global_Resynchronize ), asCALL_CDECL ) );

        BIND_ASSERT( engine->RegisterObjectMethod( "Synchronizer", "void Lock()", focMETHOD( SyncObject, Lock ), asCALL_THISCALL ) );
    }
    #endif

    return true;
}
