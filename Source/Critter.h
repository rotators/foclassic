#ifndef __CRITTER__
#define __CRITTER__

#if defined (USE_LIBEVENT)
# include "event2/buffer.h"
# include "event2/bufferevent.h"
#endif

#include "zlib.h"

#include "AI.h"
#include "BufferManager.h"
#include "CritterData.h"
#include "DataMask.h"
#include "Defines.h"
#include "Dialogs.h"
#include "GameOptions.h"
#include "Network.h"
#include "ThreadSync.h"
#include "Types.h"

extern const char* CritterEventFuncName[CRITTER_EVENT_MAX];

// Plane results
#define PLANE_RUN_GLOBAL      (0)
#define PLANE_KEEP            (1)
#define PLANE_DISCARD         (2)

// Client game states
#define STATE_NONE            (0)
#define STATE_CONNECTED       (1)
#define STATE_PLAYING         (2)
#define STATE_TRANSFERRING    (3)

class Critter;
class Client;
class Npc;
class Map;
class Location;
class MapObject;
class GlobalMapGroup;

typedef Critter*                 CritterPtr;
typedef Client*                  ClientPtr;
typedef Npc*                     NpcPtr;

typedef std::map<uint, Critter*> CrMap;
typedef std::map<uint, Client*>  ClMap;
typedef std::map<uint, Npc*>     PcMap;

typedef std::vector<Critter*>    CrVec;
typedef std::vector<Client*>     ClVec;
typedef std::vector<Npc*>        PcVec;

class Critter
{
private:
    Critter( const Critter& ) {}
    Critter& operator=( const Critter& ) {}

public:
    Critter();
    ~Critter();

    CritData         Data;    // Saved
    CritDataExt*     DataExt; // Saved
    SyncObject       Sync;
    bool             CritterIsNpc;
    uint             Flags;
    ScriptString     NameStr;
    C2BitMask        GMapFog;
    bool             IsRuning;
    uint             PrevHexTick;
    uint16           PrevHexX, PrevHexY;
    int              LockMapTransfers;
    Critter*         ThisPtr[MAX_PARAMETERS_ARRAYS];
    uint             AllowedToDownloadMap;

    static bool      ParamsRegEnabled[MAX_PARAMS];
    static uint      ParamsSendMsgLen;
    static uint16    ParamsSendCount;
    static UInt16Vec ParamsSend;
    static bool      ParamsSendEnabled[MAX_PARAMS];
    static int       ParamsSendScript[MAX_PARAMS];
    static int       ParamsChangeScript[MAX_PARAMS];
    static int       ParamsGetScript[MAX_PARAMS];
    static int       ParamsDialogGetScript[MAX_PARAMS];
    static bool      SlotDataSendEnabled[0x100];
    static int       SlotDataSendScript[0x100];
    static uint      ParamsChosenSendMask[MAX_PARAMS];
    static uint      ParametersMin[MAX_PARAMETERS_ARRAYS];
    static uint      ParametersMax[MAX_PARAMETERS_ARRAYS];
    static bool      ParametersOffset[MAX_PARAMETERS_ARRAYS];
    bool             ParamsIsChanged[MAX_PARAMS];
    IntVec           ParamsChanged;
    int              ParamLocked;
    static bool      SlotEnabled[0x100];
    static Item*     SlotEnabledCacheData[0x100];
    static Item*     SlotEnabledCacheDataExt[0x100];

    CritDataExt* GetDataExt();
    void         SetMaps( uint map_id, uint16 map_pid )
    {
        Data.MapId = map_id;
        Data.MapPid = map_pid;
    }
    void SetLexems( const char* lexems );
    bool IsLexems() { return Data.Lexems[0] != 0; }

    int  RunParamsSendScript( int bind_id, uint param_index, Critter* from_cr, Critter* to_cr );
    bool RunSlotDataSendScript( int bind_id, uint8 slot, Item* item, Critter* from_cr, Critter* to_cr );

    // Visible critters and items
    CrVec         VisCr;
    CrVec         VisCrSelf;
    CrMap         VisCrMap;
    CrMap         VisCrSelfMap;
    UIntSet       VisCr1, VisCr2, VisCr3;
    UIntSet       VisItem;
    MutexSpinlock VisItemLocker;
    uint          ViewMapId;
    uint16        ViewMapPid, ViewMapLook, ViewMapHx, ViewMapHy;
    uint8         ViewMapDir;
    uint          ViewMapLocId, ViewMapLocEnt;

