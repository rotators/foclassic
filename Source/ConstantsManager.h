#ifndef __CONSTANTS_MANAGER__
#define __CONSTANTS_MANAGER__

#include "Types.h"

namespace ConstantsManager
{
    void   Initialize( int path_type, const char* path = NULL );
    bool   AddCollection( int collection, const char* fname, int path_type );
    void   AddConstant( int collection, const char* str, int value );
    StrVec GetCollection( int collection );
    bool   IsCollectionInit( int collection );

    int         GetValue( int collection, const char* str );
    const char* GetName( int collection, int value );
    int         GetParamId( const char* str );
    const char* GetParamName( uint index );
    int         GetItemPid( const char* str );
    const char* GetItemName( uint16 pid );
    int         GetDefineValue( const char* str );
    const char* GetPictureName( uint index );
};

#endif // __CONSTANTS_MANAGER__
