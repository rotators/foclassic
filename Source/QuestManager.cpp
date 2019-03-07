#include "Core.h"

#include <algorithm>

#include "MsgStr.h"
#include "QuestManager.h"
#include "Text.h"

using namespace std;

/// QuestTab
void QuestTab::ReparseText()
{
    text = "";

    char str[128];
    for( uint i = 0; i < quests.size(); ++i )
    {
        Str::Format( str, msg->GetStr( STR_QUEST_NUMBER ), i + 1 );

        text += str;
        text += quests[i].info;
        text += "\n";
        text += msg->GetStr( STR_QUEST_PROCESS );
        text += quests[i].str;
        text += "\n\n";
    }
}

bool QuestTab::IsEmpty()
{
    return quests.empty();
}

Quest* QuestTab::AddQuest( uint16 num, string info )
{
    quests.push_back( Quest( num, info ) );
    return &quests[quests.size() - 1];
    ReparseText();
}
void QuestTab::RefreshQuest( uint16 num, string str )
{
    Quest* quest = GetQuest( num );
    if( !quest )
        return;
    quest->str = str;
    ReparseText();
}
Quest* QuestTab::GetQuest( uint16 num )
{
    auto it = std::find( quests.begin(), quests.end(), num );
    return it != quests.end() ? &(*it) : NULL;
}
void QuestTab::EraseQuest( uint16 num )
{
    auto it = std::find( quests.begin(), quests.end(), num );
    if( it != quests.end() )
        quests.erase( it );
    ReparseText();
}
QuestVec*   QuestTab::GetQuests() { return &quests; }
const char* QuestTab::GetText()   { return text.c_str(); }


void QuestManager::Init( FOMsg* quest_msg )
{
    msg = quest_msg;
}

void QuestManager::Finish()
{
    tabs.clear();
}

void QuestManager::OnQuest( uint num )
{
    // Split
    uint16 q_num = num / QUEST_MUL;
    uint16 val = num % QUEST_MUL;

    // Check valid Name of Tab
    if( !msg->Count( STR_QUEST_MAP_( q_num ) ) )
        return;

    // Get Name of Tab
    string tab_name = string( msg->GetStr( STR_QUEST_MAP_( q_num ) ) );

    // Try get Tab
    QuestTab* tab = NULL;
    auto      it_tab = tabs.find( tab_name );
    if( it_tab != tabs.end() )
        tab = &(*it_tab).second;

    // Try get Quest
    Quest* quest = NULL;
    if( tab )
        quest = tab->GetQuest( q_num );

    // Erase	quest
    if( !val )
    {
        if( tab )
        {
            tab->EraseQuest( q_num );
            if( tab->IsEmpty() )
                tabs.erase( tab_name );
        }
        return;
    }

    // Add Tab if not exists
    if( !tab )
        tab = &(*(tabs.insert( PAIR( tab_name, QuestTab( msg ) ) ) ).first).second;

    // Add Quest if not exists
    if( !quest )
        quest = tab->AddQuest( q_num, string( msg->GetStr( STR_QUEST_INFO_( q_num ) ) ) );

    // Get name of quest
    tab->RefreshQuest( q_num, string( msg->GetStr( num ) ) );
}

QuestTabMap* QuestManager::GetTabs()
{
    return &tabs;
}

QuestTab* QuestManager::GetTab( uint tab_num )
{
    if( tabs.empty() )
        return NULL;

    auto it = tabs.begin();
    while( tab_num )
    {
        ++it;
        --tab_num;
        if( it == tabs.end() )
            return NULL;
    }

    return &(*it).second;
}

Quest* QuestManager::GetQuest( uint tab_num, uint16 quest_num )
{
    QuestTab* tab = GetTab( tab_num );
    return tab ? tab->GetQuest( quest_num ) : NULL;
}

Quest* QuestManager::GetQuest( uint num )
{
    if( !msg->Count( STR_QUEST_MAP_( num / QUEST_MUL ) ) )
        return NULL;
    string tab_name = string( msg->GetStr( STR_QUEST_MAP_( num / QUEST_MUL ) ) );
    auto   it_tab = tabs.find( tab_name );
    return it_tab != tabs.end() ? (*it_tab).second.GetQuest( num / QUEST_MUL ) : NULL;
}
