#include "Core.h"
#include "App.h"

#include <unzip.h>

#include "FileSystem.h"
#include "Text.h"
#include "Zip.h"

using namespace std;

bool DataFileZip::Init( const char* fname )
{
    fileName = fname;
    zipHandle = NULL;

    char path[MAX_FOPATH];
    Str::Copy( path, fname );
    if( !ResolvePath( path ) )
    {
        App.WriteLogF( _FUNC_, " - Can't retrieve file full path.\n" );
        return false;
    }

    void* file = FileOpen( fname, false );
    if( !file )
    {
        App.WriteLogF( _FUNC_, " - Cannot open file.\n" );
        return false;
    }

    FileGetTime( file, timeCreate, timeAccess, timeWrite );

    FileClose( file );

    #ifdef FO_WINDOWS
    wchar_t path_wc[MAX_FOPATH];
    if( MultiByteToWideChar( CP_UTF8, 0, path, -1, path_wc, MAX_FOPATH ) == 0 ||
        WideCharToMultiByte( GetACP(), 0, path_wc, -1, path, MAX_FOPATH, NULL, NULL ) == 0 )
    {
        App.WriteLogF( _FUNC_, " - Code page conversion fail.\n" );
        return false;
    }
    #endif

    zipHandle = unzOpen( path );
    if( !zipHandle )
    {
        App.WriteLogF( _FUNC_, " - Cannot open ZIP file.\n" );
        return false;
    }

    if( !ReadTree() )
    {
        App.WriteLogF( _FUNC_, " - Read file tree fail.\n" );
        return false;
    }

    return true;
}

DataFileZip::~DataFileZip()
{
    if( zipHandle )
    {
        unzClose( zipHandle );
        zipHandle = NULL;
    }
}

bool DataFileZip::ReadTree()
{
    unz_global_info gi;
    if( unzGetGlobalInfo( zipHandle, &gi ) != UNZ_OK || gi.number_entry == 0 )
        return false;

    ZipFileInfo   zip_info;
    unz_file_pos  pos;
    unz_file_info info;
    for( uLong i = 0; i < gi.number_entry; i++ )
    {
        if( unzGetFilePos( zipHandle, &pos ) != UNZ_OK )
            return false;
        char buf[MAX_FOPATH];
        if( unzGetCurrentFileInfo( zipHandle, &info, buf, sizeof(buf), NULL, 0, NULL, 0 ) != UNZ_OK )
            return false;

        if( !(info.external_fa & 0x10) )     // Not folder
        {
            string name = string( buf );
            transform( name.begin(), name.end(), name.begin(), ::tolower );
            replace( name.begin(), name.end(), '/', '\\' );

            zip_info.Pos = pos;
            zip_info.UncompressedSize = info.uncompressed_size;
            filesTree.insert( make_pair( name, zip_info ) );
        }

        if( i + 1 < gi.number_entry && unzGoToNextFile( zipHandle ) != UNZ_OK )
            return false;
    }

    return true;
}

uint8* DataFileZip::OpenFile( const char* fname, uint& len )
{
    if( !zipHandle )
        return NULL;

    auto it = filesTree.find( fname );
    if( it == filesTree.end() )
        return NULL;

    ZipFileInfo& info = (*it).second;

    if( unzGoToFilePos( zipHandle, &info.Pos ) != UNZ_OK )
        return NULL;

    uint8* buf = new uint8[info.UncompressedSize + 1];
    if( !buf )
        return NULL;

    if( unzOpenCurrentFile( zipHandle ) != UNZ_OK )
    {
        delete[] buf;
        return NULL;
    }

    int read = unzReadCurrentFile( zipHandle, buf, info.UncompressedSize );
    if( unzCloseCurrentFile( zipHandle ) != UNZ_OK || read != (int)info.UncompressedSize )
    {
        delete[] buf;
        return NULL;
    }

    len = info.UncompressedSize;
    buf[len] = 0;
    return buf;
}

void DataFileZip::GetFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result )
{
    size_t path_len = Str::Length( path );
    for( auto it = filesTree.begin(), end = filesTree.end(); it != end; ++it )
    {
        const string& fname = (*it).first;
        if( !fname.compare( 0, path_len, path ) && (include_subdirs || (int)fname.find_last_of( '\\' ) < (int)path_len) )
        {
            if( ext && *ext )
            {
                size_t pos = fname.find_last_of( '.' );
                if( pos != string::npos && Str::CompareCase( &fname.c_str()[pos + 1], ext ) )
                    result.push_back( fname );
            }
            else
            {
                result.push_back( fname );
            }
        }
    }
}

void DataFileZip::GetTime( uint64* create, uint64* access, uint64* write )
{
    if( create )
        *create = timeCreate;
    if( access )
        *access = timeAccess;
    if( write )
        *write = timeWrite;
}
