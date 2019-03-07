#ifndef __HEX_MANAGER__
#define __HEX_MANAGER__

#include "CritterCl.h"
#include "ItemHex.h"
#include "ProtoMap.h"
#include "SpriteManager.h"
#include "Types.h"

#define HEX_W          (GameOpt.MapHexWidth)
#define HEX_LINE_H     (GameOpt.MapHexLineHeight)
#define HEX_REAL_H     (GameOpt.MapHexHeight)
#define HEX_OX         (GameOpt.MapHexWidth / 2)
#define HEX_OY         (GameOpt.MapHexHeight / 2)

#define TILE_OX        (GameOpt.MapTileOffsX)
#define TILE_OY        (GameOpt.MapTileOffsY)

#define ROOF_OX        (GameOpt.MapRoofOffsX)
#define ROOF_OY        (GameOpt.MapRoofOffsY)

#define MAX_MOVE_OX    (99)
#define MAX_MOVE_OY    (99)

/************************************************************************/
/* ViewField                                                            */
/************************************************************************/

struct ViewField
{
    int   HexX, HexY;
    int   ScrX, ScrY;
    float ScrXf, ScrYf;

    ViewField() : HexX( 0 ), HexY( 0 ), ScrX( 0 ), ScrY( 0 ), ScrXf( 0.0f ), ScrYf( 0.0f ) {};
};

/************************************************************************/
/* LightSource                                                          */
/************************************************************************/

struct LightSource
{
    uint16 HexX;
    uint16 HexY;
    uint   ColorRGB;
    uint8  Distance;
    uint8  Flags;
    int    Intensity;

    LightSource( uint16 hx, uint16 hy, uint color, uint8 distance, int inten, uint8 flags ) : HexX( hx ), HexY( hy ), ColorRGB( color ), Intensity( inten ), Distance( distance ), Flags( flags ) {}
};
typedef std::vector<LightSource> LightSourceVec;

/************************************************************************/
/* Field                                                                */
/************************************************************************/

struct Field
{
    struct Tile
    {
        AnyFrames* Anim;
        short      OffsX;
        short      OffsY;
        uint8      Layer;
    };
    typedef std::vector<Tile> TileVec;

    CritterCl* Crit;
    CritVec    DeadCrits;
    int        ScrX;
    int        ScrY;
    TileVec    Tiles;
    TileVec    Roofs;
    ItemHexVec Items;
    short      RoofNum;
    bool       ScrollBlock;
    bool       IsWall;
    bool       IsWallSAI;
    bool       IsWallTransp;
    bool       IsScen;
    bool       IsExitGrid;
    bool       IsNotPassed;
    bool       IsNotRaked;
    uint8      Corner;
    bool       IsNoLight;
    uint8      LightValues[3];
    bool       IsMultihex;

    void Clear();
    void AddItem( ItemHex* item );
    void EraseItem( ItemHex* item );
    void ProcessCache();
    void AddTile( AnyFrames* anim, short ox, short oy, uint8 layer, bool is_roof );
    Field() { Clear(); }
};

/************************************************************************/
/* Rain                                                                 */
/************************************************************************/

struct Drop
{
    uint  CurSprId;
    short OffsX;
    short OffsY;
    short GroundOffsY;
    short DropCnt;

    Drop() : CurSprId( 0 ), OffsX( 0 ), OffsY( 0 ), DropCnt( 0 ), GroundOffsY( 0 ) {};
    Drop( uint16 id, short x, short y, short ground_y ) : CurSprId( id ), OffsX( x ), OffsY( y ), DropCnt( -1 ), GroundOffsY( ground_y ) {};
};
typedef std::vector<Drop*> DropVec;

/************************************************************************/
/* HexField                                                             */
/************************************************************************/

class HexManager
{
    // Hexes
private:
    uint16      maxHexX, maxHexY;
    Field*      hexField;
    bool*       hexToDraw;
    char*       hexTrack;
    AnyFrames*  picTrack1, * picTrack2;
    AnyFrames*  picHexMask;
    bool        isShowTrack;
    bool        isShowHex;
    AnyFrames*  picHex[3];
    std::string curDataPrefix;

public:
    bool   ResizeField( uint16 w, uint16 h );
    Field& GetField( uint16 hx, uint16 hy )     { return hexField[hy * maxHexX + hx]; }
    bool&  GetHexToDraw( uint16 hx, uint16 hy ) { return hexToDraw[hy * maxHexX + hx]; }
    char&  GetHexTrack( uint16 hx, uint16 hy )  { return hexTrack[hy * maxHexX + hx]; }
    uint16 GetMaxHexX()                         { return maxHexX; }
    uint16 GetMaxHexY()                         { return maxHexY; }
    void   ClearHexToDraw()                     { memzero( hexToDraw, maxHexX * maxHexY * sizeof(bool) ); }
    void   ClearHexTrack()                      { memzero( hexTrack, maxHexX * maxHexY * sizeof(char) ); }
    void   SwitchShowTrack();
    bool   IsShowTrack() { return isShowTrack; };

