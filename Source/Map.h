#ifndef __MAP__
#define __MAP__

#include "scriptarray.h"

#include "Critter.h"
#include "Defines.h"
#include "Item.h"
#include "Mutex.h"
#include "ProtoMap.h"
#include "ThreadSync.h"
#include "Types.h"

extern const char* MapEventFuncName[MAP_EVENT_MAX];

// Loop times
#define MAP_LOOP_FUNC_MAX        (5)
#define MAP_LOOP_DEFAULT_TICK    (60 * 60000)
#define MAP_MAX_DATA             (100)

class Map;
class Location;

class Map
{
public:
    SyncObject Sync;

private:
    Mutex      dataLocker;
    uint8*     hexFlags;
    CrVec      mapCritters;
    ClVec      mapPlayers;
    PcVec      mapNpcs;
    ItemPtrVec hexItems;
    Location*  mapLocation;

public:
    struct MapData
    {
        uint   MapId;
        uint16 MapPid;
        uint8  MapRain;
        bool   IsTurnBasedAviable;
        int    MapTime;
        uint   ScriptId;
        int    MapDayTime[4];
        uint8  MapDayColor[12];
        uint   Reserved[20];
        int    UserData[MAP_MAX_DATA];
    } Data;

    ProtoMap* Proto;

    bool      NeedProcess;
    int       FuncId[MAP_EVENT_MAX];
    uint      LoopEnabled[MAP_LOOP_FUNC_MAX];
    uint      LoopLastTick[MAP_LOOP_FUNC_MAX];
    uint      LoopWaitTick[MAP_LOOP_FUNC_MAX];

    bool Init( ProtoMap* proto, Location* location );
    bool Generate();
    void Clear( bool full );
    void Process();
    void Lock()   { dataLocker.Lock(); }
    void Unlock() { dataLocker.Unlock(); }

    Location* GetLocation( bool lock );
    uint16    GetMaxHexX() { return Proto->Header.MaxHexX; }
    uint16    GetMaxHexY() { return Proto->Header.MaxHexY; }
    void      SetLoopTime( uint loop_num, uint ms );
    uint8     GetRain();
    void      SetRain( uint8 capacity );
    int       GetTime();
    void      SetTime( int time );
    uint      GetDayTime( uint day_part );
    void      SetDayTime( uint day_part, uint time );
    void      GetDayColor( uint day_part, uint8& r, uint8& g, uint8& b );
    void      SetDayColor( uint day_part, uint8 r, uint8 g, uint8 b );
    int       GetData( uint index );
    void      SetData( uint index, int value );

    void SetText( uint16 hx, uint16 hy, uint color, const char* text, uint16 text_len, uint16 intellect, bool unsafe_text );
    void SetTextMsg( uint16 hx, uint16 hy, uint color, uint16 text_msg, uint num_str );
    void SetTextMsgLex( uint16 hx, uint16 hy, uint color, uint16 text_msg, uint num_str, const char* lexems, uint16 lexems_len );

    bool GetStartCoord( uint16& hx, uint16& hy, uint8& dir, uint entire );
    bool GetStartCoordCar( uint16& hx, uint16& hy, ProtoItem* proto_item );
    bool FindStartHex( uint16& hx, uint16& hy, uint multihex, uint seek_radius, bool skip_unsafe );

    void SetId( uint id, uint16 pid )
    {
        Data.MapId = id;
        Data.MapPid = pid;
    }
    uint   GetId()  { return Data.MapId; }
    uint16 GetPid() { return Data.MapPid; }

    void AddCritter( Critter* cr );
    void AddCritterEvents( Critter* cr );
    void EraseCritter( Critter* cr );
    void EraseCritterEvents( Critter* cr );
    void KickPlayersToGlobalMap();

    bool AddItem( Item* item, uint16 hx, uint16 hy );
    void SetItem( Item* item, uint16 hx, uint16 hy );
    void EraseItem( uint item_id );
    void ChangeDataItem( Item* item );
    void ChangeViewItem( Item* item );
    void AnimateItem( Item* item, uint8 from_frm, uint8 to_frm );

    Item* GetItem( uint item_id );
    Item* GetItemHex( uint16 hx, uint16 hy, uint16 item_pid, Critter* picker );
    Item* GetItemDoor( uint16 hx, uint16 hy );
    Item* GetItemCar( uint16 hx, uint16 hy );
    Item* GetItemContainer( uint16 hx, uint16 hy );
    Item* GetItemGag( uint16 hx, uint16 hy );

