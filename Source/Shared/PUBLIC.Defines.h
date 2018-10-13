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
// They're weirdly formatted currently; uncrustify cannot correctly handle "#" between "MACRO_NAME" and "()"
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
#  define memzero( ptr, size )                       memset( ptr, 0, size )
#  define MAKEUINT( ch0, ch1, ch2, ch3 )             ( (uint)(uchar)(ch0) | ( (uint)(uchar)(ch1) << 8 ) | ( (uint)(uchar)(ch2) << 16 ) | ( (uint)(uchar)(ch3) << 24 ) )
#  define OFFSETOF( type, member )                   ( (int)offsetof( type, member ) )
#  define PACKUINT64( u32hi, u32lo )                 ( ( (uint64)u32hi << 32 ) | ( (uint64)u32lo ) )
#  define STATIC_ASSERT( a )                         static_assert( a, # a )
#  define STATIC_ASSERT_PRINT( func, a )             func( "STATIC_ASSERT( sizeof(%s) == %u );\n", # a, sizeof(a) )
# endif

// macros for everyone
# ifdef FOCLASSIC_EXTENSION
#  define BIN__N( x )                                (x) | x >> 3 | x >> 6 | x >> 9
#  define BIN__B( x )                                (x) & 0xf | (x) >> 12 & 0xf0
#  define BIN8( v )                                  (BIN__B( BIN__N( 0x ## v ) ) )
#  define BIN16( bin16, bin8 )                       ( (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )

#  define CLAMP( x, low, high )                      ( ( (x) > (high) ) ? (high) : ( ( (x) < (low) ) ? (low) : (x) ) )
#  define MIN( a, b )                                ( ( (a) < (b) ) ? (a) : (b) )
#  define MAX( a, b )                                ( ( (a) > (b) ) ? (a) : (b) )

#  define FLAG( x, y )                               ( ( (x) & (y) ) != 0 )
#  define SETFLAG( x, y )                            ( (x) = (x) | (y) )
#  define UNSETFLAG( x, y )                          ( (x) = ( (x) | (y) ) ^ (y) )

#  define CONVERT_GRAMM( x )                         ( (x) * 453 )
#  define IS_DIR_CORNER( dir )                       ( ( (dir) & 1 ) != 0 ) // 1, 3, 5, 7
#  define RAD( deg )                                 ( (deg) * 3.141592654f / 180.0f )
#  define UTF8_BUF_SIZE( count )                     ( (count) * 4 )
# endif  // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define BIN__N                                     # (x)(x) | x >> 3 | x >> 6 | x >> 9
#  define BIN__B                                     # (x)(x) & 0xf | (x) >> 12 & 0xf0
#  define BIN8                                       # (v)(BIN__B( BIN__N( 0x ## v ) ) )
#  define BIN16                                      # (bin16, bin8)( (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )

#  define CLAMP                                      # (x, low, high)( ( (x) > (high) ) ? (high) : ( ( (x) < (low) ) ? (low) : (x) ) )
#  define MIN                                        # (a, b)( ( (a) < (b) ) ? (a) : (b) )
#  define MAX                                        # (a, b)( ( (a) > (b) ) ? (a) : (b) )

#  define FLAG                                       # (x, y)( ( (x) & (y) ) != 0 )
#  define SETFLAG                                    # (x, y)( (x) = (x) | (y) )
#  define UNSETFLAG                                  # (x, y)( (x) = ( (x) | (y) ) ^ (y) )

#  define CONVERT_GRAMM                              # (x)( (x) * 453 )
#  define IS_DIR_CORNER                              # (dir)( ( (dir) & 1 ) != 0 ) // 1, 3, 5, 7
#  define RAD                                        # (deg)( (deg) * 3.141592654f / 180.0f )
#  define UTF8_BUF_SIZE                              # (count)( (count) * 4 )
# endif  // FOCLASSIC_SCRIPT

#endif  // FOCLASSIC_MACROS_ALLOWED

////////// Misc //////////

#define PI_FLOAT                                     (3.14159265f)
#define PI_VALUE                                     (3.141592654f)
#define PIBY2_FLOAT                                  (1.5707963f)
#define SQRT3T2_FLOAT                                (3.4641016151f)
#define SQRT3_FLOAT                                  (1.732050807568877f)
#define BIAS_FLOAT                                   (0.02f)
#define RAD2DEG                                      (57.29577951f)

////////// Limits //////////

#define MIN_INT                                      (0x80000000)
#define MAX_UINT8                                    (0xFF)
#define MAX_UINT16                                   (0xFFFF)
#define MAX_UINT                                     (0xFFFFFFFF)
#define MAX_INT                                      (0x7FFFFFFF)

#define MAX_DETERIORATION                            (10000)

// Used for name and password
#define MIN_NAME                                     (1)
#define MAX_NAME                                     (30)

// Character registration
#define MIN_AGE                                      (14)
#define MAX_AGE                                      (60)

#define MAX_PARAMS                                   (1000) // array size

// Proto
#define MAX_PROTO_CRITTERS                           (10000)
#define MAX_PROTO_ITEMS                              (30000)
#define MAX_PROTO_LOCATIONS                          (30000)
#define MAX_PROTO_MAPS                               (30000)

#define MAX_SCRIPT_NAME                              (64)

#define MAX_STORED_LOCATIONS                         (1000)

#define MIN_ZOOM                                     (0.2f)
#define MAX_ZOOM                                     (10.0f)

#ifdef FOCLASSIC_MACROS_ALLOWED

# define MAX_FOPATH                                  UTF8_BUF_SIZE( 1024 )
# define MAX_LOGTEXT                                 UTF8_BUF_SIZE( 2048 )
# define MAX_FOTEXT                                  UTF8_BUF_SIZE( 2048 )

#endif  // FOCLASSIC_MACROS_ALLOWED

////////// Magic //////////

// Access levels
#define ACCESS_CLIENT                                (0)
#define ACCESS_TESTER                                (1)
#define ACCESS_MODER                                 (2)
#define ACCESS_ADMIN                                 (3)
#define ACCESS_CUSTOM                                (ACCESS_ADMIN) // scripts/extensions

#ifdef FOCLASSIC_ENGINE
# ifdef DEV_VERSION
#  define ACCESS_DEFAULT                             (ACCESS_ADMIN)
# else
#  define ACCESS_DEFAULT                             (ACCESS_CLIENT)
# endif
#endif // FOCLASSIC_ENGINE

// AI Planes
#define AI_PLANE_MISC                                (0)
#define AI_PLANE_ATTACK                              (1)
#define AI_PLANE_WALK                                (2)
#define AI_PLANE_PICK                                (3)
#define AI_PLANE_LAST                                (AI_PLANE_PICK) // scripts/extensions

// AI Planes priorities
#define AI_PLANE_MISC_PRIORITY                       (10)            // unused
#define AI_PLANE_ATTACK_PRIORITY                     (50)
#define AI_PLANE_WALK_PRIORITY                       (20)
#define AI_PLANE_PICK_PRIORITY                       (35)            // unused

// Animation
#define ANIM_TYPE_FALLOUT                            (0)
#define ANIM_TYPE_3D                                 (1)
#define ANIM_TYPE_LAST                               (ANIM_TYPE_3D) // scripts/extensions

// Anim1
#define ANIM1_UNARMED                                (1)
// Anim2
#define ANIM2_IDLE                                   (1)
#define ANIM2_WALK                                   (3)
#define ANIM2_LIMP                                   (4)
#define ANIM2_RUN                                    (5)
#define ANIM2_PANIC_RUN                              (6)
#define ANIM2_SNEAK_WALK                             (7)
#define ANIM2_SNEAK_RUN                              (8)
#define ANIM2_IDLE_PRONE_FRONT                       (86)
#define ANIM2_IDLE_PRONE_BACK                        (87)
#define ANIM2_DEAD_FRONT                             (102)
#define ANIM2_DEAD_BACK                              (103)

// AngelScript properties
#define ANGELSCRIPT_ALLOW_UNSAFE_REFERENCES          (1) // true
#define ANGELSCRIPT_OPTIMIZE_BYTECODE                (2) // true
#define ANGELSCRIPT_COPY_SCRIPT_SECTIONS             (3)
#define ANGELSCRIPT_MAX_STACK_SIZE                   (4)
#define ANGELSCRIPT_USE_CHARACTER_LITERALS           (5) // 1
#define ANGELSCRIPT_ALLOW_MULTILINE_STRINGS          (6)
#define ANGELSCRIPT_ALLOW_IMPLICIT_HANDLE_TYPES      (7)
#define ANGELSCRIPT_BUILD_WITHOUT_LINE_CUES          (8)
#define ANGELSCRIPT_INIT_GLOBAL_VARS_AFTER_BUILD     (9)
#define ANGELSCRIPT_REQUIRE_ENUM_SCOPE               (10)
#define ANGELSCRIPT_SCRIPT_SCANNER                   (11) // 1
#define ANGELSCRIPT_INCLUDE_JIT_INSTRUCTIONS         (12)
#define ANGELSCRIPT_STRING_ENCODING                  (13)
#define ANGELSCRIPT_PROPERTY_ACCESSOR_MODE           (14)
#define ANGELSCRIPT_EXPAND_DEF_ARRAY_TO_TMPL         (15)
#define ANGELSCRIPT_AUTO_GARBAGE_COLLECT             (16) // false
#define ANGELSCRIPT_DISALLOW_GLOBAL_VARS             (17)
#define ANGELSCRIPT_ALWAYS_IMPL_DEFAULT_CONSTRUCT    (18) // true

// Item deterioration info
#define BI_BROKEN                                    (0x0F)

