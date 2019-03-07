#ifndef __CLIENT__
#define __CLIENT__

#ifndef FO_D3D
# include "theoradec.h" // Video
#endif
#include "zlib.h"

#include "scriptarray.h"
#include "scriptstring.h"

#include "BufferManager.h"
#include "DataMask.h"
#include "Defines.h"
#include "HexManager.h"
#include "IniParser.h"
#include "MsgFiles.h"
#include "Network.h"
#include "QuestManager.h"
#include "Random.h"
#include "Types.h"

class FOClient
{
public:
    static FOClient* Self;
    FOClient();
    bool Init();
    void Finish();
    void TryExit();
    bool IsScroll();
    void ProcessMouseScroll();
    void ProcessKeybScroll( bool down, uint8 dik );
    void DropScroll();
    bool IsCurInWindow();
    void FlashGameWindow();
    int  MainLoop();
    void NetDisconnect();

    uint16      NetState;
    bool        Active;
    uint*       UID1;
    std::string Password;
    HexManager  HexMngr;

    int         ShowScreenType;
    uint        ShowScreenParam;
    bool        ShowScreenNeedAnswer;

    int         ScreenModeMain;
    void ShowMainScreen( int new_screen );
    int  GetMainScreen()                  { return ScreenModeMain; }
    bool IsMainScreen( int check_screen ) { return check_screen == ScreenModeMain; }

    void ShowScreen( int screen, int p0 = -1, int p1 = 0, int p2 = 0 );
    void HideScreen( int screen, int p0 = -1, int p1 = 0, int p2 = 0 );
    int  GetActiveScreen( IntVec** screens = NULL );
    bool IsScreenPresent( int screen );
    void RunScreenScript( bool show, int screen, int p0, int p1, int p2 );

    int CurMode, CurModeLast;
    void SetCurMode( int new_cur );
    int  GetCurMode() { return CurMode; };
    void SetCurCastling( int cur1, int cur2 );
    void SetLastCurMode();
    bool IsCurMode( int check_cur ) { return check_cur == CurMode;  }
    void SetCurPos( int x, int y );

    void ParseKeyboard();
    void ParseMouse();

    char*         ComBuf;
    uint          ComLen;
    BufferManager Bin;
    BufferManager Bout;
    z_stream      ZStream;
    bool          ZStreamOk;
    uint          BytesReceive, BytesRealReceive, BytesSend;
    sockaddr_in   SockAddr, ProxyAddr;
    SOCKET        Sock;
    fd_set        SockSet, SockSetErr;
    uint*         UID0;
    bool          UIDFail;
    Item          SomeItem;
    bool          IsConnected;
    bool          InitNetBegin;
    int           InitNetReason;

    bool InitNet();
    bool FillSockAddr( sockaddr_in& saddr, const char* host, uint16 port );
    bool NetConnect();
    void ParseSocket();
    int  NetInput( bool unpack );
    bool NetOutput();
    void NetProcess();

    void Net_SendLogIn( const char* name, const char* pass );
    void Net_SendCreatePlayer( CritterCl* newcr );
    void Net_SendSaveLoad( bool save, const char* fname, UInt8Vec* pic_data );
    void Net_SendUseSkill( uint16 skill, CritterCl* cr );
    void Net_SendUseSkill( uint16 skill, ItemHex* item );
    void Net_SendUseSkill( uint16 skill, Item* item );
    void Net_SendUseItem( uint8 ap, uint item_id, uint16 item_pid, uint8 rate, uint8 target_type, uint target_id, uint16 target_pid, uint param );
    void Net_SendPickItem( uint16 targ_x, uint16 targ_y, uint16 pid );
    void Net_SendPickCritter( uint crid, uint8 pick_type );
    void Net_SendChangeItem( uint8 ap, uint item_id, uint8 from_slot, uint8 to_slot, uint count );
    void Net_SendItemCont( uint8 transfer_type, uint cont_id, uint item_id, uint count, uint8 take_flags );
    void Net_SendRateItem();
    void Net_SendSortValueItem( Item* item );
    void Net_SendTalk( uint8 is_npc, uint id_to_talk, uint8 answer );
    void Net_SendSayNpc( uint8 is_npc, uint id_to_talk, const char* str );
    void Net_SendBarter( uint npc_id, ItemVec& cont_sale, ItemVec& cont_buy );
    void Net_SendGetGameInfo();
    void Net_SendGiveGlobalInfo( uint8 info_flags );
    void Net_SendRuleGlobal( uint8 command, uint param1 = 0, uint param2 = 0 );
    void Net_SendGiveMap( bool automap, uint16 map_pid, uint loc_id, uint tiles_hash, uint walls_hash, uint scen_hash );
    void Net_SendLoadMapOk();
    void Net_SendCommand( char* str );
    void Net_SendText( const char* send_str, uint8 how_say );
    void Net_SendDir();
    void Net_SendMove( UInt8Vec steps );
    void Net_SendLevelUp( uint16 perk_up );
    void Net_SendCraftAsk( UIntVec numbers );
    void Net_SendCraft( uint craft_num );
    void Net_SendPing( uint8 ping );
    void Net_SendPlayersBarter( uint8 barter, uint param, uint param_ext );
    void Net_SendScreenAnswer( uint answer_i, const char* answer_s );
    void Net_SendGetScores();
    void Net_SendSetUserHoloStr( Item* holodisk, const char* title, const char* text );
    void Net_SendGetUserHoloStr( uint str_num );
    void Net_SendCombat( uint8 type, int val );
    void Net_SendRunScript( bool unsafe, const char* func_name, int p0, int p1, int p2, const char* p3, UIntVec& p4 );
    void Net_SendKarmaVoting( uint crid, bool val_up );
    void Net_SendRefereshMe();

    void Net_OnLoginSuccess();
    void Net_OnAddCritter( bool is_npc );
    void Net_OnRemoveCritter();
    void Net_OnText();
    void Net_OnTextMsg( bool with_lexems );
    void Net_OnMapText();
    void Net_OnMapTextMsg();
    void Net_OnMapTextMsgLex();
    void Net_OnAddItemOnMap();
    void Net_OnChangeItemOnMap();
    void Net_OnEraseItemFromMap();
    void Net_OnAnimateItem();
    void Net_OnCombatResult();
    void Net_OnEffect();
    void Net_OnFlyEffect();
    void Net_OnPlaySound( bool by_type );
    void Net_OnPing();
    void Net_OnCheckUID0();

    void Net_OnCritterDir();
    void Net_OnCritterMove();
    void Net_OnSomeItem();
    void Net_OnCritterAction();
    void Net_OnCritterKnockout();
    void Net_OnCritterMoveItem();
    void Net_OnCritterItemData();
    void Net_OnCritterAnimate();
    void Net_OnCritterSetAnims();
    void Net_OnCritterParam();
    void Net_OnCheckUID1();

    void Net_OnCritterXY();
    void Net_OnChosenParams();
    void Net_OnChosenParam();
    void Net_OnCraftAsk();
    void Net_OnCraftResult();
    void Net_OnChosenClearItems();
    void Net_OnChosenAddItem();
    void Net_OnChosenEraseItem();
    void Net_OnChosenTalk();
    void Net_OnCheckUID2();

    void Net_OnGameInfo();
    void Net_OnLoadMap();
    void Net_OnMap();
    void Net_OnGlobalInfo();
    void Net_OnGlobalEntrances();
    void Net_OnContainerInfo();
    void Net_OnFollow();
    void Net_OnPlayersBarter();
    void Net_OnPlayersBarterSetHide();
    void Net_OnShowScreen();
    void Net_OnRunClientScript();
    void Net_OnDropTimers();
    void Net_OnCritterLexems();
    void Net_OnItemLexems();
    void Net_OnCheckUID3();

    void Net_OnMsgData();
    void Net_OnProtoItemData();

    void Net_OnQuest( bool many );
    void Net_OnHoloInfo();
    void Net_OnScores();
    void Net_OnUserHoloStr();
    void Net_OnAutomapsInfo();
    void Net_OnCheckUID4();
    void Net_OnViewMap();

    void OnText( const char* str, uint crid, int how_say, uint16 intellect );
    void OnMapText( const char* str, uint16 hx, uint16 hy, uint color, uint16 intellect );

    void WaitPing();

    bool SaveLogFile();
    bool SaveScreenshot();

    UInt8Vec MoveDirs;
    uint16   MoveLastHx, MoveLastHy;

    uint     PingTick, PingCallTick;

    // Sound
    void SoundProcess();

    // MSG File
    LanguagePack CurLang;
    FOMsg*       MsgText, * MsgDlg, * MsgItem, * MsgGame, * MsgGM, * MsgCombat, * MsgQuest, * MsgHolo, * MsgUserHolo, * MsgCraft, * MsgInternal;

    const char* GetHoloText( uint str_num );
    const char* FmtGameText( uint str_num, ... );
    const char* FmtCombatText( uint str_num, ... );

    const char* FmtGenericDesc( int desc_type, int& ox, int& oy );
    const char* FmtCritLook( CritterCl* cr, int look_type );
    const char* FmtItemLook( Item* item, int look_type );

    // Intellect text
    PCharPairVec IntellectWords;
    PCharPairVec IntellectSymbols;

    void ParseIntellectWords( char* words, PCharPairVec& text );
    auto FindIntellectWord( const char* word, PCharPairVec& text, Randomizer& rnd )->PCharPairVec::iterator;
    void FmtTextIntellect( char* str, uint16 intellect );

    #define SMTH_NONE         (0)
    #define SMTH_CRITTER      (1)
    #define SMTH_ITEM         (3)
    #define SMTH_CONT_ITEM    (4)
    class SmthSelected
    {
private:
        int  smthType;
        uint smthId;
        int  smthParam;
public:
        SmthSelected() { Clear(); }
        bool operator!=( const SmthSelected& r ) { return smthType != r.smthType || smthId != r.smthId || smthParam != r.smthParam; }
        bool operator==( const SmthSelected& r ) { return smthType == r.smthType && smthId == r.smthId && smthParam == r.smthParam; }
        void Clear()
        {
            smthType = SMTH_NONE;
            smthId = 0;
        }
        bool IsSmth()     { return smthType != SMTH_NONE; }
        bool IsCritter()  { return smthType == SMTH_CRITTER; }
        bool IsItem()     { return smthType == SMTH_ITEM; }
        bool IsContItem() { return smthType == SMTH_CONT_ITEM; }
        void SetCritter( uint id )
        {
            smthType = SMTH_CRITTER;
            smthId = id;
            smthParam = 0;
        }
        void SetItem( uint id )
        {
            smthType = SMTH_ITEM;
            smthId = id;
            smthParam = 0;
        }
        void SetContItem( uint id, int cont_type )
        {
            smthType = SMTH_CONT_ITEM;
            smthId = id;
            smthParam = cont_type;
        }
        uint GetId()    { return smthId; }
        int  GetParam() { return smthParam; }
    };
    SmthSelected TargetSmth;
    Item* GetTargetContItem();

    struct ActionEvent
    {
        uint Type;
        uint Param[6];
        bool operator==( const ActionEvent& r ) { return Type == r.Type && Param[0] == r.Param[0] && Param[1] == r.Param[1] && Param[2] == r.Param[2] && Param[3] == r.Param[3] && Param[4] == r.Param[4] && Param[4] == r.Param[5]; }
        ActionEvent() {}
        ActionEvent( uint type, uint param0, uint param1, uint param2, uint param3, uint param4, uint param5 ) : Type( type )
        {
            Param[0] = param0;
            Param[1] = param1;
            Param[2] = param2;
            Param[3] = param3;
            Param[4] = param4;
            Param[5] = param5;
        }
        ActionEvent( const ActionEvent& r ) { memcpy( this, &r, sizeof(ActionEvent) ); }
    };
    typedef std::vector<ActionEvent> ActionEventVec;

