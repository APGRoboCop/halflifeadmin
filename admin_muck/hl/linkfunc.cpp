/* $Id: linkfunc.cpp,v 1.1.1.1 2001/03/13 15:59:57 darope Exp $ */

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

// new valve stuff (for 3.1.0.4)
LINK_ENTITY_TO_FUNC(CreateInterface,"CreateInterface",other_CreateInterface);  
LINK_ENTITY_TO_FUNC(GetChaseOrigin,"GetChaseOrigin",other_GetChaseOrigin); 



// new tfc stuff
LINK_ENTITY_TO_FUNC(CTF_Map,"CTF_Map",other_CTF_Map);    

// new tfc 1.1.0.4 stuff
LINK_ENTITY_TO_FUNC(info_areadef,"info_areadef",other_info_areadef);
LINK_ENTITY_TO_FUNC(info_tf_teamcheck,"info_tf_teamcheck",other_info_tf_teamcheck);
LINK_ENTITY_TO_FUNC(info_tf_teamset,"info_tf_teamset",other_info_tf_teamset); 
LINK_ENTITY_TO_FUNC(func_nogrenades,"func_nogrenades",other_func_nogrenades);

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



// new cstrike 1.0 entities
LINK_ENTITY_TO_FUNC(func_grencatch,"func_grencatch",other_func_grencatch);
LINK_ENTITY_TO_FUNC(func_weaponcheck,"func_weaponcheck",other_func_weaponcheck);
LINK_ENTITY_TO_FUNC(weapon_fiveseven,"weapon_fiveseven",other_weapon_fiveseven);
LINK_ENTITY_TO_FUNC(weapon_sg550,"weapon_sg550",other_weapon_sg550);


// new cstrike b7 entities
LINK_ENTITY_TO_FUNC(func_vehicle,"func_vehicle",other_func_vehicle);
LINK_ENTITY_TO_FUNC(func_vehiclecontrols,"func_vehiclecontrols",other_func_vehiclecontrols);      
LINK_ENTITY_TO_FUNC(weapon_elite,"weapon_elite",other_weapon_elite);   


// new cstrike 6.5 entities
LINK_ENTITY_TO_FUNC(weapon_smokegrenade,"weapon_smokegrenade", other_weapon_smokegrenade);
LINK_ENTITY_TO_FUNC(info_map_parameters,"info_map_parameters",other_info_map_parameters);

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


// firearms 2.3 entities
LINK_ENTITY_TO_FUNC(weapon_bizon,"weapon_bizon",other_weapon_bizon);
LINK_ENTITY_TO_FUNC(ammo_bizon,"ammo_bizon",other_ammo_bizon);  

// firearms 2.2 new entities
LINK_ENTITY_TO_FUNC(weapon_launcher,"weapon_launcher",other_weapon_launcher);
LINK_ENTITY_TO_FUNC(ammo_launcher,"ammo_launcher",other_ammo_launcher);
LINK_ENTITY_TO_FUNC(ammo_40mm,"ammo_40mm",other_ammo_40mm);
LINK_ENTITY_TO_FUNC(ammo_762mmbox,"ammo_762mmbox",other_ammo_762mmbox);       
LINK_ENTITY_TO_FUNC(hospital,"hospital",other_hospital);
LINK_ENTITY_TO_FUNC(marker,"marker",other_marker);        
LINK_ENTITY_TO_FUNC(weapmortar,"weapmortar",other_weapmortar);

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
 
// goldeneye 1.7
LINK_ENTITY_TO_FUNC(ammo_cloaker,"ammo_cloaker",other_ammo_cloaker);
LINK_ENTITY_TO_FUNC(ammo_areaammo,"ammo_areaammo",other_ammo_areaammo); 

//goldeneye 1.6
LINK_ENTITY_TO_FUNC(monster_mine,"monster_mine",other_monster_mine);
LINK_ENTITY_TO_FUNC(item_armor,"item_armor",other_item_armor);
LINK_ENTITY_TO_FUNC(knife_bolt,"knife_bolt",other_knife_bolt);
LINK_ENTITY_TO_FUNC(ammo_knife,"ammo_knife",other_ammo_knife);
LINK_ENTITY_TO_FUNC(weapon_pmine,"weapon_pmine",other_weapon_pmine);
LINK_ENTITY_TO_FUNC(weapon_rmine,"weapon_rmine",other_weapon_rmine);
LINK_ENTITY_TO_FUNC(guard_info,"guard_info",other_guard_info);
LINK_ENTITY_TO_FUNC(info_goldeneye,"info_goldeneye",other_info_goldeneye);
LINK_ENTITY_TO_FUNC(weapon_tmine,"weapon_tmine",other_weapon_tmine);
LINK_ENTITY_TO_FUNC(random_weapon,"random_weapon",other_random_weapon);
LINK_ENTITY_TO_FUNC(random_ammo,"random_ammo",other_random_ammo);
LINK_ENTITY_TO_FUNC(spy_escape,"spy_escape",other_spy_escape);    

// goldeneye 1.5
LINK_ENTITY_TO_FUNC(monster_proximity,"monster_proximity",other_monster_proximity);
LINK_ENTITY_TO_FUNC(weapon_kf7,"weapon_kf7",other_weapon_kf7);
LINK_ENTITY_TO_FUNC(ammo_kf7clip,"ammo_kf7clip",other_ammo_kf7clip);
LINK_ENTITY_TO_FUNC(weapon_dd44,"weapon_dd44",other_weapon_dd44);
LINK_ENTITY_TO_FUNC(ammo_dd44clip,"ammo_dd44clip",other_ammo_dd44clip);
LINK_ENTITY_TO_FUNC(weapon_goldpp7,"weapon_goldpp7",other_weapon_goldpp7);
LINK_ENTITY_TO_FUNC(ammo_goldpp7clip,"ammo_goldpp7clip",other_ammo_goldpp7clip); 
LINK_ENTITY_TO_FUNC(weapon_phantom,"weapon_phantom",other_weapon_phantom);
LINK_ENTITY_TO_FUNC(ammo_phantomclip,"ammo_phantomclip",other_ammo_phantomclip);
LINK_ENTITY_TO_FUNC(weapon_silverpp7,"weapon_silverpp7",other_weapon_silverpp7);
LINK_ENTITY_TO_FUNC(ammo_silverpp7clip,"ammo_silverpp7clip",other_ammo_silverpp7clip);
LINK_ENTITY_TO_FUNC(weapon_slappers,"weapon_slappers",other_weapon_slappers);
LINK_ENTITY_TO_FUNC(weapon_zmg,"weapon_zmg",other_weapon_zmg);
LINK_ENTITY_TO_FUNC(ammo_zmgclip,"ammo_zmgclip",other_ammo_zmgclip);



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


// more frontline entities (from 1.0) (yes, my bad with this)
LINK_ENTITY_TO_FUNC(trip_beam,"trip_beam",other_trip_beam);
LINK_ENTITY_TO_FUNC(weapon_mp5a2,"weapon_mp5a2",other_weapon_mp5a2);
LINK_ENTITY_TO_FUNC(weapon_mp5sd,"weapon_mp5sd",other_weapon_mp5sd);
LINK_ENTITY_TO_FUNC(info_player_attacker,"info_player_attacker",other_info_player_attacker);
LINK_ENTITY_TO_FUNC(info_player_defender,"info_player_defender",other_info_player_defender);
LINK_ENTITY_TO_FUNC(info_player_observer,"info_player_observer",other_info_player_observer); 



