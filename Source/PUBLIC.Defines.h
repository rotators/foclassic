/////
//
// Magic Numbers
//
/////
//
// This file contains all exposed 'magic numbers' used in FOClassic.
// It should not be edited directly by servers.
//
// Used by engine, extensions, and scripts.
//
///// Optional defines
//
// FOCLASSIC_EXTENSION
// Enables macros, C++ syntax; cannot be used with FOCLASSIC_SCRIPT
//
// FOCLASSIC_SCRIPT
// Enables macros, AngelScript syntax; cannot be used with FOCLASSIC_EXTENSION
//
// FOCLASSIC_BLEEDING_EDGE
// All values which are planned for removal/renaming in future won't be defined.
//
// FOCLASSIC_SKIP_PARAM
// Critter parameters indexes won't be defined.
//
// FOCLASSIC_SKIP_PID
// Prototype item IDs won't be defined.
//
/////

#ifndef __FOCLASSIC_FOS__
#define __FOCLASSIC_FOS__

#ifdef FOCLASSIC_ENGINE
# define FOCLASSIC_EXTENSION
# define FOCLASSIC_BLEEDING_EDGE
#endif

#ifdef FOCLASSIC_EXTENSION
# ifdef FOCLASSIC_SCRIPT
#  error "FOCLASSIC_EXTENSION and FOCLASSIC_SCRIPT cannot be both defined at same time"
# endif
#endif

#ifdef FOCLASSIC_EXTENSION
# define FOCLASSIC_MACROS_ALLOWED
#endif
#ifdef FOCLASSIC_SCRIPT
# define FOCLASSIC_MACROS_ALLOWED
#endif

///// Macros /////

#ifdef FOCLASSIC_MACROS_ALLOWED

// macros for engine/extensions only
# ifdef FOCLASSIC_EXTENSION
#  define memzero( ptr, size )                    memset( ptr, 0, size )
#  define MAKEUINT( ch0, ch1, ch2, ch3 )          ( (uint)(uchar)(ch0) | ( (uint)(uchar)(ch1) << 8 ) | ( (uint)(uchar)(ch2) << 16 ) | ( (uint)(uchar)(ch3) << 24 ) )
#  define OFFSETOF( type, member )                ( (int)offsetof( type, member ) )
#  define PACKUINT64( u32hi, u32lo )              ( ( (uint64)u32hi << 32 ) | ( (uint64)u32lo ) )
#  define STATIC_ASSERT( a )                      static_assert( a, # a )
# endif

// macros for everyone
# ifdef FOCLASSIC_EXTENSION
#  define BIN__N( x )                             (x) | x >> 3 | x >> 6 | x >> 9
#  define BIN__B( x )                             (x) & 0xf | (x) >> 12 & 0xf0
#  define BIN8( v )                               (BIN__B( BIN__N( 0x ## v ) ) )
#  define BIN16( bin16, bin8 )                    ( (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )
#  define BIN32( bin32, bin24, bin16, bin8 )      ( (BIN8( bin32 ) << 24) | (BIN8( bin24 ) << 16) | (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )

#  define MAX( a, b )                             ( ( (a) > (b) ) ? (a) : (b) )
#  define MIN( a, b )                             ( ( (a) < (b) ) ? (a) : (b) )
#  define IS_DIR_CORNER( dir )                    ( ( (dir) & 1 ) != 0 )  // 1, 3, 5, 7
#  define UTF8_BUF_SIZE( count )                  ( (count) * 4 )
# endif  // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define BIN__N                                 # (x)(x) | x >> 3 | x >> 6 | x >> 9
#  define BIN__B                                 # (x)(x) & 0xf | (x) >> 12 & 0xf0
#  define BIN8                                   # (v)(BIN__B( BIN__N( 0x ## v ) ) )
#  define BIN16                                  # (bin16, bin8)( (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )

#  define MAX                                    # (a, b)( ( (a) > (b) ) ? (a) : (b) )
#  define MIN                                    # (a, b)( ( (a) < (b) ) ? (a) : (b) )
#  define IS_DIR_CORNER                          # ( ( (dir) & 1 ) != 0 )  // 1, 3, 5, 7
#  define UTF8_BUF_SIZE                          # (count)( (count) * 4 )
# endif  // FOCLASSIC_SCRIPT

#endif  // FOCLASSIC_MACROS_ALLOWED

//// Misc /////
#define PI_FLOAT                                 (3.14159265f)
#define PIBY2_FLOAT                              (1.5707963f)
#define SQRT3T2_FLOAT                            (3.4641016151f)
#define SQRT3_FLOAT                              (1.732050807568877f)
#define BIAS_FLOAT                               (0.02f)
#define RAD2DEG                                  (57.29577951f)

///// Limits /////

#define MIN_INT                                  (0x80000000)
#define MAX_UINT8                                (0xFF)
#define MAX_UINT16                               (0xFFFF)
#define MAX_UINT                                 (0xFFFFFFFF)
#define MAX_INT                                  (0x7FFFFFFF)

#define MAX_DETERIORATION                        (10000)

// Used for name and password
#define MIN_NAME                                 (1)
#define MAX_NAME                                 (30)

#define MAX_PARAMS                               (1000)  // array size

#define MAX_PROTO_CRITTERS                       (10000)
#define MAX_PROTO_ITEMS                          (30000)
#define MAX_PROTO_LOCATIONS                      (30000)
#define MAX_PROTO_MAPS                           (30000)

#define MAX_SCRIPT_NAME                          (64)

#define MAX_STORED_LOCATIONS                     (1000)

#define MIN_ZOOM                                 (0.2f)
#define MAX_ZOOM                                 (10.0f)

#ifdef FOCLASSIC_MACROS_ALLOWED

# define MAX_FOPATH                              UTF8_BUF_SIZE( 1024 )
# define MAX_LOGTEXT                             UTF8_BUF_SIZE( 2048 )
# define MAX_FOTEXT                              UTF8_BUF_SIZE( 2048 )

#endif // FOCLASSIC_MACROS_ALLOWED

///// Magic /////

// Access levels
#define ACCESS_CLIENT                            (0)
#define ACCESS_TESTER                            (1)
#define ACCESS_MODER                             (2)
#define ACCESS_ADMIN                             (3)
#define ACCESS_LAST                              (ACCESS_ADMIN) // scripts/extensions

