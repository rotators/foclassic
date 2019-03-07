#ifndef __WORLD_SAVE__
#define __WORLD_SAVE__

#include <functional>

#include "Defines.h"
#include "Types.h"

#include "../AI.h"
#include "../Item.h"

typedef std::function<void (uint value, const std::string& name)>                                     WorldSaveCallbackCounter;
typedef std::function<void (void*& object, const std::string& name, const uint& version)>             WorldSaveCallbackObject;
typedef std::function<void (std::vector<void*>& group, const std::string& name, const uint& version)> WorldSaveCallbackGroup;

class WorldSave
{
public:
    class V1;

    struct ID
    {
        static const std::string AnyData;
        static const std::string AnyDataCounter;
        static const std::string Critter;
        static const std::string CritterCounter;
        static const std::string CritterTimeEvent;
        static const std::string Holo;
        static const std::string HoloCounter;
        static const std::string Item;
        static const std::string ItemCounter;
        static const std::string Location;
        static const std::string LocationCounter;
        static const std::string Map;
        static const std::string Score;
        static const std::string ScriptFunction;
        static const std::string ScriptFunctionCounter;
        static const std::string SinglePlayer;
        static const std::string Time;
        static const std::string TimeEvent;
        static const std::string TimeEventCounter;
        static const std::string Var;
        static const std::string VarCounter;
    };

    struct Object
    {
        // "saved"        structure is loaded as single read call; members cannot be added/reordered
        // "constructed"  structure is loaded as multiple read calls; members can be added/reordered

        struct LocationV1;
        struct LocationDataV1;
        struct MapV1;
        struct MapDataV1;
        struct CritterV1;
        struct CritterDataV1;
        struct CritterDataExtV1;
        struct CritterTimeEventV1;
        struct ItemV1;
        struct ItemDataV1;
        struct HoloV1;
        struct AnyDataV1;
        struct TimeEventV1;
        struct ScriptFunctionV1;

        // constructed
        struct Signature
        {
            // position of version/signature appended at EOF
            uint   OffsetEnd;

            // worldsave versionset to 1 in case of compatibile legacy version
            uint16 Version;

            // size of signature in bytes (start of file)
            uint8  SizeBegin;

            // size of version/signature in bytes (EOF)
            uint8  SizeEnd;

            // true if worldsave has been created by FOnline
            bool   Legacy;

            Signature();

            // Loads signature from already opened <file>
            bool Load( void* file, std::string Name );
        };

        // constructed
        struct SinglePlayerV1
        {
            uint                             Version;
            char                             Name[UTF8_BUF_SIZE( MAX_NAME )];
            char                             PasswordHash[PASS_HASH_SIZE];
            CritterDataV1*                   Data;
            CritterDataExtV1*                DataExt;
            uint                             TimeEventsCount;
            std::vector<CritterTimeEventV1*> TimeEvents;
            uint                             PicSize;
            std::vector<uint8>               PicData;

            SinglePlayerV1();
            ~SinglePlayerV1();
        };

        // saved
        struct TimeV1
        {
            uint16 YearStart;
            uint16 Year;
            uint16 Month;
            uint16 Day;
            uint16 Hour;
            uint16 Minute;
            uint16 Second;
            uint16 Multiplier;
        };

        // constructed
        struct ScoreV1
        {
            uint Index;

            uint ClientId;
            char ClientName[SCORE_NAME_LEN];
            int  Value;

            ScoreV1( uint index = 0 );
        };

        // constructed
        struct LocationV1
        {
            uint                Index;

            LocationDataV1*     Data;
            uint                MapsCount;
            std::vector<MapV1*> Maps;

            LocationV1( uint index = 0 );
            ~LocationV1();
        };

        // saved
        struct LocationDataV1
        {
            uint   LocId;
            uint16 LocPid;
            uint16 WorldX;
            uint16 WorldY;
            uint16 Radius;
            bool   Visible;
            bool   GeckVisible;
            bool   AutoGarbage;
            bool   ToGarbage;
            uint   Color;
            uint   Reserved3[59];
        };

        // constructed
        struct MapV1
        {
            uint       LocationIndex;
            uint       MapIndex;

            MapDataV1* Data;

            MapV1( uint location_index = 0, uint map_index = 0 );
            ~MapV1();
        };

        // saved
        struct MapDataV1
        {
            uint   MapId;
            uint16 MapPid;
            uint8  MapRain;
            bool   IsTurnBasedAviable;
            int    MapTime;
            uint   ScriptId;
            int    MapDayTime[4];
            uint8  MapDayColor[12];
            uint   Reserved[20];
            int    UserData[100];   // should be [MAP_MAX_DATA] ... including Map.h is not a good idea currently
        };

        // saved, part of CritterDataV1
        struct CritterBagV1
        {
            uint ItemPid;
            uint MinCnt;
            uint MaxCnt;
            uint ItemSlot;
        };

        // constructed
        struct CritterV1
        {
            uint                             Index;

            CritterDataV1*                   Data;
            CritterDataExtV1*                DataExt;

            uint                             TimeEventsCount;
            std::vector<CritterTimeEventV1*> TimeEvents;

