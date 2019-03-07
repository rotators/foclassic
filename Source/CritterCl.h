#ifndef __CRITTER_CL__
#define __CRITTER_CL__

#include "scriptstring.h"

#include "3dStuff.h"
#include "Defines.h"
#include "FlexRect.h"
#include "GraphicStructures.h"
#include "Item.h"
#include "SpriteManager.h"
#include "Types.h"

class CritterCl
{
public:
    uint         Id;
    uint16       Pid;
    uint16       HexX, HexY;
    uint8        CrDir;
    int          Params[MAX_PARAMS];
    uint         NameColor;
    uint         ContourColor;
    UInt16Vec    LastHexX, LastHexY;
    uint8        Cond;
    uint         Anim1Life;
    uint         Anim1Knockout;
    uint         Anim1Dead;
    uint         Anim2Life;
    uint         Anim2Knockout;
    uint         Anim2Dead;
    uint         Flags;
    uint         BaseType, BaseTypeAlias;
    uint         ApRegenerationTick;
    short        Multihex;
    Effect*      DrawEffect;

    ScriptString Name;
    ScriptString NameOnHead;
    ScriptString Lexems;
    ScriptString Avatar;
    std::string  Pass;

    ItemPtrVec   InvItems;
    Item*        DefItemSlotHand;
    Item*        DefItemSlotArmor;
    Item*        ItemSlotMain;
    Item*        ItemSlotExt;
    Item*        ItemSlotArmor;

    static bool  ParamsRegEnabled[MAX_PARAMS];
    static int   ParamsReg[MAX_PARAMS];
    CritterCl*   ThisPtr[MAX_PARAMETERS_ARRAYS];
    static int   ParamsChangeScript[MAX_PARAMS];
    static int   ParamsGetScript[MAX_PARAMS];
    static uint  ParametersMin[MAX_PARAMETERS_ARRAYS];
    static uint  ParametersMax[MAX_PARAMETERS_ARRAYS];
    static bool  ParametersOffset[MAX_PARAMETERS_ARRAYS];
    bool         ParamsIsChanged[MAX_PARAMS];
    IntVec       ParamsChanged;
    int          ParamLocked;
    static bool  SlotEnabled[0x100];

    CritterCl();
    ~CritterCl();
    void Init();
    void InitForRegistration();
    void Finish();
    void GenParams();

    uint        GetId()   { return Id; }
    const char* GetInfo() { return Name.c_str(); }
    uint16      GetHexX() { return HexX; }
    uint16      GetHexY() { return HexY; }
    bool        IsLastHexes();
    void        FixLastHexes();
    uint16      PopLastHexX();
    uint16      PopLastHexY();
    void        SetBaseType( uint type );
    void        SetDir( uint8 dir );
    uint8       GetDir() { return CrDir; }
    uint        GetCrTypeAlias();

    void Animate( uint anim1, uint anim2, Item* item );
    void AnimateStay();
    void Action( int action, int action_ext, Item* item, bool local_call = true );
    void Process();

    int         GetCond() { return Cond; }
    void        DrawStay( Rect r );
    const char* GetName() { return Name.c_str(); }
    const char* GetPass() { return Pass.c_str(); }

    bool IsNpc()      { return FLAG( Flags, CRITTER_FLAG_NPC ); }
    bool IsPlayer()   { return FLAG( Flags, CRITTER_FLAG_PLAYER ); }
    bool IsChosen()   { return FLAG( Flags, CRITTER_FLAG_CHOSEN ); }
    bool IsGmapRule() { return FLAG( Flags, CRITTER_FLAG_RULEGROUP ); }
    bool IsOnline()   { return !FLAG( Flags, CRITTER_FLAG_DISCONNECT ); }
    bool IsOffline()  { return FLAG( Flags, CRITTER_FLAG_DISCONNECT ); }
    bool IsLife()     { return Cond == CRITTER_CONDITION_LIFE; }
    bool IsKnockout() { return Cond == CRITTER_CONDITION_KNOCKOUT; }
    bool IsDead()     { return Cond == CRITTER_CONDITION_DEAD; }
    bool CheckFind( int find_type );
    bool IsCanTalk()    { return IsNpc() && IsLife() && IsRawParam( ST_DIALOG_ID ) && !IsRawParam( MODE_NO_TALK ); }
    bool IsCombatMode() { return GetParam( TO_BATTLE ) != 0; }
    bool IsTurnBased()  { return TB_BATTLE_TIMEOUT_CHECK( GetParam( TO_BATTLE ) ); }

    uint GetLook();
    uint GetTalkDistance();
    uint GetAttackDist();
    uint GetUseDist();
    uint GetMultihex();