#ifdef FOCLASSIC_ENGINE
# ifdef DEV_VERSION
#  define ACCESS_DEFAULT                         (ACCESS_ADMIN)
# else
#  define ACCESS_DEFAULT                         (ACCESS_CLIENT)
# endif
#endif

// Critter age
#define AGE_MAX                                  (60)
#define AGE_MIN                                  (14)

// Item deterioration info
#define BI_BROKEN                                (0x0F)

// Binary files signatures
#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define BINARY_SIGNATURE( name, type, ver )     const unsigned char name[6] = { 'F', 'O', type, ( (ver) >> 8 ) & 0xFF, ( (ver) ) & 0xFF, 0 }
#  define BINARY_SIGNATURE_VALID( sig1, sig2 )    (sig1[0] == sig2[0] && sig1[1] == sig2[1] && sig1[2] == sig2[2] && sig1[5] == sig2[5])                 // skip version check
#  define BINARY_SIGNATURE_VERSION( sig )         ( (sig[3] << 8) | sig[4] )
# endif  // FOCLASSIC_EXTENSION

#endif  // FOCLASSIC_MACROS_ALLOWED

#define BINARY_CLIENTSAVE                        'C'
#define BINARY_MAPSAVE                           'M'
#define BINARY_PROFILERSAVE                      'P'
#define BINARY_SCRIPTSAVE                        'S'
#define BINARY_WORLDSAVE                         'W'
#define BINARY_CACHE                             'c'   // reserved

// Commands
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] renumber COMMAND")
# endif
#endif
#define COMMAND_EXIT                             (1)
#define COMMAND_MYINFO                           (2)
#define COMMAND_GAMEINFO                         (3)
#define COMMAND_CRITID                           (4)
#define COMMAND_MOVECRIT                         (5)
#define COMMAND_KILLCRIT                         (6)
#define COMMAND_DISCONCRIT                       (7)
#define COMMAND_TOGLOBAL                         (8)
#define COMMAND_RESPAWN                          (9)
#define COMMAND_PARAM                            (10)
#define COMMAND_GETACCESS                        (11)
#define COMMAND_ADDITEM                          (12)
#define COMMAND_ADDITEM_SELF                     (14)
#define COMMAND_ADDNPC                           (15)
#define COMMAND_ADDLOCATION                      (16)
#define COMMAND_RELOADSCRIPTS                    (17)  // investigate
#define COMMAND_LOADSCRIPT                       (18)
#define COMMAND_RELOAD_CLIENT_SCRIPTS            (19)
#define COMMAND_RUNSCRIPT                        (20)
#define COMMAND_RELOADLOCATIONS                  (21)  // investigate
#define COMMAND_LOADLOCATION                     (22)
#define COMMAND_RELOADMAPS                       (23)  // investigate
#define COMMAND_LOADMAP                          (24)
#define COMMAND_REGENMAP                         (25)
#define COMMAND_RELOADDIALOGS                    (26)  // investigate
#define COMMAND_LOADDIALOG                       (27)
#define COMMAND_RELOADTEXTS                      (28)  // investigate
#define COMMAND_RELOADAI                         (29)  // investigate
#define COMMAND_CHECKVAR                         (30)
#define COMMAND_SETVAR                           (31)
#define COMMAND_SETTIME                          (32)
#define COMMAND_BAN                              (33)
#define COMMAND_DELETE_ACCOUNT                   (34)
#define COMMAND_CHANGE_PASSWORD                  (35)
#define COMMAND_DROP_UID                         (36)
#define COMMAND_LOG                              (37)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate CMD")
# endif
#endif
#ifndef FOCLASSIC_BLEEDING_EDGE
# define CMD_EXIT                                (COMMAND_EXIT)
# define CMD_MYINFO                              (COMMAND_MYINFO)
# define CMD_GAMEINFO                            (COMMAND_GAMEINFO)
# define CMD_CRITID                              (COMMAND_CRITID)
# define CMD_MOVECRIT                            (COMMAND_MOVECRIT)
# define CMD_KILLCRIT                            (COMMAND_KILLCRIT)
# define CMD_DISCONCRIT                          (COMMAND_DISCONCRIT)
# define CMD_TOGLOBAL                            (COMMAND_TOGLOBAL)
# define CMD_RESPAWN                             (COMMAND_RESPAWN)
# define CMD_PARAM                               (COMMAND_PARAM)
# define CMD_GETACCESS                           (COMMAND_GETACCESS)
# define CMD_ADDITEM                             (COMMAND_ADDITEM)
# define CMD_ADDITEM_SELF                        (COMMAND_ADDITEM_SELF)
# define CMD_ADDNPC                              (COMMAND_ADDNPC)
# define CMD_ADDLOCATION                         (COMMAND_ADDLOCATION)
# define CMD_RELOADSCRIPTS                       (COMMAND_RELOADSCRIPTS)
# define CMD_LOADSCRIPT                          (COMMAND_LOADSCRIPT)
# define CMD_RELOAD_CLIENT_SCRIPTS               (COMMAND_RELOAD_CLIENT_SCRIPTS)
# define CMD_RUNSCRIPT                           (COMMAND_RUNSCRIPT)
# define CMD_RELOADLOCATIONS                     (COMMAND_RELOADLOCATIONS)
# define CMD_LOADLOCATION                        (COMMAND_LOADLOCATION)
# define CMD_RELOADMAPS                          (COMMAND_RELOADMAPS)
# define CMD_LOADMAP                             (COMMAND_LOADMAP)
# define CMD_REGENMAP                            (COMMAND_REGENMAP)
# define CMD_RELOADDIALOGS                       (COMMAND_RELOADDIALOGS)
# define CMD_LOADDIALOG                          (COMMAND_LOADDIALOG)
# define CMD_RELOADTEXTS                         (COMMAND_RELOADTEXTS)
# define CMD_RELOADAI                            (COMMAND_RELOADAI)
# define CMD_CHECKVAR                            (COMMAND_CHECKVAR)
# define CMD_SETVAR                              (COMMAND_SETVAR)
# define CMD_SETTIME                             (COMMAND_SETTIME)
# define CMD_BAN                                 (COMMAND_BAN)
# define CMD_DELETE_ACCOUNT                      (COMMAND_DELETE_ACCOUNT)
# define CMD_CHANGE_PASSWORD                     (COMMAND_CHANGE_PASSWORD)
# define CMD_DROP_UID                            (COMMAND_DROP_UID)
# define CMD_LOG                                 (COMMAND_LOG)
#endif  // !FOCLASSIC_BLEEDING_EDGE

