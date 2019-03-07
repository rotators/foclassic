#include "Core.h"
#include "App.h"

#include <vector>
#include <map>
#include <cstdarg>


#include "FileSystem.h"
#include "Text.h"
#include "WorldSave.h"

#include "../AI.h"
#include "../Item.h"

using namespace std;

BINARY_SIGNATURE( WorldSignature, BINARY_TYPE_WORLDSAVE, 0 );

const string WorldSave::ID::AnyData = "AnyData";
const string WorldSave::ID::AnyDataCounter = "AnyDataCount";
const string WorldSave::ID::Critter = "Critter";
const string WorldSave::ID::CritterCounter = "CritterCount";
const string WorldSave::ID::CritterTimeEvent = "CritterTimeEvent";
const string WorldSave::ID::Holo = "Holo";
const string WorldSave::ID::HoloCounter = "HoloCount";
const string WorldSave::ID::Item = "Item";
const string WorldSave::ID::ItemCounter = "ItemCount";
const string WorldSave::ID::Location = "Location";
const string WorldSave::ID::LocationCounter = "LocationCount";
const string WorldSave::ID::Map = "Map";
const string WorldSave::ID::Score = "Score";
const string WorldSave::ID::ScriptFunction = "ScriptFunction";
const string WorldSave::ID::ScriptFunctionCounter = "ScriptFunctionCount";
const string WorldSave::ID::SinglePlayer = "SinglePlayer";
const string WorldSave::ID::Time = "Time";
const string WorldSave::ID::TimeEvent = "TimeEvent";
const string WorldSave::ID::TimeEventCounter = "TimeEventCount";
const string WorldSave::ID::Var = "Var";
const string WorldSave::ID::VarCounter = "VarCount";

template<typename T>
void DestroyPtrVec( vector<T*>& vec )
{
    while( !vec.empty() )
    {
        T* ptr = vec.back();
        if( ptr )
            delete ptr;
        vec.pop_back();
    }
}

static void StaticAsserts()
{
    STATIC_ASSERT( CRITTER_USER_DATA_SIZE == 400 );
    STATIC_ASSERT( GM_ZONES_FOG_SIZE == 2500 );
    STATIC_ASSERT( ITEM_MAX_SCRIPT_VALUES == 10 );
    STATIC_ASSERT( LEXEMS_SIZE == 128 );
    STATIC_ASSERT( MAX_ENEMY_STACK == 30 );
    STATIC_ASSERT( MAX_HOLO_INFO == 250 );
    STATIC_ASSERT( MAX_NPC_BAGS == 50 );
    STATIC_ASSERT( MAX_NPC_BAGS_PACKS == 20 );
    STATIC_ASSERT( MAX_PARAMS == 1000 );
    STATIC_ASSERT( MAX_SCRIPT_NAME == 64 );
    STATIC_ASSERT( MAX_STORED_IP == 20 );
    STATIC_ASSERT( MAX_STORED_LOCATIONS == 1000 );
    STATIC_ASSERT( SCORES_MAX == 50 );
    STATIC_ASSERT( SCORE_NAME_LEN == 64 );
    STATIC_ASSERT( USER_HOLO_MAX_LEN == 2000 );
    STATIC_ASSERT( USER_HOLO_MAX_TITLE_LEN == 40 );

    STATIC_ASSERT( sizeof(WorldSignature) == 6 );

    static const uint sizeof_str = sizeof(string);
    static const uint sizeof_ptr = sizeof(void*);
    static const uint sizeof_vec = sizeof(vector<void*>);

    STATIC_ASSERT( sizeof_vec == sizeof(vector<uint>) );
    STATIC_ASSERT( sizeof_vec == sizeof(vector<uint8>) );

    // saved
    STATIC_ASSERT( sizeof(WorldSave::Object::TimeV1) == 16 );
    STATIC_ASSERT( sizeof(WorldSave::Object::LocationDataV1) == 256 );
    STATIC_ASSERT( sizeof(WorldSave::Object::MapDataV1) == 524 );
    STATIC_ASSERT( sizeof(WorldSave::Object::CritterDataV1) == 7404 );
    STATIC_ASSERT( sizeof(WorldSave::Object::CritterBagV1) == 16 );
    STATIC_ASSERT( sizeof(WorldSave::Object::CritterDataExtV1) == 6944 );
    STATIC_ASSERT( sizeof(WorldSave::Object::CritterTimeEventV1) == 16 );
    STATIC_ASSERT( sizeof(WorldSave::Object::ItemDataV1) == 120 ); // used in NetProtocol.h (sic!)

    // constructed
    STATIC_ASSERT( sizeof(WorldSave::Object::ScoreV1) == 76 );
    STATIC_ASSERT( sizeof(WorldSave::Object::LocationV1) == 8 + sizeof_ptr + sizeof_vec );
    STATIC_ASSERT( sizeof(WorldSave::Object::MapV1) == 8 + sizeof_ptr );
    STATIC_ASSERT( sizeof(WorldSave::Object::CritterV1) == 8 + (sizeof_ptr * 2) + sizeof_vec );
    STATIC_ASSERT( sizeof(WorldSave::Object::ItemV1) == 20 + (sizeof_ptr * 2) );
    STATIC_ASSERT( sizeof(WorldSave::Object::VarV1) == 18 + /*padding*/ 2 );
    STATIC_ASSERT( sizeof(WorldSave::Object::HoloV1) == 13 + (sizeof_ptr * 2) + /*padding*/ 3 );
    STATIC_ASSERT( sizeof(WorldSave::Object::AnyDataV1) == 12 + sizeof_ptr + sizeof_vec );
    STATIC_ASSERT( sizeof(WorldSave::Object::TimeEventV1) == 22 + sizeof_ptr + sizeof_vec + /*padding*/ 2 );
    STATIC_ASSERT( sizeof(WorldSave::Object::ScriptFunctionV1) == 8 + sizeof_ptr );
}

