/////
//
// FOClassic v@FOCLASSIC_VERSION@
// Timestamp @SCRIPTS_HEADER_TIMESTAMP@
//
/////
//
// This file contains all exposed 'magic numbers' used in FOClassic.
// It should not be edited directly by game developers.
//
// Used by engine, native extensions, and scripts.
//
/////
//
// FOCLASSIC_EXTENSION
// Enables macros, C++ syntax; cannot be used with FOCLASSIC_SCRIPT.
// Enables macros/values useful in C++ code only.
//
// FOCLASSIC_SCRIPT
// Enables macros, AngelScript syntax; cannot be used with FOCLASSIC_EXTENSION.
// They're weirdly formatted currently; uncrustify cannot correctly handle "#" between "MACRO_NAME" and "()"
//
// FOCLASSIC_BLEEDING_EDGE
// All values which are planned for removal/renaming in future, or related to experimental features, won't be defined.
//
// FOCLASSIC_SKIP_PARAM
// Critter parameters indexes won't be defined.
//
// FOCLASSIC_SKIP_PID
// Items prototypes IDs won't be defined.
//
/////

#ifndef __FOCLASSIC_FOS__
#define __FOCLASSIC_FOS__

/////
//
// Init
//
/////

// Detect engine
#ifdef FOCLASSIC_ENGINE
# if !defined (__DEFINES__)
#  include "Defines.h"
# endif
# define FOCLASSIC_EXTENSION
# define FOCLASSIC_BLEEDING_EDGE
#endif  // FOCLASSIC_ENGINE

// Detect bad setup
#ifndef FOCLASSIC_EXTENSION
# ifndef FOCLASSIC_SCRIPT
#  error "FOCLASSIC_EXTENSION or FOCLASSIC_SCRIPT must be defined"
# endif
#endif

// Detect bad setup
#ifdef FOCLASSIC_EXTENSION
# ifdef FOCLASSIC_SCRIPT
#  error "FOCLASSIC_EXTENSION and FOCLASSIC_SCRIPT cannot be both defined at same time"
# endif
#endif

/////
//
// Macros
//
/////


// macros for engine/extensions only
#ifdef FOCLASSIC_EXTENSION

# define ___TOD1( x )                                     # x
# define ___TOD0( x )                                     ___TOD1( x )
# define TODO( msg )                                      message( __FILE__ "(" ___TOD0( __LINE__ ) "): [TODO] "  # msg )
# define STAGE( stage, msg )                              message( __FILE__ "(" ___TOD0( __LINE__ ) "): [STAGE " ___TOD0( stage ) "] "  # msg )
# define DEPRECATE( msg )                                 message( __FILE__ "(" ___TOD0( __LINE__ ) "): [DEPRECATE] "  # msg )
# define STAGE_DEPRECATE( stage, msg )                    message( __FILE__ "(" ___TOD0( __LINE__ ) "): [STAGE " ___TOD0( stage ) "][DEPRECATE] "  # msg )
# define STATIC_ASSERT( a )                               static_assert( a, # a )
# define STATIC_ASSERT_PRINTF( function, a )              function( "STATIC_ASSERT( sizeof(%s) == %u );\n", # a, sizeof(a) )

# define memzero( ptr, size )                             memset( ptr, 0, size )
# define MAKEUINT( ch0, ch1, ch2, ch3 )                   ( (uint)(uint8)(ch0) | ( (uint)(uint8)(ch1) << 8 ) | ( (uint)(uint8)(ch2) << 16 ) | ( (uint)(uint8)(ch3) << 24 ) )
# define OFFSETOF( type, member )                         ( (int)offsetof( type, member ) )
# define PACKUINT64( u32hi, u32lo )                       ( ( (uint64)u32hi << 32 ) | ( (uint64)u32lo ) )

#endif   // FOCLASSIC_EXTENSION

// macros for everyone
#ifdef FOCLASSIC_EXTENSION

# define BIN__N( x )                                      (x) | x >> 3 | x >> 6 | x >> 9
# define BIN__B( x )                                      (x) & 0xf | (x) >> 12 & 0xf0
# define BIN8( v )                                        (BIN__B( BIN__N( 0x ## v ) ) )
# define BIN16( bin16, bin8 )                             ( (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )

# define CLAMP( x, low, high )                            ( ( (x) > (high) ) ? (high) : ( ( (x) < (low) ) ? (low) : (x) ) )
# define MIN( a, b )                                      ( ( (a) < (b) ) ? (a) : (b) )
# define MAX( a, b )                                      ( ( (a) > (b) ) ? (a) : (b) )

# define FLAG( x, y )                                     ( ( (x) & (y) ) != 0 )
# define SETFLAG( x, y )                                  ( (x) = (x) | (y) )
# define UNSETFLAG( x, y )                                ( (x) = ( (x) | (y) ) ^ (y) )

# define CONVERT_GRAMM( x )                               ( (x) * 453 )
# define IS_DIR_CORNER( dir )                             ( ( (dir) & 1 ) != 0 ) // 1, 3, 5, 7
# define RAD( deg )                                       ( (deg) * 3.141592654f / 180.0f )
# define UTF8_BUF_SIZE( count )                           ( (count) * 4 )

# ifdef FOCLASSIC_ENGINE
#  define GAME_OPTION( option )                           (GameOpt.option)
# else
#  define GAME_OPTION( option )                           (FOClassic->option)
# endif  // !FOCLASSIC_ENGINE

# ifdef FOCLASSIC_BLEEDING_EDGE
#  ifdef FOCLASSIC_ENGINE
#   define GAME_OPTION_EXT( option )                      (GameOptExt.option)
#  else
#   define GAME_OPTION_EXT( option )                      (FOClassicExt->option)
#  endif
# endif // !FOCLASSIC_BLEEDING_EDGE

#endif    // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT

# define BIN__N                                           # (x)(x) | x >> 3 | x >> 6 | x >> 9
# define BIN__B                                           # (x)(x) & 0xf | (x) >> 12 & 0xf0
# define BIN8                                             # (v)(BIN__B( BIN__N( 0x ## v ) ) )
# define BIN16                                            # (bin16, bin8)( (BIN8( bin16 ) << 8) | (BIN8( bin8 ) ) )

# define CLAMP                                            # (x, low, high)( ( (x) > (high) ) ? (high) : ( ( (x) < (low) ) ? (low) : (x) ) )
# define MIN                                              # (a, b)( ( (a) < (b) ) ? (a) : (b) )
# define MAX                                              # (a, b)( ( (a) > (b) ) ? (a) : (b) )

# define FLAG                                             # (x, y)( ( (x) & (y) ) != 0 )
# define SETFLAG                                          # (x, y)( (x) = (x) | (y) )
# define UNSETFLAG                                        # (x, y)( (x) = ( (x) | (y) ) ^ (y) )

# define CONVERT_GRAMM                                    # (x)( (x) * 453 )
# define IS_DIR_CORNER                                    # (dir)( ( (dir) & 1 ) != 0 ) // 1, 3, 5, 7
# define RAD                                              # (deg)( (deg) * 3.141592654f / 180.0f )
# define UTF8_BUF_SIZE                                    # (count)( (count) * 4 )

# define GAME_OPTION                                      # (option)(__ ## option) // TODO? separate globalvars registered by engine

#endif                                                                             // FOCLASSIC_SCRIPT

/////
//
// Configuration
//
/////

// main sections, available at runtime

#define SECTION_NONE                                      ""               // for settings which doesn't belong to any section
#define SECTION_CLIENT                                    "Game Options"   // main section
#define SECTION_MAPPER                                    "Mapper"         // main section
#define SECTION_SERVER                                    "Server"         // main section

// detail sections, available only when reading configuration from disk

#define SECTION_CLIENT_DX                                 "ClientDX" // detail section
#define SECTION_CLIENT_GL                                 "ClientGL" // detail section
#define SECTION_MAPPER_DX                                 "MapperDX" // detail section
#define SECTION_MAPPER_GL                                 "MapperGL" // detail section

// sections merged into ConfigFile, available at runtime

#define SECTION_SERVER_DIALOGS                            "Dialogs"        // from dialogs.cfg, Server only
#define SECTION_SERVER_SCRIPTS                            "Scripts"        // from scripts.cfg, Server only
#define SECTION_CLIENT_SCRIPTS_MODULES                    "Client scripts" // from scripts.cfg, Server only
#define SECTION_CLIENT_SCRIPTS_BINDS                      "Client binds"   // from scripts.cfg, Client/Server
#define SECTION_MAPPER_SCRIPTS_MODULES                    "Mapper scripts" // from scripts.cfg, Mapper/Server
#define SECTION_MAPPER_SCRIPTS_BINDS                      "Mapper binds"   // from scripts.cfg, Mapper/Server
#define SECTION_SERVER_SCRIPTS_MODULES                    "Server scripts" // from scripts.cfg, Server only
#define SECTION_SERVER_SCRIPTS_BINDS                      "Server binds"   // from scripts.cfg, Server only

/////
//
// Misc
//
/////

#define PI_FLOAT                                          (3.14159265f)
#define PI_VALUE                                          (3.141592654f)
#define PIBY2_FLOAT                                       (1.5707963f)
#define SQRT3T2_FLOAT                                     (3.4641016151f)
#define SQRT3_FLOAT                                       (1.732050807568877f)
#define BIAS_FLOAT                                        (0.02f)
#define RAD2DEG                                           (57.29577951f)

#define DIRS_COUNT                                        (GAME_OPTION( MapHexagonal ) ? 6 : 8)
#define LEXEMS_SIZE                                       (128)

/////
//
// Limits
//
//////

#define MAX_UINT8                                         (0xFF)
#define MAX_UINT16                                        (0xFFFF)
#define MIN_INT                                           (0x80000000)
#define MAX_INT                                           (0x7FFFFFFF)
#define MAX_UINT                                          (0xFFFFFFFF)

#define MAX_FOPATH                                        UTF8_BUF_SIZE( 1024 )
#define MAX_FOTEXT                                        UTF8_BUF_SIZE( 2048 )
#define MAX_LOGTEXT                                       UTF8_BUF_SIZE( 2048 )

// Used for name and password
#define MIN_NAME                                          (1)
#define MAX_NAME                                          (30)

// Character registration
#define MIN_AGE                                           (14)
#define MAX_AGE                                           (60)

#define MAX_DETERIORATION                                 (10000)

#define MAX_HOLO_INFO                                     (250) // array size

// Proto
#define MAX_PROTO_CRITTERS                                (10000)
#define MAX_PROTO_ITEMS                                   (30000)
#define MAX_PROTO_LOCATIONS                               (30000)
#define MAX_PROTO_MAPS                                    (30000)

