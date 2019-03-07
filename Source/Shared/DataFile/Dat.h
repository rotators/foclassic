#ifndef __DATA_FILE__DAT__
#define __DATA_FILE__DAT__

#include "DataFile.h"
#include "Types.h"

class DataFileDat : public DataFile
{
private:
    typedef std::map<std::string, uint8*> IndexMap;

    IndexMap    filesTree;
    std::string fileName;
    uint8*      memTree;
    void*       datHandle;
    UInt8Vec    readBuf;

    uint64      timeCreate, timeAccess, timeWrite;

    bool ReadTree();

public:
    bool Init( const char* fname );
    ~DataFileDat();

    const std::string& GetPackName() { return fileName; }
    uint8*             OpenFile( const char* fname, uint& len );
    void               GetFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result );
    void               GetTime( uint64* create, uint64* access, uint64* write );
};

#endif // __DATA_FILE_DAT__ //
