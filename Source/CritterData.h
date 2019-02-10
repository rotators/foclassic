#ifndef __CRITTER_DATA__
#define __CRITTER_DATA__

#include "Item.h"
#include "FileManager.h"
#include "AI.h"
#include "Defines.h"

struct CritData
{
    uint       Id;
    uint16     HexX;
    uint16     HexY;
    uint16     WorldX;
    uint16     WorldY;
    uint       BaseType;
    uint8      Dir;
    uint8      Cond;
    uint8      ReservedCE;
    char       Reserved0;
    uint       ScriptId;
    uint       ShowCritterDist1;
    uint       ShowCritterDist2;
    uint       ShowCritterDist3;
    uint16     Reserved00;
    short      Multihex;
    uint       GlobalGroupUid;
    uint16     LastHexX;
    uint16     LastHexY;
    uint       Reserved1[4];
    uint       MapId;
    uint16     MapPid;
    uint16     Reserved2;
    int        Params[MAX_PARAMS];
    uint       Anim1Life;
    uint       Anim1Knockout;
    uint       Anim1Dead;
    uint       Anim2Life;
    uint       Anim2Knockout;
    uint       Anim2Dead;
    uint       Anim2KnockoutEnd;
    uint       Reserved3[3];
    char       Lexems[LEXEMS_SIZE];
    uint       Reserved4[8];
    bool       ClientToDelete;
    uint8      Reserved5;
    uint16     Reserved6;
    uint       Temp;
    uint16     Reserved8;
    uint16     HoloInfoCount;
    uint       HoloInfo[MAX_HOLO_INFO];
    uint       Reserved9[10];
    int        Scores[SCORES_MAX];
    uint8      UserData[CRITTER_USER_DATA_SIZE];
    // Npc data
    uint       HomeMap;
    uint16     HomeX;
    uint16     HomeY;
    uint8      HomeOri;
    uint8      Reserved11;
    uint16     ProtoId;
    uint       Reserved12;
    uint       Reserved13;
    uint       Reserved14;
    uint       Reserved15;
    bool       IsDataExt;
    uint8      Reserved16;
    uint16     Reserved17;
    uint       Reserved18[8];
    uint16     FavoriteItemPid[4];
    uint       Reserved19[10];
    uint       EnemyStackCount;
    uint       EnemyStack[MAX_ENEMY_STACK];
    uint       Reserved20[5];
    uint8      BagCurrentSet[MAX_NPC_BAGS_PACKS];
    short      BagRefreshTime;
    uint8      Reserved21;
    uint8      BagSize;
    NpcBagItem Bag[MAX_NPC_BAGS];
    uint       Reserved22[100];
};

struct CritDataExt
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

#endif // __CRITTER_DATA__
