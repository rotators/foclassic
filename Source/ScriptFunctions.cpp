#include "ScriptFunctions.h"

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_SERVER)

ClientScriptFunctions  ClientFunctions;
ReservedScriptFunction ClientReservedFunctions[] =
{
    { &ClientFunctions.Start, "start", "bool %s()" },
    { &ClientFunctions.Loop, "loop", "uint %s()" },
    { &ClientFunctions.GetActiveScreens, "get_active_screens", "void %s( int[]& screens )" },
    { &ClientFunctions.ScreenChange, "screen_change", "void %s( bool show, int screen, int p0, int p1, int p2 )" },
    { &ClientFunctions.RenderIface, "render_iface", "void %s( uint layer )" },
    { &ClientFunctions.RenderMap, "render_map", "void %s()" },
    { &ClientFunctions.MouseDown, "mouse_down", "bool %s( int click )" },
    { &ClientFunctions.MouseUp, "mouse_up", "bool %s( int click )" },
    { &ClientFunctions.MouseMove, "mouse_move", "void %s( int x, int y )" },
    { &ClientFunctions.KeyDown, "key_down", "bool %s( uint8 key, string& keyText )" },
    { &ClientFunctions.KeyUp, "key_up", "bool %s( uint8 key, string& keyText )" },
    { &ClientFunctions.InputLost, "input_lost", "void %s()" },
    { &ClientFunctions.CritterIn, "critter_in", "void %s( CritterCl& cr )" },
    { &ClientFunctions.CritterOut, "critter_out", "void %s( CritterCl& cr )" },
    { &ClientFunctions.ItemMapIn, "item_map_in", "void %s( ItemCl& item )" },
    { &ClientFunctions.ItemMapChanged, "item_map_changed", "void %s( ItemCl& itemNew, ItemCl& itemOld )" },
    { &ClientFunctions.ItemMapOut, "item_map_out", "void %s( ItemCl& item )" },
    { &ClientFunctions.ItemInvIn, "item_inv_in", "void %s( ItemCl& item )" },
    { &ClientFunctions.ItemInvOut, "item_inv_out", "void %s( ItemCl& item )" },
    { &ClientFunctions.MapMessage, "map_message", "bool %s( string& message, uint16& hexX, uint16& hexY, uint& color, uint& delay )" },
    { &ClientFunctions.InMessage, "in_message", "bool %s( string& message, int& sayType, uint& critterId, uint& delay )" },
    { &ClientFunctions.OutMessage, "out_message", "bool %s( string& message,int& sayType)" },
    { &ClientFunctions.ToHit, "to_hit", "int %s( CritterCl& chosen, CritterCl& crTarget, ProtoItem& weapon, uint8 weaponMode )" },
    { &ClientFunctions.HitAim, "hit_aim", "void %s( uint8& aim )" },
    { &ClientFunctions.CombatResult, "combat_result", "void %s( uint[]& data )" },
    { &ClientFunctions.GenericDesc, "generic_description", "string %s( int descriptionType, int& offsetX, int& offsetY )" },
    { &ClientFunctions.ItemLook, "item_description", "string %s( ItemCl& item, int lookType )" },
    { &ClientFunctions.CritterLook, "critter_description", "string %s( CritterCl& cr, int lookType )" },
    { &ClientFunctions.GetElevator, "get_elevator", "bool %s( uint type, uint[]& data )" },
    { &ClientFunctions.ItemCost, "item_cost", "uint %s( ItemCl& item, CritterCl& chosen, CritterCl& npc, bool sell )" },
    { &ClientFunctions.PerkCheck, "check_perk", "bool %s( CritterCl& chosen, uint perk )" },
    { &ClientFunctions.PlayerGeneration, "player_data_generate", "void %s( int[]& data )" },
    { &ClientFunctions.PlayerGenerationCheck, "player_data_check", "bool %s( string& name, int[]& data )" },
    { &ClientFunctions.CritterAction, "critter_action", "void %s( bool localCall, CritterCl& cr, int action, int actionExt, ItemCl@ item )" },
    { &ClientFunctions.Animation2dProcess, "animation2d_process", "void %s( bool animateStay, CritterCl& cr, uint anim1, uint anim2, ItemCl@ item )" },
    { &ClientFunctions.Animation3dProcess, "animation3d_process", "void %s( bool animateStay, CritterCl& cr, uint anim1, uint anim2, ItemCl@ item )" },
    { &ClientFunctions.ItemsCollection, "items_collection", "void %s( int collection, ItemCl@[]& items )" },
    { &ClientFunctions.CritterAnimation, "critter_animation", "string@ %s( int animType, uint crType, uint anim1, uint anim2, uint& pass, uint& flags, int& ox, int& oy )" },
    { &ClientFunctions.CritterAnimationSubstitute, "critter_animation_substitute", "bool %s( int animType, uint crTypeBase, uint anim1base, uint anim2base, uint& crType, uint& anim1, uint& anim2 )" },
    { &ClientFunctions.CritterAnimationFallout, "critter_animation_fallout", "bool %s( uint crType, uint& anim1, uint& anim2, uint& anim1ex, uint& anim2ex, uint& flags )" },
    { &ClientFunctions.FilenameLogfile, "filename_logfile", "void %s( string& filename )" },
    { &ClientFunctions.FilenameScreenshot, "filename_screenshot", "void %s( string& filename )" },
    { &ClientFunctions.CritterCheckMoveItem, "critter_check_move_item", "bool %s( CritterCl& cr, ItemCl& item, uint8 toSlot, ItemCl@ itemSwap )" }
};