// Binary files signatures
#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define BINARY_SIGNATURE( name, type, ver )        const unsigned char name[6] = { 'F', 'O', type, ( (ver) >> 8 ) & 0xFF, ( (ver) ) & 0xFF, 0 }
#  define BINARY_SIGNATURE_VALID( sig1, sig2 )       (sig1[0] == sig2[0] && sig1[1] == sig2[1] && sig1[2] == sig2[2] && sig1[5] == sig2[5])              // skip version check
#  define BINARY_SIGNATURE_VERSION( sig )            ( (sig[3] << 8) | sig[4] )
# endif  // FOCLASSIC_EXTENSION

#endif  // FOCLASSIC_MACROS_ALLOWED

#define BINARY_CLIENTSAVE                            'C'
#define BINARY_MAPSAVE                               'M'
#define BINARY_PROFILERSAVE                          'P'
#define BINARY_SCRIPTSAVE                            'S'
#define BINARY_WORLDSAVE                             'W'
#define BINARY_CACHE                                 'c'  // reserved

// Chosen actions
#define CHOSEN_NONE                                  (0)  //
#define CHOSEN_MOVE                                  (1)  // HexX, HexY, Is run, Cut path, Wait double click, Double click tick
#define CHOSEN_MOVE_TO_CRITTER                       (2)  // Critter id, None, Is run, Cut path, Wait double click, Double click tick
#define CHOSEN_DIR                                   (3)  // 0 (CW) or 1 (CCW)
#define CHOSEN_SHOW_ITEM                             (4)  // Item id
#define CHOSEN_HIDE_ITEM                             (5)  // Item id
#define CHOSEN_USE_ITEM                              (6)  // Item id, Item pid, Target type, Target id, Item mode, Some param (timer)
#define CHOSEN_MOVE_ITEM                             (7)  // Item id, Item count, To slot, Is barter container, Is second try
#define CHOSEN_MOVE_ITEM_CONTAINER                   (8)  // From container, Item id, Count
#define CHOSEN_TAKE_ALL                              (9)  //
#define CHOSEN_USE_SKILL_ON_CRITTER                  (10) // Skill, Critter id
#define CHOSEN_USE_SKILL_ON_ITEM                     (11) // Is inventory, Skill index, Item id
#define CHOSEN_USE_SKILL_ON_SCENERY                  (12) // Skill, Pid, HexX, HexY
#define CHOSEN_TALK_NPC                              (13) // Critter id
#define CHOSEN_PICK_ITEM                             (14) // Pid, HexX, HexY
#define CHOSEN_PICK_CRITTER                          (15) // Critter id, (loot - 0, push - 1)
#define CHOSEN_WRITE_HOLODISK                        (16) // Holodisk id

#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate shortened CHOSEN_action")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define CHOSEN_MOVE_TO_CRIT                         (CHOSEN_MOVE_TO_CRITTER)
# define CHOSEN_MOVE_ITEM_CONT                       (CHOSEN_MOVE_ITEM_CONTAINER)
# define CHOSEN_USE_SKL_ON_CRITTER                   (CHOSEN_USE_SKILL_ON_CRITTER)
# define CHOSEN_USE_SKL_ON_ITEM                      (CHOSEN_USE_SKILL_ON_ITEM)
# define CHOSEN_USE_SKL_ON_SCEN                      (CHOSEN_USE_SKILL_ON_SCENERY)
# define CHOSEN_PICK_CRIT                            (CHOSEN_PICK_CRITTER)
# define CHOSEN_WRITE_HOLO                           (CHOSEN_WRITE_HOLODISK)
#endif // !FOCLASSIC_BLEEDING_EDGE

// Colors
// see SpriteManager.h: COLOR_GAME_RGB(r,g,b), COLOR_IFACE, COLOR_IFACE_A(a), COLOR_IFACE_RED, COLOR_IFACE_GREEN
#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define COLOR_ARGB( a, r, g, b )                   ( (uint)( ( ( (a) & 0xFF ) << 24 ) | ( ( (r) & 0xFF ) << 16 ) | ( ( (g) & 0xFF ) << 8 ) | ( (b) & 0xFF ) ) )
#  define COLOR_XRGB( r, g, b )                      COLOR_ARGB( 0xFF, r, g, b )
#  define COLOR_CHANGE_ALPHA( v, a )                 ( ( ( (v) | 0xFF000000 ) ^ 0xFF000000 ) | ( (uint)(a) & 0xFF ) << 24 )
# endif   // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define COLOR_ARGB                                 # (a, r, g, b)( (uint)( ( ( (a) & 0xff ) << 24 ) | ( ( (r) & 0xff ) << 16 ) | ( ( (g) & 0xff ) << 8 ) | ( (b) & 0xff ) ) )
#  define COLOR_XRGB                                 # (r, g, b)COLOR_ARGB( 0xff, r, g, b )
#  define COLOR_CHANGE_ALPHA                         # (v, a)( ( ( (v) | 0xFF000000 ) ^ 0xFF000000 ) | ( (uint)(a) & 0xFF ) << 24 )
# endif   // FOCLASSIC_SCRIPT

# define COLOR_IFACE_FIX                             COLOR_XRGB( 103, 95, 86 )
# define COLOR_CRITTER_NAME                          COLOR_XRGB( 0xAD, 0xAD, 0xB9 )
# define COLOR_TEXT                                  COLOR_XRGB( 60, 248, 0 )
# define COLOR_TEXT_WHITE                            COLOR_XRGB( 0xFF, 0xFF, 0xFF )
# define COLOR_TEXT_DWHITE                           COLOR_XRGB( 0xBF, 0xBF, 0xBF )
# define COLOR_TEXT_RED                              COLOR_XRGB( 0xC8, 0, 0 )
# define COLOR_TEXT_DRED                             COLOR_XRGB( 0xAA, 0, 0 )
# define COLOR_TEXT_DDRED                            COLOR_XRGB( 0x66, 0, 0 )
# define COLOR_TEXT_LRED                             COLOR_XRGB( 0xFF, 0, 0 )
# define COLOR_TEXT_BLUE                             COLOR_XRGB( 0, 0, 0xC8 )
# define COLOR_TEXT_DBLUE                            COLOR_XRGB( 0, 0, 0xAA )
# define COLOR_TEXT_LBLUE                            COLOR_XRGB( 0, 0, 0xFF )
# define COLOR_TEXT_GREEN                            COLOR_XRGB( 0, 0xC8, 0 )
# define COLOR_TEXT_DGREEN                           COLOR_XRGB( 0, 0xAA, 0 )
# define COLOR_TEXT_DDGREEN                          COLOR_XRGB( 0, 0x66, 0 )
# define COLOR_TEXT_LGREEN                           COLOR_XRGB( 0, 0xFF, 0 )
# define COLOR_TEXT_BLACK                            COLOR_XRGB( 0, 0, 0 )
# define COLOR_TEXT_SBLACK                           COLOR_XRGB( 0x10, 0x10, 0x10 )
# define COLOR_TEXT_DARK                             COLOR_XRGB( 0x30, 0x30, 0x30 )
# define COLOR_TEXT_GREEN_RED                        COLOR_XRGB( 0, 0xC8, 0xC8 )
# define COLOR_TEXT_SAND                             COLOR_XRGB( 0x8F, 0x6F, 0 )

#endif   // FOCLASSIC_MACROS_ALLOWED

// Commands
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] renumber COMMAND")
# endif
#endif // FOCLASSIC_ENGINE
#define COMMAND_EXIT                                 (1)
#define COMMAND_MYINFO                               (2)
#define COMMAND_GAMEINFO                             (3)
#define COMMAND_CRITID                               (4)
#define COMMAND_MOVECRIT                             (5)
#define COMMAND_KILLCRIT                             (6)
#define COMMAND_DISCONCRIT                           (7)
#define COMMAND_TOGLOBAL                             (8)
#define COMMAND_RESPAWN                              (9)
#define COMMAND_PARAM                                (10)
#define COMMAND_GETACCESS                            (11)
#define COMMAND_ADDITEM                              (12)
#define COMMAND_ADDITEM_SELF                         (14)
#define COMMAND_ADDNPC                               (15)
#define COMMAND_ADDLOCATION                          (16)
#define COMMAND_RELOADSCRIPTS                        (17) // investigate
#define COMMAND_LOADSCRIPT                           (18)
#define COMMAND_RELOAD_CLIENT_SCRIPTS                (19)
#define COMMAND_RUNSCRIPT                            (20)
#define COMMAND_RELOADLOCATIONS                      (21) // investigate
#define COMMAND_LOADLOCATION                         (22)
#define COMMAND_RELOADMAPS                           (23) // investigate
#define COMMAND_LOADMAP                              (24)
#define COMMAND_REGENMAP                             (25)
#define COMMAND_RELOADDIALOGS                        (26) // investigate
#define COMMAND_LOADDIALOG                           (27)
#define COMMAND_RELOADTEXTS                          (28) // investigate
#define COMMAND_RELOADAI                             (29) // investigate
#define COMMAND_CHECKVAR                             (30)
#define COMMAND_SETVAR                               (31)
#define COMMAND_SETTIME                              (32)
#define COMMAND_BAN                                  (33)
#define COMMAND_DELETE_ACCOUNT                       (34)
#define COMMAND_CHANGE_PASSWORD                      (35)
#define COMMAND_DROP_UID                             (36)
#define COMMAND_LOG                                  (37)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate CMD")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define CMD_EXIT                                    (COMMAND_EXIT)
# define CMD_MYINFO                                  (COMMAND_MYINFO)
# define CMD_GAMEINFO                                (COMMAND_GAMEINFO)
# define CMD_CRITID                                  (COMMAND_CRITID)
# define CMD_MOVECRIT                                (COMMAND_MOVECRIT)
# define CMD_KILLCRIT                                (COMMAND_KILLCRIT)
# define CMD_DISCONCRIT                              (COMMAND_DISCONCRIT)
# define CMD_TOGLOBAL                                (COMMAND_TOGLOBAL)
# define CMD_RESPAWN                                 (COMMAND_RESPAWN)
# define CMD_PARAM                                   (COMMAND_PARAM)
# define CMD_GETACCESS                               (COMMAND_GETACCESS)
# define CMD_ADDITEM                                 (COMMAND_ADDITEM)
# define CMD_ADDITEM_SELF                            (COMMAND_ADDITEM_SELF)
# define CMD_ADDNPC                                  (COMMAND_ADDNPC)
# define CMD_ADDLOCATION                             (COMMAND_ADDLOCATION)
# define CMD_RELOADSCRIPTS                           (COMMAND_RELOADSCRIPTS)
# define CMD_LOADSCRIPT                              (COMMAND_LOADSCRIPT)
# define CMD_RELOAD_CLIENT_SCRIPTS                   (COMMAND_RELOAD_CLIENT_SCRIPTS)
# define CMD_RUNSCRIPT                               (COMMAND_RUNSCRIPT)
# define CMD_RELOADLOCATIONS                         (COMMAND_RELOADLOCATIONS)
# define CMD_LOADLOCATION                            (COMMAND_LOADLOCATION)
# define CMD_RELOADMAPS                              (COMMAND_RELOADMAPS)
# define CMD_LOADMAP                                 (COMMAND_LOADMAP)
# define CMD_REGENMAP                                (COMMAND_REGENMAP)
# define CMD_RELOADDIALOGS                           (COMMAND_RELOADDIALOGS)
# define CMD_LOADDIALOG                              (COMMAND_LOADDIALOG)
# define CMD_RELOADTEXTS                             (COMMAND_RELOADTEXTS)
# define CMD_RELOADAI                                (COMMAND_RELOADAI)
# define CMD_CHECKVAR                                (COMMAND_CHECKVAR)
# define CMD_SETVAR                                  (COMMAND_SETVAR)
# define CMD_SETTIME                                 (COMMAND_SETTIME)
# define CMD_BAN                                     (COMMAND_BAN)
# define CMD_DELETE_ACCOUNT                          (COMMAND_DELETE_ACCOUNT)
# define CMD_CHANGE_PASSWORD                         (COMMAND_CHANGE_PASSWORD)
# define CMD_DROP_UID                                (COMMAND_DROP_UID)
# define CMD_LOG                                     (COMMAND_LOG)
#endif   // !FOCLASSIC_BLEEDING_EDGE

