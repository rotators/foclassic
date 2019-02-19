#include "Core.h"

#include "App.h"

#include "CommandLine.h"
#include "FileSystem.h"
#include "ShowWorld.h"
#include "Text.h"
#include "WorldSaveDump.h"
#include "WorldSaveDump.h"

ShowWorld::ShowWorld()
{}

string ShowWorld::UsageArguments()
{
    return "[worldsave.fo]";
}
string ShowWorld::UsageDescription()
{
    return "???";
}

int ShowWorld::Run()
{
    string name = CommandLine->GetArg( 1 );
    string name_short = name;

    if( name.empty() )
    {
        App.WriteLog( "ERROR: Missing filename\n" );
        return Stop();
    }
    else
    {
        replace( name_short.begin(), name_short.end(), '\\', '/' );
        size_t pos = name_short.find_last_of( '/' );
        if( pos != string::npos )
            name_short = name_short.substr( pos + 1 );
    }

    if( !FileExist( name.c_str() ) )
    {
        App.WriteLog( "ERROR: file<%s> does not exists\n", name.c_str() );
        return Stop();
    }

    void* file = FileOpen( name.c_str(), false );
    if( !file )
    {
        App.WriteLog( "ERROR: file<%s> cannot be opened\n", name.c_str() );
        return Stop();
    }

    WorldSave::Object::Signature signature;
    if( !signature.LoadSignature( file, name ) )
        return Stop();

    WorldSave* world = WorldSaveDump::NewWorld( signature, file, name_short );

    if( !world )
    {
        App.WriteLog( "ERROR: unknown version<%u>\n", signature.Version );
        return Stop();
    }

    bool success = world->LoadWorld();

    return Stop( success ? EXIT_SUCCESS : EXIT_FAILURE );
}

int ShowWorld::Stop( int code /* = EXIT_FAILURE */ )
{
    return code;
}

