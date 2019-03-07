#include "Core.h"

#include "Access.h"
#include "BufferManager.h"
#include "Crypt.h"
#include "GameOptions.h"
#include "NetProtocol.h"
#include "Text.h"
#include "Types.h"

using namespace std;

struct CmdDef
{
    char  cmd[20];
    uint8 id;
};

const CmdDef cmdlist[] =
{
    { "exit", COMMAND_EXIT },
    { "myinfo", COMMAND_MYINFO },
    { "gameinfo", COMMAND_GAMEINFO },
    { "id", COMMAND_CRITID },
    { "move", COMMAND_MOVECRIT },
    { "kill", COMMAND_KILLCRIT },
    { "disconnect", COMMAND_DISCONCRIT },
    { "toglobal", COMMAND_TOGLOBAL },
    { "respawn", COMMAND_RESPAWN },
    { "param", COMMAND_PARAM },
    { "getaccess", COMMAND_GETACCESS },
    { "additem", COMMAND_ADDITEM },
    { "additemself", COMMAND_ADDITEM_SELF },
    { "ais", COMMAND_ADDITEM_SELF },
    { "addnpc", COMMAND_ADDNPC },
    { "addloc", COMMAND_ADDLOCATION },
    { "reloadscripts", COMMAND_RELOADSCRIPTS },
    { "loadscript", COMMAND_LOADSCRIPT },
    { "load", COMMAND_LOADSCRIPT },
    { "reloadclientscripts", COMMAND_RELOAD_CLIENT_SCRIPTS },
    { "rcs", COMMAND_RELOAD_CLIENT_SCRIPTS },
    { "runscript", COMMAND_RUNSCRIPT },
    { "run", COMMAND_RUNSCRIPT },
    { "reloadlocations", COMMAND_RELOADLOCATIONS },
    { "loadlocation", COMMAND_LOADLOCATION },
    { "reloadmaps", COMMAND_RELOADMAPS },
    { "loadmap", COMMAND_LOADMAP },
    { "regenmap", COMMAND_REGENMAP },
    { "reloaddialogs", COMMAND_RELOADDIALOGS },
    { "loaddialog", COMMAND_LOADDIALOG },
    { "reloadtexts", COMMAND_RELOADTEXTS },
    { "reloadai", COMMAND_RELOADAI },
    { "checkvar", COMMAND_CHECKVAR },
    { "cvar", COMMAND_CHECKVAR },
    { "setvar", COMMAND_SETVAR },
    { "svar", COMMAND_SETVAR },
    { "settime", COMMAND_SETTIME },
    { "ban", COMMAND_BAN },
    { "deleteself", COMMAND_DELETE_ACCOUNT },
    { "changepassword", COMMAND_CHANGE_PASSWORD },
    { "changepass", COMMAND_CHANGE_PASSWORD },
    { "dropuid", COMMAND_DROP_UID },
    { "drop", COMMAND_DROP_UID },
    { "log", COMMAND_LOG },
};

