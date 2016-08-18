
#include "extdll.h"

#ifdef _WIN32
extern HINSTANCE h_Library;
typedef void (FAR *LINK_ENTITY_FUNC)(entvars_t *);
#else
extern void *h_Library;
typedef void (*LINK_ENTITY_FUNC)(entvars_t *);
#include <dlfcn.h>         
#endif



#ifdef _WIN32
#define LINK_ENTITY_TO_FUNC(mapClassName, mapClassString, otherClassName) \
	extern "C" _declspec( dllexport ) void mapClassName( entvars_t *pev ); \
	void mapClassName( entvars_t *pev ) { \
      static LINK_ENTITY_FUNC otherClassName = NULL; \
	 static int missing =0; \
	  if (missing==1) return; \
      if (otherClassName == NULL) \
         otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, mapClassString); \
if (otherClassName == NULL) { missing=1; \
UTIL_LogPrintf( "[ADMIN] ERROR: couldn't find entity %s\n", mapClassString); return;} \
      (*otherClassName)(pev); }
#else
#define LINK_ENTITY_TO_FUNC(mapClassName, mapClassString, otherClassName) \
      extern "C" void mapClassName( entvars_t *pev ); \
 void mapClassName( entvars_t *pev ) { \
      static LINK_ENTITY_FUNC otherClassName = NULL; \
      static int missing=0; \
      if ( missing == 1) return; \
      if (otherClassName == NULL) \
         otherClassName = (LINK_ENTITY_FUNC)dlsym(h_Library, mapClassString); \
      if (otherClassName == NULL) { missing=1;  \
 	UTIL_LogPrintf( "[ADMIN] ERROR: couldn't find entity %s\n", mapClassString); return;} \
       (*otherClassName)(pev); }
#endif



// common weapons name (firearms and cs)
LINK_ENTITY_TO_FUNC( weapon_ak47,"weapon_ak47",  other_weapon_ak47);
LINK_ENTITY_TO_FUNC( ammo_ak47,"ammo_ak47", other_ammo_ak47 );
LINK_ENTITY_TO_FUNC( weapon_aug,"weapon_aug",other_weapon_aug );
LINK_ENTITY_TO_FUNC( ammo_aug,"ammo_aug", other_ammo_aug );
LINK_ENTITY_TO_FUNC( grenade,"grenade",other_grenade );
LINK_ENTITY_TO_FUNC( weapon_frag,"weapon_frag", other_weapon_frag);
LINK_ENTITY_TO_FUNC( weapon_knife,"weapon_knife",other_weapon_knife );         
LINK_ENTITY_TO_FUNC( weapon_flashbang,"weapon_flashbang",other_weapon_flashbang );
LINK_ENTITY_TO_FUNC( ammo_flashbang,"ammo_flashbang",other_ammo_flashbang );
LINK_ENTITY_TO_FUNC( weaponbox,"weaponbox",other_weaponbox ); 




