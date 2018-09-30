#ifndef __ACCESS__
#define __ACCESS__

#include "BufferManager.h"

void PackCommand( const char* str, BufferManager& buf, void (* logcb)( const char* ), const char* name );

#endif // __ACCESS__ //
