#include "Core.h"

#include <unzip.h>

#include "DataFile.h"
#include "FileSystem.h"
#include "Log.Shared.h"
#include "Text.h"

#include "DataFile/Dat.h"
#include "DataFile/Zip.h"

DataFile* DataFile::Open( const char* fname )
{
    if( !fname || !fname[0] )
    {
        WriteLogF( _FUNC_, " - Invalid file name, empty or nullptr.\n" );
        return NULL;
    }

    const char* ext = Str::Substring( fname, "." );
    if( !ext )
    {
        WriteLogF( _FUNC_, " - File<%s> extension not found.\n", fname );
        return NULL;
    }

    const char* ext_ = ext;
    while( ext_ = Str::Substring( ext_ + 1, "." ) )
        ext = ext_;

    if( Str::CompareCase( ext, ".dat" ) ) // Try open DAT
    {
        DataFileDat* dat = new DataFileDat();
        if( !dat || !dat->Init( fname ) )
        {
            WriteLogF( _FUNC_, " - Unable to open DAT file<%s>.\n", fname );
            if( dat )
                delete dat;
            return NULL;
        }

        return dat;
    }
    else if( Str::CompareCase( ext, ".zip" ) || Str::CompareCase( ext, ".bos" ) ) // Try open ZIP, BOS
    {
        DataFileZip* zip = new DataFileZip();
        if( !zip || !zip->Init( fname ) )
        {
            WriteLogF( _FUNC_, " - Unable to open ZIP file<%s>.\n", fname );
            if( zip )
                delete zip;
            return NULL;
        }

        return zip;
    }
    else     // Bad file format
    {
        WriteLogF( _FUNC_, " - Invalid file<%s> format; only DAT, ZIP are supported.\n", fname );
    }

    return NULL;
}
