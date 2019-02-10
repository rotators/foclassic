/************************************************************************/
/* Game options                                                         */
/************************************************************************/

#ifndef __GAME_OPTIONS__
#define __GAME_OPTIONS__

#include "scriptstring.h"

#include "Defines.h"
#include "Types.h"

struct GameOptions
{
    uint16       YearStart;
    uint         YearStartFTLo;
    uint         YearStartFTHi;
    uint16       Year;
    uint16       Month;
    uint16       Day;
    uint16       Hour;
    uint16       Minute;
    uint16       Second;
    uint         FullSecondStart;
    uint         FullSecond;
    uint16       TimeMultiplier;
    uint         GameTimeTick;

    bool         DisableTcpNagle;
    bool         DisableZlibCompression;
    uint         FloodSize;
    bool         NoAnswerShuffle;
    bool         DialogDemandRecheck;
    uint         FixBoyDefaultExperience;
    uint         SneakDivider;
    uint         LevelCap;
    bool         LevelCapAddExperience;
    uint         LookNormal;
    uint         LookMinimum;
    uint         WorldmapMaxGroupCount;
    uint         CritterIdleTick;
    uint         TurnBasedTick;
    int          DeadHitPoints;
    uint         Breaktime;
    uint         TimeoutTransfer;
    uint         TimeoutBattle;
    uint         ApRegeneration;
    uint         RtApCostCritterWalk;
    uint         RtApCostCritterRun;
    uint         RtApCostMoveItemContainer;
    uint         RtApCostMoveItemInventory;
    uint         RtApCostPickItem;
    uint         RtApCostDropItem;
    uint         RtApCostReloadWeapon;
    uint         RtApCostPickCritter;
    uint         RtApCostUseItem;
    uint         RtApCostUseSkill;
    bool         RtAlwaysRun;
    uint         TbApCostCritterMove;
    uint         TbApCostMoveItemContainer;
    uint         TbApCostMoveItemInventory;
    uint         TbApCostPickItem;
    uint         TbApCostDropItem;
    uint         TbApCostReloadWeapon;
    uint         TbApCostPickCritter;
    uint         TbApCostUseItem;
    uint         TbApCostUseSkill;
    bool         TbAlwaysRun;
    uint         ApCostAimEyes;
    uint         ApCostAimHead;
    uint         ApCostAimGroin;
    uint         ApCostAimTorso;
    uint         ApCostAimArms;
    uint         ApCostAimLegs;
    bool         RunOnCombat;
    bool         RunOnTransfer;
    uint         GlobalMapWidth;
    uint         GlobalMapHeight;
    uint         GlobalMapZoneLength;
    uint         GlobalMapMoveTime;
    uint         BagRefreshTime;
    uint         AttackAnimationsMinDist;
    uint         WhisperDist;
    uint         ShoutDist;
    int          LookChecks;
    uint         LookDir[5];
    uint         LookSneakDir[5];
    uint         LookWeight;
    bool         CustomItemCost;
    uint         RegistrationTimeout;
    uint         AccountPlayTime;
    bool         LoggingVars;
    uint         ScriptRunSuspendTimeout;
    uint         ScriptRunMessageTimeout;
    uint         TalkDistance;
    uint         NpcMaxTalkers;
    uint         MinNameLength;
    uint         MaxNameLength;
    uint         DlgTalkMinTime;
    uint         DlgBarterMinTime;
    uint         MinimumOfflineTime;

    int          StartSpecialPoints;
    int          StartTagSkillPoints;
    int          SkillMaxValue;
    int          SkillModAdd2;
    int          SkillModAdd3;
    int          SkillModAdd4;
    int          SkillModAdd5;
    int          SkillModAdd6;

    bool         AbsoluteOffsets;
    uint         SkillBegin;
    uint         SkillEnd;
    uint         TimeoutBegin;
    uint         TimeoutEnd;
    uint         KillBegin;
    uint         KillEnd;
    uint         PerkBegin;
    uint         PerkEnd;
    uint         AddictionBegin;
    uint         AddictionEnd;
    uint         KarmaBegin;
    uint         KarmaEnd;
    uint         DamageBegin;
    uint         DamageEnd;
    uint         TraitBegin;
    uint         TraitEnd;
    uint         ReputationBegin;
    uint         ReputationEnd;

    int          ReputationLoved;
    int          ReputationLiked;
    int          ReputationAccepted;
    int          ReputationNeutral;
    int          ReputationAntipathy;
    int          ReputationHated;

    bool         MapHexagonal;
    int          MapHexWidth;
    int          MapHexHeight;
    int          MapHexLineHeight;
    int          MapTileOffsX;
    int          MapTileOffsY;
    int          MapRoofOffsX;
    int          MapRoofOffsY;
    int          MapRoofSkipSize;
    float        MapCameraAngle;
    bool         MapSmoothPath;
    ScriptString MapDataPrefix;

