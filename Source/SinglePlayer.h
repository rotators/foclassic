#ifndef __SINGLE_PLAYER__
#define __SINGLE_PLAYER__

#include "Types.h"

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

#endif // __SINGLE_PLAYER__ //