// Combat modes
#define COMBAT_MODE_ANY                          (0)
#define COMBAT_MODE_REAL_TIME                    (1)
#define COMBAT_MODE_TURN_BASED                   (2)

// Constants collections
#define CONSTANTS_PARAM                          (0)
#define CONSTANTS_ITEM                           (1)
#define CONSTANTS_DEFINE                         (2)
#define CONSTANTS_PICTURE                        (3)
#define CONSTANTS_HASH                           (4)

// Take flags
#define CONTAINER_GET                            (1)
#define CONTAINER_PUT                            (2)
#define CONTAINER_GETALL                         (3)
#define CONTAINER_PUTALL                         (4)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate CONT")
# endif
#endif
#ifndef FOCLASSIC_BLEEDING_EDGE
# define CONT_GET                                (CONTAINER_GET)
# define CONT_PUT                                (CONTAINER_PUT)
# define CONT_GETALL                             (CONTAINER_GETALL)
# define CONT_PUTALL                             (CONTAINER_PUTALL)
#endif  // !FOCLASSIC_BLEEDING_EDGE

// Corner type
#define CORNER_NORTH_SOUTH                       (0)
#define CORNER_WEST                              (1)
#define CORNER_EAST                              (2)
#define CORNER_SOUTH                             (3)
#define CORNER_NORTH                             (4)
#define CORNER_EAST_WEST                         (5)

// Critter conditions
#define CRITTER_CONDITION_LIFE                   (1)
#define CRITTER_CONDITION_KNOCKOUT               (2)
#define CRITTER_CONDITION_DEAD                   (3)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate COND")
# endif
#endif
#ifndef FOCLASSIC_BLEEDING_EDGE
# define COND_LIFE                               (CRITTER_CONDITION_LIFE)
# define COND_KNOCKOUT                           (CRITTER_CONDITION_KNOCKOUT)
# define COND_DEAD                               (CRITTER_CONDITION_DEAD)
#endif  // !FOCLASSIC_BLEEDING_EDGE

// Critter events
#define CRITTER_EVENT_IDLE                       (0)
#define CRITTER_EVENT_FINISH                     (1)
#define CRITTER_EVENT_DEAD                       (2)
#define CRITTER_EVENT_RESPAWN                    (3)
#define CRITTER_EVENT_SHOW_CRITTER               (4)
#define CRITTER_EVENT_SHOW_CRITTER_1             (5)
#define CRITTER_EVENT_SHOW_CRITTER_2             (6)
#define CRITTER_EVENT_SHOW_CRITTER_3             (7)
#define CRITTER_EVENT_HIDE_CRITTER               (8)
#define CRITTER_EVENT_HIDE_CRITTER_1             (9)
#define CRITTER_EVENT_HIDE_CRITTER_2             (10)
#define CRITTER_EVENT_HIDE_CRITTER_3             (11)
#define CRITTER_EVENT_SHOW_ITEM_ON_MAP           (12)
#define CRITTER_EVENT_CHANGE_ITEM_ON_MAP         (13)
#define CRITTER_EVENT_HIDE_ITEM_ON_MAP           (14)
#define CRITTER_EVENT_ATTACK                     (15)
#define CRITTER_EVENT_ATTACKED                   (16)
#define CRITTER_EVENT_STEALING                   (17)
#define CRITTER_EVENT_MESSAGE                    (18)
#define CRITTER_EVENT_USE_ITEM                   (19)
#define CRITTER_EVENT_USE_ITEM_ON_ME             (20)
#define CRITTER_EVENT_USE_SKILL                  (21)
#define CRITTER_EVENT_USE_SKILL_ON_ME            (22)
#define CRITTER_EVENT_DROP_ITEM                  (23)
#define CRITTER_EVENT_MOVE_ITEM                  (24)
#define CRITTER_EVENT_KNOCKOUT                   (25)
#define CRITTER_EVENT_SMTH_DEAD                  (26)
#define CRITTER_EVENT_SMTH_STEALING              (27)
#define CRITTER_EVENT_SMTH_ATTACK                (28)
#define CRITTER_EVENT_SMTH_ATTACKED              (29)
#define CRITTER_EVENT_SMTH_USE_ITEM              (30)
#define CRITTER_EVENT_SMTH_USE_SKILL             (31)
#define CRITTER_EVENT_SMTH_DROP_ITEM             (32)
#define CRITTER_EVENT_SMTH_MOVE_ITEM             (33)
#define CRITTER_EVENT_SMTH_KNOCKOUT              (34)
#define CRITTER_EVENT_PLANE_BEGIN                (35)
#define CRITTER_EVENT_PLANE_END                  (36)
#define CRITTER_EVENT_PLANE_RUN                  (37)
#define CRITTER_EVENT_BARTER                     (38)
#define CRITTER_EVENT_TALK                       (39)
#define CRITTER_EVENT_GLOBAL_PROCESS             (40)
#define CRITTER_EVENT_GLOBAL_INVITE              (41)
#define CRITTER_EVENT_TURN_BASED_PROCESS         (42)
#define CRITTER_EVENT_SMTH_TURN_BASED_PROCESS    (43)
#define CRITTER_EVENT_MAX                        (44)

// Critters flags
#define CRITTER_FLAG_PLAYER                      (0x00010000)  // Player
#define CRITTER_FLAG_NPC                         (0x00020000)  // Npc
#define CRITTER_FLAG_DISCONNECT                  (0x00080000)  // In offline
#define CRITTER_FLAG_CHOSEN                      (0x00100000)  // Chosen
#define CRITTER_FLAG_RULEGROUP                   (0x00200000)  // Group rule
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate FCRIT")
# endif
#endif
#ifndef FOCLASSIC_BLEEDING_EDGE
# define FCRIT_PLAYER                            (CRITTER_FLAG_PLAYER)
# define FCRIT_NPC                               (CRITTER_FLAG_NPC)
# define FCRIT_DISCONNECT                        (CRITTER_FLAG_DISCONNECT)
# define FCRIT_CHOSEN                            (CRITTER_FLAG_CHOSEN)
# define FCRIT_RULEGROUP                         (CRITTER_FLAG_RULEGROUP)
#endif  // !FOCLASSIC_BLEEDING_EDGE

