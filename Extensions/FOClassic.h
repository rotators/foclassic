/////
//
// FOClassic v@FOCLASSIC_VERSION@
// Timestamp @EXTENSIONS_CONTENT_TIMESTAMP@
//
/////
//
// This file contains all exposed engine structures used in FOClassic.
// It should not be edited directly by game developers.
//
// Used by native extensions.
//
/////
//
// Engine compiled with MSVC 2010
// Default calling convention - cdecl
//
/////

#ifndef __FOCLASSIC_H__
#define __FOCLASSIC_H__

// Engine version
#define FOCLASSIC_STAGE                  (@FOCLASSIC_STAGE@)
#define FOCLASSIC_VERSION                (@FOCLASSIC_VERSION@)

// Detect operating system
#if defined (_WIN32) || defined (_WIN64)
# define FO_WINDOWS
#elif defined (__linux__)
# define FO_LINUX
#elif defined (__APPLE__)
# define FO_MACOSX
#else
# error "Unknown operating system."
#endif

// Detect compiler
#if defined (__GNUC__)
# define FO_GCC
#elif defined (_MSC_VER) && !defined (__MWERKS__)
# define FO_MSVC
#else
# error "Unknown compiler."
#endif

// Detect CPU
#if (defined (FO_MSVC) && defined (_M_IX86) ) || (defined (FO_GCC) && !defined (__LP64__) )
# define FO_X86
#elif (defined (FO_MSVC) && defined (_M_X64) ) || (defined (FO_GCC) && defined (__LP64__) )
# define FO_X64
# error "X64 CPU not supported for now."
#else
# error "Unknown CPU."
#endif

// Detect target
#if defined (__SERVER)
# define TARGET_NAME                     SERVER
#elif defined (__CLIENT)
# define TARGET_NAME                     CLIENT
#elif defined (__MAPPER)
# define TARGET_NAME                     MAPPER
#else
# error __SERVER / __CLIENT / __MAPPER any of this must be defined
#endif

// Detect ill setup
#if !defined (FOCLASSIC_EXTENSION)
# error "FOCLASSIC_EXTENSION not defined."
#endif

// Detect options
#if !defined (FOCLASSIC_EXTENSION_STRIP_CONST)
# define foconst                         const
#else
# define foconst
#endif

// Platform specific options
#undef NDEBUG
#define NDEBUG

#ifdef FO_MSVC
# undef _WINDOWS
# undef _MBCS
# undef _CRT_SECURE_NO_WARNINGS
# undef _CRT_SECURE_NO_DEPRECATE
# undef _HAS_ITERATOR_DEBUGGING
# undef _SECURE_SCL
# undef _HAS_EXCEPTIONS

# define _WINDOWS
# define _MBCS
# define _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_DEPRECATE
# define _HAS_ITERATOR_DEBUGGING         0
# define _SECURE_SCL                     0
# define _HAS_EXCEPTIONS                 0
#endif // FO_MSVC

#undef EXPORT
#undef EXPORT_UNINITIALIZED
#ifdef FO_WINDOWS
# ifdef FO_MSVC
#  define EXPORT                         extern "C" __declspec( dllexport )
#  define EXPORT_UNINITIALIZED           extern "C" __declspec( dllexport ) extern
# else // FO_GCC
#  define EXPORT                         extern "C" __attribute__( (dllexport) )
#  define EXPORT_UNINITIALIZED           extern "C" __attribute__( (dllexport) ) extern
# endif
#else
# define EXPORT                          extern "C" __attribute__( (visibility( "default" ) ) )
# define EXPORT_UNINITIALIZED            extern "C" __attribute__( (visibility( "default" ) ) )
#endif

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

// AngelScript
#include "AngelScript/angelscript.h"
EXPORT_UNINITIALIZED asIScriptEngine* ASEngine;

// AngelScript add-ons
#include "AngelScript/scriptany.h"
#include "AngelScript/scriptarray.h"
#include "AngelScript/scriptfile.h"
#include "AngelScript/scriptdictionary.h"
#include "AngelScript/scriptstring.h"
#include "AngelScript/scriptmath.h"

#define MAX_NPC_BAGS_PACKS               (20)   // array size
#define MAX_ENEMY_STACK                  (30)
#define MAX_NPC_BAGS                     (50)
#define MAX_STORED_LOCATIONS             (1000) // aray size
#define GM_ZONES_FOG_SIZE                (2500)
#define MAX_SCRIPT_NAME                  (64)
#define MAPOBJ_SCRIPT_NAME               (25)
#define MAPOBJ_CRITTER_PARAMS            (40)
#define PASS_HASH_SIZE                   (32)
#define MAX_STORED_IP                    (20)
#define MAX_HEX_OFFSET                   (50)
#define AP_DIVIDER                       (100)
#define MAX_CRIT_TYPES                   (1000)
#define EFFECT_TEXTURES                  (10)
#define EFFECT_SCRIPT_VALUES             (10)

#define ITEM_MAX_BLOCK_LINES             (50)
#define ITEM_MAX_CHILDS                  (5)
#define ITEM_MAX_CHILD_LINES             (6)
#define ITEM_MAX_SCRIPT_VALUES           (10)

#define TB_BATTLE_TIMEOUT                (100000000)
#define TB_BATTLE_TIMEOUT_CHECK( to )    ( (to) > 10000000 )

#define MAP_LOOP_FUNC_MAX                (5)
#define MAP_MAX_DATA                     (100)

// FOClassic types
struct Client; // TODO rename to Player
struct Critter;
struct CritterCl;
struct CritterTimeEvent;
struct CritterType;
struct Field;
struct GameOptions;
struct GameVar;
struct GlobalMapGroup;
struct Item;
struct Location;
struct Map;
struct MapObject;
struct MapEntire;
struct Mutex;
struct Npc;
struct NpcPlane;
struct ProtoItem;
struct ProtoMap;
struct ProtoLocation;
struct SceneryToClient;
struct Spinlock;
struct SpriteInfo;
struct Sprite;
struct SyncObj;
struct TemplateVar;

// FOClassic additional types
class CmdLine;
class Ini;

#ifdef __SERVER
typedef Critter                             CritterMutual;
#else
typedef CritterCl                           CritterMutual;
typedef Item                                ItemCl;
#endif

typedef signed char                         int8;
typedef unsigned char                       uint8;
typedef signed short                        int16;
typedef unsigned short                      uint16;
typedef signed int                          int32;
typedef unsigned int                        uint32;
typedef unsigned int                        uint;
#if defined (FO_MSVC)
typedef unsigned __int64                    uint64;
typedef __int64                             int64;
#elif defined (FO_GCC)
# include <inttypes.h>
typedef uint64_t                            uint64;
typedef int64_t                             int64;
#endif

typedef pair<int, int>                      IntPair;
typedef pair<uint, uint>                    UintPair;
typedef pair<uint16, uint16>                Uint16Pair;

typedef vector<uint>                        UintVec;
typedef vector<uint>::const_iterator        UintVecIt;
typedef vector<uint16>                      Uint16Vec;
typedef vector<uint16>::const_iterator      Uint16VecIt;
typedef vector<int>                         IntVec;
typedef vector<int>::const_iterator         IntVecIt;
typedef vector<UintPair>                    UintPairVec;
typedef vector<Uint16Pair>                  Uint16PairVec;
typedef set<int>                            IntSet;
typedef set<int>::const_iterator            IntSetIt;
typedef set<uint>                           UintSet;
typedef set<uint>::const_iterator           UintSetIt;
typedef map<int, int>                       IntMap;
typedef set<int, int>::const_iterator       IntMapIt;

typedef vector<NpcPlane*>                   NpcPlaneVec;
typedef vector<NpcPlane*>::const_iterator   NpcPlaneVecIt;
typedef vector<Critter*>                    CrVec;
typedef vector<Critter*>::const_iterator    CrVecIt;
typedef map<uint, Critter*>                 CrMap;
typedef map<uint, Critter*>::const_iterator CrMapIt;
typedef vector<CritterCl*>                  CrClVec;
typedef vector<CritterCl*>::const_iterator  CrClVecIt;
typedef vector<Client*>                     ClVec;
typedef vector<Client*>::const_iterator     ClVecIt;
typedef vector<Npc*>                        PcVec;
typedef vector<Npc*>::const_iterator        PcVecIt;
typedef vector<Item*>                       ItemVec;
typedef vector<Item*>::const_iterator       ItemVecIt;
typedef vector<MapObject*>                  MapObjectVec;
typedef vector<MapObject*>::const_iterator  MapObjectVecIt;
typedef vector<Map*>                        MapVec;
typedef vector<Map*>::const_iterator        MapVecIt;
typedef vector<Location*>                   LocVec;
typedef vector<Location*>::const_iterator   LocVecIt;

