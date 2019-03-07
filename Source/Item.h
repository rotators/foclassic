#ifndef __ITEM__
#define __ITEM__

#ifdef FOCLASSIC_CLIENT
# include "scriptstring.h"
#endif

#include "Defines.h"
#include "ThreadSync.h"
#include "Types.h"

class Critter;
class MapObject;

extern const char* ItemEventFuncName[ITEM_EVENT_MAX];

// Prototypes
#define PROTO_ITEM_DEFAULT_EXT      ".pro"
#define PROTO_ITEM_FILENAME         "proto.fopro_"
#define ITEM_MAX_SCRIPT_VALUES      (10)

// Blocks, childs
#define ITEM_MAX_BLOCK_LINES        (50)
#define ITEM_MAX_CHILDS             (5)
#define ITEM_MAX_CHILD_LINES        (6)

// Light flags
#define LIGHT_DISABLE_DIR( dir )    (1 << CLAMP( dir, 0, 5 ) )
#define LIGHT_GLOBAL                (0x40)
#define LIGHT_INVERSE               (0x80)

// Special item pids
#define SP_SCEN_LIGHT               (2141)             // Light Source
#define SP_SCEN_LIGHT_STOP          (4592)
#define SP_SCEN_BLOCK               (2067)             // Secret Blocking Hex
#define SP_SCEN_IBLOCK              (2344)             // Block Hex Auto Inviso
#define SP_SCEN_TRIGGER             (3852)
#define SP_WALL_BLOCK_LIGHT         (5621)             // Wall s.t. with light
#define SP_WALL_BLOCK               (5622)             // Wall s.t.
#define SP_GRID_EXITGRID            (2049)             // Exit Grid Map Marker
#define SP_GRID_ENTIRE              (3853)
#define SP_MISC_SCRBLOCK            (4012)             // Scroll block
#define SP_MISC_GRID_MAP( pid )     ( (pid) >= 4016 && (pid) <= 4023 )
#define SP_MISC_GRID_GM( pid )      ( (pid) >= 4031 && (pid) <= 4046 )

// Slot protos offsets
// 1000 - 1100 protos reserved
#define SLOT_MAIN_PROTO_OFFSET      (1000)
#define SLOT_EXT_PROTO_OFFSET       (1030)
#define SLOT_ARMOR_PROTO_OFFSET     (1060)

/************************************************************************/
/* ProtoItem                                                            */
/************************************************************************/

class ProtoItem
{
public:
    // Internal data
    uint16 ProtoId;
    int    Type;
    uint   PicMap;
    uint   PicInv;
    uint   Flags;
    bool   Stackable;
    bool   Deteriorable;
    bool   GroundLevel;
    int    Corner;
    int    Dir;
    uint8  Slot;
    uint   Weight;
    uint   Volume;
    uint   Cost;
    uint   StartCount;
    uint8  SoundId;
    uint8  Material;
    uint8  LightFlags;
    uint8  LightDistance;
    char   LightIntensity;
    uint   LightColor;
    bool   DisableEgg;
    uint16 AnimWaitBase;
    uint16 AnimWaitRndMin;
    uint16 AnimWaitRndMax;
    uint8  AnimStay[2];
    uint8  AnimShow[2];
    uint8  AnimHide[2];
    short  OffsetX;
    short  OffsetY;
    uint8  SpriteCut;
    char   DrawOrderOffsetHexY;
    uint16 RadioChannel;
    uint16 RadioFlags;
    uint8  RadioBroadcastSend;
    uint8  RadioBroadcastRecv;
    uint8  IndicatorStart;
    uint8  IndicatorMax;
    uint   HolodiskNum;
    int    StartValue[ITEM_MAX_SCRIPT_VALUES];
    uint8  BlockLines[ITEM_MAX_BLOCK_LINES];
    uint16 ChildPid[ITEM_MAX_CHILDS];
    uint8  ChildLines[ITEM_MAX_CHILDS][ITEM_MAX_CHILD_LINES];