// Critters ID
#define CRITTER_ID_START_PLAYER                  (1)
#define CRITTER_ID_START_NPC                     (5000001)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate NPC_START_ID")
#  pragma MESSAGE("[STAGE 3] deprecate USER_START_ID")
# endif
#endif
#ifndef FOCLASSIC_BLEEDING_EDGE
# define NPC_START_ID                            (CRITTER_ID_START_NPC)
# define USER_START_ID                           (CRITTER_ID_START_PLAYER)
#endif  // !FOCLASSIC_BLEEDING_EDGE

#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define CRITTER_ID_IS_NPC( id )       ( (id) >= CRITTER_ID_START_NPC )
#  define CRITTER_ID_IS_PLAYER( id )    ( (id) >= CRITTER_ID_START_PLAYER && (id) < CRITTER_ID_START_NPC )
# endif  // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define CRITTER_ID_IS_NPC                      # (id)( (id) >= CRITTER_ID_START_NPC )
#  define CRITTER_ID_IS_PLAYER                   # (id)( (id) >= CRITTER_ID_START_PLAYER && (id) < CRITTER_ID_START_NPC )
# endif  // FOCLASSIC_SCRIPT

#endif   // FOCLASSIC_MACROS_ALLOWED

// Damage types
#define DAMAGE_TYPE_UNCALLED                     (0)
#define DAMAGE_TYPE_NORMAL                       (1)
#define DAMAGE_TYPE_LASER                        (2)
#define DAMAGE_TYPE_FIRE                         (3)
#define DAMAGE_TYPE_PLASMA                       (4)
#define DAMAGE_TYPE_ELECTR                       (5)
#define DAMAGE_TYPE_EMP                          (6)
#define DAMAGE_TYPE_EXPLODE                      (7)

// Entires
#define ENTIRE_DEFAULT                           (0)
#define ENTIRE_LOG_OUT                           (241)

// Critter find types
#define FIND_LIFE                                (0x01)
#define FIND_KO                                  (0x02)
#define FIND_DEAD                                (0x04)
#define FIND_ONLY_PLAYERS                        (0x10)
#define FIND_ONLY_NPC                            (0x20)
#ifndef FOCLASSIC_ENGINE
# define FIND_LIFE_AND_KO                        (FIND_LIFE | FIND_KO)
# define FIND_LIFE_AND_DEAD                      (FIND_LIFE | FIND_DEAD)
# define FIND_KO_AND_DEAD                        (FIND_KO | FIND_DEAD)
# define FIND_ALL                                (FIND_LIFE | FIND_KO | FIND_DEAD)
#endif

#define GENDER_MALE                              (0)
#define GENDER_FEMALE                            (1)
#define GENDER_IT                                (2)

// Grid Types
#define GRID_EXITGRID                            (1)
#define GRID_STAIRS                              (2)
#define GRID_LADDERBOT                           (3)
#define GRID_LADDERTOP                           (4)
#define GRID_ELEVATOR                            (5)

// Hex flags
// ProtoMap
#ifdef FOCLASSIC_MACROS_ALLOWED

# define HEX_FLAG_BLOCK                          BIN8( 00000001 )
# define HEX_FLAG_NOTRAKE                        BIN8( 00000010 )
# define HEX_FLAG_WALL                           BIN8( 00000100 )
# define HEX_FLAG_SCEN                           BIN8( 00001000 )
# define HEX_FLAG_SCEN_GRID                      BIN8( 00010000 )
# define HEX_FLAG_TRIGGER                        BIN8( 00100000 )
// Map
# define HEX_FLAG_CRITTER                        BIN8( 00000001 )
# define HEX_FLAG_DEAD_CRITTER                   BIN8( 00000010 )
# define HEX_FLAG_ITEM                           BIN8( 00000100 )
# define HEX_FLAG_DOOR                           BIN8( 00001000 )
# define HEX_FLAG_BLOCK_ITEM                     BIN8( 00010000 )
# define HEX_FLAG_NRAKE_ITEM                     BIN8( 00100000 )
# define HEX_FLAG_WALK_ITEM                      BIN8( 01000000 )
# define HEX_FLAG_GAG_ITEM                       BIN8( 10000000 )

# define HEX_FLAG_NOWAY                          BIN16( 00010001, 00000001 )
# define HEX_FLAG_NOSHOOT                        BIN16( 00100000, 00000010 )

#endif  // FOCLASSIC_MACROS_ALLOWED

#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("deprecate FH")
# endif
#endif
#ifndef FOCLASSIC_BLEEDING_EDGE
# define FH_BLOCK                                (HEX_FLAG_BLOCK)
# define FH_NOTRAKE                              (HEX_FLAG_NOTRAKE)
# define FH_WALL                                 (HEX_FLAG_WALL)
# define FH_SCEN                                 (HEX_FLAG_SCEN)
# define FH_SCEN_GRID                            (HEX_FLAG_SCEN_GRID)
# define FH_TRIGGER                              (HEX_FLAG_TRIGGER)
# define FH_CRITTER                              (HEX_FLAG_CRITTER)
# define FH_DEAD_CRITTER                         (HEX_FLAG_DEAD_CRITTER)
# define FH_ITEM                                 (HEX_FLAG_ITEM)
# define FH_DOOR                                 (HEX_FLAG_DOOR)
# define FH_BLOCK_ITEM                           (HEX_FLAG_BLOCK_ITEM)
# define FH_NRAKE_ITEM                           (HEX_FLAG_NRAKE_ITEM)
# define FH_WALK_ITEM                            (HEX_FLAG_WALK_ITEM)
# define FH_GAG_ITEM                             (HEX_FLAG_GAG_ITEM)
# define FH_NOWAY                                (HEX_FLAG_NOWAY)
# define FH_NOSHOOT                              (HEX_FLAG_NOSHOOT)
#endif  // !FOCLASSIC_BLEEDING_EDGE

