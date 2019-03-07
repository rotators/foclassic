#include "ScriptReservedFunctions.h"

#if defined (FOCLASSIC_CLIENT) || defined (FOCLASSIC_SERVER)

ClientScriptFunctions ClientFunctions;

ReservedFunctionsMap GetClientFunctionsMap()
{
    ReservedFunctionsMap client;

    client["start"] = ReservedFunction( &ClientFunctions.Start, "bool %s()" );
    client["finish"] = ReservedFunction( &ClientFunctions.Finish, "void %s()" );
    client["loop"] = ReservedFunction( &ClientFunctions.Loop, "uint %s()" );
    client["get_active_screens"] = ReservedFunction( &ClientFunctions.GetActiveScreens, "void %s( int[]& screens )" );
    client["screen_change"] = ReservedFunction( &ClientFunctions.ScreenChange, "void %s( bool show, int screen, int p0, int p1, int p2 )" );
    client["render_iface"] = ReservedFunction( &ClientFunctions.RenderIface, "void %s( uint layer )" );
    client["render_map"] = ReservedFunction( &ClientFunctions.RenderMap, "void %s()" );
    client["mouse_down"] = ReservedFunction( &ClientFunctions.MouseDown, "bool %s( int click )" );
    client["mouse_up"] = ReservedFunction( &ClientFunctions.MouseUp, "bool %s( int click )" );
    client["mouse_move"] = ReservedFunction( &ClientFunctions.MouseMove, "void %s( int x, int y )" );
    client["key_down"] = ReservedFunction( &ClientFunctions.KeyDown, "bool %s( uint8 key, string& keyText )" );
    client["key_up"] = ReservedFunction( &ClientFunctions.KeyUp, "bool %s( uint8 key, string& keyText )" );
    client["input_lost"] = ReservedFunction( &ClientFunctions.InputLost, "void %s()" );
    client["critter_in"] = ReservedFunction( &ClientFunctions.CritterIn, "void %s( CritterCl& cr )" );
    client["critter_out"] = ReservedFunction( &ClientFunctions.CritterOut, "void %s( CritterCl& cr )" );
    client["item_map_in"] = ReservedFunction( &ClientFunctions.ItemMapIn, "void %s( ItemCl& item )" );
    client["item_map_changed"] = ReservedFunction( &ClientFunctions.ItemMapChanged, "void %s( ItemCl& itemNew, ItemCl& itemOld )" );
    client["item_map_out"] = ReservedFunction( &ClientFunctions.ItemMapOut, "void %s( ItemCl& item )" );
    client["item_inv_in"] = ReservedFunction( &ClientFunctions.ItemInvIn, "void %s( ItemCl& item )" );
    client["item_inv_out"] = ReservedFunction( &ClientFunctions.ItemInvOut, "void %s( ItemCl& item )" );
    client["map_message"] = ReservedFunction( &ClientFunctions.MapMessage, "bool %s( string& message, uint16& hexX, uint16& hexY, uint& color, uint& delay )" );
    client["in_message"] = ReservedFunction( &ClientFunctions.InMessage, "bool %s( string& message, int& sayType, uint& critterId, uint& delay )" );
    client["out_message"] = ReservedFunction( &ClientFunctions.OutMessage, "bool %s( string& message,int& sayType)" );
    client["to_hit"] = ReservedFunction( &ClientFunctions.ToHit, "int %s( CritterCl& chosen, CritterCl& crTarget, ProtoItem& weapon, uint8 weaponMode )" );
    client["hit_aim"] = ReservedFunction( &ClientFunctions.HitAim, "void %s( uint8& aim )" );
    client["combat_result"] = ReservedFunction( &ClientFunctions.CombatResult, "void %s( uint[]& data )" );
    client["generic_description"] = ReservedFunction( &ClientFunctions.GenericDesc, "string %s( int descriptionType, int& offsetX, int& offsetY )" );
    client["item_description"] = ReservedFunction( &ClientFunctions.ItemLook, "string %s( ItemCl& item, int lookType )" );
    client["critter_description"] = ReservedFunction( &ClientFunctions.CritterLook, "string %s( CritterCl& cr, int lookType )" );
    client["get_elevator"] = ReservedFunction( &ClientFunctions.GetElevator, "bool %s( uint type, uint[]& data )" );
    client["item_cost"] = ReservedFunction( &ClientFunctions.ItemCost, "uint %s( ItemCl& item, CritterCl& chosen, CritterCl& npc, bool sell )" );
    client["check_perk"] = ReservedFunction( &ClientFunctions.PerkCheck, "bool %s( CritterCl& chosen, uint perk )" );
    client["player_data_generate"] = ReservedFunction( &ClientFunctions.PlayerGeneration, "void %s( int[]& data )" );
    client["player_data_check"] = ReservedFunction( &ClientFunctions.PlayerGenerationCheck, "bool %s( string& name, int[]& data )" );
    client["critter_action"] = ReservedFunction( &ClientFunctions.CritterAction, "void %s( bool localCall, CritterCl& cr, int action, int actionExt, ItemCl@ item )" );
    client["animation2d_process"] = ReservedFunction( &ClientFunctions.Animation2dProcess, "void %s( bool animateStay, CritterCl& cr, uint anim1, uint anim2, ItemCl@ item )" );
    client["animation3d_process"] = ReservedFunction( &ClientFunctions.Animation3dProcess, "void %s( bool animateStay, CritterCl& cr, uint anim1, uint anim2, ItemCl@ item )" );
    client["items_collection"] = ReservedFunction( &ClientFunctions.ItemsCollection, "void %s( int collection, ItemCl@[]& items )" );
    client["critter_animation"] = ReservedFunction( &ClientFunctions.CritterAnimation, "string@ %s( int animType, uint crType, uint anim1, uint anim2, uint& pass, uint& flags, int& ox, int& oy )" );
    client["critter_animation_substitute"] = ReservedFunction( &ClientFunctions.CritterAnimationSubstitute, "bool %s( int animType, uint crTypeBase, uint anim1base, uint anim2base, uint& crType, uint& anim1, uint& anim2 )" );
    client["critter_animation_fallout"] = ReservedFunction( &ClientFunctions.CritterAnimationFallout, "bool %s( uint crType, uint& anim1, uint& anim2, uint& anim1ex, uint& anim2ex, uint& flags )" );
    client["filename_logfile"] = ReservedFunction( &ClientFunctions.FilenameLogfile, "void %s( string& filename )" );
    client["filename_screenshot"] = ReservedFunction( &ClientFunctions.FilenameScreenshot, "void %s( string& filename )" );
    client["critter_check_move_item"] = ReservedFunction( &ClientFunctions.CritterCheckMoveItem, "bool %s( CritterCl& cr, ItemCl& item, uint8 toSlot, ItemCl@ itemSwap )" );

    return client;
};

