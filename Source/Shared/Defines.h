#ifndef __DEFINES__
#define __DEFINES__

#include "PlatformSpecific.h"
#include "Defines.Public.h"

// Aliases

#ifndef FO_WINDOWS
# define ExitProcess( code )                  exit( code )
#endif

// Flags
#define FLAGS( x, y )                         ( ( (x) & (y) ) == y )

#define BIN32( bin32, bin24, bin16, bin8 )    ( (BIN8( bin32 ) << 24) | (BIN8( bin24 ) << 16) | (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )

#define SAFEREL( x ) \
    { if( x )        \
          (x)->Release(); (x) = NULL; }
#define SAFEDEL( x ) \
    { if( x )        \
          delete (x); (x) = NULL; }
#define SAFEDELA( x ) \
    { if( x )         \
          delete[] (x); (x) = NULL; }

// legacy comments (by cvet)
#define MESSAGE( msg )                        message( __FILE__ "(" ___TOD0( __LINE__ ) "): " # msg )

// World dump versions
#define WORLD_SAVE_V1                         (1)  // legacy
#define WORLD_SAVE_LAST                       (WORLD_SAVE_V1)
#define SINGLEPLAYER_SAVE_V1                  (1)  // legacy
#define SINGLEPLAYER_SAVE_LAST                (SINGLEPLAYER_SAVE_V1)
// Client save
#define CLIENT_SAVE_V1                        (1)  // legacy
#define CLIENT_SAVE_LAST                      (CLIENT_SAVE_V1)

// Generic
#define CRAFT_SEND_TIME                       (60000)
#define AMBIENT_SOUND_TIME                    (60000)  // Random(X/2,X);
#define EFFECT_TEXTURES                       (10)
#define EFFECT_SCRIPT_VALUES                  (10)
#define ABC_SIZE                              (26)
#define AGGRESSOR_TICK                        (60000)
#define MAX_ENEMY_STACK                       (30)
#define MAX_STORED_IP                         (20)

// Items
#define MAX_ADDED_NOGROUP_ITEMS               (30)
#define ITEM_SLOT_BEGIN                       (1000)
#define ITEM_SLOT_END                         (1099)
#define ITEM_DEF_SLOT                         (1000)
#define ITEM_DEF_ARMOR                        (1100)
#define UNARMED_PUNCH                         (1000)
#define UNARMED_KICK                          (1020)

// SceneryCl flags
#define SCEN_CAN_USE                          (0x01)
#define SCEN_CAN_TALK                         (0x02)

// Maps
#define MAX_HEX_OFFSET                        (50)     // Must be not odd

#define TIME_CAN_FOLLOW_GM                    (20000)  // Can less than Map timeout

// Proto maps
#define MAP_PROTO_EXT                         ".fomap"

// Sendmap info
#define SENDMAP_TILES                         BIN8( 00000001 )
#define SENDMAP_WALLS                         BIN8( 00000010 )
#define SENDMAP_SCENERY                       BIN8( 00000100 )

// Ping
#define PING_PING                             (0)
#define PING_WAIT                             (1)
#define PING_CLIENT                           (2)
#define PING_UID_FAIL                         (3)

#define MAX_DLGBOX_BUTTONS                    (20)

// Pick types
#define PICK_CRIT_LOOT                        (0)
#define PICK_CRIT_PUSH                        (1)

// Craft results
#define CRAFT_RESULT_NONE                     (0)
#define CRAFT_RESULT_SUCC                     (1)
#define CRAFT_RESULT_FAIL                     (2)
#define CRAFT_RESULT_TIMEOUT                  (3)

// Critters
#define CRITTER_INV_VOLUME                    (1000)

// Global map
#define GM_MAXX                               (GameOpt.GlobalMapWidth * GameOpt.GlobalMapZoneLength)
#define GM_MAXY                               (GameOpt.GlobalMapHeight * GameOpt.GlobalMapZoneLength)
#define GM_ZONE_LEN                           (GameOpt.GlobalMapZoneLength)  // Can be multiple to GM_MAXX and GM_MAXY
#define GM__MAXZONEX                          (100)
#define GM__MAXZONEY                          (100)
#define GM_ZONES_FOG_SIZE                     ( ( (GM__MAXZONEX / 4) + ( (GM__MAXZONEX % 4) ? 1 : 0 ) ) * GM__MAXZONEY )
#define GM_MAX_GROUP_COUNT                    (GameOpt.WorldmapMaxGroupCount)
#define GM_ANSWER_WAIT_TIME                   (20000)
#define GM_LIGHT_TIME                         (5000)
#define GM_ZONE( x )                          ( (x) / GM_ZONE_LEN )
#define GM_ENTRANCES_SEND_TIME                (60000)
#define GM_TRACE_TIME                         (1000)

// Follow
#define FOLLOW_DIST                           (10)
#define FOLLOW_FORCE                          (1)
#define FOLLOW_PREP                           (2)

// GM Info
#define GM_INFO_LOCATIONS                     (0x01)
#define GM_INFO_CRITTERS                      (0x02)
#define GM_INFO_GROUP_PARAM                   (0x04)
#define GM_INFO_ZONES_FOG                     (0x08)
#define GM_INFO_ALL                           (0x0F)
#define GM_INFO_FOG                           (0x10)
#define GM_INFO_LOCATION                      (0x20)

