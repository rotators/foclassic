#ifndef __VARS__
#define __VARS__

#include "Defines.h"
#include "Mutex.h"
#include "ThreadSync.h"
#include "Types.h"

#define VAR_NAME_LEN                  (256)
#define VAR_DESC_LEN                  (2048)
#define VAR_FNAME_VARS                "_vars.fos"
#define VAR_DESC_MARK                 "**********"
#define VAR_CALC_QUEST( tid, val )    ( (tid) * 1000 + (val) )

// Typedefs
class TemplateVar;
class GameVar;
typedef std::vector<TemplateVar*>  TempVarVec;
typedef std::map<uint, GameVar*>   VarsMap32;
typedef std::map<uint64, GameVar*> VarsMap64;
typedef std::vector<GameVar*>      VarsVec;


class TemplateVar
{
public:
    int         Type;
    uint16      TempId;
    std::string Name;
    std::string Desc;
    int         StartVal;
    int         MinVal;
    int         MaxVal;
    uint        Flags;

    VarsMap32   Vars;
    VarsMap64   VarsUnicum;

    bool IsNotUnicum() { return Type != VAR_TYPE_UNICUM; }
    bool IsError()     { return !TempId || !Name.size() || (IsNoBorders() && (MinVal > MaxVal || StartVal < MinVal || StartVal > MaxVal) ) || (IsQuest() && Type != VAR_TYPE_LOCAL);  }
    bool IsQuest()     { return FLAG( Flags, VAR_FLAG_QUEST ); }
    bool IsRandom()    { return FLAG( Flags, VAR_FLAG_RANDOM ); }
    bool IsNoBorders() { return FLAG( Flags, VAR_FLAG_NO_CHECK ); }
    TemplateVar() : Type( VAR_TYPE_GLOBAL ), TempId( 0 ), StartVal( 0 ), MinVal( 0 ), MaxVal( 0 ), Flags( 0 ) {}
};

#ifdef FOCLASSIC_SERVER
class Critter;

class GameVar
{
public:
    uint         MasterId;
    uint         SlaveId;
    int          VarValue;
    TemplateVar* VarTemplate;
    uint         QuestVarIndex;
    uint16       Type;
    short        RefCount;
    SyncObject   Sync;

    GameVar& operator+=( const int _right );
    GameVar& operator-=( const int _right );
    GameVar& operator*=( const int _right );
    GameVar& operator/=( const int _right );
    GameVar& operator=( const int _right );
    GameVar& operator+=( const GameVar& _right );
    GameVar& operator-=( const GameVar& _right );
    GameVar& operator*=( const GameVar& _right );
    GameVar& operator/=( const GameVar& _right );
    GameVar& operator=( const GameVar& _right );
    int      operator+( const int _right )             { return VarValue + _right; }
    int      operator-( const int _right )             { return VarValue - _right; }
    int      operator*( const int _right )             { return VarValue * _right; }
    int      operator/( const int _right )             { return VarValue / _right; }
    int      operator+( const GameVar& _right )        { return VarValue + _right.VarValue; }
    int      operator-( const GameVar& _right )        { return VarValue - _right.VarValue; }
    int      operator*( const GameVar& _right )        { return VarValue * _right.VarValue; }
    int      operator/( const GameVar& _right )        { return VarValue / _right.VarValue; }
    bool     operator>( const int _right ) const       { return VarValue > _right; }
    bool     operator<( const int _right ) const       { return VarValue < _right; }
    bool     operator==( const int _right ) const      { return VarValue == _right; }
    bool     operator>=( const int _right ) const      { return VarValue >= _right; }
    bool     operator<=( const int _right ) const      { return VarValue <= _right; }
    bool     operator!=( const int _right ) const      { return VarValue != _right; }
    bool     operator>( const GameVar& _right ) const  { return VarValue > _right.VarValue; }
    bool     operator<( const GameVar& _right ) const  { return VarValue < _right.VarValue; }
    bool     operator==( const GameVar& _right ) const { return VarValue == _right.VarValue; }
    bool     operator>=( const GameVar& _right ) const { return VarValue >= _right.VarValue; }
    bool     operator<=( const GameVar& _right ) const { return VarValue <= _right.VarValue; }
    bool     operator!=( const GameVar& _right ) const { return VarValue != _right.VarValue; }

