#ifndef __CONFIG_FILE__
#define __CONFIG_FILE__

#include "Defines.h"
#include "Types.h"

class Ini;

// Helpers
// SECTION_MAIN    Section holding settings for application
// SECTION_DETAIL  Section holding settings for current target (Client/Mapper only)
// SECTION_UNUSED  Section not used for current target (Client/Mapper only)

#if defined (FOCLASSIC_CLIENT)
# define SECTION_MAIN        (SECTION_CLIENT)
# ifdef FO_D3D
#  define SECTION_DETAIL     (SECTION_CLIENT_DX)
#  define SECTION_UNUSED     (SECTION_CLIENT_GL)
# else
#  define SECTION_DETAIL     (SECTION_CLIENT_GL)
#  define SECTION_UNUSED     (SECTION_CLIENT_DX)
# endif
#elif defined (FOCLASSIC_MAPPER)
# define SECTION_MAIN        (SECTION_MAPPER)
# ifdef FO_D3D
#  define SECTION_DETAIL     (SECTION_MAPPER_DX)
#  define SECTION_UNUSED     (SECTION_MAPPER_GL)
# else
#  define SECTION_DETAIL     (SECTION_MAPPER_GL)
#  define SECTION_UNUSED     (SECTION_MAPPER_DX)
# endif
#elif defined (FOCLASSIC_SERVER)
# define SECTION_MAIN        (SECTION_SERVER)
#endif

#if defined (FOCLASSIC_CLIENT)
# define CFG_DEF_INT_FILE    "default800x600.ini"
#elif defined (FOCLASSIC_MAPPER)
# define CFG_DEF_INT_FILE    "mapper_default.ini"
#endif

extern Ini* ConfigFile;

extern const char* GetConfigFileName();

extern bool LoadConfigFile( const char* fname, const char* main = NULL, const char* detail = NULL, const char* unused = NULL );
extern void UnloadConfigFile();

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
