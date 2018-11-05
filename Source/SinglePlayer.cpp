#include "Core.h"

#include "SinglePlayer.h"

bool             Singleplayer = false;
InterprocessData SingleplayerData;

/************************************************************************/
/* Single player                                                        */
/************************************************************************/

#ifdef FO_WINDOWS

void* SingleplayerClientProcess = NULL;

# define INTERPROCESS_DATA_SIZE    (OFFSETOF( InterprocessData, mapFileMutex ) )

HANDLE InterprocessData::Init()
{
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
    mapFile = CreateFileMapping( INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, INTERPROCESS_DATA_SIZE + sizeof(mapFileMutex), NULL );
    if( !mapFile )
        return NULL;
    mapFilePtr = NULL;

    mapFileMutex = CreateMutex( &sa, FALSE, NULL );
    if( !mapFileMutex )
        return NULL;

    if( !Lock() )
        return NULL;
    memzero( this, INTERPROCESS_DATA_SIZE );
    ( (InterprocessData*)mapFilePtr )->mapFileMutex = mapFileMutex;
    Unlock();
    return mapFile;
}

void InterprocessData::Finish()
{
    if( mapFile )
        CloseHandle( mapFile );
    if( mapFileMutex )
        CloseHandle( mapFileMutex );
    mapFile = NULL;
    mapFilePtr = NULL;
}

bool InterprocessData::Attach( HANDLE map_file )
{
    if( !map_file )
        return false;
    mapFile = map_file;
    mapFilePtr = NULL;

    // Read mutex handle
    void* ptr = MapViewOfFile( mapFile, FILE_MAP_WRITE, 0, 0, 0 );
    if( !ptr )
        return false;
    mapFileMutex = ( (InterprocessData*)ptr )->mapFileMutex;
    UnmapViewOfFile( ptr );
    if( !mapFileMutex )
        return false;

    return Refresh();
}

bool InterprocessData::Lock()
{
    if( !mapFile )
        return false;

    uint result = WaitForSingleObject( mapFileMutex, INFINITE );
    if( result != WAIT_OBJECT_0 )
        return false;

    mapFilePtr = MapViewOfFile( mapFile, FILE_MAP_WRITE, 0, 0, 0 );
    if( !mapFilePtr )
        return false;
    memcpy( this, mapFilePtr, INTERPROCESS_DATA_SIZE );
    return true;
}

void InterprocessData::Unlock()
{
    if( !mapFile || !mapFilePtr )
        return;

    memcpy( mapFilePtr, this, INTERPROCESS_DATA_SIZE );
    UnmapViewOfFile( mapFilePtr );
    mapFilePtr = NULL;

    ReleaseMutex( mapFileMutex );
}

bool InterprocessData::Refresh()
{
    if( !Lock() )
        return false;
    Unlock();
    return true;
}

#endif // FO_WINDOWS
