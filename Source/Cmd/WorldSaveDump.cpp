#include "App.h"

#include "FileSystem.h"
#include "Text.h"
#include "WorldSave.h"
#include "WorldSaveDump.h"

inline uint GetVersion( const WorldSave::Object::Signature& signature )
{
    uint16 version = signature.Version;

    if( signature.Legacy && signature.Version == 0x13 )
        version = 1;

    return version;
}

inline uint GetVersion( WorldSave* world )
{
    if( !world )
        return 0;

    return GetVersion( world->Signature );
}

WorldSaveDump::WorldSaveDump( uint version ) : DumpVersion( version )
{}

WorldSaveData& WorldSaveObject::operator[]( std::string key )
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
WorldSave* NewWorldSetup( const WorldSave::Object::Signature& signature, void* file, std::string filename )
{
    W* world = new W( signature, file, filename );

    // WorldSave
    world->LogLevel = 0;
    world->OnObjectLoaded.push_back( bind( &W::NewObject, world, placeholders::_1, placeholders::_2, placeholders::_3 ) );
    world->OnGroupLoaded.push_back( bind( &W::NewGroup, world, placeholders::_1, placeholders::_2, placeholders::_3 ) );

    // WorldSaveDump
    const char* fmt = Str::FormatBuf( "%X", signature.OffsetEnd );
    world->DumpOffsetLength = Str::Length( fmt );
    if( world->DumpOffsetLength % 2 != 0 )
        world->DumpOffsetLength++;

    // always add signature info
    world->Dump[0] = Str::FormatBuf( "%5u  Signature Version:%u Legacy:%s", signature.SizeBegin, signature.Version, signature.Legacy ? "true" : "false" );

    // TODO added too early
    if( signature.OffsetEnd )
    {
        if( !signature.Legacy )
            world->Dump[signature.OffsetEnd] = Str::FormatBuf( "%5u  Version %u", sizeof(signature.Version), signature.Version );
        else
            world->Dump[signature.OffsetEnd] = Str::FormatBuf( "%5u  Signature Version:%u Legacy:true", signature.Version, signature.SizeEnd );
    }

    return world;
}

WorldSave* WorldSaveDump::NewWorld( const WorldSave::Object::Signature& signature, void* file, const string& filename )
{
    WorldSave* world = nullptr;

    switch( GetVersion( signature ) )
    {
        case 1:
            world = NewWorldSetup<WorldSaveV1>( signature, file, filename );
            break;
    }

    return world;
}

void WorldSaveDump::CacheData( void* world, const uint& len, const std::string& name0, const std::string& name1, const uint& index0, const uint& index1 )
{
    WorldSaveObject& obj = DumpCache[name0][index0][index1];

    if( len >= MAX_UINT16 )
    {
        //
        App.WriteLog( "ERROR : object<%s> size<%u>\n", WorldSave::GetDataName( name0, name1, index0, index1 ).c_str(), len );
        throw runtime_error( "data too big" );
    }

    if( name1.empty() )
    {
        obj.Data[""] = WorldSaveData( name0, name1, index0, index1, len, FileGetPointer( world ) );
    }
    else
        obj.Data[name1] = WorldSaveData( name0, name1, index0, index1, len, FileGetPointer( world ) );
}

void WorldSaveDump::DumpObject( WorldSaveObject& object )
{
    for( auto it = object.Data.begin(), end = object.Data.end(); it != end; ++it )
    {
        WorldSaveData& data = it->second;

        if( !data.Offset )
        {
            App.WriteLog( "WARNING : NO OFFSET <%s>", it->first.c_str() );
        }

        string value;
        if( data.Value.empty() )
            value = Str::FormatBuf( " = ???" );
        else if( data.Value == "\n" )
            value = "";
        else
            value = Str::FormatBuf( " = %s", data.Value.c_str() );

        Dump[data.Offset] = Str::FormatBuf( "%5u  %s%s", data.Length, WorldSave::GetDataName( data.Name0, data.Name1, data.Index0, data.Index1 ).c_str(), value.c_str() );
    }
}