#define MAX_SCRIPT_NAME                                   (64)

#define MAX_STORED_LOCATIONS                              (1000)

#define MIN_ZOOM                                          (0.2f)
#define MAX_ZOOM                                          (10.0f)

/////
//
// Magic
//
/////

// Access levels
#define ACCESS_CLIENT                                     (0)
#define ACCESS_TESTER                                     (1)
#define ACCESS_MODER                                      (2)
#define ACCESS_ADMIN                                      (3)
#define ACCESS_LAST                                       (ACCESS_ADMIN) // extensions/scripts

#ifdef FOCLASSIC_ENGINE
# ifdef DEV_VERSION
#  define ACCESS_DEFAULT                                  (ACCESS_ADMIN)
# else
#  define ACCESS_DEFAULT                                  (ACCESS_CLIENT)
# endif
#endif   // FOCLASSIC_ENGINE

// AI Planes
#define AI_PLANE_MISC                                     (0)
#define AI_PLANE_ATTACK                                   (1)
#define AI_PLANE_WALK                                     (2)
#define AI_PLANE_PICK                                     (3)
#define AI_PLANE_LAST                                     (AI_PLANE_PICK) // extensions/scripts

// AI Planes priorities
#define AI_PLANE_MISC_PRIORITY                            (10)            // unused
#define AI_PLANE_ATTACK_PRIORITY                          (50)
#define AI_PLANE_WALK_PRIORITY                            (20)
#define AI_PLANE_PICK_PRIORITY                            (35)            // unused

// Animation
#define ANIM_TYPE_FALLOUT                                 (0)
#define ANIM_TYPE_3D                                      (1)
#define ANIM_TYPE_LAST                                    (ANIM_TYPE_3D) // extensions/scripts

// Anim1
#define ANIM1_UNARMED                                     (1)
// Anim2
#define ANIM2_IDLE                                        (1)
#define ANIM2_WALK                                        (3)
#define ANIM2_LIMP                                        (4)
#define ANIM2_RUN                                         (5)
#define ANIM2_PANIC_RUN                                   (6)
#define ANIM2_SNEAK_WALK                                  (7)
#define ANIM2_SNEAK_RUN                                   (8)
#define ANIM2_IDLE_PRONE_FRONT                            (86)
#define ANIM2_IDLE_PRONE_BACK                             (87)
#define ANIM2_DEAD_FRONT                                  (102)
#define ANIM2_DEAD_BACK                                   (103)

// AngelScript properties
#define ANGELSCRIPT_ALLOW_UNSAFE_REFERENCES               (1) // engine default: true
#define ANGELSCRIPT_OPTIMIZE_BYTECODE                     (2) // engine default: true
#define ANGELSCRIPT_COPY_SCRIPT_SECTIONS                  (3)
#define ANGELSCRIPT_MAX_STACK_SIZE                        (4)
#define ANGELSCRIPT_USE_CHARACTER_LITERALS                (5) // engine default: 1
#define ANGELSCRIPT_ALLOW_MULTILINE_STRINGS               (6)
#define ANGELSCRIPT_ALLOW_IMPLICIT_HANDLE_TYPES           (7)
#define ANGELSCRIPT_BUILD_WITHOUT_LINE_CUES               (8)
#define ANGELSCRIPT_INIT_GLOBAL_VARS_AFTER_BUILD          (9)
#define ANGELSCRIPT_REQUIRE_ENUM_SCOPE                    (10)
#define ANGELSCRIPT_SCRIPT_SCANNER                        (11) // engine default: 1
#define ANGELSCRIPT_INCLUDE_JIT_INSTRUCTIONS              (12)
#define ANGELSCRIPT_STRING_ENCODING                       (13)
#define ANGELSCRIPT_PROPERTY_ACCESSOR_MODE                (14)
#define ANGELSCRIPT_EXPAND_DEF_ARRAY_TO_TMPL              (15)
#define ANGELSCRIPT_AUTO_GARBAGE_COLLECT                  (16) // engine default: false
#define ANGELSCRIPT_DISALLOW_GLOBAL_VARS                  (17)
#define ANGELSCRIPT_ALWAYS_IMPL_DEFAULT_CONSTRUCT         (18) // engine default: true
#define ANGELSCRIPT_COMPILER_WARNINGS                     (19) // engine default: 1
#define ANGELSCRIPT_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE    (20)

// Item deterioration info
#define BI_BROKEN                                         (0x0F)

// Binary files signatures
#ifdef FOCLASSIC_EXTENSION
# define BINARY_SIGNATURE( name, type, ver )              const unsigned char name[6] = { 'F', 'O', type, ( (ver) >> 8 ) & 0xFF, ( (ver) ) & 0xFF, 0 }
# define BINARY_SIGNATURE_VALID( sig1, sig2 )             (sig1[0] == sig2[0] && sig1[1] == sig2[1] && sig1[2] == sig2[2] && sig1[5] == sig2[5]) // skip version check
# define BINARY_SIGNATURE_VERSION( sig )                  ( (sig[3] << 8) | sig[4] )
#endif                                                                                                                                           // FOCLASSIC_EXTENSION

#define BINARY_TYPE_CLIENTSAVE                            'C'
#define BINARY_TYPE_MAPSAVE                               'M'
#define BINARY_TYPE_PROFILERSAVE                          'P'
#define BINARY_TYPE_SCRIPTSAVE                            'S'
#define BINARY_TYPE_WORLDSAVE                             'W'
#define BINARY_TYPE_CACHE                                 'c'  // reserved

// Chosen actions
#define CHOSEN_NONE                                       (0)  //
#define CHOSEN_MOVE                                       (1)  // HexX, HexY, Is run, Cut path, Wait double click, Double click tick
#define CHOSEN_MOVE_TO_CRITTER                            (2)  // Critter id, None, Is run, Cut path, Wait double click, Double click tick
#define CHOSEN_DIR                                        (3)  // 0 (CW) or 1 (CCW)
#define CHOSEN_SHOW_ITEM                                  (4)  // Item id
#define CHOSEN_HIDE_ITEM                                  (5)  // Item id
#define CHOSEN_USE_ITEM                                   (6)  // Item id, Item pid, Target type, Target id, Item mode, Some param (timer)
#define CHOSEN_MOVE_ITEM                                  (7)  // Item id, Item count, To slot, Is barter container, Is second try
#define CHOSEN_MOVE_ITEM_CONTAINER                        (8)  // From container, Item id, Count
#define CHOSEN_TAKE_ALL                                   (9)  //
#define CHOSEN_USE_SKILL_ON_CRITTER                       (10) // Skill, Critter id
#define CHOSEN_USE_SKILL_ON_ITEM                          (11) // Is inventory, Skill index, Item id
#define CHOSEN_USE_SKILL_ON_SCENERY                       (12) // Skill, Pid, HexX, HexY
#define CHOSEN_TALK_NPC                                   (13) // Critter id
#define CHOSEN_PICK_ITEM                                  (14) // Pid, HexX, HexY
#define CHOSEN_PICK_CRITTER                               (15) // Critter id, (loot - 0, push - 1)
#define CHOSEN_WRITE_HOLODISK                             (16) // Holodisk id

// Hardcoded screens
#define CLIENT_SCREEN_NONE                                (0)  // Used in primary, secondary and additional screen types
// Hardcoded screens, primary
#define CLIENT_MAIN_SCREEN_LOGIN                          (1)
#define CLIENT_MAIN_SCREEN_REGISTRATION                   (2)
#define CLIENT_MAIN_SCREEN_CREDITS                        (3)
#define CLIENT_MAIN_SCREEN_OPTIONS                        (4)
#define CLIENT_MAIN_SCREEN_GAME                           (5)
#define CLIENT_MAIN_SCREEN_WORLDMAP                       (6)
#define CLIENT_MAIN_SCREEN_WAIT                           (7)

// Hardcoded screens, secondary
#define CLIENT_SCREEN_INVENTORY                           (10)
#define CLIENT_SCREEN_PICKUP                              (11)
#define CLIENT_SCREEN_MINIMAP                             (12)
#define CLIENT_SCREEN_CHARACTER                           (13)
#define CLIENT_SCREEN_DIALOG                              (14)
#define CLIENT_SCREEN_BARTER                              (15)
#define CLIENT_SCREEN_PIPBOY                              (16)
#define CLIENT_SCREEN_FIXBOY                              (17)
#define CLIENT_SCREEN_MENU                                (18)
#define CLIENT_SCREEN_AIM                                 (19)
#define CLIENT_SCREEN_SPLIT                               (20)
#define CLIENT_SCREEN_TIMER                               (21)
#define CLIENT_SCREEN_DIALOGBOX                           (22)
#define CLIENT_SCREEN_ELEVATOR                            (23)
#define CLIENT_SCREEN_SAY                                 (24)
#define CLIENT_SCREEN_CHAR_NAME                           (25)
#define CLIENT_SCREEN_CHAR_AGE                            (26)
#define CLIENT_SCREEN_CHAR_SEX                            (27)
#define CLIENT_SCREEN_WM_TOWN                             (28)
#define CLIENT_SCREEN_INPUTBOX                            (29)
#define CLIENT_SCREEN_SKILLBOX                            (30)
#define CLIENT_SCREEN_USE                                 (31)
#define CLIENT_SCREEN_PERK                                (32)
#define CLIENT_SCREEN_WM_TOWNVIEW                         (33)
#define CLIENT_SCREEN_LOADSAVE                            (34)
#define CLIENT_SCREEN_LAST                                (CLIENT_SCREEN_LOADSAVE) // extensions/scripts

// Colors
// not included (see SpriteManager.h):
// COLOR_GAME_RGB(r,g,b), COLOR_IFACE, COLOR_IFACE_A(a), COLOR_IFACE_RED, COLOR_IFACE_GREEN

#ifdef FOCLASSIC_EXTENSION
# define COLOR_ARGB( a, r, g, b )                         ( (uint)( ( ( (a) & 0xFF ) << 24 ) | ( ( (r) & 0xFF ) << 16 ) | ( ( (g) & 0xFF ) << 8 ) | ( (b) & 0xFF ) ) )
# define COLOR_XRGB( r, g, b )                            COLOR_ARGB( 0xFF, r, g, b )
# define COLOR_CHANGE_ALPHA( v, a )                       ( ( ( (v) | 0xFF000000 ) ^ 0xFF000000 ) | ( (uint)(a) & 0xFF ) << 24 )
#endif    // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define COLOR_ARGB                                       # (a, r, g, b)(uint( ( ( (a) & 0xff ) << 24 ) | ( ( (r) & 0xff ) << 16 ) | ( ( (g) & 0xff ) << 8 ) | ( (b) & 0xff ) ) )
# define COLOR_XRGB                                       # (r, g, b)COLOR_ARGB( 0xff, r, g, b )
# define COLOR_CHANGE_ALPHA                               # (v, a)( ( ( (v) | 0xFF000000 ) ^ 0xFF000000 ) | (uint( a ) & 0xFF) << 24 )
#endif    // FOCLASSIC_SCRIPT