// entities for Valve's hl.dll and Standard SDK...
LINK_ENTITY_TO_FUNC(aiscripted_sequence, "aiscripted_sequence", other_aiscripted_sequence);
LINK_ENTITY_TO_FUNC(ambient_generic, "ambient_generic", other_ambient_generic);
LINK_ENTITY_TO_FUNC(ammo_357, "ammo_357", other_ammo_357);
LINK_ENTITY_TO_FUNC(ammo_9mmAR, "ammo_9mmAR", other_ammo_9mmAR);
LINK_ENTITY_TO_FUNC(ammo_9mmbox, "ammo_9mmbox", other_ammo_9mmbox);
LINK_ENTITY_TO_FUNC(ammo_9mmclip, "ammo_9mmclip", other_ammo_9mmclip);
LINK_ENTITY_TO_FUNC(ammo_ARgrenades, "ammo_ARgrenades", other_ammo_ARgrenades);
LINK_ENTITY_TO_FUNC(ammo_buckshot, "ammo_buckshot", other_ammo_buckshot);
LINK_ENTITY_TO_FUNC(ammo_crossbow, "ammo_crossbow", other_ammo_crossbow);
LINK_ENTITY_TO_FUNC(ammo_egonclip, "ammo_egonclip", other_ammo_egonclip);
LINK_ENTITY_TO_FUNC(ammo_gaussclip, "ammo_gaussclip", other_ammo_gaussclip);
LINK_ENTITY_TO_FUNC(ammo_glockclip, "ammo_glockclip", other_ammo_glockclip);
LINK_ENTITY_TO_FUNC(ammo_mp5clip, "ammo_mp5clip", other_ammo_mp5clip);
LINK_ENTITY_TO_FUNC(ammo_mp5grenades, "ammo_mp5grenades", other_ammo_mp5grenades);
LINK_ENTITY_TO_FUNC(ammo_rpgclip, "ammo_rpgclip", other_ammo_rpgclip);
LINK_ENTITY_TO_FUNC(beam, "beam", other_beam);
LINK_ENTITY_TO_FUNC(bmortar, "bmortar", other_bmortar);
LINK_ENTITY_TO_FUNC(bodyque, "bodyque", other_bodyque);
LINK_ENTITY_TO_FUNC(button_target, "button_target", other_button_target);
LINK_ENTITY_TO_FUNC(cine_blood, "cine_blood", other_cine_blood);
LINK_ENTITY_TO_FUNC(controller_energy_ball, "controller_energy_ball", other_controller_energy_ball);
LINK_ENTITY_TO_FUNC(controller_head_ball, "controller_head_ball", other_controller_head_ball);
LINK_ENTITY_TO_FUNC(crossbow_bolt, "crossbow_bolt", other_crossbow_bolt);
LINK_ENTITY_TO_FUNC(cycler, "cycler", other_cycler);
LINK_ENTITY_TO_FUNC(cycler_prdroid, "cycler_prdroid", other_cycler_prdroid);
LINK_ENTITY_TO_FUNC(cycler_sprite, "cycler_sprite", other_cycler_sprite);
LINK_ENTITY_TO_FUNC(cycler_weapon, "cycler_weapon", other_cycler_weapon);
LINK_ENTITY_TO_FUNC(cycler_wreckage, "cycler_wreckage", other_cycler_wreckage);
LINK_ENTITY_TO_FUNC(DelayedUse, "DelayedUse", other_DelayedUse);
LINK_ENTITY_TO_FUNC(env_beam, "env_beam", other_env_beam);
LINK_ENTITY_TO_FUNC(env_beverage, "env_beverage", other_env_beverage);
LINK_ENTITY_TO_FUNC(env_blood, "env_blood", other_env_blood);
LINK_ENTITY_TO_FUNC(env_bubbles, "env_bubbles", other_env_bubbles);
LINK_ENTITY_TO_FUNC(env_debris, "env_debris", other_env_debris);
LINK_ENTITY_TO_FUNC(env_explosion, "env_explosion", other_env_explosion);
LINK_ENTITY_TO_FUNC(env_fade, "env_fade", other_env_fade);
LINK_ENTITY_TO_FUNC(env_funnel, "env_funnel", other_env_funnel);
LINK_ENTITY_TO_FUNC(env_global, "env_global", other_env_global);
LINK_ENTITY_TO_FUNC(env_glow, "env_glow", other_env_glow);
LINK_ENTITY_TO_FUNC(env_laser, "env_laser", other_env_laser);
LINK_ENTITY_TO_FUNC(env_lightning, "env_lightning", other_env_lightning);
LINK_ENTITY_TO_FUNC(env_message, "env_message", other_env_message);
LINK_ENTITY_TO_FUNC(env_render, "env_render", other_env_render);
LINK_ENTITY_TO_FUNC(env_shake, "env_shake", other_env_shake);
LINK_ENTITY_TO_FUNC(env_shooter, "env_shooter", other_env_shooter);
LINK_ENTITY_TO_FUNC(env_smoker, "env_smoker", other_env_smoker);
LINK_ENTITY_TO_FUNC(env_sound, "env_sound", other_env_sound);
LINK_ENTITY_TO_FUNC(env_spark, "env_spark", other_env_spark);
LINK_ENTITY_TO_FUNC(env_sprite, "env_sprite", other_env_sprite);
LINK_ENTITY_TO_FUNC(fireanddie, "fireanddie", other_fireanddie);
LINK_ENTITY_TO_FUNC(func_breakable, "func_breakable", other_func_breakable);
LINK_ENTITY_TO_FUNC(func_button, "func_button", other_func_button);
LINK_ENTITY_TO_FUNC(func_conveyor, "func_conveyor", other_func_conveyor);
LINK_ENTITY_TO_FUNC(func_door, "func_door", other_func_door);
LINK_ENTITY_TO_FUNC(func_door_rotating, "func_door_rotating", other_func_door_rotating);
LINK_ENTITY_TO_FUNC(func_friction, "func_friction", other_func_friction);
LINK_ENTITY_TO_FUNC(func_guntarget, "func_guntarget", other_func_guntarget);
LINK_ENTITY_TO_FUNC(func_healthcharger, "func_healthcharger", other_func_healthcharger);
LINK_ENTITY_TO_FUNC(func_illusionary, "func_illusionary", other_func_illusionary);
LINK_ENTITY_TO_FUNC(func_ladder, "func_ladder", other_func_ladder);
LINK_ENTITY_TO_FUNC(func_monsterclip, "func_monsterclip", other_func_monsterclip);
LINK_ENTITY_TO_FUNC(func_mortar_field, "func_mortar_field", other_func_mortar_field);
LINK_ENTITY_TO_FUNC(func_pendulum, "func_pendulum", other_func_pendulum);
LINK_ENTITY_TO_FUNC(func_plat, "func_plat", other_func_plat);
LINK_ENTITY_TO_FUNC(func_platrot, "func_platrot", other_func_platrot);
LINK_ENTITY_TO_FUNC(func_pushable, "func_pushable", other_func_pushable);
LINK_ENTITY_TO_FUNC(func_recharge, "func_recharge", other_func_recharge);
LINK_ENTITY_TO_FUNC(func_rotating, "func_rotating", other_func_rotating);
LINK_ENTITY_TO_FUNC(func_rot_button, "func_rot_button", other_func_rot_button);
LINK_ENTITY_TO_FUNC(func_tank, "func_tank", other_func_tank);
LINK_ENTITY_TO_FUNC(func_tankcontrols, "func_tankcontrols", other_func_tankcontrols);
LINK_ENTITY_TO_FUNC(func_tanklaser, "func_tanklaser", other_func_tanklaser);
LINK_ENTITY_TO_FUNC(func_tankmortar, "func_tankmortar", other_func_tankmortar);
LINK_ENTITY_TO_FUNC(func_tankrocket, "func_tankrocket", other_func_tankrocket);
LINK_ENTITY_TO_FUNC(func_trackautochange, "func_trackautochange", other_func_trackautochange);
LINK_ENTITY_TO_FUNC(func_trackchange, "func_trackchange", other_func_trackchange);
LINK_ENTITY_TO_FUNC(func_tracktrain, "func_tracktrain", other_func_tracktrain);
LINK_ENTITY_TO_FUNC(func_train, "func_train", other_func_train);
LINK_ENTITY_TO_FUNC(func_traincontrols, "func_traincontrols", other_func_traincontrols);
LINK_ENTITY_TO_FUNC(func_wall, "func_wall", other_func_wall);
LINK_ENTITY_TO_FUNC(func_wall_toggle, "func_wall_toggle", other_func_wall_toggle);
LINK_ENTITY_TO_FUNC(func_water, "func_water", other_func_water);
LINK_ENTITY_TO_FUNC(game_counter, "game_counter", other_game_counter);
LINK_ENTITY_TO_FUNC(game_counter_set, "game_counter_set", other_game_counter_set);
LINK_ENTITY_TO_FUNC(game_end, "game_end", other_game_end);
LINK_ENTITY_TO_FUNC(game_player_equip, "game_player_equip", other_game_player_equip);
LINK_ENTITY_TO_FUNC(game_player_hurt, "game_player_hurt", other_game_player_hurt);
LINK_ENTITY_TO_FUNC(game_player_team, "game_player_team", other_game_player_team);
LINK_ENTITY_TO_FUNC(game_score, "game_score", other_game_score);
LINK_ENTITY_TO_FUNC(game_team_master, "game_team_master", other_game_team_master);
LINK_ENTITY_TO_FUNC(game_team_set, "game_team_set", other_game_team_set);
LINK_ENTITY_TO_FUNC(game_text, "game_text", other_game_text);
LINK_ENTITY_TO_FUNC(game_zone_player, "game_zone_player", other_game_zone_player);
LINK_ENTITY_TO_FUNC(garg_stomp, "garg_stomp", other_garg_stomp);
LINK_ENTITY_TO_FUNC(gibshooter, "gibshooter", other_gibshooter);
LINK_ENTITY_TO_FUNC(hornet, "hornet", other_hornet);
LINK_ENTITY_TO_FUNC(hvr_rocket, "hvr_rocket", other_hvr_rocket);
LINK_ENTITY_TO_FUNC(infodecal, "infodecal", other_infodecal);
LINK_ENTITY_TO_FUNC(info_bigmomma, "info_bigmomma", other_info_bigmomma);
LINK_ENTITY_TO_FUNC(info_intermission, "info_intermission", other_info_intermission);
LINK_ENTITY_TO_FUNC(info_landmark, "info_landmark", other_info_landmark);
LINK_ENTITY_TO_FUNC(info_node, "info_node", other_info_node);
LINK_ENTITY_TO_FUNC(info_node_air, "info_node_air", other_info_node_air);
LINK_ENTITY_TO_FUNC(info_null, "info_null", other_info_null);
LINK_ENTITY_TO_FUNC(info_player_deathmatch, "info_player_deathmatch", other_info_player_deathmatch);
LINK_ENTITY_TO_FUNC(info_player_start, "info_player_start", other_info_player_start);
LINK_ENTITY_TO_FUNC(info_target, "info_target", other_info_target);
LINK_ENTITY_TO_FUNC(info_teleport_destination, "info_teleport_destination", other_info_teleport_destination);
LINK_ENTITY_TO_FUNC(item_airtank, "item_airtank", other_item_airtank);
LINK_ENTITY_TO_FUNC(item_antidote, "item_antidote", other_item_antidote);
LINK_ENTITY_TO_FUNC(item_battery, "item_battery", other_item_battery);
LINK_ENTITY_TO_FUNC(item_healthkit, "item_healthkit", other_item_healthkit);
LINK_ENTITY_TO_FUNC(item_longjump, "item_longjump", other_item_longjump);
LINK_ENTITY_TO_FUNC(item_security, "item_security", other_item_security);
LINK_ENTITY_TO_FUNC(item_sodacan, "item_sodacan", other_item_sodacan);
LINK_ENTITY_TO_FUNC(item_suit, "item_suit", other_item_suit);
LINK_ENTITY_TO_FUNC(laser_spot, "laser_spot", other_laser_spot);
LINK_ENTITY_TO_FUNC(light, "light", other_light);
LINK_ENTITY_TO_FUNC(light_environment, "light_environment", other_light_environment);
LINK_ENTITY_TO_FUNC(light_spot, "light_spot", other_light_spot);
LINK_ENTITY_TO_FUNC(momentary_door, "momentary_door", other_momentary_door);
LINK_ENTITY_TO_FUNC(momentary_rot_button, "momentary_rot_button", other_momentary_rot_button);
LINK_ENTITY_TO_FUNC(monstermaker, "monstermaker", other_monstermaker);
LINK_ENTITY_TO_FUNC(monster_alien_controller, "monster_alien_controller", other_monster_alien_controller);
LINK_ENTITY_TO_FUNC(monster_alien_grunt, "monster_alien_grunt", other_monster_alien_grunt);
LINK_ENTITY_TO_FUNC(monster_alien_slave, "monster_alien_slave", other_monster_alien_slave);
LINK_ENTITY_TO_FUNC(monster_apache, "monster_apache", other_monster_apache);
LINK_ENTITY_TO_FUNC(monster_babycrab, "monster_babycrab", other_monster_babycrab);
LINK_ENTITY_TO_FUNC(monster_barnacle, "monster_barnacle", other_monster_barnacle);
LINK_ENTITY_TO_FUNC(monster_barney, "monster_barney", other_monster_barney);
LINK_ENTITY_TO_FUNC(monster_barney_dead, "monster_barney_dead", other_monster_barney_dead);
LINK_ENTITY_TO_FUNC(monster_bigmomma, "monster_bigmomma", other_monster_bigmomma);
LINK_ENTITY_TO_FUNC(monster_bloater, "monster_bloater", other_monster_bloater);
LINK_ENTITY_TO_FUNC(monster_bullchicken, "monster_bullchicken", other_monster_bullchicken);
LINK_ENTITY_TO_FUNC(monster_cine2_hvyweapons, "monster_cine2_hvyweapons", other_monster_cine2_hvyweapons);
LINK_ENTITY_TO_FUNC(monster_cine2_scientist, "monster_cine2_scientist", other_monster_cine2_scientist);
LINK_ENTITY_TO_FUNC(monster_cine2_slave, "monster_cine2_slave", other_monster_cine2_slave);
LINK_ENTITY_TO_FUNC(monster_cine3_barney, "monster_cine3_barney", other_monster_cine3_barney);
LINK_ENTITY_TO_FUNC(monster_cine3_scientist, "monster_cine3_scientist", other_monster_cine3_scientist);
LINK_ENTITY_TO_FUNC(monster_cine_barney, "monster_cine_barney", other_monster_cine_barney);
LINK_ENTITY_TO_FUNC(monster_cine_panther, "monster_cine_panther", other_monster_cine_panther);
LINK_ENTITY_TO_FUNC(monster_cine_scientist, "monster_cine_scientist", other_monster_cine_scientist);
LINK_ENTITY_TO_FUNC(monster_cockroach, "monster_cockroach", other_monster_cockroach);
LINK_ENTITY_TO_FUNC(monster_flyer, "monster_flyer", other_monster_flyer);
LINK_ENTITY_TO_FUNC(monster_flyer_flock, "monster_flyer_flock", other_monster_flyer_flock);
LINK_ENTITY_TO_FUNC(monster_furniture, "monster_furniture", other_monster_furniture);
LINK_ENTITY_TO_FUNC(monster_gargantua, "monster_gargantua", other_monster_gargantua);
LINK_ENTITY_TO_FUNC(monster_generic, "monster_generic", other_monster_generic);
LINK_ENTITY_TO_FUNC(monster_gman, "monster_gman", other_monster_gman);
LINK_ENTITY_TO_FUNC(monster_grunt_repel, "monster_grunt_repel", other_monster_grunt_repel);
LINK_ENTITY_TO_FUNC(monster_headcrab, "monster_headcrab", other_monster_headcrab);
LINK_ENTITY_TO_FUNC(monster_hevsuit_dead, "monster_hevsuit_dead", other_monster_hevsuit_dead);
LINK_ENTITY_TO_FUNC(monster_hgrunt_dead, "monster_hgrunt_dead", other_monster_hgrunt_dead);
LINK_ENTITY_TO_FUNC(monster_houndeye, "monster_houndeye", other_monster_houndeye);
LINK_ENTITY_TO_FUNC(monster_human_assassin, "monster_human_assassin", other_monster_human_assassin);
LINK_ENTITY_TO_FUNC(monster_human_grunt, "monster_human_grunt", other_monster_human_grunt);
LINK_ENTITY_TO_FUNC(monster_ichthyosaur, "monster_ichthyosaur", other_monster_ichthyosaur);
LINK_ENTITY_TO_FUNC(monster_leech, "monster_leech", other_monster_leech);
LINK_ENTITY_TO_FUNC(monster_miniturret, "monster_miniturret", other_monster_miniturret);
LINK_ENTITY_TO_FUNC(monster_mortar, "monster_mortar", other_monster_mortar);
LINK_ENTITY_TO_FUNC(monster_nihilanth, "monster_nihilanth", other_monster_nihilanth);
LINK_ENTITY_TO_FUNC(monster_osprey, "monster_osprey", other_monster_osprey);
LINK_ENTITY_TO_FUNC(monster_rat, "monster_rat", other_monster_rat);
LINK_ENTITY_TO_FUNC(monster_satchel, "monster_satchel", other_monster_satchel);
LINK_ENTITY_TO_FUNC(monster_scientist, "monster_scientist", other_monster_scientist);
LINK_ENTITY_TO_FUNC(monster_scientist_dead, "monster_scientist_dead", other_monster_scientist_dead);
LINK_ENTITY_TO_FUNC(monster_sentry, "monster_sentry", other_monster_sentry);
LINK_ENTITY_TO_FUNC(monster_sitting_scientist, "monster_sitting_scientist", other_monster_sitting_scientist);
LINK_ENTITY_TO_FUNC(monster_snark, "monster_snark", other_monster_snark);
LINK_ENTITY_TO_FUNC(monster_tentacle, "monster_tentacle", other_monster_tentacle);
LINK_ENTITY_TO_FUNC(monster_tentaclemaw, "monster_tentaclemaw", other_monster_tentaclemaw);
LINK_ENTITY_TO_FUNC(monster_tripmine, "monster_tripmine", other_monster_tripmine);
LINK_ENTITY_TO_FUNC(monster_turret, "monster_turret", other_monster_turret);
LINK_ENTITY_TO_FUNC(monster_vortigaunt, "monster_vortigaunt", other_monster_vortigaunt);
LINK_ENTITY_TO_FUNC(monster_zombie, "monster_zombie", other_monster_zombie);
LINK_ENTITY_TO_FUNC(multisource, "multisource", other_multisource);
LINK_ENTITY_TO_FUNC(multi_manager, "multi_manager", other_multi_manager);
LINK_ENTITY_TO_FUNC(nihilanth_energy_ball, "nihilanth_energy_ball", other_nihilanth_energy_ball);
LINK_ENTITY_TO_FUNC(node_viewer, "node_viewer", other_node_viewer);
LINK_ENTITY_TO_FUNC(node_viewer_fly, "node_viewer_fly", other_node_viewer_fly);
LINK_ENTITY_TO_FUNC(node_viewer_human, "node_viewer_human", other_node_viewer_human);
LINK_ENTITY_TO_FUNC(node_viewer_large, "node_viewer_large", other_node_viewer_large);
LINK_ENTITY_TO_FUNC(path_corner, "path_corner", other_path_corner);
LINK_ENTITY_TO_FUNC(path_track, "path_track", other_path_track);
LINK_ENTITY_TO_FUNC(player, "player", other_player);
LINK_ENTITY_TO_FUNC(player_loadsaved, "player_loadsaved", other_player_loadsaved);
LINK_ENTITY_TO_FUNC(player_weaponstrip, "player_weaponstrip", other_player_weaponstrip);
LINK_ENTITY_TO_FUNC(rpg_rocket, "rpg_rocket", other_rpg_rocket);
LINK_ENTITY_TO_FUNC(scripted_sentence, "scripted_sentence", other_scripted_sentence);
LINK_ENTITY_TO_FUNC(scripted_sequence, "scripted_sequence", other_scripted_sequence);
LINK_ENTITY_TO_FUNC(soundent, "soundent", other_soundent);
LINK_ENTITY_TO_FUNC(spark_shower, "spark_shower", other_spark_shower);
LINK_ENTITY_TO_FUNC(speaker, "speaker", other_speaker);
LINK_ENTITY_TO_FUNC(squidspit, "squidspit", other_squidspit);
LINK_ENTITY_TO_FUNC(streak_spiral, "streak_spiral", other_streak_spiral);
LINK_ENTITY_TO_FUNC(target_cdaudio, "target_cdaudio", other_target_cdaudio);
LINK_ENTITY_TO_FUNC(testhull, "testhull", other_testhull);
LINK_ENTITY_TO_FUNC(test_effect, "test_effect", other_test_effect);
LINK_ENTITY_TO_FUNC(trigger, "trigger", other_trigger);
LINK_ENTITY_TO_FUNC(trigger_auto, "trigger_auto", other_trigger_auto);
LINK_ENTITY_TO_FUNC(trigger_autosave, "trigger_autosave", other_trigger_autosave);
LINK_ENTITY_TO_FUNC(trigger_camera, "trigger_camera", other_trigger_camera);
LINK_ENTITY_TO_FUNC(trigger_cdaudio, "trigger_cdaudio", other_trigger_cdaudio);
LINK_ENTITY_TO_FUNC(trigger_changelevel, "trigger_changelevel", other_trigger_changelevel);
LINK_ENTITY_TO_FUNC(trigger_changetarget, "trigger_changetarget", other_trigger_changetarget);
LINK_ENTITY_TO_FUNC(trigger_counter, "trigger_counter", other_trigger_counter);
LINK_ENTITY_TO_FUNC(trigger_endsection, "trigger_endsection", other_trigger_endsection);
LINK_ENTITY_TO_FUNC(trigger_gravity, "trigger_gravity", other_trigger_gravity);
LINK_ENTITY_TO_FUNC(trigger_hurt, "trigger_hurt", other_trigger_hurt);
LINK_ENTITY_TO_FUNC(trigger_monsterjump, "trigger_monsterjump", other_trigger_monsterjump);
LINK_ENTITY_TO_FUNC(trigger_multiple, "trigger_multiple", other_trigger_multiple);
LINK_ENTITY_TO_FUNC(trigger_once, "trigger_once", other_trigger_once);
LINK_ENTITY_TO_FUNC(trigger_push, "trigger_push", other_trigger_push);
LINK_ENTITY_TO_FUNC(trigger_relay, "trigger_relay", other_trigger_relay);
LINK_ENTITY_TO_FUNC(trigger_teleport, "trigger_teleport", other_trigger_teleport);
LINK_ENTITY_TO_FUNC(trigger_transition, "trigger_transition", other_trigger_transition);
//LINK_ENTITY_TO_FUNC(weaponbox, "weaponbox", other_weaponbox);
LINK_ENTITY_TO_FUNC(weapon_357, "weapon_357", other_weapon_357);
LINK_ENTITY_TO_FUNC(weapon_9mmAR, "weapon_9mmAR", other_weapon_9mmAR);
LINK_ENTITY_TO_FUNC(weapon_9mmhandgun, "weapon_9mmhandgun", other_weapon_9mmhandgun);
LINK_ENTITY_TO_FUNC(weapon_crossbow, "weapon_crossbow", other_weapon_crossbow);
LINK_ENTITY_TO_FUNC(weapon_crowbar, "weapon_crowbar", other_weapon_crowbar);
LINK_ENTITY_TO_FUNC(weapon_egon, "weapon_egon", other_weapon_egon);
LINK_ENTITY_TO_FUNC(weapon_gauss, "weapon_gauss", other_weapon_gauss);
LINK_ENTITY_TO_FUNC(weapon_glock, "weapon_glock", other_weapon_glock);
LINK_ENTITY_TO_FUNC(weapon_handgrenade, "weapon_handgrenade", other_weapon_handgrenade);
LINK_ENTITY_TO_FUNC(weapon_hornetgun, "weapon_hornetgun", other_weapon_hornetgun);
LINK_ENTITY_TO_FUNC(weapon_mp5, "weapon_mp5", other_weapon_mp5);
LINK_ENTITY_TO_FUNC(weapon_python, "weapon_python", other_weapon_python);
LINK_ENTITY_TO_FUNC(weapon_rpg, "weapon_rpg", other_weapon_rpg);
LINK_ENTITY_TO_FUNC(weapon_satchel, "weapon_satchel", other_weapon_satchel);
LINK_ENTITY_TO_FUNC(weapon_shotgun, "weapon_shotgun", other_weapon_shotgun);
LINK_ENTITY_TO_FUNC(weapon_snark, "weapon_snark", other_weapon_snark);
LINK_ENTITY_TO_FUNC(weapon_tripmine, "weapon_tripmine", other_weapon_tripmine);
LINK_ENTITY_TO_FUNC(worldspawn, "worldspawn", other_worldspawn);
LINK_ENTITY_TO_FUNC(world_items, "world_items", other_world_items);
LINK_ENTITY_TO_FUNC(xen_hair, "xen_hair", other_xen_hair);
LINK_ENTITY_TO_FUNC(xen_hull, "xen_hull", other_xen_hull);
LINK_ENTITY_TO_FUNC(xen_plantlight, "xen_plantlight", other_xen_plantlight);
LINK_ENTITY_TO_FUNC(xen_spore_large, "xen_spore_large", other_xen_spore_large);
LINK_ENTITY_TO_FUNC(xen_spore_medium, "xen_spore_medium", other_xen_spore_medium);
LINK_ENTITY_TO_FUNC(xen_spore_small, "xen_spore_small", other_xen_spore_small);
LINK_ENTITY_TO_FUNC(xen_tree, "xen_tree", other_xen_tree);
LINK_ENTITY_TO_FUNC(xen_ttrigger, "xen_ttrigger", other_xen_ttrigger);

