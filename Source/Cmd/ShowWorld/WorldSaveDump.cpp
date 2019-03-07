#include "App.h"

#include <cstddef>
#include <stdexcept>

#include "FileSystem.h"
#include "Text.h"
#include "WorldSave.h"
#include "WorldSaveDump.h"

using namespace std;

#define DUMP_ARRAY_TO_STRING( name, str )                      \
    str = "[";                                                 \
    for( uint x = 0; x < sizeof(name) / sizeof(name[0]); x++ ) \
    {                                                          \
        if( x )                                                \
            str += ",";                                        \
        str += to_string( (long long)name[x] );                \
    }                                                          \
    str += "]"

inline uint GetRealWorldVersion( const WorldSave::Object::Signature& signature )
{
    uint16 version = signature.Version;

    if( signature.Legacy && signature.Version == 0x13 )
        version = 1;

    return version;
}

void WorldSaveObject::Insert( const string& base, string name1, string value, uint16 length, uint offset_of )
{
    auto it = Data.find( base );
    if( it == Data.end() )
        return;

    string        name = (!base.empty() ? base + string( "::" ) : string() ) + name1;

    WorldSaveData data = WorldSaveData( it->second.Name0, name, it->second.Index0, it->second.Index1, length, it->second.Offset + offset_of );
    data.Value = value;
    data.Inserted = true;

    Data[name] = data;
}

WorldSaveDump::WorldSaveDump( const WorldSave::Object::Signature& signature )
{
    WorldVersion = GetRealWorldVersion( signature );

    const char* fmt = Str::FormatBuf( "%X", signature.OffsetEnd );
    OffsetLength = Str::Length( fmt );
    if( OffsetLength % 2 != 0 )
        OffsetLength++;
}

WorldSaveData& WorldSaveObject::operator[]( string key )
{
    auto it = Data.find( key );
    if( it != Data.end() )
        return it->second;
    else
    {
        static WorldSaveData ignored;
        return ignored;
    }
}

template<typename W>
WorldSave* NewWorldSetup( const WorldSave::Object::Signature& signature, void* file, string filename )
{
    W* world = new W( signature, file, filename );
    world->Dump = new WorldSaveDump( signature );

    world->LogLevel = 0;
    world->OnCounterLoaded.push_back( bind( &WorldSaveDump::OnNewCounter, world->Dump, placeholders::_1, placeholders::_2 ) );
    world->OnObjectLoaded.push_back( bind( &WorldSaveDump::OnNewObject, world->Dump, placeholders::_1, placeholders::_2, placeholders::_3 ) );
    world->OnGroupLoaded.push_back( bind( &WorldSaveDump::OnNewGroup, world->Dump, placeholders::_1, placeholders::_2, placeholders::_3 ) );

    return world;
}

WorldSave* WorldSaveDump::NewWorld( const WorldSave::Object::Signature& signature, void* file, const string& filename )
{
    switch( GetRealWorldVersion( signature ) )
    {
        case 1:
            return NewWorldSetup<WorldSaveV1>( signature, file, filename );
    }

    return nullptr;
}

void WorldSaveDump::LoadWorldBegin( WorldSave* world )
{
    WorldSave::Object::Signature signature = world->GetSignature();

    Dump[0] = Str::FormatBuf( "%5u  Signature = Version:%u Legacy:%s", signature.SizeBegin, signature.Version, signature.Legacy ? "true" : "false" );

    DumpCacheAndPrint();
}

void WorldSaveDump::LoadWorldEnd( WorldSave* world, bool success )
{
    WorldSave::Object::Signature signature = world->GetSignature();

    if( signature.OffsetEnd )
    {
        if( !signature.Legacy )
            Dump[signature.OffsetEnd] = Str::FormatBuf( "%5u  Version = %u", sizeof(signature.Version), signature.Version );
        else
            Dump[signature.OffsetEnd] = Str::FormatBuf( "%5u  Signature = Version:%u Legacy:true", signature.SizeEnd, signature.Version );

        DumpCacheAndPrint();
    }
}