// GM Rule command
#define GM_CMD_SETMOVE                        (1)  // +r-a*x,y
#define GM_CMD_STOP                           (2)  // +r-a
#define GM_CMD_TOLOCAL                        (3)  // +r-a*num_city,num_map
#define GM_CMD_KICKCRIT                       (4)  // +r-a*cr_id
#define GM_CMD_FOLLOW_CRIT                    (5)  // +r+a*cr_id
#define GM_CMD_FOLLOW                         (6)
#define GM_CMD_GIVE_RULE                      (7)
#define GM_CMD_ANSWER                         (8)
#define GM_CMD_ENTRANCES                      (9)
#define GM_CMD_VIEW_MAP                       (10)

// Client map
#define SERVER_MAP_EXT                        ".map"
#define CLIENT_MAP_FORMAT_VER                 (7)

// Coordinates
#define MAXHEX_DEF                            (200)
#define MAXHEX_MIN                            (10)
#define MAXHEX_MAX                            (10000)

// Client parameters
#define MAX_CHAT_MESSAGE                      (100)
#define MAX_SAY_NPC_TEXT                      (25)
#define MAX_SCENERY                           (5000)
#define MAX_DIALOG_TEXT                       (MAX_FOTEXT)
#define MAX_DLG_LEN_IN_BYTES                  (64 * 1024)
#define MAX_DLG_LEXEMS_TEXT                   (1000)
#define MAX_BUF_LEN                           (4096)
#define PASS_HASH_SIZE                        (32)

// Critters
#define MAX_CRIT_TYPES                        (1000)

#define MAX_ANSWERS                           (100)
#define PROCESS_TALK_TICK                     (1000)
#define FILENAME_DIALOGS_CONFIG               "dialogs.cfg"

#define FILENAME_SCRIPTS_CONFIG               "scripts.cfg"
#define MAX_START_SPECIAL                     (40)
#define TURN_BASED_TIMEOUT                    (1000)
#define MIN_VISIBLE_CRIT                      (6)
#define FADING_PERIOD                         (1000)

#define RESPOWN_TIME_PLAYER                   (3)
#define RESPOWN_TIME_NPC                      (120)
#define RESPOWN_TIME_INFINITY                 (4 * 24 * 60 * 60000)

#define MODE_WIDTH                            GAME_OPTION( ScreenWidth )
#define MODE_HEIGHT                           GAME_OPTION( ScreenHeight )

// Turn based
#define COMBAT_TB_END_TURN                    (0)
#define COMBAT_TB_END_COMBAT                  (1)

// Answer
#define ANSWER_BEGIN                          (0xF0)
#define ANSWER_END                            (0xF1)
#define ANSWER_BARTER                         (0xF2)

// Time AP
#define AP_DIVIDER                            (100)

// Players barter
#define BARTER_DIST                           (1)
// Types
#define BARTER_TRY                            (0)  // opponentId, isHide
#define BARTER_ACCEPTED                       (1)  // opponentId, isHide
#define BARTER_BEGIN                          (2)
#define BARTER_END                            (3)
#define BARTER_SET_SELF                       (4)
#define BARTER_SET_OPPONENT                   (5)
#define BARTER_UNSET_SELF                     (6)
#define BARTER_UNSET_OPPONENT                 (7)
#define BARTER_OFFER                          (8)  // isSet, isOpponent
#define BARTER_REFRESH                        (9)

#define SCORES_SEND_TIME                      (60000)
#define SCORE_NAME_LEN                        (64)

// Parameters

#define SKILL_OFFSET( skill )                 ( (skill) - (GameOpt.AbsoluteOffsets ? 0 : SKILL_BEGIN) )
#define PERK_OFFSET( perk )                   ( (perk) - (GameOpt.AbsoluteOffsets ? 0 : PERK_BEGIN) )

// Special send params
#define OTHER_BREAK_TIME                      (0 + MAX_PARAMS)
#define OTHER_WAIT_TIME                       (1 + MAX_PARAMS)
#define OTHER_FLAGS                           (2 + MAX_PARAMS)
#define OTHER_BASE_TYPE                       (3 + MAX_PARAMS)
#define OTHER_MULTIHEX                        (4 + MAX_PARAMS)
#define OTHER_YOU_TURN                        (5 + MAX_PARAMS)
#define OTHER_CLEAR_MAP                       (6 + MAX_PARAMS)
#define OTHER_TELEPORT                        (7 + MAX_PARAMS)

// Move params
// 6 next steps (each 5 bit) + stop bit + run bit
// Step bits: 012 - dir, 3 - allow, 4 - disallow
#define MOVE_PARAM_STEP_COUNT                 (6)
#define MOVE_PARAM_STEP_BITS                  (5)
#define MOVE_PARAM_STEP_DIR                   (0x7)
#define MOVE_PARAM_STEP_ALLOW                 (0x8)
#define MOVE_PARAM_STEP_DISALLOW              (0x10)
#define MOVE_PARAM_RUN                        (0x80000000)

// Holodisks
#define USER_HOLO_TEXTMSG_FILE                "FOHOLOEXT.MSG"
#define USER_HOLO_START_NUM                   (100000)
#define USER_HOLO_MAX_TITLE_LEN               (40)
#define USER_HOLO_MAX_LEN                     (2000)

#endif  // __DEFINES__ //
