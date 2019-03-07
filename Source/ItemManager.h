#ifndef __ITEM_MANAGER__
#define __ITEM_MANAGER__

#include "Defines.h"
#include "Item.h"
#include "Mutex.h"
#include "Types.h"

#ifdef FOCLASSIC_SERVER
class Critter;
class Map;
#endif

class ItemManager
{
private:
    bool         isActive;
    ProtoItem    allProto[MAX_PROTO_ITEMS];  // All
    ProtoItemVec typeProto[ITEM_TYPE_MAX];   // By type
    uint         protoHash[ITEM_TYPE_MAX];   // Hash types protos
    char*        protoScript[MAX_PROTO_ITEMS];

public:
    ProtoItemVec& GetProtos( int type )     { return typeProto[type]; }
    uint          GetProtosHash( int type ) { return protoHash[type]; }

    bool Init();
    bool IsInit() { return isActive; }
    void Finish();
    void Clear();

    #if defined (FOCLASSIC_SERVER) || defined (FONLINE_OBJECT_EDITOR) || defined (FOCLASSIC_MAPPER)
    bool LoadProtos();
    bool LoadProtos( ProtoItemVec& protos, const char* fname );
    #endif

    #ifdef FONLINE_OBJECT_EDITOR
    void SaveProtos( const char* full_path );
    void SaveProtos( ProtoItemVec& protos, const char* full_path );
    #endif

    void        ParseProtos( ProtoItemVec& protos, const char* collection_name = NULL );
    ProtoItem*  GetProtoItem( uint16 pid );
    ProtoItem*  GetAllProtos();
    void        GetCopyAllProtos( ProtoItemVec& protos );
    bool        IsInitProto( uint16 pid );
    const char* GetProtoScript( uint16 pid );
    void        ClearProtos( int type = 0xFF ); // 0xFF - All
    void        ClearProto( uint16 pid );

    #ifdef FOCLASSIC_SERVER
private:
    ItemPtrMap gameItems;
    UIntVec    itemToDelete;
    UIntVec    itemToDeleteCount;
    uint       lastItemId;
    Mutex      itemLocker;

public:
    void SaveAllItemsFile( void (*save_func)( void*, size_t ) );
    bool LoadAllItemsFile( void* f, int version );
    bool CheckProtoFunctions();
    void RunInitScriptItems();

    void GetGameItems( ItemPtrVec& items );
    uint GetItemsCount();
    void SetCritterItems( Critter* cr );
    void GetItemIds( UIntSet& item_ids );

    Item* CreateItem( uint16 pid, uint count, uint item_id = 0 );
    Item* SplitItem( Item* item, uint count );
    Item* GetItem( uint item_id, bool sync_lock );

    void ItemToGarbage( Item* item );
    void ItemGarbager();

    void NotifyChangeItem( Item* item );

    void EraseItemHolder( Item* item );
    void MoveItem( Item* item, uint count, Critter* to_cr );
    void MoveItem( Item* item, uint count, Map* to_map, uint16 to_hx, uint16 to_hy );
    void MoveItem( Item* item, uint count, Item* to_cont, uint stack_id );

    Item* AddItemContainer( Item* cont, uint16 pid, uint count, uint stack_id );
    Item* AddItemCritter( Critter* cr, uint16 pid, uint count );
    bool  SubItemCritter( Critter* cr, uint16 pid, uint count, ItemPtrVec* erased_items = NULL );
    bool  SetItemCritter( Critter* cr, uint16 pid, uint count );
    bool  MoveItemCritters( Critter* from_cr, Critter* to_cr, uint item_id, uint count );
    bool  MoveItemCritterToCont( Critter* from_cr, Item* to_cont, uint item_id, uint count, uint stack_id );
    bool  MoveItemCritterFromCont( Item* from_cont, Critter* to_cr, uint item_id, uint count );
    bool  MoveItemsContainers( Item* from_cont, Item* to_cont, uint from_stack_id, uint to_stack_id );
    bool  MoveItemsContToCritter( Item* from_cont, Critter* to_cr, uint stack_id );

    // Radio
private:
    ItemPtrVec radioItems;
    Mutex      radioItemsLocker;

public:
    void RadioRegister( Item* radio, bool add );
    void RadioSendText( Critter* cr, const char* text, uint16 text_len, bool unsafe_text, uint16 text_msg, uint num_str, UInt16Vec& channels );
    void RadioSendTextEx( uint16 channel, int broadcast_type, uint from_map_id, uint16 from_wx, uint16 from_wy, const char* text, uint16 text_len, uint16 intellect, bool unsafe_text, uint16 text_msg, uint num_str, const char* lexems );
    #endif // FOCLASSIC_SERVER

    // Items statistics
private:
    int64         itemCount[MAX_PROTO_ITEMS];
    MutexSpinlock itemCountLocker;

public:
    ItemManager();

    void        AddItemStatistics( uint16 pid, uint val );
    void        SubItemStatistics( uint16 pid, uint val );
    int64       GetItemStatistics( uint16 pid );
    std::string GetItemsStatistics();
};

extern ItemManager ItemMngr;

#endif // __ITEM_MANAGER__