// Generic
EXPORT_UNINITIALIZED asIScriptContext* ( * ScriptGetActiveContext )();
EXPORT_UNINITIALIZED const char*       (ScriptGetLibraryOptions)();
EXPORT_UNINITIALIZED const char*       (ScriptGetLibraryVersion)();
EXPORT_UNINITIALIZED void ( * Log )( const char* frmt, ... );

#if defined (FOCLASSIC_BLEEDING_EDGE)
# define FOCLASSIC_EXTENSION_ENTRY_BLEEDING_EDGE \
    GameOptionsExt * FOClassicExt;
#else
# define FOCLASSIC_EXTENSION_ENTRY_BLEEDING_EDGE
#endif

#define FOCLASSIC_EXTENSION_ENTRY( isCompiler )    \
    EXPORT void TARGET_NAME() {}                   \
    asIScriptEngine* ASEngine;                     \
    asIScriptContext* (*ScriptGetActiveContext)(); \
    const char* (ScriptGetLibraryOptions)();       \
    const char* (ScriptGetLibraryVersion)();       \
    GameOptions* FOClassic;                        \
    void (*Log)(const char* frmt, ...);            \
    FOCLASSIC_EXTENSION_ENTRY_BLEEDING_EDGE        \
    EXPORT void DllMainEx( bool isCompiler )

struct GameOptions
{
    const uint16         YearStart;
    const uint           YearStartFTLo;
    const uint           YearStartFTHi;
    const uint16         Year;
    const uint16         Month;
    const uint16         Day;
    const uint16         Hour;
    const uint16         Minute;
    const uint16         Second;
    const uint           FullSecondStart;
    const uint           FullSecond;
    const uint16         TimeMultiplier;
    const uint           GameTimeTick;

    const bool           DisableTcpNagle;
    const bool           DisableZlibCompression;
    const uint           FloodSize;
    const bool           NoAnswerShuffle;
    const bool           DialogDemandRecheck;
    const uint           FixBoyDefaultExperience;
    const uint           SneakDivider;
    const uint           LevelCap;
    const bool           LevelCapAddExperience;
    const uint           LookNormal;
    const uint           LookMinimum;
    const uint           WorldmapMaxGroupCount;
    const uint           CritterIdleTick;
    const uint           TurnBasedTick;
    const int            DeadHitPoints;
    const uint           Breaktime;
    foconst uint         TimeoutTransfer;
    const uint           TimeoutBattle;
    const uint           ApRegeneration;
    const uint           RtApCostCritterWalk;
    const uint           RtApCostCritterRun;
    const uint           RtApCostMoveItemContainer;
    const uint           RtApCostMoveItemInventory;
    const uint           RtApCostPickItem;
    const uint           RtApCostDropItem;
    const uint           RtApCostReloadWeapon;
    const uint           RtApCostPickCritter;
    const uint           RtApCostUseItem;
    const uint           RtApCostUseSkill;
    const bool           RtAlwaysRun;
    const uint           TbApCostCritterMove;
    const uint           TbApCostMoveItemContainer;
    const uint           TbApCostMoveItemInventory;
    const uint           TbApCostPickItem;
    const uint           TbApCostDropItem;
    const uint           TbApCostReloadWeapon;
    const uint           TbApCostPickCritter;
    const uint           TbApCostUseItem;
    const uint           TbApCostUseSkill;
    const bool           TbAlwaysRun;
    const uint           ApCostAimEyes;
    const uint           ApCostAimHead;
    const uint           ApCostAimGroin;
    const uint           ApCostAimTorso;
    const uint           ApCostAimArms;
    const uint           ApCostAimLegs;
    const bool           RunOnCombat;
    const bool           RunOnTransfer;
    const uint           GlobalMapWidth;
    const uint           GlobalMapHeight;
    const uint           GlobalMapZoneLength;
    const uint           GlobalMapMoveTime;
    const uint           BagRefreshTime;
    const uint           AttackAnimationsMinDist;
    const uint           WhisperDist;
    const uint           ShoutDist;
    const int            LookChecks;
    const uint           LookDir[5];
    const uint           LookSneakDir[5];
    const uint           LookWeight;
    const bool           CustomItemCost;
    const uint           RegistrationTimeout;
    const uint           AccountPlayTime;
    const bool           LoggingVars;
    const uint           ScriptRunSuspendTimeout;
    const uint           ScriptRunMessageTimeout;
    const uint           TalkDistance;
    const uint           NpcMaxTalkers;
    const uint           MinNameLength;
    const uint           MaxNameLength;
    const uint           DlgTalkMinTime;
    const uint           DlgBarterMinTime;
    const uint           MinimumOfflineTime;

    const int            StartSpecialPoints;
    const int            StartTagSkillPoints;
    const int            SkillMaxValue;
    const int            SkillModAdd2;
    const int            SkillModAdd3;
    const int            SkillModAdd4;
    const int            SkillModAdd5;
    const int            SkillModAdd6;

    const bool           AbsoluteOffsets;
    const uint           SkillBegin;
    const uint           SkillEnd;
    const uint           TimeoutBegin;
    const uint           TimeoutEnd;
    const uint           KillBegin;
    const uint           KillEnd;
    const uint           PerkBegin;
    const uint           PerkEnd;
    const uint           AddictionBegin;
    const uint           AddictionEnd;
    const uint           KarmaBegin;
    const uint           KarmaEnd;
    const uint           DamageBegin;
    const uint           DamageEnd;
    const uint           TraitBegin;
    const uint           TraitEnd;
    const uint           ReputationBegin;
    const uint           ReputationEnd;

    const int            ReputationLoved;
    const int            ReputationLiked;
    const int            ReputationAccepted;
    const int            ReputationNeutral;
    const int            ReputationAntipathy;
    const int            ReputationHated;

    foconst bool         MapHexagonal;
    foconst int          MapHexWidth;
    foconst int          MapHexHeight;
    foconst int          MapHexLineHeight;
    foconst int          MapTileOffsX;
    foconst int          MapTileOffsY;
    foconst int          MapRoofOffsX;
    foconst int          MapRoofOffsY;
    foconst int          MapRoofSkipSize;
    foconst float        MapCameraAngle;
    foconst bool         MapSmoothPath;
    foconst ScriptString MapDataPrefix;

    // Client and Mapper
    const bool           Quit;
    const bool           OpenGLRendering;
    const bool           OpenGLDebug;
    const bool           AssimpLogging;
    const int            MouseX;
    const int            MouseY;
    const int            ScrOx;
    const int            ScrOy;
    const bool           ShowTile;
    const bool           ShowRoof;
    const bool           ShowItem;
    const bool           ShowScen;
    const bool           ShowWall;
    const bool           ShowCrit;
    const bool           ShowFast;
    const bool           ShowPlayerNames;
    const bool           ShowNpcNames;
    const bool           ShowCritId;
    const bool           ScrollKeybLeft;
    const bool           ScrollKeybRight;
    const bool           ScrollKeybUp;
    const bool           ScrollKeybDown;
    const bool           ScrollMouseLeft;
    const bool           ScrollMouseRight;
    const bool           ScrollMouseUp;
    const bool           ScrollMouseDown;
    const bool           ShowGroups;
    const bool           HelpInfo;
    const bool           DebugInfo;
    const bool           DebugNet;
    const bool           DebugSprites;
    const bool           FullScreen;
    const bool           VSync;
    const int            FlushVal;
    const int            BaseTexture;
    const int            Light;
    const ScriptString   Host;
    const uint           Port;
    const uint           ProxyType;
    const ScriptString   ProxyHost;
    const uint           ProxyPort;
    const ScriptString   ProxyUser;
    const ScriptString   ProxyPass;
    const ScriptString   Name;
    const int            ScrollDelay;
    const uint           ScrollStep;
    const bool           ScrollCheck;
    const ScriptString   DataPath;
    const int            FixedFPS;
    const uint           FPS;
    const uint           PingPeriod;
    const uint           Ping;
    const bool           MsgboxInvert;
    const uint8          DefaultCombatMode;
    const bool           MessNotify;
    const bool           SoundNotify;
    const bool           AlwaysOnTop;
    const uint           TextDelay;
    const uint           DamageHitDelay;
    const int            ScreenWidth;
    const int            ScreenHeight;
    const int            MultiSampling;
    const bool           MouseScroll;
    const int            IndicatorType;
    const uint           DoubleClickTime;
    const uint8          RoofAlpha;
    const bool           HideCursor;
    const bool           DisableLMenu;
    const bool           DisableMouseEvents;
    const bool           DisableKeyboardEvents;
    const bool           HidePassword;
    const ScriptString   PlayerOffAppendix;
    const int            CombatMessagesType;
    const uint           Animation3dSmoothTime;
    const uint           Animation3dFPS;
    const int            RunModMul;
    const int            RunModDiv;
    const int            RunModAdd;
    const bool           MapZooming;
    const float          SpritesZoom;
    const float          SpritesZoomMax;
    const float          SpritesZoomMin;
    const float          EffectValues[EFFECT_SCRIPT_VALUES];
    const bool           AlwaysRun;
    const int            AlwaysRunMoveDist;
    const int            AlwaysRunUseDist;
    const ScriptString   KeyboardRemap;
    const uint           CritterFidgetTime;
    const uint           Anim2CombatBegin;
    const uint           Anim2CombatIdle;
    const uint           Anim2CombatEnd;
    const uint           RainTick;
    const int16          RainSpeedX;
    const int16          RainSpeedY;

