#ifndef __PROTO_MAP__
#define __PROTO_MAP__

#include "Defines.h"
#include "FileManager.h"
#include "Types.h"

// Generic
#define MAPOBJ_SCRIPT_NAME       (25)
#define MAPOBJ_CRITTER_PARAMS    (40)

class ProtoMap;
class MapObject // Available in fonline.h
{
public:
    uint8  MapObjType;
    uint16 ProtoId;
    uint16 MapX;
    uint16 MapY;
    short  Dir;

    uint   UID;
    uint   ContainerUID;
    uint   ParentUID;
    uint   ParentChildIndex;

    uint   LightColor;
    uint8  LightDay;
    uint8  LightDirOff;
    uint8  LightDistance;
    char   LightIntensity;

    char   ScriptName[MAPOBJ_SCRIPT_NAME + 1];
    char   FuncName[MAPOBJ_SCRIPT_NAME + 1];

    uint   Reserved[7];
    int    UserData[10];

    union
    {
        struct
        {
            uint8 Cond;
            uint  Anim1;
            uint  Anim2;
            short ParamIndex[MAPOBJ_CRITTER_PARAMS];
            int   ParamValue[MAPOBJ_CRITTER_PARAMS];
        } MCritter;

        struct
        {
            short  OffsetX;
            short  OffsetY;
            uint8  AnimStayBegin;
            uint8  AnimStayEnd;
            uint16 AnimWait;
            uint8  InfoOffset;
            uint   PicMapHash;
            uint   PicInvHash;

            uint   Count;
            uint8  ItemSlot;

            uint8  BrokenFlags;
            uint8  BrokenCount;
            uint16 Deterioration;

            uint16 AmmoPid;
            uint   AmmoCount;

            uint   LockerDoorId;
            uint16 LockerCondition;
            uint16 LockerComplexity;

            short  TrapValue;

            int    Val[10];
        } MItem;

        struct
        {
            short  OffsetX;
            short  OffsetY;
            uint8  AnimStayBegin;
            uint8  AnimStayEnd;
            uint16 AnimWait;
            uint8  InfoOffset;
            uint   PicMapHash;
            uint   PicInvHash;

            bool   CanUse;
            bool   CanTalk;
            uint   TriggerNum;

            uint8  ParamsCount;
            int    Param[5];

            uint16 ToMapPid;
            uint   ToEntire;
            uint8  ToDir;

            uint8  SpriteCut;
        } MScenery;
    };

    struct _RunTime
    {
        #ifdef FOCLASSIC_MAPPER
        ProtoMap* FromMap;
        uint      MapObjId;
        char      PicMapName[64];
        char      PicInvName[64];
        #endif
        #ifdef FOCLASSIC_SERVER
        int       BindScriptId;
        #endif
        long      RefCounter;
    } RunTime;

    MapObject()
    {
        memzero( this, sizeof(MapObject) );
        RunTime.RefCounter = 1;
    }
    MapObject( const MapObject& r )
    {
        memcpy( this, &r, sizeof(MapObject) );
        RunTime.RefCounter = 1;
    }
    MapObject& operator=( const MapObject& r )
    {
        memcpy( this, &r, sizeof(MapObject) );
        RunTime.RefCounter = 1;
        return *this;
    }

    void AddRef()  { ++RunTime.RefCounter; }
    void Release() { if( !--RunTime.RefCounter ) delete this; }
};
typedef std::vector<MapObject*> MapObjectPtrVec;
typedef std::vector<MapObject>  MapObjectVec;

struct SceneryCl
{
    uint16 ProtoId;
    uint8  Flags;
    uint8  SpriteCut;
    uint16 MapX;
    uint16 MapY;
    short  OffsetX;
    short  OffsetY;
    uint   LightColor;
    uint8  LightDistance;
    uint8  LightFlags;
    char   LightIntensity;
    uint8  InfoOffset;
    uint8  AnimStayBegin;
    uint8  AnimStayEnd;
    uint16 AnimWait;
    uint   PicMapHash;
    short  Dir;
    uint16 Reserved1;
};
typedef std::vector<SceneryCl> SceneryClVec;

class ProtoMap
{
public:
    // Header
    struct
    {
        uint   Version;
        uint16 MaxHexX, MaxHexY;
        int    WorkHexX, WorkHexY;
        char   ScriptModule[MAX_SCRIPT_NAME + 1];
        char   ScriptFunc[MAX_SCRIPT_NAME + 1];
        int    Time;
        bool   NoLogOut;
        int    DayTime[4];
        uint8  DayColor[12];

        // Deprecated
        uint16 HeaderSize;
        bool   Packed;
        uint   UnpackedDataLen;
    } Header;