WorldSave::Object::Signature::Signature() : OffsetEnd( 0 ), Version( 0 ), SizeBegin( 0 ), SizeEnd( 0 ), Legacy( false )
{}

bool WorldSave::Object::Signature::Load( void* world, string Name )
{
    FileSetPointer( world, 0, SEEK_SET );

    uint8 signature[sizeof(WorldSignature)];
    if( !FileRead( world, &signature, sizeof(signature) ) )
    {
        App.WriteLog( "Cannot read file<%s> signature\n", Name.c_str() );
        return false;
    }

    if( signature[1] == 0x0F && signature[2] == 0xAB && signature[3] == 0x01 )
    {
        if( signature[0] == 0x13 )
        {
            Legacy = true;
            Version = signature[0];

            SizeBegin = sizeof(uint);
            SizeEnd = sizeof(uint);
        }
        else
        {
            App.WriteLog( "Legacy version<0x%02X> not supported\n", signature[0] );
            return false;
        }
    }
    else if( BINARY_SIGNATURE_VALID( signature, WorldSignature ) )
    {
        Version = BINARY_SIGNATURE_VERSION( signature );

        SizeBegin = sizeof(WorldSignature);
        SizeEnd = sizeof(uint16);
    }
    else
    {
        App.WriteLog( "World file<%s> signature invalid\n", Name.c_str() );
        return false;
    }

    if( FileGetSize( world ) < (uint)(SizeBegin + SizeEnd) )
    {
        App.WriteLog( "World file<%s> corrupted : expected at least<%ub>, found<%ub>\n", Name.c_str(), SizeBegin + SizeEnd, FileGetSize( world ) );
        return false;
    }

    if( SizeEnd )
    {
        uint16 version_end = 0;

        FileSetPointer( world, -SizeEnd, SEEK_END );
        OffsetEnd = FileGetPointer( world );

        if( Legacy )
        {
            uint8 signature_end[4];
            if( !FileRead( world, &signature_end, sizeof(signature_end) ) )
            {
                App.WriteLog( "WorldSave file<%s> corrupted : cannot read signature_end\n", Name.c_str() );
                return false;
            }

            if( signature_end[1] == 0x0F && signature_end[2] == 0xAB && signature_end[3] == 0x01 )
                version_end = signature_end[0];
            else
            {
                App.WriteLog( "WorldSave file<%s> corrupted : signature_end mismatch\n", Name.c_str() );
                return false;
            }
        }
        else if( !FileRead( world, &version_end, sizeof(version_end) ) )
        {
            App.WriteLog( "WorldSave file<%s> corrupted : cannot read version_end\n", Name.c_str() );
            return false;
        }

        if( Version != version_end )
        {
            App.WriteLog( "WorldSave file<%s> corrupted : version mismatch\n", Name.c_str() );
            return false;
        }
    }

    FileSetPointer( world, SizeBegin, SEEK_SET );

    return true;
}

WorldSave::Object::SinglePlayerV1::SinglePlayerV1() : Version( 0 ), Data( nullptr ), DataExt( nullptr ), TimeEventsCount( 0 ), PicSize( 0 )
{
    memzero( Name, sizeof(Name) );
    memzero( PasswordHash, sizeof(PasswordHash) );
}

WorldSave::Object::SinglePlayerV1::~SinglePlayerV1()
{
    if( Data )
    {
        delete Data;
        Data = nullptr;
    }

    if( DataExt )
    {
        delete DataExt;
        DataExt = nullptr;
    }

    DestroyPtrVec( TimeEvents );

    PicData.clear();
}

WorldSave::Object::ScoreV1::ScoreV1( uint index /* = 0 */ ) : Index( index ), ClientId( 0 ), Value( 0 )
{
    memzero( ClientName, sizeof(ClientName) );
}

WorldSave::Object::LocationV1::LocationV1( uint index /* = 0 */ ) : Index( index ), Data( nullptr ), MapsCount( 0 )
{}

