#ifndef __AI__
#define __AI__

#include "Debugger.h"
#include "Types.h"

#define BAGS_FILE_NAME           "Bags.cfg"
#define MAX_NPC_BAGS             (50)
#define MAX_NPC_BAGS_PACKS       (20)
#define NPC_GO_HOME_WAIT_TICK    (Random( 4000, 6000 ) )

struct AIDataPlane
{
    int          Type;
    uint         Priority;
    int          Identifier;
    uint         IdentifierExt;
    AIDataPlane* ChildPlane;
    bool         IsMove;

    union
    {
        struct
        {
            bool IsRun;
            uint WaitSecond;
            int  ScriptBindId;
        } Misc;

        struct
        {
            bool   IsRun;
            uint   TargId;
            int    MinHp;
            bool   IsGag;
            uint16 GagHexX, GagHexY;
            uint16 LastHexX, LastHexY;
        } Attack;

        struct
        {
            bool   IsRun;
            uint16 HexX;
            uint16 HexY;
            uint8  Dir;
            uint   Cut;
        } Walk;

        struct
        {
            bool   IsRun;
            uint16 HexX;
            uint16 HexY;
            uint16 Pid;
            uint   UseItemId;
            bool   ToOpen;
        } Pick;

        struct
        {
            uint Buffer[8];
        } Buffer;
    };

    struct
    {
        uint   PathNum;
        uint   Iter;
        bool   IsRun;
        uint   TargId;
        uint16 HexX;
        uint16 HexY;
        uint   Cut;
        uint   Trace;
    } Move;

    AIDataPlane( uint type, uint priority );
    ~AIDataPlane();

    AIDataPlane* GetCurPlane();
    bool         IsSelfOrHas( int type );
    void         DeleteLast();

    AIDataPlane* GetCopy();

    bool         Assigned;
    int          RefCounter;
    void AddRef() { RefCounter++; }
    void Release()
    {
        RefCounter--;
        if( !RefCounter ) delete this;
    }

private:
    AIDataPlane() {}            // Disable default constructor
};
typedef std::vector<AIDataPlane*> AIDataPlaneVec;

class NpcBagItem
{
public:
    uint ItemPid;
    uint MinCnt;
    uint MaxCnt;
    uint ItemSlot;

    NpcBagItem() : ItemPid( 0 ), MinCnt( 0 ), MaxCnt( 0 ), ItemSlot( SLOT_INV ) {}
    NpcBagItem( const NpcBagItem& r ) : ItemPid( r.ItemPid ), MinCnt( r.MinCnt ), MaxCnt( r.MaxCnt ), ItemSlot( r.ItemSlot ) {}
};
typedef std::vector<NpcBagItem>                  NpcBagItems;
typedef std::vector<NpcBagItems>                 NpcBagCombination;
typedef std::vector<NpcBagCombination>           NpcBag;
typedef std::vector<NpcBag>                      NpcBagVec;
typedef std::map<std::string, NpcBagCombination> StringNpcBagCombMap;

/************************************************************************/
/*                                                                      */
/************************************************************************/

class NpcAIMngr
{
public:
    bool Init();
    void Finish();

    NpcBag& GetBag( uint num );

private:
    NpcBagVec npcBags;
    bool LoadNpcBags();
};
extern NpcAIMngr AIMngr;

#endif // __AI__
