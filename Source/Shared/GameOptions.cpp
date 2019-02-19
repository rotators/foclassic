#include "Core.h"

#include "CommandLine.h"
#include "FileSystem.h"
#include "GameOptions.h"
#include "Random.h"
#include "Timer.h"

#pragma TODO("Better solution for LogToFunc")
typedef void (* LogFuncPtr)( const char* str );
extern void LogToFunc( LogFuncPtr func_ptr, bool enable );

GameOptions    GameOpt;
GameOptionsExt GameOptExt;

GameOptions::GameOptions()
{
    YearStart = 2078;
    Year = 2078;
    Month = 9;
    Day = 8;
    Hour = 9;
    Minute = 44;
    Second = 0;
    FullSecondStart = 0;
    FullSecond = 0;
    TimeMultiplier = 0;
    GameTimeTick = 0;

    DisableTcpNagle = false;
    DisableZlibCompression = false;
    FloodSize = 2048;
    NoAnswerShuffle = false;
    DialogDemandRecheck = false;
    FixBoyDefaultExperience = 50;
    SneakDivider = 6;
    LevelCap = 99;
    LevelCapAddExperience = false;
    LookNormal = 20;
    LookMinimum = 6;
    WorldmapMaxGroupCount = 10;
    CritterIdleTick = 10000;
    TurnBasedTick = 30000;
    DeadHitPoints = -6;

    Breaktime = 1200;
    TimeoutTransfer = 3;
    TimeoutBattle = 10;
    ApRegeneration = 10000;
    RtApCostCritterWalk = 0;
    RtApCostCritterRun = 1;
    RtApCostMoveItemContainer = 0;
    RtApCostMoveItemInventory = 2;
    RtApCostPickItem = 1;
    RtApCostDropItem = 1;
    RtApCostReloadWeapon = 2;
    RtApCostPickCritter = 1;
    RtApCostUseItem = 3;
    RtApCostUseSkill = 2;
    RtAlwaysRun = false;
    TbApCostCritterMove = 1;
    TbApCostMoveItemContainer = 0;
    TbApCostMoveItemInventory = 1;
    TbApCostPickItem = 3;
    TbApCostDropItem = 1;
    TbApCostReloadWeapon = 2;
    TbApCostPickCritter = 3;
    TbApCostUseItem = 3;
    TbApCostUseSkill = 3;
    TbAlwaysRun = false;
    ApCostAimEyes = 1;
    ApCostAimHead = 1;
    ApCostAimGroin = 1;
    ApCostAimTorso = 1;
    ApCostAimArms = 1;
    ApCostAimLegs = 1;
    RunOnCombat = false;
    RunOnTransfer = false;
    GlobalMapWidth = 28;
    GlobalMapHeight = 30;
    GlobalMapZoneLength = 50;
    GlobalMapMoveTime = 500;
    BagRefreshTime = 60;
    AttackAnimationsMinDist = 0;
    WhisperDist = 2;
    ShoutDist = 200;
    LookChecks = 0;
    LookDir[0] = 0;
    LookDir[1] = 20;
    LookDir[2] = 40;
    LookDir[3] = 60;
    LookDir[4] = 60;
    LookSneakDir[0] = 90;
    LookSneakDir[1] = 60;
    LookSneakDir[2] = 30;
    LookSneakDir[3] = 0;
    LookSneakDir[4] = 0;
    LookWeight = 200;
    CustomItemCost = false;
    RegistrationTimeout = 5;
    AccountPlayTime = 0;
    LoggingVars = false;
    ScriptRunSuspendTimeout = 30000;
    ScriptRunMessageTimeout = 10000;
    TalkDistance = 3;
    NpcMaxTalkers = 1;
    MinNameLength = 4;
    MaxNameLength = 12;
    DlgTalkMinTime = 0;
    DlgBarterMinTime = 0;
    MinimumOfflineTime = 180000;

    StartSpecialPoints = 40;
    StartTagSkillPoints = 3;

    SkillMaxValue = 300;
    SkillModAdd2 = 100;
    SkillModAdd3 = 125;
    SkillModAdd4 = 150;
    SkillModAdd5 = 175;
    SkillModAdd6 = 200;

    AbsoluteOffsets = true;
    SkillBegin = 200;
    SkillEnd = 217;
    TimeoutBegin = 230;
    TimeoutEnd = 249;
    KillBegin = 260;
    KillEnd = 278;
    PerkBegin = 300;
    PerkEnd = 435;
    AddictionBegin = 470;
    AddictionEnd = 476;
    KarmaBegin = 480;
    KarmaEnd = 495;
    DamageBegin = 500;
    DamageEnd = 506;
    TraitBegin = 550;
    TraitEnd = 565;
    ReputationBegin = 570;
    ReputationEnd = 599;

    ReputationLoved = 30;
    ReputationLiked = 15;
    ReputationAccepted = 1;
    ReputationNeutral = 0;
    ReputationAntipathy = -14;
    ReputationHated = -29;

    MapHexagonal = true;
    MapHexWidth = 32;
    MapHexHeight = 16;
    MapHexLineHeight = 12;
    MapTileOffsX = -8;
    MapTileOffsY = 32;
    MapRoofOffsX = -8;
    MapRoofOffsY = -66;
    MapRoofSkipSize = 2;
    MapCameraAngle = 25.7f;
    MapSmoothPath = true;
    MapDataPrefix = "art/geometry/fallout_";

    // Client and Mapper
    Quit = false;
    OpenGLDebug = false;
    AssimpLogging = false;
    MouseX = 0;
    MouseY = 0;
    ScrOx = 0;
    ScrOy = 0;
    ShowTile = true;
    ShowRoof = true;
    ShowItem = true;
    ShowScen = true;
    ShowWall = true;
    ShowCrit = true;
    ShowFast = true;
    ShowPlayerNames = false;
    ShowNpcNames = false;
    ShowCritId = false;
    ScrollKeybLeft = false;
    ScrollKeybRight = false;
    ScrollKeybUp = false;
    ScrollKeybDown = false;
    ScrollMouseLeft = false;
    ScrollMouseRight = false;
    ScrollMouseUp = false;
    ScrollMouseDown = false;
    ShowGroups = true;
    HelpInfo = false;
    DebugInfo = false;
    DebugNet = false;
    DebugSprites = false;
    FullScreen = false;
    VSync = false;
    FlushVal = 100;
    BaseTexture = 256;
    Light = 0;
    Host = "localhost";
    Port = 0;
    ProxyType = 0;
    ProxyHost = "";
    ProxyPort = 0;
    ProxyUser = "";
    ProxyPass = "";
    Name = "";
    ScrollDelay = 10;
    ScrollStep = 1;
    ScrollCheck = true;
    DataPath = "";
    FixedFPS = 100;
    FPS = 0;
    PingPeriod = 2000;
    Ping = 0;
    MsgboxInvert = false;
    DefaultCombatMode = COMBAT_MODE_ANY;
    MessNotify = true;
    SoundNotify = true;
    AlwaysOnTop = false;
    TextDelay = 3000;
    DamageHitDelay = 0;
    ScreenWidth = 800;
    ScreenHeight = 600;
    MultiSampling = 0;
    MouseScroll = true;
    IndicatorType = INDICATOR_LINES;
    DoubleClickTime = 0;
    RoofAlpha = 200;
    HideCursor = false;
    DisableLMenu = false;
    DisableMouseEvents = false;
    DisableKeyboardEvents = false;
    HidePassword = true;
    PlayerOffAppendix = "_off";
    CombatMessagesType = 0;
    Animation3dSmoothTime = 250;
    Animation3dFPS = 10;
    RunModMul = 1;
    RunModDiv = 3;
    RunModAdd = 0;
    MapZooming = false;
    SpritesZoom = 1.0f;
    SpritesZoomMax = MAX_ZOOM;
    SpritesZoomMin = MIN_ZOOM;
    memzero( EffectValues, sizeof(EffectValues) );
    AlwaysRun = false;
    AlwaysRunMoveDist = 1;
    AlwaysRunUseDist = 5;
    KeyboardRemap = "";
    CritterFidgetTime = 50000;
    Anim2CombatBegin = 0;
    Anim2CombatIdle = 0;
    Anim2CombatEnd = 0;
    RainTick = 60;
    RainSpeedX = 0;
    RainSpeedY = 15;

    // Mapper
    ClientPath = DIR_SLASH_SD;
    ServerPath = DIR_SLASH_SD;
    ShowCorners = false;
    ShowSpriteCuts = false;
    ShowDrawOrder = false;
    SplitTilesCollection = true;

    // Engine data
    CritterChangeParameter = NULL;
    CritterTypes = NULL;

    ClientMap = NULL;
    ClientMapLight = NULL;
    ClientMapWidth = 0;
    ClientMapHeight = 0;

    GetDrawingSprites = NULL;
    GetSpriteInfo = NULL;
    GetSpriteColor = NULL;
    IsSpriteHit = NULL;

    GetNameByHash = NULL;
    GetHashByName = NULL;

    ScriptLoadModule = NULL;
    ScriptBind = NULL;
    ScriptPrepare = NULL;
    ScriptSetArgInt8 = NULL;
    ScriptSetArgInt16 = NULL;
    ScriptSetArgInt = NULL;
    ScriptSetArgInt64 = NULL;
    ScriptSetArgUInt8 = NULL;
    ScriptSetArgUInt16 = NULL;
    ScriptSetArgUInt = NULL;
    ScriptSetArgUInt64 = NULL;
    ScriptSetArgBool = NULL;
    ScriptSetArgFloat = NULL;
    ScriptSetArgDouble = NULL;
    ScriptSetArgObject = NULL;
    ScriptSetArgAddress = NULL;
    ScriptRunPrepared = NULL;
    ScriptGetReturnedInt8 = NULL;
    ScriptGetReturnedInt16 = NULL;
    ScriptGetReturnedInt = NULL;
    ScriptGetReturnedInt64 = NULL;
    ScriptGetReturnedUInt8 = NULL;
    ScriptGetReturnedUInt16 = NULL;
    ScriptGetReturnedUInt = NULL;
    ScriptGetReturnedUInt64 = NULL;
    ScriptGetReturnedBool = NULL;
    ScriptGetReturnedFloat = NULL;
    ScriptGetReturnedDouble = NULL;
    ScriptGetReturnedObject = NULL;
    ScriptGetReturnedAddress = NULL;

    Random = &::Random;
    GetTick = &Timer::FastTick;
    SetLogCallback = &LogToFunc;

    // Callbacks
    GetUseApCost = NULL;
    GetAttackDistantion = NULL;
    GetRainOffset = NULL;
}

GameOptionsExt::GameOptionsExt()
{
    WallAlpha = 0xFF;

    CommandLine = NULL;
    ConfigFile = NULL;
}