#endif // FOCLASSIC_CLIENT || FOCLASSIC_SERVER

#if defined (FOCLASSIC_MAPPER) || defined (FOCLASSIC_SERVER)

MapperScriptFunctions  MapperFunctions;
ReservedScriptFunction MapperReservedFunctions[] =
{
    { &MapperFunctions.Start, "start", "void %s()" },
    { &MapperFunctions.Loop, "loop", "uint %s()" },
    { &MapperFunctions.ConsoleMessage, "console_message", "bool %s( string& message )" },
    { &MapperFunctions.RenderIface, "render_iface", "void %s( uint layer )" },
    { &MapperFunctions.RenderMap, "render_map", "void %s()" },
    { &MapperFunctions.MouseDown, "mouse_down", "bool %s( int click )" },
    { &MapperFunctions.MouseUp, "mouse_up", "bool %s( int click )" },
    { &MapperFunctions.MouseMove, "mouse_move", "void %s( int x, int y )" },
    { &MapperFunctions.KeyDown, "key_down", "bool %s( uint8 key, string& keyText )" },
    { &MapperFunctions.KeyUp, "key_up", "bool %s( uint8 key, string& keyText )" },
    { &MapperFunctions.InputLost, "input_lost", "void %s()" },
    { &MapperFunctions.CritterAnimation, "critter_animation", "string@ %s( int animType, uint crType, uint anim1, uint anim2, uint& pass, uint& flags, int& ox, int& oy )" },
    { &MapperFunctions.CritterAnimationSubstitute, "critter_animation_substitute", "bool %s( int animType, uint crTypeBase, uint anim1base, uint anim2base, uint& crType, uint& anim1, uint& anim2 )" },
    { &MapperFunctions.CritterAnimationFallout, "critter_animation_fallout", "bool %s( uint crType, uint& anim1, uint& anim2, uint& anim1ex, uint& anim2ex, uint& flags )" }
};
#endif // FOCLASSIC_MAPPER || FOCLASSIC_SERVER

#if defined (FOCLASSIC_SERVER)