    ActionEventVec ChosenAction;
    void AddAction( bool to_front, ActionEvent act );
    void SetAction( uint type_action, uint param0 = 0, uint param1 = 0, uint param2 = 0, uint param3 = 0, uint param4 = 0, uint param5 = 0 );
    void SetAction( ActionEvent act );
    void AddActionBack( uint type_action, uint param0 = 0, uint param1 = 0, uint param2 = 0, uint param3 = 0, uint param4 = 0, uint param5 = 0 );
    void AddActionBack( ActionEvent act );
    void AddActionFront( uint type_action, uint param0 = 0, uint param1 = 0, uint param2 = 0, uint param3 = 0, uint param4 = 0, uint param5 = 0 );
    void EraseFrontAction();
    void EraseBackAction();
    bool IsAction( uint type_action );
    void ChosenChangeSlot();
    void CrittersProcess();
    void TryPickItemOnGround();

/************************************************************************/
/* Video                                                                */
/************************************************************************/
    #ifndef FO_D3D
    struct ShowVideo
    {
        std::string FileName;
        std::string SoundName;
        bool        CanStop;
    };
    typedef std::vector<ShowVideo> ShowVideoVec;

    struct VideoContext
    {
        th_dec_ctx*     Context;
        th_info         VideoInfo;
        th_comment      Comment;
        th_setup_info*  SetupInfo;
        th_ycbcr_buffer ColorBuffer;
        ogg_sync_state  SyncState;
        ogg_packet      Packet;
        struct StreamStates
        {
            static const uint COUNT = 10;
            ogg_stream_state  Streams[COUNT];
            bool              StreamsState[COUNT];
            int               MainIndex;
        } SS;
        FileManager  RawData;
        RenderTarget RT;
        uint         CurFrame;
        double       StartTime;
        double       AverageRenderTime;
    };

    ShowVideoVec  ShowVideos;
    std::string   MusicAfterVideo;
    int           MusicVolumeRestore;
    VideoContext* CurVideo;

    int  VideoDecodePacket();
    void RenderVideo();
    bool IsVideoPlayed()  { return !ShowVideos.empty(); }
    bool IsCanStopVideo() { return ShowVideos.size() && ShowVideos[0].CanStop; }
    void AddVideo( const char* video_name, bool can_stop, bool clear_sequence );
    void PlayVideo();
    void NextVideo();
    void StopVideo();
    #endif

/************************************************************************/
/* Animation                                                            */
/************************************************************************/
    struct IfaceAnim
    {
        AnyFrames* Frames;
        uint16     Flags;
        uint       CurSpr;
        uint       LastTick;
        int        ResType;

        IfaceAnim( AnyFrames* frm, int res_type );
    };
    typedef std::vector<IfaceAnim*> IfaceAnimVec;

    #define ANIMRUN_TO_END            (0x0001)
    #define ANIMRUN_FROM_END          (0x0002)
    #define ANIMRUN_CYCLE             (0x0004)
    #define ANIMRUN_STOP              (0x0008)
    #define ANIMRUN_SET_FRM( frm )    ( (uint( uint8( (frm) + 1 ) ) ) << 16 )

    IfaceAnimVec Animations;

    uint       AnimLoad( uint name_hash, uint8 dir, int res_type );
    uint       AnimLoad( const char* fname, int path_type, int res_type );
    uint       AnimGetCurSpr( uint anim_id );
    uint       AnimGetCurSprCnt( uint anim_id );
    uint       AnimGetSprCount( uint anim_id );
    AnyFrames* AnimGetFrames( uint anim_id );
    void       AnimRun( uint anim_id, uint flags );
    void       AnimProcess();
    void       AnimFree( int res_type );

/************************************************************************/
/* Screen effects                                                       */
/************************************************************************/
    struct ScreenEffect
    {
        uint BeginTick;
        uint Time;
        uint StartColor;
        uint EndColor;
        ScreenEffect( uint begin_tick, uint time, uint col, uint end_col ) : BeginTick( begin_tick ), Time( time ), StartColor( col ), EndColor( end_col ) {}
    };
    typedef std::vector<ScreenEffect> ScreenEffectVec;

    // Fading
    ScreenEffectVec ScreenEffects;
    // Quake
    int             ScreenOffsX, ScreenOffsY;
    float           ScreenOffsXf, ScreenOffsYf, ScreenOffsStep;
    uint            ScreenOffsNextTick;
    // Mirror
    Texture*        ScreenMirrorTexture;
    int             ScreenMirrorX, ScreenMirrorY;
    uint            ScreenMirrorEndTick;
    bool            ScreenMirrorStart;

    void ScreenFadeIn()  { ScreenFade( 1000, COLOR_ARGB( 0, 0, 0, 0 ), COLOR_ARGB( 255, 0, 0, 0 ), false ); }
    void ScreenFadeOut() { ScreenFade( 1000, COLOR_ARGB( 255, 0, 0, 0 ), COLOR_ARGB( 0, 0, 0, 0 ), false ); }
    void ScreenFade( uint time, uint from_color, uint to_color, bool push_back );
    void ScreenQuake( int noise, uint time );
    void ScreenMirror();
    void ProcessScreenEffectFading();
    void ProcessScreenEffectQuake();
    void ProcessScreenEffectMirror();

/************************************************************************/
/* Scripting                                                            */
/************************************************************************/
    bool        ReloadScripts( bool from_init = false );
    int         ScriptGetHitProc( CritterCl* cr, int hit_location );
    void        DrawIfaceLayer( uint layer );
    static bool PragmaCallbackCrData( const char* text );

    struct SScriptFunc
    {
        static int* DataRef_Index( CritterClPtr& cr, uint index );
        static int  DataVal_Index( CritterClPtr& cr, uint index );

        static bool Crit_IsChosen( CritterCl* cr );
        static bool Crit_IsPlayer( CritterCl* cr );
        static bool Crit_IsNpc( CritterCl* cr );
        static bool Crit_IsLife( CritterCl* cr );
        static bool Crit_IsKnockout( CritterCl* cr );
        static bool Crit_IsDead( CritterCl* cr );
        static bool Crit_IsFree( CritterCl* cr );
        static bool Crit_IsBusy( CritterCl* cr );

        static bool       Crit_IsAnim3d( CritterCl* cr );
        static bool       Crit_IsAnimAviable( CritterCl* cr, uint anim1, uint anim2 );
        static bool       Crit_IsAnimPlaying( CritterCl* cr );
        static uint       Crit_GetAnim1( CritterCl* cr );
        static void       Crit_Animate( CritterCl* cr, uint anim1, uint anim2 );
        static void       Crit_AnimateEx( CritterCl* cr, uint anim1, uint anim2, Item* item );
        static void       Crit_ClearAnim( CritterCl* cr );
        static void       Crit_Wait( CritterCl* cr, uint ms );
        static uint       Crit_ItemsCount( CritterCl* cr );
        static uint       Crit_ItemsWeight( CritterCl* cr );
        static uint       Crit_ItemsVolume( CritterCl* cr );
        static uint       Crit_CountItem( CritterCl* cr, uint16 proto_id );
        static uint       Crit_CountItemByType( CritterCl* cr, uint8 type );
        static Item*      Crit_GetItem( CritterCl* cr, uint16 proto_id, int slot );
        static uint       Crit_GetItems( CritterCl* cr, int slot, ScriptArray* items );
        static uint       Crit_GetItemsByType( CritterCl* cr, int type, ScriptArray* items );
        static ProtoItem* Crit_GetSlotProto( CritterCl* cr, int slot, uint8& mode );
        static void       Crit_SetVisible( CritterCl* cr, bool visible );
        static bool       Crit_GetVisible( CritterCl* cr );
        static void       Crit_set_ContourColor( CritterCl* cr, uint value );
        static uint       Crit_get_ContourColor( CritterCl* cr );
        static uint       Crit_GetMultihex( CritterCl* cr );
        static bool       Crit_IsTurnBasedTurn( CritterCl* cr );
        static void       Crit_GetNameTextInfo( CritterCl* cr, bool& nameVisible, int& x, int& y, int& w, int& h, int& lines );

        static bool   Item_IsStackable( Item* item );
        static bool   Item_IsDeteriorable( Item* item );
        static uint   Item_GetScriptId( Item* item );
        static uint8  Item_GetType( Item* item );
        static uint16 Item_GetProtoId( Item* item );
        static uint   Item_GetCount( Item* item );
        static bool   Item_GetMapPosition( Item* item, uint16& hx, uint16& hy );
        static void   Item_Animate( Item* item, uint8 from_frame, uint8 to_frame );
        static Item*  Item_GetChild( Item* item, uint childIndex );

        static bool  Global_get___ConsoleActive();
        static bool  Global_get___WorldmapActive();
        static bool  Global_get___WorldmapWait();
        static float Global_get___WorldmapZoom();
        static int   Global_get___WorldmapOffsetX();
        static int   Global_get___WorldmapOffsetY();
        static int   Global_get___WorldmapGroupCurX();
        static int   Global_get___WorldmapGroupCurY();
        static int   Global_get___WorldmapGroupToX();
        static int   Global_get___WorldmapGroupToY();
        static float Global_get___WorldmapGroupSpeed();

        static CritterCl*    Global_GetChosen();
        static uint          Global_GetChosenActions( ScriptArray* actions );
        static void          Global_SetChosenActions( ScriptArray* actions );
        static Item*         Global_GetItem( uint item_id );
        static uint          Global_GetCrittersDistantion( CritterCl* cr1, CritterCl* cr2 );
        static CritterCl*    Global_GetCritter( uint critter_id );
        static uint          Global_GetCritters( uint16 hx, uint16 hy, uint radius, int find_type, ScriptArray* critters );
        static uint          Global_GetCrittersByPids( uint16 pid, int find_type, ScriptArray* critters );
        static uint          Global_GetCrittersInPath( uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy, float angle, uint dist, int find_type, ScriptArray* critters );
        static uint          Global_GetCrittersInPathBlock( uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy, float angle, uint dist, int find_type, ScriptArray* critters, uint16& pre_block_hx, uint16& pre_block_hy, uint16& block_hx, uint16& block_hy );
        static void          Global_GetHexInPath( uint16 from_hx, uint16 from_hy, uint16& to_hx, uint16& to_hy, float angle, uint dist );
        static uint          Global_GetPathLengthHex( uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy, uint cut );
        static uint          Global_GetPathLengthCr( CritterCl* cr, uint16 to_hx, uint16 to_hy, uint cut );
        static void          Global_FlushScreen( uint from_color, uint to_color, uint ms );
        static void          Global_QuakeScreen( uint noise, uint ms );
        static bool          Global_PlaySound( ScriptString& sound_name );
        static bool          Global_PlaySoundType( uint8 sound_type, uint8 sound_type_ext, uint8 sound_id, uint8 sound_id_ext );
        static bool          Global_PlayMusic( ScriptString& music_name, uint pos, uint repeat );
        static void          Global_PlayVideo( ScriptString& video_name, bool can_stop );
        static bool          Global_IsTurnBased();
        static uint          Global_GetTurnBasedTime();
        static uint16        Global_GetCurrentMapPid();
        static uint          Global_GetMessageFilters( ScriptArray* filters );
        static void          Global_SetMessageFilters( ScriptArray* filters );
        static void          Global_Message( ScriptString& msg );
        static void          Global_MessageType( ScriptString& msg, int type );
        static void          Global_MessageMsg( int text_msg, uint str_num );
        static void          Global_MessageMsgType( int text_msg, uint str_num, int type );
        static void          Global_MapMessage( ScriptString& text, uint16 hx, uint16 hy, uint ms, uint color, bool fade, int ox, int oy );
        static ScriptString* Global_GetMsgStr( int text_msg, uint str_num );
        static ScriptString* Global_GetMsgStrSkip( int text_msg, uint str_num, uint skip_count );
        static uint          Global_GetMsgStrNumUpper( int text_msg, uint str_num );
        static uint          Global_GetMsgStrNumLower( int text_msg, uint str_num );
        static uint          Global_GetMsgStrCount( int text_msg, uint str_num );
        static bool          Global_IsMsgStr( int text_msg, uint str_num );
        static ScriptString* Global_ReplaceTextStr( ScriptString& text, ScriptString& replace, ScriptString& str );
        static ScriptString* Global_ReplaceTextInt( ScriptString& text, ScriptString& replace, int i );
        static ScriptString* Global_FormatTags( ScriptString& text, ScriptString* lexems );
        static int           Global_GetSomeValue( int var );
        static void          Global_MoveScreen( uint16 hx, uint16 hy, uint speed );
        static void          Global_LockScreenScroll( CritterCl* cr );
        static int           Global_GetFog( uint16 zone_x, uint16 zone_y );
        static void          Global_RefreshItemsCollection( int collection );
        static int           Global_GetScroll( int scroll_element );
        static void          Global_SetScroll( int scroll_element, int value );
        static uint          Global_GetDayTime( uint day_part );
        static void          Global_GetDayColor( uint day_part, uint8& r, uint8& g, uint8& b );