WorldSave::Object::LocationV1::~LocationV1()
{
    if( Data )
    {
        delete Data;
        Data = nullptr;
    }

    DestroyPtrVec( Maps );
}

WorldSave::Object::MapV1::MapV1( uint location_index /* = 0 */, uint map_index /* = 0 */ ) : LocationIndex( location_index ), MapIndex( map_index ), Data( nullptr )
{}

WorldSave::Object::MapV1::~MapV1()
{
    if( Data )
    {
        delete Data;
        Data = nullptr;
    }
}

WorldSave::Object::CritterV1::CritterV1( uint index /* = 0 */ ) : Index( index ), Data( nullptr ), DataExt( nullptr ), TimeEventsCount( 0 )
{}

WorldSave::Object::CritterV1::~CritterV1()
{
    if( Data )
    {
        delete Data;
        Data = nullptr;
    }

    if( DataExt )
    {
        delete DataExt;
        DataExt = nullptr;
    }

    DestroyPtrVec( TimeEvents );

    TimeEvents.clear();
}

WorldSave::Object::ItemV1::ItemV1( uint index /* = 0 */ ) : Index( index ), Data( nullptr ), Lexems( nullptr ), Id( 0 ), Pid( 0 ), LexemsLength( 0 ), Accessory( 0 )
{
    memzero( AccBuffer, sizeof(AccBuffer) );
}

WorldSave::Object::ItemV1::~ItemV1()
{
    if( Data )
    {
        delete Data;
        Data = nullptr;
    }

    if( Lexems )
        delete[] Lexems;
}

WorldSave::Object::VarV1::VarV1( uint index /* = 0 */ ) : Index( index ), TempId( 0 ), MasterId( 0 ), SlaveId( 0 ), Value( 0 )
{}

WorldSave::Object::HoloV1::HoloV1( uint index /* = 0 */ ) : Index( index ), Id( 0 ), Title( nullptr ), Text( nullptr ), TitleLength( 0 ), TextLength( 0 ), CanRW( false )
{}

WorldSave::Object::AnyDataV1::AnyDataV1( uint index /* = 0 */ ) : Index( index ), NameLength( 0 ), Name( nullptr ), DataLength( 0 )
{}

WorldSave::Object::AnyDataV1::~AnyDataV1()
{
    Data.clear();
}

WorldSave::Object::TimeEventV1::TimeEventV1( uint index /* = 0 */ ) : Index( index ), Num( 0 ), ScriptNameLength( 0 ), ScriptName( nullptr ), BeginSecond( 0 ), Rate( 0 ), ValuesLength( 0 )
{}

WorldSave::Object::ScriptFunctionV1::ScriptFunctionV1( uint index /* = 0 */ ) : Index( index ), NameLength( 0 ), Name( nullptr )
{}

WorldSave::WorldSave( WorldSave::Object::Signature signature, void* world, string filename /* = string() */ ) : Signature( signature ), World( world ), LogLevel( 1 ), FileName( filename )
{
    memzero( ReadBuffer, sizeof(ReadBuffer) );

    FileSetPointer( world, Signature.SizeBegin, SEEK_SET );
}

bool WorldSave::ReadData( void* buf, uint len, const string& name0, const string& name1 /* = string() */, const uint& index0 /* = MAX_UINT */, const uint& index1 /* = MAX_UINT */ )
{
    if( !FileRead( World, buf, len ) )
    {
        LogLoadError( "%s : length<%u>", GetDataName( name0, name1, index0, index1 ).c_str(), len );
        return false;
    }

    return true;
}

bool WorldSave::ReadCounter( uint& counter, const string& name )
{
    if( !ReadData( &counter, sizeof(counter), name ) )
        return false;

    RunCounterLoaded( counter, name );

    return true;
}

void WorldSave::RunCounterLoaded( uint& count, const string& name )
{
    for( auto it = OnCounterLoaded.begin(), end = OnCounterLoaded.end(); it != end; ++it )
    {
        auto& func = *it;
        func( count, name );
    }
}

void WorldSave::RunObjectLoaded( void*& object, const string& name, const uint& version )
{
    for( auto it = OnObjectLoaded.begin(), end = OnObjectLoaded.end(); it != end; ++it )
    {
        auto& func = *it;
        func( object, name, version );
    }
}

void WorldSave::RunGroupLoaded( vector<void*>& group, const string& name, const uint& version )
{
    for( auto it = OnGroupLoaded.begin(), end = OnGroupLoaded.end(); it != end; ++it )
    {
        auto& func = *it;
        func( group, name, version );
    }
}

string WorldSave::GetDataName( const string& name0, const string& name1 = string(), const uint& index0 = MAX_UINT, const uint& index1 = MAX_UINT /* = MAX_UINT */ )
{
    string data = name0;

    if( index0 != MAX_UINT )
    {
        data += string( "[" ) + to_string( (long long)index0 );

        if( index1 != MAX_UINT )
            data += string( "," ) + to_string( (long long)index1 );

        data += "]";
    }

    if( !name1.empty() )
        data += string( "::" ) + name1;

    return data;
}

