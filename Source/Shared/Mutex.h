#ifndef __MUTEX__
#define __MUTEX__

#include "Types.h"

#define DEFAULT_SPIN_COUNT                                (4000)
#define SCOPE_LOCK( mutex )                               volatile MutexLocker<decltype(mutex)> scope_lock__( mutex )

#ifdef FO_WINDOWS

# ifdef FO_MSVC
#  include <intrin.h>
#  define InterlockedCompareExchange                      _InterlockedCompareExchange
#  define InterlockedExchange                             _InterlockedExchange
#  define InterlockedIncrement                            _InterlockedIncrement
#  define InterlockedDecrement                            _InterlockedDecrement
# endif

#else // !FO_WINDOWS

# include <pthread.h>
# define InterlockedCompareExchange( val, exch, comp )    __sync_val_compare_and_swap( val, comp, exch )
# define InterlockedExchange( val, newval )               __sync_lock_test_and_set( val, newval )
# define InterlockedIncrement( val )                      __sync_add_and_fetch( val, 1 )
# define InterlockedDecrement( val )                      __sync_sub_and_fetch( val, 1 )

#endif // FO_WINDOWS

class Mutex
{
private:
    #ifdef FO_WINDOWS
    CRITICAL_SECTION mutexCS;
    #else
    pthread_mutex_t mutexCS;
    #endif

    #ifndef FO_MACOSX
    int dummyData[5];
    #endif

    #ifndef FO_WINDOWS
    static bool                attrInitialized;
    static pthread_mutexattr_t mutexAttr;
    static void InitAttributes();
    #endif

    Mutex( const Mutex& ) {}
    void operator=( const Mutex& ) {}

public:
    Mutex();
    ~Mutex();
    void SetSpinCount( int count );
    void Lock();
    bool TryLock();
    void Unlock();
};

class MutexEvent
{
private:
    #ifdef FO_WINDOWS
    HANDLE mutexEvent;
    #else
    pthread_mutex_t ptMutex;
    pthread_cond_t ptCond;
    bool           flag;
    #endif
    MutexEvent( const MutexEvent& ) {}
    MutexEvent& operator=( const MutexEvent& ) { return *this; }

public:
    MutexEvent();
    ~MutexEvent();
    void Allow();
    void Disallow();
    void Wait();
    #ifdef FO_WINDOWS
    void* GetHandle();
    #endif
};

class MutexCode
{
private:
    MutexEvent    mcEvent;
    Mutex         mcLocker;
    volatile long mcCounter;
    MutexCode( const MutexCode& ) {}
    MutexCode& operator=( const MutexCode& ) { return *this; }

public:
    MutexCode() : mcCounter( 0 ) {}
    void LockCode();
    void UnlockCode();
    void EnterCode();
    void LeaveCode();
};

class MutexSynchronizer
{
private:
    MutexEvent    msEvent;
    Mutex         msLocker;
    volatile long msCounter;
    MutexSynchronizer( const MutexSynchronizer& ) {}
    MutexSynchronizer& operator=( const MutexSynchronizer& ) { return *this; }

public:
    MutexSynchronizer() : msCounter( 0 ) {}
    void Synchronize( long count );
    void Resynchronize();
    void SynchronizePoint();
};

class MutexSpinlock
{
private:
    volatile long spinCounter;
    MutexSpinlock( const MutexSpinlock& ) {}
    MutexSpinlock& operator=( const MutexSpinlock& ) { return *this; }

public:
    MutexSpinlock() : spinCounter( 0 ) {}
    void Lock();
    bool TryLock();
    void Unlock();
};

template<class T>
class MutexLocker
{
private:
    T& pMutex;
    MutexLocker() {}
    MutexLocker( const MutexLocker& ) {}
    MutexLocker& operator=( const MutexLocker& ) { return *this; }

public:
    MutexLocker( T& mutex ) : pMutex( mutex ) { pMutex.Lock(); }
    ~MutexLocker() { pMutex.Unlock(); }
};

#endif // __MUTEX__