    void SyncLockCritters( bool self_critters, bool only_players );
    void ProcessVisibleCritters();
    void ProcessVisibleItems();
    void ViewMap( Map* map, int look, uint16 hx, uint16 hy, int dir );
    void ClearVisible();

    Critter* GetCritSelf( uint crid, bool sync_lock );
    void     GetCrFromVisCr( CrVec& critters, int find_type, bool vis_cr_self, bool sync_lock );

    bool AddCrIntoVisVec( Critter* add_cr );
    bool DelCrFromVisVec( Critter* del_cr );

    bool AddCrIntoVisSet1( uint crid );
    bool AddCrIntoVisSet2( uint crid );
    bool AddCrIntoVisSet3( uint crid );
    bool DelCrFromVisSet1( uint crid );
    bool DelCrFromVisSet2( uint crid );
    bool DelCrFromVisSet3( uint crid );

    bool AddIdVisItem( uint item_id );
    bool DelIdVisItem( uint item_id );
    bool CountIdVisItem( uint item_id );

    // Global map group
public:
    GlobalMapGroup* GroupSelf;
    GlobalMapGroup* GroupMove;

    // Items
protected:
    ItemPtrVec invItems;
    Item*      defItemSlotHand;
    Item*      defItemSlotArmor;

public:
    Item* ItemSlotMain;
    Item* ItemSlotExt;
    Item* ItemSlotArmor;

    void        SyncLockItems();
    bool        SetDefaultItems( ProtoItem* proto_hand, ProtoItem* proto_armor );
    Item*       GetDefaultItemSlotMain() { return defItemSlotHand; }
    void        AddItem( Item*& item, bool send );
    void        SetItem( Item* item );
    void        EraseItem( Item* item, bool send );
    Item*       GetItem( uint item_id, bool skip_hide );
    Item*       GetInvItem( uint item_id, int transfer_type );
    ItemPtrVec& GetItemsNoLock() { return invItems; }
    void        GetInvItems( ItemPtrVec& items, int transfer_type, bool lock );
    Item*       GetItemByPid( uint16 item_pid );
    Item*       GetItemByPidInvPriority( uint16 item_pid );
    Item*       GetItemByPidSlot( uint16 item_pid, int slot );
    Item*       GetAmmoForWeapon( Item* weap );
    Item*       GetAmmo( int caliber );
    Item*       GetItemCar();
    Item*       GetItemSlot( int slot );
    void        GetItemsSlot( int slot, ItemPtrVec& items, bool lock );
    void        GetItemsType( int type, ItemPtrVec& items, bool lock );
    uint        CountItemPid( uint16 item_pid );
    void        TakeDefaultItem( uint8 slot );
    bool        MoveItem( uint8 from_slot, uint8 to_slot, uint item_id, uint count );
    uint        RealCountItems() { return (uint)invItems.size(); }
    uint        CountItems();
    ItemPtrVec& GetInventory()
    {
        SyncLockItems();
        return invItems;
    }
    bool IsHaveGeckItem();

    // Scripts
protected:
    bool PrepareScriptFunc( int num_scr_func );

public:
    int FuncId[CRITTER_EVENT_MAX];
    bool ParseScript( const char* script, bool first_time );
    uint GetScriptId() { return Data.ScriptId; }

