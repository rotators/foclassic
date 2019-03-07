#include "Core.h"

#include "Deprecated.h"
#include "FileManager.h"
#include "FileSystem.h"
#include "Log.h"
#include "Text.h"
#include "Item.h"

using namespace std;

#if defined (FOCLASSIC_MAPPER) || defined (FOCLASSIC_SERVER)

bool     ListsLoaded = false;
PCharVec LstNames[PATH_LIST_COUNT];

void LoadList( const char* lst_name, int path_type )
{
    FileManager fm;
    if( !fm.LoadFile( lst_name, PATH_ROOT ) )
        return;

    char        str[1024];
    uint        str_cnt = 0;
    const char* path = FileManager::GetPath( path_type );

    PCharVec&   lst = LstNames[path_type];
    for( uint i = 0, j = (uint)lst.size(); i < j; i++ )
        SAFEDELA( lst[i] );
    lst.clear();

    while( fm.GetLine( str, 1023 ) )
    {
        // Lower text
        Str::Lower( str );

        // Skip comments
        if( !Str::Length( str ) || str[0] == '#' || str[0] == ';' )
            continue;

        // New value of line
        if( str[0] == '*' )
        {
            str_cnt = atoi( &str[1] );
            continue;
        }

        // Find ext
        char* ext = (char*)FileManager::GetExtension( str );
        if( !ext )
        {
            str_cnt++;
            WriteLogF( _FUNC_, " - Extension not found in line<%s>, skip.\n", str );
            continue;
        }

        // Cut off comments
        int j = 0;
        while( ext[j] && ext[j] != ' ' )
            j++;
        ext[j] = '\0';

        // Create name
        uint  len = Str::Length( path ) + Str::Length( str ) + 1;
        char* rec = new char[len];
        Str::Copy( rec, len, path );
        Str::Copy( rec, len, str );

        // Check for size
        if( str_cnt >= lst.size() )
            lst.resize( str_cnt + 1 );

        // Add
        lst[str_cnt] = rec;
        str_cnt++;
    }
}

string GetPicName( uint lst_num, uint16 pic_num )
{
    if( pic_num >= LstNames[lst_num].size() || !LstNames[lst_num][pic_num] )
        return "";
    return string( LstNames[lst_num][pic_num] );
}

string Deprecated_GetPicName( int pid, int type, uint16 pic_num )
{
    if( !ListsLoaded )
    {
        LoadList( "data" DIR_SLASH_S "deprecated_lists" DIR_SLASH_S "tiles.lst", PATH_ART_TILES );
        LoadList( "data" DIR_SLASH_S "deprecated_lists" DIR_SLASH_S "items.lst", PATH_ART_ITEMS );
        LoadList( "data" DIR_SLASH_S "deprecated_lists" DIR_SLASH_S "scenery.lst", PATH_ART_SCENERY );
        LoadList( "data" DIR_SLASH_S "deprecated_lists" DIR_SLASH_S "walls.lst", PATH_ART_WALLS );
        LoadList( "data" DIR_SLASH_S "deprecated_lists" DIR_SLASH_S "misc.lst", PATH_ART_MISC );
        LoadList( "data" DIR_SLASH_S "deprecated_lists" DIR_SLASH_S "intrface.lst", PATH_ART_INTRFACE );
        LoadList( "data" DIR_SLASH_S "deprecated_lists" DIR_SLASH_S "inven.lst", PATH_ART_INVEN );
        ListsLoaded = true;
    }

    if( pid == -1 )
        return GetPicName( PATH_ART_INTRFACE, pic_num );
    if( pid == -2 )
        return GetPicName( PATH_ART_TILES, pic_num );
    if( pid == -3 )
        return GetPicName( PATH_ART_INVEN, pic_num );

    if( type == ITEM_TYPE_DOOR )
        return GetPicName( PATH_ART_SCENERY, pic_num );                             // For doors from Scenery
    else if( pid == SP_MISC_SCRBLOCK || SP_MISC_GRID_MAP( pid ) || SP_MISC_GRID_GM( pid ) )
        return GetPicName( PATH_ART_MISC, pic_num );                                // For exit grids from Misc
    else if( pid >= 4000 && pid <= 4200 )
        return GetPicName( PATH_ART_MISC, pic_num );                                // From Misc
    else if( type >= ITEM_TYPE_ARMOR && type <= ITEM_TYPE_DOOR )
        return GetPicName( PATH_ART_ITEMS, pic_num );                               // From Items
    else if( type == ITEM_TYPE_GENERIC || type == ITEM_TYPE_GRID )
        return GetPicName( PATH_ART_SCENERY, pic_num );                             // From Scenery
    else if( type == ITEM_TYPE_WALL )
        return GetPicName( PATH_ART_WALLS, pic_num );                               // From Walls
    return "";
}

uint Deprecated_GetPicHash( int pid, int type, uint16 pic_num )
{
    string name = Deprecated_GetPicName( pid, type, pic_num );
    if( !name.length() )
        return 0;
    return Str::GetHash( name.c_str() );
}

void Deprecated_CondExtToAnim2( uint8 cond, uint8 cond_ext, uint& anim2ko, uint& anim2dead )
{
    if( cond == CRITTER_CONDITION_KNOCKOUT )
    {
        if( cond_ext == 2 )
            anim2ko = ANIM2_IDLE_PRONE_FRONT;          // COND_KNOCKOUT_FRONT
        anim2ko = ANIM2_IDLE_PRONE_BACK;               // COND_KNOCKOUT_BACK
    }
    else if( cond == CRITTER_CONDITION_DEAD )
    {
        switch( cond_ext )
        {
            case 1:
                anim2dead = ANIM2_DEAD_FRONT;         // COND_DEAD_FRONT
            case 2:
                anim2dead = ANIM2_DEAD_BACK;          // COND_DEAD_BACK
            case 3:
                anim2dead = 112;                      // COND_DEAD_BURST -> ANIM2_DEAD_BURST
            case 4:
                anim2dead = 110;                      // COND_DEAD_BLOODY_SINGLE -> ANIM2_DEAD_BLOODY_SINGLE
            case 5:
                anim2dead = 111;                      // COND_DEAD_BLOODY_BURST -> ANIM2_DEAD_BLOODY_BURST
            case 6:
                anim2dead = 113;                      // COND_DEAD_PULSE -> ANIM2_DEAD_PULSE
            case 7:
                anim2dead = 114;                      // COND_DEAD_PULSE_DUST -> ANIM2_DEAD_PULSE_DUST
            case 8:
                anim2dead = 115;                      // COND_DEAD_LASER -> ANIM2_DEAD_LASER
            case 9:
                anim2dead = 117;                      // COND_DEAD_EXPLODE -> ANIM2_DEAD_EXPLODE
            case 10:
                anim2dead = 116;                      // COND_DEAD_FUSED -> ANIM2_DEAD_FUSED
            case 11:
                anim2dead = 118;                      // COND_DEAD_BURN -> ANIM2_DEAD_BURN
            case 12:
                anim2dead = 118;                      // COND_DEAD_BURN2 -> ANIM2_DEAD_BURN
            case 13:
                anim2dead = 119;                      // COND_DEAD_BURN_RUN -> ANIM2_DEAD_BURN_RUN
            default:
                anim2dead = ANIM2_DEAD_FRONT;
        }
    }
}

#endif // FOCLASSIC_MAPPER || FOCLASSIC_SERVER