void WorldSave::LogLoad( const char* frmt, ... )
{
    if( !LogLevel )
        return;

    const char* filename = Str::FormatBuf( "%s%s%s", !FileName.empty() ? "<" : "", !FileName.empty() ? FileName.c_str() : "", !FileName.empty() ? ">" : "" );

    va_list     list;
    va_start( list, frmt );
    char        str[MAX_LOGTEXT] = { 0 };
    size_t      len = Str::Length( str );
    vsnprintf( &str[len], MAX_LOGTEXT - len, frmt, list );
    App.WriteLog( "Load world%s... %s\n", filename, str );

    va_end( list );
}

void WorldSave::LogLoadWarning( const char* frmt, ... )
{
    const char* filename = Str::FormatBuf( "%s%s%s", !FileName.empty() ? "<" : "", !FileName.empty() ? FileName.c_str() : "", !FileName.empty() ? ">" : "" );

    va_list     list;
    va_start( list, frmt );
    char        str[MAX_LOGTEXT] = { 0 };
    size_t      len = Str::Length( str );

    vsnprintf( &str[len], MAX_LOGTEXT - len, frmt, list );
    App.WriteLog( "Load world%s... WARNING : %s\n", filename, str );

    va_end( list );
}

void WorldSave::LogLoadError( const char* frmt, ... )
{
    const char* filename = Str::FormatBuf( "%s%s%s", !FileName.empty() ? "<" : "", !FileName.empty() ? FileName.c_str() : "", !FileName.empty() ? ">" : "" );

    va_list     list;
    va_start( list, frmt );
    char        str[MAX_LOGTEXT] = { 0 };
    size_t      len = Str::Length( str );

    vsnprintf( &str[len], MAX_LOGTEXT - len, frmt, list );
    App.WriteLog( "Load world%s... ERROR : cannot load data<%s>\n", filename, str );

    va_end( list );
}

WorldSave::Object::Signature WorldSave::GetSignature()
{
    Object::Signature signature = Signature;

    return signature;
}

bool WorldSave::LoadSinglePlayer( WorldSave::Object::SinglePlayerV1*& singleplayer )
{
    static const uint version = 1;

    if( singleplayer )
    {
        LogLoadError( "%s> reason<object not null", ID::SinglePlayer.c_str() );
        return false;
    }

    Object::SinglePlayerV1* tmp = new Object::SinglePlayerV1();

    if( !ReadData( &tmp->Version, sizeof(tmp->Version), ID::SinglePlayer, "Version" ) )
    {
        delete tmp;
        return false;
    }

    if( !tmp->Version )
    {
        RunObjectLoaded( reinterpret_cast<void*&>(tmp), ID::SinglePlayer, version );
        if( tmp )
        {
            delete tmp;
            tmp = nullptr;
        }
        return true;
    }

    if( !ReadData( tmp->Name, sizeof(tmp->Name), ID::SinglePlayer, "Name" ) )
    {
        delete tmp;
        return false;
    }

    tmp->Data = new Object::CritterDataV1();
    if( !ReadData( tmp->Data, sizeof(Object::CritterDataV1), ID::SinglePlayer, "Data" ) )
    {
        delete tmp;
        return false;
    }

    tmp->DataExt = new Object::CritterDataExtV1();
    if( !ReadData( tmp->DataExt, sizeof(Object::CritterDataExtV1), ID::SinglePlayer, "DataExt" ) )
    {
        delete tmp;
        return false;
    }

    if( !ReadData( &tmp->TimeEventsCount, sizeof(tmp->TimeEventsCount), ID::SinglePlayer, "TimeEventsCount" ) )
    {
        delete tmp;
        return false;
    }

    for( uint timevent_idx = 0; timevent_idx < tmp->TimeEventsCount; timevent_idx++ )
    {
        Object::CritterTimeEventV1* timevent = new Object::CritterTimeEventV1();

        if( !ReadData( timevent, sizeof(Object::CritterTimeEventV1), ID::SinglePlayer, "TimeEvent", timevent_idx ) )
        {
            delete timevent;
            delete tmp;
            return false;
        }

        tmp->TimeEvents.push_back( timevent );
    }

    if( !ReadData( &tmp->PicSize, sizeof(tmp->PicSize), ID::SinglePlayer, "PicSize" ) )
    {
        delete tmp;
        return false;
    }

    if( tmp->PicSize )
    {
        tmp->PicData.resize( tmp->PicSize );
        if( !ReadData( &tmp->PicData[0], tmp->PicSize, ID::SinglePlayer, "PicData" ) )
        {
            delete tmp;
            return false;
        }
    }

    RunObjectLoaded( reinterpret_cast<void*&>(tmp), ID::SinglePlayer, version );
    if( tmp )
        singleplayer = tmp;

    LogLoad( "singleplayer" );

    return true;
}