    void EventIdle();
    void EventFinish( bool deleted );
    void EventDead( Critter* killer );
    void EventRespawn();
    void EventShowCritter( Critter* cr );
    void EventShowCritter1( Critter* cr );
    void EventShowCritter2( Critter* cr );
    void EventShowCritter3( Critter* cr );
    void EventHideCritter( Critter* cr );
    void EventHideCritter1( Critter* cr );
    void EventHideCritter2( Critter* cr );
    void EventHideCritter3( Critter* cr );
    void EventShowItemOnMap( Item* item, bool added, Critter* dropper );
    void EventChangeItemOnMap( Item* item );
    void EventHideItemOnMap( Item* item, bool removed, Critter* picker );
    bool EventAttack( Critter* target );
    bool EventAttacked( Critter* attacker );
    bool EventStealing( Critter* thief, Item* item, uint count );
    void EventMessage( Critter* from_cr, int num, int val );
    bool EventUseItem( Item* item, Critter* on_critter, Item* on_item, MapObject* on_scenery );
    bool EventUseItemOnMe( Critter* who_use, Item* item );
    bool EventUseSkill( int skill, Critter* on_critter, Item* on_item, MapObject* on_scenery );
    bool EventUseSkillOnMe( Critter* who_use, int skill );
    void EventDropItem( Item* item );
    void EventMoveItem( Item* item, uint8 from_slot );
    void EventKnockout( uint anim2begin, uint anim2idle, uint anim2end, uint lost_ap, uint knock_dist );
    void EventSmthDead( Critter* from_cr, Critter* killer );
    void EventSmthStealing( Critter* from_cr, Critter* thief, bool success, Item* item, uint count );
    void EventSmthAttack( Critter* from_cr, Critter* target );
    void EventSmthAttacked( Critter* from_cr, Critter* attacker );
    void EventSmthUseItem( Critter* from_cr, Item* item, Critter* on_critter, Item* on_item, MapObject* on_scenery );
    void EventSmthUseSkill( Critter* from_cr, int skill, Critter* on_critter, Item* on_item, MapObject* on_scenery );
    void EventSmthDropItem( Critter* from_cr, Item* item );
    void EventSmthMoveItem( Critter* from_cr, Item* item, uint8 from_slot );
    void EventSmthKnockout( Critter* from_cr, uint anim2begin, uint anim2idle, uint anim2end, uint lost_ap, uint knock_dist );
    int  EventPlaneBegin( AIDataPlane* plane, int reason, Critter* some_cr, Item* some_item );
    int  EventPlaneEnd( AIDataPlane* plane, int reason, Critter* some_cr, Item* some_item );
    int  EventPlaneRun( AIDataPlane* plane, int reason, uint& p0, uint& p1, uint& p2 );
    bool EventBarter( Critter* cr_barter, bool attach, uint barter_count );
    bool EventTalk( Critter* cr_talk, bool attach, uint talk_count );
    bool EventGlobalProcess( int type, Item* car, float& x, float& y, float& to_x, float& to_y, float& speed, uint& encounter_descriptor, bool& wait_for_answer );
    bool EventGlobalInvite( Item* car, uint encounter_descriptor, int combat_mode, uint& map_id, uint16& hx, uint16& hy, uint8& dir );
    void EventTurnBasedProcess( Map* map, bool begin_turn );
    void EventSmthTurnBasedProcess( Critter* from_cr, Map* map, bool begin_turn );

    // Knockout
    uint KnockoutAp;

    void ToKnockout( uint anim2begin, uint anim2idle, uint anim2end, uint lost_ap, uint16 knock_hx, uint16 knock_hy );
    void TryUpOnKnockout();

    // Respawn, Dead
    void ToDead( uint anim2, bool send_all );

    // Cached values to avoid synchronization
    uint   CacheValuesNextTick;
    uint16 IntellectCacheValue; // Players talking
    uint   LookCacheValue;      // Critter::GetLook()

    // Break time
private:
    uint startBreakTime;
    uint breakTime;
    uint waitEndTick;

public:
    bool IsFree();
    bool IsBusy();
    void SetBreakTime( uint ms );
    void SetBreakTimeDelta( uint ms );

    void SetWait( uint ms );
    bool IsWait();

    void FullClear();

