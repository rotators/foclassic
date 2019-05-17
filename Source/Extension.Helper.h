#ifndef __EXTENSION_HELPER__
#define __EXTENSION_HELPER__

//
// This file should be included after regular headers
//

#undef EXPORT
#define EXPORT

#if defined (FOCLASSIC_SERVER) && defined (__CRITTER__)
typedef Critter     CritterMutual;
#endif

#if !defined (FOCLASSIC_SERVER) && defined (__CRITTER_CL__)
typedef CritterCl   CritterMutual;
#endif

#if !defined (FOCLASSIC_SERVER) && defined (__ITEM_CL__)
typedef Item        ItemCl;
#endif

#if defined (FOCLASSIC_SERVER) && defined (__AI__)
typedef AIDataPlane NpcPlane;
#endif

#endif // __EXTENSION_HELPER__ //
