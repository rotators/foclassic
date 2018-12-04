#include "Core.h"

#include <angelscript.h>

#include "DynamicLibrary.h"
#include "GameOptions.h"
#include "Log.h"
#include "Random.h"
#include "Script.h"

// copy macros related to registration
#define focFUNCTION( func )                        asFUNCTION( func )
#define focMETHOD( clas, member )                  asMETHOD( clas, member )
#define focMETHODPR( clas, member, decl, ret )     asMETHODPR( clas, member, decl, ret )
#define focOFFSET( clas, member )                  asOFFSET( clas, member )
#define focCALL_CDECL                              asCALL_CDECL
#define focCALL_CDECL_OBJFIRST                     asCALL_CDECL_OBJFIRST
#define focCALL_THISCALL                           asCALL_THISCALL

// other macros
#define focSIZEOF( obj )                           sizeof(obj)

#if defined (FOCLASSIC_CLIENT)
# include "Client.h"
# define BIND_CLIENT
# define BIND_CLASS                                FOClient::SScriptFunc::
# define BIND_ASSERT( x )                          if( (x) < 0 ) { WriteLog( "Bind error, line<%d>.\n", __LINE__ ); bind_errors++; }
#elif defined (FOCLASSIC_MAPPER)
# include "Mapper.h"
# define BIND_MAPPER
# define BIND_CLASS                                FOMapper::SScriptFunc::
# define BIND_ASSERT( x )                          if( (x) < 0 ) { WriteLogF( _FUNC_, " - Bind error, line<%d>.\n", __LINE__ ); }
#elif defined (FOCLASSIC_SERVER)
# include "Map.h"
# include "Server.h"
# define BIND_SERVER
# define BIND_CLASS                                FOServer::SScriptFunc::
# define BIND_ASSERT( x )                          if( (x) < 0 ) { WriteLogF( _FUNC_, " - Bind error, line<%d>.\n", __LINE__ ); return false; }
#elif defined (FOCLASSIC_SCRIPT_COMPILER)
// change the meaning of registration related macros
// allows ASCompiler to compile without knowing
# undef focFUNCTION
# undef focMETHOD
# undef focMETHODPR
# undef focOFFSET
# undef focSIZEOF
# undef focCALL_CDECL
# undef focCALL_CDECL_OBJFIRST
# undef focCALL_THISCALL
# define focCALL_CDECL                             asCALL_GENERIC
# define focCALL_CDECL_OBJFIRST                    asCALL_GENERIC
# define focCALL_THISCALL                          asCALL_GENERIC
# define focFUNCTION( func )                       asFUNCTION( DummyFunction )
# define focMETHOD( clas, method )                 asFUNCTION( DummyFunction )
# define focMETHODPR( clas, method, decl, ret )    asFUNCTION( DummyFunction )
# define focOFFSET( clas, method )                 asOFFSET( DummyClass, DummyMember )
# define focSIZEOF( obj )                          sizeof(DummyClass)
# define BIND_DUMMY
# define BIND_ASSERT( x )                          if( (x) < 0 ) { WriteLogF( _FUNC_, " - Bind error, line<%d>.\n", __LINE__ ); return false; }

struct DummyClass
{
    int DummyMember;
};

static void DummyFunction( asIScriptGeneric* ) {}

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

/* focCALL_CDECL */
bool Script::RegisterGlobalFunction( asIScriptEngine* engine, string declaration, const asSFuncPtr& function, asDWORD callConv, string name )
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

/* focCALL_CDECL_OBJFIRST */
bool Script::RegisterObjectMethod( asIScriptEngine* engine, string object, string method, const asSFuncPtr& function, asDWORD callConv, string name )
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