    // Mapper
    const ScriptString   ClientPath;
    const ScriptString   ServerPath;
    const bool           ShowCorners;
    const bool           ShowCuttedSprites;
    const bool           ShowDrawOrder;
    const bool           SplitTilesCollection;

    // Engine data
    void                 ( * CritterChangeParameter )( Critter& cr, uint index );                          // Call for correct changing critter parameter
    CritterType*         CritterTypes;                                                                     // Array of critter types, maximum is MAX_CRIT_TYPES

    Field*               ClientMap;                                                                        // Array of client map hexes, accessing - ClientMap[hexY * ClientMapWidth + hexX]
    uint8*               ClientMapLight;                                                                   // Hex light, accessing - ClientMapLight[hexY * ClientMapWidth * 3 + hexX * 3 {+ 0(R), 1(G), 2(B)}]
    uint                 ClientMapWidth;                                                                   // Map width
    uint                 ClientMapHeight;                                                                  // Map height

    Sprite**             ( * GetDrawingSprites )( uint& count );                                           // Array of currently drawing sprites, tree is sorted
    SpriteInfo*          ( * GetSpriteInfo )( uint sprId );                                                // Sprite information
    uint                 ( * GetSpriteColor )( uint sprId, int x, int y, bool affectZoom );                // Color of pixel on sprite
    bool                 ( * IsSpriteHit )( Sprite* sprite, int x, int y, bool checkEgg );                 // Is position hitting sprite

    const char*          ( * GetNameByHash )( uint hash );                                                 // Get name of file by hash
    uint                 ( * GetHashByName )( const char* name );                                          // Get hash of file name

    // TODO move to GameScript
    bool                 (* ScriptLoadModule )( const char* moduleName );
    uint                 (* ScriptBind )( const char* moduleName, const char* funcDecl, bool temporaryId );   // Returning bindId
    bool                 (* ScriptPrepare )( uint bindId );
    void                 (* ScriptSetArgInt8 )( int8 value );
    void                 (* ScriptSetArgInt16 )( int16 value );
    void                 (* ScriptSetArgInt )( int value );
    void                 (* ScriptSetArgInt64 )( int64 value );
    void                 (* ScriptSetArgUInt8 )( uint8 value );
    void                 (* ScriptSetArgUInt16 )( uint16 value );
    void                 (* ScriptSetArgUInt )( uint value );
    void                 (* ScriptSetArgUInt64 )( uint64 value );
    void                 (* ScriptSetArgBool )( bool value );
    void                 (* ScriptSetArgFloat )( float value );
    void                 (* ScriptSetArgDouble )( double value );
    void                 (* ScriptSetArgObject )( void* value );
    void                 (* ScriptSetArgAddress )( void* value );
    bool                 (* ScriptRunPrepared )();
    int8                 (* ScriptGetReturnedInt8 )();
    int16                (* ScriptGetReturnedInt16 )();
    int                  (* ScriptGetReturnedInt )();
    int64                (* ScriptGetReturnedInt64 )();
    uint8                (* ScriptGetReturnedUInt8 )();
    uint16               (* ScriptGetReturnedUInt16 )();
    uint                 (* ScriptGetReturnedUInt )();
    uint64               (* ScriptGetReturnedUInt64 )();
    bool                 (* ScriptGetReturnedBool )();
    float                (* ScriptGetReturnedFloat )();
    double               (* ScriptGetReturnedDouble )();
    void*                (* ScriptGetReturnedObject )();
    void*                (* ScriptGetReturnedAddress )();

    int                  (* Random )( int minimum, int maximumInclusive );
    uint                 (* GetTick )();
    void                 (* SetLogCallback )( void (*function)( const char* ), bool enable );

    // Callbacks
    uint                 (* GetUseApCost)( CritterMutual& cr, Item& item, uint8 mode );
    uint                 (* GetAttackDistantion )( CritterMutual& cr, Item& item, uint8 mode );
    void                 (* GetRainOffset )( int16* ox, int16* oy );
};

EXPORT_UNINITIALIZED GameOptions* FOClassic;

#if defined (FOCLASSIC_BLEEDING_EDGE)
struct GameOptionsExt
{
    uint8       WallAlpha;

    # if 0
    // TODO implement GameScript
    GameScript* Script;
    # endif

    CmdLine*    CommandLine;
    Ini*        ConfigFile;
};

EXPORT_UNINITIALIZED GameOptionsExt* FOClassicExt;
#endif


struct Mutex
{
    const int Locker1[6];      // Windows - CRITICAL_SECTION (Locker1), Linux - pthread_mutex_t (Locker1)
    const int Locker2[5];      // MacOSX - pthread_mutex_t (Locker1 + Locker2)
};

struct Spinlock
{
    const long Locker;
};

struct SyncObj
{
    const void* CurMngr;
};

struct CritterType
{
    const bool Enabled;
    const char Name[64];
    const char SoundName[64];
    const uint Alias;
    const uint Multihex;
    const int  AnimType;

    const bool CanWalk;
    const bool CanRun;
    const bool CanAim;
    const bool CanArmor;
    const bool CanRotate;

    const bool Anim1[37];   // A..Z 0..9
};

struct ProtoItem
{
    // Internal data
    const uint16 ProtoId;
    const int    Type;
    const uint   PicMap;
    const uint   PicInv;
    const uint   Flags;
    const bool   Stackable;
    const bool   Deteriorable;
    const bool   GroundLevel;
    const int    Corner;
    const int    Dir;
    const uint8  Slot;
    const uint   Weight;
    const uint   Volume;
    const uint   Cost;
    const uint   StartCount;
    const uint8  SoundId;
    const uint8  Material;
    const uint8  LightFlags;
    const uint8  LightDistance;
    const int8   LightIntensity;
    const uint   LightColor;
    const bool   DisableEgg;
    const uint16 AnimWaitBase;
    const uint16 AnimWaitRndMin;
    const uint16 AnimWaitRndMax;
    const uint8  AnimStay[2];
    const uint8  AnimShow[2];
    const uint8  AnimHide[2];
    const int16  OffsetX;
    const int16  OffsetY;
    const uint8  SpriteCut;
    const int8   DrawOrderOffsetHexY;
    const uint16 RadioChannel;
    const uint16 RadioFlags;
    const uint8  RadioBroadcastSend;
    const uint8  RadioBroadcastRecv;
    const uint8  IndicatorStart;
    const uint8  IndicatorMax;
    const uint   HolodiskNum;
    const int    StartValue[ITEM_MAX_SCRIPT_VALUES];
    const uint8  BlockLines[ITEM_MAX_BLOCK_LINES];
    const uint16 ChildPids[ITEM_MAX_CHILDS];
    const uint8  ChildLines[ITEM_MAX_CHILDS][ITEM_MAX_CHILD_LINES];

    // User data, binded with 'bindfield' pragma
    // Common
    const int    MagicPower;
    const uint8  Unused[96];
    // Armor, offset 100
    const uint   Armor_CrTypeMale;
    const uint   Armor_CrTypeFemale;
    const int    Armor_AC;
    const uint   Armor_Perk;
    const int    Armor_DRNormal;
    const int    Armor_DRLaser;
    const int    Armor_DRFire;
    const int    Armor_DRPlasma;
    const int    Armor_DRElectr;
    const int    Armor_DREmp;
    const int    Armor_DRExplode;
    const int    Armor_DTNormal;
    const int    Armor_DTLaser;
    const int    Armor_DTFire;
    const int    Armor_DTPlasma;
    const int    Armor_DTElectr;
    const int    Armor_DTEmp;
    const int    Armor_DTExplode;
    const uint8  Armor_Unused[28];
    // Weapon, offset 200
    const int    Weapon_DmgType[3];
    const uint   Weapon_Anim2[3];
    const int    Weapon_DmgMin[3];
    const int    Weapon_DmgMax[3];
    const uint16 Weapon_Effect[3];
    const bool   Weapon_Remove[3];
    const uint   Weapon_ReloadAp;
    const int    Weapon_UnarmedCriticalBonus;
    const uint   Weapon_CriticalFailture;
    const bool   Weapon_UnarmedArmorPiercing;
    const uint8  Weapon_Unused[27];
    // Ammo, offset 300
    const int    Ammo_AcMod;
    const int    Ammo_DrMod;
    const uint   Ammo_DmgMult;
    const uint   Ammo_DmgDiv;
    // Other
    const uint8  UnusedEnd[184];