// Combat modes
#define COMBAT_MODE_ANY                              (0)
#define COMBAT_MODE_REAL_TIME                        (1)
#define COMBAT_MODE_TURN_BASED                       (2)

// Constants collections
#define CONSTANTS_PARAM                              (0)
#define CONSTANTS_ITEM                               (1)
#define CONSTANTS_DEFINE                             (2)
#define CONSTANTS_PICTURE                            (3)
#define CONSTANTS_HASH                               (4)

// Take flags
#define CONTAINER_GET                                (1)
#define CONTAINER_PUT                                (2)
#define CONTAINER_GETALL                             (3)
#define CONTAINER_PUTALL                             (4)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate CONT")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define CONT_GET                                    (CONTAINER_GET)
# define CONT_PUT                                    (CONTAINER_PUT)
# define CONT_GETALL                                 (CONTAINER_GETALL)
# define CONT_PUTALL                                 (CONTAINER_PUTALL)
#endif   // !FOCLASSIC_BLEEDING_EDGE

// Contour types
#define CONTOUR_RED                                  (1)
#define CONTOUR_YELLOW                               (2)
#define CONTOUR_CUSTOM                               (3)

// Corner type
#define CORNER_NORTH_SOUTH                           (0)
#define CORNER_WEST                                  (1)
#define CORNER_EAST                                  (2)
#define CORNER_SOUTH                                 (3)
#define CORNER_NORTH                                 (4)
#define CORNER_EAST_WEST                             (5)

// Critter conditions
#define CRITTER_CONDITION_LIFE                       (1)
#define CRITTER_CONDITION_KNOCKOUT                   (2)
#define CRITTER_CONDITION_DEAD                       (3)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate COND")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define COND_LIFE                                   (CRITTER_CONDITION_LIFE)
# define COND_KNOCKOUT                               (CRITTER_CONDITION_KNOCKOUT)
# define COND_DEAD                                   (CRITTER_CONDITION_DEAD)
#endif   // !FOCLASSIC_BLEEDING_EDGE

// Critter events
#define CRITTER_EVENT_IDLE                           (0)
#define CRITTER_EVENT_FINISH                         (1)
#define CRITTER_EVENT_DEAD                           (2)
#define CRITTER_EVENT_RESPAWN                        (3)
#define CRITTER_EVENT_SHOW_CRITTER                   (4)
#define CRITTER_EVENT_SHOW_CRITTER_1                 (5)
#define CRITTER_EVENT_SHOW_CRITTER_2                 (6)
#define CRITTER_EVENT_SHOW_CRITTER_3                 (7)
#define CRITTER_EVENT_HIDE_CRITTER                   (8)
#define CRITTER_EVENT_HIDE_CRITTER_1                 (9)
#define CRITTER_EVENT_HIDE_CRITTER_2                 (10)
#define CRITTER_EVENT_HIDE_CRITTER_3                 (11)
#define CRITTER_EVENT_SHOW_ITEM_ON_MAP               (12)
#define CRITTER_EVENT_CHANGE_ITEM_ON_MAP             (13)
#define CRITTER_EVENT_HIDE_ITEM_ON_MAP               (14)
#define CRITTER_EVENT_ATTACK                         (15)
#define CRITTER_EVENT_ATTACKED                       (16)
#define CRITTER_EVENT_STEALING                       (17)
#define CRITTER_EVENT_MESSAGE                        (18)
#define CRITTER_EVENT_USE_ITEM                       (19)
#define CRITTER_EVENT_USE_ITEM_ON_ME                 (20)
#define CRITTER_EVENT_USE_SKILL                      (21)
#define CRITTER_EVENT_USE_SKILL_ON_ME                (22)
#define CRITTER_EVENT_DROP_ITEM                      (23)
#define CRITTER_EVENT_MOVE_ITEM                      (24)
#define CRITTER_EVENT_KNOCKOUT                       (25)
#define CRITTER_EVENT_SMTH_DEAD                      (26)
#define CRITTER_EVENT_SMTH_STEALING                  (27)
#define CRITTER_EVENT_SMTH_ATTACK                    (28)
#define CRITTER_EVENT_SMTH_ATTACKED                  (29)
#define CRITTER_EVENT_SMTH_USE_ITEM                  (30)
#define CRITTER_EVENT_SMTH_USE_SKILL                 (31)
#define CRITTER_EVENT_SMTH_DROP_ITEM                 (32)
#define CRITTER_EVENT_SMTH_MOVE_ITEM                 (33)
#define CRITTER_EVENT_SMTH_KNOCKOUT                  (34)
#define CRITTER_EVENT_PLANE_BEGIN                    (35)
#define CRITTER_EVENT_PLANE_END                      (36)
#define CRITTER_EVENT_PLANE_RUN                      (37)
#define CRITTER_EVENT_BARTER                         (38)
#define CRITTER_EVENT_TALK                           (39)
#define CRITTER_EVENT_GLOBAL_PROCESS                 (40)
#define CRITTER_EVENT_GLOBAL_INVITE                  (41)
#define CRITTER_EVENT_TURN_BASED_PROCESS             (42)
#define CRITTER_EVENT_SMTH_TURN_BASED_PROCESS        (43)
#define CRITTER_EVENT_MAX                            (44)

// Critters flags
#define CRITTER_FLAG_PLAYER                          (0x00010000) // Player
#define CRITTER_FLAG_NPC                             (0x00020000) // Npc
#define CRITTER_FLAG_DISCONNECT                      (0x00080000) // In offline
#define CRITTER_FLAG_CHOSEN                          (0x00100000) // Chosen
#define CRITTER_FLAG_RULEGROUP                       (0x00200000) // Group rule
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate FCRIT")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define FCRIT_PLAYER                                (CRITTER_FLAG_PLAYER)
# define FCRIT_NPC                                   (CRITTER_FLAG_NPC)
# define FCRIT_DISCONNECT                            (CRITTER_FLAG_DISCONNECT)
# define FCRIT_CHOSEN                                (CRITTER_FLAG_CHOSEN)
# define FCRIT_RULEGROUP                             (CRITTER_FLAG_RULEGROUP)
#endif   // !FOCLASSIC_BLEEDING_EDGE

// Critters ID
#define CRITTER_ID_START_PLAYER                      (1)
#define CRITTER_ID_START_NPC                         (5000001)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate NPC_START_ID")
#  pragma MESSAGE("[STAGE 3] deprecate USER_START_ID")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define NPC_START_ID                                (CRITTER_ID_START_NPC)
# define USER_START_ID                               (CRITTER_ID_START_PLAYER)
#endif   // !FOCLASSIC_BLEEDING_EDGE

#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define CRITTER_ID_IS_NPC( id )                    ( (id) >= CRITTER_ID_START_NPC )
#  define CRITTER_ID_IS_PLAYER( id )                 ( (id) >= CRITTER_ID_START_PLAYER && (id) < CRITTER_ID_START_NPC )
# endif   // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define CRITTER_ID_IS_NPC                          # (id)( (id) >= CRITTER_ID_START_NPC )
#  define CRITTER_ID_IS_PLAYER                       # (id)( (id) >= CRITTER_ID_START_PLAYER && (id) < CRITTER_ID_START_NPC )
# endif   // FOCLASSIC_SCRIPT

#endif    // FOCLASSIC_MACROS_ALLOWED

// Critter description
#define CRITTER_LOOK_NAME                            (0)
#define CRITTER_LOOK_SHORT                           (1)
#define CRITTER_LOOK_FULL                            (2)

#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate CRITTER_ONLY_NAME")
# endif
#endif   // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define CRITTER_ONLY_NAME                           (CRITTER_LOOK_NAME)
#endif // !FOCLASSIC_BLEEDING_EDGE