    bool FindPath( CritterCl* cr, uint16 start_x, uint16 start_y, uint16& end_x, uint16& end_y, UInt8Vec& steps, int cut );
    bool CutPath( CritterCl* cr, uint16 start_x, uint16 start_y, uint16& end_x, uint16& end_y, int cut );
    bool TraceBullet( uint16 hx, uint16 hy, uint16 tx, uint16 ty, uint dist, float angle, CritterCl* find_cr, bool find_cr_safe, CritVec* critters, int find_type, UInt16Pair* pre_block, UInt16Pair* block, UShortPairVec* steps, bool check_passed );

    // Center
public:
    void FindSetCenter( int cx, int cy );

private:
    void FindSetCenterDir( uint16& hx, uint16& hy, int dirs[2], int steps );

    // Map load
private:
    uint16 curPidMap;
    int    curMapTime;
    int    dayTime[4];
    uint8  dayColor[12];
    uint   curHashTiles;
    uint   curHashWalls;
    uint   curHashScen;

public:
    bool   IsMapLoaded()  { return hexField != NULL; }
    uint16 GetCurPidMap() { return curPidMap; }
    bool   LoadMap( uint16 map_pid );
    void   UnloadMap();
    void   GetMapHash( uint16 map_pid, uint& hash_tiles, uint& hash_walls, uint& hash_scen );
    bool   GetMapData( uint16 map_pid, ItemVec& items, uint16& maxhx, uint16& maxhy );
    bool   ParseScenery( SceneryCl& scen );
    int    GetDayTime();
    int    GetMapTime();
    int*   GetMapDayTime();
    uint8* GetMapDayColor();

    // Init, finish, restore
private:
    #ifndef FO_D3D
    RenderTarget rtMap;
    #endif
    Sprites      mainTree;
    ViewField* viewField;

    int        screenHexX, screenHexY;
    int        hTop, hBottom, wLeft, wRight;
    int        wVisible, hVisible;

    void InitView( int cx, int cy );
    void ResizeView();
    bool IsVisible( uint spr_id, int ox, int oy );
    bool ProcessHexBorders( uint spr_id, int ox, int oy, bool resize_map );

public:
    void ChangeZoom( int zoom );   // <0 in, >0 out, 0 normalize
    void GetScreenHexes( int& sx, int& sy );
    void GetHexCurrentPosition( uint16 hx, uint16 hy, int& x, int& y );

public:
    bool SpritesCanDrawMap;

    HexManager();
    bool Init();
    void Finish();
    void ReloadSprites();

    void PreRestore();
    void PostRestore();

    void     RebuildMap( int rx, int ry );
    void     DrawMap();
    bool     Scroll();
    Sprites& GetDrawTree() { return mainTree; }
    void     RefreshMap()  { RebuildMap( screenHexX, screenHexY ); }

    struct AutoScroll_
    {
        bool   Active;
        bool   CanStop;
        double OffsX, OffsY;
        double OffsXStep, OffsYStep;
        double Speed;
        uint   LockedCritter;
    } AutoScroll;

    void ScrollToHex( int hx, int hy, double speed, bool can_stop );

private:
    bool ScrollCheckPos( int(&positions)[4], int dir1, int dir2 );
    bool ScrollCheck( int xmod, int ymod );

public:
    void SwitchShowHex();
    void SwitchShowRain();
    void SetWeather( int time, uint8 rain );

    void SetCrit( CritterCl* cr );
    bool TransitCritter( CritterCl* cr, int hx, int hy, bool animate, bool force );

    // Critters
private:
    CritMap allCritters;
    uint    chosenId;
    uint    critterContourCrId;
    int     critterContour, crittersContour;

public:
    CritterCl* GetCritter( uint crid )
    {
        if( !crid ) return NULL;
        auto it = allCritters.find( crid );
        return it != allCritters.end() ? (*it).second : NULL;
    }
    CritterCl* GetChosen()
    {
        if( !chosenId ) return NULL;
        auto it = allCritters.find( chosenId );
        return it != allCritters.end() ? (*it).second : NULL;
    }
    void     AddCrit( CritterCl* cr );
    void     RemoveCrit( CritterCl* cr );
    void     EraseCrit( uint crid );
    void     ClearCritters();
    void     GetCritters( uint16 hx, uint16 hy, CritVec& crits, int find_type );
    CritMap& GetCritters() { return allCritters; }
    void     SetCritterContour( uint crid, int contour );
    void     SetCrittersContour( int contour );
    void     SetMultihex( uint16 hx, uint16 hy, uint multihex, bool set );

    // Items
private:
    ItemHexVec hexItems;