void WorldSaveDump::DumpObjectSimple( const string& name, const uint& value )
{
    if( DumpCache.find( name ) != DumpCache.end() )
    {
        WorldSaveObject& obj = DumpCache[name][MAX_UINT][MAX_UINT];

        obj[""].Value = to_string( (long long)value );

        DumpObject( obj );
    }
}

void WorldSaveDump::DumpProcess()
{
    // TODO needs better place for it
    if( DumpVersion == 1 )
    {
        WorldSaveV1* world = (WorldSaveV1*)this;

        // TODO SinglePlayerV1
        // TODO TimeV1
        // TODO ScoreV1[]
        DumpObjectSimple( "LocationsCount", world->Count.Locations );
        DumpObjectSimple( "CrittersCount", world->Count.Critters );
        DumpObjectSimple( "ItemsCount", world->Count.Items );
        DumpObjectSimple( "VarsCount", world->Count.Vars );
        DumpObjectSimple( "HolosCount", world->Count.Holos );
        DumpObjectSimple( "AnyDataCount", world->Count.AnyData );
        DumpObjectSimple( "TimeEventsCount", world->Count.TimeEvents );
        DumpObjectSimple( "ScriptFunctionsCount", world->Count.ScriptFunctions );
    }

    for( auto name0 = DumpCache.begin(), name0end = DumpCache.end(); name0 != name0end; ++name0 )
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

    DumpCache.clear();
}

void WorldSaveDump::DumpPrint()
{
    const char* format = Str::FormatBuf( "0x%%0%uX  %%s\n", DumpOffsetLength );

    for( auto it = Dump.begin(), end = Dump.end(); it != end; ++it )
    {
        App.WriteLog( format, it->first, it->second.c_str() );
    }
}