    // Type specific data
    const bool   Weapon_IsUnarmed;
    const int    Weapon_UnarmedTree;
    const int    Weapon_UnarmedPriority;
    const int    Weapon_UnarmedMinAgility;
    const int    Weapon_UnarmedMinUnarmed;
    const int    Weapon_UnarmedMinLevel;
    const uint   Weapon_Anim1;
    const uint   Weapon_MaxAmmoCount;
    const int    Weapon_Caliber;
    const uint16 Weapon_DefaultAmmoPid;
    const int    Weapon_MinStrength;
    const int    Weapon_Perk;
    const uint   Weapon_ActiveUses;
    const int    Weapon_Skill[USE_MAX];
    const uint   Weapon_PicUse[USE_MAX];
    const uint   Weapon_MaxDist[USE_MAX];
    const uint   Weapon_Round[USE_MAX];
    const uint   Weapon_ApCost[USE_MAX];
    const bool   Weapon_Aim[USE_MAX];
    const uint8  Weapon_SoundId[USE_MAX];
    const int    Ammo_Caliber;
    const bool   Door_NoBlockMove;
    const bool   Door_NoBlockShoot;
    const bool   Door_NoBlockLight;
    const uint   Container_Volume;
    const bool   Container_CannotPickUp;
    const bool   Container_MagicHandsGrnd;
    const bool   Container_Changeble;
    const uint16 Locker_Condition;
    const int    Grid_Type;
    const uint   Car_Speed;
    const uint   Car_Passability;
    const uint   Car_DeteriorationRate;
    const uint   Car_CrittersCapacity;
    const uint   Car_TankVolume;
    const uint   Car_MaxDeterioration;
    const uint   Car_FuelConsumption;
    const uint   Car_Entrance;
    const uint   Car_MovementType;

    bool IsItem()      const { return Type != ITEM_TYPE_GENERIC && Type != ITEM_TYPE_WALL && Type != ITEM_TYPE_GRID; }
    bool IsGeneric()   const { return Type == ITEM_TYPE_GENERIC; }
    bool IsScen()      const { return Type == ITEM_TYPE_GENERIC; }     // TODO remove duplicate IsScen() <-> IsGeneric()
    bool IsWall()      const { return Type == ITEM_TYPE_WALL; }
    bool IsArmor()     const { return Type == ITEM_TYPE_ARMOR; }
    bool IsDrug()      const { return Type == ITEM_TYPE_DRUG; }
    bool IsWeapon()    const { return Type == ITEM_TYPE_WEAPON; }
    bool IsAmmo()      const { return Type == ITEM_TYPE_AMMO; }
    bool IsMisc()      const { return Type == ITEM_TYPE_MISC; }
    bool IsKey()       const { return Type == ITEM_TYPE_KEY; }
    bool IsContainer() const { return Type == ITEM_TYPE_CONTAINER; }
    bool IsDoor()      const { return Type == ITEM_TYPE_DOOR; }
    bool IsGrid()      const { return Type == ITEM_TYPE_GRID; }
    bool IsCar()       const { return Type == ITEM_TYPE_CAR; }
    bool LockerIsChangeble() const // TODO IsLockerChangeable
    {
        if( IsDoor() ) { return true; }
        if( IsContainer() ) { return Container_Changeble; }
        return false;
    }
    bool IsCanPickUp() { return FLAG( Flags, ITEM_FLAG_CAN_PICKUP ); }
};

struct TemplateVar
{
    const int    Type;
    const uint16 TempId;
    const string Name;
    const string Desc;
    const int    StartVal;
    const int    MinVal;
    const int    MaxVal;
    const uint   Flags;

    bool IsQuest()     const { return FLAG( Flags, VAR_FLAG_QUEST ); }
    bool IsRandom()    const { return FLAG( Flags, VAR_FLAG_RANDOM ); }
    bool IsNoBorders() const { return FLAG( Flags, VAR_FLAG_NO_CHECK ); }
};

struct GameVar
{
    const uint         MasterId;
    const uint         SlaveId;
    const int          VarValue;
    const TemplateVar* VarTemplate;
    const uint         QuestVarIndex;
    const uint16       Type;
    const int16        RefCount;
    const SyncObj      Sync;

    int    GetValue()    const { return VarValue; }
    int    GetMin()      const { return VarTemplate->MinVal; }
    int    GetMax()      const { return VarTemplate->MaxVal; }
    bool   IsQuest()     const { return VarTemplate->IsQuest(); }
    uint   GetQuestStr() const { return VAR_CALC_QUEST( VarTemplate->TempId, VarValue ); }
    uint64 GetUid()      const { return ( ( (uint64)SlaveId ) << 32 ) | ( (uint64)MasterId ); }
};

struct NpcPlane
{
    const int       Type;
    const uint      Priority;
    const int       Identifier;
    const uint      IdentifierExt;
    const NpcPlane* ChildPlane;
    const bool      IsMove;

    union
    {
        struct
        {
            bool IsRun;
            uint WaitSecond;
            int  ScriptBindId;
        } const Misc;

        struct
        {
            bool   IsRun;
            uint   TargId;
            int    MinHp;
            bool   IsGag;
            uint16 GagHexX, GagHexY;
            uint16 LastHexX, LastHexY;
        } const Attack;

        struct
        {
            bool   IsRun;
            uint16 HexX;
            uint16 HexY;
            uint8  Dir;
            uint   Cut;
        } const Walk;

        struct
        {
            bool   IsRun;
            uint16 HexX;
            uint16 HexY;
            uint16 Pid;
            uint   UseItemId;
            bool   ToOpen;
        } const Pick;

        struct
        {
            uint Buffer[8];
        } const Buffer;
    };

    struct
    {
        const uint   PathNum;
        const uint   Iter;
        const bool   IsRun;
        const uint   TargId;
        const uint16 HexX;
        const uint16 HexY;
        const uint   Cut;
        const uint   Trace;
    } Move;

    const bool      Assigned;
    const int       RefCounter;

    const NpcPlane* GetCurPlane()           const { return ChildPlane ? ChildPlane->GetCurPlane() : this; }
    bool            IsSelfOrHas( int type ) const { return Type == type || (ChildPlane ? ChildPlane->IsSelfOrHas( type ) : false); }
    uint            GetChildIndex( NpcPlane* child ) const
    {
        uint index = 0;
        for( const NpcPlane* child_ = this; child_; index++ )
        {
            if( child_ == child ) { break; }
            else { child_ = child_->ChildPlane; }
        }
        return index;
    }
    uint GetChildsCount() const
    {
        uint            count = 0;
        const NpcPlane* child = ChildPlane;
        for( ; child; count++, child = child->ChildPlane ) {}
        return count;
    }
};

struct Item
{
    const uint       Id;
    const ProtoItem* Proto;
    const int        From;
    const uint8      Accessory;
    const bool       ViewPlaceOnMap;
    const int16      Reserved0;

    union
    {
        struct
        {
            uint   MapId;
            uint16 HexX;
            uint16 HexY;
        } const AccHex;

        struct
        {
            uint  Id;
            uint8 Slot;
        } const AccCritter;

        struct
        {
            uint ContainerId;
            uint StackId;
        } const AccContainer;

        const char AccBuffer[8];
    };

