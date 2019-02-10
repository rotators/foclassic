#ifndef __NET_PROTOCOL__
#define __NET_PROTOCOL__

#include "CMake.Net.h"
#include "Defines.h"

/************************************************************************/
/* Base                                                                 */
/************************************************************************/

// Fixed headers (1-9)
// NETMSG_LOGIN
// NETMSG_REGISTER
#define CONST_NETMSG_HEADER( number )         ( (uint)( (0xF0C1A51C + number - 1) ) )

// Dynamic headers
#define CALC_NETMSG_HEADER( number )          ( (uint)( (0xF0C1A51C ^ number) * (number ## ull << (number % 16) ) % 0xFFFFFFFF ) )
#define MAKE_NETMSG_HEADER( number )          CALC_NETMSG_HEADER( number )

#define PING_CLIENT_LIFE_TIME                 (15000)    // Time to ping client life

// Special message
// 0xFFFFFFFF
//   Client  DDDD
//           [uint D] data
//   Server  CCCCUUUU00000EVV
//           [uint C] connections [uint U] uptime [uint8 E] engine = 0xFC [uint16 V] version = FOCLASSIC_VERSION

#define NETRAW_HEADER                         (0xFFFFFFFF)
#define NETRAW_INVALID_VERSION                (0xFF000000)
#define NETRAW_DISCONNECT                     (0xFF000001)

// ************************************************************************
// LOGIN MESSAGES
// ************************************************************************

#define NETMSG_LOGIN                          CONST_NETMSG_HEADER( 1 )
#define NETMSG_LOGIN_SIZE                                                         \
    (sizeof(uint) + sizeof(uint16) + sizeof(uint16) + sizeof(uint) * 8 /*UIDs*/ + \
     UTF8_BUF_SIZE( MAX_NAME ) + PASS_HASH_SIZE + sizeof(uint) + sizeof(uint) * 10 /*MSG*/ + sizeof(uint) * 14 /*Proto*/ + sizeof(uint8) + 100)
// ////////////////////////////////////////////////////////////////////////
// Enter to game
// Params:
// uint16 engine_stage
// uint16 engine_version
// !uint uid4
// char name[MAX_NAME]
// !uint uid1
// char pass_hash[PASS_HASH_SIZE]
// uint msg_language
// uint hash_msg_game[TEXTMSG_MAX]
// !uint uidxor
// !uint uid3
// !uint uid2
// !uint uidor
// uint hash_proto[ITEM_TYPE_MAX]
// !uint uidcalc
// uint8 default_combat_mode
// !uint uid0
// char[100] - reserved
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_REGISTER                       CONST_NETMSG_HEADER( 2 )
// ////////////////////////////////////////////////////////////////////////
// Registration query
// Params:
// uint msg_len
// uint16 engine_stage
// uint16 engine_version
// MAX_NAME name
// char pass_hash[PASS_HASH_SIZE]
// uint16 params_count
//  uint16 param_index
//  int param_val
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_LOGIN_SUCCESS                  MAKE_NETMSG_HEADER( NETUID_LOGIN_SUCCESS )
#define NETMSG_LOGIN_SUCCESS_SIZE             (sizeof(uint) + sizeof(uint) * 2)
// ////////////////////////////////////////////////////////////////////////
// Login accepted
// uint bin_seed
// uint bout_seed
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_REGISTER_SUCCESS               MAKE_NETMSG_HEADER( NETUID_REGISTER_SUCCESS )
#define NETMSG_REGISTER_SUCCESS_SIZE          (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
// Answer about successes registration
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_PING                           MAKE_NETMSG_HEADER( NETUID_PING )
#define NETMSG_PING_SIZE                      (sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
// Ping
// uint8 ping (see Ping in FOdefines.h)
// ////////////////////////////////////////////////////////////////////////

/*#define NETMSG_UPDATE_FILES_LIST    MAKE_NETMSG_HEADER(?)
   //////////////////////////////////////////////////////////////////////////
   // Prepared message
   // uint msg_len
   // uint files_count
   //  uint16 path_len - max MAX_FOPATH
   //  char path[path_len]
   //  uint size
   //  uint hash
   //////////////////////////////////////////////////////////////////////////

   //#define NETMSG_SEND_GET_UPDATE_FILE MAKE_NETMSG_HEADER(?)
 #define NETMSG_SEND_GET_UPDATE_FILE_SIZE (sizeof(uint)+sizeof(uint))
   //////////////////////////////////////////////////////////////////////////
   // Request to updated file
   // uint file_number
   //////////////////////////////////////////////////////////////////////////

   //#define NETMSG_UPDATE_FILE          MAKE_NETMSG_HEADER(?)
   //////////////////////////////////////////////////////////////////////////
   // Portion of data
   // uint msg_len
   // uint16 portion_len
   // uint8 portion[portion_len]
   //////////////////////////////////////////////////////////////////////////

   //#define NETMSG_HELLO                MAKE_NETMSG_HEADER(?)
 #define NETMSG_HELLO_SIZE           (sizeof(uint))
   //////////////////////////////////////////////////////////////////////////
   // First message after connection
   //////////////////////////////////////////////////////////////////////////
 */