// entities for botman's bots...
LINK_ENTITY_TO_FUNC( bot, "bot", other_bot );
LINK_ENTITY_TO_FUNC( entity_botcam, "entity_botcam", other_entity_botcam );

// entities for Phineas Bot...
// (NONE REQUIRED)

// entities for Action Half-Life (version 08/23/99k)
LINK_ENTITY_TO_FUNC(ammo_hkmp5, "ammo_hkmp5", other_ammo_hkmp5);
LINK_ENTITY_TO_FUNC(ammo_pistol, "ammo_pistol", other_ammo_pistol);
LINK_ENTITY_TO_FUNC(ammo_shells, "ammo_shells", other_ammo_shells);
LINK_ENTITY_TO_FUNC(ammo_sniper, "ammo_sniper", other_ammo_sniper);
LINK_ENTITY_TO_FUNC(item_bandolier, "item_bandolier", other_item_bandolier);
LINK_ENTITY_TO_FUNC(item_flashlight, "item_flashlight", other_item_flashlight);
LINK_ENTITY_TO_FUNC(item_laser, "item_laser", other_item_laser);
LINK_ENTITY_TO_FUNC(item_silencer, "item_silencer", other_item_silencer);
LINK_ENTITY_TO_FUNC(item_stealth, "item_stealth", other_item_stealth);
LINK_ENTITY_TO_FUNC(item_vest, "item_vest", other_item_vest);
LINK_ENTITY_TO_FUNC(sams_shower, "sams_shower", other_sams_shower);
LINK_ENTITY_TO_FUNC(weapon_akimbob, "weapon_akimbob", other_weapon_akimbob);
LINK_ENTITY_TO_FUNC(weapon_anaconda, "weapon_anaconda", other_weapon_anaconda);
LINK_ENTITY_TO_FUNC(weapon_beretta, "weapon_beretta", other_weapon_beretta);
//LINK_ENTITY_TO_FUNC(weapon_frag, "weapon_frag", other_weapon_frag);
LINK_ENTITY_TO_FUNC(weapon_handcannon, "weapon_handcannon", other_weapon_handcannon);
LINK_ENTITY_TO_FUNC(weapon_hkmp5, "weapon_hkmp5", other_weapon_hkmp5);
LINK_ENTITY_TO_FUNC(weapon_ithaca, "weapon_ithaca", other_weapon_ithaca);
//LINK_ENTITY_TO_FUNC(weapon_knife, "weapon_knife", other_weapon_knife);
LINK_ENTITY_TO_FUNC(weapon_sniper, "weapon_sniper", other_weapon_sniper);


