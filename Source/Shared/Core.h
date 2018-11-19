#ifndef __CORE__
#define __CORE__

#include "CMake.h"
#include "PlatformSpecific.h"

#include "Defines.h"
#include "Types.h"

/* TODO: should be part of cmake setup
// Libevent workarounds
// Was bugged for Windows, need retest
#ifndef FO_WINDOWS
# define USE_LIBEVENT
// Linux don't want call write timeouts, need to know why and fix
# define LIBEVENT_TIMEOUTS_WORKAROUND
#endif
*/

/////
//
// Core.cpp
//
/////

// math stuff

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

// hex offsets

extern void GetHexOffsets( bool odd, short*& sx, short*& sy );
extern void GetHexInterval( int from_hx, int from_hy, int to_hx, int to_hy, int& x, int& y );

// preprocessor output formatting

extern void FormatPreprocessorOutput( string& str );

// misc

extern void RestoreMainDirectory();
extern void ShowMessage( const char* message );

#endif  // __CORE__ //