#endif // FOCLASSIC_CLIENT || FOCLASSIC_SERVER

#if defined (FOCLASSIC_MAPPER) || defined (FOCLASSIC_SERVER)

MapperScriptFunctions MapperFunctions;

ReservedFunctionsMap GetMapperFunctionsMap()
{
    ReservedFunctionsMap mapper;

    mapper["start"] = ReservedFunction( &MapperFunctions.Start, "void %s()" );
    mapper["finish"] = ReservedFunction( &MapperFunctions.Finish, "void %s()" );
    mapper["loop"] = ReservedFunction( &MapperFunctions.Loop, "uint %s()" );
    mapper["console_message"] = ReservedFunction( &MapperFunctions.ConsoleMessage, "bool %s( string& message )" );
    mapper["render_iface"] = ReservedFunction( &MapperFunctions.RenderIface, "void %s( uint layer )" );
    mapper["render_map"] = ReservedFunction( &MapperFunctions.RenderMap, "void %s()" );
    mapper["mouse_down"] = ReservedFunction( &MapperFunctions.MouseDown, "bool %s( int click )" );
    mapper["mouse_up"] = ReservedFunction( &MapperFunctions.MouseUp, "bool %s( int click )" );
    mapper["mouse_move"] = ReservedFunction( &MapperFunctions.MouseMove, "void %s( int x, int y )" );
    mapper["key_down"] = ReservedFunction( &MapperFunctions.KeyDown, "bool %s( uint8 key, string& keyText )" );
    mapper["key_up"] = ReservedFunction( &MapperFunctions.KeyUp, "bool %s( uint8 key, string& keyText )" );
    mapper["input_lost"] = ReservedFunction( &MapperFunctions.InputLost, "void %s()" );
    mapper["critter_animation"] = ReservedFunction( &MapperFunctions.CritterAnimation, "string@ %s( int animType, uint crType, uint anim1, uint anim2, uint& pass, uint& flags, int& ox, int& oy )" );
    mapper["critter_animation_substitute"] = ReservedFunction( &MapperFunctions.CritterAnimationSubstitute, "bool %s( int animType, uint crTypeBase, uint anim1base, uint anim2base, uint& crType, uint& anim1, uint& anim2 )" );
    mapper["critter_animation_fallout"] = ReservedFunction( &MapperFunctions.CritterAnimationFallout, "bool %s( uint crType, uint& anim1, uint& anim2, uint& anim1ex, uint& anim2ex, uint& flags )" );

    return mapper;
};