        static ScriptString* Global_GetLastError();
        static void          Global_Log( ScriptString& text );
        static ProtoItem*    Global_GetProtoItem( uint16 proto_id );
        static uint          Global_GetDistantion( uint16 hex_x1, uint16 hex_y1, uint16 hex_x2, uint16 hex_y2 );
        static uint8         Global_GetDirection( uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy );
        static uint8         Global_GetOffsetDir( uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy, float offset );
        static uint          Global_GetFullSecond( uint16 year, uint16 month, uint16 day, uint16 hour, uint16 minute, uint16 second );
        static void          Global_GetGameTime( uint full_second, uint16& year, uint16& month, uint16& day, uint16& day_of_week, uint16& hour, uint16& minute, uint16& second );
        static bool          Global_StrToInt( ScriptString* text, int& result );
        static bool          Global_StrToFloat( ScriptString* text, float& result );
        static uint          Global_GetTick();
        static void          Global_GetTime( uint16& year, uint16& month, uint16& day, uint16& day_of_week, uint16& hour, uint16& minute, uint16& second, uint16& milliseconds );
        static bool          Global_SetParameterGetBehaviour( uint index, ScriptString& func_name );
        static bool          Global_SetParameterChangeBehaviour( uint index, ScriptString& func_name );
        static void          Global_AllowSlot( uint8 index, ScriptString& ini_option );
        static uint          Global_DecodeUTF8( ScriptString& text, uint& length );
        static ScriptString* Global_EncodeUTF8( uint ucs );
        static void          Global_SetRegistrationParam( uint index, bool enabled );
        static uint          Global_GetAngelScriptProperty( int property );
        static bool          Global_SetAngelScriptProperty( int property, uint value );
        static uint          Global_GetStrHash( ScriptString* str );
        static bool          Global_LoadDataFile( ScriptString& dat_name );
        static int           Global_GetConstantValue( int const_collection, ScriptString* name );
        static ScriptString* Global_GetConstantName( int const_collection, int value );
        static void          Global_AddConstant( int const_collection, ScriptString* name, int value );
        static bool          Global_LoadConstants( int const_collection, ScriptString* file_name, int path_type );
        static bool          Global_IsCritterCanWalk( uint cr_type );
        static bool          Global_IsCritterCanRun( uint cr_type );
        static bool          Global_IsCritterCanRotate( uint cr_type );
        static bool          Global_IsCritterCanAim( uint cr_type );
        static bool          Global_IsCritterCanArmor( uint cr_type );
        static bool          Global_IsCritterAnim1( uint cr_type, uint index );
        static int           Global_GetCritterAnimType( uint cr_type );
        static uint          Global_GetCritterAlias( uint cr_type );
        static ScriptString* Global_GetCritterTypeName( uint cr_type );
        static ScriptString* Global_GetCritterSoundName( uint cr_type );
        static void          Global_RunServerScript( ScriptString& func_name, int p0, int p1, int p2, ScriptString* p3, ScriptArray* p4 );
        static void          Global_RunServerScriptUnsafe( ScriptString& func_name, int p0, int p1, int p2, ScriptString* p3, ScriptArray* p4 );

        static uint Global_LoadSprite( ScriptString& spr_name, int path_index );
        static uint Global_LoadSpriteHash( uint name_hash, uint8 dir );
        static int  Global_GetSpriteWidth( uint spr_id, int spr_index );
        static int  Global_GetSpriteHeight( uint spr_id, int spr_index );
        static uint Global_GetSpriteCount( uint spr_id );
        static void Global_GetTextInfo( ScriptString& text, int w, int h, int font, int flags, int& tw, int& th, int& lines );
        static void Global_DrawSprite( uint spr_id, int spr_index, int x, int y, uint color );
        static void Global_DrawSpriteOffs( uint spr_id, int spr_index, int x, int y, uint color, bool offs );
        static void Global_DrawSpritePattern( uint spr_id, int spr_index, int x, int y, int w, int h, int spr_width, int spr_height, uint color );
        static void Global_DrawSpriteSize( uint spr_id, int spr_index, int x, int y, int w, int h, bool scratch, bool center, uint color );
        static void Global_DrawSpriteSizeOffs( uint spr_id, int spr_index, int x, int y, int w, int h, bool scratch, bool center, uint color, bool offs );
        static void Global_DrawText( ScriptString& text, int x, int y, int w, int h, uint color, int font, int flags );
        static void Global_DrawPrimitive( int primitive_type, ScriptArray& data );
        static void Global_DrawMapSprite( uint16 hx, uint16 hy, uint16 proto_id, uint spr_id, int spr_index, int ox, int oy );
        static void Global_DrawCritter2d( uint crtype, uint anim1, uint anim2, uint8 dir, int l, int t, int r, int b, bool scratch, bool center, uint color );
        static void Global_DrawCritter3d( uint instance, uint crtype, uint anim1, uint anim2, ScriptArray* layers, ScriptArray* position, uint color );

        static void          Global_ShowScreen( int screen, int p0, int p1, int p2 );
        static void          Global_HideScreen( int screen, int p0, int p1, int p2 );
        static void          Global_GetHardcodedScreenPos( int screen, int& x, int& y );
        static void          Global_DrawHardcodedScreen( int screen );
        static bool          Global_GetHexPos( uint16 hx, uint16 hy, int& x, int& y );
        static bool          Global_GetMonitorHex( int x, int y, uint16& hx, uint16& hy, bool ignore_interface );
        static Item*         Global_GetMonitorItem( int x, int y, bool ignore_interface );
        static CritterCl*    Global_GetMonitorCritter( int x, int y, bool ignore_interface );
        static uint16        Global_GetMapWidth();
        static uint16        Global_GetMapHeight();
        static int           Global_GetCurrentCursor();
        static int           Global_GetLastCursor();
        static void          Global_ChangeCursor( int cursor );
        static void          Global_MoveHexByDir( uint16& hx, uint16& hy, uint8 dir, uint steps );
        static bool          Global_AppendIfaceIni( ScriptString& ini_name );
        static ScriptString* Global_GetIfaceIniStr( ScriptString& key );
        static bool          Global_Load3dFile( ScriptString& fname, int path_type );
        static void          Global_WaitPing();
        static bool          Global_LoadFont( int font, ScriptString& font_fname );
        static void          Global_SetDefaultFont( int font, uint color );
        static bool          Global_SetEffect( int effect_type, int effect_subtype, ScriptString* effect_name, ScriptString* effect_defines );
        static void          Global_RefreshMap( bool only_tiles, bool only_roof, bool only_light );
        static void          Global_MouseClick( int x, int y, int button, int cursor );
        static void          Global_KeyboardPress( uint8 key1, uint8 key2, ScriptString* key1_text, ScriptString* key2_text );
        static void          Global_SetRainAnimation( ScriptString* fall_anim_name, ScriptString* drop_anim_name );
        static void          Global_SetZoom( float zoom );
        static bool          Global_SaveScreenshot();
        static bool          Global_SaveLogFile();
    } ScriptFunc;

    static bool SpritesCanDraw;

/************************************************************************/
/* Interface                                                            */
/************************************************************************/
    int       IfaceHold;
    StrVec    IfaceIniNames;
    IniParser IfaceIni;

    bool AppendIfaceIni( const char* ini_name );
    void AppendIfaceIni( uint8* data, uint len );
    int  InitIface();
    bool IfaceLoadRect( Rect& comp, const char* name );
    void IfaceLoadRect2( Rect& comp, const char* name, int ox, int oy );
    void IfaceLoadSpr( AnyFrames*& comp, const char* name );
    void IfaceLoadAnim( uint& comp, const char* name );
    void IfaceLoadArray( IntVec& arr, const char* name );
    void IfaceFreeResources();

    bool IsCurInRect( const Rect& rect, int ax, int ay );
    bool IsCurInRect( const Rect& rect );
    bool IsCurInRectNoTransp( uint spr_id, Rect& rect, int ax, int ay );
    bool IsCurInInterface();
    bool GetCurHex( uint16& hx, uint16& hy, bool ignore_interface );

    void DrawIndicator( Rect& rect, PointVec& points, uint color, int procent, uint& tick, bool is_vertical, bool from_top_or_left );

    // Initial state
    ItemVec InvContInit;
    ItemVec BarterCont1oInit, BarterCont2Init, BarterCont2oInit;
    ItemVec PupCont2Init;
    // After script processing
    ItemVec InvCont, BarterCont1, PupCont1, UseCont;
    ItemVec BarterCont1o, BarterCont2, BarterCont2o;
    ItemVec PupCont2;

    uint  GetCurContainerItemId( const Rect& pos, int height, int scroll, ItemVec& cont );
    void  ContainerDraw( const Rect& pos, int height, int scroll, ItemVec& cont, uint skip_id );
    Item* GetContainerItem( ItemVec& cont, uint id );
    void  CollectContItems();
    void  ProcessItemsCollection( int collection, ItemVec& init_items, ItemVec& result );
    void  UpdateContLexems( ItemVec& cont, uint item_id, const char* lexems );

/************************************************************************/
/* Console                                                              */
/************************************************************************/
    AnyFrames*  ConsolePic;
    int         ConsolePicX, ConsolePicY, ConsoleTextX, ConsoleTextY;
    static bool ConsoleActive;
    std::string ConsoleStr;
    uint        ConsoleCur;
    StrVec      ConsoleHistory;
    int         ConsoleHistoryCur;
    uint8       ConsoleLastKey;
    std::string ConsoleLastKeyText;

    void ConsoleDraw();
    void ConsoleKeyDown( uint8 dik, const char* dik_text );
    void ConsoleKeyUp( uint8 dik );
    void ConsoleProcess();

/************************************************************************/
/* Inventory                                                            */
/************************************************************************/
    int InvFocus;
    #define INVF_NONE           (0)
    #define INVF_RAD_CHANNEL    (1)

    AnyFrames*  InvPWMain, * InvPBOkDw, * InvPBOkUp, * InvPBScrUpDw, * InvPBScrUpUp,
             * InvPBScrUpOff, * InvPBScrDwDw, * InvPBScrDwUp, * InvPBScrDwOff;
    uint        InvHoldId;
    std::string InvItemInfo;
    int         InvItemInfoScroll, InvItemInfoMaxScroll;
    int         InvScroll;
    int         InvX, InvY;
    Rect        InvWMain, InvWChosen;
    int         InvHeightItem;
    Rect        InvWInv, InvWSlot1, InvWSlot2, InvWArmor;
    Rect        InvBScrUp, InvBScrDn, InvBOk;
    Rect        InvWText;
    int         InvVectX, InvVectY;
    // Extended slots
    struct SlotExt
    {
        int   Index;
        char* IniName;
        Rect  Region;
    };
    typedef std::vector<SlotExt> SlotExtVec;
    SlotExtVec SlotsExt;