// Damage types
#define DAMAGE_TYPE_UNCALLED                         (0)
#define DAMAGE_TYPE_NORMAL                           (1)
#define DAMAGE_TYPE_LASER                            (2)
#define DAMAGE_TYPE_FIRE                             (3)
#define DAMAGE_TYPE_PLASMA                           (4)
#define DAMAGE_TYPE_ELECTR                           (5)
#define DAMAGE_TYPE_EMP                              (6)
#define DAMAGE_TYPE_EXPLODE                          (7)

// Inventory descriptions
#define DESC_INVENTORY_MAIN                          (0)
#define DESC_INVENTORY_SPECIAL                       (1)
#define DESC_INVENTORY_STATS                         (2)
#define DESC_INVENTORY_RESIST                        (3)

#ifdef FOCLASSIC_ENGINE
// Sprite layers
# define DRAW_ORDER                                  (20)
# define DRAW_ORDER_FLAT                             (0)
# define DRAW_ORDER_TILE                             (DRAW_ORDER_FLAT + 0)
# define DRAW_ORDER_TILE_END                         (DRAW_ORDER_FLAT + 4)
# define DRAW_ORDER_HEX_GRID                         (DRAW_ORDER_FLAT + 5)
# define DRAW_ORDER_FLAT_SCENERY                     (DRAW_ORDER_FLAT + 8)
# define DRAW_ORDER_LIGHT                            (DRAW_ORDER_FLAT + 9)
# define DRAW_ORDER_DEAD_CRITTER                     (DRAW_ORDER_FLAT + 10)
# define DRAW_ORDER_FLAT_ITEM                        (DRAW_ORDER_FLAT + 13)
# define DRAW_ORDER_TRACK                            (DRAW_ORDER_FLAT + 16)
# define DRAW_ORDER_SCENERY                          (DRAW_ORDER + 3)
# define DRAW_ORDER_ITEM                             (DRAW_ORDER + 6)
# define DRAW_ORDER_CRITTER                          (DRAW_ORDER + 9)
# define DRAW_ORDER_RAIN                             (DRAW_ORDER + 12)
# define DRAW_ORDER_LAST                             (39)
# define DRAW_ORDER_ITEM_AUTO( i )                   (i->IsFlat() ? (i->IsItem() ? DRAW_ORDER_FLAT_ITEM : DRAW_ORDER_FLAT_SCENERY) : (i->IsItem() ? DRAW_ORDER_ITEM : DRAW_ORDER_SCENERY) )
# define DRAW_ORDER_CRIT_AUTO( c )                   (c->IsDead() && !c->IsRawParam( MODE_NO_FLATTEN ) ? DRAW_ORDER_DEAD_CRITTER : DRAW_ORDER_CRITTER)
#endif

// Entires
#define ENTIRE_DEFAULT                               (0)
#define ENTIRE_LOG_OUT                               (241)

// Critter find types
#define FIND_LIFE                                    (0x01)
#define FIND_KO                                      (0x02)
#define FIND_DEAD                                    (0x04)
#define FIND_ONLY_PLAYERS                            (0x10)
#define FIND_ONLY_NPC                                (0x20)
#ifndef FOCLASSIC_ENGINE
# define FIND_LIFE_AND_KO                            (FIND_LIFE | FIND_KO)
# define FIND_LIFE_AND_DEAD                          (FIND_LIFE | FIND_DEAD)
# define FIND_KO_AND_DEAD                            (FIND_KO | FIND_DEAD)
# define FIND_ALL                                    (FIND_LIFE | FIND_KO | FIND_DEAD)
#endif // !FOCLASSIC_ENGINE

// FixBoy function call states
#define FIXBOY_LIST                                  (0)
#define FIXBOY_BUTTON                                (1)
#define FIXBOY_CRAFT                                 (2)

// FixBoy craft results
#define FIXBOY_ALLOW_CRAFT                           (0x0001)
#define FIXBOY_CHECK_TIMEOUT                         (0x0002)
#define FIXBOY_SET_TIMEOUT                           (0x0004)
#define FIXBOY_CHECK_PARAMS                          (0x0008)
#define FIXBOY_CHECK_MATERIALS                       (0x0010)
#define FIXBOY_CHECK_TOOLS                           (0x0020)
#define FIXBOY_SUB_MATERIALS                         (0x0040)
#define FIXBOY_ADD_CRAFT_ITEMS                       (0x0080)
#define FIXBOY_ADD_EXPERIENCE                        (0x0100)
#define FIXBOY_SEND_SUCC_MESSAGE                     (0x0200)
#define FIXBOY_SEND_FAIL_MESSAGE                     (0x0400)
#define FIXBOY_DEFAULT                               (0xFFFF)

// Font flags
#define FONT_FLAG_NOBREAK                            (0x0001)
#define FONT_FLAG_NOBREAK_LINE                       (0x0002)
#define FONT_FLAG_CENTERX                            (0x0004)
#define FONT_FLAG_CENTERY                            (0x0008)
#define FONT_FLAG_CENTERR                            (0x0010)
#define FONT_FLAG_BOTTOM                             (0x0020)
#define FONT_FLAG_UPPER                              (0x0040)
#define FONT_FLAG_NO_COLORIZE                        (0x0080)
#define FONT_FLAG_ALIGN                              (0x0100)
#define FONT_FLAG_BORDERED                           (0x0200)
#ifndef FOCLASSIC_ENGINE
# define FONT_FLAG_CENTERXY                          (FONT_FLAG_CENTERX | FONT_FLAG_CENTERY)
#endif // !FOCLASSIC_ENGINE

#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define FONT_FLAG_SKIPLINES( l )                   (0x0400 | ( (l) << 16 ) )
#  define FONT_FLAG_SKIPLINES_END( l )               (0x0800 | ( (l) << 16 ) )
# endif // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define FONT_FLAG_SKIPLINES                        # (l)(0x0400 | ( (l) << 16 ) )
#  define FONT_FLAG_SKIPLINES_END                    # (l)(0x0800 | ( (l) << 16 ) )
# endif // FOCLASSIC_SCRIPT

#endif   // FOCLASSIC_MACROS_ALLOWED

// Fonts
#define FONT_TYPE_FO                                 (0)
#define FONT_TYPE_NUM                                (1)
#define FONT_TYPE_BIG_NUM                            (2)
#define FONT_TYPE_SAND_NUM                           (3)
#define FONT_TYPE_SPECIAL                            (4)
#define FONT_TYPE_DEFAULT                            (5)
#define FONT_TYPE_THIN                               (6)
#define FONT_TYPE_FAT                                (7)
#define FONT_TYPE_BIG                                (8)

#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate FONT_type")
# endif
#endif   // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define FONT_FO                                     (FONT_TYPE_FO)
# define FONT_NUM                                    (FONT_TYPE_NUM)
# define FONT_BIG_NUM                                (FONT_TYPE_BIG_NUM)
# define FONT_SAND_NUM                               (FONT_TYPE_SAND_NUM)
# define FONT_SPECIAL                                (FONT_TYPE_SPECIAL)
# define FONT_DEFAULT                                (FONT_TYPE_DEFAULT)
# define FONT_THIN                                   (FONT_TYPE_THIN)
# define FONT_FAT                                    (FONT_TYPE_FAT)
# define FONT_BIG                                    (FONT_TYPE_BIG)
#endif   // !FOCLASSIC_BLEEDING_EDGE

#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate FT")
# endif
#endif   // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
// Font flags
# define FT_NOBREAK                                  (FONT_FLAG_NOBREAK)
# define FT_NOBREAK_LINE                             (FONT_FLAG_NOBREAK_LINE)
# define FT_CENTERX                                  (FONT_FLAG_CENTERX)
# define FT_CENTERY                                  (FONT_FLAG_CENTERY)
# define FT_CENTERR                                  (FONT_FLAG_CENTERR)
# define FT_BOTTOM                                   (FONT_FLAG_BOTTOM)
# define FT_UPPER                                    (FONT_FLAG_UPPER)
# define FT_NO_COLORIZE                              (FONT_FLAG_NO_COLORIZE)
# define FT_ALIGN                                    (FONT_FLAG_ALIGN)
# define FT_BORDERED                                 (FONT_FLAG_BORDERED)

# ifdef FOCLASSIC_MACROS_ALLOWED

#  ifdef FOCLASSIC_EXTENSION
#   define FT_SKIPLINES( l )                         (0x0400 | ( (l) << 16 ) )
#   define FT_SKIPLINES_END( l )                     (0x0800 | ( (l) << 16 ) )
#  endif   // FOCLASSIC_EXTENSION

#  ifdef FOCLASSIC_SCRIPT
#   define FT_SKIPLINES                              # (l)(0x0400 | ( (l) << 16 ) )
#   define FT_SKIPLINES_END                          # (l)(0x0800 | ( (l) << 16 ) )
#  endif   // FOCLASSIC_EXTENSION

# endif    // FOCLASSIC_MACROS_ALLOWED
#endif     // !FOCLASSIC_BLEEDING_EDGE

#define GENDER_MALE                                  (0)
#define GENDER_FEMALE                                (1)

// Grid Types
#define GRID_EXITGRID                                (1)
#define GRID_STAIRS                                  (2)
#define GRID_LADDERBOT                               (3)
#define GRID_LADDERTOP                               (4)
#define GRID_ELEVATOR                                (5)

// Hex flags
// ProtoMap
#ifdef FOCLASSIC_MACROS_ALLOWED