void PackCommand( const char* str, BufferManager& buf, void (*logcb)( const char* ), const char* name )
{
    char args[MAX_FOTEXT];
    Str::Copy( args, str );
    Str::EraseFrontBackSpecificChars( args );

    char  cmd_str[MAX_FOTEXT];
    Str::Copy( cmd_str, args );
    char* space = Str::Substring( cmd_str, " " );
    if( space )
    {
        *space = 0;
        Str::EraseInterval( args, Str::Length( cmd_str ) );
    }

    uint8 cmd = 0;
    for( uint cur_cmd = 0; cur_cmd < sizeof(cmdlist) / sizeof(CmdDef); cur_cmd++ )
        if( Str::CompareCase( cmd_str, cmdlist[cur_cmd].cmd ) )
            cmd = cmdlist[cur_cmd].id;
    if( !cmd )
        return;

    uint msg = NETMSG_SEND_COMMAND;
    uint msg_len = sizeof(msg) + sizeof(msg_len) + sizeof(cmd);

    switch( cmd )
    {
        case COMMAND_EXIT:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_MYINFO:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_GAMEINFO:
        {
            int type;
            if( sscanf( args, "%d", &type ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~gameinfo type>." );
                break;
            }
            msg_len += sizeof(type);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << type;
            break;
        }
        case COMMAND_CRITID:
        {
            char name[MAX_FOTEXT];
            if( sscanf( args, "%s", name ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~id name>." );
                break;
            }
            uint name_size_utf8 = UTF8_BUF_SIZE( MAX_NAME );
            name[name_size_utf8 - 1] = 0;
            msg_len += name_size_utf8;

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( name, name_size_utf8 );
            break;
        }
        case COMMAND_MOVECRIT:
        {
            uint   crid;
            uint16 hex_x;
            uint16 hex_y;
            if( sscanf( args, "%u%hu%hu", &crid, &hex_x, &hex_y ) != 3 )
            {
                logcb( "Invalid arguments. Example: <~move crid hx hy>." );
                break;
            }
            msg_len += sizeof(crid) + sizeof(hex_x) + sizeof(hex_y);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << crid;
            buf << hex_x;
            buf << hex_y;
            break;
        }
        case COMMAND_KILLCRIT:
        {
            uint crid;
            if( sscanf( args, "%u", &crid ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~kill crid>." );
                break;
            }
            msg_len += sizeof(crid);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << crid;
            break;
        }
        case COMMAND_DISCONCRIT:
        {
            uint crid;
            if( sscanf( args, "%u", &crid ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~disconnect crid>." );
                break;
            }
            msg_len += sizeof(crid);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << crid;
            break;
        }
        case COMMAND_TOGLOBAL:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_RESPAWN:
        {
            uint crid;
            if( sscanf( args, "%u", &crid ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~respawn crid>." );
                break;
            }
            msg_len += sizeof(crid);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << crid;
            break;
        }
        case COMMAND_PARAM:
        {
            uint   crid;
            uint16 param_num;
            int    param_val;
            if( sscanf( args, "%u%hu%d", &crid, &param_num, &param_val ) != 3 )
            {
                logcb( "Invalid arguments. Example: <~param crid index value>." );
                break;
            }
            msg_len += sizeof(uint) + sizeof(uint16) + sizeof(int);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << crid;
            buf << param_num;
            buf << param_val;
            break;
        }
        case COMMAND_GETACCESS:
        {
            char name_access[MAX_FOTEXT];
            char pasw_access[MAX_FOTEXT];
            if( sscanf( args, "%s%s", name_access, pasw_access ) != 2 )
                break;
            Str::Replacement( name_access, '*', ' ' );
            Str::Replacement( pasw_access, '*', ' ' );

            uint name_size_utf8 = UTF8_BUF_SIZE( MAX_NAME );
            uint pasw_size_utf8 = UTF8_BUF_SIZE( 128 );
            msg_len += name_size_utf8 + pasw_size_utf8;
            name_access[name_size_utf8 - 1] = 0;
            pasw_access[pasw_size_utf8 - 1] = 0;

            buf << msg;
            buf << msg_len;
            buf << cmd;

            buf.Push( name_access, name_size_utf8 );
            buf.Push( pasw_access, pasw_size_utf8 );
            break;
        }
        case COMMAND_ADDITEM:
        {
            uint16 hex_x;
            uint16 hex_y;
            uint16 pid;
            uint   count;
            if( sscanf( args, "%hu%hu%hu%u", &hex_x, &hex_y, &pid, &count ) != 4 )
            {
                logcb( "Invalid arguments. Example: <~additem hx hy pid count>." );
                break;
            }
            msg_len += sizeof(hex_x) + sizeof(hex_y) + sizeof(pid) + sizeof(count);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << hex_x;
            buf << hex_y;
            buf << pid;
            buf << count;
            break;
        }
        case COMMAND_ADDITEM_SELF:
        {
            uint16 pid;
            uint   count;
            if( sscanf( args, "%hu%u", &pid, &count ) != 2 )
            {
                logcb( "Invalid arguments. Example: <~additemself pid count>." );
                break;
            }
            msg_len += sizeof(pid) + sizeof(count);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << pid;
            buf << count;
            break;
        }
        case COMMAND_ADDNPC:
        {
            uint16 hex_x;
            uint16 hex_y;
            uint8  dir;
            uint16 pid;
            if( sscanf( args, "%hu%hu%hhu%hu", &hex_x, &hex_y, &dir, &pid ) != 4 )
            {
                logcb( "Invalid arguments. Example: <~addnpc hx hy dir pid>." );
                break;
            }
            msg_len += sizeof(hex_x) + sizeof(hex_y) + sizeof(dir) + sizeof(pid);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << hex_x;
            buf << hex_y;
            buf << dir;
            buf << pid;
            break;
        }
        case COMMAND_ADDLOCATION:
        {
            uint16 wx;
            uint16 wy;
            uint16 pid;
            if( sscanf( args, "%hu%hu%hu", &wx, &wy, &pid ) != 3 )
            {
                logcb( "Invalid arguments. Example: <~addloc wx wy pid>." );
                break;
            }
            msg_len += sizeof(wx) + sizeof(wy) + sizeof(pid);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << wx;
            buf << wy;
            buf << pid;
            break;
        }
        case COMMAND_RELOADSCRIPTS:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_LOADSCRIPT:
        {
            char script_name[MAX_SCRIPT_NAME + 1];
            if( sscanf( args, "%s", script_name ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~loadscript name>." );
                break;
            }
            script_name[MAX_SCRIPT_NAME] = 0;
            msg_len += MAX_SCRIPT_NAME;

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( script_name, MAX_SCRIPT_NAME );
            break;
        }
        case COMMAND_RELOAD_CLIENT_SCRIPTS:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_RUNSCRIPT:
        {
            char script_name[MAX_SCRIPT_NAME + 1];
            char func_name[MAX_SCRIPT_NAME + 1];
            uint param0, param1, param2;
            if( sscanf( args, "%s%s%u%u%u", script_name, func_name, &param0, &param1, &param2 ) != 5 )
            {
                logcb( "Invalid arguments. Example: <~runscript module func param0 param1 param2>." );
                break;
            }
            script_name[MAX_SCRIPT_NAME] = 0;
            func_name[MAX_SCRIPT_NAME] = 0;
            msg_len += MAX_SCRIPT_NAME * 2 + sizeof(uint) * 3;

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( script_name, MAX_SCRIPT_NAME );
            buf.Push( func_name, MAX_SCRIPT_NAME );
            buf << param0;
            buf << param1;
            buf << param2;
            break;
        }
        case COMMAND_RELOADLOCATIONS:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_LOADLOCATION:
        {
            uint16 loc_pid;
            if( sscanf( args, "%hu", &loc_pid ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~loadlocation pid>." );
                break;
            }
            msg_len += sizeof(loc_pid);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << loc_pid;
            break;
        }
        case COMMAND_RELOADMAPS:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_LOADMAP:
        {
            uint16 map_pid;
            if( sscanf( args, "%hu", &map_pid ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~loadmap pid>." );
                break;
            }
            msg_len += sizeof(map_pid);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << map_pid;
            break;
        }
        case COMMAND_REGENMAP:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_RELOADDIALOGS:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_LOADDIALOG:
        {
            char dlg_name[128];
            uint dlg_id;
            if( sscanf( args, "%s%u", dlg_name, &dlg_id ) != 2 )
            {
                logcb( "Invalid arguments. Example: <~loaddialog name id>." );
                break;
            }
            dlg_name[127] = 0;
            msg_len += 128 + sizeof(dlg_id);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( dlg_name, 128 );
            buf << dlg_id;
            break;
        }
        case COMMAND_RELOADTEXTS:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_RELOADAI:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_CHECKVAR:
        {
            uint16 tid_var;
            uint8  master_is_npc;
            uint   master_id;
            uint   slave_id;
            uint8  full_info;
            if( sscanf( args, "%hu%hhu%u%u%hhu", &tid_var, &master_is_npc, &master_id, &slave_id, &full_info ) != 5 )
            {
                logcb( "Invalid arguments. Example: <~checkvar tid_var master_is_npc master_id slave_id full_info>." );
                break;
            }
            msg_len += sizeof(tid_var) + sizeof(master_is_npc) + sizeof(master_id) + sizeof(slave_id) + sizeof(full_info);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << tid_var;
            buf << master_is_npc;
            buf << master_id;
            buf << slave_id;
            buf << full_info;
            break;
        }
        case COMMAND_SETVAR:
        {
            uint16 tid_var;
            uint8  master_is_npc;
            uint   master_id;
            uint   slave_id;
            int    value;
            if( sscanf( args, "%hu%hhu%u%u%d", &tid_var, &master_is_npc, &master_id, &slave_id, &value ) != 5 )
            {
                logcb( "Invalid arguments. Example: <~setvar tid_var master_is_npc master_id slave_id value>." );
                break;
            }
            msg_len += sizeof(tid_var) + sizeof(master_is_npc) + sizeof(master_id) + sizeof(slave_id) + sizeof(value);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << tid_var;
            buf << master_is_npc;
            buf << master_id;
            buf << slave_id;
            buf << value;
            break;
        }
        case COMMAND_SETTIME:
        {
            int multiplier;
            int year;
            int month;
            int day;
            int hour;
            int minute;
            int second;
            if( sscanf( args, "%d%d%d%d%d%d%d", &multiplier, &year, &month, &day, &hour, &minute, &second ) != 7 )
            {
                logcb( "Invalid arguments. Example: <~settime tmul year month day hour minute second>." );
                break;
            }
            msg_len += sizeof(multiplier) + sizeof(year) + sizeof(month) + sizeof(day) + sizeof(hour) + sizeof(minute) + sizeof(second);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf << multiplier;
            buf << year;
            buf << month;
            buf << day;
            buf << hour;
            buf << minute;
            buf << second;
            break;
        }
        case COMMAND_BAN:
        {
            istrstream str_( args );
            char       params[128] = { 0 };
            char       name[128] = { 0 };
            uint       ban_hours = 0;
            char       info[128] = { 0 };
            str_ >> params;
            if( !str_.fail() )
                str_ >> name;
            if( !str_.fail() )
                str_ >> ban_hours;
            if( !str_.fail() )
                Str::Copy( info, str_.str() );
            if( !Str::CompareCase( params, "add" ) && !Str::CompareCase( params, "add+" ) && !Str::CompareCase( params, "delete" ) && !Str::CompareCase( params, "list" ) )
            {
                logcb( "Invalid arguments. Example: <~ban [add,add+,delete,list] [user] [hours] [comment]>." );
                break;
            }
            Str::Replacement( name, '*', ' ' );
            Str::Replacement( info, '$', '*' );
            while( info[0] == ' ' )
                Str::CopyBack( info );

            uint name_size_utf8 = UTF8_BUF_SIZE( MAX_NAME );
            uint params_size_utf8 = UTF8_BUF_SIZE( MAX_NAME );
            uint info_size_utf8 = UTF8_BUF_SIZE( MAX_CHAT_MESSAGE );
            name[name_size_utf8 - 1] = 0;
            params[params_size_utf8 - 1] = 0;
            info[info_size_utf8 - 1] = 0; // FIXME Array 'info[128]' accessed at index 399, which is out of bounds.

            msg_len += name_size_utf8 + params_size_utf8 + info_size_utf8 + sizeof(ban_hours);

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( name, name_size_utf8 );
            buf.Push( params, params_size_utf8 );
            buf << ban_hours;
            buf.Push( info, info_size_utf8 );
            break;
        }
        case COMMAND_DELETE_ACCOUNT:
        {
            if( !name )
            {
                logcb( "Can't execute this command." );
                break;
            }

            char pass[128];
            if( sscanf( args, "%s", pass ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~deleteself user_password>." );
                break;
            }
            Str::Replacement( pass, '*', ' ' );
            char pass_hash[PASS_HASH_SIZE];
            Crypt.ClientPassHash( name, pass, pass_hash );
            msg_len += PASS_HASH_SIZE;

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( pass_hash, PASS_HASH_SIZE );
            break;
        }
        case COMMAND_CHANGE_PASSWORD:
        {
            if( !name )
            {
                logcb( "Can't execute this command." );
                break;
            }

            char pass[MAX_FOTEXT];
            char new_pass[MAX_FOTEXT];
            if( sscanf( args, "%s%s", pass, new_pass ) != 2 )
            {
                logcb( "Invalid arguments. Example: <~changepassword current_password new_password>." );
                break;
            }
            Str::Replacement( pass, '*', ' ' );

            // Check the new password's validity
            uint pass_len = Str::LengthUTF8( new_pass );
            if( pass_len < MIN_NAME || pass_len < GameOpt.MinNameLength || pass_len > MAX_NAME || pass_len > GameOpt.MaxNameLength )
            {
                logcb( "Invalid new password." );
                break;
            }

            char pass_hash[PASS_HASH_SIZE];
            Crypt.ClientPassHash( name, pass, pass_hash );
            Str::Replacement( new_pass, '*', ' ' );
            char new_pass_hash[PASS_HASH_SIZE];
            Crypt.ClientPassHash( name, new_pass, new_pass_hash );
            msg_len += PASS_HASH_SIZE * 2;

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( pass_hash, PASS_HASH_SIZE );
            buf.Push( new_pass_hash, PASS_HASH_SIZE );
            break;
        }
        case COMMAND_DROP_UID:
        {
            buf << msg;
            buf << msg_len;
            buf << cmd;
            break;
        }
        case COMMAND_LOG:
        {
            char flags[128];
            if( sscanf( args, "%s", flags ) != 1 )
            {
                logcb( "Invalid arguments. Example: <~log flag>. Valid flags: '+' attach, '-' detach, '--' detach all." );
                break;
            }
            msg_len += 16;

            buf << msg;
            buf << msg_len;
            buf << cmd;
            buf.Push( flags, 16 );
            break;
        }
        default:
            break;
    }
}