    void InvDraw();
    void InvMouseMove();
    void InvLMouseDown();
    void InvLMouseUp();
    void InvRMouseDown();

/************************************************************************/
/* Use                                                                  */
/************************************************************************/
    AnyFrames*   UseWMainPicNone, * UseBCancelPicDown, * UseBScrUpPicDown, * UseBScrDownPicDown,
             * UseBScrUpPicUp, * UseBScrDownPicUp, * UseBScrUpPicOff, * UseBScrDownPicOff;
    int          UseX, UseY, UseVectX, UseVectY;
    int          UseScroll, UseHeightItem;
    Rect         UseWMain, UseWChosen, UseWInv, UseBScrUp, UseBScrDown, UseBCancel;
    uint         UseHoldId;
    SmthSelected UseSelect;

    void UseDraw();
    void UseLMouseDown();
    void UseLMouseUp();
    void UseRMouseDown();
    void UseMouseMove();

/************************************************************************/
/* Game                                                                 */
/************************************************************************/
    struct MapText
    {
        uint16      HexX, HexY;
        uint        StartTick, Tick;
        std::string Text;
        uint        Color;
        bool        Fade;
        Rect        Pos;
        Rect        EndPos;
        bool operator==( const MapText& r ) { return HexX == r.HexX && HexY == r.HexY; }
    };
    typedef std::vector<MapText> MapTextVec;

    MapTextVec GameMapTexts;
    uint       GameMouseStay;

    void GameDraw();
    void GameKeyDown( uint8 dik, const char* dik_text );
    void GameLMouseDown();
    void GameLMouseUp();
    void GameRMouseDown();
    void GameRMouseUp();

/************************************************************************/
/* Main iface                                                           */
/************************************************************************/
    AnyFrames* IntMainPic, * IntPWAddMess, * IntPBAddMessDn, * IntPBMessFilter1Dn, * IntPBMessFilter2Dn, * IntPBMessFilter3Dn,
             * IntPBScrUpDn, * IntPBScrDnDn, * IntPBSlotsDn,
             * IntPBInvDn, * IntPBMenuDn, * IntPBSkillDn, * IntPBMapDn, * IntPBChaDn, * IntPBPipDn, * IntPBFixDn,
             * IntDiodeG, * IntDiodeY, * IntDiodeR, * IntBreakTimePic, * IntWApCostPicNone;

    int        IntX, IntY;
    bool       IntVisible, IntAddMess;
    Rect       IntWMain, IntWAddMess, IntBAddMess, IntBMessFilter1, IntBMessFilter2, IntBMessFilter3;
    Rect       IntBItem, IntWApCost;
    Rect       IntBChangeSlot, IntBInv, IntBMenu, IntBSkill, IntBMap, IntBChar, IntBPip, IntBFix;
    Rect       IntWMess, IntWMessLarge;
    Rect       IntAP, IntHP, IntAC, IntBreakTime;
    int        IntAPstepX, IntAPstepY, IntAPMax;
    AnyFrames* IntBItemPicDn;
    int        IntBItemOffsX, IntBItemOffsY;
    AnyFrames* IntAimPic;
    int        IntAimX, IntAimY;
    int        IntUseX, IntUseY;
    AnyFrames* IntBCombatTurnPicDown, * IntBCombatEndPicDown;
    uint       IntWCombatAnim;
    Rect       IntWCombat, IntBCombatTurn, IntBCombatEnd;

    Rect       IntWAmmoCount, IntWWearProcent, IntWAmmoCountStr, IntWWearProcentStr;
    PointVec   IntAmmoPoints, IntWearPoints;
    uint       IntAmmoTick, IntWearTick;

    void IntDraw();
    int  IntLMouseDown();
    void IntRMouseDown();
    void IntLMouseUp();
    void IntMouseMove();

/************************************************************************/
/* LMenu                                                                */
/************************************************************************/
    AnyFrames* LmenuPTalkOff, * LmenuPTalkOn, * LmenuPLookOff, * LmenuPLookOn, * LmenuPBreakOff, * LmenuPBreakOn,
             * LmenuPUseOff, * LmenuPUseOn, * LmenuPGMFollowOff, * LmenuPGMFollowOn,
             * LmenuPRotateOn, * LmenuPRotateOff, * LmenuPDropOn, * LmenuPDropOff, * LmenuPUnloadOn, * LmenuPUnloadOff,
             * LmenuPSortUpOn, * LmenuPSortUpOff, * LmenuPSortDownOn, * LmenuPSortDownOff,
             * LmenuPPickItemOn, * LmenuPPickItemOff, * LmenuPPushOn, * LmenuPPushOff,
             * LmenuPBagOn, * LmenuPBagOff, * LmenuPSkillOn, * LmenuPSkillOff,
             * LmenuPBarterOpenOn, * LmenuPBarterOpenOff, * LmenuPBarterHideOn, * LmenuPBarterHideOff,
             * LmenuPWorldmapKickOff, * LmenuPWorldmapKickOn, * LmenuPWorldmapRuleOff, * LmenuPWorldmapRuleOn,
             * LmenuPVoteUpOff, * LmenuPVoteUpOn, * LmenuPVoteDownOff, * LmenuPVoteDownOn;

    bool      LMenuActive;
    bool      LMenuTryActivated;
    uint      LMenuStartTime;
    int       LMenuX, LMenuY, LMenuRestoreCurX, LMenuRestoreCurY;
    int       LMenuNodeHeight;
    UInt8Vec* LMenuCurNodes;
    int       LMenuCurNode;
    UInt8Vec  LMenuCritNodes, LMenuScenNodes, LMenuNodes;
    int       LMenuMode;

    bool IsLMenu();
    void LMenuTryActivate();
    void LMenuStayOff();
    void LMenuTryCreate();
    void LMenuCollect();
    void LMenuSet( uint8 set_lmenu );
    void LMenuDraw();
    void LMenuMouseMove();
    void LMenuMouseUp();

/************************************************************************/
/* Login                                                                */
/************************************************************************/
    AnyFrames* LogPMain, * LogPSingleplayerMain, * LogPBLogin, * LogPBReg, * LogPBOptions, * LogPBCredits, * LogPBExit;
    int        LogFocus;

    int        LogX, LogY;
    Rect       LogMain, LogWName, LogWPass, LogBOk, LogBOkText, LogBOptions, LogBOptionsText, LogBCredits, LogBCreditsText,
               LogBReg, LogBRegText, LogBExit, LogBExitText, LogWChat, LogWVersion;

    void LogDraw();
    void LogKeyDown( uint8 dik, const char* dik_text );
    void LogLMouseDown();
    void LogLMouseUp();
    void LogTryConnect();

/************************************************************************/
/* Dialog                                                               */
/************************************************************************/
    AnyFrames* DlgPMain, * DlgPAnsw, * DlgPBBarter, * DlgPBSay, * DlgAvatarPic;
    int        DlgNextAnswX, DlgNextAnswY;
    int        DlgCurAnsw, DlgHoldAnsw;
    uint       DlgCurAnswPage, DlgMaxAnswPage;
    int        DlgVectX, DlgVectY;
    uint8      DlgIsNpc;
    uint       DlgNpcId;
    uint       DlgEndTick;

    struct Answer
    {
        uint        Page;
        Rect        Position;
        std::string Text;
        int         AnswerNum; // -1 prev page, -2 next page

        Answer( uint page, Rect pos, std::string text, uint answer_num ) : Page( page ), Position( pos ), Text( text ), AnswerNum( answer_num ) {}
    };
    std::vector<Answer> DlgAllAnswers, DlgAnswers;

    std::string         DlgMainText;
    int                 DlgMainTextCur, DlgMainTextLinesReal, DlgMainTextLinesRect;
    int                 DlgX, DlgY;
    Rect                DlgWMain, DlgWText, DlgBScrUp, DlgBScrDn, DlgAnsw, DlgAnswText, DlgWMoney, DlgBBarter,
                        DlgBBarterText, DlgBSay, DlgBSayText, DlgWAvatar, DlgWTimer;

    // Barter
    AnyFrames* BarterPMain, * BarterPBOfferDn, * BarterPBTalkDn,
             * BarterPBC1ScrUpDn, * BarterPBC2ScrUpDn, * BarterPBC1oScrUpDn, * BarterPBC2oScrUpDn,
             * BarterPBC1ScrDnDn, * BarterPBC2ScrDnDn, * BarterPBC1oScrDnDn, * BarterPBC2oScrDnDn;
    Rect BarterWMain, BarterBOffer, BarterBOfferText, BarterBTalk, BarterBTalkText, BarterWCont1Pic, BarterWCont2Pic,
         BarterWCont1, BarterWCont2, BarterWCont1o, BarterWCont2o,
         BarterBCont1ScrUp, BarterBCont2ScrUp, BarterBCont1oScrUp, BarterBCont2oScrUp,
         BarterBCont1ScrDn, BarterBCont2ScrDn, BarterBCont1oScrDn, BarterBCont2oScrDn,
         BarterWCost1, BarterWCost2, BarterWChosen, BarterWCritter;
    uint        BarterPlayerId;
    int         BarterCont1HeightItem, BarterCont2HeightItem,
                BarterCont1oHeightItem, BarterCont2oHeightItem;
    int         BarterScroll1, BarterScroll2, BarterScroll1o, BarterScroll2o;
    uint        BarterHoldId;
    uint        BarterCount;
    uint16      BarterK;
    std::string BarterText;
    // Players barter extra
    uint        BarterOpponentId;
    bool        BarterIsPlayers, BarterHide, BarterOpponentHide, BarterOffer, BarterOpponentOffer;

    bool IsScreenPlayersBarter();
    void BarterTryOffer();
    void BarterTransfer( uint item_id, int item_cont, uint item_count );
    void ContainerCalcInfo( ItemVec& cont, uint& cost, uint& weigth, uint& volume, int barter_k, bool sell );
    void FormatTags( char* text, size_t text_len, CritterCl* player, CritterCl* npc, const char* lexems );

    void DlgDraw( bool is_dialog );
    void DlgMouseMove( bool is_dialog );
    void DlgLMouseDown( bool is_dialog );
    void DlgLMouseUp( bool is_dialog );
    void DlgRMouseDown( bool is_dialog );
    void DlgKeyDown( bool is_dialog, uint8 dik, const char* dik_text );
    void DlgCollectAnswers( bool next );

/************************************************************************/
/* Mini-map                                                             */
/************************************************************************/
    #define MINIMAP_PREPARE_TICK    (1000)
    AnyFrames* LmapPMain, * LmapPBOkDw, * LmapPBScanDw, * LmapPBLoHiDw, * LmapPPix;
    PointVec   LmapPrepPix;
    Rect       LmapMain, LmapWMap, LmapBOk, LmapBScan, LmapBLoHi;
    short      LmapX, LmapY;
    int        LmapVectX, LmapVectY;
    int        LmapZoom;
    bool       LmapSwitchHi;
    uint       LmapPrepareNextTick;

    void LmapPrepareMap();
    void LmapDraw();
    void LmapLMouseDown();
    void LmapMouseMove();
    void LmapLMouseUp();

/************************************************************************/
/* Global map                                                           */
/************************************************************************/
    uint         WorldmapTilesX, WorldmapTilesY;
    AnimVec      WorldmapPic;
    char         WorldmapTilesPic[MAX_FOPATH];