// entities for Counter-Strike (Beta 6.2a)
LINK_ENTITY_TO_FUNC(armoury_entity,"armoury_entity",other_armoury_entity);
LINK_ENTITY_TO_FUNC(ammo_338magnum, "ammo_338magnum", other_ammo_338magnum);
LINK_ENTITY_TO_FUNC(ammo_357sig, "ammo_357sig", other_ammo_357sig);
LINK_ENTITY_TO_FUNC(ammo_45acp, "ammo_45acp", other_ammo_45acp);
LINK_ENTITY_TO_FUNC(ammo_50ae, "ammo_50ae", other_ammo_50ae);
LINK_ENTITY_TO_FUNC(ammo_556nato, "ammo_556nato", other_ammo_556nato);
LINK_ENTITY_TO_FUNC(ammo_57mm, "ammo_57mm", other_ammo_57mm);
LINK_ENTITY_TO_FUNC(ammo_762nato, "ammo_762nato", other_ammo_762nato);
LINK_ENTITY_TO_FUNC(ammo_9mm, "ammo_9mm", other_ammo_9mm);
LINK_ENTITY_TO_FUNC(ammo_556natobox, "ammo_556natobox", other_ammo_556natobox);
//LINK_ENTITY_TO_FUNC(ammo_flashbang, "ammo_flashbang", other_ammo_flashbang);
LINK_ENTITY_TO_FUNC(debris, "debris", other_debris);
LINK_ENTITY_TO_FUNC(env_bombglow, "env_bombglow", other_env_bombglow);
LINK_ENTITY_TO_FUNC(func_bomb_target, "func_bomb_target", other_func_bomb_target);
LINK_ENTITY_TO_FUNC(func_buyzone, "func_buyzone", other_func_buyzone);
LINK_ENTITY_TO_FUNC(func_escapezone,"func_escapezone",other_func_escapezone); 
LINK_ENTITY_TO_FUNC(func_hostage_rescue, "func_hostage_rescue", other_func_hostage_rescue);
LINK_ENTITY_TO_FUNC(func_vip_safetyzone,"func_vip_safetyzone",other_func_vip_safetyzone);    
LINK_ENTITY_TO_FUNC(hostage_entity, "hostage_entity", other_hostage_entity);
LINK_ENTITY_TO_FUNC(info_bomb_target, "info_bomb_target", other_info_bomb_target);
LINK_ENTITY_TO_FUNC(info_hostage_rescue, "info_hostage_rescue", other_info_hostage_rescue);
LINK_ENTITY_TO_FUNC(info_vip_start,"info_vip_start",other_info_vip_start); 