#define COLOR_IFACE_FIX                                   COLOR_XRGB( 103, 95, 86 )
#define COLOR_CRITTER_NAME                                COLOR_XRGB( 0xAD, 0xAD, 0xB9 )
#define COLOR_TEXT                                        COLOR_XRGB( 60, 248, 0 )
#define COLOR_TEXT_WHITE                                  COLOR_XRGB( 0xFF, 0xFF, 0xFF )
#define COLOR_TEXT_DWHITE                                 COLOR_XRGB( 0xBF, 0xBF, 0xBF )
#define COLOR_TEXT_RED                                    COLOR_XRGB( 0xC8, 0, 0 )
#define COLOR_TEXT_DRED                                   COLOR_XRGB( 0xAA, 0, 0 )
#define COLOR_TEXT_DDRED                                  COLOR_XRGB( 0x66, 0, 0 )
#define COLOR_TEXT_LRED                                   COLOR_XRGB( 0xFF, 0, 0 )
#define COLOR_TEXT_BLUE                                   COLOR_XRGB( 0, 0, 0xC8 )
#define COLOR_TEXT_DBLUE                                  COLOR_XRGB( 0, 0, 0xAA )
#define COLOR_TEXT_LBLUE                                  COLOR_XRGB( 0, 0, 0xFF )
#define COLOR_TEXT_GREEN                                  COLOR_XRGB( 0, 0xC8, 0 )
#define COLOR_TEXT_DGREEN                                 COLOR_XRGB( 0, 0xAA, 0 )
#define COLOR_TEXT_DDGREEN                                COLOR_XRGB( 0, 0x66, 0 )
#define COLOR_TEXT_LGREEN                                 COLOR_XRGB( 0, 0xFF, 0 )
#define COLOR_TEXT_BLACK                                  COLOR_XRGB( 0, 0, 0 )
#define COLOR_TEXT_SBLACK                                 COLOR_XRGB( 0x10, 0x10, 0x10 )
#define COLOR_TEXT_DARK                                   COLOR_XRGB( 0x30, 0x30, 0x30 )
#define COLOR_TEXT_GREEN_RED                              COLOR_XRGB( 0, 0xC8, 0xC8 )
#define COLOR_TEXT_SAND                                   COLOR_XRGB( 0x8F, 0x6F, 0 )

// Commands
#define COMMAND_EXIT                                      (1)
#define COMMAND_CHANGE_PASSWORD                           (2)
#define COMMAND_DELETE_ACCOUNT                            (3)
#define COMMAND_MYINFO                                    (4)
#define COMMAND_GETACCESS                                 (5)
#define COMMAND_PARAM                                     (6)
#define COMMAND_GAMEINFO                                  (7)
#define COMMAND_DROP_UID                                  (8)
#define COMMAND_CRITID                                    (9)
#define COMMAND_MOVECRIT                                  (10)
#define COMMAND_KILLCRIT                                  (11)
#define COMMAND_DISCONCRIT                                (12)
#define COMMAND_TOGLOBAL                                  (13)
#define COMMAND_RESPAWN                                   (14)
#define COMMAND_LOG                                       (15)
#define COMMAND_BAN                                       (16)
#define COMMAND_ADDITEM                                   (17)
#define COMMAND_ADDITEM_SELF                              (18)
#define COMMAND_ADDNPC                                    (19)
#define COMMAND_ADDLOCATION                               (20)
#define COMMAND_LOADSCRIPT                                (21)
#define COMMAND_RELOADSCRIPTS                             (22) // investigate
#define COMMAND_RELOAD_CLIENT_SCRIPTS                     (23)
#define COMMAND_RUNSCRIPT                                 (24)
#define COMMAND_LOADLOCATION                              (25)
#define COMMAND_RELOADLOCATIONS                           (26) // investigate
#define COMMAND_LOADMAP                                   (27)
#define COMMAND_RELOADMAPS                                (28) // investigate
#define COMMAND_REGENMAP                                  (29)
#define COMMAND_LOADDIALOG                                (30)
#define COMMAND_RELOADDIALOGS                             (31) // investigate
#define COMMAND_RELOADTEXTS                               (32) // investigate
#define COMMAND_RELOADAI                                  (33) // investigate
#define COMMAND_CHECKVAR                                  (34)
#define COMMAND_SETVAR                                    (35)
#define COMMAND_SETTIME                                   (36)

// Combat modes
#define COMBAT_MODE_ANY                                   (0)
#define COMBAT_MODE_REAL_TIME                             (1)
#define COMBAT_MODE_TURN_BASED                            (2)

// Constants collections
#define CONSTANTS_PARAM                                   (0)
#define CONSTANTS_ITEM                                    (1)
#define CONSTANTS_DEFINE                                  (2)
#define CONSTANTS_PICTURE                                 (3)
#define CONSTANTS_HASH                                    (4)

// Take flags
#define CONTAINER_GET                                     (1)
#define CONTAINER_PUT                                     (2)
#define CONTAINER_GETALL                                  (3)
#define CONTAINER_PUTALL                                  (4)

// Contour types
#define CONTOUR_RED                                       (1)
#define CONTOUR_YELLOW                                    (2)
#define CONTOUR_CUSTOM                                    (3)

// Corner type
#define CORNER_NORTH_SOUTH                                (0)
#define CORNER_WEST                                       (1)
#define CORNER_EAST                                       (2)
#define CORNER_SOUTH                                      (3)
#define CORNER_NORTH                                      (4)
#define CORNER_EAST_WEST                                  (5)

// Critter actions
//
// investigate: CRITTER_ACTION_RUN, CRITTER_ACTION_DODGE, CRITTER_ACTION_DAMAGE, CRITTER_ACTION_DAMAGE
//
// Flags for chosen:
// l - hardcoded local call
// s - hardcoded server call
// for all others critters actions call only server
//
#define CRITTER_ACTION_MOVE                               (0)       // l
#define CRITTER_ACTION_RUN                                (1)       // l
#define CRITTER_ACTION_MOVE_ITEM                          (2)       // l s      from slot                                                      +
#define CRITTER_ACTION_MOVE_ITEM_SWAP                     (3)       // l s      from slot                                                      +
#define CRITTER_ACTION_USE_ITEM                           (4)       // l s                                                                     +
#define CRITTER_ACTION_DROP_ITEM                          (5)       // l s      from slot                                                      +
#define CRITTER_ACTION_USE_WEAPON                         (6)       // l        fail attack 8 bit, use index (0-2) 4-7 bits, aim 0-3 bits      +
#define CRITTER_ACTION_RELOAD_WEAPON                      (7)       // l s                                                                     +
#define CRITTER_ACTION_USE_SKILL                          (8)       // l s      skill index (see SK_*)
#define CRITTER_ACTION_PICK_ITEM                          (9)       // l s                                                                     +
#define CRITTER_ACTION_PICK_CRITTER                       (10)      // l        0 - loot, 1 - steal, 2 - push
#define CRITTER_ACTION_OPERATE_CONTAINER                  (11)      // l s      transfer type * 10 + [0 - get, 1 - get all, 2 - put]           + (exclude get all)
#define CRITTER_ACTION_BARTER                             (12)      //   s      0 - item taken, 1 - item given                                 +
#define CRITTER_ACTION_DODGE                              (13)      //          0 - front, 1 - back
#define CRITTER_ACTION_DAMAGE                             (14)      //          0 - front, 1 - back
#define CRITTER_ACTION_DAMAGE_FORCE                       (15)      //          0 - front, 1 - back
#define CRITTER_ACTION_KNOCKOUT                           (16)      //   s      0 - knockout anim2begin
#define CRITTER_ACTION_STANDUP                            (17)      //   s      0 - knockout anim2end
#define CRITTER_ACTION_FIDGET                             (18)      // l
#define CRITTER_ACTION_DEAD                               (19)      //   s      dead type anim2 (see Anim2 in _animation.fos)
#define CRITTER_ACTION_CONNECT                            (20)      //
#define CRITTER_ACTION_DISCONNECT                         (21)      //
#define CRITTER_ACTION_RESPAWN                            (22)      //   s
#define CRITTER_ACTION_REFRESH                            (23)      //   s

// Critter conditions
#define CRITTER_CONDITION_LIFE                            (1)
#define CRITTER_CONDITION_KNOCKOUT                        (2)
#define CRITTER_CONDITION_DEAD                            (3)

// Critter events
#define CRITTER_EVENT_IDLE                                (0)
#define CRITTER_EVENT_FINISH                              (1)
#define CRITTER_EVENT_DEAD                                (2)
#define CRITTER_EVENT_RESPAWN                             (3)
#define CRITTER_EVENT_SHOW_CRITTER                        (4)
#define CRITTER_EVENT_SHOW_CRITTER_1                      (5)
#define CRITTER_EVENT_SHOW_CRITTER_2                      (6)
#define CRITTER_EVENT_SHOW_CRITTER_3                      (7)
#define CRITTER_EVENT_HIDE_CRITTER                        (8)
#define CRITTER_EVENT_HIDE_CRITTER_1                      (9)
#define CRITTER_EVENT_HIDE_CRITTER_2                      (10)
#define CRITTER_EVENT_HIDE_CRITTER_3                      (11)
#define CRITTER_EVENT_SHOW_ITEM_ON_MAP                    (12)
#define CRITTER_EVENT_CHANGE_ITEM_ON_MAP                  (13)
#define CRITTER_EVENT_HIDE_ITEM_ON_MAP                    (14)
#define CRITTER_EVENT_ATTACK                              (15)
#define CRITTER_EVENT_ATTACKED                            (16)
#define CRITTER_EVENT_STEALING                            (17)
#define CRITTER_EVENT_MESSAGE                             (18)
#define CRITTER_EVENT_USE_ITEM                            (19)
#define CRITTER_EVENT_USE_ITEM_ON_ME                      (20)
#define CRITTER_EVENT_USE_SKILL                           (21)
#define CRITTER_EVENT_USE_SKILL_ON_ME                     (22)
#define CRITTER_EVENT_DROP_ITEM                           (23)
#define CRITTER_EVENT_MOVE_ITEM                           (24)
#define CRITTER_EVENT_KNOCKOUT                            (25)
#define CRITTER_EVENT_SMTH_DEAD                           (26)
#define CRITTER_EVENT_SMTH_STEALING                       (27)
#define CRITTER_EVENT_SMTH_ATTACK                         (28)
#define CRITTER_EVENT_SMTH_ATTACKED                       (29)
#define CRITTER_EVENT_SMTH_USE_ITEM                       (30)
#define CRITTER_EVENT_SMTH_USE_SKILL                      (31)
#define CRITTER_EVENT_SMTH_DROP_ITEM                      (32)
#define CRITTER_EVENT_SMTH_MOVE_ITEM                      (33)
#define CRITTER_EVENT_SMTH_KNOCKOUT                       (34)
#define CRITTER_EVENT_PLANE_BEGIN                         (35)
#define CRITTER_EVENT_PLANE_END                           (36)
#define CRITTER_EVENT_PLANE_RUN                           (37)
#define CRITTER_EVENT_BARTER                              (38)
#define CRITTER_EVENT_TALK                                (39)
#define CRITTER_EVENT_GLOBAL_PROCESS                      (40)
#define CRITTER_EVENT_GLOBAL_INVITE                       (41)
#define CRITTER_EVENT_TURN_BASED_PROCESS                  (42)
#define CRITTER_EVENT_SMTH_TURN_BASED_PROCESS             (43)
#define CRITTER_EVENT_MAX                                 (44)