#endif // FOCLASSIC_MAPPER || FOCLASSIC_SERVER

#if defined (FOCLASSIC_SERVER)

ServerScriptFunctions ServerFunctions;

ReservedFunctionsMap GetServerFunctionsMap()
{
    ReservedFunctionsMap server;

    server["init"] = ReservedFunction( &ServerFunctions.Init, "void %s()" );
    server["start"] = ReservedFunction( &ServerFunctions.Start, "bool %s()" );
    server["get_start_time"] = ReservedFunction( &ServerFunctions.GetStartTime, "void %s( uint16& multiplier, uint16& year, uint16& month, uint16& day, uint16& hour, uint16& minute )" );
    server["finish"] = ReservedFunction( &ServerFunctions.Finish, "void %s()" );
    server["loop"] = ReservedFunction( &ServerFunctions.Loop, "uint %s()" );
    server["global_process"] = ReservedFunction( &ServerFunctions.GlobalProcess, "void %s( int processType, Critter& cr, Item@ car, float& curX, float& curY, float& toX, float& toY, float& speed, uint& encounterDescriptor, bool& waitForAnswer )" );
    server["global_invite"] = ReservedFunction( &ServerFunctions.GlobalInvite, "void %s( Critter& crLeader, Item@ car, uint encounterDescriptor, int combatMode, uint& mapId, uint16& hexX, uint16& hexY, uint8& dir )" );
    server["critter_attack"] = ReservedFunction( &ServerFunctions.CritterAttack, "void %s( Critter& cr, Critter& crTarget, ProtoItem& weapon, uint8 weaponMode, ProtoItem@ ammo )" );
    server["critter_attacked"] = ReservedFunction( &ServerFunctions.CritterAttacked, "void %s( Critter& cr, Critter& crAttacker )" );
    server["critter_stealing"] = ReservedFunction( &ServerFunctions.CritterStealing, "bool %s( Critter& cr, Critter& crThief, Item& item, uint count )" );
    server["critter_use_item"] = ReservedFunction( &ServerFunctions.CritterUseItem, "bool %s( Critter& cr, Item& item, Critter@ crTarget, Item@ itemTarget, Scenery@ scenTarget, uint param )" );
    server["critter_use_skill"] = ReservedFunction( &ServerFunctions.CritterUseSkill, "bool %s( Critter& cr, int skill, Critter@ crTarget, Item@ itemTarget, Scenery@ scenTarget ) " );
    server["critter_reload_weapon"] = ReservedFunction( &ServerFunctions.CritterReloadWeapon, "void %s( Critter& cr, Item& weapon, Item@ ammo )" );
    server["critter_init"] = ReservedFunction( &ServerFunctions.CritterInit, "void %s( Critter& cr, bool firstTime )" );
    server["critter_finish"] = ReservedFunction( &ServerFunctions.CritterFinish, "void %s( Critter& cr, bool toDelete )" );
    server["critter_idle"] = ReservedFunction( &ServerFunctions.CritterIdle, "void %s( Critter& cr )" );
    server["critter_dead"] = ReservedFunction( &ServerFunctions.CritterDead, "void %s( Critter& cr, Critter@ crKiller )" );
    server["critter_respawn"] = ReservedFunction( &ServerFunctions.CritterRespawn, "void %s( Critter& cr )" );
    server["critter_check_move_item"] = ReservedFunction( &ServerFunctions.CritterCheckMoveItem, "bool %s( Critter& cr, Item& item, uint8 toSlot, Item@ itemSwap )" );
    server["critter_move_item"] = ReservedFunction( &ServerFunctions.CritterMoveItem, "void %s( Critter& cr, Item& item, uint8 fromSlot )" );
    server["map_critter_in"] = ReservedFunction( &ServerFunctions.MapCritterIn, "void %s( Map& map, Critter& cr )" );
    server["map_critter_out"] = ReservedFunction( &ServerFunctions.MapCritterOut, "void %s( Map& map, Critter& cr )" );
    server["npc_plane_begin"] = ReservedFunction( &ServerFunctions.NpcPlaneBegin, "bool %s( Critter& npc, NpcPlane& plane, int reason, Critter@ crTarget, Item@ itemTarget )" );
    server["npc_plane_end"] = ReservedFunction( &ServerFunctions.NpcPlaneEnd, "bool %s( Critter& npc, NpcPlane& plane, int reason, Critter@ crTarget, Item@ itemTarget )" );
    server["npc_plane_run"] = ReservedFunction( &ServerFunctions.NpcPlaneRun, "bool %s( Critter& npc, NpcPlane& plane, int reason, uint& r0, uint& r1, uint& r2 )" );
    server["karma_voting"] = ReservedFunction( &ServerFunctions.KarmaVoting, "void %s( Critter& crFrom, Critter& crTo, bool valUp )" );
    server["check_look"] = ReservedFunction( &ServerFunctions.CheckLook, "bool %s( Map& map, Critter& cr, Critter& crOther )" );
    server["item_cost"] = ReservedFunction( &ServerFunctions.ItemCost, "uint %s( Item& item, Critter& cr, Critter& npc, bool buy )" );
    server["items_barter"] = ReservedFunction( &ServerFunctions.ItemsBarter, "bool %s( Item@[]& saleItems, uint[]& saleItemsCount, Item@[]& buyItems, uint[]& buyItemsCount, Critter& cr, Critter& npc )" );
    server["items_crafted"] = ReservedFunction( &ServerFunctions.ItemsCrafted, "void %s( Item@[]& items, uint[]& itemsCount, Item@[]& resources, Critter& cr )" );
    server["player_levelup"] = ReservedFunction( &ServerFunctions.PlayerLevelUp, "void %s( Critter& player, uint skillIndex, uint skillUp, uint perkIndex )" );
    server["turn_based_begin"] = ReservedFunction( &ServerFunctions.TurnBasedBegin, "void %s( Map& map )" );
    server["turn_based_end"] = ReservedFunction( &ServerFunctions.TurnBasedEnd, "void %s( Map& map )" );
    server["turn_based_process"] = ReservedFunction( &ServerFunctions.TurnBasedProcess, "void %s( Map& map, Critter& cr, bool beginTurn )" );
    server["turn_based_sequence"] = ReservedFunction( &ServerFunctions.TurnBasedSequence, "void %s( Map& map, Critter@[]& critters, Critter@ crFirstTurn )" );
    server["world_save"] = ReservedFunction( &ServerFunctions.WorldSave, "void %s( uint currentIndex, uint[]& deleteIndexes )" );
//  server["player_connect"] = ReservedFunction( &ServerFunctions.PlayerConnect, "bool %s( uint ip, string& message )" );
    server["player_registration"] = ReservedFunction( &ServerFunctions.PlayerRegistration, "bool %s( uint ip, string& name, uint& textMsg, uint& strNum )" );
    server["player_login"] = ReservedFunction( &ServerFunctions.PlayerLogin, "bool %s( uint ip, string& name, uint id, uint& textMsg, uint& strNum )" );
    server["player_getaccess"] = ReservedFunction( &ServerFunctions.PlayerGetAccess, "bool %s( Critter& cr, int access, string& password )" );
    server["player_allowcommand"] = ReservedFunction( &ServerFunctions.PlayerAllowCommand, "bool %s( Critter@ cr, string@ admin, uint8 command )" );
    server["check_trap_look"] = ReservedFunction( &ServerFunctions.CheckTrapLook, "bool %s( Map& map, Critter& cr, Item& trap )" );

    return server;
};

#endif // FOCLASSIC_SERVER