    // Objects
    MapObjectPtrVec MObjects;
    uint            LastObjectUID;

    // Tiles
    struct Tile     // 12 bytes
    {
        uint   NameHash;
        uint16 HexX, HexY;
        char   OffsX, OffsY;
        uint8  Layer;
        bool   IsRoof;
        #ifdef FOCLASSIC_MAPPER
        bool   IsSelected;
        #endif

        Tile() { memzero( this, sizeof(Tile) ); }
        Tile( uint name, uint16 hx, uint16 hy, char ox, char oy, uint8 layer, bool is_roof ) : NameHash( name ), HexX( hx ), HexY( hy ), OffsX( ox ), OffsY( oy ), Layer( layer ), IsRoof( is_roof ) {}
    };
    typedef std::vector<Tile>    TileVec;
    TileVec Tiles;
    #ifdef FOCLASSIC_MAPPER
    // For fast access
    typedef std::vector<TileVec> TileVecVec;
    TileVecVec TilesField;
    TileVecVec RoofsField;
    TileVec&   GetTiles( uint16 hx, uint16 hy, bool is_roof ) { return is_roof ? RoofsField[hy * Header.MaxHexX + hx] : TilesField[hy * Header.MaxHexX + hx]; }
    #endif

private:
    bool ReadHeader( FileManager& fm, int version );
    bool ReadTiles( FileManager& fm, int version );
    bool ReadObjects( FileManager& fm, int version );
    bool LoadTextFormat( const char* buf );
    #ifdef FOCLASSIC_MAPPER
    void SaveTextFormat( FileManager& fm );
    #endif

    #ifdef FOCLASSIC_SERVER
public:
    // To Client
    SceneryClVec    WallsToSend;
    SceneryClVec    SceneriesToSend;
    uint            HashTiles;
    uint            HashWalls;
    uint            HashScen;

    MapObjectPtrVec CrittersVec;
    MapObjectPtrVec ItemsVec;
    MapObjectPtrVec SceneryVec;
    MapObjectPtrVec GridsVec;
    uint8*          HexFlags;

private:
    bool LoadCache( FileManager& fm );
    void SaveCache( FileManager& fm );
    void BindSceneryScript( MapObject* mobj );
    #endif

public:
    // Entires
    struct MapEntire
    {
        uint   Number;
        uint16 HexX;
        uint16 HexY;
        uint8  Dir;

        MapEntire() { memzero( this, sizeof(MapEntire) ); }
        MapEntire( uint16 hx, uint16 hy, uint8 dir, uint type ) : HexX( hx ), HexY( hy ), Dir( dir ), Number( type ) {}
    };
    typedef std::vector<MapEntire> EntiresVec;

private:
    EntiresVec mapEntires;

public:
    uint       CountEntire( uint num );
    MapEntire* GetEntire( uint num, uint skip );
    MapEntire* GetEntireRandom( uint num );
    MapEntire* GetEntireNear( uint num, uint16 hx, uint16 hy );
    MapEntire* GetEntireNear( uint num, uint num_ext, uint16 hx, uint16 hy );
    void       GetEntires( uint num, EntiresVec& entires );

private:
    int         pathType;
    std::string pmapName;
    uint16      pmapPid;
    bool        isInit;

public:
    ProtoMap();
    #ifdef FOCLASSIC_SERVER
    ProtoMap( const ProtoMap& r );
    #endif
    ~ProtoMap();

    bool Init( uint16 pid, const char* name, int path_type );
    void Clear();
    bool Refresh();

    #ifdef FOCLASSIC_MAPPER
    void        GenNew();
    bool        Save( const char* fname, int path_type );
    static bool IsMapFile( const char* fname );
    #endif

    bool        IsInit()  { return isInit; }
    uint16      GetPid()  { return isInit ? pmapPid : 0; }
    const char* GetName() { return pmapName.c_str(); }

    long RefCounter;
    void AddRef()  { ++RefCounter; }
    void Release() { if( !--RefCounter ) delete this; }

    #ifdef FOCLASSIC_SERVER
    MapObject* GetMapScenery( uint16 hx, uint16 hy, uint16 pid );
    void       GetMapSceneriesHex( uint16 hx, uint16 hy, MapObjectPtrVec& mobjs );
    void       GetMapSceneriesHexEx( uint16 hx, uint16 hy, uint radius, uint16 pid, MapObjectPtrVec& mobjs );
    void       GetMapSceneriesByPid( uint16 pid, MapObjectPtrVec& mobjs );
    MapObject* GetMapGrid( uint16 hx, uint16 hy );
    #endif
};
typedef std::vector<ProtoMap>  ProtoMapVec;
typedef std::vector<ProtoMap*> ProtoMapPtrVec;

#endif // __PROTO_MAP__