// Critters flags
#define CRITTER_FLAG_PLAYER                               (0x00010000) // Player
#define CRITTER_FLAG_NPC                                  (0x00020000) // Npc
#define CRITTER_FLAG_DISCONNECT                           (0x00080000) // In offline
#define CRITTER_FLAG_CHOSEN                               (0x00100000) // Chosen
#define CRITTER_FLAG_RULEGROUP                            (0x00200000) // Group rule

// Critters ID
#define CRITTER_ID_START_PLAYER                           (1)
#define CRITTER_ID_START_NPC                              (5000001)

#ifdef FOCLASSIC_EXTENSION
# define CRITTER_ID_IS_NPC( id )                          ( (id) >= CRITTER_ID_START_NPC )
# define CRITTER_ID_IS_PLAYER( id )                       ( (id) >= CRITTER_ID_START_PLAYER && (id) < CRITTER_ID_START_NPC )
#endif    // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define CRITTER_ID_IS_NPC                                # (id)( (id) >= CRITTER_ID_START_NPC )
# define CRITTER_ID_IS_PLAYER                             # (id)( (id) >= CRITTER_ID_START_PLAYER && (id) < CRITTER_ID_START_NPC )
#endif    // FOCLASSIC_SCRIPT

// Critter description
#define CRITTER_LOOK_NAME                                 (0)
#define CRITTER_LOOK_SHORT                                (1)
#define CRITTER_LOOK_FULL                                 (2)

// Cur modes
#define CURSOR_DEFAULT                                    (0)
#define CURSOR_MOVE                                       (1)
#define CURSOR_USE_ITEM                                   (2)
#define CURSOR_USE_WEAPON                                 (3)
#define CURSOR_USE_SKILL                                  (4)
#define CURSOR_WAIT                                       (5)
#define CURSOR_HAND                                       (6)

// Damage types
#define DAMAGE_TYPE_UNCALLED                              (0)
#define DAMAGE_TYPE_NORMAL                                (1)
#define DAMAGE_TYPE_LASER                                 (2)
#define DAMAGE_TYPE_FIRE                                  (3)
#define DAMAGE_TYPE_PLASMA                                (4)
#define DAMAGE_TYPE_ELECTR                                (5)
#define DAMAGE_TYPE_EMP                                   (6)
#define DAMAGE_TYPE_EXPLODE                               (7)

// Inventory descriptions
#define DESC_INVENTORY_MAIN                               (0)
#define DESC_INVENTORY_SPECIAL                            (1)
#define DESC_INVENTORY_STATS                              (2)
#define DESC_INVENTORY_RESIST                             (3)

// Keyboard key codes
#ifndef FOCLASSIC_ENGINE
# define DIK_ESCAPE                                       0x01
# define DIK_1                                            0x02
# define DIK_2                                            0x03
# define DIK_3                                            0x04
# define DIK_4                                            0x05
# define DIK_5                                            0x06
# define DIK_6                                            0x07
# define DIK_7                                            0x08
# define DIK_8                                            0x09
# define DIK_9                                            0x0A
# define DIK_0                                            0x0B
# define DIK_MINUS                                        0x0C // - on main keyboard
# define DIK_EQUALS                                       0x0D
# define DIK_BACK                                         0x0E // backspace
# define DIK_BACKSPACE                                    DIK_BACK
# define DIK_TAB                                          0x0F
# define DIK_Q                                            0x10
# define DIK_W                                            0x11
# define DIK_E                                            0x12
# define DIK_R                                            0x13
# define DIK_T                                            0x14
# define DIK_Y                                            0x15
# define DIK_U                                            0x16
# define DIK_I                                            0x17
# define DIK_O                                            0x18
# define DIK_P                                            0x19
# define DIK_LBRACKET                                     0x1A
# define DIK_RBRACKET                                     0x1B
# define DIK_RETURN                                       0x1C // enter on main keyboard
# define DIK_ENTER                                        DIK_RETURN
# define DIK_LCONTROL                                     0x1D
# define DIK_A                                            0x1E
# define DIK_S                                            0x1F
# define DIK_D                                            0x20
# define DIK_F                                            0x21
# define DIK_G                                            0x22
# define DIK_H                                            0x23
# define DIK_J                                            0x24
# define DIK_K                                            0x25
# define DIK_L                                            0x26
# define DIK_SEMICOLON                                    0x27
# define DIK_APOSTROPHE                                   0x28
# define DIK_GRAVE                                        0x29 // accent grave
# define DIK_LSHIFT                                       0x2A
# define DIK_BACKSLASH                                    0x2B
# define DIK_Z                                            0x2C
# define DIK_X                                            0x2D
# define DIK_C                                            0x2E
# define DIK_V                                            0x2F
# define DIK_B                                            0x30
# define DIK_N                                            0x31
# define DIK_M                                            0x32
# define DIK_COMMA                                        0x33 // ,
# define DIK_PERIOD                                       0x34 // . on main keyboard
# define DIK_SLASH                                        0x35 // / on main keyboard
# define DIK_RSHIFT                                       0x36
# define DIK_MULTIPLY                                     0x37 // * on numeric keypad
# define DIK_LMENU                                        0x38 // left alt */
# define DIK_SPACE                                        0x39
# define DIK_CAPITAL                                      0x3A // caps lock
# define DIK_F1                                           0x3B
# define DIK_F2                                           0x3C
# define DIK_F3                                           0x3D
# define DIK_F4                                           0x3E
# define DIK_F5                                           0x3F
# define DIK_F6                                           0x40
# define DIK_F7                                           0x41
# define DIK_F8                                           0x42
# define DIK_F9                                           0x43
# define DIK_F10                                          0x44
# define DIK_NUMLOCK                                      0x45
# define DIK_SCROLL                                       0x46 // scroll lock
# define DIK_NUMPAD7                                      0x47
# define DIK_NUMPAD8                                      0x48
# define DIK_NUMPAD9                                      0x49
# define DIK_SUBTRACT                                     0x4A // - on numeric keypad
# define DIK_NUMPAD4                                      0x4B
# define DIK_NUMPAD5                                      0x4C
# define DIK_NUMPAD6                                      0x4D
# define DIK_ADD                                          0x4E // + on numeric keypad
# define DIK_NUMPAD1                                      0x4F
# define DIK_NUMPAD2                                      0x50
# define DIK_NUMPAD3                                      0x51
# define DIK_NUMPAD0                                      0x52
# define DIK_DECIMAL                                      0x53 // . on numeric keypad
# define DIK_F11                                          0x57
# define DIK_F12                                          0x58
# define DIK_NUMPADENTER                                  0x9C // enter on numeric keypad
# define DIK_RCONTROL                                     0x9D
# define DIK_DIVIDE                                       0xB5 // / on numeric keypad
# define DIK_SYSRQ                                        0xB7
# define DIK_RMENU                                        0xB8 // right alt
# define DIK_PAUSE                                        0xC5 // pause
# define DIK_HOME                                         0xC7 // home on arrow keypad
# define DIK_UP                                           0xC8 // up arrow on arrow keypad
# define DIK_PRIOR                                        0xC9 // page up on arrow keypad
# define DIK_LEFT                                         0xCB // left arrow on arrow keypad
# define DIK_RIGHT                                        0xCD // right arrow on arrow keypad
# define DIK_END                                          0xCF // end on arrow keypad
# define DIK_DOWN                                         0xD0 // down arrow on arrow keypad
# define DIK_NEXT                                         0xD1 // page down on arrow keypad
# define DIK_INSERT                                       0xD2 // insert on arrow keypad
# define DIK_DELETE                                       0xD3 // delete on arrow keypad
# define DIK_LWIN                                         0xDB // left windows key
# define DIK_RWIN                                         0xDC // right windows key
# define DIK_CLIPBOARD_PASTE                              0xFF
#endif // !FOCLASSIC_ENGINE

// Sprite layers
#ifdef FOCLASSIC_ENGINE
# define DRAW_ORDER                                       (20)
# define DRAW_ORDER_FLAT                                  (0)
# define DRAW_ORDER_TILE                                  (DRAW_ORDER_FLAT + 0)
# define DRAW_ORDER_TILE_END                              (DRAW_ORDER_FLAT + 4)
# define DRAW_ORDER_HEX_GRID                              (DRAW_ORDER_FLAT + 5)
# define DRAW_ORDER_FLAT_SCENERY                          (DRAW_ORDER_FLAT + 8)
# define DRAW_ORDER_LIGHT                                 (DRAW_ORDER_FLAT + 9)
# define DRAW_ORDER_DEAD_CRITTER                          (DRAW_ORDER_FLAT + 10)
# define DRAW_ORDER_FLAT_ITEM                             (DRAW_ORDER_FLAT + 13)
# define DRAW_ORDER_TRACK                                 (DRAW_ORDER_FLAT + 16)
# define DRAW_ORDER_SCENERY                               (DRAW_ORDER + 3)
# define DRAW_ORDER_ITEM                                  (DRAW_ORDER + 6)
# define DRAW_ORDER_CRITTER                               (DRAW_ORDER + 9)
# define DRAW_ORDER_RAIN                                  (DRAW_ORDER + 12)
# define DRAW_ORDER_LAST                                  (39)
# define DRAW_ORDER_ITEM_AUTO( i )                        (i->IsFlat() ? (i->IsItem() ? DRAW_ORDER_FLAT_ITEM : DRAW_ORDER_FLAT_SCENERY) : (i->IsItem() ? DRAW_ORDER_ITEM : DRAW_ORDER_SCENERY) )
# define DRAW_ORDER_CRIT_AUTO( c )                        (c->IsDead() && !c->IsRawParam( MODE_NO_FLATTEN ) ? DRAW_ORDER_DEAD_CRITTER : DRAW_ORDER_CRITTER)
#endif   // FOCLASSIC_ENGINE