    // Client and Mapper
    bool         Quit;
    bool         OpenGLRendering;
    bool         OpenGLDebug;
    bool         AssimpLogging;
    int          MouseX;
    int          MouseY;
    int          ScrOx;
    int          ScrOy;
    bool         ShowTile;
    bool         ShowRoof;
    bool         ShowItem;
    bool         ShowScen;
    bool         ShowWall;
    bool         ShowCrit;
    bool         ShowFast;
    bool         ShowPlayerNames;
    bool         ShowNpcNames;
    bool         ShowCritId;
    bool         ScrollKeybLeft;
    bool         ScrollKeybRight;
    bool         ScrollKeybUp;
    bool         ScrollKeybDown;
    bool         ScrollMouseLeft;
    bool         ScrollMouseRight;
    bool         ScrollMouseUp;
    bool         ScrollMouseDown;
    bool         ShowGroups;
    bool         HelpInfo;
    bool         DebugInfo;
    bool         DebugNet;
    bool         DebugSprites;
    bool         FullScreen;
    bool         VSync;
    int          FlushVal;
    int          BaseTexture;
    int          Light;
    ScriptString Host;
    uint         Port;
    uint         ProxyType;
    ScriptString ProxyHost;
    uint         ProxyPort;
    ScriptString ProxyUser;
    ScriptString ProxyPass;
    ScriptString Name;
    uint         ScrollDelay;
    int          ScrollStep;
    bool         ScrollCheck;
    ScriptString DataPath;
    int          FixedFPS;
    uint         FPS;
    uint         PingPeriod;
    uint         Ping;
    bool         MsgboxInvert;
    uint8        DefaultCombatMode;
    bool         MessNotify;
    bool         SoundNotify;
    bool         AlwaysOnTop;
    uint         TextDelay;
    uint         DamageHitDelay;
    int          ScreenWidth;
    int          ScreenHeight;
    int          MultiSampling;
    bool         MouseScroll;
    int          IndicatorType;
    uint         DoubleClickTime;
    uint8        RoofAlpha;
    bool         HideCursor;
    bool         DisableLMenu;
    bool         DisableMouseEvents;
    bool         DisableKeyboardEvents;
    bool         HidePassword;
    ScriptString PlayerOffAppendix;
    int          CombatMessagesType;
    uint         Animation3dSmoothTime;
    uint         Animation3dFPS;
    int          RunModMul;
    int          RunModDiv;
    int          RunModAdd;
    bool         MapZooming;
    float        SpritesZoom;
    float        SpritesZoomMax;
    float        SpritesZoomMin;
    float        EffectValues[EFFECT_SCRIPT_VALUES];
    bool         AlwaysRun;
    uint         AlwaysRunMoveDist;
    uint         AlwaysRunUseDist;
    ScriptString KeyboardRemap;
    uint         CritterFidgetTime;
    uint         Anim2CombatBegin;
    uint         Anim2CombatIdle;
    uint         Anim2CombatEnd;
    uint         RainTick;
    short        RainSpeedX;
    short        RainSpeedY;

    // Mapper
    ScriptString ClientPath;
    ScriptString ServerPath;
    bool         ShowCorners;
    bool         ShowSpriteCuts;
    bool         ShowDrawOrder;
    bool         SplitTilesCollection;

    // Engine data
    void         ( * CritterChangeParameter )( void*, uint );
    void*        CritterTypes;

    void*        ClientMap;
    uint8*       ClientMapLight;
    uint         ClientMapWidth;
    uint         ClientMapHeight;

    void*        ( * GetDrawingSprites )( uint& );
    void*        ( * GetSpriteInfo )( uint );
    uint         ( * GetSpriteColor )( uint, int, int, bool );
    bool         ( * IsSpriteHit )( void*, int, int, bool );

    const char*  ( * GetNameByHash )( uint );
    uint         ( * GetHashByName )( const char* );

    bool         ( * ScriptLoadModule )( const char* );
    uint         ( * ScriptBind )( const char*, const char*, bool );
    bool         ( * ScriptPrepare )( uint );
    void         ( * ScriptSetArgInt8 )( char );
    void         ( * ScriptSetArgInt16 )( short );
    void         ( * ScriptSetArgInt )( int );
    void         ( * ScriptSetArgInt64 )( int64 );
    void         ( * ScriptSetArgUInt8 )( uint8 );
    void         ( * ScriptSetArgUInt16 )( uint16 );
    void         ( * ScriptSetArgUInt )( uint );
    void         ( * ScriptSetArgUInt64 )( uint64 );
    void         ( * ScriptSetArgBool )( bool );
    void         ( * ScriptSetArgFloat )( float );
    void         ( * ScriptSetArgDouble )( double );
    void         ( * ScriptSetArgObject )( void* );
    void         ( * ScriptSetArgAddress )( void* );
    bool         ( * ScriptRunPrepared )();
    char         ( * ScriptGetReturnedInt8 )();
    short        ( * ScriptGetReturnedInt16 )();
    int          ( * ScriptGetReturnedInt )();
    int64        ( * ScriptGetReturnedInt64 )();
    uint8        ( * ScriptGetReturnedUInt8 )();
    uint16       ( * ScriptGetReturnedUInt16 )();
    uint         ( * ScriptGetReturnedUInt )();
    uint64       ( * ScriptGetReturnedUInt64 )();
    bool         ( * ScriptGetReturnedBool )();
    float        ( * ScriptGetReturnedFloat )();
    double       ( * ScriptGetReturnedDouble )();
    void*        ( * ScriptGetReturnedObject )();
    void*        ( * ScriptGetReturnedAddress )();

    int          ( * Random )( int, int );
    uint         ( * GetTick )();
    void         ( * SetLogCallback )( void (*)( const char* str ), bool );

    // Callbacks
    uint         ( * GetUseApCost )( void*, void*, uint8 );
    uint         ( * GetAttackDistantion )( void*, void*, uint8 );
    void         ( * GetRainOffset )( short*, short* );

    GameOptions();
};

extern struct GameOptions GameOpt;

struct GameOptionsExt
{
    uint8 WallAlpha;

    void* CommandLine;
    void* ConfigFile;

    GameOptionsExt();
};

extern struct GameOptionsExt GameOptExt;

#endif // __GAME_OPTIONS__ //