// frontline entities
LINK_ENTITY_TO_FUNC(ammo_beretta,"ammo_beretta",other_ammo_beretta);
LINK_ENTITY_TO_FUNC(capture_point,"capture_point",other_capture_point);
LINK_ENTITY_TO_FUNC(ammo_mp5a2,"ammo_mp5a2",other_ammo_mp5a2);
LINK_ENTITY_TO_FUNC(ammo_mp5sd,"ammo_mp5sd",other_ammo_mp5sd);
LINK_ENTITY_TO_FUNC(gren_smoke,"gren_smoke",other_gren_smoke);
LINK_ENTITY_TO_FUNC(weapon_spas12,"weapon_spas12",other_weapon_spas12);
LINK_ENTITY_TO_FUNC(ammo_spas12,"ammo_spas12",other_ammo_spas12);
LINK_ENTITY_TO_FUNC(my_monster,"my_monster",other_my_monster);
LINK_ENTITY_TO_FUNC(weapon_ump45,"weapon_ump45",other_weapon_ump45);
LINK_ENTITY_TO_FUNC(ammo_ump45,"ammo_ump45",other_ammo_ump45);
LINK_ENTITY_TO_FUNC(info_frontline,"info_frontline",other_info_frontline);
LINK_ENTITY_TO_FUNC(weapon_clip_generic,"weapon_clip_generic",other_weapon_clip_generic);
// frontline 0.46 new ents
LINK_ENTITY_TO_FUNC(weapon_hk21,"weapon_hk21",other_weapon_hk21);
LINK_ENTITY_TO_FUNC(ammo_hk21,"ammo_hk21",other_ammo_hk21);
LINK_ENTITY_TO_FUNC(weapon_mk23,"weapon_mk23",other_weapon_mk23);
LINK_ENTITY_TO_FUNC(ammo_mk23,"ammo_mk23",other_ammo_mk23);
LINK_ENTITY_TO_FUNC(weapon_msg90,"weapon_msg90",other_weapon_msg90);
LINK_ENTITY_TO_FUNC(ammo_msg90,"ammo_msg90",other_ammo_msg90);        

// new 0.49 ents
LINK_ENTITY_TO_FUNC(weapon_ak5,"weapon_ak5",other_weapon_ak5);
LINK_ENTITY_TO_FUNC(ammo_ak5,"ammo_ak5",other_ammo_ak5);       



// s&i entities
LINK_ENTITY_TO_FUNC(ammo_mindrayclip,"ammo_mindrayclip",other_ammo_mindrayclip);
LINK_ENTITY_TO_FUNC(ammo_vomitclip,"ammo_vomitclip",other_ammo_vomitclip); 
LINK_ENTITY_TO_FUNC(info_administrator,"info_administrator",other_info_administrator);
LINK_ENTITY_TO_FUNC(info_scientist,"info_scientist",other_info_scientist);
LINK_ENTITY_TO_FUNC(info_scientist_dead,"info_scientist_dead",other_info_scientist_dead);
LINK_ENTITY_TO_FUNC(info_scientist_start,"info_scientist_start",other_info_scientist_start);
LINK_ENTITY_TO_FUNC(info_transistor,"info_transistor",other_info_transistor);  
LINK_ENTITY_TO_FUNC(weapon_mindray,"weapon_mindray",other_weapon_mindray);
LINK_ENTITY_TO_FUNC(weapon_radio,"weapon_radio",other_weapon_radio);
LINK_ENTITY_TO_FUNC(weapon_transistor,"weapon_transistor",other_weapon_transistor);
LINK_ENTITY_TO_FUNC(weapon_vomit,"weapon_vomit",other_weapon_vomit);  



// gansta wars 2.6 
LINK_ENTITY_TO_FUNC(bot_waypoint,"bot_waypoint",other_bot_waypoint);
LINK_ENTITY_TO_FUNC(weapon_glock17,"weapon_glock17",other_weapon_glock17);
LINK_ENTITY_TO_FUNC(weapon_smkgrenade,"weapon_smkgrenade",other_weapon_smkgrenade); 
LINK_ENTITY_TO_FUNC(weapon_baretta,"weapon_baretta",other_weapon_baretta);
LINK_ENTITY_TO_FUNC(ammo_baretta,"ammo_baretta",other_ammo_baretta);
LINK_ENTITY_TO_FUNC(weapon_ak101,"weapon_ak101",other_weapon_ak101);
LINK_ENTITY_TO_FUNC(ammo_ak101,"ammo_ak101",other_ammo_ak101);
LINK_ENTITY_TO_FUNC(weapon_akimbo_glocks,"weapon_akimbo_glocks",other_weapon_akimbo_glocks);
LINK_ENTITY_TO_FUNC(weapon_bat,"weapon_bat",other_weapon_bat);
LINK_ENTITY_TO_FUNC(weapon_baton,"weapon_baton",other_weapon_baton);
LINK_ENTITY_TO_FUNC(item_briefcase,"item_briefcase",other_item_briefcase);
LINK_ENTITY_TO_FUNC(item_cash,"item_cash",other_item_cash);
LINK_ENTITY_TO_FUNC(weapon_colt,"weapon_colt",other_weapon_colt);
LINK_ENTITY_TO_FUNC(ammo_colt,"ammo_colt",other_ammo_colt);
LINK_ENTITY_TO_FUNC(weapon_dblshot,"weapon_dblshot",other_weapon_dblshot);
LINK_ENTITY_TO_FUNC(weapon_dbshot,"weapon_dbshot",other_weapon_dbshot);
LINK_ENTITY_TO_FUNC(ammo_dbshot,"ammo_dbshot",other_ammo_dbshot);
LINK_ENTITY_TO_FUNC(info_gwars_map,"info_gwars_map",other_info_gwars_map);
LINK_ENTITY_TO_FUNC(weapon_mg36,"weapon_mg36",other_weapon_mg36);
LINK_ENTITY_TO_FUNC(weapon_g36k,"weapon_g36k",other_weapon_g36k);
LINK_ENTITY_TO_FUNC(ammo_mg36,"ammo_mg36",other_ammo_mg36);
LINK_ENTITY_TO_FUNC(ammo_g36k,"ammo_g36k",other_ammo_g36k);
LINK_ENTITY_TO_FUNC(ammo_glock17,"ammo_glock17",other_ammo_glock17);
LINK_ENTITY_TO_FUNC(weapon_longslide,"weapon_longslide",other_weapon_longslide);
LINK_ENTITY_TO_FUNC(ammo_longslide,"ammo_longslide",other_ammo_longslide);
LINK_ENTITY_TO_FUNC(weapon_mac,"weapon_mac",other_weapon_mac);
LINK_ENTITY_TO_FUNC(ammo_mac,"ammo_mac",other_ammo_mac);
LINK_ENTITY_TO_FUNC(ammo_mac10,"ammo_mac10",other_ammo_mac10);
LINK_ENTITY_TO_FUNC(weapon_mossberg,"weapon_mossberg",other_weapon_mossberg);
LINK_ENTITY_TO_FUNC(ammo_mossberg,"ammo_mossberg",other_ammo_mossberg);
LINK_ENTITY_TO_FUNC(info_gangsta_dm_start,"info_gangsta_dm_start",other_info_gangsta_dm_start);
LINK_ENTITY_TO_FUNC(info_italian_start,"info_italian_start",other_info_italian_start);
LINK_ENTITY_TO_FUNC(info_russian_start,"info_russian_start",other_info_russian_start);
LINK_ENTITY_TO_FUNC(deal_italian_start,"deal_italian_start",other_deal_italian_start);
LINK_ENTITY_TO_FUNC(deal_columbian_start,"deal_columbian_start",other_deal_columbian_start);
LINK_ENTITY_TO_FUNC(deal_swat_start,"deal_swat_start",other_deal_swat_start);
LINK_ENTITY_TO_FUNC(weapon_tec9,"weapon_tec9",other_weapon_tec9);
LINK_ENTITY_TO_FUNC(ammo_tec9,"ammo_tec9",other_ammo_tec9);
LINK_ENTITY_TO_FUNC(item_flag1,"item_flag1",other_item_flag1);
LINK_ENTITY_TO_FUNC(item_flag2,"item_flag2",other_item_flag2);
LINK_ENTITY_TO_FUNC(weapon_tommygun,"weapon_tommygun",other_weapon_tommygun);
LINK_ENTITY_TO_FUNC(ammo_tommy,"ammo_tommy",other_ammo_tommy);
LINK_ENTITY_TO_FUNC(ammo_tommygun,"ammo_tommygun",other_ammo_tommygun);
LINK_ENTITY_TO_FUNC(turf_zone,"turf_zone",other_turf_zone);
LINK_ENTITY_TO_FUNC(weapon_uzi,"weapon_uzi",other_weapon_uzi);
LINK_ENTITY_TO_FUNC(ammo_uzi,"ammo_uzi",other_ammo_uzi);