    uint         WorldmapWDayTimeAnim;
    AnyFrames*   WorldmapPBTownDw, * WorldmapWMainPic, * WorldmapPGr, * WorldmapPTarg, * WorldmapPStay, * WorldmapPStayDn, * WorldmapPStayMask, * WorldmapLocPic;
    AnyFrames*   WorldmapPTownInPic, * WorldmapPTownInPicDn, * WorldmapPTownInPicMask, * WorldmapPTownViewPic, * WorldmapPTownViewPicDn, * WorldmapPTownViewPicMask;
    int          WorldmapPTownInOffsX, WorldmapPTownInOffsY, WorldmapPTownViewOffsX, WorldmapPTownViewOffsY;
    AnyFrames*   WorldmapPFollowCrit, * WorldmapPFollowCritSelf;
    AnyFrames*   WorldmapPWTab, * WorldmapPWBlankTab, * WorldmapPBTabLoc, * WorldmapPTabScrUpDw, * WorldmapPTabScrDwDw;
    AnyFrames*   WorldmapBInvPicDown, * WorldmapBMenuPicDown, * WorldmapBChaPicDown, * WorldmapBPipPicDown, * WorldmapBFixPicDown;
    AnyFrames*   WorldmapPLightPic0, * WorldmapPLightPic1;
    int          WorldmapX, WorldmapY, WorldmapVectX, WorldmapVectY, WorldmapWNameStepX, WorldmapWNameStepY;
    Rect         WorldmapWMain, WorldmapWMap, WorldmapBTown, WorldmapWName, WorldmapWChat, WorldmapWPanel, WorldmapWCar, WorldmapWLock, WorldmapWTime, WorldmapWDayTime;
    Rect         WorldmapBInv, WorldmapBMenu, WorldmapBCha, WorldmapBPip, WorldmapBFix;
    PointVec     WorldmapMapCutOff;
    static bool  WorldmapActive;
    static float WorldmapZoom;
    static int   WorldmapOffsetX, WorldmapOffsetY;

    void WorldmapNullParams();
    void WorldmapProcess();

    // Town
    AnyFrames* GmapTownPic;
    Rect       GmapTownPicPos;
    IntRectVec GmapTownButtonPos;
    IntRectVec GmapTownTextPos;
    StrVec     GmapTownText;
    int        GmapTownCurButton;
    uint       WorldmapNextShowEntrancesTick;
    uint       WorldmapShowEntrancesLocId;
    bool       WorldmapShowEntrances[0x100];

    // Mask
    C2BitMask WorldmapFog;
    PointVec  WorldmapFogPix;

    // Locations
    struct GmapLocation
    {
        uint   LocId;
        uint16 LocPid;
        uint16 LocWx;
        uint16 LocWy;
        uint16 Radius;
        uint   Color;
        bool operator==( const uint& _right ) { return this->LocId == _right;  }
    };
    typedef std::vector<GmapLocation> GmapLocationVec;
    GmapLocationVec GmapLoc;
    GmapLocation    GmapTownLoc;

    // Trace
    IntPairVec GmapTrace;

    // Params
    uint         GmapMoveTick;
    int          GmapGroupRealOldX, GmapGroupRealOldY, GmapGroupRealCurX, GmapGroupRealCurY;
    static int   WorldmapGroupCurX, WorldmapGroupCurY, WorldmapGroupToX, WorldmapGroupToY;
    static bool  WorldmapWait;
    static float WorldmapGroupSpeed;

    // Cars
    struct
    {
        uint  MasterId;
        Item* Car;
    } GmapCar;

    // Tabs
    Rect WorldmapWTabs, WorldmapWTab, WorldmapWTabLoc, WorldmapBTabLoc, WorldmapBTabsScrUp, WorldmapBTabsScrDn;
    int  WorldmapTabNextX, WorldmapTabNextY, WorldmapTabsScrX, WorldmapTabsScrY;
    uint GmapCurHoldBLocId;
    uint GmapTabsLastScr;
    int  GmapHoldX, GmapHoldY;

    void  GmapDraw();
    void  GmapTownDraw();
    void  GmapLMouseDown();
    void  GmapLMouseUp();
    void  GmapRMouseDown();
    void  GmapRMouseUp();
    void  GmapMouseMove();
    void  GmapKeyDown( uint8 dik, const char* dik_text );
    void  GmapChangeZoom( float offs, bool revert = false );
    Item* GmapGetCar();
    uint  GmapGetMouseTabLocId();
    void  GmapFreeResources();

    #define GMAP_CHECK_MAPSCR                                                       \
        do {                                                                        \
            if( WorldmapOffsetX > WorldmapWMap[0] )                                 \
                WorldmapOffsetX = WorldmapWMap[0];                                  \
            if( WorldmapOffsetY > WorldmapWMap[1] )                                 \
                WorldmapOffsetY = WorldmapWMap[1];                                  \
            if( WorldmapOffsetX < WorldmapWMap[2] - (int)(GM_MAXX / WorldmapZoom) ) \
                WorldmapOffsetX = WorldmapWMap[2] - (int)(GM_MAXX / WorldmapZoom);  \
            if( WorldmapOffsetY < WorldmapWMap[3] - (int)(GM_MAXY / WorldmapZoom) ) \
                WorldmapOffsetY = WorldmapWMap[3] - (int)(GM_MAXY / WorldmapZoom);  \
        } while( 0 )

/************************************************************************/
/* Skillbox                                                             */
/************************************************************************/
    AnyFrames*   SboxPMain, * SboxPBCancelDn, * SboxPBSneakDn, * SboxPBLockPickDn, * SboxPBStealDn,
             * SboxPBTrapsDn, * SboxPBFirstaidDn, * SboxPBDoctorDn, * SboxPBScienceDn, * SboxPBRepairDn;
    Rect         SboxWMain, SboxWMainText, SboxBCancel, SboxBCancelText, SboxBSneak, SboxBLockpick, SboxBSteal,
                 SboxBTrap, SboxBFirstAid, SboxBDoctor, SboxBScience, SboxBRepair;
    Rect         SboxTSneak, SboxTLockpick, SboxTSteal, SboxTTrap, SboxTFirstAid,
                 SboxTDoctor, SboxTScience, SboxTRepair;
    int          SboxX, SboxY;
    int          SboxVectX, SboxVectY;

    uint16       CurSkill;
    SmthSelected SboxUseOn;

    void SboxDraw();
    void SboxLMouseDown();
    void SboxMouseMove();
    void SboxLMouseUp();

/************************************************************************/
/* Menu Options                                                         */
/************************************************************************/
    AnyFrames* MoptMainPic, * MoptSingleplayerMainPic, * MoptSaveGamePicDown, * MoptLoadGamePicDown,
             * MoptOptionsPicDown, * MoptExitPicDown, * MoptResumePicDown;
    Rect       MoptMain, MoptSaveGame, MoptLoadGame, MoptOptions, MoptExit, MoptResume;
    int        MoptX, MoptY;

    void MoptDraw();
    void MoptLMouseDown();
    void MoptLMouseUp();

/************************************************************************/
/* Credits                                                              */
/************************************************************************/
    uint CreditsNextTick, CreditsMoveTick;
    int  CreditsYPos;
    bool CreaditsExt;

    void CreditsDraw();

/************************************************************************/
/* Character                                                            */
/************************************************************************/
    // Switch
    int ChaCurSwitch;
    #define CHA_SWITCH_PERKS    (0)
    #define CHA_SWITCH_KARMA    (1)
    #define CHA_SWITCH_KILLS    (2)

    AnyFrames* ChaPBSwitchPerks, * ChaPBSwitchKarma, * ChaPBSwitchKills, * ChaPBSwitchMask,
             * ChaPBSwitchScrUpUp, * ChaPBSwitchScrUpDn, * ChaPBSwitchScrDnUp, * ChaPBSwitchScrDnDn;
    Rect       ChaBSwitch, ChaTSwitch, ChaBSwitchScrUp, ChaBSwitchScrDn;

    struct SwitchElement
    {
        uint   NameStrNum;
        uint   DescStrNum;
        uint16 PictureId;
        uint   DrawFlags;
        char   Addon[64];

        SwitchElement( uint name, uint desc, uint16 pic, uint flags ) : NameStrNum( name ), DescStrNum( desc ), DrawFlags( flags ), PictureId( pic ) { memzero( Addon, sizeof(Addon) ); }
        SwitchElement( const char* add, uint flags ) : NameStrNum( 0 ), DescStrNum( 0 ), PictureId( 0 ), DrawFlags( flags ) { memcpy( Addon, add, sizeof(Addon) ); }
    };
    typedef std::vector<SwitchElement> SwitchElementVec;

    SwitchElementVec ChaSwitchText[3];
    int              ChaSwitchScroll[3];
    void ChaPrepareSwitch();

    // Pics
    AnyFrames* ChaPMain, * ChaPBPrintDn, * ChaPBOkDn, * ChaPBCancelDn;
    int        ChaX, ChaY;
    int        ChaVectX, ChaVectY;
    Rect       ChaWMain, ChaBPrint, ChaBPrintText, ChaBOk, ChaBOkText, ChaBCancel, ChaBCancelText;

    // Special
    Rect   ChaWSpecialText, ChaWSpecialValue, ChaWSpecialLevel;
    int    ChaWSpecialNextX, ChaWSpecialNextY;
    IntVec ChaSpecialParams;

    // Skills
    Rect   ChaWSkillText, ChaWSkillName, ChaWSkillValue;
    int    ChaWSkillNextX, ChaWSkillNextY;
    uint16 ChaSkillUp[MAX_PARAMS];
    Rect   ChaWUnspentSP, ChaWUnspentSPText;
    int    ChaUnspentSkillPoints;

    // Slider
    AnyFrames* ChaPWSlider, * ChaPBSliderPlusDn, * ChaPBSliderMinusDn;
    Rect       ChaBSliderPlus, ChaBSliderMinus;
    int        ChaWSliderX, ChaWSliderY;
    int        ChaCurSkill;

    // Level
    Rect ChaWLevel, ChaWExp, ChaWNextLevel;

    // Damage
    Rect ChaWDmgLife, ChaWDmg;
    int  ChaWDmgNextX, ChaWDmgNextY;

    // Stats
    Rect ChaWStatsName, ChaWStatsValue;
    int  ChaWStatsNextX, ChaWStatsNextY;

    // Tip
    Rect ChaWName, ChaWDesc, ChaWPic;
    char ChaName[MAX_FOTEXT];
    char ChaDesc[MAX_FOTEXT];
    int  ChaSkilldexPic;

    // Buttons
    AnyFrames* ChaPBNameDn, * ChaPBAgeDn, * ChaPBSexDn, * RegPBAccDn;
    Rect       ChaBName, ChaBAge, ChaBSex;

    // Registration
    AnyFrames* RegPMain, * RegPBSpecialPlusDn, * RegPBSpecialMinusDn, * RegPBTagSkillDn;
    Rect       RegWMain, RegBSpecialPlus, RegBSpecialMinus, RegBTagSkill, RegWUnspentSpecial, RegWUnspentSpecialText;
    int        RegBSpecialNextX, RegBSpecialNextY, RegBTagSkillNextX, RegBTagSkillNextY;
    int        RegCurSpecial, RegCurTagSkill;
    CritterCl* RegNewCr;

    // Trait
    AnyFrames* RegPBTraitDn;
    Rect       RegBTraitL, RegBTraitR, RegWTraitL, RegWTraitR;
    int        RegTraitNextX, RegTraitNextY;
    int        RegTraitNum;

    // Methods
    bool RegCheckData( CritterCl* newcr );
    void ChaDraw( bool is_reg );
    void ChaLMouseDown( bool is_reg );
    void ChaLMouseUp( bool is_reg );
    void ChaMouseMove( bool is_reg );

/************************************************************************/
/* Character name                                                       */
/************************************************************************/
    AnyFrames* ChaNameMainPic, * ChaNameSingleplayerMainPic;
    Rect       ChaNameWMain, ChaNameWName, ChaNameWNameText, ChaNameWPass, ChaNameWPassText;
    int        ChaNameX, ChaNameY;

    void ChaNameDraw();
    void ChaNameLMouseDown();
    void ChaNameKeyDown( uint8 dik, const char* dik_text );

/************************************************************************/
/* Character age                                                        */
/************************************************************************/
    AnyFrames* ChaAgePic, * ChaAgeBUpDn, * ChaAgeBDownDn;
    Rect       ChaAgeWMain, ChaAgeBUp, ChaAgeBDown, ChaAgeWAge;
    int        ChaAgeX, ChaAgeY;

