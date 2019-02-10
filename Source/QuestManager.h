#ifndef __QUEST_MANAGER__
#define __QUEST_MANAGER__

#include "MsgFiles.h"
#include "Types.h"

#define QUEST_MUL    1000

struct Quest
{
    uint16 num;
    string str;
    string info;
    bool   isInfo;

    bool operator==( const uint16& _num ) { return _num == num; }
    Quest( uint _num, string _info ) : num( _num ), info( _info ), isInfo( false ) {}
};
typedef vector<Quest> QuestVec;

class QuestTab
{
private:
    QuestVec quests;
    string   text;
    FOMsg*   msg;

    void ReparseText();

public:
    QuestTab( FOMsg* _msg ) : msg( _msg ) {}
    bool        IsEmpty();
    Quest*      AddQuest( uint16 num, string info );
    void        RefreshQuest( uint16 num, string str );
    Quest*      GetQuest( uint16 num );
    void        EraseQuest( uint16 num );
    QuestVec*   GetQuests();
    const char* GetText();
};
typedef map<string, QuestTab, less<string>> QuestTabMap;

class QuestManager
{
private:
    FOMsg*      msg;
    QuestTabMap tabs;

public:
    void         Init( FOMsg* quest_msg );
    void         Finish();
    void         OnQuest( uint num );
    QuestTabMap* GetTabs();
    QuestTab*    GetTab( uint tab_num );
    Quest*       GetQuest( uint tab_num, uint16 quest_num );
    Quest*       GetQuest( uint num );
};

#endif // __QUEST_MANAGER__