            CritterV1( uint index = 0 );
            ~CritterV1();
        };

        // saved
        struct CritterDataV1
        {
            uint         Id;
            uint16       HexX;
            uint16       HexY;
            uint16       WorldX;
            uint16       WorldY;
            uint         BaseType;
            uint8        Dir;
            uint8        Cond;
            uint8        ReservedCE;
            char         Reserved0;
            uint         ScriptId;
            uint         ShowCritterDist1;
            uint         ShowCritterDist2;
            uint         ShowCritterDist3;
            uint16       Reserved00;
            short        Multihex;
            uint         GlobalGroupUid;
            uint16       LastHexX;
            uint16       LastHexY;
            uint         Reserved1[4];
            uint         MapId;
            uint16       MapPid;
            uint16       Reserved2;
            int          Params[MAX_PARAMS];
            uint         Anim1Life;
            uint         Anim1Knockout;
            uint         Anim1Dead;
            uint         Anim2Life;
            uint         Anim2Knockout;
            uint         Anim2Dead;
            uint         Anim2KnockoutEnd;
            uint         Reserved3[3];
            char         Lexems[LEXEMS_SIZE];
            uint         Reserved4[8];
            bool         ClientToDelete;
            uint8        Reserved5;
            uint16       Reserved6;
            uint         Temp;
            uint16       Reserved8;
            uint16       HoloInfoCount;
            uint         HoloInfo[MAX_HOLO_INFO];
            uint         Reserved9[10];
            int          Scores[SCORES_MAX];
            uint8        UserData[CRITTER_USER_DATA_SIZE];
            // Npc data
            uint         HomeMap;
            uint16       HomeX;
            uint16       HomeY;
            uint8        HomeOri;
            uint8        Reserved11;
            uint16       ProtoId;
            uint         Reserved12;
            uint         Reserved13;
            uint         Reserved14;
            uint         Reserved15;
            bool         IsDataExt;
            uint8        Reserved16;
            uint16       Reserved17;
            uint         Reserved18[8];
            uint16       FavoriteItemPid[4];
            uint         Reserved19[10];
            uint         EnemyStackCount;
            uint         EnemyStack[MAX_ENEMY_STACK];
            uint         Reserved20[5];
            uint8        BagCurrentSet[MAX_NPC_BAGS_PACKS];
            short        BagRefreshTime;
            uint8        Reserved21;
            uint8        BagSize;
            CritterBagV1 Bag[MAX_NPC_BAGS];
            uint         Reserved22[100];
        };

        // saved
        struct CritterDataExtV1
        {
            uint   Reserved23[10];
            uint8  GlobalMapFog[GM_ZONES_FOG_SIZE];
            uint16 Reserved24;
            uint16 LocationsCount;
            uint   LocationsId[MAX_STORED_LOCATIONS];
            uint   Reserved25[40];
            uint   PlayIp[MAX_STORED_IP];
            uint16 PlayPort[MAX_STORED_IP];
            uint   CurrentIp;
            uint   Reserved26[29];
        };

        // saved
        struct CritterTimeEventV1
        {
            uint FuncNum;
            uint Rate;
            uint NextTime;
            int  Identifier;
        };

        // constructed
        struct ItemV1
        {
            uint        Index;

            ItemDataV1* Data;

            char        AccBuffer[8];
            char*       Lexems;

            uint        Id;
            uint16      Pid;

            uint8       LexemsLength;
            uint8       Accessory;

            ItemV1( uint index = 0 );
            ~ItemV1();
        };

        // saved, used in NetProtocol.h (sic!)
        struct ItemDataV1
        {
            uint16 SortValue;
            uint8  Info;
            uint8  Indicator;
            uint   PicMapHash;
            uint   PicInvHash;
            uint16 AnimWaitBase;
            uint8  AnimStay[2];
            uint8  AnimShow[2];
            uint8  AnimHide[2];
            uint   Flags;
            uint8  Mode;
            char   LightIntensity;
            uint8  LightDistance;
            uint8  LightFlags;
            uint   LightColor;
            uint16 ScriptId;
            short  TrapValue;
            uint   Count;
            uint   Cost;
            int    ScriptValues[ITEM_MAX_SCRIPT_VALUES];
            uint8  BrokenFlags;
            uint8  BrokenCount;
            uint16 Deterioration;
            uint16 AmmoPid;
            uint16 AmmoCount;
            uint   LockerId;
            uint16 LockerCondition;
            uint16 LockerComplexity;
            uint   HolodiskNumber;
            uint16 RadioChannel;
            uint16 RadioFlags;
            uint8  RadioBroadcastSend;
            uint8  RadioBroadcastRecv;
            uint16 Charge;
            short  OffsetX;
            short  OffsetY;
            short  Dir;
            char   Reserved[2];
        };

        // constructed
        struct VarV1
        {
            uint   Index;

            uint16 TempId;
            uint   MasterId;
            uint   SlaveId;
            int    Value;

            VarV1( uint index = 0 );
        };

        // constructed
        struct HoloV1
        {
            uint   Index;