    ItemPtrVec& GetItemsNoLock() { return hexItems; }
    void        GetItemsHex( uint16 hx, uint16 hy, ItemPtrVec& items, bool lock );
    void        GetItemsHexEx( uint16 hx, uint16 hy, uint radius, uint16 pid, ItemPtrVec& items, bool lock );
    void        GetItemsPid( uint16 pid, ItemPtrVec& items, bool lock );
    void        GetItemsType( int type, ItemPtrVec& items, bool lock );
    void        GetItemsTrap( uint16 hx, uint16 hy, ItemPtrVec& items, bool lock );
    void        RecacheHexBlock( uint16 hx, uint16 hy );
    void        RecacheHexShoot( uint16 hx, uint16 hy );
    void        RecacheHexBlockShoot( uint16 hx, uint16 hy );

    uint16 GetHexFlags( uint16 hx, uint16 hy );
    void   SetHexFlag( uint16 hx, uint16 hy, uint8 flag );
    void   UnsetHexFlag( uint16 hx, uint16 hy, uint8 flag );

    bool IsHexPassed( uint16 hx, uint16 hy );
    bool IsHexRaked( uint16 hx, uint16 hy );
    bool IsHexesPassed( uint16 hx, uint16 hy, uint radius );
    bool IsMovePassed( uint16 hx, uint16 hy, uint8 dir, uint multihex );
    bool IsHexItem( uint16 hx, uint16 hy ) { return FLAG( hexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_ITEM ); }

    bool IsHexTrigger( uint16 hx, uint16 hy ) { return FLAG( Proto->HexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_TRIGGER ); }
    bool IsHexTrap( uint16 hx, uint16 hy )    { return FLAG( hexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_WALK_ITEM ); }

    bool IsHexCritter( uint16 hx, uint16 hy ) { return FLAG( hexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_CRITTER | HEX_FLAG_DEAD_CRITTER ); }
    bool IsHexGag( uint16 hx, uint16 hy )     { return FLAG( hexFlags[hy * GetMaxHexX() + hx], HEX_FLAG_GAG_ITEM ); }

    bool     IsFlagCritter( uint16 hx, uint16 hy, bool dead );
    void     SetFlagCritter( uint16 hx, uint16 hy, uint multihex, bool dead );
    void     UnsetFlagCritter( uint16 hx, uint16 hy, uint multihex, bool dead );
    uint     GetNpcCount( int npc_role, int find_type );
    Critter* GetCritter( uint crid, bool sync_lock );
    Critter* GetNpc( int npc_role, int find_type, uint skip_count, bool sync_lock );
    Critter* GetHexCritter( uint16 hx, uint16 hy, bool dead, bool sync_lock );
    void     GetCrittersHex( uint16 hx, uint16 hy, uint radius, int find_type, CrVec& critters, bool sync_lock ); // Critters append

    void   GetCritters( CrVec& critters, bool sync_lock );
    void   GetPlayers( ClVec& players, bool sync_lock );
    void   GetNpcs( PcVec& npcs, bool sync_lock );
    CrVec& GetCrittersNoLock() { return mapCritters; }
    ClVec& GetPlayersNoLock()  { return mapPlayers; }
    PcVec& GetNpcsNoLock()     { return mapNpcs; }
    uint   GetCrittersCount();
    uint   GetPlayersCount();
    uint   GetNpcsCount();

    bool IsNoLogOut() { return Proto->Header.NoLogOut; }

    // Sends
    void SendEffect( uint16 eff_pid, uint16 hx, uint16 hy, uint16 radius );
    void SendFlyEffect( uint16 eff_pid, uint from_crid, uint to_crid, uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy );

    // Cars
public:
    void  GetCritterCar( Critter* cr, Item* car );
    void  SetCritterCar( uint16 hx, uint16 hy, Critter* cr, Item* car );
    bool  IsPlaceForItem( uint16 hx, uint16 hy, ProtoItem* proto_item );
    void  PlaceItemBlocks( uint16 hx, uint16 hy, ProtoItem* proto_item );
    void  ReplaceItemBlocks( uint16 hx, uint16 hy, ProtoItem* proto_item );
    Item* GetItemChild( uint16 hx, uint16 hy, ProtoItem* proto_item, uint child_index );

    // Events
private:
    bool PrepareScriptFunc( int num_scr_func );

public:
    bool ParseScript( const char* script, bool first_time );
    void EventFinish( bool to_delete );
    void EventLoop( int loop_num );
    void EventInCritter( Critter* cr );
    void EventOutCritter( Critter* cr );
    void EventCritterDead( Critter* cr, Critter* killer );
    void EventTurnBasedBegin();
    void EventTurnBasedEnd();
    void EventTurnBasedProcess( Critter* cr, bool begin_turn );