    // Send
    volatile int DisableSend;
    bool IsSendDisabled() { return DisableSend > 0; }
    void Send_Move( Critter* from_cr, uint move_params );
    void Send_Dir( Critter* from_cr );
    void Send_AddCritter( Critter* cr );
    void Send_RemoveCritter( Critter* cr );
    void Send_LoadMap( Map* map );
    void Send_XY( Critter* cr );
    void Send_AddItemOnMap( Item* item );
    void Send_ChangeItemOnMap( Item* item );
    void Send_EraseItemFromMap( Item* item );
    void Send_AnimateItem( Item* item, uint8 from_frm, uint8 to_frm );
    void Send_AddItem( Item* item );
    void Send_EraseItem( Item* item );
    void Send_ContainerInfo();
    void Send_ContainerInfo( Item* item_cont, uint8 transfer_type, bool open_screen );
    void Send_ContainerInfo( Critter* cr_cont, uint8 transfer_type, bool open_screen );
    void Send_GlobalInfo( uint8 flags );
    void Send_GlobalLocation( Location* loc, bool add );
    void Send_GlobalMapFog( uint16 zx, uint16 zy, uint8 fog );
    void Send_AllParams();
    void Send_Param( uint16 num_param );
    void Send_ParamOther( uint16 num_param, int val );
    void Send_CritterParam( Critter* cr, uint16 num_param, int val );
    void Send_Talk();
    void Send_GameInfo( Map* map );
    void Send_Text( Critter* from_cr, const char* s_str, uint8 how_say );
    void Send_TextEx( uint from_id, const char* s_str, uint16 str_len, uint8 how_say, uint16 intellect, bool unsafe_text );
    void Send_TextMsg( Critter* from_cr, uint str_num, uint8 how_say, uint16 num_msg );
    void Send_TextMsg( uint from_id, uint str_num, uint8 how_say, uint16 num_msg );
    void Send_TextMsgLex( Critter* from_cr, uint num_str, uint8 how_say, uint16 num_msg, const char* lexems );
    void Send_TextMsgLex( uint from_id, uint num_str, uint8 how_say, uint16 num_msg, const char* lexems );
    void Send_Action( Critter* from_cr, int action, int action_ext, Item* item );
    void Send_Knockout( Critter* from_cr, uint anim2begin, uint anim2idle, uint16 knock_hx, uint16 knock_hy );
    void Send_MoveItem( Critter* from_cr, Item* item, uint8 action, uint8 prev_slot );
    void Send_ItemData( Critter* from_cr, uint8 slot, Item* item, bool ext_data );
    void Send_Animate( Critter* from_cr, uint anim1, uint anim2, Item* item, bool clear_sequence, bool delay_play );
    void Send_SetAnims( Critter* from_cr, int cond, uint anim1, uint anim2 );
    void Send_CombatResult( uint* combat_res, uint len );
    void Send_Quest( uint num );
    void Send_Quests( UIntVec& nums );
    void Send_HoloInfo( bool clear, uint16 offset, uint16 count );
    void Send_AutomapsInfo( void* locs_vec, Location* loc );
    void Send_Follow( uint rule, uint8 follow_type, uint16 map_pid, uint follow_wait );
    void Send_Effect( uint16 eff_pid, uint16 hx, uint16 hy, uint16 radius );
    void Send_FlyEffect( uint16 eff_pid, uint from_crid, uint to_crid, uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy );
    void Send_PlaySound( uint crid_synchronize, const char* sound_name );
    void Send_PlaySoundType( uint crid_synchronize, uint8 sound_type, uint8 sound_type_ext, uint8 sound_id, uint8 sound_id_ext );
    void Send_CritterLexems( Critter* cr );

    // Send all
    void SendA_Move( uint move_params );
    void SendA_XY();
    void SendA_Action( int action, int action_ext, Item* item );
    void SendAA_Action( int action, int action_ext, Item* item );
    void SendA_Knockout( uint anim2begin, uint anim2idle, uint16 knock_hx, uint16 knock_hy );
    void SendAA_MoveItem( Item* item, uint8 action, uint8 prev_slot );
    void SendAA_ItemData( Item* item );
    void SendAA_Animate( uint anim1, uint anim2, Item* item, bool clear_sequence, bool delay_play );
    void SendAA_SetAnims( int cond, uint anim1, uint anim2 );
    void SendA_GlobalInfo( GlobalMapGroup* group, uint8 info_flags );
    void SendAA_Text( CrVec& to_cr, const char* str, uint8 how_say, bool unsafe_text );
    void SendAA_Msg( CrVec& to_cr, uint num_str, uint8 how_say, uint16 num_msg );
    void SendAA_MsgLex( CrVec& to_cr, uint num_str, uint8 how_say, uint16 num_msg, const char* lexems );
    void SendA_Dir();
    void SendA_Follow( uint8 follow_type, uint16 map_pid, uint follow_wait );
    void SendA_ParamOther( uint16 num_param, int val );
    void SendA_ParamCheck( uint16 num_param );

    // Chosen data
    void Send_AddAllItems();
    void Send_AllQuests();
    void Send_AllAutomapsInfo();