    struct _Data
    {
        const uint16  SortValue;
        const uint8   Info;
        const uint8   Indicator;
        const uint    PicMapHash;
        const uint    PicInvHash;
        const uint16  AnimWaitBase;
        const uint8   AnimStay[2];
        const uint8   AnimShow[2];
        const uint8   AnimHide[2];
        const uint    Flags;
        foconst uint8 Rate;
        const int8    LightIntensity;
        const uint8   LightDistance;
        const uint8   LightFlags;
        const uint    LightColor;
        const uint16  ScriptId;
        const int16   TrapValue;
        const uint    Count;
        const uint    Cost;
        const int     ScriptValues[ITEM_MAX_SCRIPT_VALUES];
        const uint8   BrokenFlags;
        const uint8   BrokenCount;
        const uint16  Deterioration;
        const uint16  AmmoPid;
        const uint16  AmmoCount;
        const uint    LockerId;
        const uint16  LockerCondition;
        const uint16  LockerComplexity;
        const uint    HolodiskNumber;
        const uint16  RadioChannel;
        const uint16  RadioFlags;
        const uint8   RadioBroadcastSend;
        const uint8   RadioBroadcastRecv;
        const uint16  Charge;
        const int16   OffsetX;
        const int16   OffsetY;
        const int16   Dir;
        const char    Reserved[2];
    } Data;

    const int16        RefCounter;
    const bool         IsNotValid;

    #ifdef __SERVER
    const int          FuncId[ITEM_EVENT_MAX];
    const Critter*     ViewByCritter;
    const ItemVec*     ChildItems;
    const char*        Lexems;
    const SyncObj      Sync;
    #endif

    #ifdef __CLIENT
    const ScriptString Lexems;
    #endif

    uint   GetId()       const { return Id; }
    uint16 GetProtoId()  const { return Proto->ProtoId; }
    uint   GetInfo()     const { return Proto->ProtoId * 100 + Data.Info; }
    uint   GetPicMap()   const { return Data.PicMapHash ? Data.PicMapHash : Proto->PicMap; }
    uint   GetPicInv()   const { return Data.PicInvHash ? Data.PicInvHash : Proto->PicInv; }
    uint8  GetType()     const { return Proto->Type; }
    bool   IsStackable() const { return Proto->Stackable; }

    bool IsPassed()           const { return FLAG( Data.Flags, ITEM_FLAG_NO_BLOCK ) && FLAG( Data.Flags, ITEM_FLAG_SHOOT_THRU ); }
    bool IsRaked()            const { return FLAG( Data.Flags, ITEM_FLAG_SHOOT_THRU ); }
    bool IsFlat()             const { return FLAG( Data.Flags, ITEM_FLAG_FLAT ); }
    bool IsHidden()           const { return FLAG( Data.Flags, ITEM_FLAG_HIDDEN ); }
    bool IsCanTalk()          const { return FLAG( Data.Flags, ITEM_FLAG_CAN_TALK ); }
    bool IsCanUse()           const { return FLAG( Data.Flags, ITEM_FLAG_CAN_USE ); }
    bool IsCanUseOnSmth()     const { return FLAG( Data.Flags, ITEM_FLAG_CAN_USE_ON_SMTH ); }
    bool IsHasTimer()         const { return FLAG( Data.Flags, ITEM_FLAG_HAS_TIMER ); }
    bool IsBadItem()          const { return FLAG( Data.Flags, ITEM_FLAG_BAD_ITEM ); }
    bool IsTwoHands()         const { return FLAG( Data.Flags, ITEM_FLAG_TWO_HANDS ); }
    bool IsBigGun()           const { return FLAG( Data.Flags, ITEM_FLAG_BIG_GUN ); }
    bool IsNoHighlight()      const { return FLAG( Data.Flags, ITEM_FLAG_NO_HIGHLIGHT ); }
    bool IsShowAnim()         const { return FLAG( Data.Flags, ITEM_FLAG_SHOW_ANIM ); }
    bool IsShowAnimExt()      const { return FLAG( Data.Flags, ITEM_FLAG_SHOW_ANIM_EXT ); }
    bool IsLightThru()        const { return FLAG( Data.Flags, ITEM_FLAG_LIGHT_THRU ); }
    bool IsAlwaysView()       const { return FLAG( Data.Flags, ITEM_FLAG_ALWAYS_VIEW ); }
    bool IsGeck()             const { return FLAG( Data.Flags, ITEM_FLAG_GECK ); }
    bool IsNoLightInfluence() const { return FLAG( Data.Flags, ITEM_FLAG_NO_LIGHT_INFLUENCE ); }
    bool IsNoLoot()           const { return FLAG( Data.Flags, ITEM_FLAG_NO_LOOT ); }
    bool IsNoSteal()          const { return FLAG( Data.Flags, ITEM_FLAG_NO_STEAL ); }
    bool IsCanPickUp()        const { return FLAG( Data.Flags, ITEM_FLAG_CAN_PICKUP ); }

    bool IsDeteriorable()   const { return Proto->Deteriorable; }
    bool IsBroken()         const { return FLAG( Data.BrokenFlags, BI_BROKEN ); }
    int  GetBrokenCount()   const { return Data.BrokenCount; }
    int  GetDeterioration() const { return Data.Deterioration; }
    int  GetDeteriorationProc() const
    {
        int val = GetDeterioration() * 100 / MAX_DETERIORATION;
        return CLAMP( val, 0, 100 );
    }

    uint GetCount()  const { return IsStackable() ? Data.Count : 1; }
    uint GetVolume() const { return GetCount() * Proto->Volume; }
    uint GetWeight() const { return GetCount() * Proto->Weight; }

    // Armor
    bool IsArmor() const { return GetType() == ITEM_TYPE_ARMOR; }

    // Weapon
    bool IsWeapon()                  const { return GetType() == ITEM_TYPE_WEAPON; }
    bool WeapIsEmpty()               const { return !Data.AmmoCount; }
    bool WeapIsFull()                const { return Data.AmmoCount >= Proto->Weapon_MaxAmmoCount; }
    uint WeapGetAmmoCount()          const { return Data.AmmoCount; }
    uint WeapGetAmmoPid()            const { return Data.AmmoPid; }
    uint WeapGetMaxAmmoCount()       const { return Proto->Weapon_MaxAmmoCount; }
    int  WeapGetAmmoCaliber()        const { return Proto->Weapon_Caliber; }
    int  WeapGetNeedStrength()       const { return Proto->Weapon_MinStrength; }
    bool WeapIsUseAviable( int use ) const { return use >= USE_PRIMARY && use <= USE_THIRD ? ( ( (Proto->Weapon_ActiveUses >> use) & 1 ) != 0 ) : false; }
    bool WeapIsCanAim( int use )     const { return use >= 0 && use < USE_MAX && Proto->Weapon_Aim[use]; }

    // Container
    bool IsContainer()          const { return Proto->IsContainer(); }
    bool ContIsCannotPickUp()   const { return Proto->Container_CannotPickUp; }
    bool ContIsMagicHandsGrnd() const { return Proto->Container_MagicHandsGrnd; }
    bool ContIsChangeble()      const { return Proto->Container_Changeble; }

    // Door
    bool IsDoor() const { return GetType() == ITEM_TYPE_DOOR; }

    // Locker
    bool IsHasLocker()       const { return IsDoor() || IsContainer(); }
    uint LockerDoorId()      const { return Data.LockerId; }
    bool LockerIsOpen()      const { return FLAG( Data.LockerCondition, LOCKER_ISOPEN ); }
    bool LockerIsClose()     const { return !LockerIsOpen(); }
    bool LockerIsChangeble() const { return Proto->LockerIsChangeble(); }
    int  LockerComplexity()  const { return Data.LockerComplexity; }

    // Ammo
    bool IsAmmo()         const { return Proto->IsAmmo(); }
    int  AmmoGetCaliber() const { return Proto->Ammo_Caliber; }

    // Key
    bool IsKey()     const { return Proto->IsKey(); }
    uint KeyDoorId() const { return Data.LockerId; }

    // Drug
    bool IsDrug() const { return Proto->IsDrug(); }

    // Misc
    bool IsMisc() const { return Proto->IsMisc(); }

    // Colorize
    bool  IsColorize() const { return FLAG( Data.Flags, ITEM_FLAG_COLORIZE ); }
    uint  GetColor()   const { return (Data.LightColor ? Data.LightColor : Proto->LightColor) & 0xFFFFFF; }
    uint8 GetAlpha()   const { return (Data.LightColor ? Data.LightColor : Proto->LightColor) >> 24; }

    // Light
    bool IsLight()           const { return FLAG( Data.Flags, ITEM_FLAG_LIGHT ); }
    int  LightGetIntensity() const { return Data.LightIntensity ? Data.LightIntensity : Proto->LightIntensity; }
    int  LightGetDistance()  const { return Data.LightDistance ? Data.LightDistance : Proto->LightDistance; }
    int  LightGetFlags()     const { return Data.LightFlags ? Data.LightFlags : Proto->LightFlags; }
    uint LightGetColor()     const { return (Data.LightColor ? Data.LightColor : Proto->LightColor) & 0xFFFFFF; }

    // Car
    bool IsCar() const { return Proto->IsCar(); }

