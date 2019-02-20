#include "Core.h"

#include "App.h"

#include "CommandLine.h"
#include "FileSystem.h"
#include "ShowWorld.h"
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
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
    }

    void* file = FileOpen( name.c_str(), false );
    if( !file )
    {
        App.WriteLog( "ERROR: file<%s> cannot be opened\n", name.c_str() );
        return EXIT_FAILURE;
    }

    WorldSave::Object::Signature signature;
    if( !signature.LoadSignature( file, name ) )
        return EXIT_FAILURE;

    WorldSave* world = WorldSaveDump::NewWorld( signature, file, name_short );

    if( !world )
    {
        App.WriteLog( "ERROR: unknown version<%u>\n", signature.Version );
        return EXIT_FAILURE;
    }

    bool success = world->LoadWorld();

    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

// Old system, left for reference
/*
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
