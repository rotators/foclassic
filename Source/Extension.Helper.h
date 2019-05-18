#ifndef __EXTENSION_HELPER__
#define __EXTENSION_HELPER__

//
// This file should be included as last (after other engine headers)
//

#undef EXPORT
#define EXPORT

// CritterMutual = Critter, if Critter.h was included
#if defined (FOCLASSIC_SERVER) && defined (__CRITTER__)
typedef Critter     CritterMutual;
#endif

// CritterMutual = Critter, if CritterCl.h was included
#if !defined (FOCLASSIC_SERVER) && defined (__CRITTER_CL__)
typedef CritterCl   CritterMutual;
#endif

// ItemCl = Item, if ItemCl.h was included
#if !defined (FOCLASSIC_SERVER) && defined (__ITEM_CL__)
typedef Item        ItemCl;
#endif

// NpcPlane = AIDataPlane, if AI.h was included
#if defined (FOCLASSIC_SERVER) && defined (__AI__)
typedef AIDataPlane NpcPlane;
#endif

// some functions in FOClassic.h has been copied from "Shared" library, available for all targets...
#include "Core.h"

// ...however they use different names

#define GetDistantion(x1,y1,x2,y2) DistGame(x1,y1,x2,y2)

#endif // __EXTENSION_HELPER__ //