bool Script::RegisterAll( asIScriptEngine* engine )
{
    uint bind_errors = 0;

// Options
    BIND_ASSERT( engine->SetEngineProperty( asEP_ALLOW_UNSAFE_REFERENCES, true ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_OPTIMIZE_BYTECODE, true ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_SCRIPT_SCANNER, 1 ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_USE_CHARACTER_LITERALS, 1 ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_AUTO_GARBAGE_COLLECT, false ) );
    BIND_ASSERT( engine->SetEngineProperty( asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT, true ) );

    #if defined (BIND_CLIENT) || defined (BIND_SERVER)
    // Reference value
    BIND_ASSERT( engine->RegisterObjectType( "DataRef", 0, asOBJ_REF | asOBJ_NOHANDLE ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "DataRef", "const int& opIndex(uint) const", focFUNCTION( BIND_CLASS DataRef_Index ), focCALL_CDECL_OBJFIRST ) );
    # if defined (BIND_SERVER)
    BIND_ASSERT( engine->RegisterObjectMethod( "DataRef", "int& opIndex(uint)", focFUNCTION( BIND_CLASS DataRef_Index ), focCALL_CDECL_OBJFIRST ) );
    # endif
    // Computed value
    BIND_ASSERT( engine->RegisterObjectType( "DataVal", 0, asOBJ_REF | asOBJ_NOHANDLE ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "DataVal", "const int opIndex(uint) const", focFUNCTION( BIND_CLASS DataVal_Index ), focCALL_CDECL_OBJFIRST ) );
    #endif    // #if defined(BIND_CLIENT) || defined(BIND_SERVER)

// Item prototype
    BIND_ASSERT( engine->RegisterObjectType( "ProtoItem", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "ProtoItem", asBEHAVE_ADDREF, "void f()", focMETHOD( ProtoItem, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "ProtoItem", asBEHAVE_RELEASE, "void f()", focMETHOD( ProtoItem, Release ), focCALL_THISCALL ) );

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

    #if defined (BIND_SERVER)
    BIND_ASSERT( engine->RegisterObjectMethod( "ProtoItem", "string@ GetScriptName() const", focFUNCTION( BIND_CLASS ProtoItem_GetScriptName ), focCALL_CDECL_OBJFIRST ) );

    /************************************************************************/
    /* Types                                                                */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectType( "GameVar", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "GameVar", asBEHAVE_ADDREF, "void f()", focMETHOD( GameVar, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "GameVar", asBEHAVE_RELEASE, "void f()", focMETHOD( GameVar, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectType( "NpcPlane", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "NpcPlane", asBEHAVE_ADDREF, "void f()", focMETHOD( AIDataPlane, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "NpcPlane", asBEHAVE_RELEASE, "void f()", focMETHOD( AIDataPlane, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectType( "Item", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Item", asBEHAVE_ADDREF, "void f()", focMETHOD( Item, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Item", asBEHAVE_RELEASE, "void f()", focMETHOD( Item, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectType( "Scenery", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Scenery", asBEHAVE_ADDREF, "void f()", focMETHOD( MapObject, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Scenery", asBEHAVE_RELEASE, "void f()", focMETHOD( MapObject, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectType( "Critter", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Critter", asBEHAVE_ADDREF, "void f()", focMETHOD( Critter, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Critter", asBEHAVE_RELEASE, "void f()", focMETHOD( Critter, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectType( "Map", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Map", asBEHAVE_ADDREF, "void f()", focMETHOD( Map, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Map", asBEHAVE_RELEASE, "void f()", focMETHOD( Map, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectType( "Location", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Location", asBEHAVE_ADDREF, "void f()", focMETHOD( Location, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Location", asBEHAVE_RELEASE, "void f()", focMETHOD( Location, Release ), focCALL_THISCALL ) );

    /************************************************************************/
    /* Synchronizer                                                         */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectType( "Synchronizer", focSIZEOF( SyncObject ), asOBJ_VALUE ) );

    BIND_ASSERT( engine->RegisterObjectBehaviour( "Synchronizer", asBEHAVE_CONSTRUCT, "void f()", focFUNCTION( BIND_CLASS Synchronizer_Constructor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "Synchronizer", asBEHAVE_DESTRUCT, "void f()", focFUNCTION( BIND_CLASS Synchronizer_Destructor ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Synchronizer", "void Lock()", focMETHOD( SyncObject, Lock ), focCALL_THISCALL ) );

    /************************************************************************/
    /* GameVar                                                              */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int GetValue() const", focMETHOD( GameVar, GetValue ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int GetMin() const", focMETHOD( GameVar, GetMin ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int GetMax() const", focMETHOD( GameVar, GetMax ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "bool IsQuest() const", focMETHOD( GameVar, IsQuest ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "uint GetQuestStr() const", focMETHOD( GameVar, GetQuestStr ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAddAssign(const int)", focMETHODPR( GameVar, operator+=, (const int), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opSubAssign(const int)", focMETHODPR( GameVar, operator-=, (const int), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opMulAssign(const int)", focMETHODPR( GameVar, operator*=, (const int), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opDivAssign(const int)", focMETHODPR( GameVar, operator/=, (const int), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAssign(const int)", focMETHODPR( GameVar, operator=, (const int), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAddAssign(const GameVar&)", focMETHODPR( GameVar, operator+=, (const GameVar &), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opSubAssign(const GameVar&)", focMETHODPR( GameVar, operator-=, (const GameVar &), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opMulAssign(const GameVar&)", focMETHODPR( GameVar, operator*=, (const GameVar &), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opDivAssign(const GameVar&)", focMETHODPR( GameVar, operator/=, (const GameVar &), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "GameVar& opAssign(const GameVar&)", focMETHODPR( GameVar, operator=, (const GameVar &), GameVar& ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opAdd(const int)", focFUNCTION( GameVarAddInt ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opSub(const int)", focFUNCTION( GameVarSubInt ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opMul(const int)", focFUNCTION( GameVarMulInt ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opDiv(const int)", focFUNCTION( GameVarDivInt ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opAdd(const GameVar&)", focFUNCTION( GameVarAddGameVar ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opSub(const GameVar&)", focFUNCTION( GameVarSubGameVar ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opMul(const GameVar&)", focFUNCTION( GameVarMulGameVar ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opDiv(const GameVar&)", focFUNCTION( GameVarDivGameVar ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "bool opEquals(const int)", focFUNCTION( GameVarEqualInt ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opCmp(const int)", focFUNCTION( GameVarCmpInt ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "bool opEquals(const GameVar&)", focFUNCTION( GameVarEqualGameVar ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "GameVar", "int opCmp(const GameVar&)", focFUNCTION( GameVarCmpGameVar ), focCALL_CDECL_OBJFIRST ) );

    /************************************************************************/
    /* NpcPlane                                                             */
    /************************************************************************/
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

    BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "NpcPlane@ GetCopy() const", focFUNCTION( BIND_CLASS NpcPlane_GetCopy ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "NpcPlane@+ SetChild(NpcPlane& child)", focFUNCTION( BIND_CLASS NpcPlane_SetChild ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "NpcPlane@+ GetChild(uint index) const", focFUNCTION( BIND_CLASS NpcPlane_GetChild ), focCALL_CDECL_OBJFIRST ) );

    // BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "uint GetIndex() const", focFUNCTION(BIND_CLASS NpcPlane_GetIndex), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "NpcPlane", "bool Misc_SetScript(string& funcName)", focFUNCTION( BIND_CLASS NpcPlane_Misc_SetScript ), focCALL_CDECL_OBJFIRST ) );

    /************************************************************************/
    /* Item                                                                 */
    /************************************************************************/
    // Methods
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool IsStackable() const", focFUNCTION( BIND_CLASS Item_IsStackable ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool IsDeteriorable() const", focFUNCTION( BIND_CLASS Item_IsDeteriorable ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Item_SetScript ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Item_GetScriptId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Item_SetEvent ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint8 GetType() const", focFUNCTION( BIND_CLASS Item_GetType ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Item_GetProtoId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetCount() const", focFUNCTION( BIND_CLASS Item_GetCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void SetCount(uint count)", focFUNCTION( BIND_CLASS Item_SetCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetCost() const", focFUNCTION( BIND_CLASS Item_GetCost ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Item@+ AddItem(uint16 protoId, uint count, uint specialId)", focFUNCTION( BIND_CLASS Container_AddItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Item@+ GetItem(uint16 protoId, uint specialId) const", focFUNCTION( BIND_CLASS Container_GetItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint GetItems(uint specialId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Container_GetItems ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Map@+ GetMapPosition(uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Item_GetMapPosition ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool ChangeProto(uint16 protoId) const", focFUNCTION( BIND_CLASS Item_ChangeProto ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void Update() const", focFUNCTION( BIND_CLASS Item_Update ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void Animate(uint8 fromFrame, uint8 toFrame)", focFUNCTION( BIND_CLASS Item_Animate ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void SetLexems(string@+ lexems)", focFUNCTION( BIND_CLASS Item_SetLexems ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "Item@+ GetChild(uint childIndex) const", focFUNCTION( BIND_CLASS Item_GetChild ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Item_EventFinish ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventAttack(Critter& attacker, Critter& target)", focFUNCTION( BIND_CLASS Item_EventAttack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventUse(Critter& cr, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Item_EventUse ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventUseOnMe(Critter& cr, Item@+ usedItem)", focFUNCTION( BIND_CLASS Item_EventUseOnMe ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool EventSkill(Critter& cr, int skill)", focFUNCTION( BIND_CLASS Item_EventSkill ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventDrop(Critter& cr)", focFUNCTION( BIND_CLASS Item_EventDrop ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventMove(Critter& cr, uint8 fromSlot)", focFUNCTION( BIND_CLASS Item_EventMove ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void EventWalk(Critter& cr, bool entered, uint8 dir)", focFUNCTION( BIND_CLASS Item_EventWalk ), focCALL_CDECL_OBJFIRST ) );

    // Parameters
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
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void set_Flags(uint value)", focFUNCTION( BIND_CLASS Item_set_Flags ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "uint get_Flags() const", focFUNCTION( BIND_CLASS Item_get_Flags ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "void set_TrapValue(int16 val)", focFUNCTION( BIND_CLASS Item_set_TrapValue ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "int16 get_TrapValue() const", focFUNCTION( BIND_CLASS Item_get_TrapValue ), focCALL_CDECL_OBJFIRST ) );
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
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool LockerOpen()", focFUNCTION( BIND_CLASS Item_LockerOpen ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Item", "bool LockerClose()", focFUNCTION( BIND_CLASS Item_LockerClose ), focCALL_CDECL_OBJFIRST ) );
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

    /************************************************************************/
    /* CraftItem
       /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectType( "CraftItem", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "CraftItem", asBEHAVE_ADDREF, "void f()", focMETHOD( CraftItem, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "CraftItem", asBEHAVE_RELEASE, "void f()", focMETHOD( CraftItem, Release ), focCALL_THISCALL ) );

    // Properties
    BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const uint Num", focOFFSET( CraftItem, Num ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const string Name", focOFFSET( CraftItem, Name ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const string Info", focOFFSET( CraftItem, Info ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const uint Experience", focOFFSET( CraftItem, Experience ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "CraftItem", "const string Script", focOFFSET( CraftItem, Script ) ) );

    // Methods
    BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetShowParams(array<uint>@+ nums, array<int>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetShowParams ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetNeedParams(array<uint>@+ nums, array<int>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedParams ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetNeedTools(array<uint16>@+ pids, array<uint>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedTools ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetNeedItems(array<uint16>@+ pids, array<uint>@+ values, array<bool>@+ ors)", focFUNCTION( BIND_CLASS CraftItem_GetNeedItems ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CraftItem", "uint GetOutItems(array<uint16>@+ pids, array<uint>@+ values)", focFUNCTION( BIND_CLASS CraftItem_GetOutItems ), focCALL_CDECL_OBJFIRST ) );

    /************************************************************************/
    /* Scenery                                                              */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectProperty( "Scenery", "const uint16 ProtoId", focOFFSET( MapObject, ProtoId ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "Scenery", "const uint16 HexX", focOFFSET( MapObject, MapX ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "Scenery", "const uint16 HexY", focOFFSET( MapObject, MapY ) ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Scenery", "bool CallSceneryFunction(Critter& cr, int skill, Item@+ item)", focFUNCTION( BIND_CLASS Scen_CallSceneryFunction ), focCALL_CDECL_OBJFIRST ) );

    /************************************************************************/
    /* Critter                                                              */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsPlayer() const", focFUNCTION( BIND_CLASS Crit_IsPlayer ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsNpc() const", focFUNCTION( BIND_CLASS Crit_IsNpc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanWalk() const", focFUNCTION( BIND_CLASS Crit_IsCanWalk ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanRun() const", focFUNCTION( BIND_CLASS Crit_IsCanRun ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanRotate() const", focFUNCTION( BIND_CLASS Crit_IsCanRotate ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCanAim() const", focFUNCTION( BIND_CLASS Crit_IsCanAim ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsAnim1(uint index) const", focFUNCTION( BIND_CLASS Crit_IsAnim1 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint8 GetAccess() const", focFUNCTION( BIND_CLASS Cl_GetAccess ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetAccess(uint8 access) const", focFUNCTION( BIND_CLASS Cl_SetAccess ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Crit_SetEvent ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetLexems(string@+ lexems)", focFUNCTION( BIND_CLASS Crit_SetLexems ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Map@+ GetMap() const", focFUNCTION( BIND_CLASS Crit_GetMap ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetMapId() const", focFUNCTION( BIND_CLASS Crit_GetMapId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint16 GetMapProtoId() const", focFUNCTION( BIND_CLASS Crit_GetMapProtoId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetHomePos(uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Crit_SetHomePos ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GetHomePos(uint& mapId, uint16& hexX, uint16& hexY, uint8& dir)", focFUNCTION( BIND_CLASS Crit_GetHomePos ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ChangeCrType(uint newType)", focFUNCTION( BIND_CLASS Crit_ChangeCrType ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void DropTimers()", focFUNCTION( BIND_CLASS Cl_DropTimers ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool MoveRandom()", focFUNCTION( BIND_CLASS Crit_MoveRandom ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool MoveToDir(uint8 dir)", focFUNCTION( BIND_CLASS Crit_MoveToDir ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToHex(uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Crit_TransitToHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToMap(uint mapId, uint16 hexX, uint16 hexY, uint8 dir, bool withGroup = false)", focFUNCTION( BIND_CLASS Crit_TransitToMapHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToMap(uint mapId, int entireNum, bool withGroup = false)", focFUNCTION( BIND_CLASS Crit_TransitToMapEntire ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToGlobal(bool requestGroup)", focFUNCTION( BIND_CLASS Crit_TransitToGlobal ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToGlobal(Critter@[]& group)", focFUNCTION( BIND_CLASS Crit_TransitToGlobalWithGroup ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool TransitToGlobalGroup(uint critterId)", focFUNCTION( BIND_CLASS Crit_TransitToGlobalGroup ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void AddScore(uint score, int val)", focFUNCTION( BIND_CLASS Crit_AddScore ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int GetScore(uint score)", focFUNCTION( BIND_CLASS Crit_GetScore ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void AddHolodiskInfo(uint holodiskNum)", focFUNCTION( BIND_CLASS Crit_AddHolodiskInfo ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EraseHolodiskInfo(uint holodiskNum)", focFUNCTION( BIND_CLASS Crit_EraseHolodiskInfo ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsHolodiskInfo(uint holodiskNum) const", focFUNCTION( BIND_CLASS Crit_IsHolodiskInfo ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsLife() const", focFUNCTION( BIND_CLASS Crit_IsLife ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsKnockout() const", focFUNCTION( BIND_CLASS Crit_IsKnockout ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsDead() const", focFUNCTION( BIND_CLASS Crit_IsDead ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsFree() const", focFUNCTION( BIND_CLASS Crit_IsFree ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsBusy() const", focFUNCTION( BIND_CLASS Crit_IsBusy ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Wait(uint ms)", focFUNCTION( BIND_CLASS Crit_Wait ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ToDead(uint anim2, Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_ToDead ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ToLife()", focFUNCTION( BIND_CLASS Crit_ToLife ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ToKnockout(uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint16 knockHx, uint16 knockHy)", focFUNCTION( BIND_CLASS Crit_ToKnockout ), focCALL_CDECL_OBJFIRST ) );

    // BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GetProtoData(int[]& data)", focFUNCTION( BIND_CLASS Npc_GetProtoData ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void RefreshVisible()", focFUNCTION( BIND_CLASS Crit_RefreshVisible ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ViewMap(Map& map, uint look, uint16 hx, uint16 hy, uint8 dir)", focFUNCTION( BIND_CLASS Crit_ViewMap ), focCALL_CDECL_OBJFIRST ) );
    // BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Mute(uint ms)", focFUNCTION( BIND_CLASS Crit_Mute ), focCALL_CDECL_OBJFIRST ) );
    // BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Ban(uint ms)", focFUNCTION( BIND_CLASS Crit_Mute ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Item@+ AddItem(uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Crit_AddItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool DeleteItem(uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Crit_DeleteItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ItemsCount() const", focFUNCTION( BIND_CLASS Crit_ItemsCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ItemsWeight() const", focFUNCTION( BIND_CLASS Crit_ItemsWeight ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ItemsVolume() const", focFUNCTION( BIND_CLASS Crit_ItemsVolume ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint CountItem(uint16 protoId) const", focFUNCTION( BIND_CLASS Crit_CountItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Item@+ GetItem(uint16 protoId, int slot) const", focFUNCTION( BIND_CLASS Crit_GetItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Item@+ GetItemById(uint itemId) const", focFUNCTION( BIND_CLASS Crit_GetItemById ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetItems(int slot, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItems ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetItemsByType(int type, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItemsByType ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "ProtoItem@+ GetSlotProto(int slot, uint8& mode) const", focFUNCTION( BIND_CLASS Crit_GetSlotProto ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool MoveItem(uint itemId, uint count, uint8 toSlot)", focFUNCTION( BIND_CLASS Crit_MoveItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool PickItem(uint16 hexX, uint16 hexY, uint16 protoId)", focFUNCTION( BIND_CLASS Crit_PickItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetFavoriteItem(int slot, uint16 pid)", focFUNCTION( BIND_CLASS Crit_SetFavoriteItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint16 GetFavoriteItem(int slot)", focFUNCTION( BIND_CLASS Crit_GetFavoriteItem ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetCritters(bool lookOnMe, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Crit_GetCritters ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetFollowGroup(int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Crit_GetFollowGroup ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Critter@+ GetFollowLeader() const", focFUNCTION( BIND_CLASS Crit_GetFollowLeader ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "Critter@[]@ GetGlobalGroup() const", focFUNCTION( BIND_CLASS Crit_GetGlobalGroup ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsGlobalGroupLeader()", focFUNCTION( BIND_CLASS Crit_IsGlobalGroupLeader ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void LeaveGlobalGroup()", focFUNCTION( BIND_CLASS Crit_LeaveGlobalGroup ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GiveGlobalGroupLead(Critter& toCr)", focFUNCTION( BIND_CLASS Crit_GiveGlobalGroupLead ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetTalkedPlayers(Critter@[]@+ players) const", focFUNCTION( BIND_CLASS Npc_GetTalkedPlayers ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsSee(Critter& cr) const", focFUNCTION( BIND_CLASS Crit_IsSeeCr ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsSeenBy(Critter& cr) const", focFUNCTION( BIND_CLASS Crit_IsSeenByCr ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsSee(Item& item) const", focFUNCTION( BIND_CLASS Crit_IsSeeItem ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Say(uint8 howSay, string& text)", focFUNCTION( BIND_CLASS Crit_Say ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SayMsg(uint8 howSay, uint16 textMsg, uint strNum)", focFUNCTION( BIND_CLASS Crit_SayMsg ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SayMsg(uint8 howSay, uint16 textMsg, uint strNum, string& lexems)", focFUNCTION( BIND_CLASS Crit_SayMsgLex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetDir(uint8 dir)", focFUNCTION( BIND_CLASS Crit_SetDir ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint ErasePlane(int planeType, bool all)", focFUNCTION( BIND_CLASS Npc_ErasePlane ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool ErasePlane(uint index)", focFUNCTION( BIND_CLASS Npc_ErasePlaneIndex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void DropPlanes()", focFUNCTION( BIND_CLASS Npc_DropPlanes ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsNoPlanes() const", focFUNCTION( BIND_CLASS Npc_IsNoPlanes ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsCurPlane(int planeType) const", focFUNCTION( BIND_CLASS Npc_IsCurPlane ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "NpcPlane@+ GetCurPlane() const", focFUNCTION( BIND_CLASS Npc_GetCurPlane ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetPlanes(NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanes ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetPlanes(int identifier, NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanesIdentifier ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetPlanes(int identifier, uint identifierExt, NpcPlane@[]@+ planes) const", focFUNCTION( BIND_CLASS Npc_GetPlanesIdentifier2 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool AddPlane(NpcPlane& plane)", focFUNCTION( BIND_CLASS Npc_AddPlane ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SendMessage(int num, int val, int to)", focFUNCTION( BIND_CLASS Crit_SendMessage ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Action(int action, int actionExt, Item@+ item)", focFUNCTION( BIND_CLASS Crit_Action ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Animate(uint anim1, uint anim2, Item@+ item, bool clearSequence, bool delayPlay)", focFUNCTION( BIND_CLASS Crit_Animate ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetAnims(int cond, uint anim1, uint anim2)", focFUNCTION( BIND_CLASS Crit_SetAnims ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void PlaySound(string& soundName, bool sendSelf)", focFUNCTION( BIND_CLASS Crit_PlaySound ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void PlaySound(uint8 soundType, uint8 soundTypeExt, uint8 soundId, uint8 soundIdExt, bool sendSelf)", focFUNCTION( BIND_CLASS Crit_PlaySoundType ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SendCombatResult(uint[]& combatResult)", focFUNCTION( BIND_CLASS Crit_SendCombatResult ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool IsKnownLoc(bool byId, uint locNum) const", focFUNCTION( BIND_CLASS Cl_IsKnownLoc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool SetKnownLoc(bool byId, uint locNum)", focFUNCTION( BIND_CLASS Cl_SetKnownLoc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool UnsetKnownLoc(bool byId, uint locNum)", focFUNCTION( BIND_CLASS Cl_UnsetKnownLoc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetFog(uint16 zoneX, uint16 zoneY, int fog)", focFUNCTION( BIND_CLASS Cl_SetFog ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int GetFog(uint16 zoneX, uint16 zoneY)", focFUNCTION( BIND_CLASS Cl_GetFog ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ShowContainer(Critter@+ contCr, Item@+ contItem, uint8 transferType)", focFUNCTION( BIND_CLASS Cl_ShowContainer ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ShowScreen(int screenType, uint param, string@+ funcName)", focFUNCTION( BIND_CLASS Cl_ShowScreen ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void RunClientScript(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Cl_RunClientScript ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void Disconnect()", focFUNCTION( BIND_CLASS Cl_Disconnect ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Crit_SetScript ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Crit_GetScriptId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetBagRefreshTime(uint realMinutes)", focFUNCTION( BIND_CLASS Crit_SetBagRefreshTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetBagRefreshTime() const", focFUNCTION( BIND_CLASS Crit_GetBagRefreshTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetInternalBag(uint16[]& pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots)", focFUNCTION( BIND_CLASS Crit_SetInternalBag ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetInternalBag(uint16[]@+ pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots) const", focFUNCTION( BIND_CLASS Crit_GetInternalBag ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Crit_GetProtoId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetMultihex() const", focFUNCTION( BIND_CLASS Crit_GetMultihex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void SetMultihex(int value)", focFUNCTION( BIND_CLASS Crit_SetMultihex ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void AddEnemyInStack(uint critterId)", focFUNCTION( BIND_CLASS Crit_AddEnemyInStack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool CheckEnemyInStack(uint critterId) const", focFUNCTION( BIND_CLASS Crit_CheckEnemyInStack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EraseEnemyFromStack(uint critterId)", focFUNCTION( BIND_CLASS Crit_EraseEnemyFromStack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ChangeEnemyStackSize(uint newSize)", focFUNCTION( BIND_CLASS Crit_ChangeEnemyStackSize ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void GetEnemyStack(uint[]& enemyStack) const", focFUNCTION( BIND_CLASS Crit_GetEnemyStack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ClearEnemyStack()", focFUNCTION( BIND_CLASS Crit_ClearEnemyStack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ClearEnemyStackNpc()", focFUNCTION( BIND_CLASS Crit_ClearEnemyStackNpc ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool AddTimeEvent(string& funcName, uint duration, int identifier)", focFUNCTION( BIND_CLASS Crit_AddTimeEvent ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool AddTimeEvent(string& funcName, uint duration, int identifier, uint rate)", focFUNCTION( BIND_CLASS Crit_AddTimeEventRate ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetTimeEvents(int identifier, uint[]@+ indexes, uint[]@+ durations, uint[]@+ rates) const", focFUNCTION( BIND_CLASS Crit_GetTimeEvents ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint GetTimeEvents(int[]& findIdentifiers, int[]@+ identifiers, uint[]@+ indexes, uint[]@+ durations, uint[]@+ rates) const", focFUNCTION( BIND_CLASS Crit_GetTimeEventsArr ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void ChangeTimeEvent(uint index, uint newDuration, uint newRate)", focFUNCTION( BIND_CLASS Crit_ChangeTimeEvent ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EraseTimeEvent(uint index)", focFUNCTION( BIND_CLASS Crit_EraseTimeEvent ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint EraseTimeEvents(int identifier)", focFUNCTION( BIND_CLASS Crit_EraseTimeEvents ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "uint EraseTimeEvents(int[]& identifiers)", focFUNCTION( BIND_CLASS Crit_EraseTimeEventsArr ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventIdle()", focFUNCTION( BIND_CLASS Crit_EventIdle ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Crit_EventFinish ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventDead(Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_EventDead ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventRespawn()", focFUNCTION( BIND_CLASS Crit_EventRespawn ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter1(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter1 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter2(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter2 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowCritter3(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventShowCritter3 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter1(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter1 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter2(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter2 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideCritter3(Critter& cr)", focFUNCTION( BIND_CLASS Crit_EventHideCritter3 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventShowItemOnMap(Item& showItem, bool added, Critter@+ dropper)", focFUNCTION( BIND_CLASS Crit_EventShowItemOnMap ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventChangeItemOnMap(Item& item)", focFUNCTION( BIND_CLASS Crit_EventChangeItemOnMap ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventHideItemOnMap(Item& hideItem, bool removed, Critter@+ picker)", focFUNCTION( BIND_CLASS Crit_EventHideItemOnMap ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventAttack(Critter& target)", focFUNCTION( BIND_CLASS Crit_EventAttack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventAttacked(Critter@+ attacker)", focFUNCTION( BIND_CLASS Crit_EventAttacked ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventStealing(Critter& thief, Item& item, uint count)", focFUNCTION( BIND_CLASS Crit_EventStealing ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventMessage(Critter& fromCr, int message, int value)", focFUNCTION( BIND_CLASS Crit_EventMessage ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseItem(Item& item, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventUseItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseItemOnMe(Critter& whoUse, Item& item)", focFUNCTION( BIND_CLASS Crit_EventUseItemOnMe ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseSkill(int skill, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventUseSkill ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventUseSkillOnMe(Critter& whoUse, int skill)", focFUNCTION( BIND_CLASS Crit_EventUseSkillOnMe ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventDropItem(Item& item)", focFUNCTION( BIND_CLASS Crit_EventDropItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventMoveItem(Item& item, uint8 fromSlot)", focFUNCTION( BIND_CLASS Crit_EventMoveItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventKnockout(uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint knockDist)", focFUNCTION( BIND_CLASS Crit_EventKnockout ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthDead(Critter& fromCr, Critter@+ killer)", focFUNCTION( BIND_CLASS Crit_EventSmthDead ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthStealing(Critter& fromCr, Critter& thief, bool success, Item& item, uint count)", focFUNCTION( BIND_CLASS Crit_EventSmthStealing ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthAttack(Critter& fromCr, Critter& target)", focFUNCTION( BIND_CLASS Crit_EventSmthAttack ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthAttacked(Critter& fromCr, Critter@+ attacker)", focFUNCTION( BIND_CLASS Crit_EventSmthAttacked ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthUseItem(Critter& fromCr, Item& item, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventSmthUseItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthUseSkill(Critter& fromCr, int skill, Critter@+ onCritter, Item@+ onItem, Scenery@+ onScenery)", focFUNCTION( BIND_CLASS Crit_EventSmthUseSkill ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthDropItem(Critter& fromCr, Item& item)", focFUNCTION( BIND_CLASS Crit_EventSmthDropItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthMoveItem(Critter& fromCr, Item& item, uint8 fromSlot)", focFUNCTION( BIND_CLASS Crit_EventSmthMoveItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthKnockout(Critter& fromCr, uint anim2begin, uint anim2idle, uint anim2end, uint lostAp, uint knockDist)", focFUNCTION( BIND_CLASS Crit_EventSmthKnockout ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int EventPlaneBegin(NpcPlane& plane, int reason, Critter@+ someCr, Item@+ someItem)", focFUNCTION( BIND_CLASS Crit_EventPlaneBegin ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int EventPlaneEnd(NpcPlane& plane, int reason, Critter@+ someCr, Item@+ someItem)", focFUNCTION( BIND_CLASS Crit_EventPlaneEnd ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "int EventPlaneRun(NpcPlane& plane, int reason, uint& p0, uint& p1, uint& p2)", focFUNCTION( BIND_CLASS Crit_EventPlaneRun ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventBarter(Critter& barterCr, bool attach, uint barterCount)", focFUNCTION( BIND_CLASS Crit_EventBarter ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventTalk(Critter& talkCr, bool attach, uint talkCount)", focFUNCTION( BIND_CLASS Crit_EventTalk ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventGlobalProcess(int type, Item@ car, float& x, float& y, float& toX, float& toY, float& speed, uint& encounterDescriptor, bool& waitForAnswer)", focFUNCTION( BIND_CLASS Crit_EventGlobalProcess ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "bool EventGlobalInvite(Item@ car, uint encounterDescriptor, int combatMode, uint& mapId, uint16& hexX, uint16& hexY, uint8& dir)", focFUNCTION( BIND_CLASS Crit_EventGlobalInvite ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventTurnBasedProcess(Map& map, bool beginTurn)", focFUNCTION( BIND_CLASS Crit_EventTurnBasedProcess ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Critter", "void EventSmthTurnBasedProcess(Critter& fromCr, Map& map, bool beginTurn)", focFUNCTION( BIND_CLASS Crit_EventSmthTurnBasedProcess ), focCALL_CDECL_OBJFIRST ) );

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

    /************************************************************************/
    /* Map                                                                  */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const bool IsNotValid", focOFFSET( Map, IsNotValid ) ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Map_GetProtoId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Location@+ GetLocation() const", focFUNCTION( BIND_CLASS Map_GetLocation ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool SetScript(string@+ script)", focFUNCTION( BIND_CLASS Map_SetScript ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Map_GetScriptId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Map_SetEvent ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetLoopTime(uint numLoop, uint ms)", focFUNCTION( BIND_CLASS Map_SetLoopTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint8 GetRain() const", focFUNCTION( BIND_CLASS Map_GetRain ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetRain(uint8 capacity)", focFUNCTION( BIND_CLASS Map_SetRain ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "int GetTime() const", focFUNCTION( BIND_CLASS Map_GetTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTime(int time)", focFUNCTION( BIND_CLASS Map_SetTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetDayTime(uint dayPart) const", focFUNCTION( BIND_CLASS Map_GetDayTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetDayTime(uint dayPart, uint time)", focFUNCTION( BIND_CLASS Map_SetDayTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b) const", focFUNCTION( BIND_CLASS Map_GetDayColor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetDayColor(uint dayPart, uint8 r, uint8 g, uint8 b)", focFUNCTION( BIND_CLASS Map_SetDayColor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTurnBasedAvailability(bool value)", focFUNCTION( BIND_CLASS Map_SetTurnBasedAvailability ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsTurnBasedAvailability() const", focFUNCTION( BIND_CLASS Map_IsTurnBasedAvailability ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void BeginTurnBased(Critter@+ firstTurnCrit)", focFUNCTION( BIND_CLASS Map_BeginTurnBased ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsTurnBased() const", focFUNCTION( BIND_CLASS Map_IsTurnBased ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EndTurnBased()", focFUNCTION( BIND_CLASS Map_EndTurnBased ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "int GetTurnBasedSequence(uint[]& crittersIds) const", focFUNCTION( BIND_CLASS Map_GetTurnBasedSequence ), focCALL_CDECL_OBJFIRST ) );
    // BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTurnBasedSequence(uint[]& crittersIds)", focFUNCTION( BIND_CLASS Map_SetTurnBasedSequence ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetData(uint index, int value)", focFUNCTION( BIND_CLASS Map_SetData ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "int GetData(uint index) const", focFUNCTION( BIND_CLASS Map_GetData ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ AddItem(uint16 hexX, uint16 hexY, uint16 protoId, uint count)", focFUNCTION( BIND_CLASS Map_AddItem ), focCALL_CDECL_OBJFIRST ) );
    // BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ AddItemFromScenery(uint16 hexX, uint16 hexY, uint16 protoId)", focFUNCTION( BIND_CLASS Map_AddItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetItem(uint itemId) const", focFUNCTION( BIND_CLASS Map_GetItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetItem(uint16 hexX, uint16 hexY, uint16 protoId) const", focFUNCTION( BIND_CLASS Map_GetItemHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItems(uint16 hexX, uint16 hexY, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItems(uint16 hexX, uint16 hexY, uint radius, uint16 protoId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsHexEx ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItems(uint16 protoId, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsByPid ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetItemsByType(int type, Item@[]@+ items) const", focFUNCTION( BIND_CLASS Map_GetItemsByType ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetDoor(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetDoor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Item@+ GetCar(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetCar ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Scenery@+ GetScenery(uint16 hexX, uint16 hexY, uint16 protoId) const", focFUNCTION( BIND_CLASS Map_GetSceneryHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetSceneries(uint16 hexX, uint16 hexY, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetSceneries(uint16 hexX, uint16 hexY, uint radius, uint16 protoId, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesHexEx ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetSceneries(uint16 protoId, Scenery@[]@+ sceneries) const", focFUNCTION( BIND_CLASS Map_GetSceneriesByPid ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ GetCritter(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_GetCritterHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ GetCritter(uint critterId) const", focFUNCTION( BIND_CLASS Map_GetCritterById ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersHex(uint16 hexX, uint16 hexY, uint radius, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCritters ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCritters(uint16 pid, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersByPids ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersInPath ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, Critter@[]@+ critters, uint16& preBlockHx, uint16& preBlockHy, uint16& blockHx, uint16& blockHy) const", focFUNCTION( BIND_CLASS Map_GetCrittersInPathBlock ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersWhoViewPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, int findType, Critter@[]@+ critters) const", focFUNCTION( BIND_CLASS Map_GetCrittersWhoViewPath ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetCrittersSeeing(Critter@[]& critters, bool lookOnThem, int find_type, Critter@[]@+ crittersResult) const", focFUNCTION( BIND_CLASS Map_GetCrittersSeeing ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist) const", focFUNCTION( BIND_CLASS Map_GetHexInPath ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void GetHexCoordWall(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist) const", focFUNCTION( BIND_CLASS Map_GetHexInPathWall ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut) const", focFUNCTION( BIND_CLASS Map_GetPathLengthHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetPathLength(Critter& cr, uint16 toHx, uint16 toHy, uint cut) const", focFUNCTION( BIND_CLASS Map_GetPathLengthCr ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool VerifyTrigger(Critter& cr, uint16 hexX, uint16 hexY, uint8 dir)", focFUNCTION( BIND_CLASS Map_VerifyTrigger ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ AddNpc(uint16 protoId, uint16 hexX, uint16 hexY, uint8 dir, int[]@+ params, int[]@+ items, string@+ script)", focFUNCTION( BIND_CLASS Map_AddNpc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetNpcCount(int npcRole, int findType) const", focFUNCTION( BIND_CLASS Map_GetNpcCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "Critter@+ GetNpc(int npcRole, int findType, uint skipCount) const", focFUNCTION( BIND_CLASS Map_GetNpc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint CountEntire(int entire) const", focFUNCTION( BIND_CLASS Map_CountEntire ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint GetEntires(int entire, uint[]@+ entires, uint16[]@+ hexX, uint16[]@+ hexY) const", focFUNCTION( BIND_CLASS Map_GetEntires ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetEntireCoords(int entire, uint skip, uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Map_GetEntireCoords ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetEntireCoords(int entire, uint skip, uint16& hexX, uint16& hexY, uint8& dir) const", focFUNCTION( BIND_CLASS Map_GetEntireCoordsDir ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetNearEntireCoords(int& entire, uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Map_GetNearEntireCoords ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool GetNearEntireCoords(int& entire, uint16& hexX, uint16& hexY, uint8& dir) const", focFUNCTION( BIND_CLASS Map_GetNearEntireCoordsDir ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsHexPassed(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_IsHexPassed ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool IsHexRaked(uint16 hexX, uint16 hexY) const", focFUNCTION( BIND_CLASS Map_IsHexRaked ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetText(uint16 hexX, uint16 hexY, uint color, string& text) const", focFUNCTION( BIND_CLASS Map_SetText ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTextMsg(uint16 hexX, uint16 hexY, uint color, uint16 textMsg, uint strNum) const", focFUNCTION( BIND_CLASS Map_SetTextMsg ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void SetTextMsg(uint16 hexX, uint16 hexY, uint color, uint16 textMsg, uint strNum, string& lexems) const", focFUNCTION( BIND_CLASS Map_SetTextMsgLex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void RunEffect(uint16 effectPid, uint16 hexX, uint16 hexY, uint16 radius) const", focFUNCTION( BIND_CLASS Map_RunEffect ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void RunFlyEffect(uint16 effectPid, Critter@+ fromCr, Critter@+ toCr, uint16 fromX, uint16 fromY, uint16 toX, uint16 toY) const", focFUNCTION( BIND_CLASS Map_RunFlyEffect ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool CheckPlaceForItem(uint16 hexX, uint16 hexY, uint16 pid) const", focFUNCTION( BIND_CLASS Map_CheckPlaceForItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void BlockHex(uint16 hexX, uint16 hexY, bool full)", focFUNCTION( BIND_CLASS Map_BlockHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void UnblockHex(uint16 hexX, uint16 hexY)", focFUNCTION( BIND_CLASS Map_UnblockHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void PlaySound(string& soundName) const", focFUNCTION( BIND_CLASS Map_PlaySound ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void PlaySound(string& soundName, uint16 hexX, uint16 hexY, uint radius) const", focFUNCTION( BIND_CLASS Map_PlaySoundRadius ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "bool Reload()", focFUNCTION( BIND_CLASS Map_Reload ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint16 GetWidth() const", focFUNCTION( BIND_CLASS Map_GetWidth ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "uint16 GetHeight() const", focFUNCTION( BIND_CLASS Map_GetHeight ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps) const", focFUNCTION( BIND_CLASS Map_MoveHexByDir ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Map_EventFinish ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop0()", focFUNCTION( BIND_CLASS Map_EventLoop0 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop1()", focFUNCTION( BIND_CLASS Map_EventLoop1 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop2()", focFUNCTION( BIND_CLASS Map_EventLoop2 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop3()", focFUNCTION( BIND_CLASS Map_EventLoop3 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventLoop4()", focFUNCTION( BIND_CLASS Map_EventLoop4 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventInCritter(Critter& cr)", focFUNCTION( BIND_CLASS Map_EventInCritter ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventOutCritter(Critter& cr)", focFUNCTION( BIND_CLASS Map_EventOutCritter ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventCritterDead(Critter& cr, Critter@+ killer)", focFUNCTION( BIND_CLASS Map_EventCritterDead ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventTurnBasedBegin()", focFUNCTION( BIND_CLASS Map_EventTurnBasedBegin ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventTurnBasedEnd()", focFUNCTION( BIND_CLASS Map_EventTurnBasedEnd ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Map", "void EventTurnBasedProcess(Critter& cr, bool beginTurn)", focFUNCTION( BIND_CLASS Map_EventTurnBasedProcess ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint Id", focOFFSET( Map, Data.MapId ) ) );
    // BIND_ASSERT( engine->RegisterObjectProperty( "Map", "bool CombatRunning", focOFFSET( Map, MapId ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint TurnBasedRound", focOFFSET( Map, TurnBasedRound ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint TurnBasedTurn", focOFFSET( Map, TurnBasedTurn ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "Map", "const uint TurnBasedWholeTurn", focOFFSET( Map, TurnBasedWholeTurn ) ) );

    /************************************************************************/
    /* Location                                                             */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Location_GetProtoId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "bool SetEvent(int eventType, string@+ funcName)", focFUNCTION( BIND_CLASS Location_SetEvent ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "uint GetMapCount() const", focFUNCTION( BIND_CLASS Location_GetMapCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "Map@+ GetMap(uint16 mapPid) const", focFUNCTION( BIND_CLASS Location_GetMap ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "Map@+ GetMapByIndex(uint index) const", focFUNCTION( BIND_CLASS Location_GetMapByIndex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "uint GetMaps(Map@[]@+ maps) const", focFUNCTION( BIND_CLASS Location_GetMaps ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "bool Reload()", focFUNCTION( BIND_CLASS Location_Reload ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "void Update()", focFUNCTION( BIND_CLASS Location_Update ), focCALL_CDECL_OBJFIRST ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "void EventFinish(bool deleted)", focFUNCTION( BIND_CLASS Location_EventFinish ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "Location", "bool EventEnter(Critter@[]& group, uint8 entrance)", focFUNCTION( BIND_CLASS Location_EventEnter ), focCALL_CDECL_OBJFIRST ) );

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

    /************************************************************************/
    /* Global                                                               */
    /************************************************************************/
    BIND_ASSERT( engine->RegisterGlobalFunction( "GameVar@+ GetGlobalVar(uint16 varId)", focFUNCTION( BIND_CLASS Global_GetGlobalVar ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "GameVar@+ GetLocalVar(uint16 varId, uint masterId)", focFUNCTION( BIND_CLASS Global_GetLocalVar ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "GameVar@+ GetUnicumVar(uint16 varId, uint masterId, uint slaveId)", focFUNCTION( BIND_CLASS Global_GetUnicumVar ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "Item@+ GetItem(uint itemId)", focFUNCTION( BIND_CLASS Global_GetItem ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItem(Item& item, uint count, Critter& toCr)", focFUNCTION( BIND_CLASS Global_MoveItemCr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItem(Item& item, uint count, Item& toCont, uint stackId)", focFUNCTION( BIND_CLASS Global_MoveItemCont ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItem(Item& item, uint count, Map& toMap, uint16 toHx, uint16 toHy)", focFUNCTION( BIND_CLASS Global_MoveItemMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItems(Item@[]& items, Critter& toCr)", focFUNCTION( BIND_CLASS Global_MoveItemsCr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItems(Item@[]& items, Item& toCont, uint stackId)", focFUNCTION( BIND_CLASS Global_MoveItemsCont ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveItems(Item@[]& items, Map& toMap, uint16 toHx, uint16 toHy)", focFUNCTION( BIND_CLASS Global_MoveItemsMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteItem(Item& item)", focFUNCTION( BIND_CLASS Global_DeleteItem ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteItems(Item@[]& items)", focFUNCTION( BIND_CLASS Global_DeleteItems ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteNpc(Critter& npc)", focFUNCTION( BIND_CLASS Global_DeleteNpc ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersDistantion(Critter& cr1, Critter& cr2)", focFUNCTION( BIND_CLASS Global_GetCrittersDistantion ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void RadioMessage(uint16 channel, string& text)", focFUNCTION( BIND_CLASS Global_RadioMessage ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void RadioMessageMsg(uint16 channel, uint16 textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_RadioMessageMsg ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void RadioMessageMsg(uint16 channel, uint16 textMsg, uint strNum, string@+ lexems)", focFUNCTION( BIND_CLASS Global_RadioMessageMsgLex ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateLocation(uint16 locPid, uint16 worldX, uint16 worldY, Critter@[]@+ critters)", focFUNCTION( BIND_CLASS Global_CreateLocation ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteLocation(uint locId)", focFUNCTION( BIND_CLASS Global_DeleteLocation ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetProtoCritter(uint16 protoId, int[]& data)", focFUNCTION( BIND_CLASS Global_GetProtoCritter ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "Critter@+ GetCritter(uint critterId)", focFUNCTION( BIND_CLASS Global_GetCritter ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "Critter@+ GetPlayer(string& name)", focFUNCTION( BIND_CLASS Global_GetPlayer ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPlayerId(string& name)", focFUNCTION( BIND_CLASS Global_GetPlayerId ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetPlayerName(uint playerId)", focFUNCTION( BIND_CLASS Global_GetPlayerName ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetGlobalMapCritters(uint16 worldX, uint16 worldY, uint radius, int findType, Critter@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetGlobalMapCritters ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventEmpty ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, uint value, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValue ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, int value, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValue ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, uint[]& values, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValues ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint CreateTimeEvent(uint beginSecond, string& funcName, int[]& values, bool save)", focFUNCTION( BIND_CLASS Global_CreateTimeEventValues ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool EraseTimeEvent(uint num)", focFUNCTION( BIND_CLASS Global_EraseTimeEvent ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetTimeEvent(uint num, uint& duration, uint[]@+ values)", focFUNCTION( BIND_CLASS Global_GetTimeEvent ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetTimeEvent(uint num, uint& duration, int[]@+ values)", focFUNCTION( BIND_CLASS Global_GetTimeEvent ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetTimeEvent(uint num, uint duration, uint[]@+ values)", focFUNCTION( BIND_CLASS Global_SetTimeEvent ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetTimeEvent(uint num, uint duration, int[]@+ values)", focFUNCTION( BIND_CLASS Global_SetTimeEvent ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int64[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int32[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int16[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int8[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint64[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint32[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint16[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint8[]& data)", focFUNCTION( BIND_CLASS Global_SetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int64[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int32[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int16[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, int8[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint64[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint32[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint16[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetAnyData(string& name, uint8[]& data, uint dataSize)", focFUNCTION( BIND_CLASS Global_SetAnyDataSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int64[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int32[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int16[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, int8[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint64[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint32[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint16[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetAnyData(string& name, uint8[]& data)", focFUNCTION( BIND_CLASS Global_GetAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsAnyData(string& name)", focFUNCTION( BIND_CLASS Global_IsAnyData ), focCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "bool AnyDataClass(?& storedClass, ?[]& array)", focFUNCTION( BIND_CLASS Global_AnyDataClass ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void EraseAnyData(string& name)", focFUNCTION( BIND_CLASS Global_EraseAnyData ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "Map@+ GetMap(uint mapId)", focFUNCTION( BIND_CLASS Global_GetMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "Map@+ GetMapByPid(uint16 mapPid, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMapByPid ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "Location@+ GetLocation(uint locId)", focFUNCTION( BIND_CLASS Global_GetLocation ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "Location@+ GetLocationByPid(uint16 locPid, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetLocationByPid ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetLocations(uint16 worldX, uint16 worldY, uint radius, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetLocations ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetVisibleLocations(uint16 worldX, uint16 worldY, uint radius, Critter@+ visibleBy, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetVisibleLocations ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetZoneLocationIds(uint16 zoneX, uint16 zoneY, uint zoneRadius, uint[]@+ locationIds)", focFUNCTION( BIND_CLASS Global_GetZoneLocationIds ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool RunDialog(Critter& player, Critter& npc, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogNpc ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool RunDialog(Critter& player, Critter& npc, uint dialogPack, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogNpcDlgPack ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool RunDialog(Critter& player, uint dialogPack, uint16 hexX, uint16 hexY, bool ignoreDistance)", focFUNCTION( BIND_CLASS Global_RunDialogHex ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int64 WorldItemCount(uint16 protoId)", focFUNCTION( BIND_CLASS Global_WorldItemCount ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetBestScore(int score, Critter@+ player, string& name)", focFUNCTION( BIND_CLASS Global_SetBestScore ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool AddTextListener(int sayType, string& firstStr, uint16 parameter, string& scriptName)", focFUNCTION( BIND_CLASS Global_AddTextListener ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void EraseTextListener(int sayType, string& firstStr, uint16 parameter)", focFUNCTION( BIND_CLASS Global_EraseTextListener ), focCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint8 ReverseDir(uint8 dir)", focFUNCTION( BIND_CLASS Global_ReverseDir ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "NpcPlane@ CreatePlane()", focFUNCTION( BIND_CLASS Global_CreatePlane ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetBagItems(uint bagId, uint16[]@+ pids, uint[]@+ minCounts, uint[]@+ maxCounts, int[]@+ slots)", focFUNCTION( BIND_CLASS Global_GetBagItems ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetChosenSendParameter(int index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetChosenSendParameter ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetSendParameter(int index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetSendParameter ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetSendParameter(int index, bool enabled, string@+ allowFunc)", focFUNCTION( BIND_CLASS Global_SetSendParameterFunc ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SwapCritters(Critter& cr1, Critter& cr2, bool withInventory, bool withVars)", focFUNCTION( BIND_CLASS Global_SwapCritters ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllItems(uint16 pid, Item@[]@+ items)", focFUNCTION( BIND_CLASS Global_GetAllItems ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllPlayers(Critter@[]@+ players)", focFUNCTION( BIND_CLASS Global_GetAllPlayers ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetRegisteredPlayers(uint[]& ids, string@[]& names)", focFUNCTION( BIND_CLASS Global_GetRegisteredPlayers ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllNpc(uint16 pid, Critter@[]@+ npc)", focFUNCTION( BIND_CLASS Global_GetAllNpc ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllMaps(uint16 pid, Map@[]@+ maps)", focFUNCTION( BIND_CLASS Global_GetAllMaps ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAllLocations(uint16 pid, Location@[]@+ locations)", focFUNCTION( BIND_CLASS Global_GetAllLocations ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetScriptId(string& scriptName, string& funcDeclaration)", focFUNCTION( BIND_CLASS Global_GetScriptId ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetScriptName(uint scriptId)", focFUNCTION( BIND_CLASS Global_GetScriptName ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int8[]@ GetItemDataMask(int maskType)", focFUNCTION( BIND_CLASS Global_GetItemDataMask ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetItemDataMask(int maskType, int8[]& mask)", focFUNCTION( BIND_CLASS Global_SetItemDataMask ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadImage(uint index, string@+ imageName, uint imageDepth, int pathType)", focFUNCTION( BIND_CLASS Global_LoadImage ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetImageColor(uint index, uint x, uint y)", focFUNCTION( BIND_CLASS Global_GetImageColor ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void Synchronize()", focFUNCTION( BIND_CLASS Global_Synchronize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void Resynchronize()", focFUNCTION( BIND_CLASS Global_Resynchronize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetParameterDialogGetBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterDialogGetBehaviour ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "CraftItem@ GetCraftItem(uint num)", focFUNCTION( BIND_CLASS Global_GetCraftItem ), focCALL_CDECL ) );
    #endif

    #ifdef BIND_CLIENT
    BIND_ASSERT( engine->RegisterObjectType( "CritterCl", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "CritterCl", asBEHAVE_ADDREF, "void f()", focMETHOD( CritterCl, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "CritterCl", asBEHAVE_RELEASE, "void f()", focMETHOD( CritterCl, Release ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectType( "ItemCl", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "ItemCl", asBEHAVE_ADDREF, "void f()", focMETHOD( Item, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "ItemCl", asBEHAVE_RELEASE, "void f()", focMETHOD( Item, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsChosen() const", focFUNCTION( BIND_CLASS Crit_IsChosen ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsPlayer() const", focFUNCTION( BIND_CLASS Crit_IsPlayer ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsNpc() const", focFUNCTION( BIND_CLASS Crit_IsNpc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsLife() const", focFUNCTION( BIND_CLASS Crit_IsLife ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsKnockout() const", focFUNCTION( BIND_CLASS Crit_IsKnockout ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsDead() const", focFUNCTION( BIND_CLASS Crit_IsDead ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsFree() const", focFUNCTION( BIND_CLASS Crit_IsFree ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsBusy() const", focFUNCTION( BIND_CLASS Crit_IsBusy ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsAnim3d() const", focFUNCTION( BIND_CLASS Crit_IsAnim3d ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsAnimAviable(uint anim1, uint anim2) const", focFUNCTION( BIND_CLASS Crit_IsAnimAviable ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsAnimPlaying() const", focFUNCTION( BIND_CLASS Crit_IsAnimPlaying ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetAnim1() const", focFUNCTION( BIND_CLASS Crit_GetAnim1 ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void Animate(uint anim1, uint anim2)", focFUNCTION( BIND_CLASS Crit_Animate ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void Animate(uint anim1, uint anim2, ItemCl@+ item)", focFUNCTION( BIND_CLASS Crit_AnimateEx ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void ClearAnim()", focFUNCTION( BIND_CLASS Crit_ClearAnim ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void Wait(uint ms)", focFUNCTION( BIND_CLASS Crit_Wait ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint ItemsCount() const", focFUNCTION( BIND_CLASS Crit_ItemsCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint ItemsWeight() const", focFUNCTION( BIND_CLASS Crit_ItemsWeight ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint ItemsVolume() const", focFUNCTION( BIND_CLASS Crit_ItemsVolume ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint CountItem(uint16 protoId) const", focFUNCTION( BIND_CLASS Crit_CountItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "ItemCl@+ GetItem(uint16 protoId, int slot) const", focFUNCTION( BIND_CLASS Crit_GetItem ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetItems(int slot, ItemCl@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItems ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetItemsByType(int type, ItemCl@[]@+ items) const", focFUNCTION( BIND_CLASS Crit_GetItemsByType ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "ProtoItem@+ GetSlotProto(int slot, uint8& mode) const", focFUNCTION( BIND_CLASS Crit_GetSlotProto ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void SetVisible(bool visible)", focFUNCTION( BIND_CLASS Crit_SetVisible ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool GetVisible() const", focFUNCTION( BIND_CLASS Crit_GetVisible ), focCALL_CDECL_OBJFIRST ) );
// BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsMyTurn() const", focFUNCTION( BIND_CLASS Crit_CheckKey ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "void set_ContourColor(uint value)", focFUNCTION( BIND_CLASS Crit_set_ContourColor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint get_ContourColor() const", focFUNCTION( BIND_CLASS Crit_get_ContourColor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "uint GetMultihex() const", focFUNCTION( BIND_CLASS Crit_GetMultihex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "CritterCl", "bool IsTurnBasedTurn() const", focFUNCTION( BIND_CLASS Crit_IsTurnBasedTurn ), focCALL_CDECL_OBJFIRST ) );

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

    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "bool IsStackable() const", focFUNCTION( BIND_CLASS Item_IsStackable ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "bool IsDeteriorable() const", focFUNCTION( BIND_CLASS Item_IsDeteriorable ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint GetScriptId() const", focFUNCTION( BIND_CLASS Item_GetScriptId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint8 GetType() const", focFUNCTION( BIND_CLASS Item_GetType ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint16 GetProtoId() const", focFUNCTION( BIND_CLASS Item_GetProtoId ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "uint GetCount() const", focFUNCTION( BIND_CLASS Item_GetCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "bool GetMapPosition(uint16& hexX, uint16& hexY) const", focFUNCTION( BIND_CLASS Item_GetMapPosition ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "ItemCl", "void Animate(uint8 fromFrame, uint8 toFrame)", focFUNCTION( BIND_CLASS Item_Animate ), focCALL_CDECL_OBJFIRST ) );
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

    BIND_ASSERT( engine->RegisterGlobalFunction( "CritterCl@+ GetChosen()", focFUNCTION( BIND_CLASS Global_GetChosen ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetChosenActions(uint[]@+ actions)", focFUNCTION( BIND_CLASS Global_GetChosenActions ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetChosenActions(uint[]@+ actions)", focFUNCTION( BIND_CLASS Global_SetChosenActions ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "ItemCl@+ GetItem(uint itemId)", focFUNCTION( BIND_CLASS Global_GetItem ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersDistantion(CritterCl& cr1, CritterCl& cr2)", focFUNCTION( BIND_CLASS Global_GetCrittersDistantion ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "CritterCl@+ GetCritter(uint critterId)", focFUNCTION( BIND_CLASS Global_GetCritter ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersHex(uint16 hexX, uint16 hexY, uint radius, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCritters ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCritters(uint16 pid, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCrittersByPids ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, CritterCl@[]@+ critters)", focFUNCTION( BIND_CLASS Global_GetCrittersInPath ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCrittersPath(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, float angle, uint dist, int findType, CritterCl@[]@+ critters, uint16& preBlockHx, uint16& preBlockHy, uint16& blockHx, uint16& blockHy)", focFUNCTION( BIND_CLASS Global_GetCrittersInPathBlock ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist)", focFUNCTION( BIND_CLASS Global_GetHexInPath ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthHex ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPathLength(CritterCl& cr, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthCr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void FlushScreen(uint fromColor, uint toColor, uint timeMs)", focFUNCTION( BIND_CLASS Global_FlushScreen ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void QuakeScreen(uint noise, uint timeMs)", focFUNCTION( BIND_CLASS Global_QuakeScreen ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool PlaySound(string& soundName)", focFUNCTION( BIND_CLASS Global_PlaySound ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool PlaySound(uint8 soundType, uint8 soundTypeExt, uint8 soundId, uint8 soundIdExt)", focFUNCTION( BIND_CLASS Global_PlaySoundType ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool PlayMusic(string& musicName, uint pos, uint repeat)", focFUNCTION( BIND_CLASS Global_PlayMusic ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void PlayVideo(string& videoName, bool canStop)", focFUNCTION( BIND_CLASS Global_PlayVideo ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsTurnBased()", focFUNCTION( BIND_CLASS Global_IsTurnBased ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetTurnBasedTime()", focFUNCTION( BIND_CLASS Global_GetTurnBasedTime ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint16 GetCurrentMapPid()", focFUNCTION( BIND_CLASS Global_GetCurrentMapPid ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMessageFilters(int[]@+ filters)", focFUNCTION( BIND_CLASS Global_GetMessageFilters ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetMessageFilters(int[]@+ filters)", focFUNCTION( BIND_CLASS Global_SetMessageFilters ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(string& text)", focFUNCTION( BIND_CLASS Global_Message ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(string& text, int type)", focFUNCTION( BIND_CLASS Global_MessageType ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_MessageMsg ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(int textMsg, uint strNum, int type)", focFUNCTION( BIND_CLASS Global_MessageMsgType ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MapMessage(string& text, uint16 hx, uint16 hy, uint timeMs, uint color, bool fade, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_MapMessage ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMsgStrSkip ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumUpper(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumUpper ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumLower(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumLower ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrCount(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrCount ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_IsMsgStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, const string& str)", focFUNCTION( BIND_CLASS Global_ReplaceTextStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, int i)", focFUNCTION( BIND_CLASS Global_ReplaceTextInt ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ FormatTags(const string& text, const string@+ lexems)", focFUNCTION( BIND_CLASS Global_FormatTags ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSomeValue(int var)", focFUNCTION( BIND_CLASS Global_GetSomeValue ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveScreen(uint16 hexX, uint16 hexY, uint speed)", focFUNCTION( BIND_CLASS Global_MoveScreen ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void LockScreenScroll(CritterCl@+ cr)", focFUNCTION( BIND_CLASS Global_LockScreenScroll ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetFog(uint16 zoneX, uint16 zoneY)", focFUNCTION( BIND_CLASS Global_GetFog ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void RefreshItemsCollection(int collection)", focFUNCTION( BIND_CLASS Global_RefreshItemsCollection ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetScroll(int scrollElement)", focFUNCTION( BIND_CLASS Global_GetScroll ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetScroll(int scrollElement, int value)", focFUNCTION( BIND_CLASS Global_SetScroll ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetDayTime(uint dayPart)", focFUNCTION( BIND_CLASS Global_GetDayTime ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b)", focFUNCTION( BIND_CLASS Global_GetDayColor ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void RunServerScript(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Global_RunServerScript ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void RunServerScriptUnsafe(string& funcName, int p0, int p1, int p2, string@+ p3, int[]@+ p4)", focFUNCTION( BIND_CLASS Global_RunServerScriptUnsafe ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(string& name, int pathIndex)", focFUNCTION( BIND_CLASS Global_LoadSprite ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(uint nameHash, uint8 dir)", focFUNCTION( BIND_CLASS Global_LoadSpriteHash ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteWidth(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteWidth ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteHeight(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteHeight ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetSpriteCount(uint sprId)", focFUNCTION( BIND_CLASS Global_GetSpriteCount ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetTextInfo(string& text, int w, int h, int font, int flags, int& tw, int& th, int& lines)", focFUNCTION( BIND_CLASS Global_GetTextInfo ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color)", focFUNCTION( BIND_CLASS Global_DrawSprite ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteOffs ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpriteSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteSizeOffs ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSpritePattern(uint sprId, int sprIndex, int x, int y, int w, int h, int sprWidth, int sprHeight, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpritePattern ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawText(string& text, int x, int y, int w, int h, uint color, int font, int flags)", focFUNCTION( BIND_CLASS Global_DrawText ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawPrimitive(int primitiveType, int[]& data)", focFUNCTION( BIND_CLASS Global_DrawPrimitive ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawMapSprite(uint16 hx, uint16 hy, uint16 effectPid, uint sprId, int sprIndex, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_DrawMapSprite ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter2d(uint crType, uint anim1, uint anim2, uint8 dir, int l, int t, int r, int b, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter2d ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter3d(uint instance, uint crType, uint anim1, uint anim2, int[]@+ layers, float[]@+ position, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter3d ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void ShowScreen(int screen, int p0, int p1, int p2)", focFUNCTION( BIND_CLASS Global_ShowScreen ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void HideScreen(int screen, int p0, int p1, int p2)", focFUNCTION( BIND_CLASS Global_HideScreen ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetHardcodedScreenPos(int screen, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHardcodedScreenPos ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawHardcodedScreen(int screen)", focFUNCTION( BIND_CLASS Global_DrawHardcodedScreen ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetHexPos(uint16 hx, uint16 hy, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHexPos ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetMonitorHex(int x, int y, uint16& hx, uint16& hy, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorHex ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "ItemCl@+ GetMonitorItem(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorItem ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "CritterCl@+ GetMonitorCritter(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorCritter ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint16 GetMapWidth()", focFUNCTION( BIND_CLASS Global_GetMapWidth ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint16 GetMapHeight()", focFUNCTION( BIND_CLASS Global_GetMapHeight ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetCurrentCursor()", focFUNCTION( BIND_CLASS Global_GetCurrentCursor ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetLastCursor()", focFUNCTION( BIND_CLASS Global_GetLastCursor ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void ChangeCursor(int cursor)", focFUNCTION( BIND_CLASS Global_ChangeCursor ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps)", focFUNCTION( BIND_CLASS Global_MoveHexByDir ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool AppendIfaceIni(string& iniName)", focFUNCTION( BIND_CLASS Global_AppendIfaceIni ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetIfaceIniStr(string& key)", focFUNCTION( BIND_CLASS Global_GetIfaceIniStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool Load3dFile(string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_Load3dFile ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void WaitPing()", focFUNCTION( BIND_CLASS Global_WaitPing ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadFont(int font, string& fontFileName)", focFUNCTION( BIND_CLASS Global_LoadFont ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetDefaultFont(int font, uint color)", focFUNCTION( BIND_CLASS Global_SetDefaultFont ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetEffect(int effectType, int effectSubtype, string@+ effectName, string@+ effectDefines = null)", focFUNCTION( BIND_CLASS Global_SetEffect ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void RefreshMap(bool onlyTiles, bool onlyRoof, bool onlyLight)", focFUNCTION( BIND_CLASS Global_RefreshMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MouseClick(int x, int y, int button, int cursor)", focFUNCTION( BIND_CLASS Global_MouseClick ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void KeyboardPress(uint8 key1, uint8 key2, string@+ key1Text = null, string@+ key2Text = null)", focFUNCTION( BIND_CLASS Global_KeyboardPress ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetRainAnimation(string@+ fallAnimName, string@+ dropAnimName)", focFUNCTION( BIND_CLASS Global_SetRainAnimation ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SaveScreenshot()", focFUNCTION( BIND_CLASS Global_SaveScreenshot ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SaveLogFile()", focFUNCTION( BIND_CLASS Global_SaveLogFile ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ConsoleActive", &BIND_CLASS ConsoleActive ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __GmapActive", &BIND_CLASS GmapActive ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __GmapWait", &BIND_CLASS GmapWait ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "float __GmapZoom", &BIND_CLASS GmapZoom ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapOffsetX", &BIND_CLASS GmapOffsetX ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapOffsetY", &BIND_CLASS GmapOffsetY ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupCurX", &BIND_CLASS GmapGroupCurX ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupCurY", &BIND_CLASS GmapGroupCurY ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupToX", &BIND_CLASS GmapGroupToX ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "int __GmapGroupToY", &BIND_CLASS GmapGroupToY ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "float __GmapGroupSpeed", &BIND_CLASS GmapGroupSpeed ) );
    #endif

    #if defined (BIND_CLIENT) || defined (BIND_SERVER)
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetFullSecond(uint16 year, uint16 month, uint16 day, uint16 hour, uint16 minute, uint16 second)", focFUNCTION( BIND_CLASS Global_GetFullSecond ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetTime(uint16& year, uint16& month, uint16& day, uint16& dayOfWeek, uint16& hour, uint16& minute, uint16& second, uint16& milliseconds)", focFUNCTION( BIND_CLASS Global_GetTime ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetGameTime(uint fullSecond, uint16& year, uint16& month, uint16& day, uint16& dayOfWeek, uint16& hour, uint16& minute, uint16& second)", focFUNCTION( BIND_CLASS Global_GetGameTime ), focCALL_CDECL ) );
// BIND_ASSERT( engine->RegisterGlobalFunction( "void GetVersion(uint& server, uint& client, uint& net)", focFUNCTION( BIND_CLASS Global_GetVersion ), focCALL_CDECL ) );
// BIND_ASSERT( engine->RegisterGlobalFunction( "void Sort(int[]& array, string& func)", focFUNCTION( BIND_CLASS Global_Sort ), focCALL_CDECL ) );
// BIND_ASSERT( engine->RegisterGlobalFunction( "void Sort(uint[]& array, string& func)", focFUNCTION( BIND_CLASS Global_Sort ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetParameterGetBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterGetBehaviour ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SetParameterChangeBehaviour(uint index, string& funcName)", focFUNCTION( BIND_CLASS Global_SetParameterChangeBehaviour ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetRegistrationParameter(uint index, bool enabled)", focFUNCTION( BIND_CLASS Global_SetRegistrationParam ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanWalk(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanWalk ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRun(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRun ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRotate(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRotate ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanAim(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanAim ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanArmor(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanArmor ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterAnim1(uint crType, uint anim1)", focFUNCTION( BIND_CLASS Global_IsCritterAnim1 ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetCritterAnimType(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAnimType ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCritterAlias(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAlias ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterTypeName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterTypeName ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterSoundName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterSoundName ), focCALL_CDECL ) );

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
    #endif

    #ifdef BIND_MAPPER
    BIND_ASSERT( engine->RegisterObjectType( "MapperObject", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperObject", asBEHAVE_ADDREF, "void f()", focMETHOD( MapObject, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperObject", asBEHAVE_RELEASE, "void f()", focMETHOD( MapObject, Release ), focCALL_THISCALL ) );

    BIND_ASSERT( engine->RegisterObjectType( "MapperMap", 0, asOBJ_REF ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperMap", asBEHAVE_ADDREF, "void f()", focMETHOD( ProtoMap, AddRef ), focCALL_THISCALL ) );
    BIND_ASSERT( engine->RegisterObjectBehaviour( "MapperMap", asBEHAVE_RELEASE, "void f()", focMETHOD( ProtoMap, Release ), focCALL_THISCALL ) );

// MapperObject
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void Update() const", focFUNCTION( BIND_CLASS MapperObject_Update ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "MapperObject@+ AddChild(uint16 pid)", focFUNCTION( BIND_CLASS MapperObject_AddChild ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "uint GetChilds(MapperObject@[]@+ objects) const", focFUNCTION( BIND_CLASS MapperObject_GetChilds ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_ScriptName() const", focFUNCTION( BIND_CLASS MapperObject_get_ScriptName ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_ScriptName(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_ScriptName ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_FuncName() const", focFUNCTION( BIND_CLASS MapperObject_get_FuncName ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_FuncName(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_FuncName ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "uint8 get_Critter_Cond() const", focFUNCTION( BIND_CLASS MapperObject_get_Critter_Cond ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_Critter_Cond(uint8 value)", focFUNCTION( BIND_CLASS MapperObject_set_Critter_Cond ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void MoveToHex(uint16 hexX, uint16 hexY)", focFUNCTION( BIND_CLASS MapperObject_MoveToHex ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void MoveToHexOffset(int x, int y)", focFUNCTION( BIND_CLASS MapperObject_MoveToHexOffset ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void MoveToDir(uint8 dir)", focFUNCTION( BIND_CLASS MapperObject_MoveToDir ), focCALL_CDECL_OBJFIRST ) );

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

// Item/critter shared parameters
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int16 OffsetX", focOFFSET( MapObject, MItem.OffsetX ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "int16 OffsetY", focOFFSET( MapObject, MItem.OffsetY ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 AnimStayBegin", focOFFSET( MapObject, MItem.AnimStayBegin ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint8 AnimStayEnd", focOFFSET( MapObject, MItem.AnimStayEnd ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperObject", "uint16 AnimWait", focOFFSET( MapObject, MItem.AnimWait ) ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_PicMap() const", focFUNCTION( BIND_CLASS MapperObject_get_PicMap ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_PicMap(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_PicMap ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "string@ get_PicInv() const", focFUNCTION( BIND_CLASS MapperObject_get_PicInv ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperObject", "void set_PicInv(const string& name)", focFUNCTION( BIND_CLASS MapperObject_set_PicInv ), focCALL_CDECL_OBJFIRST ) );
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

// MapperMap
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "MapperObject@+ AddObject(uint16 hexX, uint16 hexY, int mapObjType, uint16 pid)", focFUNCTION( BIND_CLASS MapperMap_AddObject ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "MapperObject@+ GetObject(uint16 hexX, uint16 hexY, int mapObjType, uint16 pid, uint skip) const", focFUNCTION( BIND_CLASS MapperMap_GetObject ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetObjects(uint16 hexX, uint16 hexY, uint radius, int mapObjType, uint16 pid, MapperObject@[]@+ objects) const", focFUNCTION( BIND_CLASS MapperMap_GetObjects ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void UpdateObjects() const", focFUNCTION( BIND_CLASS MapperMap_UpdateObjects ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void Resize(uint16 width, uint16 height)", focFUNCTION( BIND_CLASS MapperMap_Resize ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetTilesCount(uint16 hexX, uint16 hexY, bool roof) const", focFUNCTION( BIND_CLASS MapperMap_GetTilesCount ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void DeleteTile(uint16 hexX, uint16 hexY, bool roof, uint index)", focFUNCTION( BIND_CLASS MapperMap_DeleteTile ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetTile(uint16 hexX, uint16 hexY, bool roof, uint index) const", focFUNCTION( BIND_CLASS MapperMap_GetTileHash ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void AddTile(uint16 hexX, uint16 hexY, int offsX, int offsY, int layer, bool roof, uint picHash)", focFUNCTION( BIND_CLASS MapperMap_AddTileHash ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "string@ GetTileName(uint16 hexX, uint16 hexY, bool roof, uint index) const", focFUNCTION( BIND_CLASS MapperMap_GetTileName ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void AddTileName(uint16 hexX, uint16 hexY, int offsX, int offsY, int layer, bool roof, string@+ picName)", focFUNCTION( BIND_CLASS MapperMap_AddTileName ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "uint GetDayTime(uint dayPart) const", focFUNCTION( BIND_CLASS MapperMap_GetDayTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void SetDayTime(uint dayPart, uint time)", focFUNCTION( BIND_CLASS MapperMap_SetDayTime ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void GetDayColor(uint dayPart, uint8& r, uint8& g, uint8& b) const", focFUNCTION( BIND_CLASS MapperMap_GetDayColor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void SetDayColor(uint dayPart, uint8 r, uint8 g, uint8 b)", focFUNCTION( BIND_CLASS MapperMap_SetDayColor ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const uint16 Width", focOFFSET( ProtoMap, Header.MaxHexX ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const uint16 Height", focOFFSET( ProtoMap, Header.MaxHexY ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const int WorkHexX", focOFFSET( ProtoMap, Header.WorkHexX ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "const int WorkHexY", focOFFSET( ProtoMap, Header.WorkHexY ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "int Time", focOFFSET( ProtoMap, Header.Time ) ) );
    BIND_ASSERT( engine->RegisterObjectProperty( "MapperMap", "bool NoLogOut", focOFFSET( ProtoMap, Header.NoLogOut ) ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "string@ get_ScriptModule() const", focFUNCTION( BIND_CLASS MapperMap_get_ScriptModule ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void set_ScriptModule(const string& name)", focFUNCTION( BIND_CLASS MapperMap_set_ScriptModule ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "string@ get_ScriptFunc() const", focFUNCTION( BIND_CLASS MapperMap_get_ScriptFunc ), focCALL_CDECL_OBJFIRST ) );
    BIND_ASSERT( engine->RegisterObjectMethod( "MapperMap", "void set_ScriptFunc(const string& name)", focFUNCTION( BIND_CLASS MapperMap_set_ScriptFunc ), focCALL_CDECL_OBJFIRST ) );

// Global
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetDefaultCritterParam(uint index, int param)", focFUNCTION( BIND_CLASS Global_SetDefaultCritterParam ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "MapperMap@+ LoadMap(string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_LoadMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void UnloadMap(MapperMap@+ map)", focFUNCTION( BIND_CLASS Global_UnloadMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool SaveMap(MapperMap@+ map, string& fileName, int pathType)", focFUNCTION( BIND_CLASS Global_SaveMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool ShowMap(MapperMap@+ map)", focFUNCTION( BIND_CLASS Global_ShowMap ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetLoadedMaps(MapperMap@[]@+ maps)", focFUNCTION( BIND_CLASS Global_GetLoadedMaps ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMapFileNames(string@+ dir, string@[]@+ names)", focFUNCTION( BIND_CLASS Global_GetMapFileNames ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteObject(MapperObject@+ obj)", focFUNCTION( BIND_CLASS Global_DeleteObject ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DeleteObjects(MapperObject@[]& objects)", focFUNCTION( BIND_CLASS Global_DeleteObjects ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SelectObject(MapperObject@+ obj, bool set)", focFUNCTION( BIND_CLASS Global_SelectObject ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SelectObjects(MapperObject@[]& objects, bool set)", focFUNCTION( BIND_CLASS Global_SelectObjects ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "MapperObject@+ GetSelectedObject()", focFUNCTION( BIND_CLASS Global_GetSelectedObject ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetSelectedObjects(MapperObject@[]@+ objects)", focFUNCTION( BIND_CLASS Global_GetSelectedObjects ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "uint TabGetTileDirs(int tab, string@[]@+ dirNames, bool[]@+ includeSubdirs)", focFUNCTION( BIND_CLASS Global_TabGetTileDirs ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint TabGetItemPids(int tab, string@+ subTab, uint16[]@+ itemPids)", focFUNCTION( BIND_CLASS Global_TabGetItemPids ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint TabGetCritterPids(int tab, string@+ subTab, uint16[]@+ critterPids)", focFUNCTION( BIND_CLASS Global_TabGetCritterPids ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetTileDirs(int tab, string@[]@+ dirNames, bool[]@+ includeSubdirs)", focFUNCTION( BIND_CLASS Global_TabSetTileDirs ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetItemPids(int tab, string@+ subTab, uint16[]@+ itemPids)", focFUNCTION( BIND_CLASS Global_TabSetItemPids ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetCritterPids(int tab, string@+ subTab, uint16[]@+ critterPids)", focFUNCTION( BIND_CLASS Global_TabSetCritterPids ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void TabDelete(int tab)", focFUNCTION( BIND_CLASS Global_TabDelete ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSelect(int tab, string@+ subTab, bool show = false)", focFUNCTION( BIND_CLASS Global_TabSelect ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void TabSetName(int tab, string@+ tabName)", focFUNCTION( BIND_CLASS Global_TabSetName ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanWalk(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanWalk ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRun(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRun ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanRotate(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanRotate ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanAim(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanAim ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterCanArmor(uint crType)", focFUNCTION( BIND_CLASS Global_IsCritterCanArmor ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsCritterAnim1(uint crType, uint anim1)", focFUNCTION( BIND_CLASS Global_IsCritterAnim1 ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetCritterAnimType(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAnimType ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetCritterAlias(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterAlias ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterTypeName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterTypeName ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetCritterSoundName(uint crType)", focFUNCTION( BIND_CLASS Global_GetCritterSoundName ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetHexCoord(uint16 fromHx, uint16 fromHy, uint16& toHx, uint16& toHy, float angle, uint dist)", focFUNCTION( BIND_CLASS Global_GetHexInPath ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetPathLength(uint16 fromHx, uint16 fromHy, uint16 toHx, uint16 toHy, uint cut)", focFUNCTION( BIND_CLASS Global_GetPathLengthHex ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(string& text)", focFUNCTION( BIND_CLASS Global_Message ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void Message(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_MessageMsg ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "void MapMessage(string& text, uint16 hx, uint16 hy, uint timeMs, uint color, bool fade, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_MapMessage ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetMsgStr(int textMsg, uint strNum, uint skipCount)", focFUNCTION( BIND_CLASS Global_GetMsgStrSkip ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumUpper(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumUpper ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrNumLower(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrNumLower ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetMsgStrCount(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_GetMsgStrCount ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool IsMsgStr(int textMsg, uint strNum)", focFUNCTION( BIND_CLASS Global_IsMsgStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, const string& str)", focFUNCTION( BIND_CLASS Global_ReplaceTextStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ ReplaceText(const string& text, const string& replace, int i)", focFUNCTION( BIND_CLASS Global_ReplaceTextInt ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveScreen(uint16 hexX, uint16 hexY, uint speed)", focFUNCTION( BIND_CLASS Global_MoveScreen ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(string& name, int pathIndex)", focFUNCTION( BIND_CLASS Global_LoadSprite ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint LoadSprite(uint nameHash, uint8 dir)", focFUNCTION( BIND_CLASS Global_LoadSpriteHash ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteWidth(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteWidth ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "int GetSpriteHeight(uint sprId, int sprIndex)", focFUNCTION( BIND_CLASS Global_GetSpriteHeight ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetSpriteCount(uint sprId)", focFUNCTION( BIND_CLASS Global_GetSpriteCount ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void GetTextInfo(string& text, int w, int h, int font, int flags, int& tw, int& th, int& lines)", focFUNCTION( BIND_CLASS Global_GetTextInfo ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color)", focFUNCTION( BIND_CLASS Global_DrawSprite ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteOffs ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpriteSize ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSprite(uint sprId, int sprIndex, int x, int y, int w, int h, bool scratch, bool center, uint color, bool applyOffsets)", focFUNCTION( BIND_CLASS Global_DrawSpriteSizeOffs ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawSpritePattern(uint sprId, int sprIndex, int x, int y, int w, int h, int sprWidth, int sprHeight, uint color)", focFUNCTION( BIND_CLASS Global_DrawSpritePattern ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawText(string& text, int x, int y, int w, int h, uint color, int font, int flags)", focFUNCTION( BIND_CLASS Global_DrawText ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawPrimitive(int primitiveType, int[]& data)", focFUNCTION( BIND_CLASS Global_DrawPrimitive ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawMapSprite(uint16 hx, uint16 hy, uint16 effectPid, uint sprId, int sprIndex, int offsX, int offsY)", focFUNCTION( BIND_CLASS Global_DrawMapSprite ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter2d(uint crType, uint anim1, uint anim2, uint8 dir, int l, int t, int r, int b, bool scratch, bool center, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter2d ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void DrawCritter3d(uint instance, uint crType, uint anim1, uint anim2, int[]@+ layers, float[]@+ position, uint color)", focFUNCTION( BIND_CLASS Global_DrawCritter3d ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetHexPos(uint16 hx, uint16 hy, int& x, int& y)", focFUNCTION( BIND_CLASS Global_GetHexPos ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool GetMonitorHex(int x, int y, uint16& hx, uint16& hy, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorHex ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "MapperObject@+ GetMonitorObject(int x, int y, bool ignoreInterface = false)", focFUNCTION( BIND_CLASS Global_GetMonitorObject ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MoveHexByDir(uint16& hexX, uint16& hexY, uint8 dir, uint steps)", focFUNCTION( BIND_CLASS Global_MoveHexByDir ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetIfaceIniStr(string& key)", focFUNCTION( BIND_CLASS Global_GetIfaceIniStr ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadFont(int font, string& fontFileName)", focFUNCTION( BIND_CLASS Global_LoadFont ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetDefaultFont(int font, uint color)", focFUNCTION( BIND_CLASS Global_SetDefaultFont ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void MouseClick(int x, int y, int button, int cursor)", focFUNCTION( BIND_CLASS Global_MouseClick ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void KeyboardPress(uint8 key1, uint8 key2, string@+ key1Text = null, string@+ key2Text = null)", focFUNCTION( BIND_CLASS Global_KeyboardPress ), focCALL_CDECL ) );
    BIND_ASSERT( engine->RegisterGlobalFunction( "void SetRainAnimation(string@+ fallAnimName, string@+ dropAnimName)", focFUNCTION( BIND_CLASS Global_SetRainAnimation ), focCALL_CDECL ) );

    BIND_ASSERT( engine->RegisterGlobalProperty( "string __ClientPath", &GameOpt.ClientPath ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "string __ServerPath", &GameOpt.ServerPath ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowCorners", &GameOpt.ShowCorners ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowSpriteCuts", &GameOpt.ShowSpriteCuts ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __ShowDrawOrder", &GameOpt.ShowDrawOrder ) );
    BIND_ASSERT( engine->RegisterGlobalProperty( "bool __SplitTilesCollection", &GameOpt.SplitTilesCollection ) );
    #endif

    #if defined (BIND_CLIENT) || defined (BIND_MAPPER)
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
    #endif

    #pragma TODO("Re-implement")
    // BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MapHexagonal", &GameOpt.MapHexagonal ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapHexWidth", &GameOpt.MapHexWidth ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapHexHeight", &GameOpt.MapHexHeight ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapHexLineHeight", &GameOpt.MapHexLineHeight ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapTileOffsX", &GameOpt.MapTileOffsX ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapTileOffsY", &GameOpt.MapTileOffsY ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapRoofOffsX", &GameOpt.MapRoofOffsX ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapRoofOffsY", &GameOpt.MapRoofOffsY ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "int __MapRoofSkipSize", &GameOpt.MapRoofSkipSize ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "float __MapCameraAngle", &GameOpt.MapCameraAngle ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "bool __MapSmoothPath", &GameOpt.MapSmoothPath ) );
    // BIND_ASSERT( engine->RegisterGlobalProperty( "string __MapDataPrefix", &GameOpt.MapDataPrefix ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "void Log(string& text)", asFUNCTION( BIND_CLASS Global_Log ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetLastError()", asFUNCTION( BIND_CLASS Global_GetLastError ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "int Random(int minimum, int maximum)", asFUNCTION( Random ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "ProtoItem@+ GetProtoItem(uint16 protoId)", asFUNCTION( BIND_CLASS Global_GetProtoItem ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "bool StrToInt(string@+ text, int& result)", asFUNCTION( BIND_CLASS Global_StrToInt ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "bool StrToFloat(string@+ text, float& result)", asFUNCTION( BIND_CLASS Global_StrToFloat ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetDistantion(uint16 hexX1, uint16 hexY1, uint16 hexX2, uint16 hexY2)", asFUNCTION( BIND_CLASS Global_GetDistantion ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint8 GetDirection(uint16 fromHexX, uint16 fromHexY, uint16 toHexX, uint16 toHexY)", asFUNCTION( BIND_CLASS Global_GetDirection ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint8 GetOffsetDir(uint16 fromHexX, uint16 fromHexY, uint16 toHexX, uint16 toHexY, float offset)", asFUNCTION( BIND_CLASS Global_GetOffsetDir ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetTick()", asFUNCTION( BIND_CLASS Global_GetTick ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetAngelScriptProperty(int property)", asFUNCTION( BIND_CLASS Global_GetAngelScriptProperty ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "void SetAngelScriptProperty(int property, uint value)", asFUNCTION( BIND_CLASS Global_SetAngelScriptProperty ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint GetStrHash(string@+ str)", asFUNCTION( BIND_CLASS Global_GetStrHash ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadDataFile(string& dataFileName)", asFUNCTION( BIND_CLASS Global_LoadDataFile ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "int GetConstantValue(int constCollection, string@+ name)", asFUNCTION( BIND_CLASS Global_GetConstantValue ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "string@ GetConstantName(int constCollection, int value)", asFUNCTION( BIND_CLASS Global_GetConstantName ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "void AddConstant(int constCollection, string@+ name, int value)", asFUNCTION( BIND_CLASS Global_AddConstant ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "bool LoadConstants(int constCollection, string@+ fileName, int pathType)", asFUNCTION( BIND_CLASS Global_LoadConstants ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "void AllowSlot(uint8 index, string& slotName)", asFUNCTION( BIND_CLASS Global_AllowSlot ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "uint DecodeUTF8(const string& text, uint& length)", asFUNCTION( BIND_CLASS Global_DecodeUTF8 ), asCALL_CDECL ) );
    // BIND_ASSERT( engine->RegisterGlobalFunction( "string@ EncodeUTF8(uint ucs)", asFUNCTION( BIND_CLASS Global_EncodeUTF8 ), asCALL_CDECL ) );

    /************************************************************************/
/*                                                                      */
/************************************************************************/

    return true;
}