# define HEX_FLAG_BLOCK                              BIN8( 00000001 )
# define HEX_FLAG_NOTRAKE                            BIN8( 00000010 )
# define HEX_FLAG_WALL                               BIN8( 00000100 )
# define HEX_FLAG_SCEN                               BIN8( 00001000 )
# define HEX_FLAG_SCEN_GRID                          BIN8( 00010000 )
# define HEX_FLAG_TRIGGER                            BIN8( 00100000 )
// Map
# define HEX_FLAG_CRITTER                            BIN8( 00000001 )
# define HEX_FLAG_DEAD_CRITTER                       BIN8( 00000010 )
# define HEX_FLAG_ITEM                               BIN8( 00000100 )
# define HEX_FLAG_DOOR                               BIN8( 00001000 )
# define HEX_FLAG_BLOCK_ITEM                         BIN8( 00010000 )
# define HEX_FLAG_NRAKE_ITEM                         BIN8( 00100000 )
# define HEX_FLAG_WALK_ITEM                          BIN8( 01000000 )
# define HEX_FLAG_GAG_ITEM                           BIN8( 10000000 )

# define HEX_FLAG_NOWAY                              BIN16( 00010001, 00000001 )
# define HEX_FLAG_NOSHOOT                            BIN16( 00100000, 00000010 )

# ifdef FOCLASSIC_ENGINE
#  if FOCLASSIC_STAGE >= 3
#   pragma MESSAGE("deprecate FH")
#  endif
# endif // FOCLASSIC_ENGINE
# ifndef FOCLASSIC_BLEEDING_EDGE
#  define FH_BLOCK                                   (HEX_FLAG_BLOCK)
#  define FH_NOTRAKE                                 (HEX_FLAG_NOTRAKE)
#  define FH_WALL                                    (HEX_FLAG_WALL)
#  define FH_SCEN                                    (HEX_FLAG_SCEN)
#  define FH_SCEN_GRID                               (HEX_FLAG_SCEN_GRID)
#  define FH_TRIGGER                                 (HEX_FLAG_TRIGGER)
#  define FH_CRITTER                                 (HEX_FLAG_CRITTER)
#  define FH_DEAD_CRITTER                            (HEX_FLAG_DEAD_CRITTER)
#  define FH_ITEM                                    (HEX_FLAG_ITEM)
#  define FH_DOOR                                    (HEX_FLAG_DOOR)
#  define FH_BLOCK_ITEM                              (HEX_FLAG_BLOCK_ITEM)
#  define FH_NRAKE_ITEM                              (HEX_FLAG_NRAKE_ITEM)
#  define FH_WALK_ITEM                               (HEX_FLAG_WALK_ITEM)
#  define FH_GAG_ITEM                                (HEX_FLAG_GAG_ITEM)
#  define FH_NOWAY                                   (HEX_FLAG_NOWAY)
#  define FH_NOSHOOT                                 (HEX_FLAG_NOSHOOT)
# endif   // !FOCLASSIC_BLEEDING_EDGE

#endif    // FOCLASSIC_MACROS_ALLOWED

// Hit locations
#define HIT_LOCATION_NONE                            (0)
#define HIT_LOCATION_HEAD                            (1)
#define HIT_LOCATION_LEFT_ARM                        (2)
#define HIT_LOCATION_RIGHT_ARM                       (3)
#define HIT_LOCATION_TORSO                           (4)
#define HIT_LOCATION_RIGHT_LEG                       (5)
#define HIT_LOCATION_LEFT_LEG                        (6)
#define HIT_LOCATION_EYES                            (7)
#define HIT_LOCATION_GROIN                           (8)
#define HIT_LOCATION_UNCALLED                        (9)
#define HIT_LOCATION_MAX                             (HIT_LOCATION_UNCALLED)

// IndicatorType
#define INDICATOR_LINES                              (0)
#define INDICATOR_NUMBERS                            (1)
#define INDICATOR_BOTH                               (2)

// Accessory
#define ITEM_ACCESSORY_NONE                          (0)
#define ITEM_ACCESSORY_CRITTER                       (1)
#define ITEM_ACCESSORY_HEX                           (2)
#define ITEM_ACCESSORY_CONTAINER                     (3)

// Item data masks
#define ITEM_DATA_MASK_CHOSEN                        (0)
#define ITEM_DATA_MASK_CRITTER                       (1)
#define ITEM_DATA_MASK_CRITTER_EXT                   (2)
#define ITEM_DATA_MASK_CONTAINER                     (3)
#define ITEM_DATA_MASK_MAP                           (4)
#define ITEM_DATA_MASK_MAX                           (ITEM_DATA_MASK_MAP + 1) // array size

// Item events
#define ITEM_EVENT_FINISH                            (0)
#define ITEM_EVENT_ATTACK                            (1)
#define ITEM_EVENT_USE                               (2)
#define ITEM_EVENT_USE_ON_ME                         (3)
#define ITEM_EVENT_SKILL                             (4)
#define ITEM_EVENT_DROP                              (5)
#define ITEM_EVENT_MOVE                              (6)
#define ITEM_EVENT_WALK                              (7)
#define ITEM_EVENT_MAX                               (ITEM_EVENT_WALK + 1) // array size

// Item flags
#define ITEM_FLAG_HIDDEN                             (0x00000001)
#define ITEM_FLAG_FLAT                               (0x00000002)
#define ITEM_FLAG_NO_BLOCK                           (0x00000004)
#define ITEM_FLAG_SHOOT_THRU                         (0x00000008)
#define ITEM_FLAG_LIGHT_THRU                         (0x00000010)
#define ITEM_FLAG_MULTI_HEX                          (0x00000020)    // unused
#define ITEM_FLAG_WALL_TRANS_END                     (0x00000040)    // unused
#define ITEM_FLAG_TWO_HANDS                          (0x00000080)
#define ITEM_FLAG_BIG_GUN                            (0x00000100)
#define ITEM_FLAG_ALWAYS_VIEW                        (0x00000200)
#define ITEM_FLAG_HAS_TIMER                          (0x00000400)
#define ITEM_FLAG_BAD_ITEM                           (0x00000800)
#define ITEM_FLAG_NO_HIGHLIGHT                       (0x00001000)
#define ITEM_FLAG_SHOW_ANIM                          (0x00002000)
#define ITEM_FLAG_SHOW_ANIM_EXT                      (0x00004000)
#define ITEM_FLAG_LIGHT                              (0x00008000)
#define ITEM_FLAG_GECK                               (0x00010000)
#define ITEM_FLAG_TRAP                               (0x00020000)
#define ITEM_FLAG_NO_LIGHT_INFLUENCE                 (0x00040000)
#define ITEM_FLAG_NO_LOOT                            (0x00080000)
#define ITEM_FLAG_NO_STEAL                           (0x00100000)
#define ITEM_FLAG_GAG                                (0x00200000)
#define ITEM_FLAG_COLORIZE                           (0x00400000)
#define ITEM_FLAG_COLORIZE_INV                       (0x00800000)
#define ITEM_FLAG_CAN_USE_ON_SMTH                    (0x01000000)
#define ITEM_FLAG_CAN_LOOK                           (0x02000000)    // unused, investigate
#define ITEM_FLAG_CAN_TALK                           (0x04000000)
#define ITEM_FLAG_CAN_PICKUP                         (0x08000000)
#define ITEM_FLAG_CAN_USE                            (0x10000000)
#define ITEM_FLAG_HOLODISK                           (0x20000000)
#define ITEM_FLAG_RADIO                              (0x40000000)
#define ITEM_FLAG_CACHED                             (0x80000000)    // unused

#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate ITEM_flag")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define ITEM_HIDDEN                                 (ITEM_FLAG_HIDDEN)
# define ITEM_FLAT                                   (ITEM_FLAG_FLAT)
# define ITEM_NO_BLOCK                               (ITEM_FLAG_NO_BLOCK)
# define ITEM_SHOOT_THRU                             (ITEM_FLAG_SHOOT_THRU)
# define ITEM_LIGHT_THRU                             (ITEM_FLAG_LIGHT_THRU)
# define ITEM_MULTI_HEX                              (ITEM_FLAG_MULTI_HEX)
# define ITEM_WALL_TRANS_END                         (ITEM_FLAG_WALL_TRANS_END)
# define ITEM_TWO_HANDS                              (ITEM_FLAG_TWO_HANDS)
# define ITEM_BIG_GUN                                (ITEM_FLAG_BIG_GUN)
# define ITEM_ALWAYS_VIEW                            (ITEM_FLAG_ALWAYS_VIEW)
# define ITEM_HAS_TIMER                              (ITEM_FLAG_HAS_TIMER)
# define ITEM_BAD_ITEM                               (ITEM_FLAG_BAD_ITEM)
# define ITEM_NO_HIGHLIGHT                           (ITEM_FLAG_NO_HIGHLIGHT)
# define ITEM_SHOW_ANIM                              (ITEM_FLAG_SHOW_ANIM)
# define ITEM_SHOW_ANIM_EXT                          (ITEM_FLAG_SHOW_ANIM_EXT)
# define ITEM_LIGHT                                  (ITEM_FLAG_LIGHT)
# define ITEM_GECK                                   (ITEM_FLAG_GECK)
# define ITEM_TRAP                                   (ITEM_FLAG_TRAP)
# define ITEM_NO_LIGHT_INFLUENCE                     (ITEM_FLAG_NO_LIGHT_INFLUENCE)
# define ITEM_NO_LOOT                                (ITEM_FLAG_NO_LOOT)
# define ITEM_NO_STEAL                               (ITEM_FLAG_NO_STEAL)
# define ITEM_GAG                                    (ITEM_FLAG_GAG)
# define ITEM_COLORIZE                               (ITEM_FLAG_COLORIZE)
# define ITEM_COLORIZE_INV                           (ITEM_FLAG_COLORIZE_INV)
# define ITEM_CAN_USE_ON_SMTH                        (ITEM_FLAG_CAN_USE_ON_SMTH)
# define ITEM_CAN_LOOK                               (ITEM_FLAG_CAN_LOOK)
# define ITEM_CAN_TALK                               (ITEM_FLAG_CAN_TALK)
# define ITEM_CAN_PICKUP                             (ITEM_FLAG_CAN_PICKUP)
# define ITEM_CAN_USE                                (ITEM_FLAG_CAN_USE)
# define ITEM_HOLODISK                               (ITEM_FLAG_HOLODISK)
# define ITEM_RADIO                                  (ITEM_FLAG_RADIO)
# define ITEM_CACHED                                 (ITEM_FLAG_CACHED)
#endif   // !FOCLASSIC_BLEEDING_EDGE

