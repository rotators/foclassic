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
    bool        Inserted;

    WorldSaveData() : Value( "" ), Name0( "" ), Name1( "" ), Index0( MAX_UINT ), Index1( MAX_UINT ), Offset( 0 ), Length( 0 ), Inserted( false ) {}
    WorldSaveData( std::string name0, std::string name1, uint index0, uint index1, uint length, uint offset ) : Value( "" ), Name0( name0 ), Name1( name1 ), Index0( index0 ), Index1( index1 ), Offset( offset ), Length( length ), Inserted( false ) {}
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

    // Insert new WorldSaveData, based on already existing entry; if <base> wasn't added previously, function does nothing
    void Insert( const std::string& base, std::string name1, std::string value, uint16 length, uint offset_of );
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

    // called before WorldSave::V*::LoadWord()
    void DumpWorldBegin();

    // called after WorldSave::V*::LoadWorld()
    void DumpWorldEnd( bool result );

    // called before WorldSave::ReadData()
    void DumpDataBegin( void* file, const uint& len, const std::string& name0, const std::string& name1, const uint& index0, const uint& index1 );

    // called after WorldSave::ReadData()
    void DumpDataEnd( void* file, const uint& len, const std::string& name0, const std::string& name1, const uint& index0, const uint& index1, bool result );

    void DumpObject( WorldSaveObject& object );
    void DumpObjectSimple( const string& name, const uint& value );
    void DumpProcess();
    void DumpPrint();
    void DumpAll();

    // WorldSave callback; called after single object has been fully loaded
    // calls matching Read*() function, filling values of previously cached data
    void NewObject( void*& object, const std::string& name, const uint& version );

    // WorldSave callback; called after object group has been fully loaded
    void NewGroup( std::vector<void*>& group, const std::string& name, const uint& version );

    void ReadLocation( WorldSave::Object::LocationV1* location );
    void ReadLocationData( const std::string& base, WorldSave::Object::LocationDataV1* data, WorldSaveObject& object );
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
            WorldSaveDump::DumpDataBegin( World, len, name0, name1, index0, index1 );                                                                                                         \
            bool result = WorldSave::ReadData( buf, len, name0, name1, index0, index1 );                                                                                                      \
            WorldSaveDump::DumpDataEnd( World, len, name0, name1, index0, index1, result );                                                                                                   \
                                                                                                                                                                                              \
            return result;                                                                                                                                                                    \
        }                                                                                                                                                                                     \
                                                                                                                                                                                              \
        virtual bool LoadWorld() override                                                                                                                                                     \
        {                                                                                                                                                                                     \
            WorldSaveDump::DumpWorldBegin();                                                                                                                                                  \
            bool result = WorldSave::V ## version::LoadWorld();                                                                                                                               \
            WorldSaveDump::DumpWorldEnd( result );                                                                                                                                            \
                                                                                                                                                                                              \
            return result;                                                                                                                                                                    \
        }                                                                                                                                                                                     \
    }

WORLD_SAVE( 1 );

#endif // __WORLD_SAVE_DUMPER__ //