LINK_ENTITY_TO_FUNC(item_assaultsuit, "item_assaultsuit", other_item_assaultsuit);
LINK_ENTITY_TO_FUNC(item_kevlar, "item_kevlar", other_item_kevlar);
LINK_ENTITY_TO_FUNC(item_thighpack, "item_thighpack", other_item_thighpack);
//LINK_ENTITY_TO_FUNC(weapon_ak47, "weapon_ak47", other_weapon_ak47);
LINK_ENTITY_TO_FUNC(weapon_awp, "weapon_awp", other_weapon_awp);
//LINK_ENTITY_TO_FUNC(weapon_aug,"weapon_aug",other_weapon_aug);   
LINK_ENTITY_TO_FUNC(weapon_c4, "weapon_c4", other_weapon_c4);
LINK_ENTITY_TO_FUNC(weapon_deagle, "weapon_deagle", other_weapon_deagle);
//LINK_ENTITY_TO_FUNC(weapon_flashbang, "weapon_flashbang", other_weapon_flashbang);
LINK_ENTITY_TO_FUNC(weapon_g3sg1, "weapon_g3sg1", other_weapon_g3sg1);
LINK_ENTITY_TO_FUNC(weapon_glock18, "weapon_glock18", other_weapon_glock18);
LINK_ENTITY_TO_FUNC(weapon_hegrenade, "weapon_hegrenade", other_weapon_hegrenade);
//LINK_ENTITY_TO_FUNC(weapon_knife, "weapon_knife", other_weapon_knife);
LINK_ENTITY_TO_FUNC(weapon_m249, "weapon_m249", other_weapon_m249);
LINK_ENTITY_TO_FUNC(weapon_m3, "weapon_m3", other_weapon_m3);
LINK_ENTITY_TO_FUNC(weapon_m4a1, "weapon_m4a1", other_weapon_m4a1);
LINK_ENTITY_TO_FUNC(weapon_mac10,"weapon_mac10",other_weapon_mac10);
LINK_ENTITY_TO_FUNC(weapon_mp5navy, "weapon_mp5navy", other_weapon_mp5navy);
LINK_ENTITY_TO_FUNC(weapon_p228, "weapon_p228", other_weapon_p228);
LINK_ENTITY_TO_FUNC(weapon_p90, "weapon_p90", other_weapon_p90);
LINK_ENTITY_TO_FUNC(weapon_scout, "weapon_scout", other_weapon_scout);
LINK_ENTITY_TO_FUNC(weapon_sg552, "weapon_sg552", other_weapon_sg552);
LINK_ENTITY_TO_FUNC(weapon_shield, "weapon_shield", other_weapon_shield);
LINK_ENTITY_TO_FUNC(weapon_tmp, "weapon_tmp", other_weapon_tmp);
LINK_ENTITY_TO_FUNC(weapon_usp, "weapon_usp", other_weapon_usp);
LINK_ENTITY_TO_FUNC(weapon_xm1014, "weapon_xm1014", other_weapon_xm1014);