#define ITEM_LOOK_DEFAULT                            (0)
#define ITEM_LOOK_ONLY_NAME                          (1)
#define ITEM_LOOK_MAP                                (2)
#define ITEM_LOOK_BARTER                             (3)
#define ITEM_LOOK_INVENTORY                          (4)
#define ITEM_LOOK_WM_CAR                             (5)

// Types
#define ITEM_TYPE_OTHER                              (0)
#define ITEM_TYPE_ARMOR                              (1)
#define ITEM_TYPE_DRUG                               (2) // stacked
#define ITEM_TYPE_WEAPON                             (3) // combined
#define ITEM_TYPE_AMMO                               (4) // stacked
#define ITEM_TYPE_MISC                               (5) // combined
#define ITEM_TYPE_KEY                                (7)
#define ITEM_TYPE_CONTAINER                          (8)
#define ITEM_TYPE_DOOR                               (9)
#define ITEM_TYPE_GRID                               (10)
#define ITEM_TYPE_GENERIC                            (11)
#define ITEM_TYPE_WALL                               (12)
#define ITEM_TYPE_CAR                                (13)
#define ITEM_TYPE_LAST                               (ITEM_TYPE_CAR)      // scripts/extensions
#define ITEM_TYPE_MAX                                (ITEM_TYPE_LAST + 1) // array size

// Script events
#define LOCATION_EVENT_FINISH                        (0)
#define LOCATION_EVENT_ENTER                         (1)
#define LOCATION_EVENT_MAX                           (LOCATION_EVENT_ENTER + 1) // array size

// Locker
#define LOCKER_ISOPEN                                (0x01)
#define LOCKER_NOOPEN                                (0x10)

// Look checks
#define LOOK_CHECK_DIR                               (0x01) // Nerf look by directions of look
#define LOOK_CHECK_SNEAK_DIR                         (0x02) // Nerf opponent sneak by directions of look
#define LOOK_CHECK_SNEAK_WEIGHT                      (0x04) // Nerf sneak by current weight
#define LOOK_CHECK_TRACE                             (0x08) // Tracing for wall aviablility
#define LOOK_CHECK_SCRIPT                            (0x10) // Use bool check_look() from scripts
#define LOOK_CHECK_ITEM_SCRIPT                       (0x20) // Use bool check_trap_look() from scripts, for items with ITEM_TRAP flag

// Script events
#define MAP_EVENT_FINISH                             (0)
#define MAP_EVENT_LOOP_0                             (1)
#define MAP_EVENT_LOOP_1                             (2)
#define MAP_EVENT_LOOP_2                             (3)
#define MAP_EVENT_LOOP_3                             (4)
#define MAP_EVENT_LOOP_4                             (5)
#define MAP_EVENT_IN_CRITTER                         (6)
#define MAP_EVENT_OUT_CRITTER                        (7)
#define MAP_EVENT_CRITTER_DEAD                       (8)
#define MAP_EVENT_TURN_BASED_BEGIN                   (9)
#define MAP_EVENT_TURN_BASED_END                     (10)
#define MAP_EVENT_TURN_BASED_PROCESS                 (11)
#define MAP_EVENT_MAX                                (MAP_EVENT_TURN_BASED_PROCESS + 1) // aray size

// Map object types
#define MAP_OBJECT_CRITTER                           (0)
#define MAP_OBJECT_ITEM                              (1)
#define MAP_OBJECT_SCENERY                           (2)

// Material
#ifndef FOCLASSIC_ENGINE
# define MATERIAL_GLASS                              (0)
# define MATERIAL_METAL                              (1)
# define MATERIAL_PLASTIC                            (2)
# define MATERIAL_WOOD                               (3)
# define MATERIAL_DIRT                               (4)
# define MATERIAL_STONE                              (5)
# define MATERIAL_CEMENT                             (6)
# define MATERIAL_LEATHER                            (7)
#endif   // !FOCLASSIC_ENGINE

#define MSGBOX_GAME                                  (0)
#define MSGBOX_TALK                                  (1)
#define MSGBOX_COMBAT_RESULT                         (2)
#define MSGBOX_VIEW                                  (3)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate FOMB")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define FOMB_GAME                                   (MSGBOX_GAME)
# define FOMB_TALK                                   (MSGBOX_TALK)
# define FOMB_COMBAT_RESULT                          (MSGBOX_COMBAT_RESULT)
# define FOMB_VIEW                                   (MSGBOX_VIEW)
#endif // !FOCLASSIC_BLEEDING_EDGE

// Primitives
#define PRIMITIVE_POINTLIST                          (1)
#define PRIMITIVE_LINELIST                           (2)
#define PRIMITIVE_LINESTRIP                          (3)
#define PRIMITIVE_TRIANGLELIST                       (4)
#define PRIMITIVE_TRIANGLESTRIP                      (5)
#define PRIMITIVE_TRIANGLEFAN                        (6)

// Client and mapper paths
#define PATH_ROOT                                    (0)
#define PATH_DATA                                    (1)
#define PATH_ART                                     (2)
#define PATH_ART_CRITTERS                            (3)
#define PATH_ART_INTRFACE                            (4)
#define PATH_ART_INVEN                               (5)
#define PATH_ART_ITEMS                               (6)
#define PATH_ART_MISC                                (7)
#define PATH_ART_SCENERY                             (8)
#define PATH_ART_SKILLDEX                            (9)
#define PATH_ART_SPLASH                              (10)
#define PATH_ART_TILES                               (11)
#define PATH_ART_WALLS                               (12)
#define PATH_TEXTURES                                (13)
#define PATH_EFFECTS                                 (14)
#define PATH_SND_MUSIC                               (16)
#define PATH_SND_SFX                                 (17)
#define PATH_SCRIPTS                                 (18)
#define PATH_VIDEO                                   (19)
#define PATH_TEXTS                                   (20)
#define PATH_SAVE                                    (21)
#define PATH_FONTS                                   (22)
#define PATH_CACHE                                   (23)

// Server paths
#define PATH_SERVER_ROOT                             (30)
#define PATH_SERVER_DATA                             (31)
#define PATH_SERVER_TEXTS                            (32)
#define PATH_SERVER_DIALOGS                          (33)
#define PATH_SERVER_MAPS                             (34)
#define PATH_SERVER_PRO_ITEMS                        (35)
#define PATH_SERVER_PRO_CRITTERS                     (36)
#define PATH_SERVER_SCRIPTS                          (37)
#define PATH_SERVER_SAVE                             (38)
#define PATH_SERVER_CLIENTS                          (39)
#define PATH_SERVER_BANS                             (40)
#define PATH_SERVER_LOGS                             (41)
#define PATH_SERVER_DUMPS                            (42)
#define PATH_SERVER_PROFILER                         (43)

// Mapper paths
#define PATH_MAPPER_ROOT                             (45)
#define PATH_MAPPER_DATA                             (46)

#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate PT" )
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define PT_ROOT                                     (PATH_ROOT)
# define PT_DATA                                     (PATH_DATA)
# define PT_ART                                      (PATH_ART)
# define PT_ART_CRITTERS                             (PATH_ART_CRITTERS)
# define PT_ART_INTRFACE                             (PATH_ART_INTRFACE)
# define PT_ART_INVEN                                (PATH_ART_INVEN)
# define PT_ART_ITEMS                                (PATH_ART_ITEMS)
# define PT_ART_MISC                                 (PATH_ART_MISC)
# define PT_ART_SCENERY                              (PATH_ART_SCENERY)
# define PT_ART_SKILLDEX                             (PATH_ART_SKILLDEX)
# define PT_ART_SPLASH                               (PATH_ART_SPLASH)
# define PT_ART_TILES                                (PATH_ART_TILES)
# define PT_ART_WALLS                                (PATH_ART_WALLS)
# define PT_TEXTURES                                 (PATH_TEXTURES)
# define PT_EFFECTS                                  (PATH_EFFECTS)
# define PT_SND_MUSIC                                (PATH_SND_MUSIC)
# define PT_SND_SFX                                  (PATH_SND_SFX)
# define PT_SCRIPTS                                  (PATH_SCRIPTS)
# define PT_VIDEO                                    (PATH_VIDEO)
# define PT_TEXTS                                    (PATH_TEXTS)
# define PT_SAVE                                     (PATH_SAVE)
# define PT_FONTS                                    (PATH_FONTS)
# define PT_CACHE                                    (PATH_CACHE)
# define PT_SERVER_ROOT                              (PATH_SERVER_ROOT)
# define PT_SERVER_DATA                              (PATH_SERVER_DATA)
# define PT_SERVER_TEXTS                             (PATH_SERVER_TEXTS)
# define PT_SERVER_DIALOGS                           (PATH_SERVER_DIALOGS)
# define PT_SERVER_MAPS                              (PATH_SERVER_MAPS)
# define PT_SERVER_PRO_ITEMS                         (PATH_SERVER_PRO_ITEMS)
# define PT_SERVER_PRO_CRITTERS                      (PATH_SERVER_PRO_CRITTERS)
# define PT_SERVER_SCRIPTS                           (PATH_SERVER_SCRIPTS)
# define PT_SERVER_SAVE                              (PATH_SERVER_SAVE)
# define PT_SERVER_CLIENTS                           (PATH_SERVER_CLIENTS)
# define PT_SERVER_BANS                              (PATH_SERVER_BANS)
# define PT_SERVER_LOGS                              (PATH_SERVER_LOGS)
# define PT_SERVER_DUMPS                             (PATH_SERVER_DUMPS)
# define PT_SERVER_PROFILER                          (PATH_SERVER_PROFILER)
# define PT_MAPPER_ROOT                              (PATH_MAPPER_ROOT)
# define PT_MAPPER_DATA                              (PATH_MAPPER_DATA)
#endif // !FOCLASSIC_BLEEDING_EDGE