// Primitives
#define DRAW_PRIMITIVE_POINTLIST                          (1)
#define DRAW_PRIMITIVE_LINELIST                           (2)
#define DRAW_PRIMITIVE_LINESTRIP                          (3)
#define DRAW_PRIMITIVE_TRIANGLELIST                       (4)
#define DRAW_PRIMITIVE_TRIANGLESTRIP                      (5)
#define DRAW_PRIMITIVE_TRIANGLEFAN                        (6)

// Entires
#define ENTIRE_DEFAULT                                    (0)
#define ENTIRE_LOG_OUT                                    (241)

// Critter finding flags
#define FIND_LIFE                                         (0x01)
#define FIND_KO                                           (0x02)
#define FIND_DEAD                                         (0x04)
#define FIND_ONLY_PLAYERS                                 (0x10)
#define FIND_ONLY_NPC                                     (0x20)
#ifndef FOCLASSIC_ENGINE
# define FIND_LIFE_AND_KO                                 (FIND_LIFE | FIND_KO)
# define FIND_LIFE_AND_DEAD                               (FIND_LIFE | FIND_DEAD)
# define FIND_KO_AND_DEAD                                 (FIND_KO | FIND_DEAD)
# define FIND_ALL                                         (FIND_LIFE | FIND_KO | FIND_DEAD)
#endif   // !FOCLASSIC_ENGINE

// FixBoy function call states
#define FIXBOY_LIST                                       (0)
#define FIXBOY_BUTTON                                     (1)
#define FIXBOY_CRAFT                                      (2)

// FixBoy craft results
#define FIXBOY_ALLOW_CRAFT                                (0x0001)
#define FIXBOY_CHECK_TIMEOUT                              (0x0002)
#define FIXBOY_SET_TIMEOUT                                (0x0004)
#define FIXBOY_CHECK_PARAMS                               (0x0008)
#define FIXBOY_CHECK_MATERIALS                            (0x0010)
#define FIXBOY_CHECK_TOOLS                                (0x0020)
#define FIXBOY_SUB_MATERIALS                              (0x0040)
#define FIXBOY_ADD_CRAFT_ITEMS                            (0x0080)
#define FIXBOY_ADD_EXPERIENCE                             (0x0100)
#define FIXBOY_SEND_SUCC_MESSAGE                          (0x0200)
#define FIXBOY_SEND_FAIL_MESSAGE                          (0x0400)
#define FIXBOY_DEFAULT                                    (0xFFFF)

// Font flags
#define FONT_FLAG_NOBREAK                                 (0x0001)
#define FONT_FLAG_NOBREAK_LINE                            (0x0002)
#define FONT_FLAG_CENTERX                                 (0x0004)
#define FONT_FLAG_CENTERY                                 (0x0008)
#define FONT_FLAG_CENTERR                                 (0x0010)
#define FONT_FLAG_BOTTOM                                  (0x0020)
#define FONT_FLAG_UPPER                                   (0x0040)
#define FONT_FLAG_NO_COLORIZE                             (0x0080)
#define FONT_FLAG_ALIGN                                   (0x0100)
#define FONT_FLAG_BORDERED                                (0x0200)
#ifndef FOCLASSIC_ENGINE
# define FONT_FLAG_CENTERXY                               (FONT_FLAG_CENTERX | FONT_FLAG_CENTERY)
#endif   // !FOCLASSIC_ENGINE

#ifdef FOCLASSIC_EXTENSION
# define FONT_FLAG_SKIPLINES( l )                         (0x0400 | ( (l) << 16 ) )
# define FONT_FLAG_SKIPLINES_END( l )                     (0x0800 | ( (l) << 16 ) )
#endif   // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define FONT_FLAG_SKIPLINES                              # (l)(0x0400 | ( (l) << 16 ) )
# define FONT_FLAG_SKIPLINES_END                          # (l)(0x0800 | ( (l) << 16 ) )
#endif   // FOCLASSIC_SCRIPT

// Fonts
#define FONT_TYPE_FO                                      (0)
#define FONT_TYPE_NUM                                     (1)
#define FONT_TYPE_BIG_NUM                                 (2)
#define FONT_TYPE_SAND_NUM                                (3)
#define FONT_TYPE_SPECIAL                                 (4)
#define FONT_TYPE_DEFAULT                                 (5)
#define FONT_TYPE_THIN                                    (6)
#define FONT_TYPE_FAT                                     (7)
#define FONT_TYPE_BIG                                     (8)

// Additional fonts
#define FONT_TYPE_DIALOG                                  (9) // used on dialog/barter screen only

#define GENDER_MALE                                       (0)
#define GENDER_FEMALE                                     (1)

// Grid Types
#define GRID_EXITGRID                                     (1)
#define GRID_STAIRS                                       (2)
#define GRID_LADDERBOT                                    (3)
#define GRID_LADDERTOP                                    (4)
#define GRID_ELEVATOR                                     (5)

// Hex flags
#define HEX_FLAG_BLOCK                                    BIN8( 00000001 ) // ProtoMap
#define HEX_FLAG_NOTRAKE                                  BIN8( 00000010 ) // ProtoMap
#define HEX_FLAG_WALL                                     BIN8( 00000100 ) // ProtoMap
#define HEX_FLAG_SCEN                                     BIN8( 00001000 ) // ProtoMap
#define HEX_FLAG_SCEN_GRID                                BIN8( 00010000 ) // ProtoMap
#define HEX_FLAG_TRIGGER                                  BIN8( 00100000 ) // ProtoMap
#define HEX_FLAG_CRITTER                                  BIN8( 00000001 ) // Map
#define HEX_FLAG_DEAD_CRITTER                             BIN8( 00000010 ) // Map
#define HEX_FLAG_ITEM                                     BIN8( 00000100 ) // Map
#define HEX_FLAG_DOOR                                     BIN8( 00001000 ) // Map
#define HEX_FLAG_BLOCK_ITEM                               BIN8( 00010000 ) // Map
#define HEX_FLAG_NRAKE_ITEM                               BIN8( 00100000 ) // Map
#define HEX_FLAG_WALK_ITEM                                BIN8( 01000000 ) // Map
#define HEX_FLAG_GAG_ITEM                                 BIN8( 10000000 ) // Map

#define HEX_FLAG_NOWAY                                    BIN16( 00010001, 00000001 )
#define HEX_FLAG_NOSHOOT                                  BIN16( 00100000, 00000010 )

// Hit locations
#define HIT_LOCATION_NONE                                 (0)
#define HIT_LOCATION_HEAD                                 (1)
#define HIT_LOCATION_LEFT_ARM                             (2)
#define HIT_LOCATION_RIGHT_ARM                            (3)
#define HIT_LOCATION_TORSO                                (4)
#define HIT_LOCATION_RIGHT_LEG                            (5)
#define HIT_LOCATION_LEFT_LEG                             (6)
#define HIT_LOCATION_EYES                                 (7)
#define HIT_LOCATION_GROIN                                (8)
#define HIT_LOCATION_UNCALLED                             (9)
#define HIT_LOCATION_MAX                                  (HIT_LOCATION_UNCALLED)

// IndicatorType
#define INDICATOR_LINES                                   (0)
#define INDICATOR_NUMBERS                                 (1)
#define INDICATOR_BOTH                                    (2)

// Accessory
#define ITEM_ACCESSORY_NONE                               (0)
#define ITEM_ACCESSORY_CRITTER                            (1)
#define ITEM_ACCESSORY_HEX                                (2)
#define ITEM_ACCESSORY_CONTAINER                          (3)

// Items collections
#define ITEM_COLLECTION_INVENTORY                         (0)
#define ITEM_COLLECTION_USE                               (1)
#define ITEM_COLLECTION_BARTER                            (2)
#define ITEM_COLLECTION_BARTER_OFFER                      (3)
#define ITEM_COLLECTION_BARTER_OPPONENT                   (4)
#define ITEM_COLLECTION_BARTER_OPPONENT_OFFER             (5)
#define ITEM_COLLECTION_PICKUP                            (6)
#define ITEM_COLLECTION_PICKUP_FROM                       (7)

// Item data masks
#define ITEM_DATA_MASK_CHOSEN                             (0)
#define ITEM_DATA_MASK_CRITTER                            (1)
#define ITEM_DATA_MASK_CRITTER_EXT                        (2)
#define ITEM_DATA_MASK_CONTAINER                          (3)
#define ITEM_DATA_MASK_MAP                                (4)
#define ITEM_DATA_MASK_MAX                                (ITEM_DATA_MASK_MAP + 1) // array size

// Item events
#define ITEM_EVENT_FINISH                                 (0)
#define ITEM_EVENT_ATTACK                                 (1)
#define ITEM_EVENT_USE                                    (2)
#define ITEM_EVENT_USE_ON_ME                              (3)
#define ITEM_EVENT_SKILL                                  (4)
#define ITEM_EVENT_DROP                                   (5)
#define ITEM_EVENT_MOVE                                   (6)
#define ITEM_EVENT_WALK                                   (7)
#define ITEM_EVENT_MAX                                    (ITEM_EVENT_WALK + 1) // array size

// Item flags
#define ITEM_FLAG_HIDDEN                                  (0x00000001)
#define ITEM_FLAG_FLAT                                    (0x00000002)
#define ITEM_FLAG_NO_BLOCK                                (0x00000004)
#define ITEM_FLAG_SHOOT_THRU                              (0x00000008)
#define ITEM_FLAG_LIGHT_THRU                              (0x00000010)
#define ITEM_FLAG_MULTI_HEX                               (0x00000020) // unused
#define ITEM_FLAG_WALL_TRANS_END                          (0x00000040) // unused
#define ITEM_FLAG_TWO_HANDS                               (0x00000080)
#define ITEM_FLAG_BIG_GUN                                 (0x00000100)
#define ITEM_FLAG_ALWAYS_VIEW                             (0x00000200)
#define ITEM_FLAG_HAS_TIMER                               (0x00000400)
#define ITEM_FLAG_BAD_ITEM                                (0x00000800)
#define ITEM_FLAG_NO_HIGHLIGHT                            (0x00001000)
#define ITEM_FLAG_SHOW_ANIM                               (0x00002000)
#define ITEM_FLAG_SHOW_ANIM_EXT                           (0x00004000)
#define ITEM_FLAG_LIGHT                                   (0x00008000)
#define ITEM_FLAG_GECK                                    (0x00010000)
#define ITEM_FLAG_TRAP                                    (0x00020000)
#define ITEM_FLAG_NO_LIGHT_INFLUENCE                      (0x00040000)
#define ITEM_FLAG_NO_LOOT                                 (0x00080000)
#define ITEM_FLAG_NO_STEAL                                (0x00100000)
#define ITEM_FLAG_GAG                                     (0x00200000)
#define ITEM_FLAG_COLORIZE                                (0x00400000)
#define ITEM_FLAG_COLORIZE_INV                            (0x00800000)
#define ITEM_FLAG_CAN_USE_ON_SMTH                         (0x01000000)
#define ITEM_FLAG_CAN_LOOK                                (0x02000000) // unused, investigate
#define ITEM_FLAG_CAN_TALK                                (0x04000000)
#define ITEM_FLAG_CAN_PICKUP                              (0x08000000)
#define ITEM_FLAG_CAN_USE                                 (0x10000000)
#define ITEM_FLAG_HOLODISK                                (0x20000000)
#define ITEM_FLAG_RADIO                                   (0x40000000)
#define ITEM_FLAG_CACHED                                  (0x80000000) // unused