// entities for Team Fortress Classic (version 1)
LINK_ENTITY_TO_FUNC(building_dispenser, "building_dispenser", other_building_dispenser);
LINK_ENTITY_TO_FUNC(building_sentrygun, "building_sentrygun", other_building_sentrygun);
LINK_ENTITY_TO_FUNC(building_sentrygun_base, "building_sentrygun_base", other_building_sentrygun_base);
LINK_ENTITY_TO_FUNC(detpack, "detpack", other_detpack);
LINK_ENTITY_TO_FUNC(func_nobuild, "func_nobuild", other_func_nobuild);
LINK_ENTITY_TO_FUNC(ghost, "ghost", other_ghost);
LINK_ENTITY_TO_FUNC(info_player_teamspawn, "info_player_teamspawn", other_info_player_teamspawn);
LINK_ENTITY_TO_FUNC(info_tfdetect, "info_tfdetect", other_info_tfdetect);
LINK_ENTITY_TO_FUNC(info_tfgoal, "info_tfgoal", other_info_tfgoal);
LINK_ENTITY_TO_FUNC(info_tfgoal_timer, "info_tfgoal_timer", other_info_tfgoal_timer);
LINK_ENTITY_TO_FUNC(item_armor1, "item_armor1", other_item_armor1);
LINK_ENTITY_TO_FUNC(item_armor2, "item_armor2", other_item_armor2);
LINK_ENTITY_TO_FUNC(item_armor3, "item_armor3", other_item_armor3);
LINK_ENTITY_TO_FUNC(item_artifact_envirosuit, "item_artifact_envirosuit", other_item_artifact_envirosuit);
LINK_ENTITY_TO_FUNC(item_artifact_invisibility, "item_artifact_invisibility", other_item_artifact_invisibility);
LINK_ENTITY_TO_FUNC(item_artifact_invulnerability, "item_artifact_invulnerability", other_item_artifact_invulnerability);
LINK_ENTITY_TO_FUNC(item_artifact_super_damage, "item_artifact_super_damage", other_item_artifact_super_damage);
LINK_ENTITY_TO_FUNC(item_cells, "item_cells", other_item_cells);
LINK_ENTITY_TO_FUNC(item_health, "item_health", other_item_health);
LINK_ENTITY_TO_FUNC(item_rockets, "item_rockets", other_item_rockets);
LINK_ENTITY_TO_FUNC(item_shells, "item_shells", other_item_shells);
LINK_ENTITY_TO_FUNC(item_spikes, "item_spikes", other_item_spikes);
LINK_ENTITY_TO_FUNC(item_tfgoal, "item_tfgoal", other_item_tfgoal);
LINK_ENTITY_TO_FUNC(i_p_t, "i_p_t", other_i_p_t);
LINK_ENTITY_TO_FUNC(i_t_g, "i_t_g", other_i_t_g);
LINK_ENTITY_TO_FUNC(i_t_t, "i_t_t", other_i_t_t);
LINK_ENTITY_TO_FUNC(teledeath, "teledeath", other_teledeath);
LINK_ENTITY_TO_FUNC(tf_ammo_rpgclip, "tf_ammo_rpgclip", other_tf_ammo_rpgclip);
LINK_ENTITY_TO_FUNC(tf_flame, "tf_flame", other_tf_flame);
LINK_ENTITY_TO_FUNC(tf_flamethrower_burst, "tf_flamethrower_burst", other_tf_flamethrower_burst);
LINK_ENTITY_TO_FUNC(tf_gl_grenade, "tf_gl_grenade", other_tf_gl_grenade);
LINK_ENTITY_TO_FUNC(tf_ic_rocket, "tf_ic_rocket", other_tf_ic_rocket);
LINK_ENTITY_TO_FUNC(tf_nailgun_nail, "tf_nailgun_nail", other_tf_nailgun_nail);
LINK_ENTITY_TO_FUNC(tf_rpg_rocket, "tf_rpg_rocket", other_tf_rpg_rocket);
LINK_ENTITY_TO_FUNC(tf_weapon_ac, "tf_weapon_ac", other_tf_weapon_ac);
LINK_ENTITY_TO_FUNC(tf_weapon_autorifle, "tf_weapon_autorifle", other_tf_weapon_autorifle);
LINK_ENTITY_TO_FUNC(tf_weapon_axe, "tf_weapon_axe", other_tf_weapon_axe);
LINK_ENTITY_TO_FUNC(tf_weapon_caltrop, "tf_weapon_caltrop", other_tf_weapon_caltrop);
LINK_ENTITY_TO_FUNC(tf_weapon_caltropgrenade, "tf_weapon_caltropgrenade", other_tf_weapon_caltropgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_concussiongrenade, "tf_weapon_concussiongrenade", other_tf_weapon_concussiongrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_empgrenade, "tf_weapon_empgrenade", other_tf_weapon_empgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_flamethrower, "tf_weapon_flamethrower", other_tf_weapon_flamethrower);
LINK_ENTITY_TO_FUNC(tf_weapon_gasgrenade, "tf_weapon_gasgrenade", other_tf_weapon_gasgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_genericprimedgrenade, "tf_weapon_genericprimedgrenade", other_tf_weapon_genericprimedgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_gl, "tf_weapon_gl", other_tf_weapon_gl);
LINK_ENTITY_TO_FUNC(tf_weapon_ic, "tf_weapon_ic", other_tf_weapon_ic);
LINK_ENTITY_TO_FUNC(tf_weapon_knife, "tf_weapon_knife", other_tf_weapon_knife);
LINK_ENTITY_TO_FUNC(tf_weapon_medikit, "tf_weapon_medikit", other_tf_weapon_medikit);
LINK_ENTITY_TO_FUNC(tf_weapon_mirvbomblet, "tf_weapon_mirvbomblet", other_tf_weapon_mirvbomblet);
LINK_ENTITY_TO_FUNC(tf_weapon_mirvgrenade, "tf_weapon_mirvgrenade", other_tf_weapon_mirvgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_nailgrenade, "tf_weapon_nailgrenade", other_tf_weapon_nailgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_napalmgrenade, "tf_weapon_napalmgrenade", other_tf_weapon_napalmgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_ng, "tf_weapon_ng", other_tf_weapon_ng);
LINK_ENTITY_TO_FUNC(tf_weapon_normalgrenade, "tf_weapon_normalgrenade", other_tf_weapon_normalgrenade);
LINK_ENTITY_TO_FUNC(tf_weapon_pl, "tf_weapon_pl", other_tf_weapon_pl);
LINK_ENTITY_TO_FUNC(tf_weapon_railgun, "tf_weapon_railgun", other_tf_weapon_railgun);
LINK_ENTITY_TO_FUNC(tf_weapon_rpg, "tf_weapon_rpg", other_tf_weapon_rpg);
LINK_ENTITY_TO_FUNC(tf_weapon_shotgun, "tf_weapon_shotgun", other_tf_weapon_shotgun);
LINK_ENTITY_TO_FUNC(tf_weapon_sniperrifle, "tf_weapon_sniperrifle", other_tf_weapon_sniperrifle);
LINK_ENTITY_TO_FUNC(tf_weapon_spanner, "tf_weapon_spanner", other_tf_weapon_spanner);
LINK_ENTITY_TO_FUNC(tf_weapon_superng, "tf_weapon_superng", other_tf_weapon_superng);
LINK_ENTITY_TO_FUNC(tf_weapon_supershotgun, "tf_weapon_supershotgun", other_tf_weapon_supershotgun);
LINK_ENTITY_TO_FUNC(tf_weapon_tranq, "tf_weapon_tranq", other_tf_weapon_tranq);
LINK_ENTITY_TO_FUNC(timer, "timer", other_timer);