    int  GetParam( uint index );
    bool IsRawParam( uint index )  { return Params[index] != 0; }
    int  GetRawParam( uint index ) { return Params[index]; }
    void ChangeParam( uint index );
    void ProcessChangedParams();

    bool IsTagSkill( int index ) { return Params[TAG_SKILL1] == index || Params[TAG_SKILL2] == index || Params[TAG_SKILL3] == index || Params[TAG_SKILL4] == index; }
    uint GetMaxWeightKg()        { return GetParam( ST_CARRY_WEIGHT ) / 1000; }
    uint GetMaxVolume()          { return CRITTER_INV_VOLUME; }
    uint GetCrType();
    bool IsDmgLeg()    { return IsRawParam( DAMAGE_RIGHT_LEG ) || IsRawParam( DAMAGE_LEFT_LEG ); }
    bool IsDmgTwoLeg() { return IsRawParam( DAMAGE_RIGHT_LEG ) && IsRawParam( DAMAGE_LEFT_LEG ); }
    bool IsDmgArm()    { return IsRawParam( DAMAGE_RIGHT_ARM ) || IsRawParam( DAMAGE_LEFT_ARM ); }
    bool IsDmgTwoArm() { return IsRawParam( DAMAGE_RIGHT_ARM ) && IsRawParam( DAMAGE_LEFT_ARM ); }
    int  GetRealAp()   { return Params[ST_CURRENT_AP]; }
    int  GetAllAp()    { return GetParam( ST_CURRENT_AP ) + GetParam( ST_MOVE_AP ); }
    void SubMoveAp( int val )
    {
        ChangeParam( ST_CURRENT_AP );
        Params[ST_MOVE_AP] -= val;
    }
    void SubAp( int val )
    {
        ChangeParam( ST_CURRENT_AP );
        Params[ST_CURRENT_AP] -= val * AP_DIVIDER;
        ApRegenerationTick = 0;
    }
    bool IsHideMode() { return GetRawParam( MODE_HIDE ) != 0; }

    // Items
public:
    void        AddItem( Item* item );
    void        EraseItem( Item* item, bool animate );
    void        EraseAllItems();
    Item*       GetItem( uint item_id );
    Item*       GetItemByPid( uint16 item_pid );
    Item*       GetItemByPidInvPriority( uint16 item_pid );
    Item*       GetItemByPidSlot( uint16 item_pid, int slot );
    Item*       GetAmmo( uint caliber );
    Item*       GetItemSlot( int slot );
    void        GetItemsSlot( int slot, ItemPtrVec& items );
    void        GetItemsType( int slot, ItemPtrVec& items );
    uint        CountItemPid( uint16 item_pid );
    uint        CountItemType( uint8 type );
    bool        IsCanSortItems();
    Item*       GetItemHighSortValue();
    Item*       GetItemLowSortValue();
    void        GetInvItems( ItemVec& items );
    uint        GetItemsCount();
    uint        GetItemsCountInv();
    uint        GetItemsWeight();
    uint        GetItemsWeightKg();
    uint        GetItemsVolume();
    int         GetFreeWeight();
    int         GetFreeVolume();
    bool        IsHaveLightSources();
    Item*       GetSlotUse( uint8 num_slot, uint8& use );
    uint        GetUsePicName( uint8 num_slot );
    bool        IsItemAim( uint8 num_slot );
    uint8       GetUse()      { return ItemSlotMain->Data.Mode & 0xF; }
    uint8       GetFullRate() { return ItemSlotMain->Data.Mode; }
    bool        NextRateItem( bool prev );
    uint8       GetAim() { return (ItemSlotMain->Data.Mode >> 4) & 0xF; }
    bool        IsAim()  { return GetAim() > 0; }
    void        SetAim( uint8 hit_location );
    uint        GetUseApCost( Item* item, uint8 rate );
    ProtoItem*  GetUnarmedItem( uint8 tree, uint8 priority );
    ProtoItem*  GetProtoMain() { return ItemSlotMain->Proto; }
    ProtoItem*  GetProtoExt()  { return ItemSlotExt->Proto; }
    ProtoItem*  GetProtoArm()  { return ItemSlotArmor->Proto; }
    const char* GetMoneyStr();
    Item*       GetAmmoAvialble( Item* weap );
    bool        IsOverweight()       { return (int)GetItemsWeight() > GetParam( ST_CARRY_WEIGHT ); }
    bool        IsDoubleOverweight() { return (int)GetItemsWeight() > GetParam( ST_CARRY_WEIGHT ) * 2; }

    // Moving
public:
    bool          IsRunning;
    UShortPairVec MoveSteps;
    int           CurMoveStep;
    bool IsNeedMove() { return MoveSteps.size() && !IsWalkAnim(); }
    void ZeroSteps()
    {
        MoveSteps.clear();
        CurMoveStep = 0;
    }
    void Move( int dir );