    // User data, binded with 'bindfield' pragma
    int UserData[PROTO_ITEM_USER_DATA_SIZE / sizeof(int)];

    // Type specific data
    bool   Weapon_IsUnarmed;
    int    Weapon_UnarmedTree;
    int    Weapon_UnarmedPriority;
    int    Weapon_UnarmedMinAgility;
    int    Weapon_UnarmedMinUnarmed;
    int    Weapon_UnarmedMinLevel;
    uint   Weapon_Anim1;
    uint   Weapon_MaxAmmoCount;
    int    Weapon_Caliber;
    uint16 Weapon_DefaultAmmoPid;
    int    Weapon_MinStrength;
    int    Weapon_Perk;
    uint   Weapon_ActiveUses;
    int    Weapon_Skill[USE_MAX];
    uint   Weapon_PicUse[USE_MAX];
    uint   Weapon_MaxDist[USE_MAX];
    uint   Weapon_Round[USE_MAX];
    uint   Weapon_ApCost[USE_MAX];
    bool   Weapon_Aim[USE_MAX];
    uint8  Weapon_SoundId[USE_MAX];
    int    Ammo_Caliber;
    bool   Door_NoBlockMove;
    bool   Door_NoBlockShoot;
    bool   Door_NoBlockLight;
    uint   Container_Volume;
    bool   Container_CannotPickUp;
    bool   Container_MagicHandsGrnd;
    bool   Container_Changeble;
    uint16 Locker_Condition;
    int    Grid_Type;
    uint   Car_Speed;
    uint   Car_Passability;
    uint   Car_DeteriorationRate;
    uint   Car_CrittersCapacity;
    uint   Car_TankVolume;
    uint   Car_MaxDeterioration;
    uint   Car_FuelConsumption;
    uint   Car_Entrance;
    uint   Car_MovementType;

    void AddRef()  {}
    void Release() {}

    void Clear() { memzero( this, sizeof(ProtoItem) ); }
    uint GetHash();

    bool IsItem() { return !IsScen() && !IsWall() && !IsGrid(); }
    bool IsScen() { return Type == ITEM_TYPE_GENERIC; }
    bool IsWall() { return Type == ITEM_TYPE_WALL; }
    bool IsGrid() { return Type == ITEM_TYPE_GRID; }

    bool IsArmor()     { return Type == ITEM_TYPE_ARMOR; }
    bool IsDrug()      { return Type == ITEM_TYPE_DRUG; }
    bool IsWeapon()    { return Type == ITEM_TYPE_WEAPON; }
    bool IsAmmo()      { return Type == ITEM_TYPE_AMMO; }
    bool IsMisc()      { return Type == ITEM_TYPE_MISC; }
    bool IsKey()       { return Type == ITEM_TYPE_KEY; }
    bool IsContainer() { return Type == ITEM_TYPE_CONTAINER; }
    bool IsDoor()      { return Type == ITEM_TYPE_DOOR; }
    bool IsGeneric()   { return Type == ITEM_TYPE_GENERIC; }
    bool IsCar()       { return Type == ITEM_TYPE_CAR; }

    bool IsBlocks() { return BlockLines[0] != 0; }
    bool LockerIsChangeble()
    {
        if( IsDoor() ) return true;
        if( IsContainer() ) return Container_Changeble;
        return false;
    }
    bool IsCanPickUp() { return FLAG( Flags, ITEM_FLAG_CAN_PICKUP ); }

    bool operator==( const uint16& _r ) { return ProtoId == _r;  }
    ProtoItem() { Clear(); }

    #if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
    uint GetCurSprId();
    #endif

    #ifdef FONLINE_OBJECT_EDITOR
    std::string ScriptModule;
    std::string ScriptFunc;
    std::string PicMapStr;
    std::string PicInvStr;
    std::string WeaponPicStr[MAX_USES];
    std::string Weapon_Anim2[MAX_USES];
    #endif