    bool        IsPlayer()    { return !CritterIsNpc; }
    bool        IsNpc()       { return CritterIsNpc; }
    uint        GetId()       { return Data.Id; }
    uint        GetMap()      { return Data.MapId; }
    uint16      GetProtoMap() { return Data.MapPid; }
    void        RefreshName();
    const char* GetName() { return NameStr.c_str(); }
    const char* GetInfo();
    uint        GetCrType() { return Data.BaseType; }
    uint16      GetHexX()   { return Data.HexX; }
    uint16      GetHexY()   { return Data.HexY; }
    uint8       GetDir()    { return Data.Dir; }
    bool        IsCanWalk();
    bool        IsCanRun();
    uint        GetTimeWalk();
    uint        GetTimeRun();
    uint        GetItemsWeight();
    uint        GetItemsVolume();
    bool        IsOverweight() { return (int)GetItemsWeight() > GetParam( ST_CARRY_WEIGHT ); }
    int         GetFreeWeight();
    int         GetFreeVolume();
    int         GetParam( uint index );
    void        ChangeParam( uint index );
    void        ProcessChangedParams();
    uint        GetFollowCrId() { return Data.Params[ST_FOLLOW_CRIT]; }
    void        SetFollowCrId( uint crid )
    {
        ChangeParam( ST_FOLLOW_CRIT );
        Data.Params[ST_FOLLOW_CRIT] = crid;
    }
    bool IsRawParam( uint index )  { return Data.Params[index] != 0; }
    int  GetRawParam( uint index ) { return Data.Params[index]; }
    bool IsDmgLeg()                { return Data.Params[DAMAGE_RIGHT_LEG] != 0 || Data.Params[DAMAGE_LEFT_LEG] != 0; }
    bool IsDmgTwoLeg()             { return Data.Params[DAMAGE_RIGHT_LEG] != 0 && Data.Params[DAMAGE_LEFT_LEG] != 0; }
    bool IsDmgArm()                { return Data.Params[DAMAGE_RIGHT_ARM] != 0 || Data.Params[DAMAGE_LEFT_ARM] != 0; }
    bool IsDmgTwoArm()             { return Data.Params[DAMAGE_RIGHT_ARM] != 0 && Data.Params[DAMAGE_LEFT_ARM] != 0; }
    void SendMessage( int num, int val, int to );
    int  GetLook();
    uint GetTalkDistance( Critter* talker );
    uint GetAttackDist( Item* weap, int use );
    uint GetUseDist();
    uint GetMultihex();
    bool IsLife()     { return Data.Cond == CRITTER_CONDITION_LIFE; }
    bool IsDead()     { return Data.Cond == CRITTER_CONDITION_DEAD; }
    bool IsKnockout() { return Data.Cond == CRITTER_CONDITION_KNOCKOUT; }
    bool CheckFind( int find_type );
    int  GetRealAp() { return Data.Params[ST_CURRENT_AP]; }
    int  GetAllAp()  { return GetParam( ST_CURRENT_AP ) + GetParam( ST_MOVE_AP ); }
    void SubAp( int val )
    {
        ChangeParam( ST_CURRENT_AP );
        Data.Params[ST_CURRENT_AP] -= val * AP_DIVIDER;
        ApRegenerationTick = 0;
    }
    void SubMoveAp( int val )
    {
        ChangeParam( ST_CURRENT_AP );
        Data.Params[ST_MOVE_AP] -= val;
    }

    // Turn based
    bool IsTurnBased() { return TB_BATTLE_TIMEOUT_CHECK( GetParam( TO_BATTLE ) ); }
    bool CheckMyTurn( Map* map );
    int  GetApCostCritterMove( bool is_run ) { return IsTurnBased() ? GameOpt.TbApCostCritterMove * AP_DIVIDER * (IsDmgTwoLeg() ? 4 : (IsDmgLeg() ? 2 : 1) ) : (GetParam( TO_BATTLE ) ? (is_run ? GameOpt.RtApCostCritterRun : GameOpt.RtApCostCritterWalk) : 0); }
    int  GetApCostMoveItemContainer()        { return IsTurnBased() ? GameOpt.TbApCostMoveItemContainer : GameOpt.RtApCostMoveItemContainer; }
    int  GetApCostMoveItemInventory()
    {
        int val = IsTurnBased() ? GameOpt.TbApCostMoveItemInventory : GameOpt.RtApCostMoveItemInventory;
        if( IsRawParam( PE_QUICK_POCKETS ) ) val /= 2;
        return val;
    }
    int GetApCostPickItem()    { return IsTurnBased() ? GameOpt.TbApCostPickItem : GameOpt.RtApCostPickItem; }
    int GetApCostDropItem()    { return IsTurnBased() ? GameOpt.TbApCostDropItem : GameOpt.RtApCostDropItem; }
    int GetApCostPickCritter() { return IsTurnBased() ? GameOpt.TbApCostPickCritter : GameOpt.RtApCostPickCritter; }
    int GetApCostUseSkill()    { return IsTurnBased() ? GameOpt.TbApCostUseSkill : GameOpt.RtApCostUseSkill; }

    // Timeouts
    void SetTimeout( int timeout, uint game_seconds );
    bool IsTransferTimeouts( bool send );

    // Current container
    uint AccessContainerId;
    uint ItemTransferCount;

    // Home
    uint TryingGoHomeTick;

