#ifndef __CORE__
#define __CORE__

#include "CMake.h"
#include "PlatformSpecific.h"

#include "Defines.h"
#include "Types.h"

#ifdef FO_WINDOWS
# include <windows.h>
# define DLL_Load( name )              (void*)LoadLibrary( name )
# define DLL_Free( h )                 FreeLibrary( (HMODULE)h )
# define DLL_GetAddress( h, pname )    (size_t*)GetProcAddress( (HMODULE)h, pname )
# define DLL_Error()                   Str::ItoA( GetLastError() )
#else
# include <dlfcn.h>
# define DLL_Load( name )              (void*)dlopen( name, RTLD_NOW | RTLD_LOCAL )
# define DLL_Free( h )                 dlclose( h )
# define DLL_GetAddress( h, pname )    (size_t*)dlsym( h, pname )
# define DLL_Error()                   dlerror()# endif
#endif

/***** Externs *****/
extern bool LogicMT; // TODO move it where it belongs

// Core.cpp

extern int  Procent( int full, int peace );
extern uint NumericalNumber( uint num );
extern uint DistSqrt( int x1, int y1, int x2, int y2 );
extern uint DistGame( int x1, int y1, int x2, int y2 );
extern int  GetNearDir( int x1, int y1, int x2, int y2 );
extern int  GetFarDir( int x1, int y1, int x2, int y2 );
extern int  GetFarDir( int x1, int y1, int x2, int y2, float offset );
extern bool CheckDist( ushort x1, ushort y1, ushort x2, ushort y2, uint dist );
extern int  ReverseDir( int dir );
extern void GetStepsXY( float& sx, float& sy, int x1, int y1, int x2, int y2 );
extern void ChangeStepsXY( float& sx, float& sy, float deq );
extern bool MoveHexByDir( ushort& hx, ushort& hy, uchar dir, ushort maxhx, ushort maxhy );
extern void MoveHexByDirUnsafe( int& hx, int& hy, uchar dir );
extern bool IntersectCircleLine( int cx, int cy, int radius, int x1, int y1, int x2, int y2 );
extern void FormatPreprocessorOutput( string& str );
extern void ShowMessage( const char* message );

// Auto pointers

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

#endif  // __CORE__ //
