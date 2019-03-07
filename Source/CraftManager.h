#ifndef __CRAFT_MANAGER__
#define __CRAFT_MANAGER__

#include "MsgFiles.h"
#include "Types.h"

#define MRFIXIT_METADATA      '!'
#define MRFIXIT_NEXT          '@'
#define MRFIXIT_SPACE         ' '
#define MRFIXIT_AND           '&'
#define MRFIXIT_OR            '|'

#define MRFIXIT_METADATA_S    "!"
#define MRFIXIT_NEXT_S        "@"
#define MRFIXIT_SPACE_S       " "
#define MRFIXIT_AND_S         "&"
#define MRFIXIT_OR_S          "|"

struct CraftItem
{
public:
    // Number, name, info
    uint        Num;
    std::string Name;
    std::string Info;

    // Need parameters to show craft
    UIntVec     ShowPNum;
    IntVec      ShowPVal;
    UInt8Vec    ShowPOr;

    // Need parameters to craft
    UIntVec     NeedPNum;
    IntVec      NeedPVal;
    UInt8Vec    NeedPOr;

    // Need items to craft
    UInt16Vec   NeedItems;
    UIntVec     NeedItemsVal;
    UInt8Vec    NeedItemsOr;

    // Need tools to craft
    UInt16Vec   NeedTools;
    UIntVec     NeedToolsVal;
    UInt8Vec    NeedToolsOr;

    // New items
    UInt16Vec   OutItems;
    UIntVec     OutItemsVal;

    // Other
    std::string Script;
    uint        ScriptBindId; // In runtime
    uint        Experience;

    // Operator =
    CraftItem();
    CraftItem& operator=( const CraftItem& _right );
    bool IsValid();
    void Clear();

    void AddRef()  {}
    void Release() {}
// Set, get parse
    #ifdef FOCLASSIC_CLIENT
    void SetName( FOMsg& msg_game, FOMsg& msg_item );
    #endif

    int         SetStr( uint num, const char* str );
    const char* GetStr( bool metadata );

    #if defined (FOCLASSIC_SERVER) || defined (FONLINE_MRFIXIT)
private:
    int  SetStrParam( const char*& pstr_in, UIntVec& num_vec, IntVec& val_vec, UInt8Vec& or_vec );
    int  SetStrItem( const char*& pstr_in, UInt16Vec& pid_vec, UIntVec& count_vec, UInt8Vec& or_vec );
    void GetStrParam( char* pstr_out, UIntVec& num_vec, IntVec& val_vec, UInt8Vec& or_vec );
    void GetStrItem( char* pstr_out, UInt16Vec& pid_vec, UIntVec& count_vec, UInt8Vec& or_vec );
    #endif
};

typedef std::map<uint, CraftItem*, std::less<uint>> CraftItemMap;
typedef std::vector<CraftItem*>                     CraftItemVec;

#ifdef FOCLASSIC_SERVER
class Critter;
#endif
#ifdef FOCLASSIC_CLIENT
class CritterCl;
#endif

class CraftManager
{
private:
    CraftItemMap itemCraft;

public:
    bool operator==( const CraftManager& r );

    #ifdef FONLINE_MRFIXIT
    // Return fail crafts
    bool LoadCrafts( const char* path );
    bool SaveCrafts( const char* path );

    CraftItemMap& GetAllCrafts() { return itemCraft; }

    void EraseCraft( uint num );
    #endif

    // Return fail crafts
    bool LoadCrafts( FOMsg& msg );

    #ifdef FOCLASSIC_CLIENT
    // Item manager must be init!
    void GenerateNames( FOMsg& msg_game, FOMsg& msg_item );
    #endif

    void Finish();

    bool       AddCraft( uint num, const char* str );
    bool       AddCraft( CraftItem* craft, bool make_copy );
    CraftItem* GetCraft( uint num );
    bool       IsCraftExist( uint num );

    #ifdef FOCLASSIC_SERVER
public:
    bool IsShowCraft( Critter* cr, uint num );
    void GetShowCrafts( Critter* cr, CraftItemVec& craft_vec );
    bool IsTrueCraft( Critter* cr, uint num );
    void GetTrueCrafts( Critter* cr, CraftItemVec& craft_vec );
private:
    bool IsTrueParams( Critter* cr, UIntVec& num_vec, IntVec& val_vec, UInt8Vec& or_vec );
    bool IsTrueItems( Critter* cr, UInt16Vec& pid_vec, UIntVec& count_vec, UInt8Vec& or_vec );
    #endif
    #ifdef FOCLASSIC_CLIENT
public:
    bool IsShowCraft( CritterCl* cr, uint num );
    void GetShowCrafts( CritterCl* cr, CraftItemVec& craft_vec );
    bool IsTrueCraft( CritterCl* cr, uint num );
    void GetTrueCrafts( CritterCl* cr, CraftItemVec& craft_vec );
private:
    bool IsTrueParams( CritterCl* cr, UIntVec& num_vec, IntVec& val_vec, UInt8Vec& or_vec );
    bool IsTrueItems( CritterCl* cr, UInt16Vec& pid_vec, UIntVec& count_vec, UInt8Vec& or_vec );
    #endif

    #ifdef FOCLASSIC_SERVER
public:
    int ProcessCraft( Critter* cr, uint num );
    #endif
};

extern CraftManager MrFixit;

#endif // __CRAFT_MANAGER__ //