    void ChaAgeDraw();
    void ChaAgeLMouseDown();
    void ChaAgeLMouseUp();

/************************************************************************/
/* Character sex                                                        */
/************************************************************************/
    AnyFrames* ChaSexPic, * ChaSexBMaleDn, * ChaSexBFemaleDn;
    Rect       ChaSexWMain, ChaSexBMale, ChaSexBFemale;
    int        ChaSexX, ChaSexY;

    void ChaSexDraw();
    void ChaSexLMouseDown();
    void ChaSexLMouseUp();

/************************************************************************/
/* Perk                                                                 */
/************************************************************************/
    AnyFrames* PerkPMain, * PerkPBScrUpDn, * PerkPBScrDnDn, * PerkPBOkDn, * PerkPBCancelDn;
    Rect       PerkWMain, PerkWText, PerkWPerks, PerkWPic, PerkBScrUp, PerkBScrDn, PerkBOk, PerkBCancel, PerkBOkText, PerkBCancelText;
    int        PerkX, PerkY;
    int        PerkVectX, PerkVectY;
    int        PerkNextX, PerkNextY;
    int        PerkScroll;
    int        PerkCurPerk;
    UInt16Vec  PerkCollection;

    void PerkPrepare();
    void PerkDraw();
    void PerkLMouseDown();
    void PerkLMouseUp();
    void PerkMouseMove();

/************************************************************************/
/* Town view                                                            */
/************************************************************************/
    AnyFrames* TViewWMainPic, * TViewBBackPicDn, * TViewBEnterPicDn, * TViewBContoursPicDn;
    Rect       TViewWMain, TViewBBack, TViewBEnter, TViewBContours;
    int        TViewX, TViewY, TViewVectX, TViewVectY;
    bool       TViewShowCountours;

    #define TOWN_VIEW_FROM_NONE      (0)
    #define TOWN_VIEW_FROM_GLOBAL    (1)
    int  TViewType;
    uint TViewGmapLocId, TViewGmapLocEntrance;    // TOWN_VIEW_FROM_GLOBAL

    void TViewDraw();
    void TViewLMouseDown();
    void TViewLMouseUp();
    void TViewMouseMove();

/************************************************************************/
/* PipBoy                                                               */
/************************************************************************/
    int PipMode;
    #define PIP__NONE                (0)
    #define PIP__STATUS              (1)
    #define PIP__STATUS_QUESTS       (2)
    #define PIP__STATUS_SCORES       (3)
    #define PIP__GAMES               (4)
    #define PIP__AUTOMAPS            (5)
    #define PIP__AUTOMAPS_LOC        (6)
    #define PIP__AUTOMAPS_MAP        (7)
    #define PIP__ARCHIVES            (8)
    #define PIP__ARCHIVES_INFO       (9)

    AnyFrames* PipPMain, * PipPBStatusDn /*,*PipPBGamesDn*/, * PipPBAutomapsDn, * PipPBArchivesDn, * PipPBCloseDn, * PipPWMonitor;
    int        PipX, PipY;
    int        PipVectX, PipVectY;
    Rect       PipWMain, PipWMonitor, PipBStatus /*,PipBGames*/, PipBAutomaps, PipBArchives, PipBClose, PipWTime;
    int        PipScroll[PIP__ARCHIVES_INFO + 1];

    void PipDraw();
    void PipLMouseDown();
    void PipLMouseUp();
    void PipRMouseDown();
    void PipMouseMove();

    // Quests
    QuestManager QuestMngr;
    uint         QuestNumTab;
    uint16       QuestNumQuest;
    // HoloInfo
    uint         HoloInfo[MAX_HOLO_INFO];
    uint         PipInfoNum;
    // Scores
    uint         ScoresNextUploadTick;
    char         BestScores[SCORES_MAX][SCORE_NAME_LEN];
    // Automaps
    struct Automap
    {
        uint        LocId;
        uint16      LocPid;
        std::string LocName;
        UInt16Vec   MapPids;
        StrVec      MapNames;
        size_t      CurMap;

        Automap() : LocId( 0 ), LocPid( 0 ), CurMap( 0 ) {}
        bool operator==( const uint id ) const { return LocId == id; }
    };
    typedef std::vector<Automap> AutomapVec;
    AutomapVec Automaps;
    Automap    AutomapSelected;
    UInt16Set  AutomapWaitPids;
    UInt16Set  AutomapReceivedPids;
    PointVec   AutomapPoints;
    uint16     AutomapCurMapPid;
    float      AutomapScrX, AutomapScrY;
    float      AutomapZoom;

/************************************************************************/
/* Aim                                                                  */
/************************************************************************/
    AnyFrames* AimPWMain, * AimPBCancelDn, * AimPic;
    int        AimX, AimY;
    int        AimVectX, AimVectY;
    int        AimPicX, AimPicY;
    Rect       AimWMain, AimBCancel,
               AimWHeadT, AimWLArmT, AimWRArmT, AimWTorsoT, AimWRLegT, AimWLLegT, AimWEyesT, AimWGroinT,
               AimWHeadP, AimWLArmP, AimWRArmP, AimWTorsoP, AimWRLegP, AimWLLegP, AimWEyesP, AimWGroinP;
    int  AimHeadP, AimLArmP, AimRArmP, AimTorsoP, AimRLegP, AimLLegP, AimEyesP, AimGroinP;
    uint AimTargetId;

    void       AimDraw();
    void       AimLMouseDown();
    void       AimLMouseUp();
    void       AimMouseMove();
    AnyFrames* AimGetPic( CritterCl* cr, const char* ext );

/************************************************************************/
/* PickUp                                                               */
/************************************************************************/
    AnyFrames* PupPMain, * PupPTakeAllOn, * PupPBOkOn,
             * PupPBScrUpOn1, * PupPBScrUpOff1, * PupPBScrDwOn1, * PupPBScrDwOff1,
             * PupPBScrUpOn2, * PupPBScrUpOff2, * PupPBScrDwOn2, * PupPBScrDwOff2,
             * PupBNextCritLeftPicUp, * PupBNextCritLeftPicDown,
             * PupBNextCritRightPicUp, * PupBNextCritRightPicDown;
    uint   PupHoldId;
    int    PupScroll1, PupScroll2, PupScrollCrit;
    int    PupX, PupY;
    int    PupVectX, PupVectY;
    Rect   PupWMain, PupWInfo, PupWCont1, PupWCont2, PupBTakeAll, PupBOk,
           PupBScrUp1, PupBScrDw1, PupBScrUp2, PupBScrDw2, PupBNextCritLeft, PupBNextCritRight;
    int    PupHeightItem1, PupHeightItem2;
    uint8  PupTransferType;
    uint   PupContId, PupClosedContId, PupLastPutId;
    uint16 PupContPid;
    uint   PupCount;
    uint16 PupSize;
    uint   PupWeight;

    void       PupDraw();
    void       PupMouseMove();
    void       PupLMouseDown();
    void       PupLMouseUp();
    void       PupRMouseDown();
    void       PupTransfer( uint item_id, uint cont, uint count );
    CritVec&   PupGetLootCrits();
    CritterCl* PupGetLootCrit( int scroll );

/************************************************************************/
/* Dialog box                                                           */
/************************************************************************/
    AnyFrames* DlgboxWTopPicNone, * DlgboxWMiddlePicNone, * DlgboxWBottomPicNone, * DlgboxBButtonPicDown;
    Rect       DlgboxWTop, DlgboxWMiddle, DlgboxWBottom, DlgboxWText, DlgboxBButton, DlgboxBButtonText;
    int        DlgboxX, DlgboxY;
    int        DlgboxVectX, DlgboxVectY;
    uint8      DlgboxType;
    #define DIALOGBOX_NONE             (0)
    #define DIALOGBOX_FOLLOW           (1)
    #define DIALOGBOX_BARTER           (2)
    #define DIALOGBOX_ENCOUNTER_ANY    (3)
    #define DIALOGBOX_ENCOUNTER_RT     (4)
    #define DIALOGBOX_ENCOUNTER_TB     (5)
    #define DIALOGBOX_MANUAL           (6)
    uint        DlgboxWait;
    char        DlgboxText[MAX_FOTEXT];
    std::string DlgboxButtonText[MAX_DLGBOX_BUTTONS];
    uint        DlgboxButtonsCount;
    uint        DlgboxSelectedButton;
    // For follow
    uint8       FollowType;
    uint        FollowRuleId;
    uint16      FollowMap;
    // For barter
    uint        PBarterPlayerId;
    bool        PBarterHide;

    void DlgboxDraw();
    void DlgboxLMouseDown();
    void DlgboxLMouseUp();
    void DlgboxMouseMove();

/************************************************************************/
/* Elevator                                                             */
/************************************************************************/
    AnyFrames* ElevatorMainPic, * ElevatorExtPic, * ElevatorButtonPicDown;
    uint       ElevatorIndicatorAnim;
    Rect       ElevatorMain, ElevatorExt, ElevatorIndicator;
    uint       ElevatorButtonsCount;
    Rect       ElevatorButtons[MAX_DLGBOX_BUTTONS];
    uint       ElevatorType, ElevatorLevelsCount, ElevatorStartLevel, ElevatorCurrentLevel;
    int        ElevatorX, ElevatorY, ElevatorVectX, ElevatorVectY;
    int        ElevatorSelectedButton;
    bool       ElevatorAnswerDone;
    uint       ElevatorSendAnswerTick;

    void ElevatorDraw();
    void ElevatorLMouseDown();
    void ElevatorLMouseUp();
    void ElevatorMouseMove();
    void ElevatorGenerate( uint param );
    void ElevatorProcess();
    int  ElevatorGetCurButton();

/************************************************************************/
/* Say dialog                                                           */
/************************************************************************/
    AnyFrames* SayWMainPicNone, * SayBOkPicDown, * SayBCancelPicDown;
    int        SayX, SayY;
    int        SayVectX, SayVectY;
    Rect       SayWMain, SayWMainText, SayWSay, SayBOk, SayBOkText, SayBCancel, SayBCancelText;
    uint8      SayType;
    bool       SayOnlyNumbers;
    #define DIALOGSAY_NONE             (0)
    #define DIALOGSAY_TEXT             (1)
    #define DIALOGSAY_SAVE             (2)
    std::string SayTitle;
    std::string SayText;

    void SayDraw();
    void SayLMouseDown();
    void SayLMouseUp();
    void SayMouseMove();
    void SayKeyDown( uint8 dik, const char* dik_text );

/************************************************************************/
/* Wait                                                                 */
/************************************************************************/
    AnyFrames* WaitPic;

    void WaitDraw();

/************************************************************************/
/* Split                                                                */
/************************************************************************/
    #define MAX_SPLIT_VALUE            (100000)

    AnyFrames* SplitMainPic, * SplitPBUpDn, * SplitPBDnDn, * SplitPBAllDn, * SplitPBDoneDn,
             * SplitPBCancelDn, * SplitItemPic;
    uint       SplitItemColor;
    int        SplitX, SplitY;
    int        SplitVectX, SplitVectY;
    Rect       SplitWMain, SplitWTitle, SplitWItem, SplitBUp, SplitBDown, SplitBAll,
               SplitWValue, SplitBDone, SplitBCancel;
    uint       SplitItemId, SplitCont;
    int        SplitValue, SplitMinValue, SplitMaxValue;
    bool       SplitValueKeyPressed;
    int        SplitParentScreen;

    void SplitStart( Item* item, int to_cont );
    void SplitClose( bool change );
    void SplitDraw();
    void SplitKeyDown( uint8 dik, const char* dik_text );
    void SplitLMouseDown();
    void SplitLMouseUp();
    void SplitMouseMove();

/************************************************************************/
/* Timer                                                                */
/************************************************************************/
    #define TIMER_MIN_VALUE            (1)
    #define TIMER_MAX_VALUE            (599)