bool WorldSave::LoadTime( WorldSave::Object::TimeV1*& time )
{
    static const uint version = 1;

    if( time )
    {
        LogLoadError( "%s> reason<object not null", ID::Time.c_str() );
        return false;
    }

    Object::TimeV1* tmp = new Object::TimeV1();

    if( !ReadData( tmp, sizeof(Object::TimeV1), ID::Time ) )
        return false;

    const char* load = Str::FormatBuf( "date<%u.%u.%u> time<%02u:%02u:%02u> multiplier<x%u>", tmp->Day, tmp->Month, tmp->Year, tmp->Hour, tmp->Minute, tmp->Second, tmp->Multiplier );

    RunObjectLoaded( reinterpret_cast<void*&>(tmp), ID::Time, version );
    if( tmp )
        time = tmp;

    LogLoad( load );

    return true;
}

bool WorldSave::LoadScores( vector<Object::ScoreV1*>& scores )
{
    static const uint version = 1;

    for( uint idx = 0; idx < SCORES_MAX; idx++ )
    {
        Object::ScoreV1* score = new Object::ScoreV1( idx );

        if( !ReadData( &score->ClientId, sizeof(score->ClientId), ID::Score, "ClientId", score->Index ) )
        {
            delete score;
            return false;
        }

        if( !ReadData( score->ClientName, sizeof(score->ClientName), ID::Score, "ClientName", score->Index ) )
        {
            delete score;
            return false;
        }

        if( !ReadData( &score->Value, sizeof(score->Value), ID::Score, "Value", score->Index ) )
        {
            delete score;
            return false;
        }

        RunObjectLoaded( reinterpret_cast<void*&>(score), ID::Score, version );
        if( score )
            scores.push_back( score );
    }

    LogLoad( "scores<%u>", SCORES_MAX );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(scores), ID::Score, version );

    return true;
}

bool WorldSave::LoadLocations( vector<Object::LocationV1*>& locations )
{
    static const uint version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::LocationCounter ) )
        return false;

    uint all_maps = 0;

    for( uint loc_idx = 0; loc_idx < count; loc_idx++ )
    {
        Object::LocationV1* location = new Object::LocationV1( loc_idx );

        location->Data = new Object::LocationDataV1();
        if( !ReadData( location->Data, sizeof(Object::LocationDataV1), ID::Location, "Data", location->Index ) )
        {
            delete location;
            return false;
        }

        if( !ReadData( &location->MapsCount, sizeof(location->MapsCount), ID::Location, "MapsCount", location->Index ) )
        {
            delete location;
            return false;
        }

        for( uint map_idx = 0; map_idx < location->MapsCount; map_idx++ )
        {
            Object::MapV1* map = new Object::MapV1( location->Index, map_idx );

            map->Data = new Object::MapDataV1();
            if( !ReadData( map->Data, sizeof(Object::MapDataV1), ID::Map, "Data", map->LocationIndex, map->MapIndex ) )
            {
                delete map;
                delete location;
                return false;
            }

            location->Maps.push_back( map );
            all_maps++;
        }

        RunObjectLoaded( reinterpret_cast<void*&>(location), ID::Location, version );
        if( location )
            locations.push_back( location );
    }

    LogLoad( "locations<%u> maps<%u>", count, all_maps );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(locations), ID::Location, version );

    return true;
}

bool WorldSave::LoadCritters( vector<Object::CritterV1*>& critters )
{
    static const uint version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::CritterCounter ) )
        return false;

    uint all_timevents = 0;

    for( uint idx = 0; idx < count; idx++ )
    {
        Object::CritterV1* critter = new Object::CritterV1( idx );

        critter->Data = new Object::CritterDataV1();
        if( !ReadData( critter->Data, sizeof(Object::CritterDataV1), ID::Critter, "Data", critter->Index ) )
        {
            delete critter;
            return false;
        }

        if( critter->Data->IsDataExt )
        {
            critter->DataExt = new Object::CritterDataExtV1();
            if( !ReadData( critter->DataExt, sizeof(Object::CritterDataExtV1), ID::Critter, "DataExt", critter->Index ) )
            {
                delete critter;
                return false;
            }
        }

        if( !ReadData( &critter->TimeEventsCount, sizeof(critter->TimeEventsCount), ID::Critter, "TimeEventsCount", critter->Index ) )
        {
            delete critter;
            return false;
        }

        for( uint timevent_idx = 0; timevent_idx < critter->TimeEventsCount; timevent_idx++ )
        {
            Object::CritterTimeEventV1* timevent = new Object::CritterTimeEventV1();

            if( !ReadData( timevent, sizeof(Object::CritterTimeEventV1), ID::CritterTimeEvent, string(), critter->Index, timevent_idx ) )
            {
                delete timevent;
                delete critter;
                return false;
            }

            critter->TimeEvents.push_back( timevent );
            all_timevents++;
        }

        RunObjectLoaded( reinterpret_cast<void*&>(critter), ID::Critter, version );
        if( critter )
            critters.push_back( critter );
    }

    LogLoad( "critters<%u> time events<%u>", count, all_timevents );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(critters), ID::Critter, version );

    return true;
}