// entities for firearms rc1 (yes!)

LINK_ENTITY_TO_FUNC(ammo_anaconda,"ammo_anaconda",other_ammo_anaconda);

LINK_ENTITY_TO_FUNC(ammo_benelli,"ammo_benelli",other_ammo_benelli);
LINK_ENTITY_TO_FUNC(ammo_ber92f,"ammo_ber92f",other_ammo_ber92f);
LINK_ENTITY_TO_FUNC(ammo_ber93r,"ammo_ber93r",other_ammo_ber93r);
LINK_ENTITY_TO_FUNC(ammobox,"ammobox",other_ammobox);
LINK_ENTITY_TO_FUNC(ammo_coltgov,"ammo_coltgov",other_ammo_coltgov);
LINK_ENTITY_TO_FUNC(ammo_desert,"ammo_desert",other_ammo_desert);
LINK_ENTITY_TO_FUNC(ammo_famas,"ammo_famas",other_ammo_famas);

LINK_ENTITY_TO_FUNC(ammo_frag,"ammo_frag",other_ammo_frag);
LINK_ENTITY_TO_FUNC(ammo_g36e,"ammo_g36e",other_ammo_g36e);
LINK_ENTITY_TO_FUNC(ammo_g3a3,"ammo_g3a3",other_ammo_g3a3);
LINK_ENTITY_TO_FUNC(ammo_m11,"ammo_m11",other_ammo_m11);
LINK_ENTITY_TO_FUNC(ammo_m112,"ammo_m112",other_ammo_m112);
LINK_ENTITY_TO_FUNC(ammo_m16,"ammo_m16",other_ammo_m16);
LINK_ENTITY_TO_FUNC(ammo_m203,"ammo_m203",other_ammo_m203);
LINK_ENTITY_TO_FUNC(ammo_m4,"ammo_m4",other_ammo_m4);
LINK_ENTITY_TO_FUNC(ammo_m60,"ammo_m60",other_ammo_m60);
LINK_ENTITY_TO_FUNC(ammo_m79,"ammo_m79",other_ammo_m79);
LINK_ENTITY_TO_FUNC(ammo_m82,"ammo_m82",other_ammo_m82);
LINK_ENTITY_TO_FUNC(ammo_mp5a5,"ammo_mp5a5",other_ammo_mp5a5);     
LINK_ENTITY_TO_FUNC(ammo_mp5k,"ammo_mp5k",other_ammo_mp5k);
LINK_ENTITY_TO_FUNC(ammo_psg1,"ammo_psg1",other_ammo_psg1);
LINK_ENTITY_TO_FUNC(ammo_saiga,"ammo_saiga",other_ammo_saiga);
LINK_ENTITY_TO_FUNC(ammo_stg24,"ammo_stg24",other_ammo_stg24);
LINK_ENTITY_TO_FUNC(bullet,"bullet",other_bullet);
LINK_ENTITY_TO_FUNC(fa_drop_zone,"fa_drop_zone",other_fa_drop_zone);
LINK_ENTITY_TO_FUNC(fa_parachute,"fa_parachute",other_fa_parachute);
LINK_ENTITY_TO_FUNC(fa_push_flag,"fa_push_flag",other_fa_push_flag);
LINK_ENTITY_TO_FUNC(fa_push_point,"fa_push_point",other_fa_push_point);
LINK_ENTITY_TO_FUNC(fa_team_goal,"fa_team_goal",other_fa_team_goal);
LINK_ENTITY_TO_FUNC(fa_team_item,"fa_team_item",other_fa_team_item);
LINK_ENTITY_TO_FUNC(info_firearms_detect,"info_firearms_detect",other_info_firearms_detect);  
LINK_ENTITY_TO_FUNC(info_playerstart_blue,"info_playerstart_blue",other_info_playerstart_blue);
LINK_ENTITY_TO_FUNC(info_playerstart_red,"info_playerstart_red",other_info_playerstart_red);
LINK_ENTITY_TO_FUNC(item_bandage,"item_bandage",other_item_bandage);
LINK_ENTITY_TO_FUNC(item_claymore,"item_claymore",other_item_claymore);
LINK_ENTITY_TO_FUNC(item_flashbang,"item_flashbang",other_item_flashbang);
LINK_ENTITY_TO_FUNC(item_frag,"item_frag",other_item_frag);
LINK_ENTITY_TO_FUNC(item_stg24,"item_stg24",other_item_stg24);

