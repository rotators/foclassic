#ifndef __DEPRECATED__
#define __DEPRECATED__

#include "Types.h"

#if defined (FOCLASSIC_MAPPER) || defined (FOCLASSIC_SERVER)
// Deprecated stuff
// tiles.lst, items.lst, scenery.lst, walls.lst, misc.lst, intrface.lst, inven.lst
// pid == -1 - interface
// pid == -2 - tiles
// pid == -3 - inventory
extern string Deprecated_GetPicName( int pid, int type, ushort pic_num );
extern uint   Deprecated_GetPicHash( int pid, int type, ushort pic_num );
extern void   Deprecated_CondExtToAnim2( uchar cond, uchar cond_ext, uint& anim2ko, uint& anim2dead );
#endif

#endif // __DEPRECATED__ //
