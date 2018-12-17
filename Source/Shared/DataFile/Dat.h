#ifndef __DATA_FILE__DAT__
#define __DATA_FILE__DAT__

#include "DataFile.h"
#include "Types.h"

class DataFileDat : public DataFile
{
private:
    typedef map<string, uchar*> IndexMap;

    IndexMap filesTree;
    string   fileName;
    uchar*   memTree;
    void*    datHandle;
    UCharVec readBuf;

    uint64   timeCreate, timeAccess, timeWrite;

    bool ReadTree();

public:
    bool Init( const char* fname );
    ~DataFileDat();

    const string& GetPackName() { return fileName; }
    uchar*        OpenFile( const char* fname, uint& len );
    void          GetFileNames( const char* path, bool include_subdirs, const char* ext, StrVec& result );
    void          GetTime( uint64* create, uint64* access, uint64* write );
};

#endif // __DATA_FILE_DAT__ //