            uint   Id;
            char*  Title;
            char*  Text;
            uint16 TitleLength;
            uint16 TextLength;
            bool   CanRW;

            HoloV1( uint index = 0 );
        };

        // constructed
        struct AnyDataV1
        {
            uint               Index;

            uint               NameLength;
            char*              Name;

            uint               DataLength;
            std::vector<uint8> Data;

            AnyDataV1( uint index = 0 );
            ~AnyDataV1();
        };

        // constructed
        struct TimeEventV1
        {
            uint              Index;

            uint              Num;
            uint16            ScriptNameLength;
            char*             ScriptName;
            uint              BeginSecond;
            uint              Rate;
            uint              ValuesLength;
            std::vector<uint> Values;

            TimeEventV1( uint index = 0 );
        };

        // constructed
        struct ScriptFunctionV1
        {
            uint  Index;

            uint  NameLength;
            char* Name;

            ScriptFunctionV1( uint index = 0 );
        };
    };

private:
    char                         ReadBuffer[MAX_FOTEXT];
    WorldSave::Object::Signature Signature;

protected:
    void* World;

public:
    uint8       LogLevel;
    std::string FileName;

    // called after objects counter is loaded
    std::vector<WorldSaveCallbackCounter> OnCounterLoaded;

    // called after object is fully loaded
    // if object is changed to null during callback, it is *not* added to group
    std::vector<WorldSaveCallbackObject> OnObjectLoaded;

    // called after object group is fully loaded
    std::vector<WorldSaveCallbackGroup> OnGroupLoaded;

    WorldSave( WorldSave::Object::Signature signature, void* world, std::string filename = std::string() );   // loading
    virtual ~WorldSave() {}

    // convert arguments for ReadData() to simple string
    static std::string GetDataName( const std::string& name0, const std::string& name1, const uint& index0, const uint& index1 );
    virtual bool       ReadData( void* buf, uint len, const std::string& name0, const std::string& name1 = std::string(), const uint& index0 = MAX_UINT, const uint& index1 = MAX_UINT );

private:
    bool ReadCounter( uint& counter, const std::string& name );
    void RunCounterLoaded( uint& count, const std::string& name );
    void RunObjectLoaded( void*& object, const std::string& name, const uint& version );
    void RunGroupLoaded( std::vector<void*>& group, const std::string& name, const uint& version );

public:
    void LogLoad( const char* frmt, ... );
    void LogLoadWarning( const char* frmt, ... );
    void LogLoadError( const char* frmt, ... );

    WorldSave::Object::Signature GetSignature();

    virtual bool LoadWorld() = 0;
    virtual void UnloadWorld() = 0;

    bool LoadSinglePlayer( Object::SinglePlayerV1*& singleplayer );
    bool LoadTime( WorldSave::Object::TimeV1*& time );
    bool LoadScores( std::vector<WorldSave::Object::ScoreV1*>& scores );
    bool LoadLocations( std::vector<WorldSave::Object::LocationV1*>& locations );
    bool LoadCritters( std::vector<WorldSave::Object::CritterV1*>& critters );
    bool LoadItems( std::vector<WorldSave::Object::ItemV1*>& items );
    bool LoadVars( std::vector<WorldSave::Object::VarV1*>& vars );
    bool LoadHolo( std::vector<WorldSave::Object::HoloV1*>& holo );
    bool LoadAnyData( std::vector<WorldSave::Object::AnyDataV1*>& anydata );
    bool LoadTimeEvents( std::vector<WorldSave::Object::TimeEventV1*>& events );
    bool LoadScriptFunctions( std::vector<WorldSave::Object::ScriptFunctionV1*>& functions );
};

class WorldSave::V1 : public WorldSave
{
public:
    WorldSave::Object::SinglePlayerV1*                SinglePlayer; // null if loading multiplayer worldsave
    WorldSave::Object::TimeV1*                        Time;
    std::vector<WorldSave::Object::ScoreV1*>          Scores;
    std::vector<WorldSave::Object::LocationV1*>       Locations;
    std::vector<WorldSave::Object::CritterV1*>        Critters;
    std::vector<WorldSave::Object::ItemV1*>           Items;
    std::vector<WorldSave::Object::VarV1*>            Vars;
    std::vector<WorldSave::Object::HoloV1*>           Holos;
    std::vector<WorldSave::Object::AnyDataV1*>        AnyData;
    std::vector<WorldSave::Object::TimeEventV1*>      TimeEvents;
    std::vector<WorldSave::Object::ScriptFunctionV1*> ScriptFunctions;

    V1( WorldSave::Object::Signature signature, void* world, std::string filename = std::string() );
    virtual ~V1();

    virtual bool LoadWorld() override;
    virtual void UnloadWorld() override;

    void UnloadSinglePlayer();
    void UnloadTime();
    void UnloadScores();
    void UnloadLocations();
    void UnloadCritters();
    void UnloadItems();
    void UnloadVars();
    void UnloadHolos();
    void UnloadAnyData();
    void UnloadTimeEvents();
    void UnloadScriptFunctions();
};

#endif // __WORLD_SAVE__ //