    int          GetValue()       { return VarValue; }
    int          GetMin()         { return VarTemplate->MinVal; }
    int          GetMax()         { return VarTemplate->MaxVal; }
    bool         IsQuest()        { return VarTemplate->IsQuest(); }
    uint         GetQuestStr()    { return VAR_CALC_QUEST( VarTemplate->TempId, VarValue ); }
    bool         IsRandom()       { return VarTemplate->IsRandom(); }
    TemplateVar* GetTemplateVar() { return VarTemplate; }
    uint64       GetUid()         { return ( ( (uint64)SlaveId ) << 32 ) | ( (uint64)MasterId ); }
    uint         GetMasterId()    { return MasterId; }
    uint         GetSlaveId()     { return SlaveId; }

    void AddRef()  { RefCount++; }
    void Release() { if( !--RefCount ) delete this; }

    GameVar( uint master_id, uint slave_id, TemplateVar* var_template, int val );
    ~GameVar();
private: GameVar() {}
};

// Wrapper
int  GameVarAddInt( GameVar& var, const int _right );
int  GameVarSubInt( GameVar& var, const int _right );
int  GameVarMulInt( GameVar& var, const int _right );
int  GameVarDivInt( GameVar& var, const int _right );
int  GameVarAddGameVar( GameVar& var, GameVar& _right );
int  GameVarSubGameVar( GameVar& var, GameVar& _right );
int  GameVarMulGameVar( GameVar& var, GameVar& _right );
int  GameVarDivGameVar( GameVar& var, GameVar& _right );
bool GameVarEqualInt( const GameVar& var, const int _right );
int  GameVarCmpInt( const GameVar& var, const int _right );
bool GameVarEqualGameVar( const GameVar& var, const GameVar& _right );
int  GameVarCmpGameVar( const GameVar& var, const GameVar& _right );
#endif // FOCLASSIC_SERVER

class VarManager
{
private:
    bool         isInit;
    std::string  varsPath;
    TempVarVec   tempVars;
    StrUInt16Map varsNames;
    Mutex        varsLocker;

    bool LoadTemplateVars( const char* str, TempVarVec& vars );   // Return count error

public:
    bool Init( const char* fpath );
    void Finish();
    void Clear();
    bool IsInit() { return isInit; }

    bool         UpdateVarsTemplate();
    bool         AddTemplateVar( TemplateVar* var );
    void         EraseTemplateVar( uint16 temp_id );
    TemplateVar* GetTemplateVar( uint16 temp_id );
    uint16       GetTemplateVarId( const char* var_name );
    bool         IsTemplateVarAviable( const char* var_name );
    void         SaveTemplateVars();
    TempVarVec&  GetTemplateVars() { return tempVars; }

    #ifdef FOCLASSIC_SERVER
public:
    void     SaveVarsDataFile( void (* save_func)( void*, size_t ) );
    bool     LoadVarsDataFile( void* f, int version );
    bool     CheckVar( const char* var_name, uint master_id, uint slave_id, char oper, int val );
    bool     CheckVar( uint16 temp_id, uint master_id, uint slave_id, char oper, int val );
    GameVar* ChangeVar( const char* var_name, uint master_id, uint slave_id, char oper, int val );
    GameVar* ChangeVar( uint16 temp_id, uint master_id, uint slave_id, char oper, int val );
    GameVar* GetVar( const char* var_name, uint master_id, uint slave_id,  bool create );
    GameVar* GetVar( uint16 temp_id, uint master_id, uint slave_id,  bool create );
    void     SwapVars( uint id1, uint id2 );
    uint     ClearUnusedVars( UIntSet& ids1, UIntSet& ids2, UIntSet& ids_locs, UIntSet& ids_maps, UIntSet& ids_items );
    void     GetQuestVars( uint master_id, UIntVec& vars );
    VarsVec& GetQuestVars() { return allQuestVars; }
    uint     GetVarsCount() { return varsCount; }

private:
    VarsVec allQuestVars;
    uint    varsCount;

    bool     CheckVar( GameVar* var, char oper, int val );
    void     ChangeVar( GameVar* var, char oper, int val );
    GameVar* CreateVar( uint master_id, TemplateVar* tvar );
    GameVar* CreateVarUnicum( uint64 id, uint master_id, uint slave_id, TemplateVar* tvar );
    #endif // FOCLASSIC_SERVER
};

extern VarManager VarMngr;

#endif // __VARS__