// Hit locations
#define HIT_LOCATION_NONE                        (0)
#define HIT_LOCATION_HEAD                        (1)
#define HIT_LOCATION_LEFT_ARM                    (2)
#define HIT_LOCATION_RIGHT_ARM                   (3)
#define HIT_LOCATION_TORSO                       (4)
#define HIT_LOCATION_RIGHT_LEG                   (5)
#define HIT_LOCATION_LEFT_LEG                    (6)
#define HIT_LOCATION_EYES                        (7)
#define HIT_LOCATION_GROIN                       (8)
#define HIT_LOCATION_UNCALLED                    (9)
#define HIT_LOCATION_MAX                         (HIT_LOCATION_UNCALLED)

// IndicatorType
#define INDICATOR_LINES                          (0)
#define INDICATOR_NUMBERS                        (1)
#define INDICATOR_BOTH                           (2)

// Accessory
#define ITEM_ACCESSORY_NONE                      (0)
#define ITEM_ACCESSORY_CRITTER                   (1)
#define ITEM_ACCESSORY_HEX                       (2)
#define ITEM_ACCESSORY_CONTAINER                 (3)

// Item events
#define ITEM_EVENT_FINISH                        (0)
#define ITEM_EVENT_ATTACK                        (1)
#define ITEM_EVENT_USE                           (2)
#define ITEM_EVENT_USE_ON_ME                     (3)
#define ITEM_EVENT_SKILL                         (4)
#define ITEM_EVENT_DROP                          (5)
#define ITEM_EVENT_MOVE                          (6)
#define ITEM_EVENT_WALK                          (7)
#define ITEM_EVENT_MAX                           (ITEM_EVENT_WALK + 1)   // array size

// Item flags
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate ITEM_flag")
# endif
#endif
#define ITEM_HIDDEN                              (0x00000001)
#define ITEM_FLAT                                (0x00000002)
#define ITEM_NO_BLOCK                            (0x00000004)
#define ITEM_SHOOT_THRU                          (0x00000008)
#define ITEM_LIGHT_THRU                          (0x00000010)
#define ITEM_MULTI_HEX                           (0x00000020)   // unused
#define ITEM_WALL_TRANS_END                      (0x00000040)   // unused
#define ITEM_TWO_HANDS                           (0x00000080)
#define ITEM_BIG_GUN                             (0x00000100)
#define ITEM_ALWAYS_VIEW                         (0x00000200)
#define ITEM_HAS_TIMER                           (0x00000400)
#define ITEM_BAD_ITEM                            (0x00000800)
#define ITEM_NO_HIGHLIGHT                        (0x00001000)
#define ITEM_SHOW_ANIM                           (0x00002000)
#define ITEM_SHOW_ANIM_EXT                       (0x00004000)
#define ITEM_LIGHT                               (0x00008000)
#define ITEM_GECK                                (0x00010000)
#define ITEM_TRAP                                (0x00020000)
#define ITEM_NO_LIGHT_INFLUENCE                  (0x00040000)
#define ITEM_NO_LOOT                             (0x00080000)
#define ITEM_NO_STEAL                            (0x00100000)
#define ITEM_GAG                                 (0x00200000)
#define ITEM_COLORIZE                            (0x00400000)
#define ITEM_COLORIZE_INV                        (0x00800000)
#define ITEM_CAN_USE_ON_SMTH                     (0x01000000)
#define ITEM_CAN_LOOK                            (0x02000000)   // unused, investigate
#define ITEM_CAN_TALK                            (0x04000000)
#define ITEM_CAN_PICKUP                          (0x08000000)
#define ITEM_CAN_USE                             (0x10000000)
#define ITEM_HOLODISK                            (0x20000000)
#define ITEM_RADIO                               (0x40000000)
#define ITEM_CACHED                              (0x80000000)   // unused

// Item data masks
#define ITEM_DATA_MASK_CHOSEN                    (0)
#define ITEM_DATA_MASK_CRITTER                   (1)
#define ITEM_DATA_MASK_CRITTER_EXT               (2)
#define ITEM_DATA_MASK_CONTAINER                 (3)
#define ITEM_DATA_MASK_MAP                       (4)
#ifdef FOCLASSIC_ENGINE
# define ITEM_DATA_MASK_MAX                      (ITEM_DATA_MASK_MAP + 1)   // array size
#endif

// Types
#define ITEM_TYPE_OTHER                          (0)
#define ITEM_TYPE_ARMOR                          (1)
#define ITEM_TYPE_DRUG                           (2)   // stacked
#define ITEM_TYPE_WEAPON                         (3)   // combined
#define ITEM_TYPE_AMMO                           (4)   // stacked
#define ITEM_TYPE_MISC                           (5)   // combined
#define ITEM_TYPE_KEY                            (7)
#define ITEM_TYPE_CONTAINER                      (8)
#define ITEM_TYPE_DOOR                           (9)
#define ITEM_TYPE_GRID                           (10)
#define ITEM_TYPE_GENERIC                        (11)
#define ITEM_TYPE_WALL                           (12)
#define ITEM_TYPE_CAR                            (13)
#define ITEM_TYPE_LAST                           (ITEM_TYPE_CAR)
#ifdef FOCLASSIC_ENGINE
# define ITEM_TYPE_MAX                           (ITEM_TYPE_LAST + 1)   // array size
#endif

// Script events
#define LOCATION_EVENT_FINISH                    (0)
#define LOCATION_EVENT_ENTER                     (1)
#define LOCATION_EVENT_MAX                       (LOCATION_EVENT_ENTER + 1) // array size

// Locker
#define LOCKER_ISOPEN                            (0x01)
#define LOCKER_NOOPEN                            (0x10)

// Look checks
#define LOOK_CHECK_DIR                           (0x01)
#define LOOK_CHECK_SNEAK_DIR                     (0x02)
#define LOOK_CHECK_SNEAK_WEIGHT                  (0x04)
#define LOOK_CHECK_TRACE                         (0x08)
#define LOOK_CHECK_SCRIPT                        (0x10)
#define LOOK_CHECK_ITEM_SCRIPT                   (0x20)

// Script events
#define MAP_EVENT_FINISH                         (0)
#define MAP_EVENT_LOOP_0                         (1)
#define MAP_EVENT_LOOP_1                         (2)
#define MAP_EVENT_LOOP_2                         (3)
#define MAP_EVENT_LOOP_3                         (4)
#define MAP_EVENT_LOOP_4                         (5)
#define MAP_EVENT_IN_CRITTER                     (6)
#define MAP_EVENT_OUT_CRITTER                    (7)
#define MAP_EVENT_CRITTER_DEAD                   (8)
#define MAP_EVENT_TURN_BASED_BEGIN               (9)
#define MAP_EVENT_TURN_BASED_END                 (10)
#define MAP_EVENT_TURN_BASED_PROCESS             (11)
#define MAP_EVENT_MAX                            (MAP_EVENT_TURN_BASED_PROCESS + 1)   // aray size