#define ITEM_LOOK_DEFAULT                                 (0)
#define ITEM_LOOK_ONLY_NAME                               (1)
#define ITEM_LOOK_MAP                                     (2)
#define ITEM_LOOK_BARTER                                  (3)
#define ITEM_LOOK_INVENTORY                               (4)
#define ITEM_LOOK_WM_CAR                                  (5)

// Types
#define ITEM_TYPE_OTHER                                   (0)
#define ITEM_TYPE_ARMOR                                   (1)
#define ITEM_TYPE_DRUG                                    (2) // stacked
#define ITEM_TYPE_WEAPON                                  (3) // combined
#define ITEM_TYPE_AMMO                                    (4) // stacked
#define ITEM_TYPE_MISC                                    (5) // combined
#define ITEM_TYPE_KEY                                     (7)
#define ITEM_TYPE_CONTAINER                               (8)
#define ITEM_TYPE_DOOR                                    (9)
#define ITEM_TYPE_GRID                                    (10)
#define ITEM_TYPE_GENERIC                                 (11)
#define ITEM_TYPE_WALL                                    (12)
#define ITEM_TYPE_CAR                                     (13)
#define ITEM_TYPE_LAST                                    (ITEM_TYPE_CAR)      // extensions/scripts
#define ITEM_TYPE_MAX                                     (ITEM_TYPE_LAST + 1) // array size

// Script events
#define LOCATION_EVENT_FINISH                             (0)
#define LOCATION_EVENT_ENTER                              (1)
#define LOCATION_EVENT_MAX                                (LOCATION_EVENT_ENTER + 1) // array size

// Locker
#define LOCKER_ISOPEN                                     (0x01)
#define LOCKER_NOOPEN                                     (0x10)

// Look checks
#define LOOK_CHECK_DIR                                    (0x01) // Nerf look by directions of look
#define LOOK_CHECK_SNEAK_DIR                              (0x02) // Nerf opponent sneak by directions of look
#define LOOK_CHECK_SNEAK_WEIGHT                           (0x04) // Nerf sneak by current weight
#define LOOK_CHECK_TRACE                                  (0x08) // Tracing for wall aviablility
#define LOOK_CHECK_SCRIPT                                 (0x10) // Use bool check_look() from scripts
#define LOOK_CHECK_ITEM_SCRIPT                            (0x20) // Use bool check_trap_look() from scripts, for items with ITEM_TRAP flag

// Script events
#define MAP_EVENT_FINISH                                  (0)
#define MAP_EVENT_LOOP_0                                  (1)
#define MAP_EVENT_LOOP_1                                  (2)
#define MAP_EVENT_LOOP_2                                  (3)
#define MAP_EVENT_LOOP_3                                  (4)
#define MAP_EVENT_LOOP_4                                  (5)
#define MAP_EVENT_IN_CRITTER                              (6)
#define MAP_EVENT_OUT_CRITTER                             (7)
#define MAP_EVENT_CRITTER_DEAD                            (8)
#define MAP_EVENT_TURN_BASED_BEGIN                        (9)
#define MAP_EVENT_TURN_BASED_END                          (10)
#define MAP_EVENT_TURN_BASED_PROCESS                      (11)
#define MAP_EVENT_MAX                                     (MAP_EVENT_TURN_BASED_PROCESS + 1) // aray size

// Map object types
#define MAP_OBJECT_CRITTER                                (0)
#define MAP_OBJECT_ITEM                                   (1)
#define MAP_OBJECT_SCENERY                                (2)

// Material
#ifndef FOCLASSIC_ENGINE
# define MATERIAL_GLASS                                   (0)
# define MATERIAL_METAL                                   (1)
# define MATERIAL_PLASTIC                                 (2)
# define MATERIAL_WOOD                                    (3)
# define MATERIAL_DIRT                                    (4)
# define MATERIAL_STONE                                   (5)
# define MATERIAL_CEMENT                                  (6)
# define MATERIAL_LEATHER                                 (7)
#endif   // !FOCLASSIC_ENGINE

// Mouse click state
#define MOUSE_CLICK_LEFT                                  (0)
#define MOUSE_CLICK_RIGHT                                 (1)
#define MOUSE_CLICK_MIDDLE                                (2)
#define MOUSE_CLICK_WHEEL_UP                              (3)
#define MOUSE_CLICK_WHEEL_DOWN                            (4)
#define MOUSE_CLICK_EXT0                                  (5)
#define MOUSE_CLICK_EXT1                                  (6)
#define MOUSE_CLICK_EXT2                                  (7)
#define MOUSE_CLICK_EXT3                                  (8)
#define MOUSE_CLICK_EXT4                                  (9)

// Message box
#define MSGBOX_GAME                                       (0)
#define MSGBOX_TALK                                       (1)
#define MSGBOX_COMBAT_RESULT                              (2)
#define MSGBOX_VIEW                                       (3)

// Client and mapper paths
#define PATH_ROOT                                         (0)
#define PATH_DATA                                         (1)
#define PATH_ART                                          (2)
#define PATH_ART_CRITTERS                                 (3)
#define PATH_ART_INTRFACE                                 (4)
#define PATH_ART_INVEN                                    (5)
#define PATH_ART_ITEMS                                    (6)
#define PATH_ART_MISC                                     (7)
#define PATH_ART_SCENERY                                  (8)
#define PATH_ART_SKILLDEX                                 (9)
#define PATH_ART_SPLASH                                   (10)
#define PATH_ART_TILES                                    (11)
#define PATH_ART_WALLS                                    (12)
#define PATH_TEXTURES                                     (13)
#define PATH_EFFECTS                                      (14)
#define PATH_SND_MUSIC                                    (16)
#define PATH_SND_SFX                                      (17)
#define PATH_SCRIPTS                                      (18)
#define PATH_VIDEO                                        (19)
#define PATH_TEXTS                                        (20)
#define PATH_SAVE                                         (21)
#define PATH_FONTS                                        (22)
#define PATH_CACHE                                        (23)

// Server paths
#define PATH_SERVER_ROOT                                  (30)
#define PATH_SERVER_DATA                                  (31)
#define PATH_SERVER_TEXTS                                 (32)
#define PATH_SERVER_DIALOGS                               (33)
#define PATH_SERVER_MAPS                                  (34)
#define PATH_SERVER_PRO_ITEMS                             (35)
#define PATH_SERVER_PRO_CRITTERS                          (36)
#define PATH_SERVER_SCRIPTS                               (37)
#define PATH_SERVER_SAVE                                  (38)
#define PATH_SERVER_CLIENTS                               (39)
#define PATH_SERVER_BANS                                  (40)
#define PATH_SERVER_LOGS                                  (41)
#define PATH_SERVER_DUMPS                                 (42)
#define PATH_SERVER_PROFILER                              (43)

// Mapper paths
#define PATH_MAPPER_ROOT                                  (45)
#define PATH_MAPPER_DATA                                  (46)

// Radio flags - Item::RadioFlags, ItemCl::RadioFlags
#define RADIO_DISABLE_SEND                                (0x01)
#define RADIO_DISABLE_RECV                                (0x02)
// Broadcast
#define RADIO_BROADCAST_WORLD                             (0)
#define RADIO_BROADCAST_MAP                               (20)
#define RADIO_BROADCAST_LOCATION                          (40)
#define RADIO_BROADCAST_FORCE_ALL                         (250)

#ifdef FOCLASSIC_EXTENSION
# define RADIO_BROADCAST_ZONE( x )                        (100 + CLAMP( x, 1, 100 ) )      // 1..100
#endif                                                                                     // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define RADIO_BROADCAST_ZONE                             # (x)(100 + CLAMP( x, 1, 100 ) ) // 1..100
#endif                                                                                     // FOCLASSIC_SCRIPT

// AI Plane begin/end/run reasons
// Begin
#define REASON_GO_HOME                                    (10)
#define REASON_FOUND_IN_ENEMY_STACK                       (11)
#define REASON_FROM_DIALOG                                (12)
#define REASON_FROM_SCRIPT                                (13)
#define REASON_RUN_AWAY                                   (14)
// End
#define REASON_SUCCESS                                    (30)
#define REASON_HEX_TOO_FAR                                (31)
#define REASON_HEX_BUSY                                   (32)
#define REASON_HEX_BUSY_RING                              (33)
#define REASON_DEADLOCK                                   (34)
#define REASON_TRACE_FAIL                                 (35)
#define REASON_POSITION_NOT_FOUND                         (36)
#define REASON_FIND_PATH_ERROR                            (37)
#define REASON_CANT_WALK                                  (38)
#define REASON_TARGET_DISAPPEARED                         (39)
#define REASON_USE_ITEM_NOT_FOUND                         (40)
#define REASON_GAG_CRITTER                                (41)
#define REASON_GAG_ITEM                                   (42)
#define REASON_NO_UNARMED                                 (43)
// Run
#define REASON_ATTACK_TARGET                              (50)
#define REASON_ATTACK_WEAPON                              (51)
#define REASON_ATTACK_DISTANTION                          (52)
#define REASON_ATTACK_USE_AIM                             (53)

