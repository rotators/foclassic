#ifndef __UTILS__
#define __UTILS__

#include "Types.h"

extern uint GetDoubleClickTicks();

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_MAPPER)

extern uint GetColorDay( int* day_time, uint8* colors, int game_time, int* light );

#endif // FOCLASSIC_CLIENT || FOCLASSIC_MAPPER

#ifdef FOCLASSIC_MAPPER
const uint8 SELECT_ALPHA = 100;
#endif

template<class T>
inline bool CompareContainers( const T& a, const T& b )
{
    return a.size() == b.size() && (a.empty() || !memcmp( &a[0], &b[0], a.size() * sizeof(a[0]) ) );
}

#endif // __UTILS__ //
