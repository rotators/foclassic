#ifndef __DATA_FILE__ZIP__
#define __DATA_FILE__ZIP__

#include <unzip.h>

#include "DataFile.h"
#include "Types.h"

class DataFileZip : public DataFile
{
private:
    struct ZipFileInfo
    {
        unz_file_pos Pos;
        uLong        UncompressedSize;
    };
    typedef std::map<std::string, ZipFileInfo> IndexMap;

    IndexMap    filesTree;
    std::string fileName;
    unzFile     zipHandle;

    uint64      timeCreate, timeAccess, timeWrite;

    bool ReadTree();

public:
    bool Init( const char* fname );
    ~DataFileZip();

    const std::string& GetPackName() { return fileName; }
    uint8*             OpenFile( const char* fname, uint& len );
    void               GetFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result );
    void               GetTime( uint64* create, uint64* access, uint64* write );
};

#endif // __DATA_FILE_ZIP__ //