    // Trap
    bool IsTrap()       const { return FLAG( Data.Flags, ITEM_FLAG_TRAP ); }
    int  TrapGetValue() const { return Data.TrapValue; }
};

struct GlobalMapGroup
{
    const CrVec    Group;
    const Critter* Rule;
    const uint     CarId;
    const float    CurX, CurY;
    const float    ToX, ToY;
    const float    Speed;
    const bool     IsSetMove;
    const uint     TimeCanFollow;
    const bool     IsMultiply;
    const uint     ProcessLastTick;
    const uint     EncounterDescriptor;
    const uint     EncounterTick;
    const bool     EncounterForce;
};

struct CritterTimeEvent
{
    const uint FuncNum;
    const uint Rate;
    const uint NextTime;
    const int  Identifier;
};
typedef vector<CritterTimeEvent>                 CritterTimeEventVec;
typedef vector<CritterTimeEvent>::const_iterator CritterTimeEventVecIt;

struct Critter
{
    const uint   Id;
    const uint16 HexX;
    const uint16 HexY;
    const uint16 WorldX;
    const uint16 WorldY;
    const uint   BaseType;
    const uint8  Dir;
    const uint8  Cond;
    const uint8  ReservedCE;
    const uint8  Reserved0;
    const uint   ScriptId;
    const uint   ShowCritterDist1;
    const uint   ShowCritterDist2;
    const uint   ShowCritterDist3;
    const uint16 Reserved00;
    const int16  Multihex;
    const uint   GlobalGroupUid;
    const uint16 LastHexX;
    const uint16 LastHexY;
    const uint   Reserved1[4];
    const uint   MapId;
    const uint16 MapPid;
    const uint16 Reserved2;
    const int    Params[MAX_PARAMS];
    const uint   Anim1Life;
    const uint   Anim1Knockout;
    const uint   Anim1Dead;
    const uint   Anim2Life;
    const uint   Anim2Knockout;
    const uint   Anim2Dead;
    const uint   Anim2KnockoutEnd;
    const uint   Reserved3[3];
    const char   Lexems[LEXEMS_SIZE];
    const uint   Reserved4[8];
    const bool   ClientToDelete;
    const uint8  Reserved5;
    const uint16 Reserved6;
    const uint   Temp;
    const uint16 Reserved8;
    const uint16 HoloInfoCount;
    const uint   HoloInfo[MAX_HOLO_INFO];
    const uint   Reserved9[10];
    const int    Scores[SCORES_MAX];

    // Binded with pragma bindfield
    const uint   GlobalMapMoveCounter;
    const uint8  UserData[CRITTER_USER_DATA_SIZE - sizeof(uint)];

    // Npc data
    const uint   HomeMap;
    const uint16 HomeX;
    const uint16 HomeY;
    const uint8  HomeDir;
    const uint8  Reserved11;
    const uint16 ProtoId;
    const uint   Reserved12;
    const uint   Reserved13;
    const uint   Reserved14;
    const uint   Reserved15;
    const bool   IsDataExt;
    const uint8  Reserved16;
    const uint16 Reserved17;
    const uint   Reserved18[8];
    const uint16 FavoriteItemPid[4];
    const uint   Reserved19[10];
    const uint   EnemyStackCount;
    const uint   EnemyStack[MAX_ENEMY_STACK];
    const uint   Reserved20[5];
    const uint8  BagCurrentSet[MAX_NPC_BAGS_PACKS];
    const int16  BagRefreshTime;
    const uint8  Reserved21;
    const uint8  BagSize;
    struct
    {
        const uint ItemPid;
        const uint MinCnt;
        const uint MaxCnt;
        const uint ItemSlot;
    } const Bag[MAX_NPC_BAGS];
    const uint Reserved22[100];

    // Ext data
    struct
    {
        const uint   Reserved23[10];
        const uint8  GlobalMapFog[GM_ZONES_FOG_SIZE];
        const uint16 Reserved24;
        const uint16 LocationsCount;
        const uint   LocationsId[MAX_STORED_LOCATIONS];
        const uint   Reserved25[40];
        const uint   PlayIp[MAX_STORED_IP];       // 0 - registration ip
        const uint16 PlayPort[MAX_STORED_IP];
        const uint   CurrentIp;
        const uint   Reserved26[29];
    }* const DataExt;

    const SyncObj      Sync;
    const bool         CritterIsNpc;
    const uint         Flags;
    const ScriptString NameStr;

    struct
    {
        const bool   IsAlloc;
        const uint8* Data;
        const uint   Width;
        const uint   Height;
        const uint   WidthB;
    } const GMapFog;

    const bool                IsRuning;
    const uint                PrevHexTick;
    const uint16              PrevHexX, PrevHexY;
    const int                 LockMapTransfers;
    const Critter*            ThisPtr[MAX_PARAMETERS_ARRAYS];
    const uint                AllowedToDownloadMap;
    const bool                ParamsIsChanged[MAX_PARAMS];
    const IntVec              ParamsChanged;
    const int                 ParamLocked;
    const CrVec               VisCr;
    const CrVec               VisCrSelf;
    const CrMap               VisCrMap;
    const CrMap               VisCrSelfMap;
    const UintSet             VisCr1, VisCr2, VisCr3;
    const UintSet             VisItem;
    const Spinlock            VisItemLocker;
    const uint                ViewMapId;
    const uint16              ViewMapPid, ViewMapLook, ViewMapHx, ViewMapHy;
    const uint8               ViewMapDir;
    const uint                ViewMapLocId, ViewMapLocEnt;

    const GlobalMapGroup*     GroupSelf;
    const GlobalMapGroup*     GroupMove;

    const ItemVec             InvItems;
    const Item*               DefItemSlotHand;
    const Item*               DefItemSlotArmor;
    const Item*               ItemSlotMain;
    const Item*               ItemSlotExt;
    const Item*               ItemSlotArmor;
    const int                 FuncId[CRITTER_EVENT_MAX];
    const uint                KnockoutAp;
    const uint                NextIntellectCachingTick;
    const uint16              IntellectCacheValue;
    const uint                LookCacheValue;
    const uint                StartBreakTime;
    const uint                BreakTime;
    const uint                WaitEndTick;
    const int                 DisableSend;
    const uint                AccessContainerId;
    const uint                ItemTransferCount;
    const uint                TryingGoHomeTick;

    const CritterTimeEventVec CrTimeEvents;

    const uint                GlobalIdleNextTick;
    const uint                ApRegenerationTick;
    const bool                IsNotValid;
    const int                 RefCounter;

    uint                      GetItemsWeight() const
    {
        uint res = 0;
        for( ItemVecIt it = InvItems.begin(), end = InvItems.end(); it != end; ++it )
        {
            const Item* item = *it;
            if( !item->IsHidden() ) { res += item->GetWeight(); }
        }
        return res;
    }

    uint GetItemsVolume() const
    {
        uint res = 0;
        for( ItemVecIt it = InvItems.begin(), end = InvItems.end(); it != end; ++it )
        {
            const Item* item = *it;
            if( !item->IsHidden() ) { res += item->GetVolume(); }
        }
        return res;
    }
};

struct Client : Critter
{
    const char Name[UTF8_BUF_SIZE( MAX_NAME )];
    const char PassHash[PASS_HASH_SIZE];
    uint8      Access;
    const uint LanguageMsg;
    const uint UID[5];
};

struct Npc : Critter
{
    const uint        NextRefreshBagTick;
    const NpcPlaneVec AiPlanes;
    const uint        Reserved;
};

struct CritterCl
{
    const uint          Id;
    const uint16        Pid;
    const uint16        HexX, HexY;
    const uint8         Dir;
    const int           Params[MAX_PARAMS];
    const uint          NameColor;
    const uint          ContourColor;
    const Uint16Vec     LastHexX, LastHexY;
    const uint8         Cond;
    const uint          Anim1Life;
    const uint          Anim1Knockout;
    const uint          Anim1Dead;
    const uint          Anim2Life;
    const uint          Anim2Knockout;
    const uint          Anim2Dead;
    const uint          Flags;
    const uint          BaseType, BaseTypeAlias;
    const uint          ApRegenerationTick;
    const int16         Multihex;
    void*               DrawEffect;

    const ScriptString  Name;
    const ScriptString  NameOnHead;
    const ScriptString  Lexems;
    const ScriptString  Avatar;
    const string        PasswordReg;

    const ItemVec       InvItems;
    const Item*         DefItemSlotHand;
    const Item*         DefItemSlotArmor;
    const Item*         ItemSlotMain;
    const Item*         ItemSlotExt;
    const Item*         ItemSlotArmor;