    #ifdef FOCLASSIC_MAPPER
    char* CollectionName;
    #endif
};

typedef std::vector<ProtoItem> ProtoItemVec;

class Item;
typedef std::map<uint, Item*>  ItemPtrMap;
typedef std::vector<Item*>     ItemPtrVec;
typedef std::vector<Item>      ItemVec;

/************************************************************************/
/* Item                                                                 */
/************************************************************************/

class Item
{
public:
    uint       Id;
    ProtoItem* Proto;
    int        From;
    uint8      Accessory;
    bool       ViewPlaceOnMap;
    short      Reserved0;

    union     // 8
    {
        struct
        {
            uint   MapId;
            uint16 HexX;
            uint16 HexY;
        } AccHex;

        struct
        {
            uint  Id;
            uint8 Slot;
        } AccCritter;

        struct
        {
            uint ContainerId;
            uint StackId;
        } AccContainer;

        char AccBuffer[8];
    };

    struct ItemData     // 120, size used in NetProto.h
    {
        static char SendMask[ITEM_DATA_MASK_MAX][120];

        uint16      SortValue;
        uint8       Info;
        uint8       Indicator;
        uint        PicMapHash;
        uint        PicInvHash;
        uint16      AnimWaitBase;
        uint8       AnimStay[2];
        uint8       AnimShow[2];
        uint8       AnimHide[2];
        uint        Flags;
        uint8       Mode;
        char        LightIntensity;
        uint8       LightDistance;
        uint8       LightFlags;
        uint        LightColor;
        uint16      ScriptId;
        short       TrapValue;
        uint        Count;
        uint        Cost;
        int         ScriptValues[ITEM_MAX_SCRIPT_VALUES];
        uint8       BrokenFlags;
        uint8       BrokenCount;
        uint16      Deterioration;
        uint16      AmmoPid;
        uint16      AmmoCount;
        uint        LockerId;
        uint16      LockerCondition;
        uint16      LockerComplexity;
        uint        HolodiskNumber;
        uint16      RadioChannel;
        uint16      RadioFlags;
        uint8       RadioBroadcastSend;
        uint8       RadioBroadcastRecv;
        uint16      Charge;
        short       OffsetX;
        short       OffsetY;
        short       Dir;
        char        Reserved[2];
    } Data;

    short RefCounter;
    bool  IsNotValid;

    #ifdef FOCLASSIC_SERVER
    int         FuncId[ITEM_EVENT_MAX];
    Critter*    ViewByCritter;
    ItemPtrVec* ChildItems;
    char*       PLexems;
    SyncObject  Sync;
    #endif
    #ifdef FOCLASSIC_CLIENT
    ScriptString Lexems;
    #endif

    #if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_SERVER)
    Item();
    ~Item();
    #endif

    bool operator==( const uint& id ) { return Id == id; }

    void AddRef()  { RefCounter++; }
    void Release() { if( --RefCounter <= 0 ) delete this; }

    #ifdef FOCLASSIC_SERVER
    void FullClear();

    bool ParseScript( const char* script, bool first_time );
    bool PrepareScriptFunc( int num_scr_func );
    void EventFinish( bool deleted );
    bool EventAttack( Critter* cr, Critter* target );
    bool EventUse( Critter* cr, Critter* on_critter, Item* on_item, MapObject* on_scenery );
    bool EventUseOnMe( Critter* cr, Item* used_item );
    bool EventSkill( Critter* cr, int skill );
    void EventDrop( Critter* cr );
    void EventMove( Critter* cr, uint8 from_slot );
    void EventWalk( Critter* cr, bool entered, uint8 dir );
    #endif // FOCLASSIC_SERVER

    void        Init( ProtoItem* proto );
    Item*       Clone();
    void        SetSortValue( ItemPtrVec& items );
    static void SortItems( ItemVec& items );