// called after WorldSave::Object is fully loaded
void WorldSaveDump::NewObject( void*& object, const string& name, const uint& version )
{
    bool unknown_version = false;

    if( !object )
        throw runtime_error( "Received null object from WorldSave" );
    else if( name.empty() )
        throw runtime_error( "Received unnamed object from WorldSave" );
    else if( !version )
        throw runtime_error( "Received unversioned object from WorldSave" );
    //
    else if( name == "Location" )
    {
        if( version == 1 )
        {
            ReadLocation( (WorldSave::Object::LocationV1*)object );
            delete (WorldSave::Object::LocationV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == "Critter" )
    {
        if( version == 1 )
        {
            ReadCritter( (WorldSave::Object::CritterV1*)object );
            delete (WorldSave::Object::CritterV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == "Item" )
    {
        if( version == 1 )
        {
            ReadItem( (WorldSave::Object::ItemV1*)object );
            delete (WorldSave::Object::ItemV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == "Var" )
    {
        if( version == 1 )
        {
            ReadVar( (WorldSave::Object::VarV1*)object );
            delete (WorldSave::Object::VarV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == "Holo" )
    {
        if( version == 1 )
        {
            ReadHolo( (WorldSave::Object::HoloV1*)object );
            delete (WorldSave::Object::HoloV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == "AnyData" )
    {
        if( version == 1 )
        {
            ReadAnyData( (WorldSave::Object::AnyDataV1*)object );
            delete(WorldSave::Object::AnyDataV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == "TimeEvent" )
    {
        if( version == 1 )
        {
            ReadTimeEvent( (WorldSave::Object::TimeEventV1*)object );
            delete (WorldSave::Object::TimeEventV1*)object;
        }
        else
            unknown_version = true;
    }
    else if( name == "ScriptFunction" )
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
        App.WriteLog( "ERROR : unknown object : object<%s> version<%u>", name.c_str(), version );
        throw runtime_error( "Received unknown object from WorldSave" );
    }

    // known objects should be already deleted at this point, object with unsupported version has to be kept intact
    if( unknown_version )
        App.WriteLog( "WARNING : unsupported version : object<%s> version<%u>", name.c_str(), version );
    else
        object = nullptr;
}

void WorldSaveDump::NewGroup( std::vector<void*>& group, const std::string& name, const uint& version )
{
    // Selective unloading from previous system, left for reference
    /*
       if (DumpVersion == 1)
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

    DumpProcess();
    DumpPrint();
    Dump.clear();
}

void WorldSaveDump::ReadLocation( WorldSave::Object::LocationV1* location )
{
    WorldSaveObject& object = DumpCache["Location"][location->Index][MAX_UINT];

    object["Data"].Value = "\n";     // TODO
    object["MapsCount"].Value = to_string( (long long)location->MapsCount );

    for( auto it = location->Maps.begin(), end = location->Maps.end(); it != end; ++it )
    {
        ReadMap( *it );
    }
}

void WorldSaveDump::ReadMap( WorldSave::Object::MapV1* map )
{
    WorldSaveObject& object = DumpCache["Map"][map->LocationIndex][map->MapIndex];

    object["Data"].Value = "\n";     // TODO
}

void WorldSaveDump::ReadCritter( WorldSave::Object::CritterV1* critter )
{
    WorldSaveObject& object = DumpCache["Critter"][critter->Index][MAX_UINT];

    object["Data"].Value = "\n";     // TODO
    object["DataExt"].Value = "\n";  // TODO
    object["TimeEventsCount"].Value = to_string( (long long)critter->TimeEventsCount );

    // TODO CritterTimeEvent
}

void WorldSaveDump::ReadItem( WorldSave::Object::ItemV1* item )
{
    WorldSaveObject& object = DumpCache["Item"][item->Index][MAX_UINT];

    object["Id"].Value = to_string( (long long)item->Id );
    object["Pid"].Value = to_string( (long long)item->Pid );
    object["Accessory"].Value = to_string( (long long)item->Accessory );

    object["AccBuffer"].Value = "[";
    for( uint idx = 0; idx < sizeof(item->AccBuffer) / sizeof(item->AccBuffer[0]); idx++ )
    {
        if( idx )
            object["AccBuffer"].Value += ",";
        object["AccBuffer"].Value += to_string( (long long)item->AccBuffer[idx] );
    }
    object["AccBuffer"].Value += "]";

    object["Data"].Value = "\n";     // TODO
    object["LexemsLength"].Value = to_string( (long long)item->LexemsLength );
    object["Lexems"].Value = Str::FormatBuf( "\"%s\"", item->Lexems );
}

void WorldSaveDump::ReadVar( WorldSave::Object::VarV1* var )
{
    WorldSaveObject& object = DumpCache["Var"][var->Index][MAX_UINT];

    object["TempId"].Value = to_string( (long long)var->TempId );
    object["MasterId"].Value = to_string( (long long)var->MasterId );
    object["SlaveId"].Value = to_string( (long long)var->SlaveId );
    object["Value"].Value = to_string( (long long)var->Value );
}

void WorldSaveDump::ReadHolo( WorldSave::Object::HoloV1* holo )
{
    WorldSaveObject& object = DumpCache["Holo"][holo->Index][MAX_UINT];

    object["TitleLength"].Value = to_string( (long long)holo->TitleLength );
    object["Title"].Value = Str::FormatBuf( "\"%s\"", holo->Title );
    object["TextLength"].Value = to_string( (long long)holo->TextLength );
    object["Text"].Value = Str::FormatBuf( "\"%s\"", holo->Text );
}

void WorldSaveDump::ReadAnyData( WorldSave::Object::AnyDataV1* anydata )
{
    WorldSaveObject& object = DumpCache["AnyData"][anydata->Index][MAX_UINT];

    object["NameLength"].Value = to_string( (long long)anydata->NameLength );
    object["Name"].Value = anydata->Name;
    object["DataLength"].Value = to_string( (long long)anydata->DataLength );
    object["Data"].Value = "\n"; // TODO?
}

void WorldSaveDump::ReadTimeEvent( WorldSave::Object::TimeEventV1* timevent )
{
    WorldSaveObject& object = DumpCache["TimeEvent"][timevent->Index][MAX_UINT];

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
    WorldSaveObject& object = DumpCache["ScriptFunction"][function->Index][MAX_UINT];

    object["NameLength"].Value = to_string( (long long)function->NameLength );
    object["Name"].Value = function->Name;
}