    const CritterCl*    ThisPtr[MAX_PARAMETERS_ARRAYS];
    const bool          ParamsIsChanged[MAX_PARAMS];
    const IntVec        ParamsChanged;
    const int           ParamLocked;

    const bool          IsRuning;
    const Uint16PairVec MoveSteps;
};

struct MapObject
{
    const uint8  MapObjType;
    const uint16 ProtoId;
    const uint16 MapX;
    const uint16 MapY;
    const int16  Dir;

    const uint   UID;
    const uint   ContainerUID;
    const uint   ParentUID;
    const uint   ParentChildIndex;

    const uint   LightRGB;
    const uint8  LightDay;
    const uint8  LightDirOff;
    const uint8  LightDistance;
    const int8   LightIntensity;

    const char   ScriptName[MAPOBJ_SCRIPT_NAME + 1];
    const char   FuncName[MAPOBJ_SCRIPT_NAME + 1];

    const uint   Reserved[7];
    const int    UserData[10];

    union
    {
        struct
        {
            uint8 Cond;
            uint  Anim1;
            uint  Anim2;
            int16 ParamIndex[MAPOBJ_CRITTER_PARAMS];
            int   ParamValue[MAPOBJ_CRITTER_PARAMS];
        } const MCritter;

        struct
        {
            int16  OffsetX;
            int16  OffsetY;
            uint8  AnimStayBegin;
            uint8  AnimStayEnd;
            uint16 AnimWait;
            uint8  InfoOffset;
            uint   PicMapHash;
            uint   PicInvHash;

            uint   Count;
            uint8  ItemSlot;

            uint8  BrokenFlags;
            uint8  BrokenCount;
            uint16 Deterioration;

            uint16 AmmoPid;
            uint   AmmoCount;

            uint   LockerDoorId;
            uint16 LockerCondition;
            uint16 LockerComplexity;

            int16  TrapValue;

            int    Val[10];
        } const MItem;

        struct
        {
            int16  OffsetX;
            int16  OffsetY;
            uint8  AnimStayBegin;
            uint8  AnimStayEnd;
            uint16 AnimWait;
            uint8  InfoOffset;
            uint   PicMapHash;
            uint   PicInvHash;

            bool   CanUse;
            bool   CanTalk;
            uint   TriggerNum;

            uint8  ParamsCount;
            int    Param[5];

            uint16 ToMapPid;
            uint   ToEntire;
            uint8  ToDir;
        } const MScenery;
    };
};

struct MapEntire
{
    const uint   Number;
    const uint16 HexX;
    const uint16 HexY;
    const uint8  Dir;
};
typedef vector<MapEntire> EntiresVec;

struct SceneryToClient
{
    const uint16 ProtoId;
    const uint8  Flags;
    const uint8  Reserved0;
    const uint16 MapX;
    const uint16 MapY;
    const int16  OffsetX;
    const int16  OffsetY;
    const uint   LightColor;
    const uint8  LightDistance;
    const uint8  LightFlags;
    const int8   LightIntensity;
    const uint8  InfoOffset;
    const uint8  AnimStayBegin;
    const uint8  AnimStayEnd;
    const uint16 AnimWait;
    const uint   PicMapHash;
    const int16  Dir;
    const uint16 Reserved1;
};
typedef vector<SceneryToClient> SceneryToClientVec;

struct ProtoMap
{
    struct
    {
        const uint   Version;
        const uint16 MaxHexX, MaxHexY;
        const int    WorkHexX, WorkHexY;
        const char   ScriptModule[MAX_SCRIPT_NAME + 1];
        const char   ScriptFunc[MAX_SCRIPT_NAME + 1];
        const int    Time;
        const bool   NoLogOut;
        const int    DayTime[4];
        const uint8  DayColor[12];

        // Deprecated
        const uint16 HeaderSize;
        const bool   Packed;
        const uint   UnpackedDataLen;
    } const Header;

    const MapObjectVec MObjects;
    const uint         LastObjectUID;

    struct Tile
    {
        const uint   NameHash;
        const uint16 HexX, HexY;
        const int8   OffsX, OffsY;
        const uint8  Layer;
        const bool   IsRoof;
        #ifdef __MAPPER
        const bool   IsSelected;
        #endif
    };
    typedef vector<Tile>    TileVec;
    const TileVec    Tiles;

    #ifdef __MAPPER
    typedef vector<TileVec> TileVecVec;
    const TileVecVec TilesField;
    const TileVecVec RoofsField;

    const TileVec&   GetTiles( uint16 hx, uint16 hy, bool is_roof ) const
    {
        uint index = hy * Header.MaxHexX + hx;
        return is_roof ? RoofsField[index] : TilesField[index];
    }
    #endif

    #ifdef __SERVER
    const SceneryToClientVec WallsToSend;
    const SceneryToClientVec SceneriesToSend;
    const uint               HashTiles;
    const uint               HashWalls;
    const uint               HashScen;

    const MapObjectVec       CrittersVec;
    const MapObjectVec       ItemsVec;
    const MapObjectVec       SceneriesVec;
    const MapObjectVec       GridsVec;
    const uint8*             HexFlags;
    #endif // ifdef __SERVER

    const EntiresVec         MapEntires;

    const int                PathType;
    const string             Name;
    const uint16             Pid;
};

struct Map
{
    const SyncObj   Sync;
    const Mutex     DataLocker;
    const uint8*    HexFlags;
    const CrVec     MapCritters;
    const ClVec     MapPlayers;
    const PcVec     MapNpcs;
    const ItemVec   HexItems;
    const Location* MapLocation;

    struct
    {
        const uint   MapId;
        const uint16 MapPid;
        const uint8  MapRain;
        const bool   IsTurnBasedAviable;
        const int    MapTime;
        const uint   ScriptId;
        const int    MapDayTime[4];
        const uint8  MapDayColor[12];
        const uint   Reserved[20];
        const int    UserData[MAP_MAX_DATA];
    } const Data;

    const ProtoMap* Proto;

    const bool      NeedProcess;
    const int       FuncId[MAP_EVENT_MAX];
    const uint      LoopEnabled[MAP_LOOP_FUNC_MAX];
    const uint      LoopLastTick[MAP_LOOP_FUNC_MAX];
    const uint      LoopWaitTick[MAP_LOOP_FUNC_MAX];

    const bool      IsTurnBasedOn;
    const uint      TurnBasedEndTick;
    const int       TurnSequenceCur;
    const UintVec   TurnSequence;
    const bool      IsTurnBasedTimeout;
    const uint      TurnBasedBeginSecond;
    const bool      NeedEndTurnBased;
    const uint      TurnBasedRound;
    const uint      TurnBasedTurn;
    const uint      TurnBasedWholeTurn;

    const bool      IsNotValid;
    const int16     RefCounter;

    uint16 GetMaxHexX() const { return Proto->Header.MaxHexX; }
    uint16 GetMaxHexY() const { return Proto->Header.MaxHexY; }

    #ifdef __SERVER
    bool   IsHexTrigger( uint16 hx, uint16 hy ) const { return FLAG( Proto->HexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_TRIGGER ); }
    bool   IsHexTrap( uint16 hx, uint16 hy )    const { return FLAG( HexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_WALK_ITEM ); }
    bool   IsHexCritter( uint16 hx, uint16 hy ) const { return FLAG( HexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_CRITTER | HEX_FLAG_DEAD_CRITTER ); }
    bool   IsHexGag( uint16 hx, uint16 hy )     const { return FLAG( HexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_GAG_ITEM ); }
    uint16 GetHexFlags( uint16 hx, uint16 hy )  const { return (HexFlags[hy * GetMaxHexX() + hx] << 8) | Proto->HexFlags[hy * GetMaxHexX() + hx]; }
    bool   IsHexPassed( uint16 hx, uint16 hy )  const { return !FLAG( GetHexFlags( hx, hy ), HEX_FLAG_NOWAY ); }
    bool   IsHexRaked( uint16 hx, uint16 hy )   const { return !FLAG( GetHexFlags( hx, hy ), HEX_FLAG_NOSHOOT ); }
    #endif
};

struct ProtoLocation
{
    const bool        IsInit;
    const uint16      LocPid;
    const string      Name;

    const uint        MaxPlayers;
    const Uint16Vec   ProtoMapPids;
    const Uint16Vec   AutomapsPids;
    const UintPairVec Entrance;
    const int         ScriptBindId;

    const uint16      Radius;
    const bool        Visible;
    const bool        AutoGarbage;
    const bool        GeckVisible;
};

struct Location
{
    const SyncObj Sync;
    const MapVec  LocMaps;

