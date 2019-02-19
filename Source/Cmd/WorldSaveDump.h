#ifndef __WORLD_SAVE_DUMPER__
#define __WORLD_SAVE_DUMPER__

#include "Types.h"
#include "WorldSave.h"

struct WorldSaveData
{
    std::string Value;
    std::string Name0;
    std::string Name1;
    uint        Index0;
    uint        Index1;

    uint        Offset;
    uint16      Length;

    WorldSaveData() : Value( "" ), Name0( "" ), Name1( "" ), Index0( MAX_UINT ), Index1( MAX_UINT ), Length( 0 ), Offset( 0 ) {}
    WorldSaveData( std::string name0, std::string name1, uint index0, uint index1, uint length, uint offset ) : Value( "" ), Name0( name0 ), Name1( name1 ), Index0( index0 ), Index1( index1 ), Length( length ), Offset( offset ) {}
};

struct WorldSaveObject
{
    // object.Data["Name"]  use when creating WorldSaveData
    // object["Name"]       use when filling WorldSaveData::Value
    //
    // Second syntax returns dummy data if <key> wasn't cached previously;
    // prevents adding broken entries for optional data (such as ItemV1::Lexems)

    std::map<string, WorldSaveData> Data;

    WorldSaveData& operator[]( std::string key );
};

class WorldSaveDump
{
public:
    const uint                                              DumpVersion;
    uint                                                    DumpOffsetLength;

    std::map<uint, std::string>                             Dump;
    std::map<string, map<uint, map<uint, WorldSaveObject>>> DumpCache;

    WorldSaveDump( uint version );

    // creates new WorldSave object matching version from signature, plus additional configuration
    // [signature] must be already loaded, and [file] opened
    static WorldSave* NewWorld( const WorldSave::Object::Signature& signature, void* file, const string& filename );

    // called before each WorldSave::ReadData
    // stores each
    void CacheData( void* world, const uint& len, const std::string& name0, const std::string& name1, const uint& index0, const uint& index1 );

    void DumpObject( WorldSaveObject& object );
    void DumpObjectSimple( const string& name, const uint& value );
    void DumpProcess();
    void DumpPrint();

    // WorldSave callback; called after single object has been fully loaded
    // calls matching Read*() function, filling values of previously cached data
    void NewObject( void*& object, const std::string& name, const uint& version );

    // WorldSave callback; called after object group has been fully loaded
    // used to unload already processed parts of worldsave
    void NewGroup( std::vector<void*>& group, const std::string& name, const uint& version );

    void ReadLocation( WorldSave::Object::LocationV1* location );
    void ReadMap( WorldSave::Object::MapV1* map );
    void ReadCritter( WorldSave::Object::CritterV1* critter );
    void ReadItem( WorldSave::Object::ItemV1* item );
    void ReadVar( WorldSave::Object::VarV1* var );
    void ReadHolo( WorldSave::Object::HoloV1* holo );
    void ReadAnyData( WorldSave::Object::AnyDataV1* anydata );
    void ReadTimeEvent( WorldSave::Object::TimeEventV1* timevent );
    void ReadScriptFunction( WorldSave::Object::ScriptFunctionV1* function );
};

#define WORLD_SAVE( version )                                                                                                                                                                 \
    class WorldSaveV ## version : public WorldSave::V ## version, public WorldSaveDump                                                                                                        \
    {                                                                                                                                                                                         \
public:                                                                                                                                                                                       \
        WorldSaveV ## version( WorldSave::Object::Signature signature, void* file, std::string filename ) : WorldSave::V ## version( signature, file, filename ), WorldSaveDump( version ) {} \
                                                                                                                                                                                              \
        virtual bool ReadData( void* buf, uint len, const std::string& name0, const std::string& name1 = string(), const uint &index0 = MAX_UINT, const uint &index1 = MAX_UINT ) override    \
        {                                                                                                                                                                                     \
            WorldSaveDump::CacheData( World, len, name0, name1, index0, index1 );                                                                                                             \
            return WorldSave::ReadData( buf, len, name0, name1, index0, index1 );                                                                                                             \
        }                                                                                                                                                                                     \
    }

WORLD_SAVE( 1 );

#endif // __WORLD_SAVE_DUMPER__ //