#define NETMSG_SINGLEPLAYER_SAVE_LOAD         MAKE_NETMSG_HEADER( NETUID_SINGLEPLAYER_SAVE_LOAD )
// ////////////////////////////////////////////////////////////////////////
// Singleplayer
// uint msg_len
// bool save
// uint16 fname_len
// char fname[fname_len]
// if save
//  uint save_pic_len
//  uint8 save_pic[save_pic_len]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CHECK_UID0                     MAKE_NETMSG_HEADER( NETUID_CHECK_UID0 )
#define NETMSG_CHECK_UID1                     MAKE_NETMSG_HEADER( NETUID_CHECK_UID1 )
#define NETMSG_CHECK_UID2                     MAKE_NETMSG_HEADER( NETUID_CHECK_UID2 )
#define NETMSG_CHECK_UID3                     MAKE_NETMSG_HEADER( NETUID_CHECK_UID3 )
#define NETMSG_CHECK_UID4                     MAKE_NETMSG_HEADER( NETUID_CHECK_UID4 )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// uint uid3
// uint xor for uid0
// uint8 rnd_count, 1..21
// uint uid1
// uint xor for uid2
// char rnd_data[rnd_count] - random numbers
// uint uid2
// uint xor for uid1
// uint uid4
// uint8 rnd_count2, 2..12
// uint xor for uid3
// uint xor for uid4
// uint uid0
// char rnd_data2[rnd_count] - random numbers
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CHECK_UID_EXT                  MAKE_NETMSG_HEADER( NETUID_CHECK_UID_EXT )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// ADD/REMOVE CRITTER
// ************************************************************************

#define NETMSG_ADD_PLAYER                     MAKE_NETMSG_HEADER( NETUID_ADD_PLAYER )
// ////////////////////////////////////////////////////////////////////////
// Add player on map.
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_ADD_NPC                        MAKE_NETMSG_HEADER( NETUID_ADD_NPC )
// ////////////////////////////////////////////////////////////////////////
// Add npc on map.
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_REMOVE_CRITTER                 MAKE_NETMSG_HEADER( NETUID_REMOVE_CRITTER )
#define NETMSG_REMOVE_CRITTER_SIZE            (sizeof(uint) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
// Remove critter from map.
// Params:
// uint crid
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// Commands, lexems
// ************************************************************************

#define NETMSG_SEND_COMMAND                   MAKE_NETMSG_HEADER( NETUID_SEND_COMMAND )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint8 cmd (see Commands in Access.h)
// Ext parameters
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_LEXEMS                 MAKE_NETMSG_HEADER( NETUID_CRITTER_LEXEMS )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint critter_id
// uint16 lexems_length
// char lexems[lexems_length]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_ITEM_LEXEMS                    MAKE_NETMSG_HEADER( NETUID_ITEM_LEXEMS )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint item_id
// uint16 lexems_length
// char lexems[lexems_length]
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// TEXT
// ************************************************************************