    // All
    uint   GetId()            { return Id; }
    uint16 GetProtoId()       { return Proto->ProtoId; }
    uint   GetInfo()          { return Proto->ProtoId * 100 + Data.Info; }
    uint   GetPicMap()        { return Data.PicMapHash ? Data.PicMapHash : Proto->PicMap; }
    uint   GetPicInv()        { return Data.PicInvHash ? Data.PicInvHash : Proto->PicInv; }
    bool   IsValidAccessory() { return Accessory == ITEM_ACCESSORY_CRITTER || Accessory == ITEM_ACCESSORY_HEX || Accessory == ITEM_ACCESSORY_CONTAINER; }

    uint GetCount();
    void Count_Set( uint val );
    void Count_Add( uint val );
    void Count_Sub( uint val );

    int    GetType() { return Proto->Type; }
    void   SetMode( uint8 mode );
    uint16 GetSortValue() { return Data.SortValue; }

    bool IsStackable() { return Proto->Stackable; }
    bool IsBlocks()    { return Proto->IsBlocks(); }

    bool IsPassed()           { return FLAG( Data.Flags, ITEM_FLAG_NO_BLOCK ) && FLAG( Data.Flags, ITEM_FLAG_SHOOT_THRU ); }
    bool IsRaked()            { return FLAG( Data.Flags, ITEM_FLAG_SHOOT_THRU ); }
    bool IsFlat()             { return FLAG( Data.Flags, ITEM_FLAG_FLAT ); }
    bool IsHidden()           { return FLAG( Data.Flags, ITEM_FLAG_HIDDEN ); }
    bool IsCanPickUp()        { return FLAG( Data.Flags, ITEM_FLAG_CAN_PICKUP ); }
    bool IsCanTalk()          { return FLAG( Data.Flags, ITEM_FLAG_CAN_TALK ); }
    bool IsCanUse()           { return FLAG( Data.Flags, ITEM_FLAG_CAN_USE ); }
    bool IsCanUseOnSmth()     { return FLAG( Data.Flags, ITEM_FLAG_CAN_USE_ON_SMTH ); }
    bool IsHasTimer()         { return FLAG( Data.Flags, ITEM_FLAG_HAS_TIMER ); }
    bool IsBadItem()          { return FLAG( Data.Flags, ITEM_FLAG_BAD_ITEM ); }
    bool IsTwoHands()         { return FLAG( Data.Flags, ITEM_FLAG_TWO_HANDS ); }
    bool IsBigGun()           { return FLAG( Data.Flags, ITEM_FLAG_BIG_GUN ); }
    bool IsNoHighlight()      { return FLAG( Data.Flags, ITEM_FLAG_NO_HIGHLIGHT ); }
    bool IsShowAnim()         { return FLAG( Data.Flags, ITEM_FLAG_SHOW_ANIM ); }
    bool IsShowAnimExt()      { return FLAG( Data.Flags, ITEM_FLAG_SHOW_ANIM_EXT ); }
    bool IsLightThru()        { return FLAG( Data.Flags, ITEM_FLAG_LIGHT_THRU ); }
    bool IsAlwaysView()       { return FLAG( Data.Flags, ITEM_FLAG_ALWAYS_VIEW ); }
    bool IsGeck()             { return FLAG( Data.Flags, ITEM_FLAG_GECK ); }
    bool IsNoLightInfluence() { return FLAG( Data.Flags, ITEM_FLAG_NO_LIGHT_INFLUENCE ); }
    bool IsNoLoot()           { return FLAG( Data.Flags, ITEM_FLAG_NO_LOOT ); }
    bool IsNoSteal()          { return FLAG( Data.Flags, ITEM_FLAG_NO_STEAL ); }
    bool IsGag()              { return FLAG( Data.Flags, ITEM_FLAG_GAG ); }