// Radio flags - Item::RadioFlags, ItemCl::RadioFlags
#define RADIO_DISABLE_SEND                           (0x01)
#define RADIO_DISABLE_RECV                           (0x02)
// Broadcast
#define RADIO_BROADCAST_WORLD                        (0)
#define RADIO_BROADCAST_MAP                          (20)
#define RADIO_BROADCAST_LOCATION                     (40)
#define RADIO_BROADCAST_FORCE_ALL                    (250)

#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define RADIO_BROADCAST_ZONE( x )                  (100 + CLAMP( x, 1, 100 ) ) // 1..100
# endif // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define RADIO_BROADCAST_ZONE                       # (x)(100 + CLAMP( x, 1, 100 ) ) // 1..100
# endif // FOCLASSIC_SCRIPT

#endif // FOCLASSIC_MACROS_ALLOWED

// AI Plane begin/end/run reasons
// Begin
#define REASON_GO_HOME                               (10)
#define REASON_FOUND_IN_ENEMY_STACK                  (11)
#define REASON_FROM_DIALOG                           (12)
#define REASON_FROM_SCRIPT                           (13)
#define REASON_RUN_AWAY                              (14)
// End
#define REASON_SUCCESS                               (30)
#define REASON_HEX_TOO_FAR                           (31)
#define REASON_HEX_BUSY                              (32)
#define REASON_HEX_BUSY_RING                         (33)
#define REASON_DEADLOCK                              (34)
#define REASON_TRACE_FAIL                            (35)
#define REASON_POSITION_NOT_FOUND                    (36)
#define REASON_FIND_PATH_ERROR                       (37)
#define REASON_CANT_WALK                             (38)
#define REASON_TARGET_DISAPPEARED                    (39)
#define REASON_USE_ITEM_NOT_FOUND                    (40)
#define REASON_GAG_CRITTER                           (41)
#define REASON_GAG_ITEM                              (42)
#define REASON_NO_UNARMED                            (43)
// Run
#define REASON_ATTACK_TARGET                         (50)
#define REASON_ATTACK_WEAPON                         (51)
#define REASON_ATTACK_DISTANTION                     (52)
#define REASON_ATTACK_USE_AIM                        (53)

// Slots
#define SLOT_INV                                     (0)
#define SLOT_HAND1                                   (1)
#define SLOT_HAND2                                   (2)
#define SLOT_ARMOR                                   (3)
#define SLOT_GROUND                                  (255)
#define SLOT_LAST                                    (SLOT_ARMOR) // scripts/extensions

// Say types
#define SAY_NORM                                     (1)
#define SAY_NORM_ON_HEAD                             (2)
#define SAY_SHOUT                                    (3)
#define SAY_SHOUT_ON_HEAD                            (4)
#define SAY_EMOTE                                    (5)
#define SAY_EMOTE_ON_HEAD                            (6)
#define SAY_WHISP                                    (7)
#define SAY_WHISP_ON_HEAD                            (8)
#define SAY_SOCIAL                                   (9)
#define SAY_RADIO                                    (10)
#define SAY_NETMSG                                   (11)
#define SAY_DIALOG                                   (12)
#define SAY_APPEND                                   (13)
#define SAY_ENCOUNTER_ANY                            (14)
#define SAY_ENCOUNTER_RT                             (15)
#define SAY_ENCOUNTER_TB                             (16)
#define SAY_FIX_RESULT                               (17)
#define SAY_DIALOGBOX_TEXT                           (18)
#define SAY_SAY_TITLE                                (39)
#define SAY_SAY_TEXT                                 (40)
#define SAY_FLASH_WINDOW                             (41)

#ifdef FOCLASSIC_MACROS_ALLOWED

// Max 20 buttons (0..19)

# ifdef FOCLASSIC_EXTENSION
#  define SAY_DIALOGBOX_BUTTON( b )                  (19 + (b) )
# endif   // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define SAY_DIALOGBOX_BUTTON                       # (b)(19 + (b) )
# endif   // FOCLASSIC_SCRIPT

#endif    // FOCLASSIC_MACROS_ALLOWED

// Special skill values
#define SKILL_PICK_ON_GROUND                         (-1)
#define SKILL_PUT_CONT                               (-2)
#define SKILL_TAKE_CONT                              (-3)
#define SKILL_TAKE_ALL_CONT                          (-4)
#define SKILL_LOOT_CRITTER                           (-5)
#define SKILL_PUSH_CRITTER                           (-6)
#define SKILL_TALK                                   (-7)

// Sounds
#define SOUND_BUTTON1_IN                             "BUTIN1"
#define SOUND_BUTTON2_IN                             "BUTIN2"
#define SOUND_BUTTON3_IN                             "BUTIN3"
#define SOUND_BUTTON4_IN                             "BUTIN4"
#define SOUND_BUTTON1_OUT                            "BUTOUT1"
#define SOUND_BUTTON2_OUT                            "BUTOUT2"
#define SOUND_BUTTON3_OUT                            "BUTOUT3"
#define SOUND_BUTTON4_OUT                            "BUTOUT4"
#define SOUND_LMENU                                  "IACCUXX1"
#define SOUND_COMBAT_MODE_ON                         "ICIBOXX1"
#define SOUND_COMBAT_MODE_OFF                        "ICIBCXX1"

// Target types
#define TARGET_SELF                                  (0)
#define TARGET_SELF_ITEM                             (1)
#define TARGET_CRITTER                               (2)
#define TARGET_ITEM                                  (3)
#define TARGET_SCENERY                               (4)
#define TARGET_LAST                                  (TARGET_SCENERY + 1) // scripts/extensions

#define TEXTMSG_TEXT                                 (0)
#define TEXTMSG_DLG                                  (1)
#define TEXTMSG_ITEM                                 (2)
#define TEXTMSG_GAME                                 (3)
#define TEXTMSG_GM                                   (4)
#define TEXTMSG_COMBAT                               (5)
#define TEXTMSG_QUEST                                (6)
#define TEXTMSG_HOLO                                 (7)
#define TEXTMSG_CRAFT                                (8)
#define TEXTMSG_INTERNAL                             (9)
#define TEXTMSG_MAX                                  (TEXTMSG_INTERNAL + 1) // array size

// Transfer types
#define TRANSFER_CLOSE                               (0)
#define TRANSFER_HEX_CONT_UP                         (1)
#define TRANSFER_HEX_CONT_DOWN                       (2)
#define TRANSFER_SELF_CONT                           (3)
#define TRANSFER_CRIT_LOOT                           (4)
#define TRANSFER_CRIT_STEAL                          (5)
#define TRANSFER_CRIT_BARTER                         (6)
#define TRANSFER_FAR_CONT                            (7)
#define TRANSFER_FAR_CRIT                            (8)

// Uses
#define USE_PRIMARY                                  (0)
#define USE_SECONDARY                                (1)
#define USE_THIRD                                    (2)
#define USE_RELOAD                                   (3)
#define USE_USE                                      (4)
#define MAX_USES                                     (3)
#define USE_NONE                                     (15)

#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define MAKE_ITEM_MODE( use, aim )                 ( ( ( (aim) << 4 ) | ( (use) & 0xF ) ) & 0xFF )
# endif   // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define MAKE_ITEM_MODE                             # (use, aim)( ( ( (aim) << 4 ) | ( (use) & 0xF ) ) & 0xFF )
# endif   // FOCLASSIC_SCRIPT

#endif    // FOCLASSIC_MACROS_ALLOWED

// Vars calc
#ifdef FOCLASSIC_MACROS_ALLOWED

# ifdef FOCLASSIC_EXTENSION
#  define VAR_CALC_QUEST( tid, val )                 ( (tid) * 1000 + (val) )
# endif   // FOCLASSIC_EXTENSION

# ifdef FOCLASSIC_SCRIPT
#  define VAR_CALC_QUEST                             # (tid, val)( (tid) * 1000 + (val) )
# endif   // FOCLASSIC_SCRIPT

#endif    // FOCLASSIC_MACROS_ALLOWED

// Vars types
#define VAR_TYPE_GLOBAL                              (0)
#define VAR_TYPE_LOCAL                               (1)
#define VAR_TYPE_UNICUM                              (2)
#define VAR_TYPE_LOCAL_LOCATION                      (3)
#define VAR_TYPE_LOCAL_MAP                           (4)
#define VAR_TYPE_LOCAL_ITEM                          (5)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate VAR_GLOBAL VAR_LOCAL VAR_UNICUM")
# endif
#endif   // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define VAR_GLOBAL                                  (VAR_TYPE_GLOBAL)
# define VAR_LOCAL                                   (VAR_TYPE_LOCAL)
# define VAR_UNICUM                                  (VAR_TYPE_UNICUM)
# define VAR_LOCAL_LOCATION                          (VAR_TYPE_LOCAL_LOCATION)
# define VAR_LOCAL_MAP                               (VAR_TYPE_LOCAL_MAP)
# define VAR_LOCAL_ITEM                              (VAR_TYPE_LOCAL_ITEM)
#endif   // !FOCLASSIC_BLEEDING_EDGE

// Var flags
#define VAR_FLAG_QUEST                               (0x01)
#define VAR_FLAG_RANDOM                              (0x02)
#define VAR_FLAG_NO_CHECK                            (0x04)

// Worldmap fog
#define WORLDMAP_FOG_FULL                            (0)
#define WORLDMAP_FOG_HALF                            (1)
#define WORLDMAP_FOG_HALF_EX                         (2)
#define WORLDMAP_FOG_NONE                            (3)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate GM_FOG")
# endif
#endif   // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define GM_FOG_FULL                                 (WORLDMAP_FOG_FULL)
# define GM_FOG_HALF                                 (WORLDMAP_FOG_HALF)
# define GM_FOG_HALF_EX                              (WORLDMAP_FOG_HALF_EX)
# define GM_FOG_NONE                                 (WORLDMAP_FOG_NONE)
#endif // !FOCLASSIC_BLEEDING_EDGE

