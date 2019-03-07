#include "Core.h"

#include <unzip.h>

#include "../App.h"

#include "Dat.h"
#include "FileSystem.h"
#include "Text.h"

using namespace std;

bool DataFileDat::Init( const char* fname )
{
    datHandle = NULL;
    memTree = NULL;
    fileName = fname;
    readBuf.resize( 0x40000 );

    datHandle = FileOpen( fname, false );
    if( !datHandle )
    {
        App.WriteLogF( _FUNC_, " - Cannot open file.\n" );
        return false;
    }

    FileGetTime( datHandle, timeCreate, timeAccess, timeWrite );

    if( !ReadTree() )
    {
        App.WriteLogF( _FUNC_, " - Read file tree fail.\n" );
        return false;
    }

    return true;
}

DataFileDat::~DataFileDat()
{
    if( datHandle )
    {
        FileClose( datHandle );
        datHandle = NULL;
    }
    SAFEDELA( memTree );
}

bool DataFileDat::ReadTree()
{
    uint version;
    if( !FileSetPointer( datHandle, -12, SEEK_END ) )
        return false;
    if( !FileRead( datHandle, &version, 4 ) )
        return false;

    // DAT 2.1 Arcanum
    if( version == 0x44415431 ) // 1TAD
    {
        // Readed data
        uint files_total, tree_size;

        // Read info
        if( !FileSetPointer( datHandle, -4, SEEK_END ) )
            return false;
        if( !FileRead( datHandle, &tree_size, 4 ) )
            return false;

        // Read tree
        if( !FileSetPointer( datHandle, -(int)tree_size, SEEK_END ) )
            return false;
        if( !FileRead( datHandle, &files_total, 4 ) )
            return false;
        tree_size -= 28 + 4;     // Subtract information block and files total
        memTree = new uint8[tree_size];
        memzero( memTree, tree_size );
        if( !FileRead( datHandle, memTree, tree_size ) )
            return false;

        // Indexing tree
        uint8* ptr = memTree;
        uint8* end_ptr = memTree + tree_size;
        while( true )
        {
            uint fnsz, type;                             // Include zero
            memcpy( &fnsz, ptr, sizeof(fnsz) );
            memcpy( &type, ptr + 4 + fnsz + 4, sizeof(type) );

            if( fnsz > 1 && type != 0x400 ) // Not folder
            {
                string name = string( (const char*)ptr + 4, fnsz );
                transform( name.begin(), name.end(), name.begin(), ::tolower );
                replace( name.begin(), name.end(), '/', '\\' );

                if( type == 2 )
                    *(ptr + 4 + fnsz + 7) = 1;                 // Compressed
                filesTree.insert( make_pair( name, ptr + 4 + fnsz + 7 ) );
            }

            if( ptr + fnsz + 24 >= end_ptr )
                break;
            ptr += fnsz + 24;
        }

        return true;
    }

    // DAT 2.0 Fallout2
    // Readed data
    uint dir_count, dat_size, files_total, tree_size;

    // Read info
    if( !FileSetPointer( datHandle, -8, SEEK_END ) )
        return false;
    if( !FileRead( datHandle, &tree_size, 4 ) )
        return false;
    if( !FileRead( datHandle, &dat_size, 4 ) )
        return false;

    // Check for DAT1.0 Fallout1 dat file
    if( !FileSetPointer( datHandle, 0, SEEK_SET ) )
        return false;
    if( !FileRead( datHandle, &dir_count, 4 ) )
        return false;
    dir_count >>= 24;
    if( dir_count == 0x01 || dir_count == 0x33 )
        return false;

    // Check for truncated
    if( FileGetSize( datHandle ) != dat_size )
        return false;

    // Read tree
    if( !FileSetPointer( datHandle, -( (int)tree_size + 8 ), SEEK_END ) )
        return false;
    if( !FileRead( datHandle, &files_total, 4 ) )
        return false;
    tree_size -= 4;
    memTree = new uint8[tree_size];
    memzero( memTree, tree_size );
    if( !FileRead( datHandle, memTree, tree_size ) )
        return false;

    // Indexing tree
    uint8* ptr = memTree;
    uint8* end_ptr = memTree + tree_size;
    while( true )
    {
        uint fnsz;
        memcpy( &fnsz, ptr, sizeof(fnsz) );

        if( fnsz )
        {
            string name = string( (const char*)ptr + 4, fnsz );
            transform( name.begin(), name.end(), name.begin(), ::tolower );
            replace( name.begin(), name.end(), '/', '\\' );

            filesTree.insert( make_pair( string( name ), ptr + 4 + fnsz ) );
        }

        if( ptr + fnsz + 17 >= end_ptr )
            break;
        ptr += fnsz + 17;
    }

    return true;
}

uint8* DataFileDat::OpenFile( const char* fname, uint& len )
{
    if( !datHandle )
        return NULL;

    auto it = filesTree.find( fname );
    if( it == filesTree.end() )
        return NULL;

    uint8* ptr = it->second;
    uint8  type;
    uint   real_size, packed_size, offset;

    memcpy( &type, ptr, sizeof(type) );
    memcpy( &real_size, ptr + 1, sizeof(real_size) );
    memcpy( &packed_size, ptr + 5, sizeof(packed_size) );
    memcpy( &offset, ptr + 9, sizeof(offset) );

    if( !FileSetPointer( datHandle, offset, SEEK_SET ) )
        return NULL;

    len = real_size;
    uint8* buf = new uint8[len + 1];

    if( !type )
    {
        if( !FileRead( datHandle, buf, len ) )
        {
            delete[] buf;
            return NULL;
        }
    }
    else
    {
        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        stream.next_in = Z_NULL;
        stream.avail_in = 0;
        if( inflateInit( &stream ) != Z_OK )
        {
            delete[] buf;
            return NULL;
        }

        stream.next_out = buf;
        stream.avail_out = real_size;

        uint left = packed_size;
        while( stream.avail_out )
        {
            if( !stream.avail_in && left > 0 )
            {
                stream.next_in = &readBuf[0];
                uint rb;
                if( !FileRead( datHandle, &readBuf[0], MIN( left, (uint)readBuf.size() ), &rb ) )
                {
                    delete[] buf;
                    return NULL;
                }
                stream.avail_in = rb;
                left -= rb;
            }
            int r = inflate( &stream, Z_NO_FLUSH );
            if( r != Z_OK && r != Z_STREAM_END )
            {
                delete[] buf;
                return NULL;
            }
            if( r == Z_STREAM_END )
                break;
        }

        inflateEnd( &stream );
    }

    buf[len] = 0;
    return buf;
}

void DataFileDat::GetFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result )
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

void DataFileDat::GetTime( uint64* create, uint64* access, uint64* write )
{
    if( create )
        *create = timeCreate;
    if( access )
        *access = timeAccess;
    if( write )
        *write = timeWrite;
}
