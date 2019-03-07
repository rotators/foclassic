/************************************************************************/
/* Thread                                                               */
/************************************************************************/

#include "Core.h"

#include "Mutex.h"
#include "Text.h"
#include "Thread.h"

using namespace std;

THREAD char Thread::threadName[64] = { 0 };
UIntStrMap  Thread::threadNames;
Mutex       Thread::threadNamesLocker;

void* ThreadBeginExecution( void* args )
{
    void** args_ = (void**)args;
    void   ( * func )( void* ) = (void (*)( void* ) )args_[0];
    void*  func_arg = args_[1];
    char*  name = (char*)args_[2];
    Thread::SetCurrentName( name );
    delete[] name;
    free( args );
    func( func_arg );
    return NULL;
}

Thread::Thread()
{
    isStarted = false;
    pthread_attr_init( &threadAttr );
}

Thread::~Thread()
{
    pthread_attr_destroy( &threadAttr );
}

bool Thread::Start( void (*func)( void* ), const char* name, void* arg /* = NULL */ )
{
    void** args = (void**)malloc( sizeof(void*) * 3 );
    char*  name_ = Str::Duplicate( name );
    args[0] = (void*)func, args[1] = arg, args[2] = name_;
    isStarted = (pthread_create( &threadId, &threadAttr, ThreadBeginExecution, args ) == 0);
    return isStarted;
}

void Thread::Wait()
{
    if( isStarted )
        pthread_join( threadId, NULL );
    isStarted = false;
}

void Thread::Finish()
{
    if( isStarted )
        pthread_cancel( threadId );
    isStarted = false;
}

#ifdef FO_WINDOWS
HANDLE Thread::GetWindowsHandle()
{
    return pthread_getw32threadhandle_np( threadId );
}
#endif

#ifndef FO_WINDOWS
pid_t Thread::GetPid()
{
    return (pid_t)threadId;
}
#endif

uint Thread::GetCurrentId()
{
    #ifdef FO_WINDOWS
    return (uint)GetCurrentThreadId();
    #else
    return (uint)pthread_self();
    #endif
}

void Thread::SetCurrentName( const char* name )
{
    if( threadName[0] )
        return;

    Str::Copy( threadName, name );
    SCOPE_LOCK( threadNamesLocker );
    threadNames.insert( PAIR( GetCurrentId(), string( threadName ) ) );
}

const char* Thread::GetCurrentName()
{
    return threadName;
}

const char* Thread::FindName( uint thread_id )
{
    SCOPE_LOCK( threadNamesLocker );
    auto it = threadNames.find( thread_id );
    return it != threadNames.end() ? (*it).second.c_str() : NULL;
}

void Thread::Sleep( uint ms )
{
    #ifdef FO_WINDOWS
    ::Sleep( ms );
    #else
    struct timespec req;
    req.tv_sec = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000;
    while( nanosleep( &req, &req ) == -1 && errno == EINTR )
        continue;
    #endif
}