    // ReSet
private:
    bool needReSet;
    uint reSetTick;

public:
    bool IsNeedReSet();
    void ReSetOk();

    // Time
public:
    uint TickCount;
    uint StartTick;

    void TickStart( uint ms );
    void TickNull();
    bool IsFree();

    // Animation
public:
    static AnyFrames* DefaultAnim;
    void*             Layers3d;
    uint GetAnim1( Item* anim_item = NULL );
    uint GetAnim2();
    void ProcessAnim( bool animate_stay, bool is2d, uint anim1, uint anim2, Item* item );
    int* GetLayers3dData();
    bool IsAnimAviable( uint anim1, uint anim2 );

private:
    uint curSpr, lastEndSpr;
    uint animStartTick;

    struct CritterAnim
    {
        AnyFrames* Anim;
        uint       AnimTick;
        int        BeginFrm;
        int        EndFrm;
        bool       MoveText;
        int        DirOffs;
        uint       IndCrType, IndAnim1, IndAnim2;
        Item*      ActiveItem;
        CritterAnim() {}
        CritterAnim( AnyFrames* anim, uint tick, int beg_frm, int end_frm, bool move_text, int dir_offs, uint ind_crtype, uint ind_anim1, uint ind_anim2, Item* item ) : Anim( anim ), AnimTick( tick ), BeginFrm( beg_frm ), EndFrm( end_frm ), MoveText( move_text ), DirOffs( dir_offs ),
            IndCrType( ind_crtype ), IndAnim1( ind_anim1 ), IndAnim2( ind_anim2 ), ActiveItem( item ) {}
    };
    typedef std::vector<CritterAnim> CritterAnimVec;

    CritterAnimVec animSequence;
    CritterAnim    stayAnim;

    CritterAnim* GetCurAnim() { return IsAnim() ? &animSequence[0] : NULL; }
    void         NextAnim( bool erase_front );

public:
    Animation3d* Anim3d;
    Animation3d* Anim3dStay;
    bool         Visible;
    uint8        Alpha;
    Rect         DRect;
    bool         SprDrawValid;
    Sprite*      SprDraw;
    uint         SprId;
    short        SprOx, SprOy;
    // Extra offsets
    short        OxExtI, OyExtI;
    float        OxExtF, OyExtF;
    float        OxExtSpeed, OyExtSpeed;
    uint         OffsExtNextTick;

    void         SetSprRect();
    bool         Is3dAnim()  { return Anim3d != NULL; }
    Animation3d* GetAnim3d() { return Anim3d; }
    bool         IsAnim()    { return animSequence.size() > 0; }
    bool         IsWalkAnim();
    void         ClearAnim();

    void SetOffs( short set_ox, short set_oy, bool move_text );
    void ChangeOffs( short change_ox, short change_oy, bool move_text );
    void AccamulateOffs();
    void AddOffsExt( short ox, short oy );
    void GetWalkHexOffsets( int dir, int& ox, int& oy );

    // Stay sprite
private:
    int  staySprDir;
    uint staySprTick;

    // Finish
private:
    uint finishingTime;

public:
    bool IsFinishing();
    bool IsFinish();

    // Fade
// private:
public:
    bool fadingEnable;
private:
    bool fadeUp;

    void  SetFade( bool fade_up );
    uint8 GetFadeAlpha();

public:
    uint FadingTick;

    // Text
public:
    Rect GetTextRect();
    void SetText( const char* str, uint color, uint text_delay );
    void GetNameTextInfo( bool& nameVisible, int& x, int& y, int& w, int& h, int& lines );
    void DrawTextOnHead();

private:
    Rect        textRect;
    uint        tickFidget;
    std::string strTextOnHead;
    uint        tickStartText;
    uint        tickTextDelay;
    uint        textOnHeadColor;

    // Ap cost
public:
    int GetApCostCritterMove( bool is_run );
    int GetApCostMoveItemContainer();
    int GetApCostMoveItemInventory();
    int GetApCostPickItem();
    int GetApCostDropItem();
    int GetApCostPickCritter();
    int GetApCostUseSkill();

    // Ref counter
public:
    short RefCounter;
    bool  IsNotValid;
    void AddRef() { RefCounter++; }
    void Release()
    {
        RefCounter--;
        if( RefCounter <= 0 ) delete this;
    }
};

typedef std::map<uint, CritterCl*, std::less<uint>> CritMap;
typedef std::vector<CritterCl*>                     CritVec;
typedef CritterCl*                                  CritterClPtr;

#endif // __CRITTER_CL__ //