void WorldSaveDump::ReadDataBegin( void* file, const uint& len, const string& name0, const string& name1, const uint& index0, const uint& index1 )
{
    if( len >= MAX_UINT16 )
    {
        // WorldSaveDump stores data length as uint16, unlike WorldSave which uses uint32
        App.WriteLog( "ERROR : object<%s> size<%u>\n", WorldSave::GetDataName( name0, name1, index0, index1 ).c_str(), len );
        throw length_error( "WorldSave data" );
    }

    WorldSaveObject& obj = Cache[name0][index0][index1];
    obj.Data[name1] = WorldSaveData( name0, name1, index0, index1, len, FileGetPointer( file ) );
}

void WorldSaveDump::ReadDataEnd( void* file, const uint& len, const string& name0, const string& name1, const uint& index0, const uint& index1, bool success )
{
    // NOP
}

void WorldSaveDump::DumpObject( WorldSaveObject& object )
{
    for( auto it = object.Data.begin(), end = object.Data.end(); it != end; ++it )
    {
        WorldSaveData& data = it->second;

        if( !data.Offset )
        {
            App.WriteLog( "WARNING : NO OFFSET <%s>\n", it->first.c_str() );
        }

        string value;
        if( data.Value.empty() )
            value = Str::FormatBuf( " = ???" );
        else if( data.Value == "\n" )
            value = "";
        else
            value = Str::FormatBuf( " = %s", data.Value.c_str() );

        Dump[data.Offset] = Str::FormatBuf( "%5u %s%s%s", data.Length, data.Inserted ? "*" : " ", WorldSave::GetDataName( data.Name0, data.Name1, data.Index0, data.Index1 ).c_str(), value.c_str() );
    }
}

void WorldSaveDump::DumpCounter( const string& name, const uint& value )
{
    auto it = Cache.find( name );
    if( it != Cache.end() )
    {
        WorldSaveObject& obj = it->second[MAX_UINT][MAX_UINT];

        obj[""].Value = to_string( (long long)value );

        DumpObject( obj );
    }
}

void WorldSaveDump::DumpCache()
{
    for( auto name0 = Cache.begin(), name0end = Cache.end(); name0 != name0end; ++name0 )
    {
        for( auto index0 = name0->second.begin(), index0end = name0->second.end(); index0 != index0end; ++index0 )
        {
            for( auto index1 = index0->second.begin(), index1end = index0->second.end(); index1 != index1end; ++index1 )
            {
                WorldSaveObject& obj = index1->second;

                DumpObject( obj );
            }

            index0->second.clear();
        }

        name0->second.clear();
    }

    Cache.clear();
}

void WorldSaveDump::DumpPrint()
{
    const char* format = Str::FormatBuf( "0x%%0%uX  %%s\n", OffsetLength );

    for( auto it = Dump.begin(), end = Dump.end(); it != end; ++it )
    {
        App.WriteLog( format, it->first, it->second.c_str() );
    }
}

void WorldSaveDump::DumpCacheAndPrint()
{
    DumpCache();
    DumpPrint();
    Dump.clear();
}

void WorldSaveDump::OnNewCounter( uint count, const string& name )
{
    DumpCounter( name, count );
}

