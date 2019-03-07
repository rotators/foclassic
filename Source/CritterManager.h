#ifndef __CRITTER_MANAGER__
#define __CRITTER_MANAGER__

#include "Defines.h"
#include "CritterData.h"

#ifdef FOCLASSIC_SERVER
# include "Critter.h"
#endif


class CritterManager
{
private:
    bool        isActive;
    FileManager fileMngr;
    CritData    allProtos[MAX_PROTO_CRITTERS];

public:
    CritterManager();

    bool Init();
    bool IsInit() { return isActive; }
    void Finish();
    void Clear();

    bool      LoadProtos();
    CritData* GetProto( uint16 proto_id );
    CritData* GetAllProtos();

    #ifdef FOCLASSIC_SERVER
private:
    CrMap   allCritters;
    UIntVec crToDelete;
    uint    lastNpcId;
    uint    playersCount, npcCount;
    Mutex   crLocker;

public:
    void SaveCrittersFile( void (* save_func)( void*, size_t ) );
    bool LoadCrittersFile( void* f, uint version );

    void RunInitScriptCritters();

    void CritterToGarbage( Critter* cr );
    void CritterGarbager();

    Npc* CreateNpc( uint16 proto_id, uint params_count, int* params, uint items_count, int* items, const char* script, Map* map, uint16 hx, uint16 hy, uint8 dir, bool accuracy );
    Npc* CreateNpc( uint16 proto_id, bool copy_data );

    void     AddCritter( Critter* cr );
    CrMap&   GetCrittersNoLock() { return allCritters; }
    void     GetCopyCritters( CrVec& critters, bool sync_lock );
    void     GetCopyNpcs( PcVec& npcs, bool sync_lock );
    void     GetCopyPlayers( ClVec& players, bool sync_lock );
    void     GetGlobalMapCritters( uint16 wx, uint16 wy, uint radius, int find_type, CrVec& critters, bool sync_lock );
    Critter* GetCritter( uint crid, bool sync_lock );
    Client*  GetPlayer( uint crid, bool sync_lock );
    Client*  GetPlayer( const char* name, bool sync_lock );
    Npc*     GetNpc( uint crid, bool sync_lock );
    void     EraseCritter( Critter* cr );
    void     GetNpcIds( UIntSet& npc_ids );

    uint PlayersInGame();
    uint NpcInGame();
    uint CrittersInGame();
    #endif // FOCLASSIC_SERVER


    #ifdef FOCLASSIC_MAPPER
    std::string ProtosCollectionName[MAX_PROTO_CRITTERS];
    #endif
};

extern CritterManager CrMngr;

#endif // __CRITTER_MANAGER__