    void SetHome( uint map_id, uint16 hx, uint16 hy, uint8 dir )
    {
        Data.HomeMap = map_id;
        Data.HomeX = hx;
        Data.HomeY = hy;
        Data.HomeOri = dir;
    }
    uint   GetHomeMap() { return Data.HomeMap; }
    uint16 GetHomeX()   { return Data.HomeX; }
    uint16 GetHomeY()   { return Data.HomeY; }
    uint8  GetHomeOri() { return Data.HomeOri; }
    bool   IsInHome()   { return !GetMap() || (Data.HomeOri == GetDir() && Data.HomeX == GetHexX() && Data.HomeY == GetHexY() && Data.HomeMap == GetMap() ); }

    // Enemy stack
    void     AddEnemyInStack( uint crid );
    bool     CheckEnemyInStack( uint crid );
    void     EraseEnemyInStack( uint crid );
    Critter* ScanEnemyStack();

    // Time events
    struct CrTimeEvent
    {
        uint FuncNum;
        uint Rate;
        uint NextTime;
        int  Identifier;
        void operator=( const CrTimeEvent& r )
        {
            FuncNum = r.FuncNum;
            Rate = r.Rate;
            NextTime = r.NextTime;
            Identifier = r.Identifier;
        }
    };
    typedef std::vector<CrTimeEvent> CrTimeEventVec;
    CrTimeEventVec CrTimeEvents;

    void AddCrTimeEvent( uint func_num, uint rate, uint duration, int identifier );
    void EraseCrTimeEvent( int index );
    void ContinueTimeEvents( int offs_time );

    // Other
    uint GlobalIdleNextTick;
    uint ApRegenerationTick;

    // Reference counter
    bool IsNotValid;
    bool CanBeRemoved;
    long RefCounter;
    void AddRef()  { InterlockedIncrement( &RefCounter ); }
    void Release() { if( !InterlockedDecrement( &RefCounter ) ) Delete(); }
    void Delete();
};

class Client : public Critter
{
public:
    char          Name[UTF8_BUF_SIZE( MAX_NAME )];   // Saved
    char          PassHash[PASS_HASH_SIZE];          // Saved
    uint8         Access;
    uint          LanguageMsg;
    uint          UID[5];
    SOCKET        Sock;
    sockaddr_in   From;
    BufferManager Bin, Bout;
    UInt8Vec      NetIOBuffer;
    int           GameState;
    bool          IsDisconnected;
    uint          DisconnectTick;
    bool          DisableZlib;
    z_stream      Zstrm;
    bool          ZstrmInit;
    uint          ConnectTime;
    uint          LastSendedMapTick;
    char          LastSay[UTF8_BUF_SIZE( MAX_CHAT_MESSAGE )];
    uint          LastSayEqualCount;
    uint          RadioMessageSended;

    #if defined (USE_LIBEVENT)
    struct NetIOArg
    {
        Mutex         Locker;
        Client*       PClient;
        bufferevent*  BEV;
        # if defined (LIBEVENT_TIMEOUTS_WORKAROUND)
        MutexSpinlock BEVLocker;
        # endif
    }* NetIOArgPtr;
    # define BIN_BEGIN( cl_ )     cl_->Bin.Lock()
    # define BIN_END( cl_ )       cl_->Bin.Unlock()
    # define BOUT_BEGIN( cl_ )    cl_->Bout.Lock()
    # if defined (LIBEVENT_TIMEOUTS_WORKAROUND)
    typedef void ( * SendCallback )( bufferevent*, void* );
    static SendCallback SendData;
    #  define BOUT_END( cl_ )                                             \
        cl_->Bout.Unlock();                                               \
        cl_->NetIOArgPtr->BEVLocker.Lock();                               \
        if( cl_->NetIOArgPtr->BEV )                                       \
        {                                                                 \
            bufferevent_lock( cl_->NetIOArgPtr->BEV );                    \
            cl_->NetIOArgPtr->BEVLocker.Unlock();                         \
            (*Client::SendData)(cl_->NetIOArgPtr->BEV, cl_->NetIOArgPtr); \
            bufferevent_unlock( cl_->NetIOArgPtr->BEV );                  \
        }                                                                 \
        else                                                              \
        {                                                                 \
            cl_->NetIOArgPtr->BEVLocker.Unlock();                         \
        }
    # else
    #  define BOUT_END( cl_ )     cl_->Bout.Unlock();
    # endif
    #else // IOCP
    struct NetIOArg
    {
        WSAOVERLAPPED OV;
        Mutex         Locker;
        void*         PClient;
        WSABUF        Buffer;
        long          Operation;
        DWORD         Flags;
        DWORD         Bytes;
    }* NetIOIn, * NetIOOut;
    # define WSA_BUF_SIZE         (4096)
    # define WSAOP_FREE           (0)
    # define WSAOP_SEND           (1)
    # define WSAOP_RECV           (2)
    typedef void ( * SendCallback )( NetIOArg* );
    static SendCallback SendData;
    # define BIN_BEGIN( cl_ )     cl_->Bin.Lock()
    # define BIN_END( cl_ )       cl_->Bin.Unlock()
    # define BOUT_BEGIN( cl_ )    cl_->Bout.Lock()
    # define BOUT_END( cl_ )                                                                                                                         \
        cl_->Bout.Unlock(); if( !cl_->IsOffline() && InterlockedCompareExchange( &cl_->NetIOOut->Operation, WSAOP_SEND, WSAOP_FREE ) == WSAOP_FREE ) \
            (*Client::SendData)(cl_->NetIOOut)
    #endif
    void Shutdown();

public:
    uint        GetIp();
    const char* GetIpStr();
    uint16      GetPort();

public:
    bool IsOnline();
    bool IsOffline();
    void Disconnect();
    void RemoveFromGame();
    uint GetOfflineTime();