// called after WorldSave::Object is fully loaded
void WorldSaveDump::OnNewObject( void*& object, const string& name, const uint& version )
{
    bool unknown_version = false;

    if( !object )
        throw runtime_error( "Received null object from WorldSave" );
    else if( name.empty() )
        throw runtime_error( "Received unnamed object from WorldSave" );
    else if( !version )
        throw runtime_error( "Received unversioned object from WorldSave" );
    //
    else if( name == WorldSave::ID::SinglePlayer )
    {
        if( version == 1 )
        {
            ReadSinglePlayer( (WorldSave::Object::SinglePlayerV1*)object );
            delete (WorldSave::Object::SinglePlayerV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::Time )
    {
        if( version == 1 )
        {
            ReadTime( (WorldSave::Object::TimeV1*)object );
            delete (WorldSave::Object::TimeV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::Score )
    {
        if( version == 1 )
        {
            ReadScore( (WorldSave::Object::ScoreV1*)object );
            delete (WorldSave::Object::ScoreV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::Location )
    {
        if( version == 1 )
        {
            ReadLocation( (WorldSave::Object::LocationV1*)object );
            delete (WorldSave::Object::LocationV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::Critter )
    {
        if( version == 1 )
        {
            ReadCritter( (WorldSave::Object::CritterV1*)object );
            delete (WorldSave::Object::CritterV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::Item )
    {
        if( version == 1 )
        {
            ReadItem( (WorldSave::Object::ItemV1*)object );
            delete (WorldSave::Object::ItemV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::Var )
    {
        if( version == 1 )
        {
            ReadVar( (WorldSave::Object::VarV1*)object );
            delete (WorldSave::Object::VarV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::Holo )
    {
        if( version == 1 )
        {
            ReadHolo( (WorldSave::Object::HoloV1*)object );
            delete (WorldSave::Object::HoloV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::AnyData )
    {
        if( version == 1 )
        {
            ReadAnyData( (WorldSave::Object::AnyDataV1*)object );
            delete(WorldSave::Object::AnyDataV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::TimeEvent )
    {
        if( version == 1 )
        {
            ReadTimeEvent( (WorldSave::Object::TimeEventV1*)object );
            delete (WorldSave::Object::TimeEventV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == WorldSave::ID::ScriptFunction )
    {
        if( version == 1 )
        {
            ReadScriptFunction( (WorldSave::Object::ScriptFunctionV1*)object );
            delete (WorldSave::Object::ScriptFunctionV1*)object;
        }
        else
            unknown_version = true;
    }
    else
    {
        App.WriteLog( "ERROR : unknown new object : object<%s> version<%u>", name.c_str(), version );
        throw runtime_error( "Received unknown object from WorldSave" );
    }

    // known objects should be already deleted at this point, object with unsupported version has to be kept intact
    if( unknown_version )
        App.WriteLog( "WARNING : unsupported version : object<%s> version<%u>", name.c_str(), version );
    else
        object = nullptr;
}

void WorldSaveDump::OnNewGroup( vector<void*>& group, const string& name, const uint& version )
{
    // Selective unloading from previous system, left for reference
    /*
       if (WorldVersion == 1)
       {
            WorldSaveV1* world = (WorldSaveV1*)this;

            void (WorldSaveV1::*unload)() = nullptr;

            if (name == "Location")
                    unload = &WorldSaveV1::UnloadLocations;
            else if (name == "Critter")
                    unload = &WorldSaveV1::UnloadCritters;
            else if (name == "Item")
                    unload = &WorldSaveV1::UnloadItems;

            if (unload)
            {
                    (*world.*unload)();
                    string target = name;
                    transform(target.begin(), target.end(), target.begin(), ::tolower);
                    world->LogLoad("unloaded<%ss>", target.c_str());
            }
       }
     */

    DumpCacheAndPrint();
}

void WorldSaveDump::ReadSinglePlayer( WorldSave::Object::SinglePlayerV1* singleplayer )
{
    WorldSaveObject& object = Cache[WorldSave::ID::SinglePlayer][MAX_UINT][MAX_UINT];

    object["Version"].Value = to_string( (long long)singleplayer->Version );

    if( !singleplayer->Version )
        return;

    // TODO
}

void WorldSaveDump::ReadTime( WorldSave::Object::TimeV1* time )
{
    if( !time )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Time][MAX_UINT][MAX_UINT];

    object.Insert( "", "YearStart", to_string( (long long)time->YearStart ), sizeof(time->YearStart), offsetof( WorldSave::Object::TimeV1, YearStart ) );
    object.Insert( "", "Year", to_string( (long long)time->Year ), sizeof(time->Year), offsetof( WorldSave::Object::TimeV1, Year ) );
    object.Insert( "", "Month", to_string( (long long)time->Month ), sizeof(time->Month), offsetof( WorldSave::Object::TimeV1, Month ) );
    object.Insert( "", "Day", to_string( (long long)time->Day ), sizeof(time->Day), offsetof( WorldSave::Object::TimeV1, Day ) );
    object.Insert( "", "Hour", to_string( (long long)time->Hour ), sizeof(time->Hour), offsetof( WorldSave::Object::TimeV1, Hour ) );
    object.Insert( "", "Minute", to_string( (long long)time->Minute ), sizeof(time->Minute), offsetof( WorldSave::Object::TimeV1, Minute ) );
    object.Insert( "", "Second", to_string( (long long)time->Second ), sizeof(time->Second), offsetof( WorldSave::Object::TimeV1, Second ) );
    object.Insert( "", "Multiplier", to_string( (long long)time->Multiplier ), sizeof(time->Multiplier), offsetof( WorldSave::Object::TimeV1, Multiplier ) );
}

void WorldSaveDump::ReadScore( WorldSave::Object::ScoreV1* score )
{
    if( !score )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Score][score->Index][MAX_UINT];

    object["ClientId"].Value = to_string( (long long)score->ClientId );
    object["ClientName"].Value = score->ClientName;
    object["Value"].Value = to_string( (long long)score->Value );
}

void WorldSaveDump::ReadLocation( WorldSave::Object::LocationV1* location )
{
    if( !location )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Location][location->Index][MAX_UINT];

    ReadLocationData( "Data", location->Data, object );

    object["MapsCount"].Value = to_string( (long long)location->MapsCount );
    for( auto it = location->Maps.begin(), end = location->Maps.end(); it != end; ++it )
    {
        ReadMap( *it );
    }
}

void WorldSaveDump::ReadLocationData( const string& base, WorldSave::Object::LocationDataV1* data, WorldSaveObject& object )
{
    if( !data )
        return;

    object.Insert( base, "Id", to_string( (long long)data->LocId ), sizeof(data->LocId), offsetof( WorldSave::Object::LocationDataV1, LocId ) );
    object.Insert( base, "Pid", to_string( (long long)data->LocPid ), sizeof(data->LocPid), offsetof( WorldSave::Object::LocationDataV1, LocPid ) );
    object.Insert( base, "WorldX", to_string( (long long)data->WorldX ), sizeof(data->WorldX), offsetof( WorldSave::Object::LocationDataV1, WorldX ) );
    object.Insert( base, "WorldY", to_string( (long long)data->WorldY ), sizeof(data->WorldY), offsetof( WorldSave::Object::LocationDataV1, WorldY ) );
    object.Insert( base, "Radius", to_string( (long long)data->Radius ), sizeof(data->Radius), offsetof( WorldSave::Object::LocationDataV1, Radius ) );
    object.Insert( base, "Visible", data->Visible ? "true" : "false", sizeof(data->Visible), offsetof( WorldSave::Object::LocationDataV1, Visible ) );
    object.Insert( base, "GeckVisible", data->GeckVisible ? "true" : "false", sizeof(data->GeckVisible), offsetof( WorldSave::Object::LocationDataV1, GeckVisible ) );
    object.Insert( base, "AutoGarbage", data->AutoGarbage ? "true" : "false", sizeof(data->AutoGarbage), offsetof( WorldSave::Object::LocationDataV1, AutoGarbage ) );
    object.Insert( base, "ToGarbage", data->ToGarbage ? "true" : "false", sizeof(data->ToGarbage), offsetof( WorldSave::Object::LocationDataV1, ToGarbage ) );
    object.Insert( base, "Color", to_string( (long long)data->Color ), sizeof(data->Color), offsetof( WorldSave::Object::LocationDataV1, Color ) );
    object.Insert( base, "Reserved3", "\n", sizeof(data->Reserved3), offsetof( WorldSave::Object::LocationDataV1, Reserved3 ) );
}

void WorldSaveDump::ReadMap( WorldSave::Object::MapV1* map )
{
    if( !map )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Map][map->LocationIndex][map->MapIndex];

    object["Data"].Value = "\n";
    ReadMapData( "Data", map->Data, object );
}

void WorldSaveDump::ReadMapData( const string& base, WorldSave::Object::MapDataV1* data, WorldSaveObject& object )
{
    if( !data )
        return;

    string tmp;

    object.Insert( base, "MapId", to_string( (long long)data->MapId ), sizeof(data->MapId), offsetof( WorldSave::Object::MapDataV1, MapId ) );
    object.Insert( base, "MapPid", to_string( (long long)data->MapPid ), sizeof(data->MapPid), offsetof( WorldSave::Object::MapDataV1, MapPid ) );
    object.Insert( base, "MapRain", to_string( (long long)data->MapRain ), sizeof(data->MapRain), offsetof( WorldSave::Object::MapDataV1, MapRain ) );
    object.Insert( base, "IsTurnbasedAviable", data->IsTurnBasedAviable ? "true" : "false", sizeof(data->IsTurnBasedAviable), offsetof( WorldSave::Object::MapDataV1, IsTurnBasedAviable ) );
    object.Insert( base, "MapTime", to_string( (long long)data->MapTime ), sizeof(data->MapTime), offsetof( WorldSave::Object::MapDataV1, MapTime ) );
    object.Insert( base, "ScriptId", to_string( (long long)data->ScriptId ), sizeof(data->ScriptId), offsetof( WorldSave::Object::MapDataV1, ScriptId ) );

    DUMP_ARRAY_TO_STRING( data->MapDayTime, tmp );
    object.Insert( base, "MapDayTime", tmp, sizeof(data->MapDayTime), offsetof( WorldSave::Object::MapDataV1, MapDayTime ) );

    DUMP_ARRAY_TO_STRING( data->MapDayColor, tmp );
    object.Insert( base, "MapDayColor", tmp, sizeof(data->MapDayColor), offsetof( WorldSave::Object::MapDataV1, MapDayColor ) );

    DUMP_ARRAY_TO_STRING( data->UserData, tmp );
    object.Insert( base, "UserData", tmp, sizeof(data->UserData), offsetof( WorldSave::Object::MapDataV1, UserData ) );
}

void WorldSaveDump::ReadCritter( WorldSave::Object::CritterV1* critter )
{
    if( !critter )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Critter][critter->Index][MAX_UINT];

    object["Data"].Value = "\n";
    ReadCritterData( "Data", critter->Data, object );

    object["DataExt"].Value = "\n";  // TODO
    ReadCritterDataExt( "DataExt", critter->DataExt, object );

    object["TimeEventsCount"].Value = to_string( (long long)critter->TimeEventsCount );
    // TODO CritterTimeEvent
}

void WorldSaveDump::ReadCritterData( const std::string& base, WorldSave::Object::CritterDataV1* data, WorldSaveObject& object )
{
    if( !data )
        return;

    string tmp;

    object.Insert( base, "Id", to_string( (long long)data->Id ), sizeof(data->Id), offsetof( WorldSave::Object::CritterDataV1, Id ) );
    object.Insert( base, "HexX", to_string( (long long)data->HexX ), sizeof(data->HexX), offsetof( WorldSave::Object::CritterDataV1, HexX ) );
    object.Insert( base, "Hexy", to_string( (long long)data->HexY ), sizeof(data->HexY), offsetof( WorldSave::Object::CritterDataV1, HexY ) );
    object.Insert( base, "WorldX", to_string( (long long)data->WorldX ), sizeof(data->WorldX), offsetof( WorldSave::Object::CritterDataV1, WorldX ) );
    object.Insert( base, "WorldY", to_string( (long long)data->WorldY ), sizeof(data->WorldY), offsetof( WorldSave::Object::CritterDataV1, WorldY ) );
    object.Insert( base, "BaseType", to_string( (long long)data->BaseType ), sizeof(data->BaseType), offsetof( WorldSave::Object::CritterDataV1, BaseType ) );
    object.Insert( base, "Dir", to_string( (long long)data->Dir ), sizeof(data->Dir), offsetof( WorldSave::Object::CritterDataV1, Dir ) );
    object.Insert( base, "Cond", to_string( (long long)data->Cond ), sizeof(data->Cond), offsetof( WorldSave::Object::CritterDataV1, Cond ) );
    object.Insert( base, "ScriptId", to_string( (long long)data->ScriptId ), sizeof(data->ScriptId), offsetof( WorldSave::Object::CritterDataV1, ScriptId ) );
    object.Insert( base, "ShowCritterDist1", to_string( (long long)data->ShowCritterDist1 ), sizeof(data->ShowCritterDist1), offsetof( WorldSave::Object::CritterDataV1, ShowCritterDist1 ) );
    object.Insert( base, "ShowCritterDist2", to_string( (long long)data->ShowCritterDist2 ), sizeof(data->ShowCritterDist2), offsetof( WorldSave::Object::CritterDataV1, ShowCritterDist2 ) );
    object.Insert( base, "ShowCritterDist3", to_string( (long long)data->ShowCritterDist3 ), sizeof(data->ShowCritterDist3), offsetof( WorldSave::Object::CritterDataV1, ShowCritterDist3 ) );
    object.Insert( base, "Multihex", to_string( (long long)data->Multihex ), sizeof(data->Multihex), offsetof( WorldSave::Object::CritterDataV1, Multihex ) );
    object.Insert( base, "GlobalGroupUid", to_string( (long long)data->GlobalGroupUid ), sizeof(data->GlobalGroupUid), offsetof( WorldSave::Object::CritterDataV1, GlobalGroupUid ) );
    object.Insert( base, "LastHexX", to_string( (long long)data->LastHexX ), sizeof(data->LastHexX), offsetof( WorldSave::Object::CritterDataV1, LastHexX ) );
    object.Insert( base, "LastHexY", to_string( (long long)data->LastHexY ), sizeof(data->LastHexY), offsetof( WorldSave::Object::CritterDataV1, LastHexY ) );
    object.Insert( base, "MapId", to_string( (long long)data->MapId ), sizeof(data->MapId), offsetof( WorldSave::Object::CritterDataV1, MapId ) );
    object.Insert( base, "MapPid", to_string( (long long)data->MapPid ), sizeof(data->MapPid), offsetof( WorldSave::Object::CritterDataV1, MapPid ) );

    DUMP_ARRAY_TO_STRING( data->Params, tmp );
    object.Insert( base, "Params", tmp, sizeof(data->Params), offsetof( WorldSave::Object::CritterDataV1, Params ) );

    object.Insert( base, "Anim1Life", to_string( (long long)data->Anim1Life ), sizeof(data->Anim1Life), offsetof( WorldSave::Object::CritterDataV1, Anim1Life ) );
    object.Insert( base, "Anim1Knockout", to_string( (long long)data->Anim1Knockout ), sizeof(data->Anim1Knockout), offsetof( WorldSave::Object::CritterDataV1, Anim1Knockout ) );
    object.Insert( base, "Anim1Dead", to_string( (long long)data->Anim1Dead ), sizeof(data->Anim1Dead), offsetof( WorldSave::Object::CritterDataV1, Anim1Dead ) );
    object.Insert( base, "Anim2Life", to_string( (long long)data->Anim2Life ), sizeof(data->Anim2Life), offsetof( WorldSave::Object::CritterDataV1, Anim2Life ) );
    object.Insert( base, "Anim2Knockout", to_string( (long long)data->Anim2Knockout ), sizeof(data->Anim2Knockout), offsetof( WorldSave::Object::CritterDataV1, Anim2Knockout ) );
    object.Insert( base, "Anim2Dead", to_string( (long long)data->Anim2Dead ), sizeof(data->Anim2Dead), offsetof( WorldSave::Object::CritterDataV1, Anim2Dead ) );
    object.Insert( base, "Anim2KnockoutEnd", to_string( (long long)data->Anim2KnockoutEnd ), sizeof(data->Anim2KnockoutEnd), offsetof( WorldSave::Object::CritterDataV1, Anim2KnockoutEnd ) );
    object.Insert( base, "Lexems", Str::Length( data->Lexems ) ? data->Lexems : "\n", sizeof(data->Lexems), offsetof( WorldSave::Object::CritterDataV1, Lexems ) );
    object.Insert( base, "ClientToDelete", data->ClientToDelete ? "true" : "false", sizeof(data->ClientToDelete), offsetof( WorldSave::Object::CritterDataV1, ClientToDelete ) );
    object.Insert( base, "Temp", to_string( (long long)data->Temp ), sizeof(data->Temp), offsetof( WorldSave::Object::CritterDataV1, Temp ) );
    object.Insert( base, "HoloInfoCount", to_string( (long long)data->HoloInfoCount ), sizeof(data->HoloInfoCount), offsetof( WorldSave::Object::CritterDataV1, HoloInfoCount ) );

    DUMP_ARRAY_TO_STRING( data->HoloInfo, tmp );
    object.Insert( base, "HoloInfo", tmp, sizeof(data->HoloInfo), offsetof( WorldSave::Object::CritterDataV1, HoloInfo ) );

    DUMP_ARRAY_TO_STRING( data->Scores, tmp );
    object.Insert( base, "Scores", tmp, sizeof(data->Scores), offsetof( WorldSave::Object::CritterDataV1, Scores ) );

    DUMP_ARRAY_TO_STRING( data->UserData, tmp );
    object.Insert( base, "UserData", tmp, sizeof(data->UserData), offsetof( WorldSave::Object::CritterDataV1, UserData ) );

    // Npc

    object.Insert( base, "HomeMap", to_string( (long long)data->HomeMap ), sizeof(data->HomeMap), offsetof( WorldSave::Object::CritterDataV1, HomeMap ) );
    object.Insert( base, "HomeX", to_string( (long long)data->HomeX ), sizeof(data->HomeX), offsetof( WorldSave::Object::CritterDataV1, HomeX ) );
    object.Insert( base, "HomeY", to_string( (long long)data->HomeY ), sizeof(data->HomeY), offsetof( WorldSave::Object::CritterDataV1, HomeY ) );
    object.Insert( base, "HomeOri", to_string( (long long)data->HomeOri ), sizeof(data->HomeOri), offsetof( WorldSave::Object::CritterDataV1, HomeOri ) );
    object.Insert( base, "ProtoId", to_string( (long long)data->ProtoId ), sizeof(data->ProtoId), offsetof( WorldSave::Object::CritterDataV1, ProtoId ) );
    object.Insert( base, "IsDataExt", data->IsDataExt ? "true" : "false", sizeof(data->IsDataExt), offsetof( WorldSave::Object::CritterDataV1, IsDataExt ) );

    DUMP_ARRAY_TO_STRING( data->FavoriteItemPid, tmp );
    object.Insert( base, "FavoriteItemPid", tmp, sizeof(data->FavoriteItemPid), offsetof( WorldSave::Object::CritterDataV1, FavoriteItemPid ) );
    object.Insert( base, "EnemyStackCount", to_string( (long long)data->EnemyStackCount ), sizeof(data->EnemyStackCount), offsetof( WorldSave::Object::CritterDataV1, EnemyStackCount ) );

    DUMP_ARRAY_TO_STRING( data->EnemyStack, tmp );
    object.Insert( base, "EnemyStack", tmp, sizeof(data->EnemyStack), offsetof( WorldSave::Object::CritterDataV1, EnemyStack ) );

    DUMP_ARRAY_TO_STRING( data->BagCurrentSet, tmp );
    object.Insert( base, "BagCurrentSet", tmp, sizeof(data->BagCurrentSet), offsetof( WorldSave::Object::CritterDataV1, BagCurrentSet ) );

    object.Insert( base, "BagRefreshTime", to_string( (long long)data->BagRefreshTime ), sizeof(data->BagRefreshTime), offsetof( WorldSave::Object::CritterDataV1, BagRefreshTime ) );
    object.Insert( base, "BagSize", to_string( (long long)data->BagSize ), sizeof(data->BagSize), offsetof( WorldSave::Object::CritterDataV1, BagSize ) );

    for( uint x = 0; x < sizeof(data->Bag) / sizeof(data->Bag[0]); x++ )
    {
        uint                             offset = sizeof(data->Bag) * x;
        WorldSave::Object::CritterBagV1& bag = data->Bag[x];

        object.Insert( base, Str::FormatBuf( "Bag[%u]::ItemPid", x ), to_string( (long long)bag.ItemPid ), sizeof(bag.ItemPid), offset + offsetof( WorldSave::Object::CritterBagV1, ItemPid ) );
        object.Insert( base, Str::FormatBuf( "Bag[%u]::MinCnt", x ), to_string( (long long)bag.MinCnt ), sizeof(bag.MinCnt), offset + offsetof( WorldSave::Object::CritterBagV1, MinCnt ) );
        object.Insert( base, Str::FormatBuf( "Bag[%u]::MaxCnt", x ), to_string( (long long)bag.MaxCnt ), sizeof(bag.MaxCnt), offset + offsetof( WorldSave::Object::CritterBagV1, MaxCnt ) );
        object.Insert( base, Str::FormatBuf( "Bag[%u]::ItemSlot", x ), to_string( (long long)bag.ItemSlot ), sizeof(bag.ItemPid), offset + offsetof( WorldSave::Object::CritterBagV1, ItemSlot ) );
    }
}

void WorldSaveDump::ReadCritterDataExt( const std::string& base, WorldSave::Object::CritterDataExtV1* data, WorldSaveObject& object )
{
    if( !data )
        return;
}

void WorldSaveDump::ReadItem( WorldSave::Object::ItemV1* item )
{
    if( !item )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Item][item->Index][MAX_UINT];

    object["Id"].Value = to_string( (long long)item->Id );
    object["Pid"].Value = to_string( (long long)item->Pid );
    object["Accessory"].Value = to_string( (long long)item->Accessory );
    DUMP_ARRAY_TO_STRING( item->AccBuffer, object["AccBuffer"].Value );
    object["Data"].Value = "\n";     // TODO
    object["LexemsLength"].Value = to_string( (long long)item->LexemsLength );
    object["Lexems"].Value = Str::FormatBuf( "\"%s\"", item->Lexems );
}

void WorldSaveDump::ReadVar( WorldSave::Object::VarV1* var )
{
    if( !var )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Var][var->Index][MAX_UINT];

    object["TempId"].Value = to_string( (long long)var->TempId );
    object["MasterId"].Value = to_string( (long long)var->MasterId );
    object["SlaveId"].Value = to_string( (long long)var->SlaveId );
    object["Value"].Value = to_string( (long long)var->Value );
}

void WorldSaveDump::ReadHolo( WorldSave::Object::HoloV1* holo )
{
    if( !holo )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::Holo][holo->Index][MAX_UINT];

    object["TitleLength"].Value = to_string( (long long)holo->TitleLength );
    object["Title"].Value = Str::FormatBuf( "\"%s\"", holo->Title );
    object["TextLength"].Value = to_string( (long long)holo->TextLength );
    object["Text"].Value = Str::FormatBuf( "\"%s\"", holo->Text );
}

void WorldSaveDump::ReadAnyData( WorldSave::Object::AnyDataV1* anydata )
{
    if( !anydata )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::AnyData][anydata->Index][MAX_UINT];

    object["NameLength"].Value = to_string( (long long)anydata->NameLength );
    object["Name"].Value = anydata->Name;
    object["DataLength"].Value = to_string( (long long)anydata->DataLength );
    object["Data"].Value = "\n"; // TODO?
}

void WorldSaveDump::ReadTimeEvent( WorldSave::Object::TimeEventV1* timevent )
{
    if( !timevent )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::TimeEvent][timevent->Index][MAX_UINT];

    object["Num"].Value = to_string( (long long)timevent->Num );
    object["ScriptNameLength"].Value = to_string( (long long)timevent->ScriptNameLength );
    object["ScriptName"].Value = timevent->ScriptName;
    object["BeginSecond"].Value = to_string( (long long)timevent->BeginSecond );
    object["Rate"].Value = to_string( (long long)timevent->Rate );
    object["ValuesLength"].Value = to_string( (long long)timevent->ValuesLength );
    object["Values"].Value = "\n";     // TODO?
}

void WorldSaveDump::ReadScriptFunction( WorldSave::Object::ScriptFunctionV1* function )
{
    if( !function )
        return;

    WorldSaveObject& object = Cache[WorldSave::ID::ScriptFunction][function->Index][MAX_UINT];

    object["NameLength"].Value = to_string( (long long)function->NameLength );
    object["Name"].Value = function->Name;
}
