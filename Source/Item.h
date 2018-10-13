#ifndef __ITEM__
#define __ITEM__

#include "Common.h"
#include "FileManager.h"
#include "Text.h"
#include "Crypt.h"

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
    ushort ProtoId;
    int    Type;
    uint   PicMap;
    uint   PicInv;
    uint   Flags;
    bool   Stackable;
    bool   Deteriorable;
    bool   GroundLevel;
    int    Corner;
    int    Dir;
    uchar  Slot;
    uint   Weight;
    uint   Volume;
    uint   Cost;
    uint   StartCount;
    uchar  SoundId;
    uchar  Material;
    uchar  LightFlags;
    uchar  LightDistance;
    char   LightIntensity;
    uint   LightColor;
    bool   DisableEgg;
    ushort AnimWaitBase;
    ushort AnimWaitRndMin;
    ushort AnimWaitRndMax;
    uchar  AnimStay[2];
    uchar  AnimShow[2];
    uchar  AnimHide[2];
    short  OffsetX;
    short  OffsetY;
    uchar  SpriteCut;
    char   DrawOrderOffsetHexY;
    ushort RadioChannel;
    ushort RadioFlags;
    uchar  RadioBroadcastSend;
    uchar  RadioBroadcastRecv;
    uchar  IndicatorStart;
    uchar  IndicatorMax;
    uint   HolodiskNum;
    int    StartValue[ITEM_MAX_SCRIPT_VALUES];
    uchar  BlockLines[ITEM_MAX_BLOCK_LINES];
    ushort ChildPid[ITEM_MAX_CHILDS];
    uchar  ChildLines[ITEM_MAX_CHILDS][ITEM_MAX_CHILD_LINES];

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
    ushort Weapon_DefaultAmmoPid;
    int    Weapon_MinStrength;
    int    Weapon_Perk;
    uint   Weapon_ActiveUses;
    int    Weapon_Skill[MAX_USES];
    uint   Weapon_PicUse[MAX_USES];
    uint   Weapon_MaxDist[MAX_USES];
    uint   Weapon_Round[MAX_USES];
    uint   Weapon_ApCost[MAX_USES];
    bool   Weapon_Aim[MAX_USES];
    uchar  Weapon_SoundId[MAX_USES];
    int    Ammo_Caliber;
    bool   Door_NoBlockMove;
    bool   Door_NoBlockShoot;
    bool   Door_NoBlockLight;
    uint   Container_Volume;
    bool   Container_CannotPickUp;
    bool   Container_MagicHandsGrnd;
    bool   Container_Changeble;
    ushort Locker_Condition;
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

    void Clear()   { memzero( this, sizeof(ProtoItem) ); }
    uint GetHash() { return Crypt.Crc32( (uchar*)this, sizeof(ProtoItem) ); }

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

    bool operator==( const ushort& _r ) { return ProtoId == _r;  }
    ProtoItem() { Clear(); }

    #if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
    uint GetCurSprId();
    #endif

    #ifdef FONLINE_OBJECT_EDITOR
    string ScriptModule;
    string ScriptFunc;
    string PicMapStr;
    string PicInvStr;
    string WeaponPicStr[MAX_USES];
    string Weapon_Anim2[MAX_USES];
    #endif

    #ifdef FOCLASSIC_MAPPER
    char* CollectionName;
    #endif
};

typedef vector<ProtoItem> ProtoItemVec;

class Item;
typedef map<uint, Item*>  ItemPtrMap;
typedef vector<Item*>     ItemPtrVec;
typedef vector<Item>      ItemVec;

/************************************************************************/
/* Item                                                                 */
/************************************************************************/

class Item
{
public:
    uint       Id;
    ProtoItem* Proto;
    int        From;
    uchar      Accessory;
    bool       ViewPlaceOnMap;
    short      Reserved0;

    union     // 8
    {
        struct
        {
            uint   MapId;
            ushort HexX;
            ushort HexY;
        } AccHex;

        struct
        {
            uint  Id;
            uchar Slot;
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

