#ifndef __CORE__
#define __CORE__

#include "CMake.h"
#include "PlatformSpecific.h"

#include "Defines.h"
#include "Types.h"

#ifdef FO_WINDOWS
# include <windows.h>
#endif

/***** Externs *****/
extern bool LogicMT;

/************************************************************************/
/* Auto pointers                                                        */
/************************************************************************/

template<class T>
class AutoPtr
{
public:
    AutoPtr( T* ptr ) : Ptr( ptr ) {}
    ~AutoPtr() { if( Ptr ) delete Ptr; }
    T& operator*() const  { return *Get(); }
    T* operator->() const { return Get(); }
    T* Get() const        { return Ptr; }
    T* Release()
    {
        T* tmp = Ptr;
        Ptr = NULL;
        return tmp;
    }
    void Reset( T* ptr )
    {
        if( ptr != Ptr && Ptr != 0 ) delete Ptr;
        Ptr = ptr;
    }
    bool IsValid() const { return Ptr != NULL; }

private:
    T* Ptr;
};

template<class T>
class AutoPtrArr
{
public:
    AutoPtrArr( T* ptr ) : Ptr( ptr ) {}
    ~AutoPtrArr() { if( Ptr ) delete[] Ptr; }
    T& operator*() const  { return *Get(); }
    T* operator->() const { return Get(); }
    T* Get() const        { return Ptr; }
    T* Release()
    {
        T* tmp = Ptr;
        Ptr = NULL;
        return tmp;
    }
    void Reset( T* ptr )
    {
        if( ptr != Ptr && Ptr != 0 ) delete[] Ptr;
        Ptr = ptr;
    }
    bool IsValid() const { return Ptr != NULL; }

private:
    T* Ptr;
};

#endif // __CORE__ //