// arg 1.0 entities
LINK_ENTITY_TO_FUNC(item_beans,"item_beans",other_item_beans);
LINK_ENTITY_TO_FUNC(BALLS_airball,"BALLS_airball",other_BALLS_airball);
LINK_ENTITY_TO_FUNC(weapon_discoballs,"weapon_discoballs",other_weapon_discoballs);
LINK_ENTITY_TO_FUNC(ammo_BALLS,"ammo_BALLS",other_ammo_BALLS);
LINK_ENTITY_TO_FUNC(weapon_ga2uss,"weapon_ga2uss",other_weapon_ga2uss);
LINK_ENTITY_TO_FUNC(weapon_holybsword,"weapon_holybsword",other_weapon_holybsword);
LINK_ENTITY_TO_FUNC(weapon_aandagger,"weapon_aandagger",other_weapon_aandagger);
LINK_ENTITY_TO_FUNC(weapon_musket,"weapon_musket",other_weapon_musket);
LINK_ENTITY_TO_FUNC(weapon_rumgun,"weapon_rumgun",other_weapon_rumgun);
LINK_ENTITY_TO_FUNC(monster_llama,"monster_llama",other_monster_llama);
LINK_ENTITY_TO_FUNC(weapon_tucan,"weapon_tucan",other_weapon_tucan); 


// arg 0.50 
LINK_ENTITY_TO_FUNC(weapon_cocogrenade,"weapon_cocogrenade",other_weapon_cocogrenade);
LINK_ENTITY_TO_FUNC(weapon_not_in_use_grenade,"weapon_not_in_use_grenade",other_weapon_not_in_use_grenade);
LINK_ENTITY_TO_FUNC(item_antidotee,"item_antidotee",other_item_antidotee);
LINK_ENTITY_TO_FUNC(hallucgrenade,"hallucgrenade",other_hallucgrenade); 
LINK_ENTITY_TO_FUNC(weapon_tdagger,"weapon_tdagger",other_weapon_tdagger);
LINK_ENTITY_TO_FUNC(weapon_longsword,"weapon_longsword",other_weapon_longsword);
LINK_ENTITY_TO_FUNC(weapon_stupid,"weapon_stupid",other_weapon_stupid);
LINK_ENTITY_TO_FUNC(FOOT_bolt,"FOOT_bolt",other_FOOT_bolt);
LINK_ENTITY_TO_FUNC(ammo_FOOT,"ammo_FOOT",other_ammo_FOOT);
LINK_ENTITY_TO_FUNC(weapon_flute,"weapon_flute",other_weapon_flute);
LINK_ENTITY_TO_FUNC(weapon_ndagger,"weapon_ndagger",other_weapon_ndagger);
LINK_ENTITY_TO_FUNC(FISHINGROD_bolt,"FISHINGROD_bolt",other_FISHINGROD_bolt);
LINK_ENTITY_TO_FUNC(weapon_fishingrod,"weapon_fishingrod",other_weapon_fishingrod);
LINK_ENTITY_TO_FUNC(ammo_FISHINGROD,"ammo_FISHINGROD",other_ammo_FISHINGROD);
LINK_ENTITY_TO_FUNC(weapon_bsword,"weapon_bsword",other_weapon_bsword);
LINK_ENTITY_TO_FUNC(whiskey_whisk,"whiskey_whisk",other_whiskey_whisk);
LINK_ENTITY_TO_FUNC(weapon_whiskey,"weapon_whiskey",other_weapon_whiskey);
LINK_ENTITY_TO_FUNC(ammo_whiskey,"ammo_whiskey",other_ammo_whiskey);
LINK_ENTITY_TO_FUNC(weapon_mop,"weapon_mop",other_weapon_mop);
 
// wizard wars beta 2
LINK_ENTITY_TO_FUNC(monster_balllightning,"monster_balllightning",other_monster_balllightning);
LINK_ENTITY_TO_FUNC(proj_lightningcloud,"proj_lightningcloud",other_proj_lightningcloud);
LINK_ENTITY_TO_FUNC(proj_lightningsatchel,"proj_lightningsatchel",other_proj_lightningsatchel);  
LINK_ENTITY_TO_FUNC(weapon_balllightningspell,"weapon_balllightningspell",other_weapon_balllightningspell);
LINK_ENTITY_TO_FUNC(weapon_lightningboltspell,"weapon_lightningboltspell",other_weapon_lightningboltspell);
LINK_ENTITY_TO_FUNC(weapon_lightningcloudspell,"weapon_lightningcloudspell",other_weapon_lightningcloudspell);   