    AnyFrames* TimerMainPic, * TimerBUpPicDown, * TimerBDownPicDown, * TimerBDonePicDown,
             * TimerBCancelPicDown, * TimerItemPic;
    uint       TimerItemColor;
    int        TimerX, TimerY;
    int        TimerVectX, TimerVectY;
    Rect       TimerWMain, TimerWTitle, TimerWItem, TimerBUp, TimerBDown, TimerWValue,
               TimerBDone, TimerBCancel;
    int        TimerValue;
    uint       TimerItemId;

    void TimerStart( uint item_id, AnyFrames* pic, uint pic_color );
    void TimerClose( bool done );
    void TimerDraw();
    void TimerKeyDown( uint8 dik, const char* dik_text );
    void TimerLMouseDown();
    void TimerLMouseUp();
    void TimerMouseMove();

/************************************************************************/
/* FixBoy                                                               */
/************************************************************************/
    int FixMode;
    #define FIX_MODE_LIST              (0)
    #define FIX_MODE_FIXIT             (1)
    #define FIX_MODE_RESULT            (2)

    AnyFrames* FixMainPic, * FixPBDoneDn, * FixPBScrUpDn, * FixPBScrDnDn, * FixPBFixDn;
    Rect       FixWMain, FixBDone, FixBScrUp, FixBScrDn, FixWWin, FixBFix;
    int        FixX, FixY, FixVectX, FixVectY;
    int        FixCurCraft;

    struct SCraft
    {
        Rect        Pos;
        std::string Name;
        uint        Num;
        bool        IsTrue;

        SCraft( Rect& pos, std::string& name, uint num, bool is_true )
        {
            Pos = pos;
            Name = name;
            Num = num;
            IsTrue = is_true;
        }
        SCraft( const SCraft& _right )
        {
            Pos = _right.Pos;
            Name = _right.Name;
            Num = _right.Num;
            IsTrue = _right.IsTrue;
        }
        SCraft& operator=( const SCraft& _right )
        {
            Pos = _right.Pos;
            Name = _right.Name;
            Num = _right.Num;
            IsTrue = _right.IsTrue;
            return *this;
        }
    };
    typedef std::vector<SCraft>    SCraftVec;
    typedef std::vector<SCraftVec> SCraftVecVec;

    SCraftVecVec FixCraftLst;
    int          FixScrollLst;
    SCraftVecVec FixCraftFix;
    int          FixScrollFix;
    uint8        FixResult;

    struct FixDrawComponent
    {
        bool        IsText;
        Rect        Place;

        std::string Text;
        AnyFrames*  Anim;

        FixDrawComponent( Rect& r, std::string& text ) : IsText( true ), Anim( NULL )
        {
            Place = r;
            Text = text;
        }
        FixDrawComponent( Rect& r, AnyFrames* anim ) : IsText( false ), Anim( anim ) { Place = r; }
    };
    typedef std::vector<FixDrawComponent*> FixDrawComponentVec;
    #define FIX_DRAW_PIC_WIDTH         (40)
    #define FIX_DRAW_PIC_HEIGHT        (40)

    FixDrawComponentVec FixDrawComp;
    std::string         FixResultStr;
    UIntSet             FixShowCraft;
    uint                FixNextShowCraftTick;

    void       FixGenerate( int fix_mode );
    void       FixGenerateStrLine( std::string& str, Rect& r );
    void       FixGenerateItems( UInt16Vec& items_vec, UIntVec& val_vec, UInt8Vec& or_vec, std::string& str, Rect& r, int& x );
    int        GetMouseCraft();
    SCraftVec* GetCurSCrafts();

    void FixDraw();
    void FixLMouseDown();
    void FixLMouseUp();
    void FixMouseMove();

/************************************************************************/
/* Input Box                                                            */
/************************************************************************/
    int IboxMode;
    #define IBOX_MODE_NONE             (0)
    #define IBOX_MODE_HOLO             (1)

    AnyFrames*  IboxWMainPicNone, * IboxBDonePicDown, * IboxBCancelPicDown;
    Rect        IboxWMain, IboxWTitle, IboxWText, IboxBDone, IboxBDoneText, IboxBCancel, IboxBCancelText;
    int         IboxX, IboxY, IboxVectX, IboxVectY;
    std::string IboxTitle, IboxText;
    uint        IboxTitleCur, IboxTextCur;
    uint8       IboxLastKey;
    std::string IboxLastKeyText;

    // Holodisk
    uint IboxHolodiskId;

    void IboxDraw();
    void IboxLMouseDown();
    void IboxLMouseUp();
    void IboxKeyDown( uint8 dik, const char* dik_text );
    void IboxKeyUp( uint8 dik );
    void IboxProcess();
    void IboxMouseMove();

/************************************************************************/
/* Save/Load                                                            */
/************************************************************************/
    #define SAVE_LOAD_IMAGE_WIDTH      (400)
    #define SAVE_LOAD_IMAGE_HEIGHT     (300)

    AnyFrames* SaveLoadMainPic, * SaveLoadScrUpPicDown, * SaveLoadScrDownPicDown,
             * SaveLoadDonePicDown, * SaveLoadBackPicDown;
    Rect       SaveLoadMain, SaveLoadText, SaveLoadScrUp, SaveLoadScrDown, SaveLoadSlots, SaveLoadPic,
               SaveLoadInfo, SaveLoadDone, SaveLoadDoneText, SaveLoadBack, SaveLoadBackText;
    int        SaveLoadX, SaveLoadY, SaveLoadCX, SaveLoadCY, SaveLoadVectX, SaveLoadVectY;
    bool       SaveLoadLoginScreen, SaveLoadSave;
    bool       SaveLoadProcessDraft, SaveLoadDraftValid;

    #ifdef FO_D3D
    Surface_     SaveLoadDraft;
    #else
    RenderTarget SaveLoadDraft;
    #endif

    struct SaveLoadDataSlot
    {
        std::string Name;
        std::string Info;
        std::string InfoExt;
        std::string FileName;
        uint64      RealTime;
        UInt8Vec    PicData;
    };
    typedef std::vector<SaveLoadDataSlot> SaveLoadDataSlotVec;
    SaveLoadDataSlotVec SaveLoadDataSlots;
    uint                SaveLoadClickSlotTick;
    int                 SaveLoadSlotIndex, SaveLoadClickSlotIndex;
    int                 SaveLoadSlotScroll, SaveLoadSlotsMax;
    std::string         SaveLoadFileName;

    void SaveLoadCollect();
    void SaveLoadSaveGame( const char* name );
    void SaveLoadFillDraft();
    void SaveLoadShowDraft();
    void SaveLoadProcessDone();

    void SaveLoadDraw();
    void SaveLoadLMouseDown();
    void SaveLoadLMouseUp();
    void SaveLoadMouseMove();

/************************************************************************/
/* Cursor                                                               */
/************************************************************************/
    AnyFrames* CurPDef, * CurPMove, * CurPMoveBlock, * CurPHand, * CurPUseItem, * CurPUseSkill, * CurPWait,
             * CurPScrRt, * CurPScrLt, * CurPScrUp, * CurPScrDw, * CurPScrRU, * CurPScrLU, * CurPScrRD, * CurPScrLD;

    void CurDraw();

/************************************************************************/
/* Generic                                                              */
/************************************************************************/
    uint DaySumRGB;

    void SetDayTime( bool refresh );
    void ProcessMouseWheel( int data );
    void SetGameColor( uint color );

    CritterCl* Chosen;

    void       AddCritter( CritterCl* cr );
    CritterCl* GetCritter( uint crid ) { return HexMngr.GetCritter( crid ); }
    ItemHex*   GetItem( uint item_id ) { return HexMngr.GetItemById( item_id ); }
    void       ClearCritters();
    void       EraseCritter( uint remid );

    bool  IsTurnBased;
    uint  TurnBasedTime;
    uint  TurnBasedCurCritterId;
    bool  NoLogOut;
    uint* UID3, * UID2;

    bool IsTurnBasedMyTurn();

    bool     RebuildLookBorders;
    bool     DrawLookBorders, DrawShootBorders;
    PointVec LookBorders, ShootBorders;

    void LookBordersPrepare();
    void LookBordersDraw();

/************************************************************************/
/* MessBox                                                              */
/************************************************************************/
    struct MessBoxMessage
    {
        int         Type;
        std::string Mess;
        std::string Time;

        MessBoxMessage( int type, const char* mess, const char* time ) : Type( type ), Mess( mess ), Time( time ) {}
        MessBoxMessage( const MessBoxMessage& r )
        {
            Type = r.Type;
            Mess = r.Mess;
            Time = r.Time;
        }
        MessBoxMessage& operator=( const MessBoxMessage& r )
        {
            Type = r.Type;
            Mess = r.Mess;
            Time = r.Time;
            return *this;
        }
    };
    typedef std::vector<MessBoxMessage> MessBoxMessageVec;

    MessBoxMessageVec MessBox;
    std::string       MessBoxCurText;
    int               MessBoxScroll, MessBoxMaxScroll, MessBoxScrollLines;
    IntVec            MessBoxFilters;

    void MessBoxGenerate();
    void AddMess( int mess_type, const char* msg );
    void MessBoxDraw();
    Rect MessBoxCurRectDraw();
    Rect MessBoxCurRectScroll();
    bool MessBoxLMouseDown();

/************************************************************************/
/*                                                                      */
/************************************************************************/
};

// Lmenu
#define LMENU_SHOW_TIME                (400)
#define LMENU_OFF                      (0)
#define LMENU_PLAYER                   (1)
#define LMENU_NPC                      (2)
#define LMENU_ITEM                     (3)
#define LMENU_ITEM_INV                 (4)
#define LMENU_GMAP_CRIT                (5)
// Lmenu Nodes
#define LMENU_NODE_LOOK                (0)
#define LMENU_NODE_TALK                (1)
#define LMENU_NODE_BREAK               (2)
#define LMENU_NODE_PICK                (3)
#define LMENU_NODE_GMFOLLOW            (4)
#define LMENU_NODE_ROTATE              (5)
#define LMENU_NODE_DROP                (6)
#define LMENU_NODE_UNLOAD              (7)
#define LMENU_NODE_USE                 (8)
#define LMENU_NODE_SORT_UP             (10)
#define LMENU_NODE_SORT_DOWN           (11)
#define LMENU_NODE_PICK_ITEM           (12)
#define LMENU_NODE_PUSH                (13)
#define LMENU_NODE_BAG                 (14)
#define LMENU_NODE_SKILL               (15)
#define LMENU_NODE_BARTER_OPEN         (16)
#define LMENU_NODE_BARTER_HIDE         (17)
#define LMENU_NODE_GMAP_KICK           (18)
#define LMENU_NODE_GMAP_RULE           (19)
#define LMENU_NODE_VOTE_UP             (20)
#define LMENU_NODE_VOTE_DOWN           (21)

// Proxy types
#define PROXY_SOCKS4                   (1)
#define PROXY_SOCKS5                   (2)
#define PROXY_HTTP                     (3)

// InitNetReason
#define INIT_NET_REASON_NONE           (0)
#define INIT_NET_REASON_CACHE          (1)
#define INIT_NET_REASON_LOGIN          (2)
#define INIT_NET_REASON_REG            (3)
#define INIT_NET_REASON_LOAD           (4)

