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

    std::map<std::string, WorldSaveData> Data;

    WorldSaveData& operator[]( std::string key );

    // Insert new WorldSaveData, based on already existing entry; if <base> wasn't added previously, function does nothing
    void Insert( const std::string& base, std::string name1, std::string value, uint16 length, uint offset_of );
};

class WorldSaveDump
{
private:
    uint WorldVersion;
    uint OffsetLength;

public:
    std::map<uint, std::string>                                            Dump;
    std::map<std::string, std::map<uint, std::map<uint, WorldSaveObject>>> Cache;

    WorldSaveDump( const WorldSave::Object::Signature& signature );

    // creates new WorldSave object matching version from signature, plus additional configuration
    // [signature] must be already loaded, and [file] opened
    static WorldSave* NewWorld( const WorldSave::Object::Signature& signature, void* file, const std::string& filename );

    // called before WorldSave::V*::LoadWord()
    void LoadWorldBegin( WorldSave* world );

    // called after WorldSave::V*::LoadWorld()
    void LoadWorldEnd( WorldSave* world, bool success );

    void DumpObject( WorldSaveObject& object );
    void DumpCounter( const std::string& name, const uint& value );
    void DumpCache();
    void DumpPrint();
    void DumpCacheAndPrint();

    // called before WorldSave::ReadData()
    void ReadDataBegin( void* file, const uint& len, const std::string& name0, const std::string& name1, const uint& index0, const uint& index1 );

    // called after WorldSave::ReadData()
    void ReadDataEnd( void* file, const uint& len, const std::string& name0, const std::string& name1, const uint& index0, const uint& index1, bool success );

    // WorldSave callback
    // called after object counter has been loaded
    void OnNewCounter( uint count, const std::string& name );

    // WorldSave callback
    // called after single object has been fully loaded
    // calls matching Read*() function, filling values of previously cached data
    void OnNewObject( void*& object, const std::string& name, const uint& version );

    // WorldSave callback
    // called after object group has been fully loaded
    void OnNewGroup( std::vector<void*>& group, const std::string& name, const uint& version );

    void ReadSinglePlayer( WorldSave::Object::SinglePlayerV1* singleplayer );
    void ReadTime( WorldSave::Object::TimeV1* time );
    void ReadScore( WorldSave::Object::ScoreV1* score );
    void ReadLocation( WorldSave::Object::LocationV1* location );
    void ReadLocationData( const std::string& base, WorldSave::Object::LocationDataV1* data, WorldSaveObject& object );
    void ReadMap( WorldSave::Object::MapV1* map );
    void ReadMapData( const std::string& base, WorldSave::Object::MapDataV1* data, WorldSaveObject& object );
    void ReadCritter( WorldSave::Object::CritterV1* critter );
    void ReadCritterData( const std::string& base, WorldSave::Object::CritterDataV1* data, WorldSaveObject& object );
    void ReadCritterDataExt( const std::string& base, WorldSave::Object::CritterDataExtV1* data, WorldSaveObject& object );
    void ReadCritterTimeEvent( WorldSave::Object::CritterTimeEventV1* timevent );
    void ReadItem( WorldSave::Object::ItemV1* item );
    void ReadVar( WorldSave::Object::VarV1* var );
    void ReadHolo( WorldSave::Object::HoloV1* holo );
    void ReadAnyData( WorldSave::Object::AnyDataV1* anydata );
    void ReadTimeEvent( WorldSave::Object::TimeEventV1* timevent );
    void ReadScriptFunction( WorldSave::Object::ScriptFunctionV1* function );
};

#define WORLD_SAVE( version )                                                                                                                                                                   \
    class WorldSaveV ## version : public WorldSave::V ## version                                                                                                                                \
    {                                                                                                                                                                                           \
public:                                                                                                                                                                                         \
        WorldSaveDump* Dump;                                                                                                                                                                    \
                                                                                                                                                                                                \
        WorldSaveV ## version( WorldSave::Object::Signature signature, void* file, std::string filename ) : WorldSave::V ## version( signature, file, filename ), Dump( nullptr ) {}            \
        virtual ~WorldSaveV ## version() { if( Dump ) { delete Dump; Dump = nullptr; } }                                                                                                        \
                                                                                                                                                                                                \
        virtual bool LoadWorld() override                                                                                                                                                       \
        {                                                                                                                                                                                       \
            Dump->LoadWorldBegin( this );                                                                                                                                                       \
            bool result = WorldSave::V ## version::LoadWorld();                                                                                                                                 \
            Dump->LoadWorldEnd( this, result );                                                                                                                                                 \
                                                                                                                                                                                                \
            return result;                                                                                                                                                                      \
        }                                                                                                                                                                                       \
                                                                                                                                                                                                \
        virtual bool ReadData( void* buf, uint len, const std::string& name0, const std::string& name1 = std::string(), const uint &index0 = MAX_UINT, const uint &index1 = MAX_UINT ) override \
        {                                                                                                                                                                                       \
            Dump->ReadDataBegin( World, len, name0, name1, index0, index1 );                                                                                                                    \
            bool result = WorldSave::ReadData( buf, len, name0, name1, index0, index1 );                                                                                                        \
            Dump->ReadDataEnd( World, len, name0, name1, index0, index1, result );                                                                                                              \
                                                                                                                                                                                                \
            return result;                                                                                                                                                                      \
        }                                                                                                                                                                                       \
    }

WORLD_SAVE( 1 );

#endif // __WORLD_SAVE_DUMPER__ //