LINK_ENTITY_TO_FUNC(ammo_satchels,"ammo_satchels",other_ammo_satchels);
LINK_ENTITY_TO_FUNC(ammo_spellbook,"ammo_spellbook",other_ammo_spellbook);
LINK_ENTITY_TO_FUNC(beanstalk,"beanstalk",other_beanstalk);
LINK_ENTITY_TO_FUNC(monster_giantplant,"monster_giantplant",other_monster_giantplant);
LINK_ENTITY_TO_FUNC(monster_skeleton,"monster_skeleton",other_monster_skeleton);
LINK_ENTITY_TO_FUNC(monster_spiritwiz,"monster_spiritwiz",other_monster_spiritwiz);
LINK_ENTITY_TO_FUNC(monster_thornbush,"monster_thornbush",other_monster_thornbush);
LINK_ENTITY_TO_FUNC(monster_tornado,"monster_tornado",other_monster_tornado);
LINK_ENTITY_TO_FUNC(monster_wizardclone,"monster_wizardclone",other_monster_wizardclone);
LINK_ENTITY_TO_FUNC(monster_wombat,"monster_wombat",other_monster_wombat);
LINK_ENTITY_TO_FUNC(proj_bird,"proj_bird",other_proj_bird);
LINK_ENTITY_TO_FUNC(proj_bursatchel,"proj_bursatchel",other_proj_bursatchel);
LINK_ENTITY_TO_FUNC(proj_cocklebur,"proj_cocklebur",other_proj_cocklebur);
LINK_ENTITY_TO_FUNC(proj_comet,"proj_comet",other_proj_comet);
LINK_ENTITY_TO_FUNC(proj_doublemagicmissle,"proj_doublemagicmissle",other_proj_doublemagicmissle);
LINK_ENTITY_TO_FUNC(proj_earthquakesatchel,"proj_earthquakesatchel",other_proj_earthquakesatchel);
LINK_ENTITY_TO_FUNC(proj_fireball,"proj_fireball",other_proj_fireball);
LINK_ENTITY_TO_FUNC(proj_firesatchel,"proj_firesatchel",other_proj_firesatchel);
LINK_ENTITY_TO_FUNC(proj_firespiral,"proj_firespiral",other_proj_firespiral);
LINK_ENTITY_TO_FUNC(proj_fissure,"proj_fissure",other_proj_fissure);
LINK_ENTITY_TO_FUNC(proj_flyingskull,"proj_flyingskull",other_proj_flyingskull);
LINK_ENTITY_TO_FUNC(proj_healhurtsatchel,"proj_healhurtsatchel",other_proj_healhurtsatchel);
LINK_ENTITY_TO_FUNC(proj_icepoke,"proj_icepoke",other_proj_icepoke);
LINK_ENTITY_TO_FUNC(proj_magicmissle,"proj_magicmissle",other_proj_magicmissle);
LINK_ENTITY_TO_FUNC(proj_meteor,"proj_meteor",other_proj_meteor);
LINK_ENTITY_TO_FUNC(proj_mindmissle,"proj_mindmissle",other_proj_mindmissle);
LINK_ENTITY_TO_FUNC(proj_poisonsatchel,"proj_poisonsatchel",other_proj_poisonsatchel);
LINK_ENTITY_TO_FUNC(proj_rollingstone,"proj_rollingstone",other_proj_rollingstone);
LINK_ENTITY_TO_FUNC(proj_suctionsatchel,"proj_suctionsatchel",other_proj_suctionsatchel);
LINK_ENTITY_TO_FUNC(proj_tcrystal,"proj_tcrystal",other_proj_tcrystal);
LINK_ENTITY_TO_FUNC(proj_throwingbone,"proj_throwingbone",other_proj_throwingbone);
LINK_ENTITY_TO_FUNC(proj_whirlwind,"proj_whirlwind",other_proj_whirlwind);
LINK_ENTITY_TO_FUNC(slAGruntTakeCoverFromEnemy,"slAGruntTakeCoverFromEnemy",other_slAGruntTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(slAGruntThreatDisplay,"slAGruntThreatDisplay",other_slAGruntThreatDisplay);
LINK_ENTITY_TO_FUNC(slAssassinTakeCoverFromBestSound,"slAssassinTakeCoverFromBestSound",other_slAssassinTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(slAssassinTakeCoverFromEnemy,"slAssassinTakeCoverFromEnemy",other_slAssassinTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(slAssassinTakeCoverFromEnemy2,"slAssassinTakeCoverFromEnemy2",other_slAssassinTakeCoverFromEnemy2);
LINK_ENTITY_TO_FUNC(slBaFaceTarget,"slBaFaceTarget",other_slBaFaceTarget);
LINK_ENTITY_TO_FUNC(slControllerTakeCover,"slControllerTakeCover",other_slControllerTakeCover);
LINK_ENTITY_TO_FUNC(slFaceTarget,"slFaceTarget",other_slFaceTarget);
LINK_ENTITY_TO_FUNC(slFaceTargetScared,"slFaceTargetScared",other_slFaceTargetScared);
LINK_ENTITY_TO_FUNC(slGruntTakeCover,"slGruntTakeCover",other_slGruntTakeCover);
LINK_ENTITY_TO_FUNC(slGruntTakeCoverFromBestSound,"slGruntTakeCoverFromBestSound",other_slGruntTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(slGruntTossGrenadeCover,"slGruntTossGrenadeCover",other_slGruntTossGrenadeCover);
LINK_ENTITY_TO_FUNC(slIdleTrigger,"slIdleTrigger",other_slIdleTrigger);
LINK_ENTITY_TO_FUNC(slRunToScript,"slRunToScript",other_slRunToScript);
LINK_ENTITY_TO_FUNC(slTakeCoverFromBestSound,"slTakeCoverFromBestSound",other_slTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(slTakeCoverFromEnemy,"slTakeCoverFromEnemy",other_slTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(slTakeCoverFromOrigin,"slTakeCoverFromOrigin",other_slTakeCoverFromOrigin);
LINK_ENTITY_TO_FUNC(slTlkIdleEyecontact,"slTlkIdleEyecontact",other_slTlkIdleEyecontact);
LINK_ENTITY_TO_FUNC(slTlkIdleWatchClient,"slTlkIdleWatchClient",other_slTlkIdleWatchClient);
LINK_ENTITY_TO_FUNC(slTwitchDie,"slTwitchDie",other_slTwitchDie);
LINK_ENTITY_TO_FUNC(slWalkToScript,"slWalkToScript",other_slWalkToScript);
LINK_ENTITY_TO_FUNC(tlAGruntTakeCoverFromEnemy,"tlAGruntTakeCoverFromEnemy",other_tlAGruntTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(tlAGruntThreatDisplay,"tlAGruntThreatDisplay",other_tlAGruntThreatDisplay);
LINK_ENTITY_TO_FUNC(tlAssassinTakeCoverFromBestSound,"tlAssassinTakeCoverFromBestSound",other_tlAssassinTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(tlAssassinTakeCoverFromEnemy,"tlAssassinTakeCoverFromEnemy",other_tlAssassinTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(tlAssassinTakeCoverFromEnemy2,"tlAssassinTakeCoverFromEnemy2",other_tlAssassinTakeCoverFromEnemy2);
LINK_ENTITY_TO_FUNC(tlBaFaceTarget,"tlBaFaceTarget",other_tlBaFaceTarget);
LINK_ENTITY_TO_FUNC(tlControllerTakeCover,"tlControllerTakeCover",other_tlControllerTakeCover);
LINK_ENTITY_TO_FUNC(tlFaceTarget,"tlFaceTarget",other_tlFaceTarget);
LINK_ENTITY_TO_FUNC(tlFaceTargetScared,"tlFaceTargetScared",other_tlFaceTargetScared);
LINK_ENTITY_TO_FUNC(tlGruntTakeCover1,"tlGruntTakeCover1",other_tlGruntTakeCover1);
LINK_ENTITY_TO_FUNC(tlGruntTakeCoverFromBestSound,"tlGruntTakeCoverFromBestSound",other_tlGruntTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(tlGruntTossGrenadeCover1,"tlGruntTossGrenadeCover1",other_tlGruntTossGrenadeCover1);
LINK_ENTITY_TO_FUNC(tlTakeCoverFromBestSound,"tlTakeCoverFromBestSound",other_tlTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(tlTakeCoverFromEnemy,"tlTakeCoverFromEnemy",other_tlTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(tlTakeCoverFromOrigin,"tlTakeCoverFromOrigin",other_tlTakeCoverFromOrigin);
LINK_ENTITY_TO_FUNC(tlTlkIdleEyecontact,"tlTlkIdleEyecontact",other_tlTlkIdleEyecontact);
LINK_ENTITY_TO_FUNC(tlTlkIdleWatchClient,"tlTlkIdleWatchClient",other_tlTlkIdleWatchClient);
LINK_ENTITY_TO_FUNC(tlTlkIdleWatchClientStare,"tlTlkIdleWatchClientStare",other_tlTlkIdleWatchClientStare);
LINK_ENTITY_TO_FUNC(tlTwitchDie,"tlTwitchDie",other_tlTwitchDie);
LINK_ENTITY_TO_FUNC(weapon_beanstalkspell,"weapon_beanstalkspell",other_weapon_beanstalkspell);
LINK_ENTITY_TO_FUNC(weapon_bearbite,"weapon_bearbite",other_weapon_bearbite);
LINK_ENTITY_TO_FUNC(weapon_bearclaw,"weapon_bearclaw",other_weapon_bearclaw);
LINK_ENTITY_TO_FUNC(weapon_birdspell,"weapon_birdspell",other_weapon_birdspell);
LINK_ENTITY_TO_FUNC(weapon_cometspell,"weapon_cometspell",other_weapon_cometspell);
LINK_ENTITY_TO_FUNC(weapon_deathrayspell,"weapon_deathrayspell",other_weapon_deathrayspell);
LINK_ENTITY_TO_FUNC(weapon_doublemagicmisslespell,"weapon_doublemagicmisslespell",other_weapon_doublemagicmisslespell);
LINK_ENTITY_TO_FUNC(weapon_fireballspell,"weapon_fireballspell",other_weapon_fireballspell);
LINK_ENTITY_TO_FUNC(weapon_flamelickspell,"weapon_flamelickspell",other_weapon_flamelickspell);
LINK_ENTITY_TO_FUNC(weapon_forcespell,"weapon_forcespell",other_weapon_forcespell);
LINK_ENTITY_TO_FUNC(weapon_giantplantspell,"weapon_giantplantspell",other_weapon_giantplantspell);
LINK_ENTITY_TO_FUNC(weapon_icepokespell,"weapon_icepokespell",other_weapon_icepokespell);
LINK_ENTITY_TO_FUNC(weapon_levitationspell,"weapon_levitationspell",other_weapon_levitationspell);
LINK_ENTITY_TO_FUNC(weapon_magicmisslespell,"weapon_magicmisslespell",other_weapon_magicmisslespell);
LINK_ENTITY_TO_FUNC(weapon_meteorspell,"weapon_meteorspell",other_weapon_meteorspell);
LINK_ENTITY_TO_FUNC(weapon_mindmisslespell,"weapon_mindmisslespell",other_weapon_mindmisslespell);
LINK_ENTITY_TO_FUNC(weapon_rollingstonespell,"weapon_rollingstonespell",other_weapon_rollingstonespell);
LINK_ENTITY_TO_FUNC(weapon_skeletonspell,"weapon_skeletonspell",other_weapon_skeletonspell);
LINK_ENTITY_TO_FUNC(weapon_skullspell,"weapon_skullspell",other_weapon_skullspell);
LINK_ENTITY_TO_FUNC(weapon_spiritwizspell,"weapon_spiritwizspell",other_weapon_spiritwizspell);
LINK_ENTITY_TO_FUNC(weapon_spotboltspell,"weapon_spotboltspell",other_weapon_spotboltspell);
LINK_ENTITY_TO_FUNC(weapon_staff,"weapon_staff",other_weapon_staff);
LINK_ENTITY_TO_FUNC(weapon_thornblastspell,"weapon_thornblastspell",other_weapon_thornblastspell);
LINK_ENTITY_TO_FUNC(weapon_tornadospell,"weapon_tornadospell",other_weapon_tornadospell);
LINK_ENTITY_TO_FUNC(weapon_whirlwindspell,"weapon_whirlwindspell",other_weapon_whirlwindspell);
LINK_ENTITY_TO_FUNC(weapon_whiterayspell,"weapon_whiterayspell",other_weapon_whiterayspell);
LINK_ENTITY_TO_FUNC(weapon_wombatspell,"weapon_wombatspell",other_weapon_wombatspell);

// swarm (?? version)
LINK_ENTITY_TO_FUNC(ammo_adrenaline,"ammo_adrenaline",other_ammo_adrenaline);
LINK_ENTITY_TO_FUNC(ammo_needle,"ammo_needle",other_ammo_needle);
LINK_ENTITY_TO_FUNC(ammo_swarmprimary,"ammo_swarmprimary",other_ammo_swarmprimary);
LINK_ENTITY_TO_FUNC(ammo_swarmsecondary,"ammo_swarmsecondary",other_ammo_swarmsecondary);
LINK_ENTITY_TO_FUNC(ammo_swarm_xen_secondary,"ammo_swarm_xen_secondary",other_ammo_swarm_xen_secondary);
LINK_ENTITY_TO_FUNC(garg_hull,"garg_hull",other_garg_hull);
LINK_ENTITY_TO_FUNC(grapple_hook,"grapple_hook",other_grapple_hook);
LINK_ENTITY_TO_FUNC(item_swarm_flag,"item_swarm_flag",other_item_swarm_flag);
LINK_ENTITY_TO_FUNC(monster_c4,"monster_c4",other_monster_c4);
LINK_ENTITY_TO_FUNC(swarm_garg_stomp,"swarm_garg_stomp",other_swarm_garg_stomp);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_acontroller,"swarm_spawnpt_acontroller",other_swarm_spawnpt_acontroller);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_agarg,"swarm_spawnpt_agarg",other_swarm_spawnpt_agarg);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_aheadcrab,"swarm_spawnpt_aheadcrab",other_swarm_spawnpt_aheadcrab);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_aslave,"swarm_spawnpt_aslave",other_swarm_spawnpt_aslave);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_atripod,"swarm_spawnpt_atripod",other_swarm_spawnpt_atripod);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hassassin,"swarm_spawnpt_hassassin",other_swarm_spawnpt_hassassin);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hcommander,"swarm_spawnpt_hcommander",other_swarm_spawnpt_hcommander);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hengineer,"swarm_spawnpt_hengineer",other_swarm_spawnpt_hengineer);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hgrunt,"swarm_spawnpt_hgrunt",other_swarm_spawnpt_hgrunt);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hscientist,"swarm_spawnpt_hscientist",other_swarm_spawnpt_hscientist);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_human,"swarm_spawnpt_human",other_swarm_spawnpt_human);
LINK_ENTITY_TO_FUNC(swarm_team_alien,"swarm_team_alien",other_swarm_team_alien);
LINK_ENTITY_TO_FUNC(swarm_team_human,"swarm_team_human",other_swarm_team_human);
LINK_ENTITY_TO_FUNC(trigger_regen,"trigger_regen",other_trigger_regen);
LINK_ENTITY_TO_FUNC(weapon_adrenaline,"weapon_adrenaline",other_weapon_adrenaline);
LINK_ENTITY_TO_FUNC(weapon_eballblaster,"weapon_eballblaster",other_weapon_eballblaster);
LINK_ENTITY_TO_FUNC(weapon_islavezap,"weapon_islavezap",other_weapon_islavezap);
LINK_ENTITY_TO_FUNC(weapon_needle,"weapon_needle",other_weapon_needle);
LINK_ENTITY_TO_FUNC(weapon_pipebomb,"weapon_pipebomb",other_weapon_pipebomb);
LINK_ENTITY_TO_FUNC(weapon_sonicgrenade,"weapon_sonicgrenade",other_weapon_sonicgrenade);
LINK_ENTITY_TO_FUNC(weapon_swarm_gargflame,"weapon_swarm_gargflame",other_weapon_swarm_gargflame);



// entities for Opposing Force
LINK_ENTITY_TO_FUNC(ammo_556, "ammo_556",other_ammo_556);
LINK_ENTITY_TO_FUNC(ammo_762, "ammo_762",other_ammo_762);
LINK_ENTITY_TO_FUNC(ammo_eagleclip, "ammo_eagleclip",other_ammo_eagleclip);
LINK_ENTITY_TO_FUNC(ammo_spore, "ammo_spore",other_ammo_spore);
LINK_ENTITY_TO_FUNC(charged_bolt, "charged_bolt",other_charged_bolt);
LINK_ENTITY_TO_FUNC(displacer_ball, "displacer_ball",other_displacer_ball);
LINK_ENTITY_TO_FUNC(eagle_laser, "eagle_laser",other_eagle_laser);
LINK_ENTITY_TO_FUNC(env_blowercannon, "env_blowercannon",other_env_blowercannon);
LINK_ENTITY_TO_FUNC(env_electrified_wire, "env_electrified_wire",other_env_electrified_wire);
LINK_ENTITY_TO_FUNC(env_genewormcloud, "env_genewormcloud",other_env_genewormcloud);
LINK_ENTITY_TO_FUNC(env_genewormspawn, "env_genewormspawn",other_env_genewormspawn);
LINK_ENTITY_TO_FUNC(env_rope, "env_rope",other_env_rope);
LINK_ENTITY_TO_FUNC(env_spritetrain, "env_spritetrain",other_env_spritetrain);
LINK_ENTITY_TO_FUNC(func_op4mortarcontroller, "func_op4mortarcontroller",other_func_op4mortarcontroller);
LINK_ENTITY_TO_FUNC(func_tank_of, "func_tank_of",other_func_tank_of);
LINK_ENTITY_TO_FUNC(func_tankcontrols_of, "func_tankcontrols_of",other_func_tankcontrols_of);
LINK_ENTITY_TO_FUNC(func_tanklaser_of, "func_tanklaser_of",other_func_tanklaser_of);
LINK_ENTITY_TO_FUNC(func_tankmortar_of, "func_tankmortar_of",other_func_tankmortar_of);
LINK_ENTITY_TO_FUNC(func_tankrocket_of, "func_tankrocket_of",other_func_tankrocket_of);
LINK_ENTITY_TO_FUNC(gonomeguts, "gonomeguts",other_gonomeguts);
LINK_ENTITY_TO_FUNC(grapple_tip, "grapple_tip",other_grapple_tip);
LINK_ENTITY_TO_FUNC(hvr_blkop_rocket, "hvr_blkop_rocket",other_hvr_blkop_rocket);
LINK_ENTITY_TO_FUNC(info_ctfdetect, "info_ctfdetect",other_info_ctfdetect);
LINK_ENTITY_TO_FUNC(info_ctfspawn, "info_ctfspawn",other_info_ctfspawn);
LINK_ENTITY_TO_FUNC(info_ctfspawn_powerup, "info_ctfspawn_powerup",other_info_ctfspawn_powerup);
LINK_ENTITY_TO_FUNC(info_displacer_earth_target, "info_displacer_earth_target",other_info_displacer_earth_target);
LINK_ENTITY_TO_FUNC(info_displacer_xen_target, "info_displacer_xen_target",other_info_displacer_xen_target);
LINK_ENTITY_TO_FUNC(info_pitworm, "info_pitworm",other_info_pitworm);
LINK_ENTITY_TO_FUNC(info_pitworm_steam_lock, "info_pitworm_steam_lock",other_info_pitworm_steam_lock);
LINK_ENTITY_TO_FUNC(item_ctfaccelerator, "item_ctfaccelerator",other_item_ctfaccelerator);
LINK_ENTITY_TO_FUNC(item_ctfbackpack, "item_ctfbackpack",other_item_ctfbackpack);
LINK_ENTITY_TO_FUNC(item_ctfbase, "item_ctfbase",other_item_ctfbase);
LINK_ENTITY_TO_FUNC(item_ctfflag, "item_ctfflag",other_item_ctfflag);
LINK_ENTITY_TO_FUNC(item_ctflongjump, "item_ctflongjump",other_item_ctflongjump);
LINK_ENTITY_TO_FUNC(item_ctfportablehev, "item_ctfportablehev",other_item_ctfportablehev);
LINK_ENTITY_TO_FUNC(item_ctfregeneration, "item_ctfregeneration",other_item_ctfregeneration);
LINK_ENTITY_TO_FUNC(item_generic, "item_generic",other_item_generic);
LINK_ENTITY_TO_FUNC(item_nuclearbomb, "item_nuclearbomb",other_item_nuclearbomb);
LINK_ENTITY_TO_FUNC(item_nuclearbombbutton, "item_nuclearbombbutton",other_item_nuclearbombbutton);
LINK_ENTITY_TO_FUNC(item_nuclearbombtimer, "item_nuclearbombtimer",other_item_nuclearbombtimer);
LINK_ENTITY_TO_FUNC(monster_ShockTrooper_dead, "monster_ShockTrooper_dead",other_monster_ShockTrooper_dead);
LINK_ENTITY_TO_FUNC(monster_alien_babyvoltigore, "monster_alien_babyvoltigore",other_monster_alien_babyvoltigore);
LINK_ENTITY_TO_FUNC(monster_alien_slave_dead, "monster_alien_slave_dead",other_monster_alien_slave_dead);
LINK_ENTITY_TO_FUNC(monster_alien_voltigore, "monster_alien_voltigore",other_monster_alien_voltigore);
LINK_ENTITY_TO_FUNC(monster_assassin_repel, "monster_assassin_repel",other_monster_assassin_repel);
LINK_ENTITY_TO_FUNC(monster_blkop_apache, "monster_blkop_apache",other_monster_blkop_apache);
LINK_ENTITY_TO_FUNC(monster_blkop_osprey, "monster_blkop_osprey",other_monster_blkop_osprey);
LINK_ENTITY_TO_FUNC(monster_cleansuit_scientist, "monster_cleansuit_scientist",other_monster_cleansuit_scientist);
LINK_ENTITY_TO_FUNC(monster_cleansuit_scientist_dead, "monster_cleansuit_scientist_dead",other_monster_cleansuit_scientist_dead);
LINK_ENTITY_TO_FUNC(monster_drillsergeant, "monster_drillsergeant",other_monster_drillsergeant);
LINK_ENTITY_TO_FUNC(monster_fgrunt_repel, "monster_fgrunt_repel",other_monster_fgrunt_repel);
LINK_ENTITY_TO_FUNC(monster_geneworm, "monster_geneworm",other_monster_geneworm);
LINK_ENTITY_TO_FUNC(monster_gonome, "monster_gonome",other_monster_gonome);
LINK_ENTITY_TO_FUNC(monster_gonome_dead, "monster_gonome_dead",other_monster_gonome_dead);
LINK_ENTITY_TO_FUNC(monster_grunt_ally_repel, "monster_grunt_ally_repel",other_monster_grunt_ally_repel);
LINK_ENTITY_TO_FUNC(monster_hfgrunt_dead, "monster_hfgrunt_dead",other_monster_hfgrunt_dead);
LINK_ENTITY_TO_FUNC(monster_houndeye_dead, "monster_houndeye_dead",other_monster_houndeye_dead);
LINK_ENTITY_TO_FUNC(monster_human_friendly_grunt, "monster_human_friendly_grunt",other_monster_human_friendly_grunt);
LINK_ENTITY_TO_FUNC(monster_human_grunt_ally, "monster_human_grunt_ally",other_monster_human_grunt_ally);
LINK_ENTITY_TO_FUNC(monster_human_grunt_ally_dead, "monster_human_grunt_ally_dead",other_monster_human_grunt_ally_dead);
LINK_ENTITY_TO_FUNC(monster_human_medic_ally, "monster_human_medic_ally",other_monster_human_medic_ally);
LINK_ENTITY_TO_FUNC(monster_human_torch_ally, "monster_human_torch_ally",other_monster_human_torch_ally);
LINK_ENTITY_TO_FUNC(monster_male_assassin, "monster_male_assassin",other_monster_male_assassin);
LINK_ENTITY_TO_FUNC(monster_massassin_dead, "monster_massassin_dead",other_monster_massassin_dead);
LINK_ENTITY_TO_FUNC(monster_medic_ally_repel, "monster_medic_ally_repel",other_monster_medic_ally_repel);
LINK_ENTITY_TO_FUNC(monster_op4loader, "monster_op4loader",other_monster_op4loader);
LINK_ENTITY_TO_FUNC(monster_otis, "monster_otis",other_monster_otis);
LINK_ENTITY_TO_FUNC(monster_otis_dead, "monster_otis_dead",other_monster_otis_dead);
LINK_ENTITY_TO_FUNC(monster_penguin, "monster_penguin",other_monster_penguin);
LINK_ENTITY_TO_FUNC(monster_pitdrone, "monster_pitdrone",other_monster_pitdrone);
LINK_ENTITY_TO_FUNC(monster_pitworm, "monster_pitworm",other_monster_pitworm);
LINK_ENTITY_TO_FUNC(monster_pitworm_up, "monster_pitworm_up",other_monster_pitworm_up);
LINK_ENTITY_TO_FUNC(monster_recruit, "monster_recruit",other_monster_recruit);
LINK_ENTITY_TO_FUNC(monster_shockroach, "monster_shockroach",other_monster_shockroach);
LINK_ENTITY_TO_FUNC(monster_shocktrooper, "monster_shocktrooper",other_monster_shocktrooper);
LINK_ENTITY_TO_FUNC(monster_shocktrooper_repel, "monster_shocktrooper_repel",other_monster_shocktrooper_repel);
LINK_ENTITY_TO_FUNC(monster_sitting_cleansuit_scientist, "monster_sitting_cleansuit_scientist",other_monster_sitting_cleansuit_scientist);
LINK_ENTITY_TO_FUNC(monster_skeleton_dead, "monster_skeleton_dead",other_monster_skeleton_dead);
LINK_ENTITY_TO_FUNC(monster_torch_ally_repel, "monster_torch_ally_repel",other_monster_torch_ally_repel);
LINK_ENTITY_TO_FUNC(monster_zombie_barney, "monster_zombie_barney",other_monster_zombie_barney);
LINK_ENTITY_TO_FUNC(monster_zombie_soldier, "monster_zombie_soldier",other_monster_zombie_soldier);
LINK_ENTITY_TO_FUNC(monster_zombie_soldier_dead, "monster_zombie_soldier_dead",other_monster_zombie_soldier_dead);
LINK_ENTITY_TO_FUNC(mortar_shell, "mortar_shell",other_mortar_shell);
LINK_ENTITY_TO_FUNC(op4mortar, "op4mortar",other_op4mortar);
LINK_ENTITY_TO_FUNC(pitdronespike, "pitdronespike",other_pitdronespike);
LINK_ENTITY_TO_FUNC(pitworm_gib, "pitworm_gib",other_pitworm_gib);
LINK_ENTITY_TO_FUNC(pitworm_gibshooter, "pitworm_gibshooter",other_pitworm_gibshooter);
LINK_ENTITY_TO_FUNC(rope_sample, "rope_sample",other_rope_sample);
LINK_ENTITY_TO_FUNC(rope_segment, "rope_segment",other_rope_segment);
LINK_ENTITY_TO_FUNC(shock_beam, "shock_beam",other_shock_beam);
LINK_ENTITY_TO_FUNC(spore, "spore",other_spore);
LINK_ENTITY_TO_FUNC(trigger_ctfgeneric, "trigger_ctfgeneric",other_trigger_ctfgeneric);
LINK_ENTITY_TO_FUNC(trigger_geneworm_hit, "trigger_geneworm_hit",other_trigger_geneworm_hit);
LINK_ENTITY_TO_FUNC(trigger_kill_nogib, "trigger_kill_nogib",other_trigger_kill_nogib);
LINK_ENTITY_TO_FUNC(trigger_playerfreeze, "trigger_playerfreeze",other_trigger_playerfreeze);
LINK_ENTITY_TO_FUNC(trigger_xen_return, "trigger_xen_return",other_trigger_xen_return);
LINK_ENTITY_TO_FUNC(weapon_displacer, "weapon_displacer",other_weapon_displacer);
LINK_ENTITY_TO_FUNC(weapon_eagle, "weapon_eagle",other_weapon_eagle);
LINK_ENTITY_TO_FUNC(weapon_grapple, "weapon_grapple",other_weapon_grapple);
LINK_ENTITY_TO_FUNC(weapon_penguin, "weapon_penguin",other_weapon_penguin);
LINK_ENTITY_TO_FUNC(weapon_pipewrench, "weapon_pipewrench",other_weapon_pipewrench);
LINK_ENTITY_TO_FUNC(weapon_shockrifle, "weapon_shockrifle",other_weapon_shockrifle);
LINK_ENTITY_TO_FUNC(weapon_shockroach, "weapon_shockroach",other_weapon_shockroach);
LINK_ENTITY_TO_FUNC(weapon_sniperrifle, "weapon_sniperrifle",other_weapon_sniperrifle);
LINK_ENTITY_TO_FUNC(weapon_sporelauncher, "weapon_sporelauncher",other_weapon_sporelauncher);

// wasteland mod
LINK_ENTITY_TO_FUNC(ammo_caseless,"ammo_caseless",other_ammo_caseless);
LINK_ENTITY_TO_FUNC(ammo_Deagle,"ammo_Deagle",other_ammo_Deagle);
LINK_ENTITY_TO_FUNC(ammo_fnfal,"ammo_fnfal",other_ammo_fnfal);
LINK_ENTITY_TO_FUNC(dc_info_objective,"dc_info_objective",other_dc_info_objective);
LINK_ENTITY_TO_FUNC(dc_info_objectposition,"dc_info_objectposition",other_dc_info_objectposition);
LINK_ENTITY_TO_FUNC(dc_object,"dc_object",other_dc_object);
LINK_ENTITY_TO_FUNC(dc_trigger_objectcapture,"dc_trigger_objectcapture",other_dc_trigger_objectcapture);
LINK_ENTITY_TO_FUNC(dc_trigger_reset,"dc_trigger_reset",other_dc_trigger_reset);
LINK_ENTITY_TO_FUNC(dc_trigger_team,"dc_trigger_team",other_dc_trigger_team);
LINK_ENTITY_TO_FUNC(info_team_ronin,"info_team_ronin",other_info_team_ronin);
LINK_ENTITY_TO_FUNC(info_team_scavenger,"info_team_scavenger",other_info_team_scavenger);
LINK_ENTITY_TO_FUNC(info_team_usmc,"info_team_usmc",other_info_team_usmc);
LINK_ENTITY_TO_FUNC(laser_aimer,"laser_aimer",other_laser_aimer);
LINK_ENTITY_TO_FUNC(weapon_boltrifle,"weapon_boltrifle",other_weapon_boltrifle);
LINK_ENTITY_TO_FUNC(weapon_deserteagle,"weapon_deserteagle",other_weapon_deserteagle);
LINK_ENTITY_TO_FUNC(weapon_fnfal,"weapon_fnfal",other_weapon_fnfal);
LINK_ENTITY_TO_FUNC(weapon_g11,"weapon_g11",other_weapon_g11);
LINK_ENTITY_TO_FUNC(weapon_G11,"weapon_G11",other_weapon_G11);
LINK_ENTITY_TO_FUNC(weapon_jackhammer,"weapon_jackhammer",other_weapon_jackhammer);
LINK_ENTITY_TO_FUNC(weapon_katana,"weapon_katana",other_weapon_katana);
LINK_ENTITY_TO_FUNC(weapon_molotov,"weapon_molotov",other_weapon_molotov);
LINK_ENTITY_TO_FUNC(weapon_rifle,"weapon_rifle",other_weapon_rifle);
LINK_ENTITY_TO_FUNC(weapon_sawedoff,"weapon_sawedoff",other_weapon_sawedoff);
LINK_ENTITY_TO_FUNC(weapon_sledge,"weapon_sledge",other_weapon_sledge);
LINK_ENTITY_TO_FUNC(weapon_spear,"weapon_spear",other_weapon_spear);
LINK_ENTITY_TO_FUNC(spear_bolt,"spear_bolt",other_spear_bolt);

// Wasteland beta 2
LINK_ENTITY_TO_FUNC(RONINTeamScore,"RONINTeamScore",other_RONINTeamScore);
LINK_ENTITY_TO_FUNC(SCAVENGERTeamScore,"SCAVENGERTeamScore",other_SCAVENGERTeamScore);
LINK_ENTITY_TO_FUNC(USMCTeamScore,"USMCTeamScore",other_USMCTeamScore);




// svencoop 1.9 entities
LINK_ENTITY_TO_FUNC(monster_chumtoad,"monster_chumtoad",other_monster_chumtoad);
LINK_ENTITY_TO_FUNC(squadmaker,"squadmaker",other_squadmaker);
LINK_ENTITY_TO_FUNC(trigger_random,"trigger_random",other_trigger_random);
LINK_ENTITY_TO_FUNC(trigger_random_time,"trigger_random_time",other_trigger_random_time);
LINK_ENTITY_TO_FUNC(weapon_uziakimbo,"weapon_uziakimbo",other_weapon_uziakimbo);



// DoD 1.0
LINK_ENTITY_TO_FUNC(weapon_handgrenade_ex,"weapon_handgrenade_ex",other_weapon_handgrenade_ex);
LINK_ENTITY_TO_FUNC(weapon_amerknife,"weapon_amerknife",other_weapon_amerknife);
LINK_ENTITY_TO_FUNC(weapon_germanknife,"weapon_germanknife",other_weapon_germanknife);
LINK_ENTITY_TO_FUNC(dod_object,"dod_object",other_dod_object);
LINK_ENTITY_TO_FUNC(dod_score_ent,"dod_score_ent",other_dod_score_ent);
LINK_ENTITY_TO_FUNC(dod_secondary,"dod_secondary",other_dod_secondary);
LINK_ENTITY_TO_FUNC(func_destroy_objective,"func_destroy_objective",other_func_destroy_objective);
LINK_ENTITY_TO_FUNC(weapon_stickgrenade_ex,"weapon_stickgrenade_ex",other_weapon_stickgrenade_ex);
LINK_ENTITY_TO_FUNC(info_player_axis,"info_player_axis",other_info_player_axis);
LINK_ENTITY_TO_FUNC(info_player_allies,"info_player_allies",other_info_player_allies);
LINK_ENTITY_TO_FUNC(info_initial_player_axis,"info_initial_player_axis",other_info_initial_player_axis);
LINK_ENTITY_TO_FUNC(info_initial_player_allies,"info_initial_player_allies",other_info_initial_player_allies);
LINK_ENTITY_TO_FUNC(weapon_stickgrenade,"weapon_stickgrenade",other_weapon_stickgrenade);
LINK_ENTITY_TO_FUNC(dod_object_goal,"dod_object_goal",other_dod_object_goal);
LINK_ENTITY_TO_FUNC(trigger_control,"trigger_control",other_trigger_control);
LINK_ENTITY_TO_FUNC(dod_control_point,"dod_control_point",other_dod_control_point);
LINK_ENTITY_TO_FUNC(dod_control_point_master,"dod_control_point_master",other_dod_control_point_master);
LINK_ENTITY_TO_FUNC(weapon_garand,"weapon_garand",other_weapon_garand);
LINK_ENTITY_TO_FUNC(ammo_garand,"ammo_garand",other_ammo_garand);
LINK_ENTITY_TO_FUNC(weapon_gewehr,"weapon_gewehr",other_weapon_gewehr);
LINK_ENTITY_TO_FUNC(ammo_gewehr,"ammo_gewehr",other_ammo_gewehr);
LINK_ENTITY_TO_FUNC(weapon_kar,"weapon_kar",other_weapon_kar);
LINK_ENTITY_TO_FUNC(ammo_kar,"ammo_kar",other_ammo_kar);
LINK_ENTITY_TO_FUNC(weapon_luger,"weapon_luger",other_weapon_luger);
LINK_ENTITY_TO_FUNC(ammo_luger,"ammo_luger",other_ammo_luger);
LINK_ENTITY_TO_FUNC(weapon_mp40,"weapon_mp40",other_weapon_mp40);
LINK_ENTITY_TO_FUNC(weapon_mp44,"weapon_mp44",other_weapon_mp44);
LINK_ENTITY_TO_FUNC(ammo_mp44,"ammo_mp44",other_ammo_mp44);
LINK_ENTITY_TO_FUNC(ammo_mp40,"ammo_mp40",other_ammo_mp40);
LINK_ENTITY_TO_FUNC(weapon_spring,"weapon_spring",other_weapon_spring);
LINK_ENTITY_TO_FUNC(ammo_spring,"ammo_spring",other_ammo_spring);
LINK_ENTITY_TO_FUNC(weapon_thompson,"weapon_thompson",other_weapon_thompson);
LINK_ENTITY_TO_FUNC(ammo_thompson,"ammo_thompson",other_ammo_thompson);
LINK_ENTITY_TO_FUNC(weapon_bar,"weapon_bar",other_weapon_bar);
LINK_ENTITY_TO_FUNC(ammo_bar ,"ammo_bar",other_ammo_bar);