#define NETMSG_SEND_TEXT                      MAKE_NETMSG_HEADER( NETUID_SEND_TEXT )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint8 how_say (see Say types in FOdefines.h)
// uint16 len
// char[len] str
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_TEXT                   MAKE_NETMSG_HEADER( NETUID_CRITTER_TEXT )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// CrID crid
// uint8 how_say
// uint16 intellect
// bool unsafe_text
// uint16 len
// char[len] str
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_MSG                            MAKE_NETMSG_HEADER( NETUID_MSG )
#define NETMSG_MSG_SIZE            \
    (sizeof(uint) + sizeof(uint) + \
     sizeof(uint8) + sizeof(uint16) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// CrID crid
// uint8 how_say
// uint16 MSG_num
// uint num_str
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_MSG_LEX                        MAKE_NETMSG_HEADER( NETUID_MSG_LEX )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// CrID crid
// uint8 how_say
// uint16 MSG_num
// uint num_str
// uint16 lex_len
// char lexems[lex_len]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_MAP_TEXT                       MAKE_NETMSG_HEADER( NETUID_MAP_TEXT )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint16 hx
// uint16 hy
// uint color
// uint16 len
// char[len] str
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_MAP_TEXT_MSG                   MAKE_NETMSG_HEADER( NETUID_MAP_TEXT_MSG )
#define NETMSG_MAP_TEXT_MSG_SIZE         \
    (sizeof(uint) + sizeof(uint16) * 2 + \
     sizeof(uint) + sizeof(uint16) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint16 hx
// uint16 hy
// uint color
// uint16 MSG_num
// uint num_str
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_MAP_TEXT_MSG_LEX               MAKE_NETMSG_HEADER( NETUID_MAP_TEXT_MSG_LEX )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint16 hx
// uint16 hy
// uint color
// uint16 MSG_num
// uint num_str
// uint16 lexems_len
// char lexems[lexems_len]
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// DIR/MOVE
// ************************************************************************

#define NETMSG_DIR                            MAKE_NETMSG_HEADER( NETUID_DIR )
#define NETMSG_DIR_SIZE                       (sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint8 dir
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_DIR                    MAKE_NETMSG_HEADER( NETUID_CRITTER_DIR )
#define NETMSG_CRITTER_DIR_SIZE               (sizeof(uint) + sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// CrID id
// uint8 dir
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_MOVE_WALK                 MAKE_NETMSG_HEADER( NETUID_SEND_MOVE_WALK )
#define NETMSG_SEND_MOVE_WALK_SIZE            (sizeof(uint) + sizeof(uint) + sizeof(uint16) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint move_params
// uint16 hx
// uint16 hy
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_MOVE_RUN                  MAKE_NETMSG_HEADER( NETUID_SEND_MOVE_RUN )
#define NETMSG_SEND_MOVE_RUN_SIZE             (sizeof(uint) + sizeof(uint) + sizeof(uint16) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint move_params
// uint16 hx
// uint16 hy
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_MOVE                   MAKE_NETMSG_HEADER( NETUID_CRITTER_MOVE )
#define NETMSG_CRITTER_MOVE_SIZE              (sizeof(uint) + sizeof(uint) * 2 + sizeof(uint16) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint id
// uint move_params
// uint16 hx
// uint16 hy
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_XY                     MAKE_NETMSG_HEADER( NETUID_CRITTER_XY )
#define NETMSG_CRITTER_XY_SIZE     \
    (sizeof(uint) + sizeof(uint) + \
     sizeof(uint16) * 2 + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// CrID crid
// uint16 hex_x
// uint16 hex_y
// uint8 dir
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// CHOSEN Params
// ************************************************************************

#define NETMSG_ALL_PARAMS                     MAKE_NETMSG_HEADER( NETUID_ALL_PARAMS )
#define NETMSG_ALL_PARAMS_SIZE                (sizeof(uint) + MAX_PARAMS * sizeof(int) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// parameters[]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_PARAM                          MAKE_NETMSG_HEADER( NETUID_PARAM )
#define NETMSG_PARAM_SIZE                     (sizeof(uint) + sizeof(uint16) + sizeof(int) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint16 num_param
// int val
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_PARAM                  MAKE_NETMSG_HEADER( NETUID_CRITTER_PARAM )
#define NETMSG_CRITTER_PARAM_SIZE             (sizeof(uint) + sizeof(uint) + sizeof(uint16) + sizeof(int) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid
// uint16 num_param
// int val
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_LEVELUP                   MAKE_NETMSG_HEADER( NETUID_SEND_LEVELUP )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint16 count_skill_up
//	for count_skill_up
//	uint16 num_skill
//	uint16 val_up
// uint16 perk_up
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRAFT_ASK                      MAKE_NETMSG_HEADER( NETUID_CRAFT_ASK )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// uint16 count
// uint crafts_nums[count]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_CRAFT                     MAKE_NETMSG_HEADER( NETUID_SEND_CRAFT )
#define NETMSG_SEND_CRAFT_SIZE                (sizeof(uint) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// uint craft_num
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRAFT_RESULT                   MAKE_NETMSG_HEADER( NETUID_CRAFT_RESULT )
#define NETMSG_CRAFT_RESULT_SIZE              (sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
//
// uint8 craft_result (see Craft results in FOdefines.h)
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// CHOSEN ITEMS
// ************************************************************************

#define NETMSG_CLEAR_ITEMS                    MAKE_NETMSG_HEADER( NETUID_CLEAR_ITEMS )
#define NETMSG_CLEAR_ITEMS_SIZE               (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_ADD_ITEM                       MAKE_NETMSG_HEADER( NETUID_ADD_ITEM )
#define NETMSG_ADD_ITEM_SIZE       \
    (sizeof(uint) + sizeof(uint) + \
     sizeof(uint16) + sizeof(uint8) + 120 /*ItemData*/)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint id
// uint16 pid
// uint8 slot
// Item::ItemData Data
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_REMOVE_ITEM                    MAKE_NETMSG_HEADER( NETUID_REMOVE_ITEM )
#define NETMSG_REMOVE_ITEM_SIZE               (sizeof(uint) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint item_id
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_SORT_VALUE_ITEM           MAKE_NETMSG_HEADER( NETUID_SEND_SORT_VALUE_ITEM )
#define NETMSG_SEND_SORT_VALUE_ITEM_SIZE      (sizeof(uint) + sizeof(uint) + sizeof(uint16) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint item_id
// uint16 sort_val
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// ITEMS ON MAP
// ************************************************************************

#define NETMSG_ADD_ITEM_ON_MAP                MAKE_NETMSG_HEADER( NETUID_ADD_ITEM_ON_MAP )
#define NETMSG_ADD_ITEM_ON_MAP_SIZE \
    (sizeof(uint) + sizeof(uint) +  \
     sizeof(uint16) * 3 + sizeof(uint8) + 120 /*ItemData*/)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint item_id
// uint16 item_pid
// uint16 item_x
// uint16 item_y
// uint8 is_added
// Item::ItemData data
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CHANGE_ITEM_ON_MAP             MAKE_NETMSG_HEADER( NETUID_CHANGE_ITEM_ON_MAP )
#define NETMSG_CHANGE_ITEM_ON_MAP_SIZE        (sizeof(uint) + sizeof(uint) + 120 /*ItemData*/)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint item_id
// Item::ItemData data
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_RATE_ITEM                 MAKE_NETMSG_HEADER( NETUID_SEND_RATE_ITEM )
#define NETMSG_SEND_RATE_ITEM_SIZE            (sizeof(uint) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint rate of main item
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_ERASE_ITEM_FROM_MAP            MAKE_NETMSG_HEADER( NETUID_ERASE_ITEM_FROM_MAP )
#define NETMSG_ERASE_ITEM_FROM_MAP_SIZE       (sizeof(uint) + sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint item_id
// uint8 is_deleted
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_ANIMATE_ITEM                   MAKE_NETMSG_HEADER( NETUID_ANIMATE_ITEM )
#define NETMSG_ANIMATE_ITEM_SIZE              (sizeof(uint) + sizeof(uint) + sizeof(uint8) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint item_id
// uint8 from_frm
// uint8 to_frm
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// CHOSEN USE ITEM
// ************************************************************************

#define NETMSG_SEND_CHANGE_ITEM               MAKE_NETMSG_HEADER( NETUID_SEND_CHANGE_ITEM )
#define NETMSG_SEND_CHANGE_ITEM_SIZE                                   \
    (sizeof(uint) + sizeof(uint8) + sizeof(uint) + sizeof(uint8) * 2 + \
     sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint8 ap
// uint item_id
// uint8 from_slot
// uint8 to_slot
// uint count
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_PICK_ITEM                 MAKE_NETMSG_HEADER( NETUID_SEND_PICK_ITEM )
#define NETMSG_SEND_PICK_ITEM_SIZE            (sizeof(uint) + sizeof(uint16) * 3)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint16 targ_x
// uint16 targ_y
// uint16 pid
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CONTAINER_INFO                 MAKE_NETMSG_HEADER( NETUID_CONTAINER_INFO )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint msg_len
// uint8 transfer_type
// uint talk_time
// uint cont_id
// uint16 cont_pid or barter_k
// uint count_items
//	while(cont_items)
//	uint item_id
//	uint16 item_pid
//	uint item_count
//	Data
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_ITEM_CONT                 MAKE_NETMSG_HEADER( NETUID_SEND_ITEM_CONT )
#define NETMSG_SEND_ITEM_CONT_SIZE                     \
    (sizeof(uint) + sizeof(uint8) + sizeof(uint) * 2 + \
     sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint8 transfer_type (see Transfer types in FOdefines.h)
// uint cont_id
// uint item_id
// uint item_count
// uint8 take_flags (see Take flags in FOdefines.h)
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_USE_ITEM                  MAKE_NETMSG_HEADER( NETUID_SEND_USE_ITEM )
#define NETMSG_SEND_USE_ITEM_SIZE                  \
    (sizeof(uint) + sizeof(uint8) + sizeof(uint) + \
     sizeof(uint16) + sizeof(uint8) + sizeof(uint8) + sizeof(uint) + sizeof(uint16) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
// Use some item.
// Params:
// uint8 ap
// uint item_id
// uint16 item_pid
// uint8 rate
// uint8 target_type
// uint target_id
// uint16 target_pid
// uint param
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_USE_SKILL                 MAKE_NETMSG_HEADER( NETUID_SEND_USE_SKILL )
#define NETMSG_SEND_USE_SKILL_SIZE                   \
    (sizeof(uint) + sizeof(uint16) + sizeof(uint8) + \
     sizeof(uint) + sizeof(uint16) )
// ////////////////////////////////////////////////////////////////////////
// Use some skill.
// Params:
// uint16 skill
// uint8 targ_type
// uint target_id
// uint16 target_pid
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_PICK_CRITTER              MAKE_NETMSG_HEADER( NETUID_SEND_PICK_CRITTER )
#define NETMSG_SEND_PICK_CRITTER_SIZE         (sizeof(uint) + sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
// Critter picking.
// Params:
// uint crid
// uint8 pick_type (see Pick types in FOdefines.h)
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// CRITTER ACTION
// ************************************************************************

#define NETMSG_SOME_ITEM                      MAKE_NETMSG_HEADER( NETUID_SOME_ITEM )
#define NETMSG_SOME_ITEM_SIZE                       \
    (sizeof(uint) + sizeof(uint) + sizeof(uint16) + \
     sizeof(uint8) + 120 /*ItemData*/)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint item_id
// uint16 item_pid
// uint8 slot
// Item::ItemData data
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_ACTION                 MAKE_NETMSG_HEADER( NETUID_CRITTER_ACTION )
#define NETMSG_CRITTER_ACTION_SIZE \
    (sizeof(uint) + sizeof(uint) + \
     sizeof(int) * 2 + sizeof(bool) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid
// int action
// int action_ext
// bool is_item
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_KNOCKOUT               MAKE_NETMSG_HEADER( NETUID_CRITTER_KNOCKOUT )
#define NETMSG_CRITTER_KNOCKOUT_SIZE \
    (sizeof(uint) + sizeof(uint) +   \
     sizeof(uint) * 2 + sizeof(uint16) + sizeof(uint16) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid
// uint anim2begin
// uint anim2idle
// uint16 knock_hx
// uint16 knock_hy
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_MOVE_ITEM              MAKE_NETMSG_HEADER( NETUID_CRITTER_MOVE_ITEM )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_ITEM_DATA              MAKE_NETMSG_HEADER( NETUID_CRITTER_ITEM_DATA )
#define NETMSG_CRITTER_ITEM_DATA_SIZE \
    (sizeof(uint) + sizeof(uint) +    \
     sizeof(uint8) + 120 /*ItemData*/)
// ////////////////////////////////////////////////////////////////////////
// Item data changed
// uint crid
// uint8 slot
// Item::ItemData data
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_ANIMATE                MAKE_NETMSG_HEADER( NETUID_CRITTER_ANIMATE )
#define NETMSG_CRITTER_ANIMATE_SIZE    \
    (sizeof(uint) + sizeof(uint) * 3 + \
     sizeof(bool) + sizeof(bool) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid
// uint ind1
// uint ind2
// bool is_item
// bool clear_sequence
// bool delay_play
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_CRITTER_SET_ANIMS              MAKE_NETMSG_HEADER( NETUID_CRITTER_SET_ANIMS )
#define NETMSG_CRITTER_SET_ANIMS_SIZE         (sizeof(uint) + sizeof(int) + sizeof(uint) * 3)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid
// int cond
// uint ind1
// uint ind2
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_COMBAT_RESULTS                 MAKE_NETMSG_HEADER( NETUID_COMBAT_RESULTS )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_EFFECT                         MAKE_NETMSG_HEADER( NETUID_EFFECT )
#define NETMSG_EFFECT_SIZE                    (sizeof(uint) + sizeof(uint16) * 4)
// ////////////////////////////////////////////////////////////////////////
// explode
// Params:
// uint16 eff_pid
// uint16 hex_x
// uint16 hex_y
// uint16 radius
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_FLY_EFFECT                     MAKE_NETMSG_HEADER( NETUID_FLY_EFFECT )
#define NETMSG_FLY_EFFECT_SIZE       \
    (sizeof(uint) + sizeof(uint16) + \
     sizeof(uint) * 2 + sizeof(uint16) * 4)
// ////////////////////////////////////////////////////////////////////////
// shoot
// Params:
// uint16 eff_pid
// CrID eff_cr1_id
// CrID eff_cr2_id
// uint16 eff_cr1_hx
// uint16 eff_cr1_hy
// uint16 eff_cr2_hx
// uint16 eff_cr2_hy
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_PLAY_SOUND                     MAKE_NETMSG_HEADER( NETUID_PLAY_SOUND )
#define NETMSG_PLAY_SOUND_SIZE                (sizeof(uint) + sizeof(uint) + 100)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid_synchronize
// char sound_name[16]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_PLAY_SOUND_TYPE                MAKE_NETMSG_HEADER( NETUID_PLAY_SOUND_TYPE )
#define NETMSG_PLAY_SOUND_TYPE_SIZE           (sizeof(uint) + sizeof(uint) + sizeof(uint8) * 4)
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid_synchronize
// uint8 sound_type
// uint8 sound_type_ext
// uint8 sound_id
// uint8 sound_id_ext
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_KARMA_VOTING              MAKE_NETMSG_HEADER( NETUID_SEND_KARMA_VOTING )
#define NETMSG_SEND_KARMA_VOTING_SIZE         (sizeof(uint) + sizeof(uint) + sizeof(bool) )
// ////////////////////////////////////////////////////////////////////////
//
// Params:
// uint crid
// bool val_up
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// TALK&BARTER NPC
// ************************************************************************

#define NETMSG_SEND_TALK_NPC                  MAKE_NETMSG_HEADER( NETUID_SEND_TALK_NPC )
#define NETMSG_SEND_TALK_NPC_SIZE   \
    (sizeof(uint) + sizeof(uint8) + \
     sizeof(uint) + sizeof(uint8) )
// ////////////////////////////////////////////////////////////////////////
//
// uint8 is_npc
// uint id_talk
// uint8 answer - see Answer in FOdefines.h
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_SAY_NPC                   MAKE_NETMSG_HEADER( NETUID_SEND_SAY_NPC )
#define NETMSG_SEND_SAY_NPC_SIZE    \
    (sizeof(uint) + sizeof(uint8) + \
     sizeof(uint) + MAX_SAY_NPC_TEXT)
// ////////////////////////////////////////////////////////////////////////
// say about
// uint8 is_npc
// uint id_talk
// char str[MAX_NET_TEXT] - see Answer in FOdefines.h
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_TALK_NPC                       MAKE_NETMSG_HEADER( NETUID_TALK_NPC )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// uint8 is_npc
// uint crid
// uint8 all_answers
// uint main_text
// uint answ_text
// uint talk_time
// uint16 lexems_length
// char[lexems_length] lexems
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_BARTER                    MAKE_NETMSG_HEADER( NETUID_SEND_BARTER )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// CrID crid
//
// uint16 count_sale
// for count_sale
//	uint item_id
//	uint item_count
//
// uint16 count_buy
// for count_buy
//	uint item_id
//	uint item_count
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_PLAYERS_BARTER                 MAKE_NETMSG_HEADER( NETUID_PLAYERS_BARTER )
#define NETMSG_PLAYERS_BARTER_SIZE  \
    (sizeof(uint) + sizeof(uint8) + \
     sizeof(uint) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// uint8 barter
// uint param
// uint param_ext
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_PLAYERS_BARTER_SET_HIDE        MAKE_NETMSG_HEADER( NETUID_PLAYERS_BARTER_SET_HIDE )
#define NETMSG_PLAYERS_BARTER_SET_HIDE_SIZE \
    (sizeof(uint) + sizeof(uint) +          \
     sizeof(uint16) + sizeof(uint) + 120 /*ItemData*/)
// ////////////////////////////////////////////////////////////////////////
//
// uint id
// uint16 pid
// uint count
// Item::ItemData data
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// GAMETIME
// ************************************************************************

#define NETMSG_SEND_GET_INFO                  MAKE_NETMSG_HEADER( NETUID_SEND_GET_INFO )
#define NETMSG_SEND_GET_TIME_SIZE             (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_GAME_INFO                      MAKE_NETMSG_HEADER( NETUID_GAME_INFO )
#define NETMSG_GAME_INFO_SIZE            \
    (sizeof(uint) + sizeof(uint16) * 8 + \
     sizeof(int) + sizeof(uint8) + sizeof(bool) * 2 + sizeof(int) * 4 + sizeof(uint8) * 12)
// ////////////////////////////////////////////////////////////////////////
// Generic game info
// uint16 GameOpt.YearStart;
// uint16 GameOpt.Year;
// uint16 GameOpt.Month;
// uint16 GameOpt.Day;
// uint16 GameOpt.Hour;
// uint16 GameOpt.Minute;
// uint16 GameOpt.Second;
// uint16 GameOpt.TimeMultiplier;
// int time
// uint8 rain
// bool turn_based
// bool no_log_out
// int day_time[4]
// uint8 day_color[12]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_COMBAT                    MAKE_NETMSG_HEADER( NETUID_SEND_COMBAT )
#define NETMSG_SEND_COMBAT_SIZE               (sizeof(uint) + sizeof(uint8) + sizeof(int) )
// ////////////////////////////////////////////////////////////////////////
// Turn based
// uint8 type (see Combat in FOdefines.h)
// int val
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// MAP
// ************************************************************************

#define NETMSG_LOADMAP                        MAKE_NETMSG_HEADER( NETUID_LOADMAP )
#define NETMSG_LOADMAP_SIZE          \
    (sizeof(uint) + sizeof(uint16) + \
     sizeof(int) + sizeof(uint8) + sizeof(uint) * 3)
// ////////////////////////////////////////////////////////////////////////
//
// uint16 num_pid
// int map_time
// uint8 map_rain
// uint ver_tiles
// uint ver_walls
// uint ver_scen
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_MAP                            MAKE_NETMSG_HEADER( NETUID_MAP )
// ////////////////////////////////////////////////////////////////////////
// Map data
// uint msg_len
// uint16 pid_map
// uint8 send_info (see Sendmap info in FOdefines.h)
// uint ver_tiles
//	for TILESX*TILESY
//		uint16 roof
//		uint16 floor
// uint count_walls
//	for count_walls
//		WallToSend
// uint count_scen
//	for count_scen
//		ScenToSend (see ScenToSend in FOdefines.h)
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_GIVE_MAP                  MAKE_NETMSG_HEADER( NETUID_SEND_GIVE_MAP )
#define NETMSG_SEND_GIVE_MAP_SIZE                   \
    (sizeof(uint) + sizeof(bool) + sizeof(uint16) + \
     sizeof(uint) + sizeof(uint) * 3)
// ////////////////////////////////////////////////////////////////////////
// Request on map data, on map loading or for automap
// bool automap
// uint16 map_pid
// uint loc_id
// uint ver_tiles
// uint ver_walls
// uint ver_scen
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_LOAD_MAP_OK               MAKE_NETMSG_HEADER( NETUID_SEND_LOAD_MAP_OK )
#define NETMSG_SEND_LOAD_MAP_OK_SIZE          (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
// Message about successfully map loading
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SHOW_SCREEN                    MAKE_NETMSG_HEADER( NETUID_SHOW_SCREEN )
#define NETMSG_SHOW_SCREEN_SIZE               (sizeof(uint) + sizeof(int) + sizeof(uint) + sizeof(bool) )
// ////////////////////////////////////////////////////////////////////////
//
// int screen_type
// uint param
// bool need_answer
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_RUN_CLIENT_SCRIPT              MAKE_NETMSG_HEADER( NETUID_RUN_CLIENT_SCRIPT )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_RUN_SERVER_SCRIPT         MAKE_NETMSG_HEADER( NETUID_SEND_RUN_SERVER_SCRIPT )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_SCREEN_ANSWER             MAKE_NETMSG_HEADER( NETUID_SEND_SCREEN_ANSWER )
#define NETMSG_SEND_SCREEN_ANSWER_SIZE        (sizeof(uint) + sizeof(uint) + MAX_SAY_NPC_TEXT)
// ////////////////////////////////////////////////////////////////////////
//
// uint answer_i
// char answer_s[MAX_SAY_NPC_TEXT]
// ////////////////////////////////////////////////////////////////////////


#define NETMSG_DROP_TIMERS                    MAKE_NETMSG_HEADER( NETUID_DROP_TIMERS )
#define NETMSG_DROP_TIMERS_SIZE               (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_REFRESH_ME                MAKE_NETMSG_HEADER( NETUID_SEND_REFRESH_ME )
#define NETMSG_SEND_REFRESH_ME_SIZE           (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_VIEW_MAP                       MAKE_NETMSG_HEADER( NETUID_VIEW_MAP )
#define NETMSG_VIEW_MAP_SIZE                  (sizeof(uint) + sizeof(uint16) * 2 + sizeof(uint) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// uint16 hx, hy
// uint loc_id, loc_ent
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// GLOBAL
// ************************************************************************

#define NETMSG_SEND_GIVE_GLOBAL_INFO          MAKE_NETMSG_HEADER( NETUID_SEND_GIVE_GLOBAL_INFO )
#define NETMSG_SEND_GIVE_GLOBAL_INFO_SIZE     (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// uint8 info_flag; (see GM Info in FOdefines.h)
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_GLOBAL_INFO                    MAKE_NETMSG_HEADER( NETUID_GLOBAL_INFO )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// uint8 info_flag; (see GM info in FOdefines.h)
// Data
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_RULE_GLOBAL               MAKE_NETMSG_HEADER( NETUID_SEND_RULE_GLOBAL )
#define NETMSG_SEND_RULE_GLOBAL_SIZE \
    (sizeof(uint) + sizeof(uint8) +  \
     sizeof(uint) * 2)
// ////////////////////////////////////////////////////////////////////////
//
// uint8 command; (see GM Rule command in FOdefines.h)
// uint param1;
// uint param2;
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_FOLLOW                         MAKE_NETMSG_HEADER( NETUID_FOLLOW )
#define NETMSG_FOLLOW_SIZE         \
    (sizeof(uint) + sizeof(uint) + \
     sizeof(uint8) + sizeof(uint16) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
// uint follow_rule
// uint8 follow_type (see Follow in FOdefines.h)
// uint16 map_pid
// uint waiting_time
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_GLOBAL_ENTRANCES               MAKE_NETMSG_HEADER( NETUID_GLOBAL_ENTRANCES )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// uint loc_id
// uint8 count
//  uint8 entrances[count]
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// Cached data
// ************************************************************************

#define NETMSG_MSG_DATA                       MAKE_NETMSG_HEADER( NETUID_MSG_DATA )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// uint MSG_language
// uint16 MSG_num
// uint MSG_hash
// char[msg_len-(sizeof(uint)-sizeof(msg_len)-sizeof(MSG_language)-
//	sizeof(MSG_num)-sizeof(MSG_hash))] MSG_data
//	uint num
//	uint len
//	char data[len]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_ITEM_PROTOS                    MAKE_NETMSG_HEADER( NETUID_ITEM_PROTOS )
// ////////////////////////////////////////////////////////////////////////
//
// uint msg_len
// uint8 item_type
// uint item_hash
// ProtoItem[(msg_len-sizeof(uint)-sizeof(msg_len)-sizeof(item_type)-
//	sizeof(msg_hash))/sizeof(ProtoItem)]
// ////////////////////////////////////////////////////////////////////////

// ************************************************************************
// Quest, Holodisk info, Automaps info
// ************************************************************************

#define NETMSG_QUEST                          MAKE_NETMSG_HEADER( NETUID_QUEST )
#define NETMSG_QUEST_SIZE                     (sizeof(uint) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
// uint str_num
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_QUESTS                         MAKE_NETMSG_HEADER( NETUID_QUESTS )
// ////////////////////////////////////////////////////////////////////////
// uint msg_len
// uint num_count
//	for num_count
//	uint str_num
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_HOLO_INFO                      MAKE_NETMSG_HEADER( NETUID_HOLO_INFO )
// ////////////////////////////////////////////////////////////////////////
// uint msg_len
// bool clear
// uint16 offset
// uint16 num_count
//	for num_count
//	uint16 holo_num
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_SET_USER_HOLO_STR         MAKE_NETMSG_HEADER( NETUID_SEND_SET_USER_HOLO_STR )
// ////////////////////////////////////////////////////////////////////////
// uint msg_len
// uint holodisk_id
// uint16 title_len
// uint16 text_len
// char title[title_len]
// char text[text_len]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_GET_USER_HOLO_STR         MAKE_NETMSG_HEADER( NETUID_SEND_GET_USER_HOLO_STR )
#define NETMSG_SEND_GET_USER_HOLO_STR_SIZE    (sizeof(uint) + sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
// uint str_num
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_USER_HOLO_STR                  MAKE_NETMSG_HEADER( NETUID_USER_HOLO_STR )
// ////////////////////////////////////////////////////////////////////////
// uint msg_len
// uint str_num
// uint16 text_len
// char text[text_len]
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SEND_GET_SCORES                MAKE_NETMSG_HEADER( NETUID_SEND_GET_SCORES )
#define NETMSG_SEND_GET_SCORES_SIZE           (sizeof(uint) )
// ////////////////////////////////////////////////////////////////////////
//
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_SCORES                         MAKE_NETMSG_HEADER( NETUID_SCORES )
#define NETMSG_SCORES_SIZE                    (sizeof(uint) + SCORE_NAME_LEN * SCORES_MAX)
// ////////////////////////////////////////////////////////////////////////
// for SCORES_MAX
//	char[MAX_NAME] client_name - without null-terminated character
// ////////////////////////////////////////////////////////////////////////

#define NETMSG_AUTOMAPS_INFO                  MAKE_NETMSG_HEADER( NETUID_AUTOMAPS_INFO )
// ////////////////////////////////////////////////////////////////////////
// Automaps information
// uint msg_len
// bool clear_list
// uint16 locations_count
//  for locations_count
//  uint location_id
//  uint16 location_pid
//  uint16 maps_count
//   for maps_count
//   uint16 map_pid
// ////////////////////////////////////////////////////////////////////////

#endif // __NET_PROTOCOL__ //