// Old system, left for reference
/*
   template<class T>
   void InitVectorDump(WorldDump& dump, const char* name, const uint& offset, const uint& count, const T& vec)
   {
        dump[offset] = Str::FormatBuf("Count::%s:%u", name, count);

        if (count != vec.size())
                App.WriteLog("Dump world... WARNING Count::%s != %s::size() : %u != %u\n", name, name, count, vec.size());
   }

   void WorldSaveDump::DumpSignature(WorldDump& dump, const WorldSave::SignatureData& signature )
   {
        dump[0] = Str::FormatBuf("Signature Version:%u Legacy:%s Size:%u", signature.Version, signature.Legacy ? "true" : "false", signature.SizeBegin);

        if (signature.OffsetEnd)
        {
                if (!signature.Legacy)
                        dump[signature.OffsetEnd] = Str::FormatBuf("Version %u", signature.Version);
                else
                        dump[signature.OffsetEnd] = Str::FormatBuf("Signature Version:%u Legacy:true Size:%u", signature.Version, signature.SizeEnd);
        }
   }

   void WorldSaveDump::DumpWorld(WorldDump & dump, const WorldSave* world)
   {
        if (!world)
                return;

        DumpSignature(dump, world->Signature);

        uint16 version = world->Signature.Version;
        if (world->Signature.Legacy && world->Signature.Version == 0x13)
                version = 1;

        switch (version)
        {
                case 1:
                {
                        DumpWorld(dump, *(WorldSave::V1*)world );
                        break;
                }
                default:
                        break;
        }
   }

   void WorldSaveDump::DumpWorld(WorldDump& dump, const WorldSave::V1& world)
   {
        /*
        if (world.Offset.SinglePlayer)
        {
                dump[world.Offset.SinglePlayer] = Str::FormatBuf("SinglePlayer %s", world.SinglePlayer.Marker ? "true" : "false");
        }

        if (world.Offset.Time)
                DumpTime(dump, world.Offset.Time, world.Time);

        if (world.Offset.Scores)
        {
                const uint count = sizeof(world.Scores) / sizeof(world.Scores[0]);

                for (uint8 idx = 0; idx < count; idx++)
                {
                        uint offsetScore = world.Offset.Scores + (sizeof(WorldSave::ScoreV1) * idx);
                        DumpScore(dump, offsetScore, idx, world.Scores[idx]);
                }
        }

        if (world.Offset.LocationsCount)
        {
                InitVectorDump(dump, "Locations", world.Offset.LocationsCount, world.Count.Locations, world.Locations);
                for (auto it = world.Locations.begin(), end=world.Locations.end();it!=end;++it)
                {
                        DumpLocation(dump, *it);
                }
        }

        if (world.Offset.CrittersCount)
        {
                InitVectorDump(dump, "Critters", world.Offset.CrittersCount, world.Count.Critters, world.Critters);
                for (auto it = world.Critters.begin(), end = world.Critters.end(); it != end; ++it)
                {
                        DumpCritter(dump, *it);
                }
        }

        if (world.Offset.ItemsCount)
        {
                InitVectorDump(dump, "Items", world.Offset.ItemsCount, world.Count.Items, world.Items);
        }

        if (world.Offset.VarsCount)
        {
                InitVectorDump(dump, "Vars", world.Offset.VarsCount, world.Count.Vars, world.Vars);
        }

        if (world.Offset.HolosCount)
        {
                InitVectorDump(dump, "Holos", world.Offset.HolosCount, world.Count.Holos, world.Holos);
        }

        if (world.Offset.AnyDataCount)
        {
                InitVectorDump(dump, "AnyData", world.Offset.AnyDataCount, world.Count.AnyData, world.AnyData);
        }
   }

   void WorldSaveDump::DumpTime(WorldDump & dump, const uint& offsetTime, const WorldSave::TimeV1 & time)
   {
        if (!offsetTime)
                return;

        dump[offsetTime + offsetof(WorldSave::TimeV1, YearStart)] = Str::FormatBuf("YearStart %u", time.YearStart);
        dump[offsetTime + offsetof(WorldSave::TimeV1, Year)] = Str::FormatBuf("Year %u", time.Year);
        dump[offsetTime + offsetof(WorldSave::TimeV1, Month)] = Str::FormatBuf("Month %u", time.Month);
        dump[offsetTime + offsetof(WorldSave::TimeV1, Day)] = Str::FormatBuf("Day %u", time.Day);
        dump[offsetTime + offsetof(WorldSave::TimeV1, Hour)] = Str::FormatBuf("Hour %u", time.Hour);
        dump[offsetTime + offsetof(WorldSave::TimeV1, Minute)] = Str::FormatBuf("Minute %u", time.Minute);
        dump[offsetTime + offsetof(WorldSave::TimeV1, Second)] = Str::FormatBuf("Second %u", time.Second);
        dump[offsetTime + offsetof(WorldSave::TimeV1, Multiplier)] = Str::FormatBuf("Multiplier %u", time.Multiplier);
   }

   void WorldSaveDump::DumpScore(WorldDump& dump, const uint& offsetScore, const uint& index, const WorldSave::ScoreV1& score)
   {
        if (!offsetScore)
                return;

        dump[offsetScore + offsetof(WorldSave::ScoreV1, ClientId)] = Str::FormatBuf("Score[%u] ClientId:%u", index, score.ClientId);
        dump[offsetScore + offsetof(WorldSave::ScoreV1, ClientName)] = Str::FormatBuf("Score[%u] ClientName:%s", index, score.ClientName);
        dump[offsetScore + offsetof(WorldSave::ScoreV1, Value)] = Str::FormatBuf("Score[%u] Value:%d", index, score.Value);
   }

   void WorldSaveDump::DumpLocation(WorldDump & dump, const WorldSave::LocationV1* location)
   {
        if (!location)
                return;
        if (location && location->OffsetData)
                DumpLocationData(dump, location->OffsetData, location->Index, location->Data);

        if (location && location->OffsetMapsCount)
        {
                dump[location->OffsetMapsCount] = Str::FormatBuf("Location[%u] MapsCount:%u", location->Index, location->MapsCount);
                if (location->MapsCount != location->Maps.size())
                {
                        App.WriteLog("Dump world... WARNING Locations[%u]::MapsCount != Locations[%u]::Maps::size() : %u != %u\n",
                                location->Index, location->Index, location->MapsCount, location->Maps.size()
                        );
                }

                for (auto it=location->Maps.begin(),end=location->Maps.end();it!=end;++it)
                {
                        DumpMap(dump, *it);
                }
        }
   }

   void WorldSaveDump::DumpLocationData(WorldDump & dump, const uint & offset, const uint & index, const WorldSave::LocationDataV1* location_data)
   {
        if (!offset || !location_data)
                return;

        dump[offset + offsetof(WorldSave::LocationDataV1, LocId)] = Str::FormatBuf("Location[%u] Id:%u", index, location_data->LocId);
        dump[offset + offsetof(WorldSave::LocationDataV1, LocPid)] = Str::FormatBuf("Location[%u] Pid:%u", index, location_data->LocPid);
        dump[offset + offsetof(WorldSave::LocationDataV1, WorldX)] = Str::FormatBuf("Location[%u] WorldX:%u", index, location_data->WorldX);
        dump[offset + offsetof(WorldSave::LocationDataV1, WorldY)] = Str::FormatBuf("Location[%u] WorldY:%u", index, location_data->WorldY);
        dump[offset + offsetof(WorldSave::LocationDataV1, Radius)] = Str::FormatBuf("Location[%u] Radius:%u", index, location_data->Radius);
        dump[offset + offsetof(WorldSave::LocationDataV1, Visible)] = Str::FormatBuf("Location[%u] Visible:%s", index, location_data->Visible ? "true" : "false");
        dump[offset + offsetof(WorldSave::LocationDataV1, GeckVisible)] = Str::FormatBuf("Location[%u] GeckVisible:%s", index, location_data->GeckVisible ? "true" : "false");
        dump[offset + offsetof(WorldSave::LocationDataV1, AutoGarbage)] = Str::FormatBuf("Location[%u] AutoGarbage:%s", index, location_data->AutoGarbage ? "true" : "false");
        dump[offset + offsetof(WorldSave::LocationDataV1, ToGarbage)] = Str::FormatBuf("Location[%u] ToGarbage:%s", index, location_data->ToGarbage ? "true" : "false");
        dump[offset + offsetof(WorldSave::LocationDataV1, Color)] = Str::FormatBuf("Location[%u] Color:%u", index, location_data->Color);
   }

   void WorldSaveDump::DumpMap(WorldDump & dump, const WorldSave::MapV1* map)
   {
        if (map && map->OffsetData)
                DumpMapData(dump, map->OffsetData, map->LocationIndex, map->MapIndex, map->Data);
   }

   void WorldSaveDump::DumpMapData(WorldDump & dump, const uint & offset, const uint & location_index, const uint & map_index, const WorldSave::MapDataV1* map_data)
   {
        if (!offset || !map_data)
                return;

        dump[offset + offsetof(WorldSave::MapDataV1, MapId)] = Str::FormatBuf("Map[%u,%u] Id:%u", location_index, map_index, map_data->MapId);
        dump[offset + offsetof(WorldSave::MapDataV1, MapPid)] = Str::FormatBuf("Map[%u,%u] Pid:%u", location_index, map_index, map_data->MapPid);
        dump[offset + offsetof(WorldSave::MapDataV1, MapRain)] = Str::FormatBuf("Map[%u,%u] Rain:%u", location_index, map_index, map_data->MapRain);
        dump[offset + offsetof(WorldSave::MapDataV1, IsTurnBasedAviable)] = Str::FormatBuf("Map[%u,%u] TurnBased:%s", location_index, map_index, map_data->IsTurnBasedAviable ? "true" : "false");
        dump[offset + offsetof(WorldSave::MapDataV1, MapTime)] = Str::FormatBuf("Map[%u,%u] Time:%d", location_index, map_index, map_data->MapTime);
        dump[offset + offsetof(WorldSave::MapDataV1, ScriptId)] = Str::FormatBuf("Map[%u,%u] ScriptId:%u", location_index, map_index, map_data->ScriptId);
        dump[offset + offsetof(WorldSave::MapDataV1, MapDayTime)] = Str::FormatBuf("Map[%u,%u] DayTime:%d", location_index, map_index, map_data->MapDayTime[0]);
        for (uint8 x = 1; x < sizeof(map_data->MapDayTime) / sizeof(map_data->MapDayTime[0]); x++)
        {
                dump[offset + offsetof(WorldSave::MapDataV1, MapDayTime)] += Str::FormatBuf(",%d", map_data->MapDayTime[x]);
        }
        dump[offset + offsetof(WorldSave::MapDataV1, MapDayColor)] = Str::FormatBuf("Map[%u,%u] DayColor:%u", location_index, map_index, map_data->MapDayColor[0]);
        for (uint8 x = 1; x < sizeof(map_data->MapDayColor) / sizeof(map_data->MapDayColor[0]); x++)
        {
                dump[offset + offsetof(WorldSave::MapDataV1, MapDayColor)] += Str::FormatBuf(",%u", map_data->MapDayColor[x]);
        }
        dump[offset + offsetof(WorldSave::MapDataV1, UserData)] = Str::FormatBuf("Map[%u,%u] UserData:%u", location_index, map_index, map_data->UserData[0]);
        for (uint8 x = 1; x < sizeof(map_data->UserData) / sizeof(map_data->UserData[0]); x++)
        {
                dump[offset + offsetof(WorldSave::MapDataV1, UserData)] += Str::FormatBuf(",%d", map_data->UserData[x]);
        }
   }

   void WorldSaveDump::DumpCritter(WorldDump & dump, const WorldSave::CritterV1* critter)
   {
        if( critter && critter->OffsetData )
                DumpCritterData( dump, critter->OffsetData, critter->Index, critter->Data );
   }

   void WorldSaveDump::DumpCritterData(WorldDump & dump, const uint& offset, const uint & index, const WorldSave::CritterDataV1* critter_data)
   {
        if (!offset || !critter_data)
                return;

        dump[offset + offsetof(WorldSave::CritterDataV1, Id)] = Str::FormatBuf("Critter[%u] Id:%u", index, critter_data->Id);
        dump[offset + offsetof(WorldSave::CritterDataV1, HexX)] = Str::FormatBuf("Critter[%u] HexX:%u", index, critter_data->HexX );
        dump[offset + offsetof(WorldSave::CritterDataV1, HexY)] = Str::FormatBuf("Critter[%u] HexY:%u", index, critter_data->HexY);
        dump[offset + offsetof(WorldSave::CritterDataV1, WorldX)] = Str::FormatBuf("Critter[%u] WorldX:%u", index, critter_data->WorldX);
        dump[offset + offsetof(WorldSave::CritterDataV1, WorldY)] = Str::FormatBuf("Critter[%u] WorldY:%u", index, critter_data->WorldY);
        dump[offset + offsetof(WorldSave::CritterDataV1, BaseType)] = Str::FormatBuf("Critter[%u] BaseType:%u", index, critter_data->BaseType );
        dump[offset + offsetof(WorldSave::CritterDataV1, Dir)] = Str::FormatBuf("Critter[%u] Dir:%u", index, critter_data->Dir);
        dump[offset + offsetof(WorldSave::CritterDataV1, Cond)] = Str::FormatBuf("Critter[%u] Cond:%u", index, critter_data->Cond);
        dump[offset + offsetof(WorldSave::CritterDataV1, ScriptId)] = Str::FormatBuf("Critter[%u] ScriptId:%u", index, critter_data->ScriptId);
        dump[offset + offsetof(WorldSave::CritterDataV1, ShowCritterDist1)] = Str::FormatBuf("Critter[%u] ShowCritterDist1:%u", index, critter_data->ShowCritterDist1);
        dump[offset + offsetof(WorldSave::CritterDataV1, ShowCritterDist2)] = Str::FormatBuf("Critter[%u] ShowCritterDist2:%u", index, critter_data->ShowCritterDist2);
        dump[offset + offsetof(WorldSave::CritterDataV1, ShowCritterDist3)] = Str::FormatBuf("Critter[%u] ShowCritterDist3:%u", index, critter_data->ShowCritterDist3);
        dump[offset + offsetof(WorldSave::CritterDataV1, Multihex)] = Str::FormatBuf("Critter[%u] %d", index,critter_data->Multihex);
        dump[offset + offsetof(WorldSave::CritterDataV1, GlobalGroupUid)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, LastHexX)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, LastHexY)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, MapId)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, MapPid)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Params)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Anim1Life)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Anim1Knockout)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Anim1Dead)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Anim2Life)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Anim2Knockout)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Anim2Dead)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Anim2KnockoutEnd)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Lexems)] = Str::FormatBuf("Critter[%u] Lexems:%s", index,critter_data->Lexems);
        dump[offset + offsetof(WorldSave::CritterDataV1, ClientToDelete)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Temp)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, HoloInfoCount)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, HoloInfo)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Scores)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, UserData)] = Str::FormatBuf("Critter[%u]", index);

        dump[offset + offsetof(WorldSave::CritterDataV1, HomeMap)] = Str::FormatBuf("Critter[%u] HomeMap:%u", index,critter_data->HomeMap);
        dump[offset + offsetof(WorldSave::CritterDataV1, HomeX)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, HomeY)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, HomeOri)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, ProtoId)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, IsDataExt)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, FavoriteItemPid[4])] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, EnemyStackCount)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, EnemyStack)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, BagCurrentSet)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, BagRefreshTime)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, BagSize)] = Str::FormatBuf("Critter[%u]", index);
        dump[offset + offsetof(WorldSave::CritterDataV1, Bag)] = Str::FormatBuf("Critter[%u]", index);
   }

 */
