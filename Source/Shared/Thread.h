#ifndef __THREAD__
#define __THREAD__

#include "Mutex.h"
#include "Types.h"

/************************************************************************/
/* Threads                                                              */
/************************************************************************/

#if !defined (FONLINE_NPCEDITOR) && !defined (FONLINE_MRFIXIT)

# include "pthread.h"

class Thread
{
private:
    static THREAD char threadName[64];
    static UIntStrMap  threadNames;
    static Mutex       threadNamesLocker;
    bool               isStarted;
    pthread_t          threadId;
    pthread_attr_t     threadAttr;

public:
    Thread();
    ~Thread();
    bool Start( void (*func)( void* ), const char* name, void* arg = NULL );
    void Wait();
    void Finish();

    # ifdef FO_WINDOWS
    HANDLE GetWindowsHandle();
    # else
    pid_t GetPid();
    # endif

    static uint        GetCurrentId();
    static void        SetCurrentName( const char* name );
    static const char* GetCurrentName();
    static const char* FindName( uint thread_id );
    static void        Sleep( uint ms );
};

#endif

#endif // __THREAD__ //