        ushort      SortValue;
        uchar       Info;
        uchar       Indicator;
        uint        PicMapHash;
        uint        PicInvHash;
        ushort      AnimWaitBase;
        uchar       AnimStay[2];
        uchar       AnimShow[2];
        uchar       AnimHide[2];
        uint        Flags;
        uchar       Mode;
        char        LightIntensity;
        uchar       LightDistance;
        uchar       LightFlags;
        uint        LightColor;
        ushort      ScriptId;
        short       TrapValue;
        uint        Count;
        uint        Cost;
        int         ScriptValues[ITEM_MAX_SCRIPT_VALUES];
        uchar       BrokenFlags;
        uchar       BrokenCount;
        ushort      Deterioration;
        ushort      AmmoPid;
        ushort      AmmoCount;
        uint        LockerId;
        ushort      LockerCondition;
        ushort      LockerComplexity;
        uint        HolodiskNumber;
        ushort      RadioChannel;
        ushort      RadioFlags;
        uchar       RadioBroadcastSend;
        uchar       RadioBroadcastRecv;
        ushort      Charge;
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
    void EventMove( Critter* cr, uchar from_slot );
    void EventWalk( Critter* cr, bool entered, uchar dir );
    #endif // FOCLASSIC_SERVER

    void        Init( ProtoItem* proto );
    Item*       Clone();
    void        SetSortValue( ItemPtrVec& items );
    static void SortItems( ItemVec& items );

    // All
    uint   GetId()            { return Id; }
    ushort GetProtoId()       { return Proto->ProtoId; }
    uint   GetInfo()          { return Proto->ProtoId * 100 + Data.Info; }
    uint   GetPicMap()        { return Data.PicMapHash ? Data.PicMapHash : Proto->PicMap; }
    uint   GetPicInv()        { return Data.PicInvHash ? Data.PicInvHash : Proto->PicInv; }
    bool   IsValidAccessory() { return Accessory == ITEM_ACCESSORY_CRITTER || Accessory == ITEM_ACCESSORY_HEX || Accessory == ITEM_ACCESSORY_CONTAINER; }

    uint GetCount();
    void Count_Set( uint val );
    void Count_Add( uint val );
    void Count_Sub( uint val );

    int    GetType() { return Proto->Type; }
    void   SetMode( uchar mode );
    ushort GetSortValue() { return Data.SortValue; }

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
    bool WeapIsCanAim( int use )     { return use >= 0 && use < MAX_USES && Proto->Weapon_Aim[use]; }
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
    Item* ContGetItemByPid( ushort pid, uint stack_id );
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
    uchar GetAlpha()    { return (Data.LightColor ? Data.LightColor : Proto->LightColor) >> 24; }
    uint  GetInvColor() { return FLAG( Data.Flags, ITEM_FLAG_COLORIZE_INV ) ? (Data.LightColor ? Data.LightColor : Proto->LightColor) : 0; }

    // Light
    bool IsLight() { return FLAG( Data.Flags, ITEM_FLAG_LIGHT ); }
    uint LightGetHash()
    {
        if( !IsLight() ) return 0;
        if( Data.LightIntensity ) return Crypt.Crc32( (uchar*)&Data.LightIntensity, 7 ) + FLAG( Data.Flags, ITEM_FLAG_LIGHT );
        return (uint)Proto;
    }
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

    bool operator==( const uint& id ) { return Id == id;  }

    #ifdef FOCLASSIC_SERVER
    Item()
    {
        memzero( this, sizeof(Item) );
        RefCounter = 1;
        IsNotValid = false;
        MEMORY_PROCESS( MEMORY_ITEM, sizeof(Item) );
    }
    ~Item()
    {
        Proto = NULL;
        if( PLexems ) MEMORY_PROCESS( MEMORY_ITEM, -LEXEMS_SIZE );
        SAFEDELA( PLexems );
        MEMORY_PROCESS( MEMORY_ITEM, -(int)sizeof(Item) );
    }
    #elif FOCLASSIC_CLIENT
    Item()
    {
        memzero( this, OFFSETOF( Item, IsNotValid ) );
        RefCounter = 1;
        IsNotValid = false;
    }
    ~Item() { Proto = NULL; }
    #endif
};

/************************************************************************/
/*                                                                      */
/************************************************************************/

#define FOREACH_PROTO_ITEM_LINES( lines, hx, hy, maxhx, maxhy, work )        \
    int hx__ = hx, hy__ = hy;                                                \
    int maxhx__ = maxhx, maxhy__ = maxhy;                                    \
    for( uint i__ = 0; i__ < sizeof(lines); i__++ )                          \
    {                                                                        \
        uchar block__ = lines[i__];                                          \
        uchar dir__ = (block__ >> 4);                                        \
        if( dir__ >= DIRS_COUNT )                                            \
            break;                                                           \
        uchar steps__ = (block__ & 0xF);                                     \
        if( !steps__ )                                                       \
            break;                                                           \
        for( uchar j__ = 0; j__ < steps__; j__++ )                           \
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