// Worldmap events
#define WORLDMAP_PROCESS_MOVE                        (0)
#define WORLDMAP_PROCESS_ENTER                       (1)
#define WORLDMAP_PROCESS_START_FAST                  (2)
#define WORLDMAP_PROCESS_START                       (3)
#define WORLDMAP_PROCESS_SET_MOVE                    (4)
#define WORLDMAP_PROCESS_STOPPED                     (5)
#define WORLDMAP_PROCESS_NPC_IDLE                    (6)
#define WORLDMAP_PROCESS_KICK                        (7)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate GLOBAL_PROCESS")
# endif
#endif   // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define GLOBAL_PROCESS_MOVE                         (WORLDMAP_PROCESS_MOVE)
# define GLOBAL_PROCESS_ENTER                        (WORLDMAP_PROCESS_ENTER)
# define GLOBAL_PROCESS_START_FAST                   (WORLDMAP_PROCESS_START_FAST)
# define GLOBAL_PROCESS_START                        (WORLDMAP_PROCESS_START)
# define GLOBAL_PROCESS_SET_MOVE                     (WORLDMAP_PROCESS_SET_MOVE)
# define GLOBAL_PROCESS_STOPPED                      (WORLDMAP_PROCESS_STOPPED)
# define GLOBAL_PROCESS_NPC_IDLE                     (WORLDMAP_PROCESS_NPC_IDLE)
# define GLOBAL_PROCESS_KICK                         (WORLDMAP_PROCESS_KICK)
#endif   // !FOCLASSIC_BLEEDING_EDGE

// Worldmap walk types
#define WORLDMAP_WALK_GROUND                         (0)
#define WORLDMAP_WALK_FLY                            (1)
#define WORLDMAP_WALK_WATER                          (2)
#ifdef FOCLASSIC_ENGINE
# if FOCLASSIC_STAGE >= 3
#  pragma MESSAGE("[STAGE 3] deprecate GM")
# endif
#endif // FOCLASSIC_ENGINE
#ifndef FOCLASSIC_BLEEDING_EDGE
# define GM_WALK_GROUND                              (WORLDMAP_WALK_GROUND)
# define GM_WALK_FLY                                 (WORLDMAP_WALK_FLY)
# define GM_WALK_WATER                               (WORLDMAP_WALK_WATER)
#endif   // !FOCLASSIC_BLEEDING_EDGE

///// Items //////

#ifndef FOCLASSIC_SKIP_PID

# define PID_BOTTLE_CAPS                             (41)

#endif   // !FOCLASSIC_SKIP_PID

///// Parameters /////

#ifndef FOCLASSIC_SKIP_PARAM

// Stats
// Param[0..6] is always set to 5 on registration
# define ST_STRENGTH                                 (0)  // Weapon_MinStrength check
# define ST_PERCEPTION                               (1)  // Critter look = LookNormal + ST_PERCEPTION * 3 + ST_BONUS_LOOK + MultiHex
# define ST_CHARISMA                                 (3)  // MapManager::GM_LeaveGroup( Critter* cr )
# define ST_INTELLECT                                (4)  // Critter::IntellectCacheValue
# define ST_AGILITY                                  (5)  // Weapon_UnarmedMinAgility check
# define ST_MAX_LIFE                                 (7)  // ClientInterface.cpp
# define ST_ACTION_POINTS                            (8)
# define ST_ARMOR_CLASS                              (9)  // ClientInterface.cpp
# define ST_MELEE_DAMAGE                             (10) // ClientInterface.cpp
# define ST_CARRY_WEIGHT                             (11)
# define ST_SEQUENCE                                 (12) // ClientInterface.cpp
# define ST_HEALING_RATE                             (13) // ClientInterface.cpp
# define ST_CRITICAL_CHANCE                          (14) // ClientInterface.cpp
# define ST_NORMAL_RESIST                            (23) // ClientInterface.cpp
# define ST_RADIATION_RESISTANCE                     (30) // ClientInterface.cpp
# define ST_POISON_RESISTANCE                        (31) // ClientInterface.cpp
# define ST_AGE                                      (70) // ClientInterface.cpp, character registration
# define ST_GENDER                                   (71) // ClientInterface.cpp, character registration
# define ST_CURRENT_HP                               (72)
# define ST_POISONING_LEVEL                          (73) // ClientInterface.cpp
# define ST_RADIATION_LEVEL                          (74) // ClientInterface.cpp
# define ST_CURRENT_AP                               (75)
# define ST_EXPERIENCE                               (76)
# define ST_LEVEL                                    (77)
# define ST_UNSPENT_SKILL_POINTS                     (78)
# define ST_UNSPENT_PERKS                            (79)
# define ST_KARMA                                    (80) // ClientInterface.cpp
# define ST_FOLLOW_CRIT                              (81)
# define ST_REPLICATION_MONEY                        (82) // ClientInterface.cpp
# define ST_REPLICATION_COUNT                        (83) // ClientInterface.cpp
# define ST_REPLICATION_TIME                         (84)
# define ST_REPLICATION_COST                         (85) // ClientInterface.cpp
# define ST_TURN_BASED_AC                            (86)
# define ST_MAX_MOVE_AP                              (87)
# define ST_MOVE_AP                                  (88)
# define ST_NPC_ROLE                                 (89)
# define ST_BONUS_LOOK                               (101) // Critter look = LookNormal + ST_PERCEPTION * 3 + ST_BONUS_LOOK + MultiHex
# define ST_HANDS_ITEM_AND_MODE                      (102)
# define ST_FREE_BARTER_PLAYER                       (103)
# define ST_DIALOG_ID                                (104)
# define ST_AI_ID                                    (105) // Mapper.cpp, ProtoMap.cpp ('version < 6' block)
# define ST_TEAM_ID                                  (106)
# define ST_BAG_ID                                   (107)
# define ST_LAST_WEAPON_ID                           (110)
# define ST_BASE_CRTYPE                              (112) // Mapper.cpp
# define ST_TALK_DISTANCE                            (115)
# define ST_SCALE_FACTOR                             (116)
# define ST_WALK_TIME                                (117)
# define ST_RUN_TIME                                 (118)
# define ST_MAX_TALKERS                              (119)
# define ST_ANIM3D_LAYER_BEGIN                       (150)
# define ST_ANIM3D_LAYER_END                         (179)

// Skills
# define SK_UNARMED                                  (203)
# define SK_FIRST_AID                                (206)
# define SK_DOCTOR                                   (207)
# define SK_SNEAK                                    (208)
# define SK_LOCKPICK                                 (209)
# define SK_STEAL                                    (210)
# define SK_TRAPS                                    (211)
# define SK_SCIENCE                                  (212)
# define SK_REPAIR                                   (213)
# define SK_SPEECH                                   (214) // TalkTime = MAX( RAW(SK_SPEECH) ) * 1000, DlgTalkMinTime )
# define SK_BARTER                                   (215)

// Tagged skills
# define TAG_SKILL1                                  (226)
# define TAG_SKILL2                                  (227)
# define TAG_SKILL3                                  (228)
# define TAG_SKILL4                                  (229)

// Timeout
# define TO_SK_REPAIR                                (232)
# define TO_SK_SCIENCE                               (233)
# define TO_BATTLE                                   (238)
# define TO_TRANSFER                                 (239)
# define TO_REMOVE_FROM_GAME                         (240)
# define TO_KARMA_VOTING                             (242)

# define PE_SILENT_RUNNING                           (316)
# define PE_MASTER_TRADER                            (318)
# define PE_QUICK_POCKETS                            (349)

# define DAMAGE_POISONED                             (500) // ClientInterface.cpp
# define DAMAGE_RADIATED                             (501) // ClientInterface.cpp
# define DAMAGE_RIGHT_ARM                            (503)
# define DAMAGE_LEFT_ARM                             (504)
# define DAMAGE_RIGHT_LEG                            (505)
# define DAMAGE_LEFT_LEG                             (506)

// Modes
# define MODE_HIDE                                   (510)
# define MODE_NO_STEAL                               (511)
# define MODE_NO_BARTER                              (512)
# define MODE_NO_ENEMY_STACK                         (513)
# define MODE_NO_PVP                                 (514)
# define MODE_END_COMBAT                             (515)
# define MODE_DEFAULT_COMBAT                         (516)
# define MODE_NO_HOME                                (517)
# define MODE_GECK                                   (518)
# define MODE_NO_FAVORITE_ITEM                       (519)
# define MODE_NO_ITEM_GARBAGER                       (520)
# define MODE_DLG_SCRIPT_BARTER                      (521)
# define MODE_UNLIMITED_AMMO                         (522)
# define MODE_INVULNERABLE                           (527) // Critter::ToDead() does nothing
# define MODE_NO_FLATTEN                             (528) // dead && !RAW( MODE_NO_FLATTEN ) ? DRAW_ORDER_DEAD_CRITTER : DRAW_ORDER_CRITTER
# define MODE_RANGE_HTH                              (530)
# define MODE_NO_LOOT                                (532)
# define MODE_NO_PUSH                                (536)
# define MODE_NO_UNARMED                             (537)
# define MODE_NO_AIM                                 (538)
# define MODE_NO_WALK                                (539)
# define MODE_NO_RUN                                 (540)
# define MODE_NO_TALK                                (541)

#endif   // !FOCLASSIC_SKIP_PARAMS

// Cleanup
#ifndef FOCLASSIC_SCRIPT
# undef FOCLASSIC_MACROS_ALLOWED   // TODO scripts don't have #undef
#endif

#ifdef FOCLASSIC_ENGINE
# undef FOCLASSIC_EXTENSION
# undef FOCLASSIC_BLEEDING_EDGE
#endif   // FOCLASSIC_ENGINE

#endif   // __FOCLASSIC_FOS__ //
