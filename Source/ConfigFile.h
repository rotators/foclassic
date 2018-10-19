#ifndef __CONFIG_FILE__
#define __CONFIG_FILE__

// Config file
#define CLIENT_CONFIG_APP    "Game Options"

#if defined (FOCLASSIC_CLIENT)
# define CFG_DEF_INT_FILE    "default800x600.ini"
#elif defined (FOCLASSIC_MAPPER)
# define CFG_DEF_INT_FILE    "mapper_default.ini"
#endif

const char* GetConfigFileName();

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)
extern void GetClientOptions();
#endif

#if defined (FOCLASSIC_SERVER)
extern bool FOQuit;
extern int  ServerGameSleep;
extern uint VarsGarbageTime;
extern bool WorldSaveManager;
extern bool LogicMT;

extern void GetServerOptions();
#endif

#endif // __CONFIG_FILE__ //