// Say types
#define SAY_NORM                                          (1) // public
#define SAY_NORM_ON_HEAD                                  (2) // public
#define SAY_SHOUT                                         (3) // public
#define SAY_SHOUT_ON_HEAD                                 (4) // public
#define SAY_EMOTE                                         (5) // public
#define SAY_EMOTE_ON_HEAD                                 (6) // public
#define SAY_WHISP                                         (7) // public
#define SAY_WHISP_ON_HEAD                                 (8) // public
#define SAY_SOCIAL                                        (9)
#define SAY_RADIO                                         (10)
#define SAY_NETMSG                                        (11)
#define SAY_DIALOG                                        (12)
#define SAY_APPEND                                        (13)
#define SAY_ENCOUNTER_ANY                                 (14)
#define SAY_ENCOUNTER_RT                                  (15)
#define SAY_ENCOUNTER_TB                                  (16)
#define SAY_FIX_RESULT                                    (17)
#define SAY_DIALOGBOX_TEXT                                (18)
// 19..38 SAY_DIALOGBOX_BUTTON
#define SAY_SAY_TITLE                                     (39)
#define SAY_SAY_TEXT                                      (40)
#define SAY_FLASH_WINDOW                                  (41)

// Max 20 buttons (0..19)

#ifdef FOCLASSIC_EXTENSION
# define SAY_DIALOGBOX_BUTTON( b )                        (19 + (b) )
#endif    // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define SAY_DIALOGBOX_BUTTON                             # (b)(19 + (b) )
#endif    // FOCLASSIC_SCRIPT

// Scores
#define SCORE_SPEAKER                                     (3)
#define SCORE_TRADER                                      (4)
#define SCORES_MAX                                        (50)

// Scroll elements
#define SCROLL_MESSBOX                                    (0)
#define SCROLL_INVENTORY                                  (1)
#define SCROLL_INVENTORY_ITEM_INFO                        (2)
#define SCROLL_PICKUP                                     (3)
#define SCROLL_PICKUP_FROM                                (4)
#define SCROLL_USE                                        (5)
#define SCROLL_BARTER                                     (6)
#define SCROLL_BARTER_OFFER                               (7)
#define SCROLL_BARTER_OPPONENT                            (8)
#define SCROLL_BARTER_OPPONENT_OFFER                      (9)
#define SCROLL_GLOBAL_MAP_CITIES_X                        (10)
#define SCROLL_GLOBAL_MAP_CITIES_Y                        (11)
#define SCROLL_SPLIT_VALUE                                (12)
#define SCROLL_TIMER_VALUE                                (13)
#define SCROLL_PERK                                       (14)
#define SCROLL_DIALOG_TEXT                                (15)
#define SCROLL_MAP_ZOOM_VALUE                             (16)
#define SCROLL_CHARACTER_PERKS                            (17)
#define SCROLL_CHARACTER_KARMA                            (18)
#define SCROLL_CHARACTER_KILLS                            (19)
#define SCROLL_PIPBOY_STATUS                              (20)
#define SCROLL_PIPBOY_STATUS_QUESTS                       (21)
#define SCROLL_PIPBOY_STATUS_SCORES                       (22)
#define SCROLL_PIPBOY_AUTOMAPS                            (23)
#define SCROLL_PIPBOY_ARCHIVES                            (24)
#define SCROLL_PIPBOY_ARCHIVES_INFO                       (25)

// SendMessage()
#define SENDMESSAGE_TO_VISIBLE_ME                         (0)
#define SENDMESSAGE_TO_IAM_VISIBLE                        (1)
#define SENDMESSAGE_TO_ALL_ON_MAP                         (2)

// Slots
#define SLOT_INV                                          (0)
#define SLOT_HAND1                                        (1)
#define SLOT_HAND2                                        (2)
#define SLOT_ARMOR                                        (3)
#define SLOT_GROUND                                       (255)
#define SLOT_LAST                                         (SLOT_ARMOR) // extensions/scripts

// Show screen modes
// Ouput: it is 'uint param' in Critter::ShowScreen.
// Input: I - integer value 'uint answerI', S - string value 'string& answerS' in 'answer_' function.
#define SHOW_SCREEN_CLOSE                                 (0)  // Close top window.
#define SHOW_SCREEN_TIMER                                 (1)  // Timer box. Output: picture index in INVEN.LST. Input I: time in game minutes (1..599).
#define SHOW_SCREEN_DIALOGBOX                             (2)  // Dialog box. Output: buttons count - 0..20 (exit button added automatically). Input I: Choosed button - 0..19.
#define SHOW_SCREEN_SKILLBOX                              (3)  // Skill box. Input I: selected skill.
#define SHOW_SCREEN_BAG                                   (4)  // Bag box. Input I: id of selected item.
#define SHOW_SCREEN_SAY                                   (5)  // Say box. Output: all symbols - 0 or only numbers - any other number. Input S: typed string.
#define SHOW_SCREEN_ELEVATOR                              (6)  // Elevator. Output: look ELEVATOR_* macro. Input I: Choosed level button.
#define SHOW_SCREEN_INVENTORY                             (7)  // Inventory.
#define SHOW_SCREEN_CHARACTER                             (8)  // Character.
#define SHOW_SCREEN_FIXBOY                                (9)  // Fix-boy.
#define SHOW_SCREEN_PIPBOY                                (10) // Pip-boy.
#define SHOW_SCREEN_MINIMAP                               (11) // Mini-map.

// Special skill values
#define SKILL_PICK_ON_GROUND                              (-1)
#define SKILL_PUT_CONT                                    (-2)
#define SKILL_TAKE_CONT                                   (-3)
#define SKILL_TAKE_ALL_CONT                               (-4)
#define SKILL_LOOT_CRITTER                                (-5)
#define SKILL_PUSH_CRITTER                                (-6)
#define SKILL_TALK                                        (-7)

// Sounds
#define SOUND_BUTTON1_IN                                  "BUTIN1"
#define SOUND_BUTTON2_IN                                  "BUTIN2"
#define SOUND_BUTTON3_IN                                  "BUTIN3"
#define SOUND_BUTTON4_IN                                  "BUTIN4"
#define SOUND_BUTTON1_OUT                                 "BUTOUT1"
#define SOUND_BUTTON2_OUT                                 "BUTOUT2"
#define SOUND_BUTTON3_OUT                                 "BUTOUT3"
#define SOUND_BUTTON4_OUT                                 "BUTOUT4"
#define SOUND_LMENU                                       "IACCUXX1"
#define SOUND_COMBAT_MODE_ON                              "ICIBOXX1"
#define SOUND_COMBAT_MODE_OFF                             "ICIBCXX1"

// Sprites cutting
#define SPRITE_CUT_HORIZONTAL                             (1)
#define SPRITE_CUT_VERTICAL                               (2)

// Target types
#define TARGET_SELF                                       (0)
#define TARGET_SELF_ITEM                                  (1)
#define TARGET_CRITTER                                    (2)
#define TARGET_ITEM                                       (3)
#define TARGET_SCENERY                                    (4)

#define TEXTMSG_TEXT                                      (0)
#define TEXTMSG_DLG                                       (1)
#define TEXTMSG_ITEM                                      (2)
#define TEXTMSG_GAME                                      (3)
#define TEXTMSG_GM                                        (4)
#define TEXTMSG_COMBAT                                    (5)
#define TEXTMSG_QUEST                                     (6)
#define TEXTMSG_HOLO                                      (7)
#define TEXTMSG_CRAFT                                     (8)
#define TEXTMSG_INTERNAL                                  (9)
#define TEXTMSG_MAX                                       (TEXTMSG_INTERNAL + 1) // array size

// Transfer types
#define TRANSFER_CLOSE                                    (0)
#define TRANSFER_HEX_CONT_UP                              (1)
#define TRANSFER_HEX_CONT_DOWN                            (2)
#define TRANSFER_SELF_CONT                                (3)
#define TRANSFER_CRIT_LOOT                                (4)
#define TRANSFER_CRIT_STEAL                               (5)
#define TRANSFER_CRIT_BARTER                              (6)
#define TRANSFER_FAR_CONT                                 (7)
#define TRANSFER_FAR_CRIT                                 (8)

// Uses
#define USE_PRIMARY                                       (0)
#define USE_SECONDARY                                     (1)
#define USE_THIRD                                         (2)
#define USE_RELOAD                                        (3)
#define USE_USE                                           (4)
#define USE_NONE                                          (15)
#define USE_MAX                                           (3) // array size

#ifdef FOCLASSIC_EXTENSION
# define MAKE_ITEM_MODE( use, aim )                       ( ( ( (aim) << 4 ) | ( (use) & 0xF ) ) & 0xFF )
#endif    // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define MAKE_ITEM_MODE                                   # (use, aim)( ( ( (aim) << 4 ) | ( (use) & 0xF ) ) & 0xFF )
#endif    // FOCLASSIC_SCRIPT

// User data
#define CRITTER_USER_DATA_SIZE                            (400)
#define PROTO_ITEM_USER_DATA_SIZE                         (500)

// Vars calc
#ifdef FOCLASSIC_EXTENSION
# define VAR_CALC_QUEST( tid, val )                       ( (tid) * 1000 + (val) )
#endif    // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define VAR_CALC_QUEST                                   # (tid, val)( (tid) * 1000 + (val) )
#endif    // FOCLASSIC_SCRIPT

// Vars types
#define VAR_TYPE_GLOBAL                                   (0)
#define VAR_TYPE_LOCAL                                    (1)
#define VAR_TYPE_UNICUM                                   (2)
#define VAR_TYPE_LOCAL_LOCATION                           (3)
#define VAR_TYPE_LOCAL_MAP                                (4)
#define VAR_TYPE_LOCAL_ITEM                               (5)

// Var flags
#define VAR_FLAG_QUEST                                    (0x01)
#define VAR_FLAG_RANDOM                                   (0x02)
#define VAR_FLAG_NO_CHECK                                 (0x04)

// Worldmap fog
#define WORLDMAP_FOG_FULL                                 (0)
#define WORLDMAP_FOG_HALF                                 (1)
#define WORLDMAP_FOG_HALF_EX                              (2)
#define WORLDMAP_FOG_NONE                                 (3)

// Worldmap events
#define WORLDMAP_PROCESS_MOVE                             (0)
#define WORLDMAP_PROCESS_ENTER                            (1)
#define WORLDMAP_PROCESS_START_FAST                       (2)
#define WORLDMAP_PROCESS_START                            (3)
#define WORLDMAP_PROCESS_SET_MOVE                         (4)
#define WORLDMAP_PROCESS_STOPPED                          (5)
#define WORLDMAP_PROCESS_NPC_IDLE                         (6)
#define WORLDMAP_PROCESS_KICK                             (7)

// Worldmap walk types
#define WORLDMAP_WALK_GROUND                              (0)
#define WORLDMAP_WALK_FLY                                 (1)
#define WORLDMAP_WALK_WATER                               (2)

/////
//
// Prototype ID
//
/////

#ifndef FOCLASSIC_SKIP_PID

# define PID_BOTTLE_CAPS                                  (41)

#endif   // !FOCLASSIC_SKIP_PID

