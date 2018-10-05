#ifndef __COMMON__
#define __COMMON__

// For debugging
// #define DEV_VERSION

// Some platform specific definitions
#include "PlatformSpecific.h"

// Standard API
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <algorithm>
#include <math.h>
#ifdef FO_WINDOWS
# include <Windows.h>
#else
# include <errno.h>
# include <string.h> // strerror
# include <unistd.h>
# define ERRORSTR             strerror( errno )
# define ExitProcess( code )           exit( code )
#endif

// Network
const char* GetLastSocketError();
#ifdef FO_WINDOWS
# include <winsock2.h>
# define socklen_t            int
# if defined (FO_MSVC)
#  pragma comment( lib, "Ws2_32.lib" )
# endif
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# define SOCKET               int
# define INVALID_SOCKET       (-1)
# define SOCKET_ERROR         (-1)
# define closesocket          close
# define SD_RECEIVE           SHUT_RD
# define SD_SEND              SHUT_WR
# define SD_BOTH              SHUT_RDWR
#endif

// DLL
#ifdef FO_WINDOWS
# define DLL_Load( name )              (void*)LoadLibrary( name )
# define DLL_Free( h )                 FreeLibrary( (HMODULE)h )
# define DLL_GetAddress( h, pname )    (size_t*)GetProcAddress( (HMODULE)h, pname )
# define DLL_Error()                   Str::ItoA( GetLastError() )
#else
# include <dlfcn.h>
# define DLL_Load( name )              (void*)dlopen( name, RTLD_NOW | RTLD_LOCAL )
# define DLL_Free( h )                 dlclose( h )
# define DLL_GetAddress( h, pname )    (size_t*)dlsym( h, pname )
# define DLL_Error()                   dlerror()
#endif

// FOnline stuff
#include "Types.h"
#include "Defines.h"
#include "Log.h"
#include "Timer.h"
#include "Debugger.h"
#include "Exception.h"
#include "FlexRect.h"
#include "Random.h"
#include "Mutex.h"
#include "Text.h"
#include "FileSystem.h"
#include "AngelScript/scriptstring.h"

typedef vector<Rect>  IntRectVec;
typedef vector<RectF> FltRectVec;

extern char   CommandLine[MAX_FOTEXT];
extern char** CommandLineArgValues;
extern uint   CommandLineArgCount;
void SetCommandLine( uint argc, char** argv );

extern Randomizer DefaultRandomizer;
int Random( int minimum, int maximum );

int  Procent( int full, int peace );
uint NumericalNumber( uint num );
uint DistSqrt( int x1, int y1, int x2, int y2 );
uint DistGame( int x1, int y1, int x2, int y2 );
int  GetNearDir( int x1, int y1, int x2, int y2 );
int  GetFarDir( int x1, int y1, int x2, int y2 );
int  GetFarDir( int x1, int y1, int x2, int y2, float offset );
bool CheckDist( ushort x1, ushort y1, ushort x2, ushort y2, uint dist );
int  ReverseDir( int dir );
void GetStepsXY( float& sx, float& sy, int x1, int y1, int x2, int y2 );
void ChangeStepsXY( float& sx, float& sy, float deq );
bool MoveHexByDir( ushort& hx, ushort& hy, uchar dir, ushort maxhx, ushort maxhy );
void MoveHexByDirUnsafe( int& hx, int& hy, uchar dir );
bool IntersectCircleLine( int cx, int cy, int radius, int x1, int y1, int x2, int y2 );
void RestoreMainDirectory();
void ShowMessage( const char* message );
uint GetDoubleClickTicks();

// Containers comparator template
template<class T>
inline bool CompareContainers( const T& a, const T& b ) { return a.size() == b.size() && (a.empty() || !memcmp( &a[0], &b[0], a.size() * sizeof(a[0]) ) ); }

// Hex offsets
void GetHexOffsets( bool odd, short*& sx, short*& sy );
void GetHexInterval( int from_hx, int from_hy, int to_hx, int to_hy, int& x, int& y );

// Config file
#define CLIENT_CONFIG_APP     "Game Options"
const char* GetConfigFileName();

// Window name
const char* GetWindowName();

// Shared structure
struct ScoreType
{
    uint ClientId;
    char ClientName[SCORE_NAME_LEN];
    int  Value;
};

/************************************************************************/
/* Client & Mapper                                                      */
/************************************************************************/
#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)