    struct
    {
        const uint   LocId;
        const uint16 LocPid;
        const uint16 WX;
        const uint16 WY;
        const uint16 Radius;
        const bool   Visible;
        const bool   GeckVisible;
        const bool   AutoGarbage;
        const bool   ToGarbage;
        const uint   Color;
        const uint   Reserved3[59];
    } const Data;

    const ProtoLocation* Proto;
    const int            GeckCount;
    const int            FuncId[LOCATION_EVENT_MAX];

    const bool           IsNotValid;
    const int16          RefCounter;

    bool IsToGarbage() const { return Data.AutoGarbage || Data.ToGarbage; }
    bool IsVisible()   const { return Data.Visible || (Data.GeckVisible && GeckCount > 0); }
};

struct Field
{
    struct Tile
    {
        const void* Anim;
        const int16 OffsX;
        const int16 OffsY;
        const uint8 Layer;
    };
    typedef vector<Tile> TileVec;

    const CritterCl* Crit;
    const CrClVec    DeadCrits;
    const int        ScrX;
    const int        ScrY;
    const TileVec    Tiles;
    const TileVec    Roofs;
    const ItemVec    Items;
    const int16      RoofNum;
    const bool       ScrollBlock;
    const bool       IsWall;
    const bool       IsWallSAI;
    const bool       IsWallTransp;
    const bool       IsScen;
    const bool       IsExitGrid;
    const bool       IsNotPassed;
    const bool       IsNotRaked;
    const uint8      Corner;
    const bool       IsNoLight;
    const uint8      LightValues[3];
    const bool       IsMultihex;
};

struct SpriteInfo
{
    const void*  Surface;
    const float  SurfaceUV[4];
    const uint16 Width;
    const uint16 Height;
    const int16  OffsX;
    const int16  OffsY;
    const void*  DrawEffect;
    const void*  Anim3d;     // If Anim3d != NULL than this is pure 3d animation
};

struct Sprite
{
    // Ordering
    const int     DrawOrderType;     // 0..4 - flat, 5..9 - normal
    const uint    DrawOrderPos;
    const uint    TreeIndex;

    // Sprite information, pass to GetSpriteInfo
    const uint    SprId;
    const uint*   PSprId;     // If PSprId == NULL than used SprId

    // Positions
    const int     HexX, HexY;
    const int     ScrX, ScrY;
    const int16*  OffsX, * OffsY;

    // Cutting
    const int     CutType;     // See Sprites cutting
    const Sprite* Parent, * Child;
    const float   CutX, CutW, CutTexL, CutTexR;

    // Other
    const uint8*  Alpha;
    const uint8*  Light;
    const int     EggType;
    const int     ContourType;
    const uint    ContourColor;
    const uint    Color;
    const uint    FlashMask;
    const void*   DrawEffect;
    const bool*   ValidCallback;
    const bool    Valid;     // If Valid == false than this sprite not valid

    #ifdef __MAPPER
    const int     CutOyL, CutOyR;
    #endif

    uint const GetSprId()
    {
        return PSprId ? *PSprId : SprId;
    }

    SpriteInfo* const GetSprInfo()
    {
        return FOClassic->GetSpriteInfo( PSprId ? *PSprId : SprId );
    }

    void const GetPos( int& x, int& y )
    {
        SpriteInfo* si = GetSprInfo();
        x = (int)( (float)(ScrX - si->Width / 2 + si->OffsX + (OffsX ? *OffsX : 0) + FOClassic->ScrOx) / FOClassic->SpritesZoom );
        y = (int)( (float)(ScrY - si->Height    + si->OffsY + (OffsY ? *OffsY : 0) + FOClassic->ScrOy) / FOClassic->SpritesZoom );
    }
};


inline Field* GetField( uint hexX, uint hexY )
{
    if( !FOClassic->ClientMap || hexX >= FOClassic->ClientMapWidth || hexY >= FOClassic->ClientMapHeight )
    {
        return NULL;
    }
    return &FOClassic->ClientMap[hexY * FOClassic->ClientMapWidth + hexX];
}

inline uint GetFieldLight( uint hexX, uint hexY )
{
    if( !FOClassic->ClientMapLight || hexX >= FOClassic->ClientMapWidth || hexY >= FOClassic->ClientMapHeight )
    {
        return 0;
    }
    uint r = FOClassic->ClientMapLight[hexY * FOClassic->ClientMapWidth * 3 + hexX * 3 + 0];
    uint g = FOClassic->ClientMapLight[hexY * FOClassic->ClientMapWidth * 3 + hexX * 3 + 1];
    uint b = FOClassic->ClientMapLight[hexY * FOClassic->ClientMapWidth * 3 + hexX * 3 + 2];
    uint rgb = (r << 16) | (g << 8) | (b);
    return rgb;
}

inline int GetDirection( int x1, int y1, int x2, int y2 )
{
    if( FOClassic->MapHexagonal )
    {
        float hx = (float)x1;
        float hy = (float)y1;
        float tx = (float)x2;
        float ty = (float)y2;
        float nx = 3 * (tx - hx);
        float ny = (ty - hy) * SQRT3T2_FLOAT - (float(x2 & 1) - float(x1 & 1) ) * SQRT3_FLOAT;
        float dir = 180.0f + RAD2DEG * atan2f( ny, nx );

        if( dir >= 60.0f  && dir < 120.0f )
        {
            return 5;
        }
        if( dir >= 120.0f && dir < 180.0f )
        {
            return 4;
        }
        if( dir >= 180.0f && dir < 240.0f )
        {
            return 3;
        }
        if( dir >= 240.0f && dir < 300.0f )
        {
            return 2;
        }
        if( dir >= 300.0f )
        {
            return 1;
        }
        return 0;
    }
    else
    {
        float dir = 180.0f + RAD2DEG * atan2( (float)(x2 - x1), (float)(y2 - y1) );

        if( dir >= 22.5f  && dir <  67.5f )
        {
            return 7;
        }
        if( dir >= 67.5f  && dir < 112.5f )
        {
            return 0;
        }
        if( dir >= 112.5f && dir < 157.5f )
        {
            return 1;
        }
        if( dir >= 157.5f && dir < 202.5f )
        {
            return 2;
        }
        if( dir >= 202.5f && dir < 247.5f )
        {
            return 3;
        }
        if( dir >= 247.5f && dir < 292.5f )
        {
            return 4;
        }
        if( dir >= 292.5f && dir < 337.5f )
        {
            return 5;
        }
        return 6;
    }
}

inline int GetDistantion( int x1, int y1, int x2, int y2 )
{
    if( FOClassic->MapHexagonal )
    {
        int dx = (x1 > x2 ? x1 - x2 : x2 - x1);
        if( x1 % 2 == 0 )
        {
            if( y2 <= y1 )
            {
                int rx = y1 - y2 - dx / 2;
                return dx + (rx > 0 ? rx : 0);
            }
            else
            {
                int rx = y2 - y1 - (dx + 1) / 2;
                return dx + (rx > 0 ? rx : 0);
            }
        }
        else
        {
            if( y2 >= y1 )
            {
                int rx = y2 - y1 - dx / 2;
                return dx + (rx > 0 ? rx : 0);
            }
            else
            {
                int rx = y1 - y2 - (dx + 1) / 2;
                return dx + (rx > 0 ? rx : 0);
            }
        }
    }
    else
    {
        int dx = abs( x2 - x1 );
        int dy = abs( y2 - y1 );
        return max( dx, dy );
    }
}

inline void static_asserts()
{
    STATIC_ASSERT( sizeof(char) == 1 );
    STATIC_ASSERT( sizeof(short) == 2 );
    STATIC_ASSERT( sizeof(int) == 4 );
    STATIC_ASSERT( sizeof(int64) == 8 );
    STATIC_ASSERT( sizeof(uint8) == 1 );
    STATIC_ASSERT( sizeof(uint16) == 2 );
    STATIC_ASSERT( sizeof(uint) == 4 );
    STATIC_ASSERT( sizeof(uint64) == 8 );
    STATIC_ASSERT( sizeof(bool) == 1 );
    STATIC_ASSERT( sizeof(string) == 28 );
    #if defined (FO_X86)
    STATIC_ASSERT( sizeof(size_t) == 4 );
    STATIC_ASSERT( sizeof(void*) == 4 );
    #elif defined (FO_X64)
    STATIC_ASSERT( sizeof(size_t) == 8 );
    STATIC_ASSERT( sizeof(void*) == 8 );
    #endif
    STATIC_ASSERT( sizeof(size_t) == sizeof(void*) );
}

// Cleanup
#undef foconst

#endif // __FOCLASSIC_H__ //