/////
//
// Parameters
//
/////

// Limits
#define MAX_PARAMS                                        (1000) // array size
#define MAX_PARAMETERS_ARRAYS                             (100)  // array size

// Skill
#define SKILL_BEGIN                                       (GAME_OPTION( SkillBegin ) )
#define SKILL_END                                         (GAME_OPTION( SkillEnd ) )
#define SKILL_COUNT                                       (SKILL_END - SKILL_BEGIN + 1)
#define MAX_SKILL_VAL                                     (GAME_OPTION( SkillMaxValue ) )

// Perk
#define PERK_BEGIN                                        (GAME_OPTION( PerkBegin ) )
#define PERK_END                                          (GAME_OPTION( PerkEnd ) )
#define PERK_COUNT                                        (PERK_END - PERK_BEGIN + 1)

// Timeout
#define TIMEOUT_BEGIN                                     (GAME_OPTION( TimeoutBegin ) )
#define TIMEOUT_END                                       (GAME_OPTION( TimeoutEnd ) )
#define TIMEOUT_COUNT                                     (TIMEOUT_END - TIMEOUT_BEGIN + 1)
#define TB_BATTLE_TIMEOUT                                 (100000000)
#define TB_BATTLE_TIMEOUT_CHECK( to )                     ( (to) > 10000000 )
// Kill
#define KILL_BEGIN                                        (GAME_OPTION( KillBegin ) )
#define KILL_END                                          (GAME_OPTION( KillEnd ) )

// Addiction
#define ADDICTION_BEGIN                                   (GAME_OPTION( AddictionBegin ) )
#define ADDICTION_END                                     (GAME_OPTION( AddictionEnd ) )

// Karma
#define KARMA_BEGIN                                       (GAME_OPTION( KarmaBegin ) )
#define KARMA_END                                         (GAME_OPTION( KarmaEnd ) )

// Damage
#define DAMAGE_BEGIN                                      (GAME_OPTION( DamageBegin ) )
#define DAMAGE_END                                        (GAME_OPTION( DamageEnd ) )

// Trait
#define TRAIT_BEGIN                                       (GAME_OPTION( TraitBegin ) )
#define TRAIT_END                                         (GAME_OPTION( TraitEnd ) )
#define TRAIT_COUNT                                       (TRAIT_END - TRAIT_BEGIN + 1)

// Reputation
#define REPUTATION_BEGIN                                  (GAME_OPTION( ReputationBegin ) )
#define REPUTATION_END                                    (GAME_OPTION( ReputationEnd ) )

#ifdef FOCLASSIC_EXTENSION
# ifndef FOCLASSIC_ENGINE
#  define SKILL_OFFSET( skill )                           ( (skill) + (GAME_OPTION( AbsoluteOffsets ) ? 0 : SKILL_BEGIN) ) // investigate
#  define PERK_OFFSET( perk )                             ( (perk)  + (GAME_OPTION( AbsoluteOffsets ) ? 0 : PERK_BEGIN) )  // investigate
# endif                                                                                                                    // !FOCLASSIC_ENGINE
#endif                                                                                                                     // FOCLASSIC_EXTENSION

#ifdef FOCLASSIC_SCRIPT
# define SKILL_OFFSET                                     # (skill)( (skill) + (GAME_OPTION( AbsoluteOffsets ) ? 0 : SKILL_BEGIN) )
# define PERK_OFFSET                                      # (perk)( (perk)  + (GAME_OPTION( AbsoluteOffsets ) ? 0 : PERK_BEGIN) )
#endif   // FOCLASSIC_SCRIPT


#ifndef FOCLASSIC_SKIP_PARAM

// Stats
// Param[0..6] is always set to 5 on registration (without defines used)
# define ST_STRENGTH                                      (0)  // Weapon_MinStrength check
# define ST_PERCEPTION                                    (1)  // Critter look = LookNormal + ST_PERCEPTION * 3 + ST_BONUS_LOOK + MultiHex
# define ST_CHARISMA                                      (3)  // MapManager::GM_LeaveGroup( Critter* cr )
# define ST_INTELLECT                                     (4)  // Critter::IntellectCacheValue
# define ST_AGILITY                                       (5)  // Weapon_UnarmedMinAgility check
# define ST_MAX_LIFE                                      (7)  // ClientInterface.cpp
# define ST_ACTION_POINTS                                 (8)
# define ST_ARMOR_CLASS                                   (9)  // ClientInterface.cpp
# define ST_MELEE_DAMAGE                                  (10) // ClientInterface.cpp
# define ST_CARRY_WEIGHT                                  (11)
# define ST_SEQUENCE                                      (12) // ClientInterface.cpp
# define ST_HEALING_RATE                                  (13) // ClientInterface.cpp
# define ST_CRITICAL_CHANCE                               (14) // ClientInterface.cpp
# define ST_NORMAL_RESIST                                 (23) // ClientInterface.cpp
# define ST_RADIATION_RESISTANCE                          (30) // ClientInterface.cpp
# define ST_POISON_RESISTANCE                             (31) // ClientInterface.cpp
# define ST_AGE                                           (70) // ClientInterface.cpp, character registration
# define ST_GENDER                                        (71) // ClientInterface.cpp, character registration
# define ST_CURRENT_HP                                    (72)
# define ST_POISONING_LEVEL                               (73) // ClientInterface.cpp
# define ST_RADIATION_LEVEL                               (74) // ClientInterface.cpp
# define ST_CURRENT_AP                                    (75)
# define ST_EXPERIENCE                                    (76)
# define ST_LEVEL                                         (77)
# define ST_UNSPENT_SKILL_POINTS                          (78)
# define ST_UNSPENT_PERKS                                 (79)
# define ST_KARMA                                         (80) // ClientInterface.cpp
# define ST_FOLLOW_CRIT                                   (81)
# define ST_REPLICATION_MONEY                             (82) // ClientInterface.cpp
# define ST_REPLICATION_COUNT                             (83) // ClientInterface.cpp
# define ST_REPLICATION_TIME                              (84)
# define ST_REPLICATION_COST                              (85) // ClientInterface.cpp
# define ST_TURN_BASED_AC                                 (86)
# define ST_MAX_MOVE_AP                                   (87)
# define ST_MOVE_AP                                       (88)
# define ST_NPC_ROLE                                      (89)
# define ST_BONUS_LOOK                                    (101) // Critter look = LookNormal + ST_PERCEPTION * 3 + ST_BONUS_LOOK + MultiHex
# define ST_HANDS_ITEM_AND_MODE                           (102)
# define ST_FREE_BARTER_PLAYER                            (103)
# define ST_DIALOG_ID                                     (104)
# define ST_AI_ID                                         (105) // Mapper.cpp, ProtoMap.cpp ('version < 6' block)
# define ST_TEAM_ID                                       (106)
# define ST_BAG_ID                                        (107)
# define ST_LAST_WEAPON_ID                                (110)
# define ST_BASE_CRTYPE                                   (112) // Mapper.cpp
# define ST_TALK_DISTANCE                                 (115)
# define ST_SCALE_FACTOR                                  (116)
# define ST_WALK_TIME                                     (117)
# define ST_RUN_TIME                                      (118)
# define ST_MAX_TALKERS                                   (119)
# define ST_ANIM3D_LAYER_BEGIN                            (150)
# define ST_ANIM3D_LAYER_END                              (179)

// Skills
# define SK_UNARMED                                       (203)
# define SK_FIRST_AID                                     (206)
# define SK_DOCTOR                                        (207)
# define SK_SNEAK                                         (208)
# define SK_LOCKPICK                                      (209)
# define SK_STEAL                                         (210)
# define SK_TRAPS                                         (211)
# define SK_SCIENCE                                       (212)
# define SK_REPAIR                                        (213)
# define SK_SPEECH                                        (214) // TalkTime = MAX( RAW(SK_SPEECH) ) * 1000, DlgTalkMinTime )
# define SK_BARTER                                        (215)

// Tagged skills
# define TAG_SKILL1                                       (226)
# define TAG_SKILL2                                       (227)
# define TAG_SKILL3                                       (228)
# define TAG_SKILL4                                       (229)

// Timeout
# define TO_SK_REPAIR                                     (232)
# define TO_SK_SCIENCE                                    (233)
# define TO_BATTLE                                        (238)
# define TO_TRANSFER                                      (239)
# define TO_REMOVE_FROM_GAME                              (240)
# define TO_KARMA_VOTING                                  (242)

# define PE_SILENT_RUNNING                                (316)
# define PE_MASTER_TRADER                                 (318)
# define PE_QUICK_POCKETS                                 (349)

# define DAMAGE_POISONED                                  (500) // ClientInterface.cpp
# define DAMAGE_RADIATED                                  (501) // ClientInterface.cpp
# define DAMAGE_RIGHT_ARM                                 (503)
# define DAMAGE_LEFT_ARM                                  (504)
# define DAMAGE_RIGHT_LEG                                 (505)
# define DAMAGE_LEFT_LEG                                  (506)

// Modes
# define MODE_HIDE                                        (510)
# define MODE_NO_STEAL                                    (511)
# define MODE_NO_BARTER                                   (512)
# define MODE_NO_ENEMY_STACK                              (513)
# define MODE_NO_PVP                                      (514)
# define MODE_END_COMBAT                                  (515)
# define MODE_DEFAULT_COMBAT                              (516)
# define MODE_NO_HOME                                     (517)
# define MODE_GECK                                        (518)
# define MODE_NO_FAVORITE_ITEM                            (519)
# define MODE_NO_ITEM_GARBAGER                            (520)
# define MODE_DLG_SCRIPT_BARTER                           (521)
# define MODE_UNLIMITED_AMMO                              (522)
# define MODE_INVULNERABLE                                (527) // Critter::ToDead() does nothing
# define MODE_NO_FLATTEN                                  (528) // dead && !RAW( MODE_NO_FLATTEN ) ? DRAW_ORDER_DEAD_CRITTER : DRAW_ORDER_CRITTER
# define MODE_RANGE_HTH                                   (530)
# define MODE_NO_LOOT                                     (532)
# define MODE_NO_PUSH                                     (536)
# define MODE_NO_UNARMED                                  (537)
# define MODE_NO_AIM                                      (538)
# define MODE_NO_WALK                                     (539)
# define MODE_NO_RUN                                      (540)
# define MODE_NO_TALK                                     (541)

#endif   // !FOCLASSIC_SKIP_PARAMS

// Cleanup

#ifdef FOCLASSIC_ENGINE
# undef FOCLASSIC_EXTENSION
# undef FOCLASSIC_BLEEDING_EDGE
#endif   // FOCLASSIC_ENGINE

#endif   // __FOCLASSIC_FOS__ //