// Map object types
#define MAP_OBJECT_CRITTER                       (0)
#define MAP_OBJECT_ITEM                          (1)
#define MAP_OBJECT_SCENERY                       (2)

// Material
#ifndef FOCLASSIC_ENGINE
# define MATERIAL_GLASS                          (0)
# define MATERIAL_METAL                          (1)
# define MATERIAL_PLASTIC                        (2)
# define MATERIAL_WOOD                           (3)
# define MATERIAL_DIRT                           (4)
# define MATERIAL_STONE                          (5)
# define MATERIAL_CEMENT                         (6)
# define MATERIAL_LEATHER                        (7)
#endif  // !FOCLASSIC_ENGINE

// Client and mapper paths
#define PT_ROOT                                  (0)
#define PT_DATA                                  (1)
#define PT_ART                                   (2)
#define PT_ART_CRITTERS                          (3)
#define PT_ART_INTRFACE                          (4)
#define PT_ART_INVEN                             (5)
#define PT_ART_ITEMS                             (6)
#define PT_ART_MISC                              (7)
#define PT_ART_SCENERY                           (8)
#define PT_ART_SKILLDEX                          (9)
#define PT_ART_SPLASH                            (10)
#define PT_ART_TILES                             (11)
#define PT_ART_WALLS                             (12)
#define PT_TEXTURES                              (13)
#define PT_EFFECTS                               (14)
#define PT_SND_MUSIC                             (16)
#define PT_SND_SFX                               (17)
#define PT_SCRIPTS                               (18)
#define PT_VIDEO                                 (19)
#define PT_TEXTS                                 (20)
#define PT_SAVE                                  (21)
#define PT_FONTS                                 (22)
#define PT_CACHE                                 (23)

// Server paths
#define PT_SERVER_ROOT                           (30)
#define PT_SERVER_DATA                           (31)
#define PT_SERVER_TEXTS                          (32)
#define PT_SERVER_DIALOGS                        (33)
#define PT_SERVER_MAPS                           (34)
#define PT_SERVER_PRO_ITEMS                      (35)
#define PT_SERVER_PRO_CRITTERS                   (36)
#define PT_SERVER_SCRIPTS                        (37)
#define PT_SERVER_SAVE                           (38)
#define PT_SERVER_CLIENTS                        (39)
#define PT_SERVER_BANS                           (40)
#define PT_SERVER_LOGS                           (41)
#define PT_SERVER_DUMPS                          (42)
#define PT_SERVER_PROFILER                       (43)

// Mapper paths
#define PT_MAPPER_ROOT                           (45)
#define PT_MAPPER_DATA                           (46)

// Slots
#define SLOT_INV                                 (0)
#define SLOT_HAND1                               (1)
#define SLOT_HAND2                               (2)
#define SLOT_ARMOR                               (3)
#define SLOT_GROUND                              (255)
#define SLOT_LAST                                (SLOT_ARMOR)

// Say types
#define SAY_NORM                                 (1)
#define SAY_NORM_ON_HEAD                         (2)
#define SAY_SHOUT                                (3)
#define SAY_SHOUT_ON_HEAD                        (4)
#define SAY_EMOTE                                (5)
#define SAY_EMOTE_ON_HEAD                        (6)
#define SAY_WHISP                                (7)
#define SAY_WHISP_ON_HEAD                        (8)
#define SAY_SOCIAL                               (9)
#define SAY_RADIO                                (10)
#define SAY_NETMSG                               (11)
#define SAY_DIALOG                               (12)
#define SAY_APPEND                               (13)
#define SAY_ENCOUNTER_ANY                        (14)
#define SAY_ENCOUNTER_RT                         (15)
#define SAY_ENCOUNTER_TB                         (16)
#define SAY_FIX_RESULT                           (17)
#define SAY_DIALOGBOX_TEXT                       (18)
#define SAY_SAY_TITLE                            (39)
#define SAY_SAY_TEXT                             (40)
#define SAY_FLASH_WINDOW                         (41)

#ifdef FOCLASSIC_MACROS_ALLOWED

// Max 20 buttons (0..19)

# ifdef FOCLASSIC_EXTENSION
#  define SAY_DIALOGBOX_BUTTON( b )     (19 + (b) )
# endif  // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define SAY_DIALOGBOX_BUTTON                   # (b)(19 + (b) )
# endif  // FOCLASSIC_SCRIPT

#endif   // FOCLASSIC_MACROS_ALLOWED

// Special skill values
#define SKILL_PICK_ON_GROUND                     (-1)
#define SKILL_PUT_CONT                           (-2)
#define SKILL_TAKE_CONT                          (-3)
#define SKILL_TAKE_ALL_CONT                      (-4)
#define SKILL_LOOT_CRITTER                       (-5)
#define SKILL_PUSH_CRITTER                       (-6)
#define SKILL_TALK                               (-7)

// Target types
#define TARGET_SELF                              (0)
#define TARGET_SELF_ITEM                         (1)
#define TARGET_CRITTER                           (2)
#define TARGET_ITEM                              (3)
#define TARGET_SCENERY                           (4)
#define TARGET_LAST                              (TARGET_SCENERY + 1) // scripts/extensions

#define TEXTMSG_TEXT                             (0)
#define TEXTMSG_DLG                              (1)
#define TEXTMSG_ITEM                             (2)
#define TEXTMSG_GAME                             (3)
#define TEXTMSG_GM                               (4)
#define TEXTMSG_COMBAT                           (5)
#define TEXTMSG_QUEST                            (6)
#define TEXTMSG_HOLO                             (7)
#define TEXTMSG_CRAFT                            (8)
#define TEXTMSG_INTERNAL                         (9)
#define TEXTMSG_MAX                              (TEXTMSG_INTERNAL + 1)  // array size

