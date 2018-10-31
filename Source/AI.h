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
            ushort GagHexX, GagHexY;
            ushort LastHexX, LastHexY;
        } Attack;

        struct
        {
            bool   IsRun;
            ushort HexX;
            ushort HexY;
            uchar  Dir;
            uint   Cut;
        } Walk;

        struct
        {
            bool   IsRun;
            ushort HexX;
            ushort HexY;
            ushort Pid;
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
        ushort HexX;
        ushort HexY;
        uint   Cut;
        uint   Trace;
    } Move;

    AIDataPlane* GetCurPlane()           { return ChildPlane ? ChildPlane->GetCurPlane() : this; }
    bool         IsSelfOrHas( int type ) { return Type == type || (ChildPlane ? ChildPlane->IsSelfOrHas( type ) : false); }
    void         DeleteLast()
    {
        if( ChildPlane )
        {
            if( ChildPlane->ChildPlane ) ChildPlane->DeleteLast();
            else SAFEREL( ChildPlane );
        }
    }

    AIDataPlane* GetCopy()
    {
        AIDataPlane* copy = new AIDataPlane( Type, Priority );
        if( !copy ) return NULL;
        memcpy( copy->Buffer.Buffer, Buffer.Buffer, sizeof(Buffer.Buffer) );
        AIDataPlane* result = copy;
        AIDataPlane* plane_child = ChildPlane;
        while( plane_child )
        {
            copy->ChildPlane = new AIDataPlane( plane_child->Type, plane_child->Priority );
            if( !copy->ChildPlane ) return NULL;
            copy->ChildPlane->Assigned = true;
            memcpy( copy->ChildPlane->Buffer.Buffer, plane_child->Buffer.Buffer, sizeof(plane_child->Buffer.Buffer) );
            plane_child = plane_child->ChildPlane;
            copy = copy->ChildPlane;
        }
        return result;
    }

    bool Assigned;
    int  RefCounter;
    void AddRef() { RefCounter++; }
    void Release()
    {
        RefCounter--;
        if( !RefCounter ) delete this;
    }
    AIDataPlane( uint type, uint priority ) : Type( type ), Priority( priority ), Identifier( 0 ), IdentifierExt( 0 ), ChildPlane( NULL ), IsMove( false ), Assigned( false ), RefCounter( 1 )
    {
        memzero( &Buffer, sizeof(Buffer) );
        memzero( &Move, sizeof(Move) );
        MEMORY_PROCESS( MEMORY_NPC_PLANE, sizeof(AIDataPlane) );
    }
    ~AIDataPlane()
    {
        SAFEREL( ChildPlane );
        MEMORY_PROCESS( MEMORY_NPC_PLANE, -(int)sizeof(AIDataPlane) );
    }
private: AIDataPlane() {}        // Disable default constructor
};
typedef vector<AIDataPlane*> AIDataPlaneVec;

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
typedef vector<NpcBagItem>             NpcBagItems;
typedef vector<NpcBagItems>            NpcBagCombination;
typedef vector<NpcBagCombination>      NpcBag;
typedef vector<NpcBag>                 NpcBagVec;
typedef map<string, NpcBagCombination> StringNpcBagCombMap;

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
