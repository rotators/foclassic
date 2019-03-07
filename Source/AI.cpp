#include "Core.h"

#include "AI.h"
#include "Text.h"
#include "ConstantsManager.h"
#include "FileManager.h"
#include "Item.h"
#include "IniParser.h"
#include "Log.h"

using namespace std;

NpcAIMngr AIMngr;

/************************************************************************/
/* Parsers                                                              */
/************************************************************************/

string ParseBagComb( const char* str )
{
    while( *str == ' ' )
        str++;
    return string( str );
}

NpcBagItems ParseBagItems( const char* str )
{
    NpcBagItems items;

label_ParseNext:
    NpcBagItem i;
    while( *str == ' ' )
        str++;
    char  buf[128];
    char* pbuf = buf;
    // Parse pid
    for( ; *str != ':' && *str != '+' && *str != '^' && *str; str++, pbuf++ )
        *pbuf = *str;
    *pbuf = 0;
    int pid = ConstantsManager::GetItemPid( buf );
    if( pid < 0 )
        return items;
    i.ItemPid = pid;
    // Parse place
    if( *str == '^' )
    {
        str++;
        if( *str == 'm' )
            i.ItemSlot = SLOT_HAND1;
        else if( *str == 'e' )
            i.ItemSlot = SLOT_HAND2;
        else if( *str == 'a' )
            i.ItemSlot = SLOT_ARMOR;
        if( *str )
            str++;
    }
    // Parse min count
    if( *str == ':' )
    {
        str++;
        pbuf = buf;
        for( ; *str != '-' && *str != '+' && *str; str++, pbuf++ )
            *pbuf = *str;
        *pbuf = 0;
        i.MinCnt = atoi( buf );
    }
    else
    {
        i.MinCnt = 1;
    }
    // Parse max count
    if( *str == '-' )
    {
        str++;
        pbuf = buf;
        for( ; *str != '+' && *str; str++, pbuf++ )
            *pbuf = *str;
        *pbuf = 0;
        i.MaxCnt = atoi( buf );
    }
    else
    {
        i.MaxCnt = i.MinCnt;
    }
    // Parse alias
    if( *str == '+' )
    {
        items.push_back( i );
        str++;
        goto label_ParseNext;
    }

    items.push_back( i );
    return items;
}

AIDataPlane::AIDataPlane( uint type, uint priority ) : Type( type ), Priority( priority ), Identifier( 0 ), IdentifierExt( 0 ), ChildPlane( NULL ), IsMove( false ), Assigned( false ), RefCounter( 1 )
{
    memzero( &Buffer, sizeof(Buffer) );
    memzero( &Move, sizeof(Move) );
    MEMORY_PROCESS( MEMORY_NPC_PLANE, sizeof(AIDataPlane) );
}

AIDataPlane::~AIDataPlane()
{
    SAFEREL( ChildPlane );
    MEMORY_PROCESS( MEMORY_NPC_PLANE, -(int)sizeof(AIDataPlane) );
}

AIDataPlane* AIDataPlane::GetCurPlane()
{
    return ChildPlane ? ChildPlane->GetCurPlane() : this;
}

bool AIDataPlane::IsSelfOrHas( int type )
{
    return Type == type || (ChildPlane ? ChildPlane->IsSelfOrHas( type ) : false);
}

void AIDataPlane::DeleteLast()
{
    if( ChildPlane )
    {
        if( ChildPlane->ChildPlane )
            ChildPlane->DeleteLast();
        else
            SAFEREL( ChildPlane );
    }
}