    uint GetVolume()    { return GetCount() * Proto->Volume; }
    uint GetVolume1st() { return Proto->Volume; }
    uint GetWeight()    { return GetCount() * Proto->Weight; }
    uint GetWeight1st() { return Proto->Weight; }
    uint GetCost()      { return GetCount() * GetCost1st(); }
    uint GetCost1st();
    // uint GetCost1st(){return Data.Cost?Data.Cost:Proto->Cost;}

    #if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
    uint GetCurSprId();
    #endif

    #ifdef FOCLASSIC_SERVER
    void SetLexems( const char* lexems );
    #endif

    // Deterioration
    #ifdef FOCLASSIC_SERVER
    void Repair();
    #endif

    bool IsDeteriorable() { return Proto->Deteriorable; }
    bool IsBroken()       { return FLAG( Data.BrokenFlags, BI_BROKEN ); }
    int  GetDeteriorationProc()
    {
        int val = Data.Deterioration * 100 / MAX_DETERIORATION;
        return CLAMP( val, 0, 100 );
    }

    // Armor
    bool IsArmor() { return GetType() == ITEM_TYPE_ARMOR; }

    // Weapon
    bool IsWeapon()                  { return GetType() == ITEM_TYPE_WEAPON; }
    bool WeapIsEmpty()               { return !Data.AmmoCount; }
    bool WeapIsFull()                { return Data.AmmoCount >= Proto->Weapon_MaxAmmoCount; }
    uint WeapGetAmmoCount()          { return Data.AmmoCount; }
    uint WeapGetAmmoPid()            { return Data.AmmoPid; }
    uint WeapGetMaxAmmoCount()       { return Proto->Weapon_MaxAmmoCount; }
    int  WeapGetAmmoCaliber()        { return Proto->Weapon_Caliber; }
    bool WeapIsUseAviable( int use ) { return use >= USE_PRIMARY && use <= USE_THIRD ? ( ( (Proto->Weapon_ActiveUses >> use) & 1 ) != 0 ) : false; }
    bool WeapIsCanAim( int use )     { return use >= 0 && use < USE_MAX && Proto->Weapon_Aim[use]; }
    void WeapLoadHolder();

    // Container
    bool IsContainer()          { return Proto->IsContainer(); }
    bool ContIsCannotPickUp()   { return Proto->Container_CannotPickUp; }
    bool ContIsMagicHandsGrnd() { return Proto->Container_MagicHandsGrnd; }
    bool ContIsChangeble()      { return Proto->Container_Changeble; }
    #ifdef FOCLASSIC_SERVER
    void  ContAddItem( Item*& item, uint stack_id );
    void  ContSetItem( Item* item );
    void  ContEraseItem( Item* item );
    Item* ContGetItem( uint item_id, bool skip_hide );
    void  ContGetAllItems( ItemPtrVec& items, bool skip_hide, bool sync_lock );
    Item* ContGetItemByPid( uint16 pid, uint stack_id );
    void  ContGetItems( ItemPtrVec& items, uint stack_id, bool sync_lock );
    int   ContGetFreeVolume( uint stack_id );
    bool  ContIsItems();
    #endif

    // Door
    bool IsDoor() { return GetType() == ITEM_TYPE_DOOR; }

    // Locker
    bool IsHasLocker()       { return IsDoor() || IsContainer(); }
    uint LockerDoorId()      { return Data.LockerId; }
    bool LockerIsOpen()      { return FLAG( Data.LockerCondition, LOCKER_ISOPEN ); }
    bool LockerIsClose()     { return !LockerIsOpen(); }
    bool LockerIsChangeble() { return Proto->LockerIsChangeble(); }
    int  LockerComplexity()  { return Data.LockerComplexity; }

    // Ammo
    bool IsAmmo()         { return Proto->IsAmmo(); }
    int  AmmoGetCaliber() { return Proto->Ammo_Caliber; }