LINK_ENTITY_TO_FUNC(weapon_benelli,"weapon_benelli",other_weapon_benelli);
LINK_ENTITY_TO_FUNC(weapon_ber92f,"weapon_ber92f",other_weapon_ber92f);
LINK_ENTITY_TO_FUNC(weapon_ber93r,"weapon_ber93r",other_weapon_ber93r);
LINK_ENTITY_TO_FUNC(weapon_claymore,"weapon_claymore",other_weapon_claymore);
LINK_ENTITY_TO_FUNC(weapon_coltgov,"weapon_coltgov",other_weapon_coltgov);
LINK_ENTITY_TO_FUNC(weapon_desert,"weapon_desert",other_weapon_desert);
LINK_ENTITY_TO_FUNC(weapon_famas,"weapon_famas",other_weapon_famas);
LINK_ENTITY_TO_FUNC(weapon_g36e,"weapon_g36e",other_weapon_g36e);
LINK_ENTITY_TO_FUNC(weapon_g3a3,"weapon_g3a3",other_weapon_g3a3);
LINK_ENTITY_TO_FUNC(weapon_m11,"weapon_m11",other_weapon_m11);
LINK_ENTITY_TO_FUNC(weapon_m16,"weapon_m16",other_weapon_m16);
LINK_ENTITY_TO_FUNC(weapon_m4,"weapon_m4",other_weapon_m4);
LINK_ENTITY_TO_FUNC(weapon_m60,"weapon_m60",other_weapon_m60);
LINK_ENTITY_TO_FUNC(weapon_m79,"weapon_m79",other_weapon_m79);
LINK_ENTITY_TO_FUNC(weapon_m82,"weapon_m82",other_weapon_m82);
LINK_ENTITY_TO_FUNC(weapon_mp5a5,"weapon_mp5a5",other_weapon_mp5a5);    
LINK_ENTITY_TO_FUNC(weapon_mp5k,"weapon_mp5k",other_weapon_mp5k);
LINK_ENTITY_TO_FUNC(weapon_psg1,"weapon_psg1",other_weapon_psg1);
LINK_ENTITY_TO_FUNC(weapon_saiga,"weapon_saiga",other_weapon_saiga);
LINK_ENTITY_TO_FUNC(weapon_stg24,"weapon_stg24",other_weapon_stg24);
LINK_ENTITY_TO_FUNC(ammo_mc51,"ammo_mc51",other_ammo_mc51);
LINK_ENTITY_TO_FUNC(weapon_mc51,"weapon_mc51",other_weapon_mc51);    
 

// golden eye 1.1b support



LINK_ENTITY_TO_FUNC(ammo_AK47clip,"ammo_AK47clip",other_ammo_AK47clip);
LINK_ENTITY_TO_FUNC(ammo_colt45clip,"ammo_colt45clip",other_ammo_colt45clip);
LINK_ENTITY_TO_FUNC(ammo_d5kclip,"ammo_d5kclip",other_ammo_d5kclip);
LINK_ENTITY_TO_FUNC(ammo_deagleclip,"ammo_deagleclip",other_ammo_deagleclip);
LINK_ENTITY_TO_FUNC(ammo_p99clip,"ammo_p99clip",other_ammo_p99clip);
LINK_ENTITY_TO_FUNC(ammo_rcp90clip,"ammo_rcp90clip",other_ammo_rcp90clip);
LINK_ENTITY_TO_FUNC(ammo_veprclip,"ammo_veprclip",other_ammo_veprclip);
LINK_ENTITY_TO_FUNC(carried_book_team1,"carried_book_team1",other_carried_book_team1);
LINK_ENTITY_TO_FUNC(carried_book_team2,"carried_book_team2",other_carried_book_team2);
LINK_ENTITY_TO_FUNC(info_mapdetect,"info_mapdetect",other_info_mapdetect);
LINK_ENTITY_TO_FUNC(item_book_team1,"item_book_team1",other_item_book_team1);
LINK_ENTITY_TO_FUNC(item_book_team2,"item_book_team2",other_item_book_team2);
LINK_ENTITY_TO_FUNC(spawnfar,"spawnfar",other_spawnfar);
LINK_ENTITY_TO_FUNC(weapon_colt45,"weapon_colt45",other_weapon_colt45);
LINK_ENTITY_TO_FUNC(weapon_d5k,"weapon_d5k",other_weapon_d5k);
LINK_ENTITY_TO_FUNC(weapon_p99,"weapon_p99",other_weapon_p99);
LINK_ENTITY_TO_FUNC(weapon_proximity,"weapon_proximity",other_weapon_proximity);
LINK_ENTITY_TO_FUNC(weapon_rcp90,"weapon_rcp90",other_weapon_rcp90);
LINK_ENTITY_TO_FUNC(weapon_triplaser,"weapon_triplaser",other_weapon_triplaser);
LINK_ENTITY_TO_FUNC(weapon_vepr,"weapon_vepr",other_weapon_vepr);



// Oz support

LINK_ENTITY_TO_FUNC(ctf_blueflag,"ctf_blueflag",other_ctf_blueflag);
LINK_ENTITY_TO_FUNC(ctf_bluespawn,"ctf_bluespawn",other_ctf_bluespawn);
LINK_ENTITY_TO_FUNC(ctf_cloak,"ctf_cloak",other_ctf_cloak);
LINK_ENTITY_TO_FUNC(ctf_crowbar,"ctf_crowbar",other_ctf_crowbar);
LINK_ENTITY_TO_FUNC(ctf_cycle,"ctf_cycle",other_ctf_cycle);
LINK_ENTITY_TO_FUNC(ctf_frag,"ctf_frag",other_ctf_frag);
LINK_ENTITY_TO_FUNC(ctf_hook,"ctf_hook",other_ctf_hook);
LINK_ENTITY_TO_FUNC(ctf_lowgrav,"ctf_lowgrav",other_ctf_lowgrav);
LINK_ENTITY_TO_FUNC(ctf_protect,"ctf_protect",other_ctf_protect);
LINK_ENTITY_TO_FUNC(ctf_redflag,"ctf_redflag",other_ctf_redflag);
LINK_ENTITY_TO_FUNC(ctf_redspawn,"ctf_redspawn",other_ctf_redspawn);
LINK_ENTITY_TO_FUNC(ctf_regen,"ctf_regen",other_ctf_regen);
LINK_ENTITY_TO_FUNC(ctf_superglock,"ctf_superglock",other_ctf_superglock);
LINK_ENTITY_TO_FUNC(ctf_trap,"ctf_trap",other_ctf_trap);
LINK_ENTITY_TO_FUNC(ctf_vamp,"ctf_vamp",other_ctf_vamp);
LINK_ENTITY_TO_FUNC(grapple_bolt,"grapple_bolt",other_grapple_bolt);