AIDataPlane* AIDataPlane::GetCopy()
{
    AIDataPlane* copy = new AIDataPlane( Type, Priority );
    if( !copy )
        return NULL;
    memcpy( copy->Buffer.Buffer, Buffer.Buffer, sizeof(Buffer.Buffer) );
    AIDataPlane* result = copy;
    AIDataPlane* plane_child = ChildPlane;
    while( plane_child )
    {
        copy->ChildPlane = new AIDataPlane( plane_child->Type, plane_child->Priority );
        if( !copy->ChildPlane )
            return NULL;
        copy->ChildPlane->Assigned = true;
        memcpy( copy->ChildPlane->Buffer.Buffer, plane_child->Buffer.Buffer, sizeof(plane_child->Buffer.Buffer) );
        plane_child = plane_child->ChildPlane;
        copy = copy->ChildPlane;
    }
    return result;
}



/************************************************************************/
/* Init/Finish                                                          */
/************************************************************************/

bool NpcAIMngr::Init()
{
    WriteLog( "Initialize AI manager...\n" );
    if( !LoadNpcBags() )
        return false;
    WriteLog( "Initialize AI manager... complete.\n" );
    return true;
}

void NpcAIMngr::Finish()
{
    WriteLog( "Finish AI manager...\n" );
    npcBags.clear();
    WriteLog( "Finish AI manager... complete\n" );
}

/************************************************************************/
/* Get                                                                  */
/************************************************************************/

NpcBag& NpcAIMngr::GetBag( uint num )
{
    return num < npcBags.size() ? npcBags[num] : npcBags[0];
}

/************************************************************************/
/* Load                                                                 */
/************************************************************************/

bool NpcAIMngr::LoadNpcBags()
{
    WriteLog( "Load bags...\n" );

    IniParser bags_txt;
    if( !bags_txt.LoadFile( BAGS_FILE_NAME, PATH_SERVER_DATA ) )
    {
        WriteLog( "Load bags... file<%s> not found\n", FileManager::GetFullPath( BAGS_FILE_NAME, PATH_SERVER_DATA ) );
        return false;
    }

    int                 end_bag = bags_txt.GetInt( "end_bag", -1 );
    int                 bag_count = 0;
    char*               bag_str = new char[0x10000];
    StringNpcBagCombMap loaded_comb;

    npcBags.resize( end_bag + 1 );
    // LogSetProperty(LogProp_WithTime,0);

    for( int i = 0; i <= end_bag; i++ )
    {
        if( !bags_txt.GetStr( Str::FormatBuf( "bag_%d", i ), "", bag_str ) )
            continue;
        NpcBag& cur_bag = npcBags[i];

        StrVec  comb;
        Str::ParseLine<StrVec, string (*)( const char* )>( bag_str, ' ', comb, ParseBagComb );

        for( uint j = 0; j < comb.size(); j++ )
        {
            string& c = comb[j];
            auto    it = loaded_comb.find( c );
            if( it == loaded_comb.end() )
            {
                // Get combination line
                if( !bags_txt.GetStr( c.c_str(), "", bag_str ) )
                {
                    WriteLog( "Items combination<%s> not found.\n", c.c_str() );
                    delete[] bag_str;
                    return false;
                }

                // Parse
                NpcBagCombination items_comb;
                Str::ParseLine<NpcBagCombination, NpcBagItems (*)( const char* )>( bag_str, ' ', items_comb, ParseBagItems );
                // Check
                for( uint l = 0; l < items_comb.size(); l++ )
                {
                    NpcBagItems& items = items_comb[l];
                    for( uint k = 0; k < items.size(); k++ )
                    {
                        NpcBagItem& b = items[k];
                        if( b.ItemPid >= MAX_PROTO_ITEMS || b.MinCnt > b.MaxCnt )
                        {
                            WriteLog( "Invalid items combination<%s>, Item combination<%d>, number<%d>.\n", c.c_str(), l, k );
                            delete[] bag_str;
                            return false;
                        }
                    }
                }

                loaded_comb.insert( PAIR( c, items_comb ) );
                it = loaded_comb.find( c );
            }

            cur_bag.push_back( (*it).second );
        }

        bag_count++;
    }
    // LogSetProperty(LogProp_WithTime,1);

    delete[] bag_str;
    WriteLog( "Load bags... loaded<%d>\n", bag_count );
    return true;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