ServerScriptFunctions  ServerFunctions;
ReservedScriptFunction ServerReservedFunctions[] =
{
    { &ServerFunctions.Init, "init", "void %s()" },
    { &ServerFunctions.Start, "start", "bool %s()" },
    { &ServerFunctions.GetStartTime, "get_start_time", "void %s( uint16& multiplier, uint16& year, uint16& month, uint16& day, uint16& hour, uint16& minute )" },
    { &ServerFunctions.Finish, "finish", "void %s()" },
    { &ServerFunctions.Loop, "loop", "uint %s()" },
    { &ServerFunctions.GlobalProcess, "global_process", "void %s( int processType, Critter& cr, Item@ car, float& curX, float& curY, float& toX, float& toY, float& speed, uint& encounterDescriptor, bool& waitForAnswer )" },
    { &ServerFunctions.GlobalInvite, "global_invite", "void %s( Critter& crLeader, Item@ car, uint encounterDescriptor, int combatMode, uint& mapId, uint16& hexX, uint16& hexY, uint8& dir )" },
    { &ServerFunctions.CritterAttack, "critter_attack", "void %s( Critter& cr, Critter& crTarget, ProtoItem& weapon, uint8 weaponMode, ProtoItem@ ammo )" },
    { &ServerFunctions.CritterAttacked, "critter_attacked", "void %s( Critter& cr, Critter& crAttacker )" },
    { &ServerFunctions.CritterStealing, "critter_stealing", "bool %s( Critter& cr, Critter& crThief, Item& item, uint count )" },
    { &ServerFunctions.CritterUseItem, "critter_use_item", "bool %s( Critter& cr, Item& item, Critter@ crTarget, Item@ itemTarget, Scenery@ scenTarget, uint param )" },
    { &ServerFunctions.CritterUseSkill, "critter_use_skill", "bool %s( Critter& cr, int skill, Critter@ crTarget, Item@ itemTarget, Scenery@ scenTarget ) " },
    { &ServerFunctions.CritterReloadWeapon, "critter_reload_weapon", "void %s( Critter& cr, Item& weapon, Item@ ammo )" },
    { &ServerFunctions.CritterInit, "critter_init", "void %s( Critter& cr, bool firstTime )" },
    { &ServerFunctions.CritterFinish, "critter_finish", "void %s( Critter& cr, bool toDelete )" },
    { &ServerFunctions.CritterIdle, "critter_idle", "void %s( Critter& cr )" },
    { &ServerFunctions.CritterDead, "critter_dead", "void %s( Critter& cr, Critter@ crKiller )" },
    { &ServerFunctions.CritterRespawn, "critter_respawn", "void %s( Critter& cr )" },
    { &ServerFunctions.CritterCheckMoveItem, "critter_check_move_item", "bool %s( Critter& cr, Item& item, uint8 toSlot, Item@ itemSwap )" },
    { &ServerFunctions.CritterMoveItem, "critter_move_item", "void %s( Critter& cr, Item& item, uint8 fromSlot )" },
    { &ServerFunctions.MapCritterIn, "map_critter_in", "void %s( Map& map, Critter& cr )" },
    { &ServerFunctions.MapCritterOut, "map_critter_out", "void %s( Map& map, Critter& cr )" },
    { &ServerFunctions.NpcPlaneBegin, "npc_plane_begin", "bool %s( Critter& npc, NpcPlane& plane, int reason, Critter@ crTarget, Item@ itemTarget )" },
    { &ServerFunctions.NpcPlaneEnd, "npc_plane_end", "bool %s( Critter& npc, NpcPlane& plane, int reason, Critter@ crTarget, Item@ itemTarget )" },
    { &ServerFunctions.NpcPlaneRun, "npc_plane_run", "bool %s( Critter& npc, NpcPlane& plane, int reason, uint& r0, uint& r1, uint& r2 )" },
    { &ServerFunctions.KarmaVoting, "karma_voting", "void %s( Critter& crFrom, Critter& crTo, bool valUp )" },
    { &ServerFunctions.CheckLook, "check_look", "bool %s( Map& map, Critter& cr, Critter& crOther )" },
    { &ServerFunctions.ItemCost, "item_cost", "uint %s( Item& item, Critter& cr, Critter& npc, bool buy )" },
    { &ServerFunctions.ItemsBarter, "items_barter", "bool %s( Item@[]& saleItems, uint[]& saleItemsCount, Item@[]& buyItems, uint[]& buyItemsCount, Critter& cr, Critter& npc )" },
    { &ServerFunctions.ItemsCrafted, "items_crafted", "void %s( Item@[]& items, uint[]& itemsCount, Item@[]& resources, Critter& cr )" },
    { &ServerFunctions.PlayerLevelUp, "player_levelup", "void %s( Critter& player, uint skillIndex, uint skillUp, uint perkIndex )" },
    { &ServerFunctions.TurnBasedBegin, "turn_based_begin", "void %s( Map& map )" },
    { &ServerFunctions.TurnBasedEnd, "turn_based_end", "void %s( Map& map )" },
    { &ServerFunctions.TurnBasedProcess, "turn_based_process", "void %s( Map& map, Critter& cr, bool beginTurn )" },
    { &ServerFunctions.TurnBasedSequence, "turn_based_sequence", "void %s( Map& map, Critter@[]& critters, Critter@ crFirstTurn )" },
    { &ServerFunctions.WorldSave, "world_save", "void %s( uint currentIndex, uint[]& deleteIndexes )" },
//  { &ServerFunctions.PlayerConnect, "player_connect", "bool %s( uint ip, string& message )" },
    { &ServerFunctions.PlayerRegistration, "player_registration", "bool %s( uint ip, string& name, uint& textMsg, uint& strNum )" },
    { &ServerFunctions.PlayerLogin, "player_login", "bool %s( uint ip, string& name, uint id, uint& textMsg, uint& strNum )" },
    { &ServerFunctions.PlayerGetAccess, "player_getaccess", "bool %s( Critter& cr, int access, string& password )" },
    { &ServerFunctions.PlayerAllowCommand, "player_allowcommand", "bool %s( Critter@ cr, string@ admin, uint8 command )" },
    { &ServerFunctions.CheckTrapLook, "check_trap_look", "bool %s( Map& map, Critter& cr, Item& trap )" }
};

#endif // FOCLASSIC_SERVER