    // Ping
private:
    uint pingNextTick;
    bool pingOk;

public:
    bool IsToPing();
    void PingClient();
    void PingOk( uint next_ping );

    // Sends
    void Send_Move( Critter* from_cr, uint move_params );
    void Send_Dir( Critter* from_cr );
    void Send_AddCritter( Critter* cr );
    void Send_RemoveCritter( Critter* cr );
    void Send_LoadMap( Map* map );
    void Send_XY( Critter* cr );
    void Send_AddItemOnMap( Item* item );
    void Send_ChangeItemOnMap( Item* item );
    void Send_EraseItemFromMap( Item* item );
    void Send_AnimateItem( Item* item, uint8 from_frm, uint8 to_frm );
    void Send_AddItem( Item* item );
    void Send_EraseItem( Item* item );
    void Send_ContainerInfo();
    void Send_ContainerInfo( Item* item_cont, uint8 transfer_type, bool open_screen );
    void Send_ContainerInfo( Critter* cr_cont, uint8 transfer_type, bool open_screen );
    void Send_GlobalInfo( uint8 flags );
    void Send_GlobalLocation( Location* loc, bool add );
    void Send_GlobalMapFog( uint16 zx, uint16 zy, uint8 fog );
    void Send_AllParams();
    void Send_Param( uint16 num_param );
    void Send_ParamOther( uint16 num_param, int val );
    void Send_CritterParam( Critter* cr, uint16 num_param, int val );
    void Send_Talk();
    void Send_GameInfo( Map* map );
    void Send_Text( Critter* from_cr, const char* s_str, uint8 how_say );
    void Send_TextEx( uint from_id, const char* s_str, uint16 str_len, uint8 how_say, uint16 intellect, bool unsafe_text );
    void Send_TextMsg( Critter* from_cr, uint str_num, uint8 how_say, uint16 num_msg );
    void Send_TextMsg( uint from_id, uint str_num, uint8 how_say, uint16 num_msg );
    void Send_TextMsgLex( Critter* from_cr, uint num_str, uint8 how_say, uint16 num_msg, const char* lexems );
    void Send_TextMsgLex( uint from_id, uint num_str, uint8 how_say, uint16 num_msg, const char* lexems );
    void Send_Action( Critter* from_cr, int action, int action_ext, Item* item );
    void Send_Knockout( Critter* from_cr, uint anim2begin, uint anim2idle, uint16 knock_hx, uint16 knock_hy );
    void Send_MoveItem( Critter* from_cr, Item* item, uint8 action, uint8 prev_slot );
    void Send_ItemData( Critter* from_cr, uint8 slot, Item* item, bool ext_data );
    void Send_Animate( Critter* from_cr, uint anim1, uint anim2, Item* item, bool clear_sequence, bool delay_play );
    void Send_SetAnims( Critter* from_cr, int cond, uint anim1, uint anim2 );
    void Send_CombatResult( uint* combat_res, uint len );
    void Send_Quest( uint num );
    void Send_Quests( UIntVec& nums );
    void Send_HoloInfo( bool clear, uint16 offset, uint16 count );
    void Send_AutomapsInfo( void* locs_vec, Location* loc );
    void Send_Follow( uint rule, uint8 follow_type, uint16 map_pid, uint follow_wait );
    void Send_Effect( uint16 eff_pid, uint16 hx, uint16 hy, uint16 radius );
    void Send_FlyEffect( uint16 eff_pid, uint from_crid, uint to_crid, uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy );
    void Send_PlaySound( uint crid_synchronize, const char* sound_name );
    void Send_PlaySoundType( uint crid_synchronize, uint8 sound_type, uint8 sound_type_ext, uint8 sound_id, uint8 sound_id_ext );
    void Send_CritterLexems( Critter* cr );
    void Send_MapText( uint16 hx, uint16 hy, uint color, const char* text, uint16 text_len, uint16 intellect, bool unsafe_text );
    void Send_MapTextMsg( uint16 hx, uint16 hy, uint color, uint16 num_msg, uint num_str );
    void Send_MapTextMsgLex( uint16 hx, uint16 hy, uint color, uint16 num_msg, uint num_str, const char* lexems, uint16 lexems_len );
    void Send_UserHoloStr( uint str_num, const char* text, uint16 text_len );
    void Send_PlayersBarter( uint8 barter, uint param, uint param_ext );
    void Send_PlayersBarterSetHide( Item* item, uint count );
    void Send_ShowScreen( int screen_type, uint param, bool need_answer );
    void Send_RunClientScript( const char* func_name, int p0, int p1, int p2, const char* p3, UIntVec& p4 );
    void Send_DropTimers();
    void Send_ViewMap();
    void Send_ItemLexems( Item* item );     // Without checks!
    void Send_ItemLexemsNull( Item* item ); // Without checks!
    void Send_CheckUIDS();
    void Send_SomeItem( Item* item );       // Without checks!