bool WorldSave::LoadItems( vector<Object::ItemV1*>& items )
{
    static const uint version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::ItemCounter ) )
        return false;

    uint all_lexems = 0;

    for( uint idx = 0; idx < count; idx++ )
    {
        Object::ItemV1* item = new Object::ItemV1( idx );

        if( !ReadData( &item->Id, sizeof(item->Id), ID::Item, "Id", item->Index ) )
        {
            delete item;
            return false;
        }

        if( !ReadData( &item->Pid, sizeof(item->Pid), ID::Item, "Pid", item->Index ) )
        {
            delete item;
            return false;
        }

        if( !ReadData( &item->Accessory, sizeof(item->Accessory), ID::Item, "Accessory", item->Index ) )
        {
            delete item;
            return false;
        }

        if( !ReadData( &item->AccBuffer[0], sizeof(item->AccBuffer), ID::Item, "AccBuffer", item->Index ) )
        {
            delete item;
            return false;
        }

        item->Data = new Object::ItemDataV1();
        if( !ReadData( item->Data, sizeof(Object::ItemDataV1), ID::Item, "Data", item->Index ) )
        {
            delete item;
            return false;
        }

        if( !ReadData( &item->LexemsLength, sizeof(item->LexemsLength), ID::Item, "LexemsLength", item->Index ) )
        {
            delete item;
            return false;
        }

        if( item->LexemsLength >= LEXEMS_SIZE )        // NOTE: allowed in r412 (stripped on load)
        {
            LogLoadError( "%s : invalid size<%u>", GetDataName( ID::Item, "LexemsLength", item->Index ).c_str(), item->LexemsLength );
            delete item;
            return false;
        }

        if( item->LexemsLength )
        {
            memzero( ReadBuffer, sizeof(ReadBuffer) );
            if( !ReadData( ReadBuffer, item->LexemsLength, ID::Item, "Lexems", item->Index ) )
            {
                delete item;
                return false;
            }

            ReadBuffer[item->LexemsLength] = 0;
            item->Lexems = Str::Duplicate( ReadBuffer );

            if( !item->Lexems )
            {
                LogLoadError( "%s : duplicate fail", GetDataName( ID::Item, "Lexems", item->Index ) );
                delete item;
                return false;
            }

            if( !Str::CompareCase( ReadBuffer, item->Lexems ) )
            {
                LogLoadError( "%s : \"%s\" != \"%s\"", GetDataName( ID::Item, "Lexems", item->Index ).c_str(), ReadBuffer, item->Lexems );
                delete item;
                return false;
            }

            all_lexems++;
        }

        RunObjectLoaded( reinterpret_cast<void*&>(item), ID::Item, version );
        if( item )
            items.push_back( item );
    }

    LogLoad( "items<%u> lexems<%u>", count, all_lexems );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(items), ID::Item, version );

    return true;
}

bool WorldSave::LoadVars( vector<Object::VarV1*>& vars )
{
    static const uint version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::VarCounter ) )
        return false;

    for( uint idx = 0; idx < count; idx++ )
    {
        Object::VarV1* var = new Object::VarV1( idx );

        if( !ReadData( &var->TempId, sizeof(var->TempId), ID::Var, "TempId", var->Index ) )
        {
            delete var;
            return false;
        }

        if( !ReadData( &var->MasterId, sizeof(var->MasterId), ID::Var, "MasterId", var->Index ) )
        {
            delete var;
            return false;
        }

        if( !ReadData( &var->SlaveId, sizeof(var->SlaveId), ID::Var, "SlaveId", var->Index ) )
        {
            delete var;
            return false;
        }

        if( !ReadData( &var->Value, sizeof(var->Value), ID::Var, "Value", var->Index ) )
        {
            delete var;
            return false;
        }

        RunObjectLoaded( reinterpret_cast<void*&>(var), ID::Var, version );
        if( var )
            vars.push_back( var );
    }

    LogLoad( "vars<%u>", count );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(vars), ID::Var, version );

    return true;
}