// Transfer types
#define TRANSFER_CLOSE                           (0)
#define TRANSFER_HEX_CONT_UP                     (1)
#define TRANSFER_HEX_CONT_DOWN                   (2)
#define TRANSFER_SELF_CONT                       (3)
#define TRANSFER_CRIT_LOOT                       (4)
#define TRANSFER_CRIT_STEAL                      (5)
#define TRANSFER_CRIT_BARTER                     (6)
#define TRANSFER_FAR_CONT                        (7)
#define TRANSFER_FAR_CRIT                        (8)

// Uses
#define USE_PRIMARY                              (0)
#define USE_SECONDARY                            (1)
#define USE_THIRD                                (2)
#define USE_RELOAD                               (3)
#define USE_USE                                  (4)
#define MAX_USES                                 (3)
#define USE_NONE                                 (15)

#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define MAKE_ITEM_MODE( use, aim )    ( ( ( (aim) << 4 ) | ( (use) & 0xF ) ) & 0xFF )
# endif  // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define MAKE_ITEM_MODE                         # (use, aim)( ( ( (aim) << 4 ) | ( (use) & 0xF ) ) & 0xFF )
# endif  // FOCLASSIC_SCRIPT

#endif   // FOCLASSIC_MACROS_ALLOWED

// Vars calc
#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define VAR_CALC_QUEST( tid, val )    ( (tid) * 1000 + (val) )
# endif // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define VAR_CALC_QUEST                         # (tid, val)( (tid) * 1000 + (val) )
# endif // FOCLASSIC_SCRIPT

#endif  // FOCLASSIC_MACROS_ALLOWED

// Vars types
#define VAR_TYPE_GLOBAL                          (0)
#define VAR_TYPE_LOCAL                           (1)
#define VAR_TYPE_UNICUM                          (2)
#define VAR_TYPE_LOCAL_LOCATION                  (3)
#define VAR_TYPE_LOCAL_MAP                       (4)
#define VAR_TYPE_LOCAL_ITEM                      (5)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate VAR_GLOBAL VAR_LOCAL VAR_UNICUM")
# endif
#endif  // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define VAR_GLOBAL                              (VAR_TYPE_GLOBAL)
# define VAR_LOCAL                               (VAR_TYPE_LOCAL)
# define VAR_UNICUM                              (VAR_TYPE_UNICUM)
# define VAR_LOCAL_LOCATION                      (VAR_TYPE_LOCAL_LOCATION)
# define VAR_LOCAL_MAP                           (VAR_TYPE_LOCAL_MAP)
# define VAR_LOCAL_ITEM                          (VAR_TYPE_LOCAL_ITEM)
#endif  // !FOCLASSIC_BLEEDING_EDGE

// Var flags
#define VAR_FLAG_QUEST                           (0x01)
#define VAR_FLAG_RANDOM                          (0x02)
#define VAR_FLAG_NO_CHECK                        (0x04)

// Worldmap fog
#define WORLDMAP_FOG_FULL                        (0)
#define WORLDMAP_FOG_HALF                        (1)
#define WORLDMAP_FOG_HALF_EX                     (2)
#define WORLDMAP_FOG_NONE                        (3)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate GM_FOG")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define GM_FOG_FULL                             (WORLDMAP_FOG_FULL)
# define GM_FOG_HALF                             (WORLDMAP_FOG_HALF)
# define GM_FOG_HALF_EX                          (WORLDMAP_FOG_HALF_EX)
# define GM_FOG_NONE                             (WORLDMAP_FOG_NONE)
#endif

// Worldmap events
#define WORLDMAP_PROCESS_MOVE                    (0)
#define WORLDMAP_PROCESS_ENTER                   (1)
#define WORLDMAP_PROCESS_START_FAST              (2)
#define WORLDMAP_PROCESS_START                   (3)
#define WORLDMAP_PROCESS_SET_MOVE                (4)
#define WORLDMAP_PROCESS_STOPPED                 (5)
#define WORLDMAP_PROCESS_NPC_IDLE                (6)
#define WORLDMAP_PROCESS_KICK                    (7)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("deprecate GLOBAL_PROCESS")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define GLOBAL_PROCESS_MOVE                     (WORLDMAP_PROCESS_MOVE)
# define GLOBAL_PROCESS_ENTER                    (WORLDMAP_PROCESS_ENTER)
# define GLOBAL_PROCESS_START_FAST               (WORLDMAP_PROCESS_START_FAST)
# define GLOBAL_PROCESS_START                    (WORLDMAP_PROCESS_START)
# define GLOBAL_PROCESS_SET_MOVE                 (WORLDMAP_PROCESS_SET_MOVE)
# define GLOBAL_PROCESS_STOPPED                  (WORLDMAP_PROCESS_STOPPED)
# define GLOBAL_PROCESS_NPC_IDLE                 (WORLDMAP_PROCESS_NPC_IDLE)
# define GLOBAL_PROCESS_KICK                     (WORLDMAP_PROCESS_KICK)
#endif // !FOCLASSIC_BLEEDING_EDGE

// Worldmap walk types
#define WORLDMAP_WALK_GROUND                     (0)
#define WORLDMAP_WALK_FLY                        (1)
#define WORLDMAP_WALK_WATER                      (2)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate GM")
# endif
#endif
#ifndef FOCLASSIC_BLEEDING_EDGE
# define GM_WALK_GROUND                          (WORLDMAP_WALK_GROUND)
# define GM_WALK_FLY                             (WORLDMAP_WALK_FLY)
# define GM_WALK_WATER                           (WORLDMAP_WALK_WATER)
#endif // !FOCLASSIC_BLEEDING_EDGE

///// Items //////

#ifndef FOCLASSIC_SKIP_PID

# define PID_BOTTLE_CAPS                         (41)

#endif // !FOCLASSIC_SKIP_PID

///// Parameters /////

#ifndef FOCLASSIC_SKIP_PARAM

