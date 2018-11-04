#ifndef __CONFIG_FILE__
#define __CONFIG_FILE__

#include "Types.h"

class Ini;

// Main sections
#define EMPTY_SECTION           ""
#define CLIENT_SECTION          "Game Options"
#define MAPPER_SECTION          "Mapper"
#define SERVER_SECTION          "Server"

// Detail sections
#define CLIENT_DX_SECTION       "ClientDX"
#define CLIENT_GL_SECTION       "ClientGL"
#define MAPPER_DX_SECTION       "MapperDX"
#define MAPPER_GL_SECTION       "MapperGL"

// Helpers
// APP_SECTION         Main section for current target
// APP_SECTION_DETAIL  Detail section for current target (Client/Mapper only)
// APP_SECTION_UNUSED  Detail section not used for current target (Client/Mapper only)

#if defined (FOCLASSIC_CLIENT)
# define APP_SECTION            (CLIENT_SECTION)
# ifdef FO_D3D
#  define APP_SECTION_DETAIL    (CLIENT_DX_SECTION)
#  define APP_SECTION_UNUSED    (CLIENT_GL_SECTION)
# else
#  define APP_SECTION_DETAIL    (CLIENT_GL_SECTION)
#  define APP_SECTION_UNUSED    (CLIENT_DX_SECTION)
# endif
#elif defined (FOCLASSIC_MAPPER)
# define APP_SECTION            (MAPPER_SECTION)
# ifdef FO_D3D
#  define APP_SECTION_DETAIL    (MAPPER_DX_SECTION)
#  define APP_SECTION_UNUSED    (MAPPER_GL_SECTION)
# else
#  define APP_SECTION_DETAIL    (MAPPER_GL_SECTION)
#  define APP_SECTION_UNUSED    (MAPPER_DX_SECTION)
# endif
#elif defined (FOCLASSIC_SERVER)
# define APP_SECTION            (SERVER_SECTION)
#endif

#if defined (FOCLASSIC_CLIENT)
# define CFG_DEF_INT_FILE       "default800x600.ini"
#elif defined (FOCLASSIC_MAPPER)
# define CFG_DEF_INT_FILE       "mapper_default.ini"
#endif

extern Ini* ConfigFile;

extern bool LoadConfigFile( const char* fname );
extern void UnloadConfigFile();

const char* GetConfigFileName();

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
extern void GetClientOptions();
#endif

#if defined (FOCLASSIC_MAPPER)
extern void GetMapperOptions();
#endif

#if defined (FOCLASSIC_SERVER)
extern bool FOQuit;
extern int  ServerGameSleep;
extern uint VarsGarbageTime;
extern bool WorldSaveManager;
extern bool LogicMT;
#endif

#if defined (FOCLASSIC_SERVER) || defined (FOCLASSIC_MAPPER)
extern void GetServerOptions();
#endif

#endif // __CONFIG_FILE__ //