bool WorldSave::LoadHolo( vector<Object::HoloV1*>& holos )
{
    static const uint version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::HoloCounter ) )
        return false;

    for( uint idx = 0; idx < count; idx++ )
    {
        Object::HoloV1* holo = new Object::HoloV1( idx );

        if( !ReadData( &holo->Id, sizeof(holo->Id), ID::Holo, "Id", holo->Index ) )
        {
            delete holo;
            return false;
        }

        if( !ReadData( &holo->CanRW, sizeof(holo->CanRW), ID::Holo, "CanRW", holo->Index ) )
        {
            delete holo;
            return false;
        }

        if( !ReadData( &holo->TitleLength, sizeof(holo->TitleLength), ID::Holo, "TitleLength", holo->Index ) )
        {
            delete holo;
            return false;
        }

        if( holo->TitleLength > USER_HOLO_MAX_TITLE_LEN )
        {
            LogLoadWarning( "%s : length<%u> -> <%u>", GetDataName( ID::Holo, "TitleLength", holo->Index ).c_str(), holo->TitleLength, USER_HOLO_MAX_TITLE_LEN );
            holo->TitleLength = USER_HOLO_MAX_TITLE_LEN;
        }

        memzero( ReadBuffer, sizeof(ReadBuffer) );
        if( !ReadData( ReadBuffer, holo->TitleLength, ID::Holo, "Title", holo->Index ) )
        {
            delete holo;
            return false;
        }

        ReadBuffer[holo->TitleLength] = 0;
        holo->Title = Str::Duplicate( ReadBuffer );

        if( !ReadData( &holo->TextLength, sizeof(holo->TextLength), ID::Holo, "TextLength", holo->Index ) )
        {
            delete holo;
            return false;
        }

        if( holo->TextLength > USER_HOLO_MAX_LEN )
        {
            LogLoadWarning( "%s : length<%u> -> <%u>", GetDataName( ID::Holo, "TextLength", holo->Index ).c_str(), holo->TextLength, USER_HOLO_MAX_LEN );
            holo->TextLength = USER_HOLO_MAX_LEN;
        }

        memzero( ReadBuffer, sizeof(ReadBuffer) );
        if( !ReadData( ReadBuffer, holo->TextLength, ID::Holo, "Text", holo->Index ) )
        {
            delete holo;
            return false;
        }

        ReadBuffer[holo->TextLength] = 0;
        holo->Text = Str::Duplicate( ReadBuffer );

        RunObjectLoaded( reinterpret_cast<void*&>(holo), ID::Holo, version );
        if( holo )
            holos.push_back( holo );
    }

    LogLoad( "holos<%u>", count );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(holos), ID::Holo, version );

    return true;
}

bool WorldSave::LoadAnyData( vector<Object::AnyDataV1*>& anydata )
{
    static const uint version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::AnyDataCounter ) )
        return false;

    for( uint idx = 0; idx < count; idx++ )
    {
        Object::AnyDataV1* any = new Object::AnyDataV1( idx );

        if( !ReadData( &any->NameLength, sizeof(any->NameLength), ID::AnyData, "NameLength", any->Index ) )
        {
            delete any;
            return false;
        }

        memzero( ReadBuffer, sizeof(ReadBuffer) );
        if( !ReadData( ReadBuffer, any->NameLength, ID::AnyData, "Name", any->Index ) )
        {
            delete any;
            return false;
        }

        ReadBuffer[any->NameLength] = 0;
        any->Name = Str::Duplicate( ReadBuffer );

        if( !ReadData( &any->DataLength, sizeof(any->DataLength), ID::AnyData, "DataLength", any->Index ) )
        {
            delete any;
            return false;
        }

        if( any->DataLength )
        {
            any->Data.resize( any->DataLength );
            if( !ReadData( &any->Data[0], any->DataLength, ID::AnyData, "Data", any->Index ) )
            {
                delete any;
                return false;
            }
        }

        RunObjectLoaded( reinterpret_cast<void*&>(any), ID::AnyData, version );
        if( any )
            anydata.push_back( any );
    }

    LogLoad( "anydata<%u>", count );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(anydata), ID::AnyData, version );

    return true;
}

bool WorldSave::LoadTimeEvents( vector<Object::TimeEventV1*>& events )
{
    static const uint _Version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::TimeEventCounter ) )
        return false;

    for( uint idx = 0; idx < count; idx++ )
    {
        Object::TimeEventV1* timevent = new Object::TimeEventV1( idx );

        if( !ReadData( &timevent->Num, sizeof(timevent->Num), ID::TimeEvent, "Num", timevent->Index ) )
        {
            delete timevent;
            return false;
        }

        if( !ReadData( &timevent->ScriptNameLength, sizeof(timevent->ScriptNameLength), ID::TimeEvent, "ScriptNameLength", timevent->Index ) )
        {
            delete timevent;
            return false;
        }

        memzero( ReadBuffer, sizeof(ReadBuffer) );
        if( !ReadData( ReadBuffer, timevent->ScriptNameLength, ID::TimeEvent, "ScriptName", timevent->Index ) )
        {
            delete timevent;
            return false;
        }

        ReadBuffer[timevent->ScriptNameLength] = 0;
        timevent->ScriptName = Str::Duplicate( ReadBuffer );

        if( !ReadData( &timevent->BeginSecond, sizeof(timevent->BeginSecond), ID::TimeEvent, "BeginSecond", timevent->Index ) )
        {
            delete timevent;
            return false;
        }

        if( !ReadData( &timevent->Rate, sizeof(timevent->Rate), ID::TimeEvent, "Rate", timevent->Index ) )
        {
            delete timevent;
            return false;
        }

        if( !ReadData( &timevent->ValuesLength, sizeof(timevent->ValuesLength), ID::TimeEvent, "ValuesLength", timevent->Index ) )
        {
            delete timevent;
            return false;
        }

        if( timevent->ValuesLength )
        {
            timevent->Values.resize( timevent->ValuesLength );
            if( !ReadData( &timevent->Values[0], sizeof(uint) * timevent->ValuesLength, ID::TimeEvent, "Values", timevent->Index ) )
            {
                delete timevent;
                return false;
            }
        }

        RunObjectLoaded( reinterpret_cast<void*&>(timevent), ID::TimeEvent, _Version );
        if( timevent )
            events.push_back( timevent );
    }

    LogLoad( "time events<%u>", count );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(events), ID::TimeEvent, _Version );

    return true;
}