    // Key
    bool IsKey()     { return Proto->IsKey(); }
    uint KeyDoorId() { return Data.LockerId; }

    // Drug
    bool IsDrug() { return Proto->IsDrug(); }

    // Misc
    bool IsMisc() { return Proto->IsMisc(); }

    // Colorize
    bool  IsColorize()  { return FLAG( Data.Flags, ITEM_FLAG_COLORIZE ); }
    uint  GetColor()    { return (Data.LightColor ? Data.LightColor : Proto->LightColor) & 0xFFFFFF; }
    uint8 GetAlpha()    { return (Data.LightColor ? Data.LightColor : Proto->LightColor) >> 24; }
    uint  GetInvColor() { return FLAG( Data.Flags, ITEM_FLAG_COLORIZE_INV ) ? (Data.LightColor ? Data.LightColor : Proto->LightColor) : 0; }

    // Light
    bool IsLight() { return FLAG( Data.Flags, ITEM_FLAG_LIGHT ); }
    uint LightGetHash();
    int  LightGetIntensity() { return Data.LightIntensity ? Data.LightIntensity : Proto->LightIntensity; }
    int  LightGetDistance()  { return Data.LightDistance ? Data.LightDistance : Proto->LightDistance; }
    int  LightGetFlags()     { return Data.LightFlags ? Data.LightFlags : Proto->LightFlags; }
    uint LightGetColor()     { return (Data.LightColor ? Data.LightColor : Proto->LightColor) & 0xFFFFFF; }

    // Radio
    bool IsRadio()           { return FLAG( Data.Flags, ITEM_FLAG_RADIO ); }
    bool RadioIsSendActive() { return !FLAG( Data.RadioFlags, RADIO_DISABLE_SEND ); }
    bool RadioIsRecvActive() { return !FLAG( Data.RadioFlags, RADIO_DISABLE_RECV ); }

    // Car
    bool IsCar() { return Proto->IsCar(); }

    #ifdef FOCLASSIC_SERVER
    Item* GetChild( uint child_index );
    #endif

    // Holodisk
    bool IsHolodisk()               { return FLAG( Data.Flags, ITEM_FLAG_HOLODISK ); }
    uint HolodiskGetNum()           { return Data.HolodiskNumber; }
    void HolodiskSetNum( uint num ) { Data.HolodiskNumber = num; }

    // Trap
    bool IsTrap()                { return FLAG( Data.Flags, ITEM_FLAG_TRAP ); }
    void TrapSetValue( int val ) { Data.TrapValue = val; }
    int  TrapGetValue()          { return Data.TrapValue; }
};

/************************************************************************/
/*                                                                      */
/************************************************************************/

#define FOREACH_PROTO_ITEM_LINES( lines, hx, hy, maxhx, maxhy, work )        \
    int hx__ = hx, hy__ = hy;                                                \
    int maxhx__ = maxhx, maxhy__ = maxhy;                                    \
    for( uint i__ = 0; i__ < sizeof(lines); i__++ )                          \
    {                                                                        \
        uint8 block__ = lines[i__];                                          \
        uint8 dir__ = (block__ >> 4);                                        \
        if( dir__ >= DIRS_COUNT )                                            \
            break;                                                           \
        uint8 steps__ = (block__ & 0xF);                                     \
        if( !steps__ )                                                       \
            break;                                                           \
        for( uint8 j__ = 0; j__ < steps__; j__++ )                           \
        {                                                                    \
            MoveHexByDirUnsafe( hx__, hy__, dir__ );                         \
            if( hx__ < 0 || hy__ < 0 || hx__ >= maxhx__ || hy__ >= maxhy__ ) \
                continue;                                                    \
            hx = hx__, hy = hy__;                                            \
            work                                                             \
        }                                                                    \
    }

/************************************************************************/
/*                                                                      */
/************************************************************************/

#endif // __ITEM__
