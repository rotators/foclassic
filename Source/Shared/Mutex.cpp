#include "Core.h"

#include "Mutex.h"
#include "Thread.h"

#ifndef FO_WINDOWS
// Mutex static stuff
bool                Mutex::attrInitialized = false;
pthread_mutexattr_t Mutex::mutexAttr;
#endif

Mutex::Mutex()
{
    #ifdef FO_WINDOWS
    InitializeCriticalSectionAndSpinCount( &mutexCS, DEFAULT_SPIN_COUNT );
    #else
    InitAttributes();
    pthread_mutex_init( &mutexCS, &mutexAttr );
    #endif
}

Mutex::~Mutex()
{
    #ifdef FO_WINDOWS
    DeleteCriticalSection( &mutexCS );
    #else
    pthread_mutex_destroy( &mutexCS );
    #endif
}

void Mutex::SetSpinCount( int count )
{
    #ifdef FO_WINDOWS
    SetCriticalSectionSpinCount( &mutexCS, count );
    #else
    // TODO Linux
    #endif
}

void Mutex::Lock()
{
    #ifdef FO_WINDOWS
    EnterCriticalSection( &mutexCS );
    #else
    pthread_mutex_lock( &mutexCS );
    #endif
}

bool Mutex::TryLock()
{
    #ifdef FO_WINDOWS
    return TryEnterCriticalSection( &mutexCS ) != FALSE;
    #else
    return pthread_mutex_trylock( &mutexCS ) == 0;
    #endif
}

void Mutex::Unlock()
{
    #ifdef FO_WINDOWS
    LeaveCriticalSection( &mutexCS );
    #else
    pthread_mutex_unlock( &mutexCS );
    #endif
}

#ifndef FO_WINDOWS

void Mutex::InitAttributes() // static
{
    if( !attrInitialized )
    {
        pthread_mutexattr_init( &mutexAttr );
        pthread_mutexattr_settype( &mutexAttr, PTHREAD_MUTEX_RECURSIVE );
        attrInitialized = true;
    }
}

#endif // !FO_WINDOWS

/// MutexEvent

MutexEvent::MutexEvent()
{
    #ifdef FO_WINDOWS
    mutexEvent = CreateEvent( NULL, TRUE, TRUE, NULL );
    #else
    pthread_mutex_init( &ptMutex, NULL );
    pthread_cond_init( &ptCond, NULL );
    flag = true;
    #endif
}

MutexEvent::~MutexEvent()
{
    #ifdef FO_WINDOWS
    CloseHandle( mutexEvent );
    #else
    pthread_mutex_destroy( &ptMutex );
    pthread_cond_destroy( &ptCond );
    #endif
}

void MutexEvent::Allow()
{
    #ifdef FO_WINDOWS
    SetEvent( mutexEvent );
    #else
    pthread_mutex_lock( &ptMutex );
    flag = true;
    pthread_cond_broadcast( &ptCond );
    pthread_mutex_unlock( &ptMutex );
    #endif
}

void MutexEvent::Disallow()
{
    #ifdef FO_WINDOWS
    ResetEvent( mutexEvent );
    #else
    pthread_mutex_lock( &ptMutex );
    flag = false;
    pthread_mutex_unlock( &ptMutex );
    #endif
}

void MutexEvent::Wait()
{
    #ifdef FO_WINDOWS
    WaitForSingleObject( mutexEvent, INFINITE );
    #else
    pthread_mutex_lock( &ptMutex );
    if( !flag )
        pthread_cond_wait( &ptCond, &ptMutex );
    pthread_mutex_unlock( &ptMutex );
    #endif
}

#ifdef FO_WINDOWS
void* MutexEvent::GetHandle()
{
    return mutexEvent;
}
#endif

void MutexCode::LockCode()
{
    mcLocker.Lock();
    mcEvent.Disallow();
    while( InterlockedCompareExchange( &mcCounter, 0, 0 ) )
    {
        Thread::Sleep( 0 );
    }
    mcLocker.Unlock();
}

void MutexCode::UnlockCode()
{
    mcEvent.Allow();
}

void MutexCode::EnterCode()
{
    mcLocker.Lock();
    mcEvent.Wait();
    InterlockedIncrement( &mcCounter );
    mcLocker.Unlock();
}

void MutexCode::LeaveCode()
{
    InterlockedDecrement( &mcCounter );
}
/// MutexSynchronizer

void MutexSynchronizer::Synchronize( long count )
{
    InterlockedIncrement( &msCounter );
    msLocker.Lock();
    msEvent.Wait();
    InterlockedDecrement( &msCounter );
    msEvent.Disallow();
    while( InterlockedCompareExchange( &msCounter, 0, 0 ) != count )
    {
        Thread::Sleep( 0 );
    }
    msLocker.Unlock();
}

void MutexSynchronizer::Resynchronize()
{
    msEvent.Allow();
}

void MutexSynchronizer::SynchronizePoint()
{
    InterlockedIncrement( &msCounter );
    msLocker.Lock();
    msEvent.Wait();
    InterlockedDecrement( &msCounter );
    msLocker.Unlock();
}

/// MutexSpinlock

void MutexSpinlock::Lock()
{
    while( InterlockedCompareExchange( &spinCounter, 1, 0 ) )
    {
        /*Wait*/
    }
}
bool MutexSpinlock::TryLock()
{
    return InterlockedCompareExchange( &spinCounter, 1, 0 ) == 0;
}

void MutexSpinlock::Unlock()
{
    InterlockedExchange( &spinCounter, 0 );
}