bool WorldSave::LoadScriptFunctions( vector<Object::ScriptFunctionV1*>& functions )
{
    static const uint version = 1;
    uint              count = 0;

    if( !ReadCounter( count, ID::ScriptFunctionCounter ) )
        return false;

    for( uint idx = 0; idx < count; idx++ )
    {
        Object::ScriptFunctionV1* func = new Object::ScriptFunctionV1( idx );

        if( !ReadData( &func->NameLength, sizeof(func->NameLength), ID::ScriptFunction, "NameLength", func->Index ) )
        {
            delete func;
            return false;
        }

        if( func->NameLength )
        {
            memzero( ReadBuffer, sizeof(ReadBuffer) );
            if( !ReadData( ReadBuffer, func->NameLength, ID::ScriptFunction, "Name", func->Index ) )
            {
                delete func;
                return false;
            }

            ReadBuffer[func->NameLength] = 0;
            func->Name = Str::Duplicate( ReadBuffer );
        }

        RunObjectLoaded( reinterpret_cast<void*&>(func), ID::ScriptFunction, version );
        if( func )
            functions.push_back( func );
    }

    LogLoad( "script functions<%u>", count );
    RunGroupLoaded( reinterpret_cast<vector<void*>&>(functions), ID::ScriptFunction, version );

    return true;
}

WorldSave::V1::V1( WorldSave::Object::Signature signature, void* world, string filename /* = string() */ ) :
    WorldSave( signature, world, filename ),
    SinglePlayer( nullptr ), Time( nullptr )
{}

WorldSave::V1::~V1()
{
    UnloadWorld();
}

bool WorldSave::V1::LoadWorld()
{
    LogLoad( "" );

    if( !LoadSinglePlayer( SinglePlayer ) )
        return false;

    if( !LoadTime( Time ) )
        return false;

    if( !LoadScores( Scores ) )
        return false;

    if( !LoadLocations( Locations ) )
        return false;

    if( !LoadCritters( Critters ) )
        return false;

    if( !LoadItems( Items ) )
        return false;

    if( !LoadVars( Vars ) )
        return false;

    if( !LoadHolo( Holos ) )
        return false;

    if( !LoadAnyData( AnyData ) )
        return false;

    if( !LoadTimeEvents( TimeEvents ) )
        return false;

    if( !LoadScriptFunctions( ScriptFunctions ) )
        return false;

    LogLoad( "OK" );

    return true;
}

void WorldSave::V1::UnloadWorld()
{
    UnloadSinglePlayer();
    UnloadTime();
    UnloadScores();
    UnloadLocations();
    UnloadCritters();
    UnloadItems();
    UnloadVars();
    UnloadHolos();
    UnloadAnyData();
    UnloadTimeEvents();
    UnloadScriptFunctions();
}

void WorldSave::V1::UnloadSinglePlayer()
{
    if( SinglePlayer )
    {
        delete SinglePlayer;
        SinglePlayer = nullptr;
    }
}

void WorldSave::V1::UnloadTime()
{
    if( Time )
    {
        delete Time;
        Time = nullptr;
    }
}

void WorldSave::V1::UnloadScores()
{
    DestroyPtrVec( Scores );
}

void WorldSave::V1::UnloadLocations()
{
    DestroyPtrVec( Locations );
}

void WorldSave::V1::UnloadCritters()
{
    DestroyPtrVec( Critters );
}

void WorldSave::V1::UnloadItems()
{
    DestroyPtrVec( Items );
}

void WorldSave::V1::UnloadVars()
{
    DestroyPtrVec( Vars );
}

void WorldSave::V1::UnloadHolos()
{
    DestroyPtrVec( Holos );
}

void WorldSave::V1::UnloadAnyData()
{
    DestroyPtrVec( AnyData );
}

void WorldSave::V1::UnloadTimeEvents()
{
    DestroyPtrVec( TimeEvents );
}

void WorldSave::V1::UnloadScriptFunctions()
{
    DestroyPtrVec( ScriptFunctions );
}