# define PI_VALUE             (3.141592654f)
# define COLOR_ARGB( a, r, g, b )      ( (uint)( ( ( (a) & 0xff ) << 24 ) | ( ( (r) & 0xff ) << 16 ) | ( ( (g) & 0xff ) << 8 ) | ( (b) & 0xff ) ) )
# define COLOR_XRGB( r, g, b )         COLOR_ARGB( 0xff, r, g, b )


# ifdef FO_D3D
#  ifndef D3D_DEBUG_INFO
#   pragma comment(lib,"d3dx9.lib")
#  else
#   pragma comment(lib,"d3dx9d.lib")
#  endif
# endif

# include "Assimp/aiTypes.h"

// Todo: Complete shadow maps
// # define SHADOW_MAP

# ifdef FOCLASSIC_CLIENT
#  include "ResourceClient.h"
#  define CFG_DEF_INT_FILE    "default800x600.ini"
# else // FOCLASSIC_MAPPER
#  include "ResourceMapper.h"
const uchar SELECT_ALPHA = 100;
#  define CFG_DEF_INT_FILE    "mapper_default.ini"
# endif

# define PATH_MAP_FLAGS       DIR_SLASH_SD "Data" DIR_SLASH_S "maps" DIR_SLASH_S ""
# define PATH_TEXT_FILES      DIR_SLASH_SD "Data" DIR_SLASH_S "text" DIR_SLASH_S ""

uint GetColorDay( int* day_time, uchar* colors, int game_time, int* light );
void GetClientOptions();

struct ClientScriptFunctions
{
    int Start;
    int Loop;
    int GetActiveScreens;
    int ScreenChange;
    int RenderIface;
    int RenderIfaceScreen;
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
} extern ClientFunctions;

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
} extern MapperFunctions;

#endif
/************************************************************************/
/* Server                                                               */
/************************************************************************/
#ifdef FOCLASSIC_SERVER

# include "Script.h"
# include "ThreadSync.h"
# include "Jobs.h"

extern bool FOQuit;
extern int  ServerGameSleep;
extern int  MemoryDebugLevel;
extern uint VarsGarbageTime;
extern bool WorldSaveManager;
extern bool LogicMT;

void GetServerOptions();

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
} extern ServerFunctions;

// Net events
# if defined (USE_LIBEVENT)
#  if defined (FO_MSVC)
#   pragma comment( lib, "libevent_core.lib" )
#  endif
# endif

#endif
/************************************************************************/
/* Tools                                                                */
/************************************************************************/
#ifdef FONLINE_NPCEDITOR
# include <strstream>
# include <fstream>

# define _CRT_SECURE_NO_DEPRECATE
# define MAX_TEXT_DIALOG    (1000)

# define ScriptString       string
#endif
#ifdef FONLINE_MRFIXIT
# define ScriptString       string
#endif


/************************************************************************/
/* File logger                                                          */
/************************************************************************/

class FileLogger
{
private:
    FILE* logFile;
    uint  startTick;

public:
    FileLogger( const char* fname );
    ~FileLogger();
    void Write( const char* fmt, ... );
};

/************************************************************************/
/* Single player                                                        */
/************************************************************************/

#ifdef FO_WINDOWS
class InterprocessData
{
public:
    ushort NetPort;
    bool   Pause;

private:
    HANDLE mapFileMutex;
    HANDLE mapFile;
    void*  mapFilePtr;

public:
    HANDLE Init();
    void   Finish();
    bool   Attach( HANDLE map_file );
    bool   Lock();
    void   Unlock();
    bool   Refresh();
};

extern HANDLE SingleplayerClientProcess;
#else
// Todo: linux
class InterprocessData
{
public:
    ushort NetPort;
    bool   Pause;
};
#endif

extern bool             Singleplayer;
extern InterprocessData SingleplayerData;



/************************************************************************/
/*                                                                      */
/************************************************************************/

// Deprecated stuff
// tiles.lst, items.lst, scenery.lst, walls.lst, misc.lst, intrface.lst, inven.lst
// pid == -1 - interface
// pid == -2 - tiles
// pid == -3 - inventory
string Deprecated_GetPicName( int pid, int type, ushort pic_num );
uint   Deprecated_GetPicHash( int pid, int type, ushort pic_num );
void   Deprecated_CondExtToAnim2( uchar cond, uchar cond_ext, uint& anim2ko, uint& anim2dead );

// Preprocessor output formatting
void FormatPreprocessorOutput( string& str );

#endif // __COMMON__
