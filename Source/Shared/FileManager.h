#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include "DataFile.h"
#include "Defines.h"
#include "Types.h"

#define PATH_LIST_COUNT    (50)

class FileManager
{
public:
    static bool SetPathType( int path_type, const char* path );

    static void SetDataPath( const char* path );
    static void SetCacheName( const char* name );
    static void InitDataFiles( const char* path );
    static bool LoadDataFile( const char* path );
    static void EndOfWork();

    bool   LoadFile( const char* fname, int path_type );
    bool   LoadStream( const uint8* stream, uint length );
    void   UnloadFile();
    uint8* ReleaseBuffer();

    void SetCurPos( uint pos );
    void GoForward( uint offs );
    void GoBack( uint offs );
    bool FindFragment( const uint8* fragment, uint fragment_len, uint begin_offs );

    bool   GetLine( char* str, uint len );
    bool   CopyMem( void* ptr, uint size );
    void   GetStr( char* str );
    uint8  GetUChar();
    uint16 GetBEUShort();
    uint16 GetLEUShort();
    uint   GetBEUInt();
    uint   GetLEUInt();
    uint   GetLE3UChar();
    float  GetBEFloat();
    float  GetLEFloat();

    void   SwitchToRead();
    void   SwitchToWrite();
    void   ClearOutBuf();
    bool   ResizeOutBuf();
    void   SetPosOutBuf( uint pos );
    bool   SaveOutBufToFile( const char* fname, int path_type );
    uint8* GetOutBuf()    { return dataOutBuf; }
    uint   GetOutBufLen() { return endOutBuf; }

    void SetData( void* data, uint len );
    void SetStr( const char* fmt, ... );
    void SetUChar( uint8 data );
    void SetBEUShort( uint16 data );
    void SetLEUShort( uint16 data );
    void SetBEUInt( uint data );
    void SetLEUInt( uint data );

    static const char* GetFullPath( const char* fname, int path_type );
    static void        GetFullPath( const char* fname, int path_type, char* get_path );
    static const char* GetPath( int path_type );
    static const char* GetDataPath( int path_type );
    static void        FormatPath( char* path, bool first_skipped = false );
    static void        ExtractPath( const char* fname, char* path );
    static void        ExtractFileName( const char* fname, char* name );
    static void        MakeFilePath( const char* name, const char* path, char* result );
    static void        CreateDirectoryTree( const char* path );
    static const char* GetExtension( const char* fname ); // EXT without dot
    static char*       EraseExtension( char* fname );     // Erase EXT with dot

    bool   IsLoaded()  { return fileBuf != NULL; }
    uint8* GetBuf()    { return fileBuf; }
    uint8* GetCurBuf() { return fileBuf + curPos; }
    uint   GetCurPos() { return curPos; }
    uint   GetFsize()  { return fileSize; }
    bool   IsEOF()     { return curPos >= fileSize; }
    void   GetTime( uint64* create, uint64* access, uint64* write );
    int    ParseLinesInt( const char* fname, int path_type, IntVec& lines );

    static DataFileVec& GetDataFiles() { return dataFiles; }
    static void         GetFolderFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result );
    static void         GetDatsFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result );

    FileManager() : dataOutBuf( NULL ), posOutBuf( 0 ), endOutBuf( 0 ), lenOutBuf( 0 ), fileSize( 0 ), curPos( 0 ), fileBuf( NULL ) {};
    ~FileManager()
    {
        UnloadFile();
        ClearOutBuf();
    }

private:
    static char        dataPath[MAX_FOPATH];
    static DataFileVec dataFiles;

    uint               fileSize;
    uint8*             fileBuf;
    uint               curPos;

    uint8*             dataOutBuf;
    uint               posOutBuf;
    uint               endOutBuf;
    uint               lenOutBuf;

    uint64             timeCreate, timeAccess, timeWrite;

    static void RecursiveDirLook( const char* init_dir, bool include_subdirs, const char* ext, StrVec& result );
};

#endif // __FILE_MANAGER__