// Stats
# define ST_STRENGTH                             (0)
# define ST_PERCEPTION                           (1)  // Critter look = LookNormal + ST_PERCEPTION * 3 + ST_BONUS_LOOK + MultiHex
# define ST_CHARISMA                             (3)
# define ST_INTELLECT                            (4)
# define ST_AGILITY                              (5)
# define ST_MAX_LIFE                             (7)   // ClientInterface.cpp
# define ST_ACTION_POINTS                        (8)
# define ST_ARMOR_CLASS                          (9)   // ClientInterface.cpp
# define ST_MELEE_DAMAGE                         (10)  // ClientInterface.cpp
# define ST_CARRY_WEIGHT                         (11)
# define ST_SEQUENCE                             (12)  // ClientInterface.cpp
# define ST_HEALING_RATE                         (13)  // ClientInterface.cpp
# define ST_CRITICAL_CHANCE                      (14)  // ClientInterface.cpp
# define ST_NORMAL_RESIST                        (23)  // ClientInterface.cpp
# define ST_RADIATION_RESISTANCE                 (30)  // ClientInterface.cpp
# define ST_POISON_RESISTANCE                    (31)  // ClientInterface.cpp
# define ST_AGE                                  (70)  // ClientInterface.cpp, character registration
# define ST_GENDER                               (71)  // ClientInterface.cpp, character registration
# define ST_CURRENT_HP                           (72)
# define ST_POISONING_LEVEL                      (73)  // ClientInterface.cpp
# define ST_RADIATION_LEVEL                      (74)  // ClientInterface.cpp
# define ST_CURRENT_AP                           (75)
# define ST_EXPERIENCE                           (76)
# define ST_LEVEL                                (77)
# define ST_UNSPENT_SKILL_POINTS                 (78)
# define ST_UNSPENT_PERKS                        (79)
# define ST_KARMA                                (80)  // ClientInterface.cpp
# define ST_FOLLOW_CRIT                          (81)
# define ST_REPLICATION_MONEY                    (82)  // ClientInterface.cpp
# define ST_REPLICATION_COUNT                    (83)  // ClientInterface.cpp
# define ST_REPLICATION_TIME                     (84)
# define ST_REPLICATION_COST                     (85)  // ClientInterface.cpp
# define ST_TURN_BASED_AC                        (86)
# define ST_MAX_MOVE_AP                          (87)
# define ST_MOVE_AP                              (88)
# define ST_NPC_ROLE                             (89)
# define ST_BONUS_LOOK                           (101)  // Critter look = LookNormal + ST_PERCEPTION * 3 + ST_BONUS_LOOK + MultiHex
# define ST_HANDS_ITEM_AND_MODE                  (102)
# define ST_FREE_BARTER_PLAYER                   (103)
# define ST_DIALOG_ID                            (104)
# define ST_AI_ID                                (105)
# define ST_TEAM_ID                              (106)
# define ST_BAG_ID                               (107)
# define ST_LAST_WEAPON_ID                       (110)
# define ST_BASE_CRTYPE                          (112)
# define ST_TALK_DISTANCE                        (115)
# define ST_SCALE_FACTOR                         (116)
# define ST_WALK_TIME                            (117)
# define ST_RUN_TIME                             (118)
# define ST_MAX_TALKERS                          (119)
# define ST_ANIM3D_LAYER_BEGIN                   (150)
# define ST_ANIM3D_LAYER_END                     (179)

// Skills
# define SK_UNARMED                              (203)
# define SK_FIRST_AID                            (206)
# define SK_DOCTOR                               (207)
# define SK_SNEAK                                (208)
# define SK_LOCKPICK                             (209)
# define SK_STEAL                                (210)
# define SK_TRAPS                                (211)
# define SK_SCIENCE                              (212)
# define SK_REPAIR                               (213)
# define SK_SPEECH                               (214)  // TalkTime = MAX( RAW(SK_SPEECH) ) * 1000, DlgTalkMinTime )
# define SK_BARTER                               (215)

// Tagged skills
# define TAG_SKILL1                              (226)
# define TAG_SKILL2                              (227)
# define TAG_SKILL3                              (228)
# define TAG_SKILL4                              (229)

// Timeout
# define TO_SK_REPAIR                            (232)
# define TO_SK_SCIENCE                           (233)
# define TO_BATTLE                               (238)
# define TO_TRANSFER                             (239)
# define TO_REMOVE_FROM_GAME                     (240)
# define TO_KARMA_VOTING                         (242)

# define PE_SILENT_RUNNING                       (316)
# define PE_MASTER_TRADER                        (318)
# define PE_QUICK_POCKETS                        (349)

# define DAMAGE_POISONED                         (500)  // ClientInterface.cpp
# define DAMAGE_RADIATED                         (501)  // ClientInterface.cpp
# define DAMAGE_RIGHT_ARM                        (503)
# define DAMAGE_LEFT_ARM                         (504)
# define DAMAGE_RIGHT_LEG                        (505)
# define DAMAGE_LEFT_LEG                         (506)

// Modes
# define MODE_HIDE                               (510)
# define MODE_NO_STEAL                           (511)
# define MODE_NO_BARTER                          (512)
# define MODE_NO_ENEMY_STACK                     (513)
# define MODE_NO_PVP                             (514)
# define MODE_END_COMBAT                         (515)
# define MODE_DEFAULT_COMBAT                     (516)
# define MODE_NO_HOME                            (517)
# define MODE_GECK                               (518)
# define MODE_NO_FAVORITE_ITEM                   (519)
# define MODE_NO_ITEM_GARBAGER                   (520)
# define MODE_DLG_SCRIPT_BARTER                  (521)
# define MODE_UNLIMITED_AMMO                     (522)
# define MODE_INVULNERABLE                       (527)  // Critter::ToDead() does nothing
# define MODE_NO_FLATTEN                         (528)  // dead && !RAW( MODE_NO_FLATTEN ) ? DRAW_ORDER_DEAD_CRITTER : DRAW_ORDER_CRITTER
# define MODE_RANGE_HTH                          (530)
# define MODE_NO_LOOT                            (532)
# define MODE_NO_PUSH                            (536)
# define MODE_NO_UNARMED                         (537)
# define MODE_NO_AIM                             (538)
# define MODE_NO_WALK                            (539)
# define MODE_NO_RUN                             (540)
# define MODE_NO_TALK                            (541)

#endif // !FOCLASSIC_SKIP_PARAMS

// Cleanup
#ifndef FOCLASSIC_SCRIPT
# undef FOCLASSIC_MACROS_ALLOWED // TODO scripts don't have #undef
#endif

#ifdef FOCLASSIC_ENGINE
# undef FOCLASSIC_EXTENSION
# undef FOCLASSIC_BLEEDING_EDGE
#endif // FOCLASSIC_ENGINE

#endif // __FOCLASSIC_FOS__ //
