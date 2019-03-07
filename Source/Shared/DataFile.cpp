#include "Core.h"
#include "App.h"

#include "DataFile.h"
#include "FileSystem.h"
#include "Text.h"

#include "DataFile/Dat.h"
#include "DataFile/Zip.h"

DataFile* DataFile::Open( const char* fname )
{
    if( !fname || !fname[0] )
    {
        App.WriteLogF( _FUNC_, " - Invalid file name, empty or nullptr.\n" );
        return nullptr;
    }

    const char* ext = Str::Substring( fname, "." );
    if( !ext )
    {
        App.WriteLogF( _FUNC_, " - File<%s> extension not found.\n", fname );
        return nullptr;
    }

    const char* ext_ = ext;
    while( ext_ = Str::Substring( ext_ + 1, "." ) )
        ext = ext_;

    if( Str::CompareCase( ext, ".dat" ) ) // Try open DAT
    {
        DataFileDat* dat = new DataFileDat();
        if( !dat || !dat->Init( fname ) )
        {
            App.WriteLogF( _FUNC_, " - Unable to open DAT file<%s>.\n", fname );
            if( dat )
                delete dat;
            return nullptr;
        }

        return dat;
    }
    else if( Str::CompareCase( ext, ".zip" ) || Str::CompareCase( ext, ".bos" ) ) // Try open ZIP, BOS
    {
        DataFileZip* zip = new DataFileZip();
        if( !zip || !zip->Init( fname ) )
        {
            App.WriteLogF( _FUNC_, " - Unable to open ZIP file<%s>.\n", fname );
            if( zip )
                delete zip;
            return nullptr;
        }

        return zip;
    }
    else     // Bad file format
    {
        App.WriteLogF( _FUNC_, " - Invalid file<%s> format; only DAT, ZIP are supported.\n", fname );
    }

    return nullptr;
}
