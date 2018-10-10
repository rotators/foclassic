#ifndef __SCRIPT_FUNCTIONS__
#define __SCRIPT_FUNCTIONS__

struct ReservedScriptFunction
{
    int* BindId;
    char FuncName[256];
    char FuncDecl[256];
};

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_SERVER)

struct ClientScriptFunctions
{
    int Start;
    int Loop;
    int GetActiveScreens;
    int ScreenChange;
    int RenderIface;
//  int RenderIfaceScreen;
    int RenderMap;
    int MouseDown;
    int MouseUp;
    int MouseMove;
    int KeyDown;
    int KeyUp;
    int InputLost;
    int CritterIn;
    int CritterOut;
    int ItemMapIn;
    int ItemMapChanged;
    int ItemMapOut;
    int ItemInvIn;
    int ItemInvOut;
    int MapMessage;
    int InMessage;
    int OutMessage;
    int ToHit;
    int HitAim;
    int CombatResult;
    int GenericDesc;
    int ItemLook;
    int CritterLook;
    int GetElevator;
    int ItemCost;
    int PerkCheck;
    int PlayerGeneration;
    int PlayerGenerationCheck;
    int CritterAction;
    int Animation2dProcess;
    int Animation3dProcess;
    int ItemsCollection;
    int CritterAnimation;
    int CritterAnimationSubstitute;
    int CritterAnimationFallout;
    int FilenameLogfile;
    int FilenameScreenshot;
    int CritterCheckMoveItem;
};

extern ClientScriptFunctions  ClientFunctions;
extern ReservedScriptFunction ClientReservedFunctions[];

#endif // FOCLASSIC_CLIENT || FOCLASSIC_SERVER
#if defined (FOCLASSIC_MAPPER) || defined (FOCLASSIC_SERVER)

struct MapperScriptFunctions
{
    int Start;
    int Loop;
    int ConsoleMessage;
    int RenderIface;
    int RenderMap;
    int MouseDown;
    int MouseUp;
    int MouseMove;
    int KeyDown;
    int KeyUp;
    int InputLost;
    int CritterAnimation;
    int CritterAnimationSubstitute;
    int CritterAnimationFallout;
};

extern MapperScriptFunctions  MapperFunctions;
extern ReservedScriptFunction MapperReservedFunctions[];

#endif // FOCLASSIC_MAPPER || FOCLASSIC_SERVER
#if defined (FOCLASSIC_SERVER)

struct ServerScriptFunctions
{
    int Init;
    int Start;
    int GetStartTime;
    int Finish;
    int Loop;
    int GlobalProcess;
    int GlobalInvite;
    int CritterAttack;
    int CritterAttacked;
    int CritterStealing;
    int CritterUseItem;
    int CritterUseSkill;
    int CritterReloadWeapon;
    int CritterInit;
    int CritterFinish;
    int CritterIdle;
    int CritterDead;
    int CritterRespawn;
    int CritterCheckMoveItem;
    int CritterMoveItem;
    int MapCritterIn;
    int MapCritterOut;
    int NpcPlaneBegin;
    int NpcPlaneEnd;
    int NpcPlaneRun;
    int KarmaVoting;
    int CheckLook;
    int ItemCost;
    int ItemsBarter;
    int ItemsCrafted;
    int PlayerLevelUp;
    int TurnBasedBegin;
    int TurnBasedEnd;
    int TurnBasedProcess;
    int TurnBasedSequence;
    int WorldSave;
    int PlayerRegistration;
    int PlayerLogin;
    int PlayerGetAccess;
    int PlayerAllowCommand;
    int CheckTrapLook;
};

extern ServerScriptFunctions  ServerFunctions;
extern ReservedScriptFunction ServerReservedFunctions[];

#endif // FOCLASSIC_SERVER

#endif // __SCRIPT_FUNCTIONS__ //