    void PlaceItemBlocks( uint16 hx, uint16 hy, ProtoItem* proto_item );
    void ReplaceItemBlocks( uint16 hx, uint16 hy, ProtoItem* proto_item );

public:
    bool        AddItem( uint id, uint16 pid, uint16 hx, uint16 hy, bool is_added, Item::ItemData* data );
    void        ChangeItem( uint id, const Item::ItemData& data );
    void        FinishItem( uint id, bool is_deleted );
    auto        DeleteItem( ItemHex* item, bool with_delete = true )->ItemHexVec::iterator;
    void        PushItem( ItemHex* item );
    ItemHex*    GetItem( uint16 hx, uint16 hy, uint16 pid );
    ItemHex*    GetItemById( uint16 hx, uint16 hy, uint id );
    ItemHex*    GetItemById( uint id );
    void        GetItems( uint16 hx, uint16 hy, ItemHexVec& items );
    ItemHexVec& GetItems() { return hexItems; }
    Rect        GetRectForText( uint16 hx, uint16 hy );
    void        ProcessItems();

    // Light
private:
    bool           requestRebuildLight;
    uint8*         hexLight;
    uint           lightPointsCount;
    PointVecVec    lightPoints;
    PointVec       lightSoftPoints;
    LightSourceVec lightSources;
    LightSourceVec lightSourcesScen;

    void MarkLight( uint16 hx, uint16 hy, uint inten );
    void MarkLightEndNeighbor( uint16 hx, uint16 hy, bool north_south, uint inten );
    void MarkLightEnd( uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy, uint inten );
    void MarkLightStep( uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy, uint inten );
    void TraceLight( uint16 from_hx, uint16 from_hy, uint16& hx, uint16& hy, int dist, uint inten );
    void ParseLightTriangleFan( LightSource& ls );
    void ParseLight( uint16 hx, uint16 hy, int dist, uint inten, uint flags );
    void RealRebuildLight();
    void CollectLightSources();

public:
    void            ClearHexLight()                     { memzero( hexLight, maxHexX * maxHexY * sizeof(uint8) * 3 ); }
    uint8*          GetLightHex( uint16 hx, uint16 hy ) { return &hexLight[hy * maxHexX * 3 + hx * 3]; }
    void            RebuildLight()                      { requestRebuildLight = true; }
    LightSourceVec& GetLights()                         { return lightSources; }

    // Tiles, roof
private:
    Sprites tilesTree;
    int     roofSkip;
    Sprites roofTree;

    bool CheckTilesBorder( Field::Tile& tile, bool is_roof );

public:
    void RebuildTiles();
    void RebuildRoof();
    void SetSkipRoof( int hx, int hy );
    void MarkRoofNum( int hx, int hy, int num );

    // Pixel get
public:
    bool       GetHexPixel( int x, int y, uint16& hx, uint16& hy );
    ItemHex*   GetItemPixel( int x, int y, bool& item_egg ); // With transparent egg
    CritterCl* GetCritterPixel( int x, int y, bool ignore_dead_and_chosen );
    void       GetSmthPixel( int x, int y, ItemHex*& item, CritterCl*& cr );

    // Effects
public:
    bool RunEffect( uint16 eff_pid, uint16 from_hx, uint16 from_hy, uint16 to_hx, uint16 to_hy );

    // Rain
private:
    DropVec     rainData;
    int         rainCapacity;
    std::string picRainFallName;
    std::string picRainDropName;
    AnyFrames*  picRainFall;
    AnyFrames*  picRainDrop;
    Sprites     roofRainTree;

public:
    void ProcessRain();
    void SetRainAnimation( const char* fall_anim_name, const char* drop_anim_name );

    // Cursor
public:
    void SetCursorPos( int x, int y, bool show_steps, bool refresh );
    void SetCursorVisible( bool visible ) { isShowCursor = visible; }
    void DrawCursor( uint spr_id );
    void DrawCursor( const char* text );

private:
    bool       isShowCursor;
    int        drawCursorX;
    AnyFrames* cursorPrePic, * cursorPostPic, * cursorXPic;
    int        cursorX, cursorY;

/************************************************************************/
/* Mapper                                                               */
/************************************************************************/

    #ifdef FOCLASSIC_MAPPER
public:
    // Proto map
    ProtoMap* CurProtoMap;
    bool SetProtoMap( ProtoMap& pmap );

    // Selected tile, roof
public:
    void ClearSelTiles();
    void ParseSelTiles();
    void SetTile( uint name_hash, uint16 hx, uint16 hy, short ox, short oy, uint8 layer, bool is_roof, bool select );

    // Ignore pids to draw
private:
    UInt16Set fastPids;
    UInt16Set ignorePids;

public:
    void AddFastPid( uint16 pid );
    bool IsFastPid( uint16 pid );
    void ClearFastPids();
    void AddIgnorePid( uint16 pid );
    void SwitchIgnorePid( uint16 pid );
    bool IsIgnorePid( uint16 pid );
    void ClearIgnorePids();

    void GetHexesRect( const Rect& rect, UShortPairVec& hexes );
    void MarkPassedHexes();
    void AffectItem( MapObject* mobj, ItemHex* item );
    void AffectCritter( MapObject* mobj, CritterCl* cr );
    #endif // FOCLASSIC_MAPPER

};

#endif // __HEX_MANAGER__ //
