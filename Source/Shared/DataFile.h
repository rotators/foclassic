#ifndef __DATA_FILE__
#define __DATA_FILE__

// Supports:
//  - Fallout DAT
//  - Arcanum DAT
//  - Zip

#include "Types.h"

class DataFile
{
public:
    static DataFile* Open( const char* fname );

    virtual ~DataFile() {}

    virtual const std::string& GetPackName() = 0;
    virtual uint8*             OpenFile( const char* fname, uint& len ) = 0;
    virtual void               GetFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result ) = 0;
    virtual void               GetTime( uint64* create, uint64* access, uint64* write ) = 0;
};

typedef std::vector<DataFile*> DataFileVec;

#endif // __DATA_FILE__ //