    // Turn based game
public:
    bool    IsTurnBasedOn;
    uint    TurnBasedEndTick;
    int     TurnSequenceCur;
    UIntVec TurnSequence;
    bool    IsTurnBasedTimeout;
    uint    TurnBasedBeginSecond;
    bool    NeedEndTurnBased;
    uint    TurnBasedRound;
    uint    TurnBasedTurn;
    uint    TurnBasedWholeTurn;

    void BeginTurnBased( Critter* first_cr );
    void EndTurnBased();
    bool TryEndTurnBased(); // ?!?
    void ProcessTurnBased();
    bool IsCritterTurn( Critter* cr );
    uint GetCritterTurnId();
    uint GetCritterTurnTime();
    void EndCritterTurn();
    void NextCritterTurn();
    void GenerateSequence( Critter* first_cr );

    // Constructor, destructor
public:
    Map();
    ~Map();

    bool  IsNotValid;
    short RefCounter;
    void AddRef() { RefCounter++; }
    void Release()
    {
        RefCounter--;
        if( RefCounter <= 0 ) delete this;
    }
};
typedef std::map<uint, Map*> MapMap;
typedef std::vector<Map*>    MapVec;

class ProtoLocation
{
public:
    bool        IsInit;
    uint16      LocPid;
    std::string Name;

    uint        MaxPlayers;
    UInt16Vec   ProtoMapPids;
    UInt16Vec   AutomapsPids;
    UIntPairVec Entrance;
    int         ScriptBindId;

    uint16      Radius;
    bool        Visible;
    bool        AutoGarbage;
    bool        GeckVisible;

    ProtoLocation() : IsInit( false ) {};
};
typedef std::vector<ProtoLocation> ProtoLocVec;


extern const char* LocationEventFuncName[LOCATION_EVENT_MAX];

class Location
{
public:
    SyncObject Sync;

private:
    MapVec locMaps;

public:
    struct LocData
    {
        uint   LocId;
        uint16 LocPid;
        uint16 WX;
        uint16 WY;
        uint16 Radius;
        bool   Visible;
        bool   GeckVisible;
        bool   AutoGarbage;
        bool   ToGarbage;
        uint   Color;
        uint   Reserved3[59];
    } Data;

    ProtoLocation* Proto;
    volatile int   GeckCount;
    int            FuncId[LOCATION_EVENT_MAX];

    bool       Init( ProtoLocation* proto, uint16 wx, uint16 wy );
    void       Clear( bool full );
    void       Update();
    bool       IsVisible()       { return Data.Visible || (Data.GeckVisible && GeckCount > 0); }
    uint       GetId()           { return Data.LocId; }
    void       SetId( uint _id ) { Data.LocId = _id; }
    uint16     GetPid()          { return Data.LocPid; }
    uint       GetRadius()       { return Data.Radius; }
    MapVec&    GetMapsNoLock()   { return locMaps; };
    void       GetMaps( MapVec& maps, bool lock );
    uint       GetMapsCount() { return (uint)locMaps.size(); }
    Map*       GetMap( uint count );
    bool       GetTransit( Map* from_map, uint& id_map, uint16& hx, uint16& hy, uint8& dir );
    bool       IsAutomaps()                { return !Proto->AutomapsPids.empty(); }
    bool       IsAutomap( uint16 map_pid ) { return std::find( Proto->AutomapsPids.begin(), Proto->AutomapsPids.end(), map_pid ) != Proto->AutomapsPids.end(); }
    UInt16Vec& GetAutomaps()               { return Proto->AutomapsPids; }
    bool       IsCanEnter( uint players_count );

    bool IsNoCrit();
    bool IsNoPlayer();
    bool IsNoNpc();
    bool IsCanDelete();

// Events
private:
    bool PrepareScriptFunc( int num_scr_func );

public:
    void EventFinish( bool to_delete );
    bool EventEnter( ScriptArray* group, uint8 entrance );

public:
    bool  IsNotValid;
    short RefCounter;
    void AddRef() { RefCounter++; }
    void Release()
    {
        RefCounter--;
        if( RefCounter <= 0 ) delete this;
    }
    Location() : RefCounter( 1 ), Proto( NULL ), IsNotValid( false ) { memzero( (void*)&Data, sizeof(Data) ); }
};
typedef std::map<uint, Location*> LocMap;
typedef std::vector<Location*>    LocVec;

#endif // __MAP__