// Interface elements
#define IFACE_NONE                     (0)
#define IFACE_INT_ITEM                 (1)
#define IFACE_INT_CHSLOT               (2)
#define IFACE_INT_INV                  (3)
#define IFACE_INT_MENU                 (4)
#define IFACE_INT_SKILL                (5)
#define IFACE_INT_MAP                  (6)
#define IFACE_INT_CHAR                 (7)
#define IFACE_INT_PIP                  (8)
#define IFACE_INT_FIX                  (9)
#define IFACE_INT_ADDMESS              (10)
#define IFACE_INT_FILTER1              (11)
#define IFACE_INT_FILTER2              (12)
#define IFACE_INT_FILTER3              (13)
#define IFACE_INT_COMBAT_TURN          (14)
#define IFACE_INT_COMBAT_END           (15)
#define IFACE_INT_MAIN                 (16)
#define IFACE_INV_INV                  (20)
#define IFACE_INV_SLOT1                (21)
#define IFACE_INV_SLOT2                (22)
#define IFACE_INV_ARMOR                (23)
#define IFACE_INV_SLOTS_EXT            (24)
#define IFACE_INV_SCRUP                (25)
#define IFACE_INV_SCRDW                (26)
#define IFACE_INV_OK                   (27)
#define IFACE_INV_MAIN                 (28)
#define IFACE_USE_INV                  (40)
#define IFACE_USE_SCRUP                (41)
#define IFACE_USE_SCRDW                (42)
#define IFACE_USE_CANCEL               (43)
#define IFACE_USE_MAIN                 (44)
#define IFACE_GAME_MNEXT               (60)
#define IFACE_LOG_NAME                 (80)
#define IFACE_LOG_PASS                 (81)
#define IFACE_LOG_PLAY__NEWGAME        (82)
#define IFACE_LOG_REG__LOADGAME        (83)
#define IFACE_LOG_OPTIONS              (84)
#define IFACE_LOG_CREDITS              (85)
#define IFACE_LOG_EXIT                 (86)
#define IFACE_DLG_MAIN                 (100)
#define IFACE_DLG_SCR_UP               (101)
#define IFACE_DLG_SCR_DN               (102)
#define IFACE_DLG_ANSWER               (103)
#define IFACE_DLG_BARTER               (104)
#define IFACE_DLG_SAY                  (105)
#define IFACE_BARTER_OFFER             (106)
#define IFACE_BARTER_TALK              (107)
#define IFACE_BARTER_CONT1             (108)
#define IFACE_BARTER_CONT2             (109)
#define IFACE_BARTER_CONT1O            (110)
#define IFACE_BARTER_CONT2O            (111)
#define IFACE_BARTER_CONT1SU           (112)
#define IFACE_BARTER_CONT1SD           (113)
#define IFACE_BARTER_CONT2SU           (114)
#define IFACE_BARTER_CONT2SD           (115)
#define IFACE_BARTER_CONT1OSU          (116)
#define IFACE_BARTER_CONT1OSD          (117)
#define IFACE_BARTER_CONT2OSU          (118)
#define IFACE_BARTER_CONT2OSD          (119)
#define IFACE_LMAP_OK                  (120)
#define IFACE_LMAP_SCAN                (121)
#define IFACE_LMAP_LOHI                (122)
#define IFACE_LMAP_MAIN                (123)
#define IFACE_GMAP_MAP                 (140)
#define IFACE_GMAP_TOWN                (141)
#define IFACE_GMAP_TABBTN              (142)
#define IFACE_GMAP_TABSCRUP            (143)
#define IFACE_GMAP_TABSCRDW            (144)
#define IFACE_GMAP_TOLOC               (145)
#define IFACE_GMAP_TOWN_BUT            (146)
#define IFACE_GMAP_VIEW_BUT            (147)
#define IFACE_GMAP_INV                 (148)
#define IFACE_GMAP_MENU                (149)
#define IFACE_GMAP_CHA                 (150)
#define IFACE_GMAP_PIP                 (151)
#define IFACE_GMAP_FIX                 (152)
#define IFACE_GMAP_MOVE_MAP            (153)
#define IFACE_SBOX_CANCEL              (160)
#define IFACE_SBOX_MAIN                (161)
#define IFACE_SBOX_FIRSTAID            (162)
#define IFACE_SBOX_DOCTOR              (163)
#define IFACE_SBOX_SNEAK               (164)
#define IFACE_SBOX_LOCKPICK            (165)
#define IFACE_SBOX_STEAL               (166)
#define IFACE_SBOX_TRAP                (167)
#define IFACE_SBOX_SCIENCE             (168)
#define IFACE_SBOX_REPAIR              (169)
#define IFACE_MOPT_SAVEGAME            (180)
#define IFACE_MOPT_LOADGAME            (181)
#define IFACE_MOPT_OPTIONS             (182)
#define IFACE_MOPT_EXIT                (183)
#define IFACE_MOPT_RESUME              (184)
#define IFACE_CHA_PRINT                (200)
#define IFACE_CHA_OK                   (201)
#define IFACE_CHA_CANCEL               (202)
#define IFACE_CHA_PLUS                 (203)
#define IFACE_CHA_MINUS                (204)
#define IFACE_CHA_MAIN                 (205)
#define IFACE_CHA_NAME                 (206)
#define IFACE_CHA_AGE                  (207)
#define IFACE_CHA_SEX                  (208)
#define IFACE_CHA_SW_SCRUP             (209)
#define IFACE_CHA_SW_SCRDN             (210)
#define IFACE_REG_SPEC_PL              (211)
#define IFACE_REG_SPEC_MN              (212)
#define IFACE_REG_TAGSKILL             (213)
#define IFACE_REG_TRAIT_L              (214)
#define IFACE_REG_TRAIT_R              (215)
#define IFACE_CHA_NAME_NAME            (220)
#define IFACE_CHA_NAME_PASS            (221)
#define IFACE_CHA_AGE_UP               (222)
#define IFACE_CHA_AGE_DOWN             (223)
#define IFACE_CHA_SEX_MALE             (224)
#define IFACE_CHA_SEX_FEMALE           (225)
#define IFACE_PERK_MAIN                (240)
#define IFACE_PERK_SCRUP               (241)
#define IFACE_PERK_SCRDN               (242)
#define IFACE_PERK_OK                  (243)
#define IFACE_PERK_CANCEL              (244)
#define IFACE_PERK_PERKS               (245)
#define IFACE_TOWN_VIEW_MAIN           (250)
#define IFACE_TOWN_VIEW_BACK           (251)
#define IFACE_TOWN_VIEW_ENTER          (252)
#define IFACE_TOWN_VIEW_CONTOUR        (253)
#define IFACE_PIP_STATUS               (260)
// #define IFACE_PIP_GAMES        (261)
#define IFACE_PIP_AUTOMAPS             (262)
#define IFACE_PIP_AUTOMAPS_SCR         (263)
#define IFACE_PIP_ARCHIVES             (264)
#define IFACE_PIP_CLOSE                (265)
#define IFACE_PIP_MAIN                 (266)
#define IFACE_AIM_CANCEL               (280)
#define IFACE_AIM_HEAD                 (281)
#define IFACE_AIM_LARM                 (282)
#define IFACE_AIM_RARM                 (283)
#define IFACE_AIM_TORSO                (284)
#define IFACE_AIM_RLEG                 (285)
#define IFACE_AIM_LLEG                 (286)
#define IFACE_AIM_EYES                 (287)
#define IFACE_AIM_GROIN                (288)
#define IFACE_AIM_MAIN                 (289)
#define IFACE_PUP_CONT1                (300)
#define IFACE_PUP_CONT2                (301)
#define IFACE_PUP_OK                   (302)
#define IFACE_PUP_SCRUP1               (303)
#define IFACE_PUP_SCRDOWN1             (304)
#define IFACE_PUP_SCRUP2               (305)
#define IFACE_PUP_SCRDOWN2             (306)
#define IFACE_PUP_TAKEALL              (307)
#define IFACE_PUP_SCRCR_L              (308)
#define IFACE_PUP_SCRCR_R              (309)
#define IFACE_PUP_MAIN                 (310)
#define IFACE_DIALOG_BTN               (320)
#define IFACE_DIALOG_MAIN              (321)
#define IFACE_ELEVATOR_MAIN            (330)
#define IFACE_ELEVATOR_BTN             (331)
#define IFACE_SAY_OK                   (340)
#define IFACE_SAY_CANCEL               (341)
#define IFACE_SAY_MAIN                 (342)
#define IFACE_SPLIT_MAIN               (360)
#define IFACE_SPLIT_UP                 (361)
#define IFACE_SPLIT_DOWN               (362)
#define IFACE_SPLIT_ALL                (363)
#define IFACE_SPLIT_DONE               (364)
#define IFACE_SPLIT_CANCEL             (365)
#define IFACE_TIMER_MAIN               (380)
#define IFACE_TIMER_UP                 (381)
#define IFACE_TIMER_DOWN               (382)
#define IFACE_TIMER_DONE               (383)
#define IFACE_TIMER_CANCEL             (384)
#define IFACE_FIX_DONE                 (400)
#define IFACE_FIX_SCRUP                (401)
#define IFACE_FIX_SCRDN                (402)
#define IFACE_FIX_CHOOSE               (403)
#define IFACE_FIX_FIX                  (404)
#define IFACE_FIX_MAIN                 (405)
#define IFACE_IBOX_DONE                (420)
#define IFACE_IBOX_CANCEL              (421)
#define IFACE_IBOX_TITLE               (422)
#define IFACE_IBOX_TEXT                (423)
#define IFACE_IBOX_MAIN                (424)
#define IFACE_SAVELOAD_MAIN            (440)
#define IFACE_SAVELOAD_SCR_UP          (441)
#define IFACE_SAVELOAD_SCR_DN          (442)
#define IFACE_SAVELOAD_DONE            (443)
#define IFACE_SAVELOAD_BACK            (444)

#define ACCELERATE_NONE                (0)
#define ACCELERATE_CONSOLE             (1)
#define ACCELERATE_IBOX                (2)
#define ACCELERATE_PAGE_UP             (3)
#define ACCELERATE_PAGE_DOWN           (4)
#define ACCELERATE_MESSBOX             (5)
#define ACCELERATE_SPLIT_UP            (6)
#define ACCELERATE_SPLIT_DOWN          (7)
#define ACCELERATE_TIMER_UP            (8)
#define ACCELERATE_TIMER_DOWN          (9)
#define ACCELERATE_USE_SCRUP           (10)
#define ACCELERATE_USE_SCRDOWN         (11)
#define ACCELERATE_INV_SCRUP           (12)
#define ACCELERATE_INV_SCRDOWN         (13)
#define ACCELERATE_PUP_SCRUP1          (14)
#define ACCELERATE_PUP_SCRDOWN1        (15)
#define ACCELERATE_PUP_SCRUP2          (16)
#define ACCELERATE_PUP_SCRDOWN2        (17)
#define ACCELERATE_CHA_SW_SCRUP        (18)
#define ACCELERATE_CHA_SW_SCRDOWN      (19)
#define ACCELERATE_CHA_PLUS            (20)
#define ACCELERATE_CHA_MINUS           (21)
#define ACCELERATE_CHA_AGE_UP          (22)
#define ACCELERATE_CHA_AGE_DOWN        (23)
#define ACCELERATE_BARTER_CONT1SU      (24)
#define ACCELERATE_BARTER_CONT1SD      (25)
#define ACCELERATE_BARTER_CONT2SU      (26)
#define ACCELERATE_BARTER_CONT2SD      (27)
#define ACCELERATE_BARTER_CONT1OSU     (28)
#define ACCELERATE_BARTER_CONT1OSD     (29)
#define ACCELERATE_BARTER_CONT2OSU     (30)
#define ACCELERATE_BARTER_CONT2OSD     (31)
#define ACCELERATE_PERK_SCRUP          (32)
#define ACCELERATE_PERK_SCRDOWN        (33)
#define ACCELERATE_DLG_TEXT_UP         (34)
#define ACCELERATE_DLG_TEXT_DOWN       (35)
#define ACCELERATE_SAVE_LOAD_SCR_UP    (36)
#define ACCELERATE_SAVE_LOAD_SCR_DN    (37)

#endif // __CLIENT__