    // Locations
    bool CheckKnownLocById( uint loc_id );
    bool CheckKnownLocByPid( uint16 loc_pid );
    void AddKnownLoc( uint loc_id );
    void EraseKnownLoc( uint loc_id );

    // Players barter
    bool BarterOffer;
    bool BarterHide;
    uint BarterOpponent;
    struct BarterItem
    {
        uint Id;
        uint Pid;
        uint Count;
        bool operator==( uint id ) { return Id == id; }
        BarterItem( uint id, uint pid, uint count ) : Id( id ), Pid( pid ), Count( count ) {}
    };
    typedef std::vector<BarterItem> BarterItemVec;
    BarterItemVec BarterItems;

    Client*     BarterGetOpponent( uint opponent_id );
    void        BarterEnd();
    void        BarterRefresh( Client* opponent );
    BarterItem* BarterGetItem( uint item_id );
    void        BarterEraseItem( uint item_id );

    // Timers
    uint LastSendScoresTick, LastSendCraftTick, LastSendEntrancesTick, LastSendEntrancesLocId;
    void DropTimers( bool send );

    // Dialogs
private:
    uint talkNextTick;

public:
    Talking Talk;
    bool IsTalking() { return Talk.TalkType != TALK_NONE; }
    void ProcessTalk( bool force );
    void CloseTalk();

    // Screen callback
    int ScreenCallbackBindId;

    Client();
    ~Client();
};

class Npc : public Critter
{
    // Bag
public:
    uint NextRefreshBagTick;
    bool IsNeedRefreshBag();
    void RefreshBag();

    // AI plane
private:
    AIDataPlaneVec aiPlanes;

public:
    bool AddPlane( int reason, AIDataPlane* plane, bool is_child, Critter* some_cr = NULL, Item* some_item = NULL );
    void NextPlane( int reason, Critter* some_cr = NULL, Item* some_item = NULL );
    bool RunPlane( int reason, uint& r0, uint& r1, uint& r2 );
    bool IsPlaneAviable( int plane_type );
    bool IsCurPlane( int plane_type )
    {
        AIDataPlane* p = GetCurPlane();
        return p ? p->Type == plane_type : false;
    }
    AIDataPlane*    GetCurPlane() { return aiPlanes.size() ? aiPlanes[0]->GetCurPlane() : NULL; }
    AIDataPlaneVec& GetPlanes()   { return aiPlanes; }
    void            DropPlanes();
    void            SetBestCurPlane();
    bool            IsNoPlanes() { return aiPlanes.empty(); }

    // Dialogs
public:
    uint GetTalkedPlayers();
    bool IsTalkedPlayers();
    uint GetBarterPlayers();
    bool IsFreeToTalk();
    bool IsPlaneNoTalk();

    // Target
public:
    void MoveToHex( int reason, uint16 hx, uint16 hy, uint8 ori, bool is_run, uint8 cut );
    void SetTarget( int reason, Critter* target, int min_hp, bool is_gag );

    // Params
    uint16 GetProtoId( void ) const { return Data.ProtoId; }

    uint Reserved;

    Npc();
    ~Npc();
};

#endif // __CRITTER__
