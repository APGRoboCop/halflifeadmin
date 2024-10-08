/* $Id: linkfunc.cpp,v 1.1.1.2 2003/11/07 18:50:39 darope Exp $
 *
 *   This file is part of Admin Mod.
 *
 *   Admin Mod is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Admin Mod is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Admin Mod; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *   In addition, as a special exception, the author gives permission to
 *   link the code of this program with the Half-Life Game Engine ("HL
 *   Engine") and Modified Game Libraries ("MODs") developed by VALVe,
 *   L.L.C ("Valve") and Modified Game Libraries developed by Gearbox
 *   Software ("Gearbox").  You must obey the GNU General Public License
 *   in all respects for all of the code used other than the HL Engine and
 *   MODs from Valve or Gearbox. If you modify this file, you may extend
 *   this exception to your version of the file, but you are not obligated
 *   to do so.  If you do not wish to do so, delete this exception statement
 *   from your version.
 *
*/

#include "extdll.h"

#ifdef _WIN32
extern HINSTANCE h_Library;
typedef void(FAR* LINK_ENTITY_FUNC)(entvars_t*);
#else
extern void* h_Library;
typedef void (*LINK_ENTITY_FUNC)(entvars_t*);
#include <dlfcn.h>
#endif

#ifdef _WIN32
#define LINK_ENTITY_TO_FUNC(mapClassName, mapClassString, otherClassName)                 \
    extern "C" _declspec(dllexport) void mapClassName(entvars_t* pev);                    \
    void mapClassName(entvars_t* pev)                                                     \
    {                                                                                     \
        static LINK_ENTITY_FUNC otherClassName = nullptr;                                    \
        static int missing = 0;                                                           \
        if(missing == 1)                                                                  \
            return;                                                                       \
        if((otherClassName) == nullptr)                                                        \
            (otherClassName) = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, mapClassString); \
        if((otherClassName) == nullptr) {                                                      \
            missing = 1;                                                                  \
            UTIL_LogPrintf("[ADMIN] ERROR: couldn't find entity %s\n", mapClassString);   \
            return;                                                                       \
        }                                                                                 \
        (*(otherClassName))(pev);                                                           \
    }
#else
#define LINK_ENTITY_TO_FUNC(mapClassName, mapClassString, otherClassName)               \
    extern "C" void mapClassName(entvars_t* pev);                                       \
    void mapClassName(entvars_t* pev)                                                   \
    {                                                                                   \
        static LINK_ENTITY_FUNC otherClassName = nullptr;                                  \
        static int missing = 0;                                                         \
        if(missing == 1)                                                                \
            return;                                                                     \
        if(otherClassName == nullptr)                                                      \
            otherClassName = (LINK_ENTITY_FUNC)dlsym(h_Library, mapClassString);        \
        if(otherClassName == nullptr) {                                                    \
            missing = 1;                                                                \
            UTIL_LogPrintf("[ADMIN] ERROR: couldn't find entity %s\n", mapClassString); \
            return;                                                                     \
        }                                                                               \
        (*otherClassName)(pev);                                                         \
    }
#endif
/*
// common weapons name (firearms and cs)
LINK_ENTITY_TO_FUNC(weapon_ak47, "weapon_ak47", other_weapon_ak47);
LINK_ENTITY_TO_FUNC(ammo_ak47, "ammo_ak47", other_ammo_ak47);
LINK_ENTITY_TO_FUNC(weapon_aug, "weapon_aug", other_weapon_aug);
LINK_ENTITY_TO_FUNC(ammo_aug, "ammo_aug", other_ammo_aug);
LINK_ENTITY_TO_FUNC(grenade, "grenade", other_grenade);
LINK_ENTITY_TO_FUNC(weapon_frag, "weapon_frag", other_weapon_frag);
LINK_ENTITY_TO_FUNC(weapon_knife, "weapon_knife", other_weapon_knife);
LINK_ENTITY_TO_FUNC(weapon_flashbang, "weapon_flashbang", other_weapon_flashbang);
LINK_ENTITY_TO_FUNC(ammo_flashbang, "ammo_flashbang", other_ammo_flashbang);
LINK_ENTITY_TO_FUNC(weaponbox, "weaponbox", other_weaponbox);

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
// LINK_ENTITY_TO_FUNC(weaponbox, "weaponbox", other_weaponbox);
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
LINK_ENTITY_TO_FUNC(CreateInterface, "CreateInterface", other_CreateInterface);
LINK_ENTITY_TO_FUNC(GetChaseOrigin, "GetChaseOrigin", other_GetChaseOrigin);

// new tfc stuff
LINK_ENTITY_TO_FUNC(CTF_Map, "CTF_Map", other_CTF_Map);

// new tfc 1.1.0.4 stuff
LINK_ENTITY_TO_FUNC(info_areadef, "info_areadef", other_info_areadef);
LINK_ENTITY_TO_FUNC(info_tf_teamcheck, "info_tf_teamcheck", other_info_tf_teamcheck);
LINK_ENTITY_TO_FUNC(info_tf_teamset, "info_tf_teamset", other_info_tf_teamset);
LINK_ENTITY_TO_FUNC(func_nogrenades, "func_nogrenades", other_func_nogrenades);

// entities for botman's bots...
LINK_ENTITY_TO_FUNC(bot, "bot", other_bot);
LINK_ENTITY_TO_FUNC(entity_botcam, "entity_botcam", other_entity_botcam);

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
// LINK_ENTITY_TO_FUNC(weapon_frag, "weapon_frag", other_weapon_frag);
LINK_ENTITY_TO_FUNC(weapon_handcannon, "weapon_handcannon", other_weapon_handcannon);
LINK_ENTITY_TO_FUNC(weapon_hkmp5, "weapon_hkmp5", other_weapon_hkmp5);
LINK_ENTITY_TO_FUNC(weapon_ithaca, "weapon_ithaca", other_weapon_ithaca);
// LINK_ENTITY_TO_FUNC(weapon_knife, "weapon_knife", other_weapon_knife);
LINK_ENTITY_TO_FUNC(weapon_sniper, "weapon_sniper", other_weapon_sniper);

// entities for Counter-Strike (Beta 6.2a)
LINK_ENTITY_TO_FUNC(armoury_entity, "armoury_entity", other_armoury_entity);
LINK_ENTITY_TO_FUNC(ammo_338magnum, "ammo_338magnum", other_ammo_338magnum);
LINK_ENTITY_TO_FUNC(ammo_357sig, "ammo_357sig", other_ammo_357sig);
LINK_ENTITY_TO_FUNC(ammo_45acp, "ammo_45acp", other_ammo_45acp);
LINK_ENTITY_TO_FUNC(ammo_50ae, "ammo_50ae", other_ammo_50ae);
LINK_ENTITY_TO_FUNC(ammo_556nato, "ammo_556nato", other_ammo_556nato);
LINK_ENTITY_TO_FUNC(ammo_57mm, "ammo_57mm", other_ammo_57mm);
LINK_ENTITY_TO_FUNC(ammo_762nato, "ammo_762nato", other_ammo_762nato);
LINK_ENTITY_TO_FUNC(ammo_9mm, "ammo_9mm", other_ammo_9mm);
LINK_ENTITY_TO_FUNC(ammo_556natobox, "ammo_556natobox", other_ammo_556natobox);
// LINK_ENTITY_TO_FUNC(ammo_flashbang, "ammo_flashbang", other_ammo_flashbang);
LINK_ENTITY_TO_FUNC(debris, "debris", other_debris);
LINK_ENTITY_TO_FUNC(env_bombglow, "env_bombglow", other_env_bombglow);
LINK_ENTITY_TO_FUNC(func_bomb_target, "func_bomb_target", other_func_bomb_target);
LINK_ENTITY_TO_FUNC(func_buyzone, "func_buyzone", other_func_buyzone);
LINK_ENTITY_TO_FUNC(func_escapezone, "func_escapezone", other_func_escapezone);
LINK_ENTITY_TO_FUNC(func_hostage_rescue, "func_hostage_rescue", other_func_hostage_rescue);
LINK_ENTITY_TO_FUNC(func_vip_safetyzone, "func_vip_safetyzone", other_func_vip_safetyzone);
LINK_ENTITY_TO_FUNC(hostage_entity, "hostage_entity", other_hostage_entity);
LINK_ENTITY_TO_FUNC(info_bomb_target, "info_bomb_target", other_info_bomb_target);
LINK_ENTITY_TO_FUNC(info_hostage_rescue, "info_hostage_rescue", other_info_hostage_rescue);
LINK_ENTITY_TO_FUNC(info_vip_start, "info_vip_start", other_info_vip_start);
LINK_ENTITY_TO_FUNC(item_assaultsuit, "item_assaultsuit", other_item_assaultsuit);
LINK_ENTITY_TO_FUNC(item_kevlar, "item_kevlar", other_item_kevlar);
LINK_ENTITY_TO_FUNC(item_thighpack, "item_thighpack", other_item_thighpack);
// LINK_ENTITY_TO_FUNC(weapon_ak47, "weapon_ak47", other_weapon_ak47);
LINK_ENTITY_TO_FUNC(weapon_awp, "weapon_awp", other_weapon_awp);
// LINK_ENTITY_TO_FUNC(weapon_aug,"weapon_aug",other_weapon_aug);
LINK_ENTITY_TO_FUNC(weapon_c4, "weapon_c4", other_weapon_c4);
LINK_ENTITY_TO_FUNC(weapon_deagle, "weapon_deagle", other_weapon_deagle);
// LINK_ENTITY_TO_FUNC(weapon_flashbang, "weapon_flashbang", other_weapon_flashbang);
LINK_ENTITY_TO_FUNC(weapon_g3sg1, "weapon_g3sg1", other_weapon_g3sg1);
LINK_ENTITY_TO_FUNC(weapon_glock18, "weapon_glock18", other_weapon_glock18);
LINK_ENTITY_TO_FUNC(weapon_hegrenade, "weapon_hegrenade", other_weapon_hegrenade);
// LINK_ENTITY_TO_FUNC(weapon_knife, "weapon_knife", other_weapon_knife);
LINK_ENTITY_TO_FUNC(weapon_m249, "weapon_m249", other_weapon_m249);
LINK_ENTITY_TO_FUNC(weapon_m3, "weapon_m3", other_weapon_m3);
LINK_ENTITY_TO_FUNC(weapon_m4a1, "weapon_m4a1", other_weapon_m4a1);
LINK_ENTITY_TO_FUNC(weapon_mac10, "weapon_mac10", other_weapon_mac10);
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
LINK_ENTITY_TO_FUNC(func_grencatch, "func_grencatch", other_func_grencatch);
LINK_ENTITY_TO_FUNC(func_weaponcheck, "func_weaponcheck", other_func_weaponcheck);
LINK_ENTITY_TO_FUNC(weapon_fiveseven, "weapon_fiveseven", other_weapon_fiveseven);
LINK_ENTITY_TO_FUNC(weapon_sg550, "weapon_sg550", other_weapon_sg550);

// new cstrike b7 entities
LINK_ENTITY_TO_FUNC(func_vehicle, "func_vehicle", other_func_vehicle);
LINK_ENTITY_TO_FUNC(func_vehiclecontrols, "func_vehiclecontrols", other_func_vehiclecontrols);
LINK_ENTITY_TO_FUNC(weapon_elite, "weapon_elite", other_weapon_elite);

// new cstrike 6.5 entities
LINK_ENTITY_TO_FUNC(weapon_smokegrenade, "weapon_smokegrenade", other_weapon_smokegrenade);
LINK_ENTITY_TO_FUNC(info_map_parameters, "info_map_parameters", other_info_map_parameters);

// CZ/CS 1.6 entities - [APG]RoboCop[CL] (famas_already included from FA 2.4)
LINK_ENTITY_TO_FUNC(weapon_galil, "weapon_galil", other_weapon_galil);

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
LINK_ENTITY_TO_FUNC(item_artifact_invulnerability,
    "item_artifact_invulnerability",
    other_item_artifact_invulnerability);
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
LINK_ENTITY_TO_FUNC(tf_weapon_genericprimedgrenade,
    "tf_weapon_genericprimedgrenade",
    other_tf_weapon_genericprimedgrenade);
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

// entities for FireArms 2.6 - Added by Frank McNeil for MarineBot - [APG]RoboCop[CL]

//ammo
//LINK_ENTITY_TO_FUNC(ammo_ak47);
LINK_ENTITY_TO_FUNC(ammo_ak74);
LINK_ENTITY_TO_FUNC(ammo_anaconda);
LINK_ENTITY_TO_FUNC(ammo_benelli);
LINK_ENTITY_TO_FUNC(ammo_ber92f);
LINK_ENTITY_TO_FUNC(ammo_ber93r);
LINK_ENTITY_TO_FUNC(ammo_bizon);
LINK_ENTITY_TO_FUNC(ammo_coltgov);
LINK_ENTITY_TO_FUNC(ammo_concussion);
LINK_ENTITY_TO_FUNC(ammo_desert);
LINK_ENTITY_TO_FUNC(ammo_famas);
LINK_ENTITY_TO_FUNC(ammo_frag);
LINK_ENTITY_TO_FUNC(ammo_g36e);
LINK_ENTITY_TO_FUNC(ammo_g3a3);
LINK_ENTITY_TO_FUNC(ammo_gp25);
LINK_ENTITY_TO_FUNC(ammo_m16);
LINK_ENTITY_TO_FUNC(ammo_m203);
LINK_ENTITY_TO_FUNC(ammo_m60);
LINK_ENTITY_TO_FUNC(ammo_m79);
LINK_ENTITY_TO_FUNC(ammo_m82);
LINK_ENTITY_TO_FUNC(ammo_mp5a5);
LINK_ENTITY_TO_FUNC(ammo_pkm);
LINK_ENTITY_TO_FUNC(ammo_saiga);
LINK_ENTITY_TO_FUNC(ammo_ssg3000);
LINK_ENTITY_TO_FUNC(ammo_sterling);
LINK_ENTITY_TO_FUNC(ammo_stg24);
LINK_ENTITY_TO_FUNC(ammo_svd);
LINK_ENTITY_TO_FUNC(ammo_uzi);
//LINK_ENTITY_TO_FUNC(weapon_ak47);
LINK_ENTITY_TO_FUNC(weapon_ak74);
LINK_ENTITY_TO_FUNC(weapon_anaconda);
LINK_ENTITY_TO_FUNC(weapon_benelli);
LINK_ENTITY_TO_FUNC(weapon_ber92f);
LINK_ENTITY_TO_FUNC(weapon_ber93r);
LINK_ENTITY_TO_FUNC(weapon_bizon);
LINK_ENTITY_TO_FUNC(weapon_claymore);
LINK_ENTITY_TO_FUNC(weapon_coltgov);
LINK_ENTITY_TO_FUNC(weapon_concussion);
LINK_ENTITY_TO_FUNC(weapon_desert);
LINK_ENTITY_TO_FUNC(weapon_frag);
LINK_ENTITY_TO_FUNC(weapon_g36e);
LINK_ENTITY_TO_FUNC(weapon_g3a3);
LINK_ENTITY_TO_FUNC(weapon_knife);
LINK_ENTITY_TO_FUNC(weapon_m16);
LINK_ENTITY_TO_FUNC(weapon_m60);
LINK_ENTITY_TO_FUNC(weapon_m79);
LINK_ENTITY_TO_FUNC(weapon_m82);
LINK_ENTITY_TO_FUNC(weapon_mp5a5);
LINK_ENTITY_TO_FUNC(weapon_pkm);
LINK_ENTITY_TO_FUNC(weapon_saiga);
LINK_ENTITY_TO_FUNC(weapon_ssg3000);
LINK_ENTITY_TO_FUNC(weapon_sterling);
LINK_ENTITY_TO_FUNC(weapon_stg24);
LINK_ENTITY_TO_FUNC(weapon_svd);
LINK_ENTITY_TO_FUNC(weapon_uzi);
//items
LINK_ENTITY_TO_FUNC(item_bandage);
LINK_ENTITY_TO_FUNC(item_claymore);
LINK_ENTITY_TO_FUNC(item_concussion);
LINK_ENTITY_TO_FUNC(item_frag);
LINK_ENTITY_TO_FUNC(item_stg24);
//mics
//LINK_ENTITY_TO_FUNC(ammobox);
LINK_ENTITY_TO_FUNC(fa_drop_zone);
LINK_ENTITY_TO_FUNC(fa_parachute);
LINK_ENTITY_TO_FUNC(fa_push_flag);
LINK_ENTITY_TO_FUNC(fa_push_point);
LINK_ENTITY_TO_FUNC(fa_sd_object);
LINK_ENTITY_TO_FUNC(fa_team_goal);
LINK_ENTITY_TO_FUNC(fa_team_item);
LINK_ENTITY_TO_FUNC(hospital);
LINK_ENTITY_TO_FUNC(info_firearms_detect);
LINK_ENTITY_TO_FUNC(info_playerstart_blue);
LINK_ENTITY_TO_FUNC(info_playerstart_red);
LINK_ENTITY_TO_FUNC(marker);
LINK_ENTITY_TO_FUNC(mortarshell);
LINK_ENTITY_TO_FUNC(weapmortar);

// probably new entities for FireArms 2.65
LINK_ENTITY_TO_FUNC(bullet);
LINK_ENTITY_TO_FUNC(env_spriteshooter);
LINK_ENTITY_TO_FUNC(medevac);
LINK_ENTITY_TO_FUNC(player_helmet);
LINK_ENTITY_TO_FUNC(trigger_zone);
LINK_ENTITY_TO_FUNC(vote_llama);

// new entities for FireArms 2.7
//LINK_ENTITY_TO_FUNC(ammo_m249);
//LINK_ENTITY_TO_FUNC(weapon_m249);
LINK_ENTITY_TO_FUNC(env_fog);

// new entities for FireArms 2.8
LINK_ENTITY_TO_FUNC(ammo_remington);
LINK_ENTITY_TO_FUNC(env_precip);
LINK_ENTITY_TO_FUNC(env_sun);
LINK_ENTITY_TO_FUNC(item_flashlight);
LINK_ENTITY_TO_FUNC(item_nvg);
LINK_ENTITY_TO_FUNC(weapon_remington);

// new entities for FireArms 2.9
LINK_ENTITY_TO_FUNC(ammo_m14);
LINK_ENTITY_TO_FUNC(ammo_m4);
LINK_ENTITY_TO_FUNC(weapon_m14);
LINK_ENTITY_TO_FUNC(weapon_m4);
LINK_ENTITY_TO_FUNC(trigger_noclaymores);
LINK_ENTITY_TO_FUNC(trigger_reinforcements);

// entities for FireArms 2.5
LINK_ENTITY_TO_FUNC(item_flashbang);
LINK_ENTITY_TO_FUNC(weapon_flashbang);
LINK_ENTITY_TO_FUNC(weapon_mc51);
LINK_ENTITY_TO_FUNC(weapon_m11);
LINK_ENTITY_TO_FUNC(weapon_psg1);
LINK_ENTITY_TO_FUNC(weapon_aug);
LINK_ENTITY_TO_FUNC(ammo_flashbang);
LINK_ENTITY_TO_FUNC(ammo_mc51);
LINK_ENTITY_TO_FUNC(ammo_m11);
LINK_ENTITY_TO_FUNC(ammo_psg1);
LINK_ENTITY_TO_FUNC(ammo_aug);
LINK_ENTITY_TO_FUNC(ammo_m112);

// entities for FireArms 2.4
LINK_ENTITY_TO_FUNC(ammo_mp5k);
LINK_ENTITY_TO_FUNC(weapon_mp5k);


// firearms 2.3 entities
LINK_ENTITY_TO_FUNC(weapon_bizon, "weapon_bizon", other_weapon_bizon);
LINK_ENTITY_TO_FUNC(ammo_bizon, "ammo_bizon", other_ammo_bizon);

// firearms 2.2 new entities
LINK_ENTITY_TO_FUNC(weapon_launcher, "weapon_launcher", other_weapon_launcher);
LINK_ENTITY_TO_FUNC(ammo_launcher, "ammo_launcher", other_ammo_launcher);
LINK_ENTITY_TO_FUNC(ammo_40mm, "ammo_40mm", other_ammo_40mm);
LINK_ENTITY_TO_FUNC(ammo_762mmbox, "ammo_762mmbox", other_ammo_762mmbox);
LINK_ENTITY_TO_FUNC(hospital, "hospital", other_hospital);
LINK_ENTITY_TO_FUNC(marker, "marker", other_marker);
LINK_ENTITY_TO_FUNC(weapmortar, "weapmortar", other_weapmortar);

// entities for firearms rc1 (yes!)

LINK_ENTITY_TO_FUNC(ammo_anaconda, "ammo_anaconda", other_ammo_anaconda);
LINK_ENTITY_TO_FUNC(ammo_benelli, "ammo_benelli", other_ammo_benelli);
LINK_ENTITY_TO_FUNC(ammo_ber92f, "ammo_ber92f", other_ammo_ber92f);
LINK_ENTITY_TO_FUNC(ammo_ber93r, "ammo_ber93r", other_ammo_ber93r);
LINK_ENTITY_TO_FUNC(ammobox, "ammobox", other_ammobox);
LINK_ENTITY_TO_FUNC(ammo_coltgov, "ammo_coltgov", other_ammo_coltgov);
LINK_ENTITY_TO_FUNC(ammo_desert, "ammo_desert", other_ammo_desert);
LINK_ENTITY_TO_FUNC(ammo_famas, "ammo_famas", other_ammo_famas);
LINK_ENTITY_TO_FUNC(ammo_frag, "ammo_frag", other_ammo_frag);
LINK_ENTITY_TO_FUNC(ammo_g36e, "ammo_g36e", other_ammo_g36e);
LINK_ENTITY_TO_FUNC(ammo_g3a3, "ammo_g3a3", other_ammo_g3a3);
LINK_ENTITY_TO_FUNC(ammo_m11, "ammo_m11", other_ammo_m11);
LINK_ENTITY_TO_FUNC(ammo_m112, "ammo_m112", other_ammo_m112);
LINK_ENTITY_TO_FUNC(ammo_m16, "ammo_m16", other_ammo_m16);
LINK_ENTITY_TO_FUNC(ammo_m203, "ammo_m203", other_ammo_m203);
LINK_ENTITY_TO_FUNC(ammo_m4, "ammo_m4", other_ammo_m4);
LINK_ENTITY_TO_FUNC(ammo_m60, "ammo_m60", other_ammo_m60);
LINK_ENTITY_TO_FUNC(ammo_m79, "ammo_m79", other_ammo_m79);
LINK_ENTITY_TO_FUNC(ammo_m82, "ammo_m82", other_ammo_m82);
LINK_ENTITY_TO_FUNC(ammo_mp5a5, "ammo_mp5a5", other_ammo_mp5a5);
LINK_ENTITY_TO_FUNC(ammo_mp5k, "ammo_mp5k", other_ammo_mp5k);
LINK_ENTITY_TO_FUNC(ammo_psg1, "ammo_psg1", other_ammo_psg1);
LINK_ENTITY_TO_FUNC(ammo_saiga, "ammo_saiga", other_ammo_saiga);
LINK_ENTITY_TO_FUNC(ammo_stg24, "ammo_stg24", other_ammo_stg24);
LINK_ENTITY_TO_FUNC(bullet, "bullet", other_bullet);
LINK_ENTITY_TO_FUNC(fa_drop_zone, "fa_drop_zone", other_fa_drop_zone);
LINK_ENTITY_TO_FUNC(fa_parachute, "fa_parachute", other_fa_parachute);
LINK_ENTITY_TO_FUNC(fa_push_flag, "fa_push_flag", other_fa_push_flag);
LINK_ENTITY_TO_FUNC(fa_push_point, "fa_push_point", other_fa_push_point);
LINK_ENTITY_TO_FUNC(fa_team_goal, "fa_team_goal", other_fa_team_goal);
LINK_ENTITY_TO_FUNC(fa_team_item, "fa_team_item", other_fa_team_item);
LINK_ENTITY_TO_FUNC(info_firearms_detect, "info_firearms_detect", other_info_firearms_detect);
LINK_ENTITY_TO_FUNC(info_playerstart_blue, "info_playerstart_blue", other_info_playerstart_blue);
LINK_ENTITY_TO_FUNC(info_playerstart_red, "info_playerstart_red", other_info_playerstart_red);
LINK_ENTITY_TO_FUNC(item_bandage, "item_bandage", other_item_bandage);
LINK_ENTITY_TO_FUNC(item_claymore, "item_claymore", other_item_claymore);
LINK_ENTITY_TO_FUNC(item_flashbang, "item_flashbang", other_item_flashbang);
LINK_ENTITY_TO_FUNC(item_frag, "item_frag", other_item_frag);
LINK_ENTITY_TO_FUNC(item_stg24, "item_stg24", other_item_stg24);
LINK_ENTITY_TO_FUNC(weapon_benelli, "weapon_benelli", other_weapon_benelli);
LINK_ENTITY_TO_FUNC(weapon_ber92f, "weapon_ber92f", other_weapon_ber92f);
LINK_ENTITY_TO_FUNC(weapon_ber93r, "weapon_ber93r", other_weapon_ber93r);
LINK_ENTITY_TO_FUNC(weapon_claymore, "weapon_claymore", other_weapon_claymore);
LINK_ENTITY_TO_FUNC(weapon_coltgov, "weapon_coltgov", other_weapon_coltgov);
LINK_ENTITY_TO_FUNC(weapon_desert, "weapon_desert", other_weapon_desert);
LINK_ENTITY_TO_FUNC(weapon_famas, "weapon_famas", other_weapon_famas);
LINK_ENTITY_TO_FUNC(weapon_g36e, "weapon_g36e", other_weapon_g36e);
LINK_ENTITY_TO_FUNC(weapon_g3a3, "weapon_g3a3", other_weapon_g3a3);
LINK_ENTITY_TO_FUNC(weapon_m11, "weapon_m11", other_weapon_m11);
LINK_ENTITY_TO_FUNC(weapon_m16, "weapon_m16", other_weapon_m16);
LINK_ENTITY_TO_FUNC(weapon_m4, "weapon_m4", other_weapon_m4);
LINK_ENTITY_TO_FUNC(weapon_m60, "weapon_m60", other_weapon_m60);
LINK_ENTITY_TO_FUNC(weapon_m79, "weapon_m79", other_weapon_m79);
LINK_ENTITY_TO_FUNC(weapon_m82, "weapon_m82", other_weapon_m82);
LINK_ENTITY_TO_FUNC(weapon_mp5a5, "weapon_mp5a5", other_weapon_mp5a5);
LINK_ENTITY_TO_FUNC(weapon_mp5k, "weapon_mp5k", other_weapon_mp5k);
LINK_ENTITY_TO_FUNC(weapon_psg1, "weapon_psg1", other_weapon_psg1);
LINK_ENTITY_TO_FUNC(weapon_saiga, "weapon_saiga", other_weapon_saiga);
LINK_ENTITY_TO_FUNC(weapon_stg24, "weapon_stg24", other_weapon_stg24);
LINK_ENTITY_TO_FUNC(ammo_mc51, "ammo_mc51", other_ammo_mc51);
LINK_ENTITY_TO_FUNC(weapon_mc51, "weapon_mc51", other_weapon_mc51);

// goldeneye 1.7
LINK_ENTITY_TO_FUNC(ammo_cloaker, "ammo_cloaker", other_ammo_cloaker);
LINK_ENTITY_TO_FUNC(ammo_areaammo, "ammo_areaammo", other_ammo_areaammo);

// goldeneye 1.6
LINK_ENTITY_TO_FUNC(monster_mine, "monster_mine", other_monster_mine);
LINK_ENTITY_TO_FUNC(item_armor, "item_armor", other_item_armor);
LINK_ENTITY_TO_FUNC(knife_bolt, "knife_bolt", other_knife_bolt);
LINK_ENTITY_TO_FUNC(ammo_knife, "ammo_knife", other_ammo_knife);
LINK_ENTITY_TO_FUNC(weapon_pmine, "weapon_pmine", other_weapon_pmine);
LINK_ENTITY_TO_FUNC(weapon_rmine, "weapon_rmine", other_weapon_rmine);
LINK_ENTITY_TO_FUNC(guard_info, "guard_info", other_guard_info);
LINK_ENTITY_TO_FUNC(info_goldeneye, "info_goldeneye", other_info_goldeneye);
LINK_ENTITY_TO_FUNC(weapon_tmine, "weapon_tmine", other_weapon_tmine);
LINK_ENTITY_TO_FUNC(random_weapon, "random_weapon", other_random_weapon);
LINK_ENTITY_TO_FUNC(random_ammo, "random_ammo", other_random_ammo);
LINK_ENTITY_TO_FUNC(spy_escape, "spy_escape", other_spy_escape);

// goldeneye 1.5
LINK_ENTITY_TO_FUNC(monster_proximity, "monster_proximity", other_monster_proximity);
LINK_ENTITY_TO_FUNC(weapon_kf7, "weapon_kf7", other_weapon_kf7);
LINK_ENTITY_TO_FUNC(ammo_kf7clip, "ammo_kf7clip", other_ammo_kf7clip);
LINK_ENTITY_TO_FUNC(weapon_dd44, "weapon_dd44", other_weapon_dd44);
LINK_ENTITY_TO_FUNC(ammo_dd44clip, "ammo_dd44clip", other_ammo_dd44clip);
LINK_ENTITY_TO_FUNC(weapon_goldpp7, "weapon_goldpp7", other_weapon_goldpp7);
LINK_ENTITY_TO_FUNC(ammo_goldpp7clip, "ammo_goldpp7clip", other_ammo_goldpp7clip);
LINK_ENTITY_TO_FUNC(weapon_phantom, "weapon_phantom", other_weapon_phantom);
LINK_ENTITY_TO_FUNC(ammo_phantomclip, "ammo_phantomclip", other_ammo_phantomclip);
LINK_ENTITY_TO_FUNC(weapon_silverpp7, "weapon_silverpp7", other_weapon_silverpp7);
LINK_ENTITY_TO_FUNC(ammo_silverpp7clip, "ammo_silverpp7clip", other_ammo_silverpp7clip);
LINK_ENTITY_TO_FUNC(weapon_slappers, "weapon_slappers", other_weapon_slappers);
LINK_ENTITY_TO_FUNC(weapon_zmg, "weapon_zmg", other_weapon_zmg);
LINK_ENTITY_TO_FUNC(ammo_zmgclip, "ammo_zmgclip", other_ammo_zmgclip);

// golden eye 1.1b support
LINK_ENTITY_TO_FUNC(ammo_AK47clip, "ammo_AK47clip", other_ammo_AK47clip);
LINK_ENTITY_TO_FUNC(ammo_colt45clip, "ammo_colt45clip", other_ammo_colt45clip);
LINK_ENTITY_TO_FUNC(ammo_d5kclip, "ammo_d5kclip", other_ammo_d5kclip);
LINK_ENTITY_TO_FUNC(ammo_deagleclip, "ammo_deagleclip", other_ammo_deagleclip);
LINK_ENTITY_TO_FUNC(ammo_p99clip, "ammo_p99clip", other_ammo_p99clip);
LINK_ENTITY_TO_FUNC(ammo_rcp90clip, "ammo_rcp90clip", other_ammo_rcp90clip);
LINK_ENTITY_TO_FUNC(ammo_veprclip, "ammo_veprclip", other_ammo_veprclip);
LINK_ENTITY_TO_FUNC(carried_book_team1, "carried_book_team1", other_carried_book_team1);
LINK_ENTITY_TO_FUNC(carried_book_team2, "carried_book_team2", other_carried_book_team2);
LINK_ENTITY_TO_FUNC(info_mapdetect, "info_mapdetect", other_info_mapdetect);
LINK_ENTITY_TO_FUNC(item_book_team1, "item_book_team1", other_item_book_team1);
LINK_ENTITY_TO_FUNC(item_book_team2, "item_book_team2", other_item_book_team2);
LINK_ENTITY_TO_FUNC(spawnfar, "spawnfar", other_spawnfar);
LINK_ENTITY_TO_FUNC(weapon_colt45, "weapon_colt45", other_weapon_colt45);
LINK_ENTITY_TO_FUNC(weapon_d5k, "weapon_d5k", other_weapon_d5k);
LINK_ENTITY_TO_FUNC(weapon_p99, "weapon_p99", other_weapon_p99);
LINK_ENTITY_TO_FUNC(weapon_proximity, "weapon_proximity", other_weapon_proximity);
LINK_ENTITY_TO_FUNC(weapon_rcp90, "weapon_rcp90", other_weapon_rcp90);
LINK_ENTITY_TO_FUNC(weapon_triplaser, "weapon_triplaser", other_weapon_triplaser);
LINK_ENTITY_TO_FUNC(weapon_vepr, "weapon_vepr", other_weapon_vepr);

// Oz support
LINK_ENTITY_TO_FUNC(ctf_blueflag, "ctf_blueflag", other_ctf_blueflag);
LINK_ENTITY_TO_FUNC(ctf_bluespawn, "ctf_bluespawn", other_ctf_bluespawn);
LINK_ENTITY_TO_FUNC(ctf_cloak, "ctf_cloak", other_ctf_cloak);
LINK_ENTITY_TO_FUNC(ctf_crowbar, "ctf_crowbar", other_ctf_crowbar);
LINK_ENTITY_TO_FUNC(ctf_cycle, "ctf_cycle", other_ctf_cycle);
LINK_ENTITY_TO_FUNC(ctf_frag, "ctf_frag", other_ctf_frag);
LINK_ENTITY_TO_FUNC(ctf_hook, "ctf_hook", other_ctf_hook);
LINK_ENTITY_TO_FUNC(ctf_lowgrav, "ctf_lowgrav", other_ctf_lowgrav);
LINK_ENTITY_TO_FUNC(ctf_protect, "ctf_protect", other_ctf_protect);
LINK_ENTITY_TO_FUNC(ctf_redflag, "ctf_redflag", other_ctf_redflag);
LINK_ENTITY_TO_FUNC(ctf_redspawn, "ctf_redspawn", other_ctf_redspawn);
LINK_ENTITY_TO_FUNC(ctf_regen, "ctf_regen", other_ctf_regen);
LINK_ENTITY_TO_FUNC(ctf_superglock, "ctf_superglock", other_ctf_superglock);
LINK_ENTITY_TO_FUNC(ctf_trap, "ctf_trap", other_ctf_trap);
LINK_ENTITY_TO_FUNC(ctf_vamp, "ctf_vamp", other_ctf_vamp);
LINK_ENTITY_TO_FUNC(grapple_bolt, "grapple_bolt", other_grapple_bolt);

// more frontline entities (from 1.0) (yes, my bad with this)
LINK_ENTITY_TO_FUNC(trip_beam, "trip_beam", other_trip_beam);
LINK_ENTITY_TO_FUNC(weapon_mp5a2, "weapon_mp5a2", other_weapon_mp5a2);
LINK_ENTITY_TO_FUNC(weapon_mp5sd, "weapon_mp5sd", other_weapon_mp5sd);
LINK_ENTITY_TO_FUNC(info_player_attacker, "info_player_attacker", other_info_player_attacker);
LINK_ENTITY_TO_FUNC(info_player_defender, "info_player_defender", other_info_player_defender);
LINK_ENTITY_TO_FUNC(info_player_observer, "info_player_observer", other_info_player_observer);

// frontline entities
LINK_ENTITY_TO_FUNC(ammo_beretta, "ammo_beretta", other_ammo_beretta);
LINK_ENTITY_TO_FUNC(capture_point, "capture_point", other_capture_point);
LINK_ENTITY_TO_FUNC(ammo_mp5a2, "ammo_mp5a2", other_ammo_mp5a2);
LINK_ENTITY_TO_FUNC(ammo_mp5sd, "ammo_mp5sd", other_ammo_mp5sd);
LINK_ENTITY_TO_FUNC(gren_smoke, "gren_smoke", other_gren_smoke);
LINK_ENTITY_TO_FUNC(weapon_spas12, "weapon_spas12", other_weapon_spas12);
LINK_ENTITY_TO_FUNC(ammo_spas12, "ammo_spas12", other_ammo_spas12);
LINK_ENTITY_TO_FUNC(my_monster, "my_monster", other_my_monster);
LINK_ENTITY_TO_FUNC(weapon_ump45, "weapon_ump45", other_weapon_ump45);
LINK_ENTITY_TO_FUNC(ammo_ump45, "ammo_ump45", other_ammo_ump45);
LINK_ENTITY_TO_FUNC(info_frontline, "info_frontline", other_info_frontline);
LINK_ENTITY_TO_FUNC(weapon_clip_generic, "weapon_clip_generic", other_weapon_clip_generic);
// frontline 0.46 new ents
LINK_ENTITY_TO_FUNC(weapon_hk21, "weapon_hk21", other_weapon_hk21);
LINK_ENTITY_TO_FUNC(ammo_hk21, "ammo_hk21", other_ammo_hk21);
LINK_ENTITY_TO_FUNC(weapon_mk23, "weapon_mk23", other_weapon_mk23);
LINK_ENTITY_TO_FUNC(ammo_mk23, "ammo_mk23", other_ammo_mk23);
LINK_ENTITY_TO_FUNC(weapon_msg90, "weapon_msg90", other_weapon_msg90);
LINK_ENTITY_TO_FUNC(ammo_msg90, "ammo_msg90", other_ammo_msg90);

// new 0.49 ents
LINK_ENTITY_TO_FUNC(weapon_ak5, "weapon_ak5", other_weapon_ak5);
LINK_ENTITY_TO_FUNC(ammo_ak5, "ammo_ak5", other_ammo_ak5);

// s&i entities
LINK_ENTITY_TO_FUNC(ammo_mindrayclip, "ammo_mindrayclip", other_ammo_mindrayclip);
LINK_ENTITY_TO_FUNC(ammo_vomitclip, "ammo_vomitclip", other_ammo_vomitclip);
LINK_ENTITY_TO_FUNC(info_administrator, "info_administrator", other_info_administrator);
LINK_ENTITY_TO_FUNC(info_scientist, "info_scientist", other_info_scientist);
LINK_ENTITY_TO_FUNC(info_scientist_dead, "info_scientist_dead", other_info_scientist_dead);
LINK_ENTITY_TO_FUNC(info_scientist_start, "info_scientist_start", other_info_scientist_start);
LINK_ENTITY_TO_FUNC(info_transistor, "info_transistor", other_info_transistor);
LINK_ENTITY_TO_FUNC(weapon_mindray, "weapon_mindray", other_weapon_mindray);
LINK_ENTITY_TO_FUNC(weapon_radio, "weapon_radio", other_weapon_radio);
LINK_ENTITY_TO_FUNC(weapon_transistor, "weapon_transistor", other_weapon_transistor);
LINK_ENTITY_TO_FUNC(weapon_vomit, "weapon_vomit", other_weapon_vomit);

// gansta wars 2.6
LINK_ENTITY_TO_FUNC(bot_waypoint, "bot_waypoint", other_bot_waypoint);
LINK_ENTITY_TO_FUNC(weapon_glock17, "weapon_glock17", other_weapon_glock17);
LINK_ENTITY_TO_FUNC(weapon_smkgrenade, "weapon_smkgrenade", other_weapon_smkgrenade);
LINK_ENTITY_TO_FUNC(weapon_baretta, "weapon_baretta", other_weapon_baretta);
LINK_ENTITY_TO_FUNC(ammo_baretta, "ammo_baretta", other_ammo_baretta);
LINK_ENTITY_TO_FUNC(weapon_ak101, "weapon_ak101", other_weapon_ak101);
LINK_ENTITY_TO_FUNC(ammo_ak101, "ammo_ak101", other_ammo_ak101);
LINK_ENTITY_TO_FUNC(weapon_akimbo_glocks, "weapon_akimbo_glocks", other_weapon_akimbo_glocks);
LINK_ENTITY_TO_FUNC(weapon_bat, "weapon_bat", other_weapon_bat);
LINK_ENTITY_TO_FUNC(weapon_baton, "weapon_baton", other_weapon_baton);
LINK_ENTITY_TO_FUNC(item_briefcase, "item_briefcase", other_item_briefcase);
LINK_ENTITY_TO_FUNC(item_cash, "item_cash", other_item_cash);
LINK_ENTITY_TO_FUNC(weapon_colt, "weapon_colt", other_weapon_colt);
LINK_ENTITY_TO_FUNC(ammo_colt, "ammo_colt", other_ammo_colt);
LINK_ENTITY_TO_FUNC(weapon_dblshot, "weapon_dblshot", other_weapon_dblshot);
LINK_ENTITY_TO_FUNC(weapon_dbshot, "weapon_dbshot", other_weapon_dbshot);
LINK_ENTITY_TO_FUNC(ammo_dbshot, "ammo_dbshot", other_ammo_dbshot);
LINK_ENTITY_TO_FUNC(info_gwars_map, "info_gwars_map", other_info_gwars_map);
LINK_ENTITY_TO_FUNC(weapon_mg36, "weapon_mg36", other_weapon_mg36);
LINK_ENTITY_TO_FUNC(weapon_g36k, "weapon_g36k", other_weapon_g36k);
LINK_ENTITY_TO_FUNC(ammo_mg36, "ammo_mg36", other_ammo_mg36);
LINK_ENTITY_TO_FUNC(ammo_g36k, "ammo_g36k", other_ammo_g36k);
LINK_ENTITY_TO_FUNC(ammo_glock17, "ammo_glock17", other_ammo_glock17);
LINK_ENTITY_TO_FUNC(weapon_longslide, "weapon_longslide", other_weapon_longslide);
LINK_ENTITY_TO_FUNC(ammo_longslide, "ammo_longslide", other_ammo_longslide);
LINK_ENTITY_TO_FUNC(weapon_mac, "weapon_mac", other_weapon_mac);
LINK_ENTITY_TO_FUNC(ammo_mac, "ammo_mac", other_ammo_mac);
LINK_ENTITY_TO_FUNC(ammo_mac10, "ammo_mac10", other_ammo_mac10);
LINK_ENTITY_TO_FUNC(weapon_mossberg, "weapon_mossberg", other_weapon_mossberg);
LINK_ENTITY_TO_FUNC(ammo_mossberg, "ammo_mossberg", other_ammo_mossberg);
LINK_ENTITY_TO_FUNC(info_gangsta_dm_start, "info_gangsta_dm_start", other_info_gangsta_dm_start);
LINK_ENTITY_TO_FUNC(info_italian_start, "info_italian_start", other_info_italian_start);
LINK_ENTITY_TO_FUNC(info_russian_start, "info_russian_start", other_info_russian_start);
LINK_ENTITY_TO_FUNC(deal_italian_start, "deal_italian_start", other_deal_italian_start);
LINK_ENTITY_TO_FUNC(deal_columbian_start, "deal_columbian_start", other_deal_columbian_start);
LINK_ENTITY_TO_FUNC(deal_swat_start, "deal_swat_start", other_deal_swat_start);
LINK_ENTITY_TO_FUNC(weapon_tec9, "weapon_tec9", other_weapon_tec9);
LINK_ENTITY_TO_FUNC(ammo_tec9, "ammo_tec9", other_ammo_tec9);
LINK_ENTITY_TO_FUNC(item_flag1, "item_flag1", other_item_flag1);
LINK_ENTITY_TO_FUNC(item_flag2, "item_flag2", other_item_flag2);
LINK_ENTITY_TO_FUNC(weapon_tommygun, "weapon_tommygun", other_weapon_tommygun);
LINK_ENTITY_TO_FUNC(ammo_tommy, "ammo_tommy", other_ammo_tommy);
LINK_ENTITY_TO_FUNC(ammo_tommygun, "ammo_tommygun", other_ammo_tommygun);
LINK_ENTITY_TO_FUNC(turf_zone, "turf_zone", other_turf_zone);
LINK_ENTITY_TO_FUNC(weapon_uzi, "weapon_uzi", other_weapon_uzi);
LINK_ENTITY_TO_FUNC(ammo_uzi, "ammo_uzi", other_ammo_uzi);

// arg 1.0 entities
LINK_ENTITY_TO_FUNC(item_beans, "item_beans", other_item_beans);
LINK_ENTITY_TO_FUNC(BALLS_airball, "BALLS_airball", other_BALLS_airball);
LINK_ENTITY_TO_FUNC(weapon_discoballs, "weapon_discoballs", other_weapon_discoballs);
LINK_ENTITY_TO_FUNC(ammo_BALLS, "ammo_BALLS", other_ammo_BALLS);
LINK_ENTITY_TO_FUNC(weapon_ga2uss, "weapon_ga2uss", other_weapon_ga2uss);
LINK_ENTITY_TO_FUNC(weapon_holybsword, "weapon_holybsword", other_weapon_holybsword);
LINK_ENTITY_TO_FUNC(weapon_aandagger, "weapon_aandagger", other_weapon_aandagger);
LINK_ENTITY_TO_FUNC(weapon_musket, "weapon_musket", other_weapon_musket);
LINK_ENTITY_TO_FUNC(weapon_rumgun, "weapon_rumgun", other_weapon_rumgun);
LINK_ENTITY_TO_FUNC(monster_llama, "monster_llama", other_monster_llama);
LINK_ENTITY_TO_FUNC(weapon_tucan, "weapon_tucan", other_weapon_tucan);

// arg 0.50
LINK_ENTITY_TO_FUNC(weapon_cocogrenade, "weapon_cocogrenade", other_weapon_cocogrenade);
LINK_ENTITY_TO_FUNC(weapon_not_in_use_grenade, "weapon_not_in_use_grenade", other_weapon_not_in_use_grenade);
LINK_ENTITY_TO_FUNC(item_antidotee, "item_antidotee", other_item_antidotee);
LINK_ENTITY_TO_FUNC(hallucgrenade, "hallucgrenade", other_hallucgrenade);
LINK_ENTITY_TO_FUNC(weapon_tdagger, "weapon_tdagger", other_weapon_tdagger);
LINK_ENTITY_TO_FUNC(weapon_longsword, "weapon_longsword", other_weapon_longsword);
LINK_ENTITY_TO_FUNC(weapon_stupid, "weapon_stupid", other_weapon_stupid);
LINK_ENTITY_TO_FUNC(FOOT_bolt, "FOOT_bolt", other_FOOT_bolt);
LINK_ENTITY_TO_FUNC(ammo_FOOT, "ammo_FOOT", other_ammo_FOOT);
LINK_ENTITY_TO_FUNC(weapon_flute, "weapon_flute", other_weapon_flute);
LINK_ENTITY_TO_FUNC(weapon_ndagger, "weapon_ndagger", other_weapon_ndagger);
LINK_ENTITY_TO_FUNC(FISHINGROD_bolt, "FISHINGROD_bolt", other_FISHINGROD_bolt);
LINK_ENTITY_TO_FUNC(weapon_fishingrod, "weapon_fishingrod", other_weapon_fishingrod);
LINK_ENTITY_TO_FUNC(ammo_FISHINGROD, "ammo_FISHINGROD", other_ammo_FISHINGROD);
LINK_ENTITY_TO_FUNC(weapon_bsword, "weapon_bsword", other_weapon_bsword);
LINK_ENTITY_TO_FUNC(whiskey_whisk, "whiskey_whisk", other_whiskey_whisk);
LINK_ENTITY_TO_FUNC(weapon_whiskey, "weapon_whiskey", other_weapon_whiskey);
LINK_ENTITY_TO_FUNC(ammo_whiskey, "ammo_whiskey", other_ammo_whiskey);
LINK_ENTITY_TO_FUNC(weapon_mop, "weapon_mop", other_weapon_mop);

// wizard wars beta 2
LINK_ENTITY_TO_FUNC(monster_balllightning, "monster_balllightning", other_monster_balllightning);
LINK_ENTITY_TO_FUNC(proj_lightningcloud, "proj_lightningcloud", other_proj_lightningcloud);
LINK_ENTITY_TO_FUNC(proj_lightningsatchel, "proj_lightningsatchel", other_proj_lightningsatchel);
LINK_ENTITY_TO_FUNC(weapon_balllightningspell, "weapon_balllightningspell", other_weapon_balllightningspell);
LINK_ENTITY_TO_FUNC(weapon_lightningboltspell, "weapon_lightningboltspell", other_weapon_lightningboltspell);
LINK_ENTITY_TO_FUNC(weapon_lightningcloudspell, "weapon_lightningcloudspell", other_weapon_lightningcloudspell);

LINK_ENTITY_TO_FUNC(ammo_satchels, "ammo_satchels", other_ammo_satchels);
LINK_ENTITY_TO_FUNC(ammo_spellbook, "ammo_spellbook", other_ammo_spellbook);
LINK_ENTITY_TO_FUNC(beanstalk, "beanstalk", other_beanstalk);
LINK_ENTITY_TO_FUNC(monster_giantplant, "monster_giantplant", other_monster_giantplant);
LINK_ENTITY_TO_FUNC(monster_skeleton, "monster_skeleton", other_monster_skeleton);
LINK_ENTITY_TO_FUNC(monster_spiritwiz, "monster_spiritwiz", other_monster_spiritwiz);
LINK_ENTITY_TO_FUNC(monster_thornbush, "monster_thornbush", other_monster_thornbush);
LINK_ENTITY_TO_FUNC(monster_tornado, "monster_tornado", other_monster_tornado);
LINK_ENTITY_TO_FUNC(monster_wizardclone, "monster_wizardclone", other_monster_wizardclone);
LINK_ENTITY_TO_FUNC(monster_wombat, "monster_wombat", other_monster_wombat);
LINK_ENTITY_TO_FUNC(proj_bird, "proj_bird", other_proj_bird);
LINK_ENTITY_TO_FUNC(proj_bursatchel, "proj_bursatchel", other_proj_bursatchel);
LINK_ENTITY_TO_FUNC(proj_cocklebur, "proj_cocklebur", other_proj_cocklebur);
LINK_ENTITY_TO_FUNC(proj_comet, "proj_comet", other_proj_comet);
LINK_ENTITY_TO_FUNC(proj_doublemagicmissle, "proj_doublemagicmissle", other_proj_doublemagicmissle);
LINK_ENTITY_TO_FUNC(proj_earthquakesatchel, "proj_earthquakesatchel", other_proj_earthquakesatchel);
LINK_ENTITY_TO_FUNC(proj_fireball, "proj_fireball", other_proj_fireball);
LINK_ENTITY_TO_FUNC(proj_firesatchel, "proj_firesatchel", other_proj_firesatchel);
LINK_ENTITY_TO_FUNC(proj_firespiral, "proj_firespiral", other_proj_firespiral);
LINK_ENTITY_TO_FUNC(proj_fissure, "proj_fissure", other_proj_fissure);
LINK_ENTITY_TO_FUNC(proj_flyingskull, "proj_flyingskull", other_proj_flyingskull);
LINK_ENTITY_TO_FUNC(proj_healhurtsatchel, "proj_healhurtsatchel", other_proj_healhurtsatchel);
LINK_ENTITY_TO_FUNC(proj_icepoke, "proj_icepoke", other_proj_icepoke);
LINK_ENTITY_TO_FUNC(proj_magicmissle, "proj_magicmissle", other_proj_magicmissle);
LINK_ENTITY_TO_FUNC(proj_meteor, "proj_meteor", other_proj_meteor);
LINK_ENTITY_TO_FUNC(proj_mindmissle, "proj_mindmissle", other_proj_mindmissle);
LINK_ENTITY_TO_FUNC(proj_poisonsatchel, "proj_poisonsatchel", other_proj_poisonsatchel);
LINK_ENTITY_TO_FUNC(proj_rollingstone, "proj_rollingstone", other_proj_rollingstone);
LINK_ENTITY_TO_FUNC(proj_suctionsatchel, "proj_suctionsatchel", other_proj_suctionsatchel);
LINK_ENTITY_TO_FUNC(proj_tcrystal, "proj_tcrystal", other_proj_tcrystal);
LINK_ENTITY_TO_FUNC(proj_throwingbone, "proj_throwingbone", other_proj_throwingbone);
LINK_ENTITY_TO_FUNC(proj_whirlwind, "proj_whirlwind", other_proj_whirlwind);
LINK_ENTITY_TO_FUNC(slAGruntTakeCoverFromEnemy, "slAGruntTakeCoverFromEnemy", other_slAGruntTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(slAGruntThreatDisplay, "slAGruntThreatDisplay", other_slAGruntThreatDisplay);
LINK_ENTITY_TO_FUNC(slAssassinTakeCoverFromBestSound,
    "slAssassinTakeCoverFromBestSound",
    other_slAssassinTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(slAssassinTakeCoverFromEnemy, "slAssassinTakeCoverFromEnemy", other_slAssassinTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(slAssassinTakeCoverFromEnemy2,
    "slAssassinTakeCoverFromEnemy2",
    other_slAssassinTakeCoverFromEnemy2);
LINK_ENTITY_TO_FUNC(slBaFaceTarget, "slBaFaceTarget", other_slBaFaceTarget);
LINK_ENTITY_TO_FUNC(slControllerTakeCover, "slControllerTakeCover", other_slControllerTakeCover);
LINK_ENTITY_TO_FUNC(slFaceTarget, "slFaceTarget", other_slFaceTarget);
LINK_ENTITY_TO_FUNC(slFaceTargetScared, "slFaceTargetScared", other_slFaceTargetScared);
LINK_ENTITY_TO_FUNC(slGruntTakeCover, "slGruntTakeCover", other_slGruntTakeCover);
LINK_ENTITY_TO_FUNC(slGruntTakeCoverFromBestSound,
    "slGruntTakeCoverFromBestSound",
    other_slGruntTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(slGruntTossGrenadeCover, "slGruntTossGrenadeCover", other_slGruntTossGrenadeCover);
LINK_ENTITY_TO_FUNC(slIdleTrigger, "slIdleTrigger", other_slIdleTrigger);
LINK_ENTITY_TO_FUNC(slRunToScript, "slRunToScript", other_slRunToScript);
LINK_ENTITY_TO_FUNC(slTakeCoverFromBestSound, "slTakeCoverFromBestSound", other_slTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(slTakeCoverFromEnemy, "slTakeCoverFromEnemy", other_slTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(slTakeCoverFromOrigin, "slTakeCoverFromOrigin", other_slTakeCoverFromOrigin);
LINK_ENTITY_TO_FUNC(slTlkIdleEyecontact, "slTlkIdleEyecontact", other_slTlkIdleEyecontact);
LINK_ENTITY_TO_FUNC(slTlkIdleWatchClient, "slTlkIdleWatchClient", other_slTlkIdleWatchClient);
LINK_ENTITY_TO_FUNC(slTwitchDie, "slTwitchDie", other_slTwitchDie);
LINK_ENTITY_TO_FUNC(slWalkToScript, "slWalkToScript", other_slWalkToScript);
LINK_ENTITY_TO_FUNC(tlAGruntTakeCoverFromEnemy, "tlAGruntTakeCoverFromEnemy", other_tlAGruntTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(tlAGruntThreatDisplay, "tlAGruntThreatDisplay", other_tlAGruntThreatDisplay);
LINK_ENTITY_TO_FUNC(tlAssassinTakeCoverFromBestSound,
    "tlAssassinTakeCoverFromBestSound",
    other_tlAssassinTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(tlAssassinTakeCoverFromEnemy, "tlAssassinTakeCoverFromEnemy", other_tlAssassinTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(tlAssassinTakeCoverFromEnemy2,
    "tlAssassinTakeCoverFromEnemy2",
    other_tlAssassinTakeCoverFromEnemy2);
LINK_ENTITY_TO_FUNC(tlBaFaceTarget, "tlBaFaceTarget", other_tlBaFaceTarget);
LINK_ENTITY_TO_FUNC(tlControllerTakeCover, "tlControllerTakeCover", other_tlControllerTakeCover);
LINK_ENTITY_TO_FUNC(tlFaceTarget, "tlFaceTarget", other_tlFaceTarget);
LINK_ENTITY_TO_FUNC(tlFaceTargetScared, "tlFaceTargetScared", other_tlFaceTargetScared);
LINK_ENTITY_TO_FUNC(tlGruntTakeCover1, "tlGruntTakeCover1", other_tlGruntTakeCover1);
LINK_ENTITY_TO_FUNC(tlGruntTakeCoverFromBestSound,
    "tlGruntTakeCoverFromBestSound",
    other_tlGruntTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(tlGruntTossGrenadeCover1, "tlGruntTossGrenadeCover1", other_tlGruntTossGrenadeCover1);
LINK_ENTITY_TO_FUNC(tlTakeCoverFromBestSound, "tlTakeCoverFromBestSound", other_tlTakeCoverFromBestSound);
LINK_ENTITY_TO_FUNC(tlTakeCoverFromEnemy, "tlTakeCoverFromEnemy", other_tlTakeCoverFromEnemy);
LINK_ENTITY_TO_FUNC(tlTakeCoverFromOrigin, "tlTakeCoverFromOrigin", other_tlTakeCoverFromOrigin);
LINK_ENTITY_TO_FUNC(tlTlkIdleEyecontact, "tlTlkIdleEyecontact", other_tlTlkIdleEyecontact);
LINK_ENTITY_TO_FUNC(tlTlkIdleWatchClient, "tlTlkIdleWatchClient", other_tlTlkIdleWatchClient);
LINK_ENTITY_TO_FUNC(tlTlkIdleWatchClientStare, "tlTlkIdleWatchClientStare", other_tlTlkIdleWatchClientStare);
LINK_ENTITY_TO_FUNC(tlTwitchDie, "tlTwitchDie", other_tlTwitchDie);
LINK_ENTITY_TO_FUNC(weapon_beanstalkspell, "weapon_beanstalkspell", other_weapon_beanstalkspell);
LINK_ENTITY_TO_FUNC(weapon_bearbite, "weapon_bearbite", other_weapon_bearbite);
LINK_ENTITY_TO_FUNC(weapon_bearclaw, "weapon_bearclaw", other_weapon_bearclaw);
LINK_ENTITY_TO_FUNC(weapon_birdspell, "weapon_birdspell", other_weapon_birdspell);
LINK_ENTITY_TO_FUNC(weapon_cometspell, "weapon_cometspell", other_weapon_cometspell);
LINK_ENTITY_TO_FUNC(weapon_deathrayspell, "weapon_deathrayspell", other_weapon_deathrayspell);
LINK_ENTITY_TO_FUNC(weapon_doublemagicmisslespell,
    "weapon_doublemagicmisslespell",
    other_weapon_doublemagicmisslespell);
LINK_ENTITY_TO_FUNC(weapon_fireballspell, "weapon_fireballspell", other_weapon_fireballspell);
LINK_ENTITY_TO_FUNC(weapon_flamelickspell, "weapon_flamelickspell", other_weapon_flamelickspell);
LINK_ENTITY_TO_FUNC(weapon_forcespell, "weapon_forcespell", other_weapon_forcespell);
LINK_ENTITY_TO_FUNC(weapon_giantplantspell, "weapon_giantplantspell", other_weapon_giantplantspell);
LINK_ENTITY_TO_FUNC(weapon_icepokespell, "weapon_icepokespell", other_weapon_icepokespell);
LINK_ENTITY_TO_FUNC(weapon_levitationspell, "weapon_levitationspell", other_weapon_levitationspell);
LINK_ENTITY_TO_FUNC(weapon_magicmisslespell, "weapon_magicmisslespell", other_weapon_magicmisslespell);
LINK_ENTITY_TO_FUNC(weapon_meteorspell, "weapon_meteorspell", other_weapon_meteorspell);
LINK_ENTITY_TO_FUNC(weapon_mindmisslespell, "weapon_mindmisslespell", other_weapon_mindmisslespell);
LINK_ENTITY_TO_FUNC(weapon_rollingstonespell, "weapon_rollingstonespell", other_weapon_rollingstonespell);
LINK_ENTITY_TO_FUNC(weapon_skeletonspell, "weapon_skeletonspell", other_weapon_skeletonspell);
LINK_ENTITY_TO_FUNC(weapon_skullspell, "weapon_skullspell", other_weapon_skullspell);
LINK_ENTITY_TO_FUNC(weapon_spiritwizspell, "weapon_spiritwizspell", other_weapon_spiritwizspell);
LINK_ENTITY_TO_FUNC(weapon_spotboltspell, "weapon_spotboltspell", other_weapon_spotboltspell);
LINK_ENTITY_TO_FUNC(weapon_staff, "weapon_staff", other_weapon_staff);
LINK_ENTITY_TO_FUNC(weapon_thornblastspell, "weapon_thornblastspell", other_weapon_thornblastspell);
LINK_ENTITY_TO_FUNC(weapon_tornadospell, "weapon_tornadospell", other_weapon_tornadospell);
LINK_ENTITY_TO_FUNC(weapon_whirlwindspell, "weapon_whirlwindspell", other_weapon_whirlwindspell);
LINK_ENTITY_TO_FUNC(weapon_whiterayspell, "weapon_whiterayspell", other_weapon_whiterayspell);
LINK_ENTITY_TO_FUNC(weapon_wombatspell, "weapon_wombatspell", other_weapon_wombatspell);

// swarm (?? version)
LINK_ENTITY_TO_FUNC(ammo_adrenaline, "ammo_adrenaline", other_ammo_adrenaline);
LINK_ENTITY_TO_FUNC(ammo_needle, "ammo_needle", other_ammo_needle);
LINK_ENTITY_TO_FUNC(ammo_swarmprimary, "ammo_swarmprimary", other_ammo_swarmprimary);
LINK_ENTITY_TO_FUNC(ammo_swarmsecondary, "ammo_swarmsecondary", other_ammo_swarmsecondary);
LINK_ENTITY_TO_FUNC(ammo_swarm_xen_secondary, "ammo_swarm_xen_secondary", other_ammo_swarm_xen_secondary);
LINK_ENTITY_TO_FUNC(garg_hull, "garg_hull", other_garg_hull);
LINK_ENTITY_TO_FUNC(grapple_hook, "grapple_hook", other_grapple_hook);
LINK_ENTITY_TO_FUNC(item_swarm_flag, "item_swarm_flag", other_item_swarm_flag);
LINK_ENTITY_TO_FUNC(monster_c4, "monster_c4", other_monster_c4);
LINK_ENTITY_TO_FUNC(swarm_garg_stomp, "swarm_garg_stomp", other_swarm_garg_stomp);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_acontroller, "swarm_spawnpt_acontroller", other_swarm_spawnpt_acontroller);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_agarg, "swarm_spawnpt_agarg", other_swarm_spawnpt_agarg);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_aheadcrab, "swarm_spawnpt_aheadcrab", other_swarm_spawnpt_aheadcrab);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_aslave, "swarm_spawnpt_aslave", other_swarm_spawnpt_aslave);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_atripod, "swarm_spawnpt_atripod", other_swarm_spawnpt_atripod);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hassassin, "swarm_spawnpt_hassassin", other_swarm_spawnpt_hassassin);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hcommander, "swarm_spawnpt_hcommander", other_swarm_spawnpt_hcommander);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hengineer, "swarm_spawnpt_hengineer", other_swarm_spawnpt_hengineer);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hgrunt, "swarm_spawnpt_hgrunt", other_swarm_spawnpt_hgrunt);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_hscientist, "swarm_spawnpt_hscientist", other_swarm_spawnpt_hscientist);
LINK_ENTITY_TO_FUNC(swarm_spawnpt_human, "swarm_spawnpt_human", other_swarm_spawnpt_human);
LINK_ENTITY_TO_FUNC(swarm_team_alien, "swarm_team_alien", other_swarm_team_alien);
LINK_ENTITY_TO_FUNC(swarm_team_human, "swarm_team_human", other_swarm_team_human);
LINK_ENTITY_TO_FUNC(trigger_regen, "trigger_regen", other_trigger_regen);
LINK_ENTITY_TO_FUNC(weapon_adrenaline, "weapon_adrenaline", other_weapon_adrenaline);
LINK_ENTITY_TO_FUNC(weapon_eballblaster, "weapon_eballblaster", other_weapon_eballblaster);
LINK_ENTITY_TO_FUNC(weapon_islavezap, "weapon_islavezap", other_weapon_islavezap);
LINK_ENTITY_TO_FUNC(weapon_needle, "weapon_needle", other_weapon_needle);
LINK_ENTITY_TO_FUNC(weapon_pipebomb, "weapon_pipebomb", other_weapon_pipebomb);
LINK_ENTITY_TO_FUNC(weapon_sonicgrenade, "weapon_sonicgrenade", other_weapon_sonicgrenade);
LINK_ENTITY_TO_FUNC(weapon_swarm_gargflame, "weapon_swarm_gargflame", other_weapon_swarm_gargflame);

// entities for Opposing Force
LINK_ENTITY_TO_FUNC(ammo_556, "ammo_556", other_ammo_556);
LINK_ENTITY_TO_FUNC(ammo_762, "ammo_762", other_ammo_762);
LINK_ENTITY_TO_FUNC(ammo_eagleclip, "ammo_eagleclip", other_ammo_eagleclip);
LINK_ENTITY_TO_FUNC(ammo_spore, "ammo_spore", other_ammo_spore);
LINK_ENTITY_TO_FUNC(charged_bolt, "charged_bolt", other_charged_bolt);
LINK_ENTITY_TO_FUNC(displacer_ball, "displacer_ball", other_displacer_ball);
LINK_ENTITY_TO_FUNC(eagle_laser, "eagle_laser", other_eagle_laser);
LINK_ENTITY_TO_FUNC(env_blowercannon, "env_blowercannon", other_env_blowercannon);
LINK_ENTITY_TO_FUNC(env_electrified_wire, "env_electrified_wire", other_env_electrified_wire);
LINK_ENTITY_TO_FUNC(env_genewormcloud, "env_genewormcloud", other_env_genewormcloud);
LINK_ENTITY_TO_FUNC(env_genewormspawn, "env_genewormspawn", other_env_genewormspawn);
LINK_ENTITY_TO_FUNC(env_rope, "env_rope", other_env_rope);
LINK_ENTITY_TO_FUNC(env_spritetrain, "env_spritetrain", other_env_spritetrain);
LINK_ENTITY_TO_FUNC(func_op4mortarcontroller, "func_op4mortarcontroller", other_func_op4mortarcontroller);
LINK_ENTITY_TO_FUNC(func_tank_of, "func_tank_of", other_func_tank_of);
LINK_ENTITY_TO_FUNC(func_tankcontrols_of, "func_tankcontrols_of", other_func_tankcontrols_of);
LINK_ENTITY_TO_FUNC(func_tanklaser_of, "func_tanklaser_of", other_func_tanklaser_of);
LINK_ENTITY_TO_FUNC(func_tankmortar_of, "func_tankmortar_of", other_func_tankmortar_of);
LINK_ENTITY_TO_FUNC(func_tankrocket_of, "func_tankrocket_of", other_func_tankrocket_of);
LINK_ENTITY_TO_FUNC(gonomeguts, "gonomeguts", other_gonomeguts);
LINK_ENTITY_TO_FUNC(grapple_tip, "grapple_tip", other_grapple_tip);
LINK_ENTITY_TO_FUNC(hvr_blkop_rocket, "hvr_blkop_rocket", other_hvr_blkop_rocket);
LINK_ENTITY_TO_FUNC(info_ctfdetect, "info_ctfdetect", other_info_ctfdetect);
LINK_ENTITY_TO_FUNC(info_ctfspawn, "info_ctfspawn", other_info_ctfspawn);
LINK_ENTITY_TO_FUNC(info_ctfspawn_powerup, "info_ctfspawn_powerup", other_info_ctfspawn_powerup);
LINK_ENTITY_TO_FUNC(info_displacer_earth_target, "info_displacer_earth_target", other_info_displacer_earth_target);
LINK_ENTITY_TO_FUNC(info_displacer_xen_target, "info_displacer_xen_target", other_info_displacer_xen_target);
LINK_ENTITY_TO_FUNC(info_pitworm, "info_pitworm", other_info_pitworm);
LINK_ENTITY_TO_FUNC(info_pitworm_steam_lock, "info_pitworm_steam_lock", other_info_pitworm_steam_lock);
LINK_ENTITY_TO_FUNC(item_ctfaccelerator, "item_ctfaccelerator", other_item_ctfaccelerator);
LINK_ENTITY_TO_FUNC(item_ctfbackpack, "item_ctfbackpack", other_item_ctfbackpack);
LINK_ENTITY_TO_FUNC(item_ctfbase, "item_ctfbase", other_item_ctfbase);
LINK_ENTITY_TO_FUNC(item_ctfflag, "item_ctfflag", other_item_ctfflag);
LINK_ENTITY_TO_FUNC(item_ctflongjump, "item_ctflongjump", other_item_ctflongjump);
LINK_ENTITY_TO_FUNC(item_ctfportablehev, "item_ctfportablehev", other_item_ctfportablehev);
LINK_ENTITY_TO_FUNC(item_ctfregeneration, "item_ctfregeneration", other_item_ctfregeneration);
LINK_ENTITY_TO_FUNC(item_generic, "item_generic", other_item_generic);
LINK_ENTITY_TO_FUNC(item_nuclearbomb, "item_nuclearbomb", other_item_nuclearbomb);
LINK_ENTITY_TO_FUNC(item_nuclearbombbutton, "item_nuclearbombbutton", other_item_nuclearbombbutton);
LINK_ENTITY_TO_FUNC(item_nuclearbombtimer, "item_nuclearbombtimer", other_item_nuclearbombtimer);
LINK_ENTITY_TO_FUNC(monster_ShockTrooper_dead, "monster_ShockTrooper_dead", other_monster_ShockTrooper_dead);
LINK_ENTITY_TO_FUNC(monster_alien_babyvoltigore, "monster_alien_babyvoltigore", other_monster_alien_babyvoltigore);
LINK_ENTITY_TO_FUNC(monster_alien_slave_dead, "monster_alien_slave_dead", other_monster_alien_slave_dead);
LINK_ENTITY_TO_FUNC(monster_alien_voltigore, "monster_alien_voltigore", other_monster_alien_voltigore);
LINK_ENTITY_TO_FUNC(monster_assassin_repel, "monster_assassin_repel", other_monster_assassin_repel);
LINK_ENTITY_TO_FUNC(monster_blkop_apache, "monster_blkop_apache", other_monster_blkop_apache);
LINK_ENTITY_TO_FUNC(monster_blkop_osprey, "monster_blkop_osprey", other_monster_blkop_osprey);
LINK_ENTITY_TO_FUNC(monster_cleansuit_scientist, "monster_cleansuit_scientist", other_monster_cleansuit_scientist);
LINK_ENTITY_TO_FUNC(monster_cleansuit_scientist_dead,
    "monster_cleansuit_scientist_dead",
    other_monster_cleansuit_scientist_dead);
LINK_ENTITY_TO_FUNC(monster_drillsergeant, "monster_drillsergeant", other_monster_drillsergeant);
LINK_ENTITY_TO_FUNC(monster_fgrunt_repel, "monster_fgrunt_repel", other_monster_fgrunt_repel);
LINK_ENTITY_TO_FUNC(monster_geneworm, "monster_geneworm", other_monster_geneworm);
LINK_ENTITY_TO_FUNC(monster_gonome, "monster_gonome", other_monster_gonome);
LINK_ENTITY_TO_FUNC(monster_gonome_dead, "monster_gonome_dead", other_monster_gonome_dead);
LINK_ENTITY_TO_FUNC(monster_grunt_ally_repel, "monster_grunt_ally_repel", other_monster_grunt_ally_repel);
LINK_ENTITY_TO_FUNC(monster_hfgrunt_dead, "monster_hfgrunt_dead", other_monster_hfgrunt_dead);
LINK_ENTITY_TO_FUNC(monster_houndeye_dead, "monster_houndeye_dead", other_monster_houndeye_dead);
LINK_ENTITY_TO_FUNC(monster_human_friendly_grunt, "monster_human_friendly_grunt", other_monster_human_friendly_grunt);
LINK_ENTITY_TO_FUNC(monster_human_grunt_ally, "monster_human_grunt_ally", other_monster_human_grunt_ally);
LINK_ENTITY_TO_FUNC(monster_human_grunt_ally_dead,
    "monster_human_grunt_ally_dead",
    other_monster_human_grunt_ally_dead);
LINK_ENTITY_TO_FUNC(monster_human_medic_ally, "monster_human_medic_ally", other_monster_human_medic_ally);
LINK_ENTITY_TO_FUNC(monster_human_torch_ally, "monster_human_torch_ally", other_monster_human_torch_ally);
LINK_ENTITY_TO_FUNC(monster_male_assassin, "monster_male_assassin", other_monster_male_assassin);
LINK_ENTITY_TO_FUNC(monster_massassin_dead, "monster_massassin_dead", other_monster_massassin_dead);
LINK_ENTITY_TO_FUNC(monster_medic_ally_repel, "monster_medic_ally_repel", other_monster_medic_ally_repel);
LINK_ENTITY_TO_FUNC(monster_op4loader, "monster_op4loader", other_monster_op4loader);
LINK_ENTITY_TO_FUNC(monster_otis, "monster_otis", other_monster_otis);
LINK_ENTITY_TO_FUNC(monster_otis_dead, "monster_otis_dead", other_monster_otis_dead);
LINK_ENTITY_TO_FUNC(monster_penguin, "monster_penguin", other_monster_penguin);
LINK_ENTITY_TO_FUNC(monster_pitdrone, "monster_pitdrone", other_monster_pitdrone);
LINK_ENTITY_TO_FUNC(monster_pitworm, "monster_pitworm", other_monster_pitworm);
LINK_ENTITY_TO_FUNC(monster_pitworm_up, "monster_pitworm_up", other_monster_pitworm_up);
LINK_ENTITY_TO_FUNC(monster_recruit, "monster_recruit", other_monster_recruit);
LINK_ENTITY_TO_FUNC(monster_shockroach, "monster_shockroach", other_monster_shockroach);
LINK_ENTITY_TO_FUNC(monster_shocktrooper, "monster_shocktrooper", other_monster_shocktrooper);
LINK_ENTITY_TO_FUNC(monster_shocktrooper_repel, "monster_shocktrooper_repel", other_monster_shocktrooper_repel);
LINK_ENTITY_TO_FUNC(monster_sitting_cleansuit_scientist,
    "monster_sitting_cleansuit_scientist",
    other_monster_sitting_cleansuit_scientist);
LINK_ENTITY_TO_FUNC(monster_skeleton_dead, "monster_skeleton_dead", other_monster_skeleton_dead);
LINK_ENTITY_TO_FUNC(monster_torch_ally_repel, "monster_torch_ally_repel", other_monster_torch_ally_repel);
LINK_ENTITY_TO_FUNC(monster_zombie_barney, "monster_zombie_barney", other_monster_zombie_barney);
LINK_ENTITY_TO_FUNC(monster_zombie_soldier, "monster_zombie_soldier", other_monster_zombie_soldier);
LINK_ENTITY_TO_FUNC(monster_zombie_soldier_dead, "monster_zombie_soldier_dead", other_monster_zombie_soldier_dead);
LINK_ENTITY_TO_FUNC(mortar_shell, "mortar_shell", other_mortar_shell);
LINK_ENTITY_TO_FUNC(op4mortar, "op4mortar", other_op4mortar);
LINK_ENTITY_TO_FUNC(pitdronespike, "pitdronespike", other_pitdronespike);
LINK_ENTITY_TO_FUNC(pitworm_gib, "pitworm_gib", other_pitworm_gib);
LINK_ENTITY_TO_FUNC(pitworm_gibshooter, "pitworm_gibshooter", other_pitworm_gibshooter);
LINK_ENTITY_TO_FUNC(rope_sample, "rope_sample", other_rope_sample);
LINK_ENTITY_TO_FUNC(rope_segment, "rope_segment", other_rope_segment);
LINK_ENTITY_TO_FUNC(shock_beam, "shock_beam", other_shock_beam);
LINK_ENTITY_TO_FUNC(spore, "spore", other_spore);
LINK_ENTITY_TO_FUNC(trigger_ctfgeneric, "trigger_ctfgeneric", other_trigger_ctfgeneric);
LINK_ENTITY_TO_FUNC(trigger_geneworm_hit, "trigger_geneworm_hit", other_trigger_geneworm_hit);
LINK_ENTITY_TO_FUNC(trigger_kill_nogib, "trigger_kill_nogib", other_trigger_kill_nogib);
LINK_ENTITY_TO_FUNC(trigger_playerfreeze, "trigger_playerfreeze", other_trigger_playerfreeze);
LINK_ENTITY_TO_FUNC(trigger_xen_return, "trigger_xen_return", other_trigger_xen_return);
LINK_ENTITY_TO_FUNC(weapon_displacer, "weapon_displacer", other_weapon_displacer);
LINK_ENTITY_TO_FUNC(weapon_eagle, "weapon_eagle", other_weapon_eagle);
LINK_ENTITY_TO_FUNC(weapon_grapple, "weapon_grapple", other_weapon_grapple);
LINK_ENTITY_TO_FUNC(weapon_penguin, "weapon_penguin", other_weapon_penguin);
LINK_ENTITY_TO_FUNC(weapon_pipewrench, "weapon_pipewrench", other_weapon_pipewrench);
LINK_ENTITY_TO_FUNC(weapon_shockrifle, "weapon_shockrifle", other_weapon_shockrifle);
LINK_ENTITY_TO_FUNC(weapon_shockroach, "weapon_shockroach", other_weapon_shockroach);
LINK_ENTITY_TO_FUNC(weapon_sniperrifle, "weapon_sniperrifle", other_weapon_sniperrifle);
LINK_ENTITY_TO_FUNC(weapon_sporelauncher, "weapon_sporelauncher", other_weapon_sporelauncher);

// wasteland mod
LINK_ENTITY_TO_FUNC(ammo_caseless, "ammo_caseless", other_ammo_caseless);
LINK_ENTITY_TO_FUNC(ammo_Deagle, "ammo_Deagle", other_ammo_Deagle);
LINK_ENTITY_TO_FUNC(ammo_fnfal, "ammo_fnfal", other_ammo_fnfal);
LINK_ENTITY_TO_FUNC(dc_info_objective, "dc_info_objective", other_dc_info_objective);
LINK_ENTITY_TO_FUNC(dc_info_objectposition, "dc_info_objectposition", other_dc_info_objectposition);
LINK_ENTITY_TO_FUNC(dc_object, "dc_object", other_dc_object);
LINK_ENTITY_TO_FUNC(dc_trigger_objectcapture, "dc_trigger_objectcapture", other_dc_trigger_objectcapture);
LINK_ENTITY_TO_FUNC(dc_trigger_reset, "dc_trigger_reset", other_dc_trigger_reset);
LINK_ENTITY_TO_FUNC(dc_trigger_team, "dc_trigger_team", other_dc_trigger_team);
LINK_ENTITY_TO_FUNC(info_team_ronin, "info_team_ronin", other_info_team_ronin);
LINK_ENTITY_TO_FUNC(info_team_scavenger, "info_team_scavenger", other_info_team_scavenger);
LINK_ENTITY_TO_FUNC(info_team_usmc, "info_team_usmc", other_info_team_usmc);
LINK_ENTITY_TO_FUNC(laser_aimer, "laser_aimer", other_laser_aimer);
LINK_ENTITY_TO_FUNC(weapon_boltrifle, "weapon_boltrifle", other_weapon_boltrifle);
LINK_ENTITY_TO_FUNC(weapon_deserteagle, "weapon_deserteagle", other_weapon_deserteagle);
LINK_ENTITY_TO_FUNC(weapon_fnfal, "weapon_fnfal", other_weapon_fnfal);
LINK_ENTITY_TO_FUNC(weapon_g11, "weapon_g11", other_weapon_g11);
LINK_ENTITY_TO_FUNC(weapon_G11, "weapon_G11", other_weapon_G11);
LINK_ENTITY_TO_FUNC(weapon_jackhammer, "weapon_jackhammer", other_weapon_jackhammer);
LINK_ENTITY_TO_FUNC(weapon_katana, "weapon_katana", other_weapon_katana);
LINK_ENTITY_TO_FUNC(weapon_molotov, "weapon_molotov", other_weapon_molotov);
LINK_ENTITY_TO_FUNC(weapon_rifle, "weapon_rifle", other_weapon_rifle);
LINK_ENTITY_TO_FUNC(weapon_sawedoff, "weapon_sawedoff", other_weapon_sawedoff);
LINK_ENTITY_TO_FUNC(weapon_sledge, "weapon_sledge", other_weapon_sledge);
LINK_ENTITY_TO_FUNC(weapon_spear, "weapon_spear", other_weapon_spear);
LINK_ENTITY_TO_FUNC(spear_bolt, "spear_bolt", other_spear_bolt);

// Wasteland beta 2
LINK_ENTITY_TO_FUNC(RONINTeamScore, "RONINTeamScore", other_RONINTeamScore);
LINK_ENTITY_TO_FUNC(SCAVENGERTeamScore, "SCAVENGERTeamScore", other_SCAVENGERTeamScore);
LINK_ENTITY_TO_FUNC(USMCTeamScore, "USMCTeamScore", other_USMCTeamScore);

// svencoop 1.9 entities
LINK_ENTITY_TO_FUNC(monster_chumtoad, "monster_chumtoad", other_monster_chumtoad);
LINK_ENTITY_TO_FUNC(squadmaker, "squadmaker", other_squadmaker);
LINK_ENTITY_TO_FUNC(trigger_random, "trigger_random", other_trigger_random);
LINK_ENTITY_TO_FUNC(trigger_random_time, "trigger_random_time", other_trigger_random_time);
LINK_ENTITY_TO_FUNC(weapon_uziakimbo, "weapon_uziakimbo", other_weapon_uziakimbo);

// DoD 1.3 - added all Entity for DoD 1.3 - [APG]RoboCop[CL]
LINK_ENTITY_TO_FUNC(weapon_handgrenade_ex, "weapon_handgrenade_ex", other_weapon_handgrenade_ex);
LINK_ENTITY_TO_FUNC(weapon_amerknife, "weapon_amerknife", other_weapon_amerknife);
LINK_ENTITY_TO_FUNC(weapon_germanknife, "weapon_germanknife", other_weapon_germanknife);
LINK_ENTITY_TO_FUNC(dod_object, "dod_object", other_dod_object);
LINK_ENTITY_TO_FUNC(dod_score_ent, "dod_score_ent", other_dod_score_ent);
LINK_ENTITY_TO_FUNC(dod_secondary, "dod_secondary", other_dod_secondary);
LINK_ENTITY_TO_FUNC(func_destroy_objective, "func_destroy_objective", other_func_destroy_objective);
LINK_ENTITY_TO_FUNC(weapon_stickgrenade_ex, "weapon_stickgrenade_ex", other_weapon_stickgrenade_ex);
LINK_ENTITY_TO_FUNC(info_player_axis, "info_player_axis", other_info_player_axis);
LINK_ENTITY_TO_FUNC(info_player_allies, "info_player_allies", other_info_player_allies);
LINK_ENTITY_TO_FUNC(info_initial_player_axis, "info_initial_player_axis", other_info_initial_player_axis);
LINK_ENTITY_TO_FUNC(info_initial_player_allies, "info_initial_player_allies", other_info_initial_player_allies);
LINK_ENTITY_TO_FUNC(weapon_stickgrenade, "weapon_stickgrenade", other_weapon_stickgrenade);
LINK_ENTITY_TO_FUNC(dod_object_goal, "dod_object_goal", other_dod_object_goal);
LINK_ENTITY_TO_FUNC(trigger_control, "trigger_control", other_trigger_control);
LINK_ENTITY_TO_FUNC(dod_control_point, "dod_control_point", other_dod_control_point);
LINK_ENTITY_TO_FUNC(dod_control_point_master, "dod_control_point_master", other_dod_control_point_master);
LINK_ENTITY_TO_FUNC(weapon_garand, "weapon_garand", other_weapon_garand);
LINK_ENTITY_TO_FUNC(ammo_garand, "ammo_garand", other_ammo_garand);
LINK_ENTITY_TO_FUNC(weapon_gewehr, "weapon_gewehr", other_weapon_gewehr);
LINK_ENTITY_TO_FUNC(ammo_gewehr, "ammo_gewehr", other_ammo_gewehr);
LINK_ENTITY_TO_FUNC(weapon_kar, "weapon_kar", other_weapon_kar);
LINK_ENTITY_TO_FUNC(ammo_kar, "ammo_kar", other_ammo_kar);
LINK_ENTITY_TO_FUNC(weapon_luger, "weapon_luger", other_weapon_luger);
LINK_ENTITY_TO_FUNC(ammo_luger, "ammo_luger", other_ammo_luger);
LINK_ENTITY_TO_FUNC(weapon_mp40, "weapon_mp40", other_weapon_mp40);
LINK_ENTITY_TO_FUNC(weapon_mp44, "weapon_mp44", other_weapon_mp44);
LINK_ENTITY_TO_FUNC(ammo_mp44, "ammo_mp44", other_ammo_mp44);
LINK_ENTITY_TO_FUNC(ammo_mp40, "ammo_mp40", other_ammo_mp40);
LINK_ENTITY_TO_FUNC(weapon_spring, "weapon_spring", other_weapon_spring);
LINK_ENTITY_TO_FUNC(ammo_spring, "ammo_spring", other_ammo_spring);
LINK_ENTITY_TO_FUNC(weapon_thompson, "weapon_thompson", other_weapon_thompson);
LINK_ENTITY_TO_FUNC(ammo_thompson, "ammo_thompson", other_ammo_thompson);
LINK_ENTITY_TO_FUNC(weapon_bar, "weapon_bar", other_weapon_bar);
LINK_ENTITY_TO_FUNC(ammo_bar, "ammo_bar", other_ammo_bar);

// Extra entities acquired from Metamod 1.20 linkgame.cpp - [APG]RoboCop[CL}
LINK_ENTITY_TO_GAME(AngleIMatrix);

LINK_ENTITY_TO_GAME(AngleMatrix);

LINK_ENTITY_TO_GAME(BALLS_airball);

LINK_ENTITY_TO_GAME(CTF_Map);

LINK_ENTITY_TO_GAME(CreateInterface);

LINK_ENTITY_TO_GAME(DLLClassName);

LINK_ENTITY_TO_GAME(DelayedUse);

LINK_ENTITY_TO_GAME(Electro_Bolt);

LINK_ENTITY_TO_GAME(FISHINGROD_bolt);

LINK_ENTITY_TO_GAME(FOOT_bolt);

LINK_ENTITY_TO_GAME(FlyingTomaHawk);

LINK_ENTITY_TO_GAME(GetChaseOrigin);

LINK_ENTITY_TO_GAME(IsoBomb);

LINK_ENTITY_TO_GAME(PythRound);

LINK_ENTITY_TO_GAME(RONINTeamScore);

LINK_ENTITY_TO_GAME(SA_Model);

LINK_ENTITY_TO_GAME(SCAVENGERTeamScore);

LINK_ENTITY_TO_GAME(Satelite__Laser);

LINK_ENTITY_TO_GAME(TSAmmoPack);

LINK_ENTITY_TO_GAME(TSWorldGun);

LINK_ENTITY_TO_GAME(TS_PowerUp);

LINK_ENTITY_TO_GAME(TS_bomb);

LINK_ENTITY_TO_GAME(USMCTeamScore);

LINK_ENTITY_TO_GAME(Zombie2Spit);

LINK_ENTITY_TO_GAME(_spit);

LINK_ENTITY_TO_GAME(_squadfix);

LINK_ENTITY_TO_GAME(acidgib);

LINK_ENTITY_TO_GAME(active_bomb);

LINK_ENTITY_TO_GAME(afterimage);

LINK_ENTITY_TO_GAME(aiscripted_sequence);

LINK_ENTITY_TO_GAME(ak74grenade);

LINK_ENTITY_TO_GAME(alien_slave_energy_ball);

LINK_ENTITY_TO_GAME(alien_slave_sapper);

LINK_ENTITY_TO_GAME(alienresourcetower);

LINK_ENTITY_TO_GAME(ambient_generic);

LINK_ENTITY_TO_GAME(ammo_0mm45);

LINK_ENTITY_TO_GAME(ammo_0mm50b);

LINK_ENTITY_TO_GAME(ammo_10mmbuckshot);

LINK_ENTITY_TO_GAME(ammo_10mmbuckshotbox);

LINK_ENTITY_TO_GAME(ammo_127mm);

LINK_ENTITY_TO_GAME(ammo_12g);

LINK_ENTITY_TO_GAME(ammo_1911clip);

LINK_ENTITY_TO_GAME(ammo_2mm);

LINK_ENTITY_TO_GAME(ammo_30cal);

LINK_ENTITY_TO_GAME(ammo_30mm);

LINK_ENTITY_TO_GAME(ammo_338Magnum);

LINK_ENTITY_TO_GAME(ammo_338magnum);

LINK_ENTITY_TO_GAME(ammo_357);

LINK_ENTITY_TO_GAME(ammo_357SIG);

LINK_ENTITY_TO_GAME(ammo_357sig);

LINK_ENTITY_TO_GAME(ammo_40glclip);

LINK_ENTITY_TO_GAME(ammo_40mm);

LINK_ENTITY_TO_GAME(ammo_44);

LINK_ENTITY_TO_GAME(ammo_45ACP);

LINK_ENTITY_TO_GAME(ammo_45acp);

LINK_ENTITY_TO_GAME(ammo_45cal);

LINK_ENTITY_TO_GAME(ammo_50AE);

LINK_ENTITY_TO_GAME(ammo_50ae);

LINK_ENTITY_TO_GAME(ammo_50cal);

LINK_ENTITY_TO_GAME(ammo_556);

LINK_ENTITY_TO_GAME(ammo_556AR);

LINK_ENTITY_TO_GAME(ammo_556Nato);

LINK_ENTITY_TO_GAME(ammo_556box);

LINK_ENTITY_TO_GAME(ammo_556nato);

LINK_ENTITY_TO_GAME(ammo_556natobox);

LINK_ENTITY_TO_GAME(ammo_57mm);

LINK_ENTITY_TO_GAME(ammo_5mm56);

LINK_ENTITY_TO_GAME(ammo_5mm7);

LINK_ENTITY_TO_GAME(ammo_762);

LINK_ENTITY_TO_GAME(ammo_762Magnum);

LINK_ENTITY_TO_GAME(ammo_762Nato);

LINK_ENTITY_TO_GAME(ammo_762mmbox);

LINK_ENTITY_TO_GAME(ammo_762nato);

LINK_ENTITY_TO_GAME(ammo_762x54);

LINK_ENTITY_TO_GAME(ammo_766mm);

LINK_ENTITY_TO_GAME(ammo_7mm62);

LINK_ENTITY_TO_GAME(ammo_86mm);

LINK_ENTITY_TO_GAME(ammo_8mm);

LINK_ENTITY_TO_GAME(ammo_9mm);

LINK_ENTITY_TO_GAME(ammo_9mmAR);

LINK_ENTITY_TO_GAME(ammo_9mmbox);

LINK_ENTITY_TO_GAME(ammo_9mmclip);

LINK_ENTITY_TO_GAME(ammo_AK47clip);

LINK_ENTITY_TO_GAME(ammo_ARgrenades);

LINK_ENTITY_TO_GAME(ammo_BALLS);

LINK_ENTITY_TO_GAME(ammo_CEnfield);

LINK_ENTITY_TO_GAME(ammo_Deagle);

LINK_ENTITY_TO_GAME(ammo_Dmissile);

LINK_ENTITY_TO_GAME(ammo_EnergyCells);

LINK_ENTITY_TO_GAME(ammo_FISHINGROD);

LINK_ENTITY_TO_GAME(ammo_FOOT);

LINK_ENTITY_TO_GAME(ammo_FTclip);

LINK_ENTITY_TO_GAME(ammo_GMauserG98);

LINK_ENTITY_TO_GAME(ammo_GluonCells);

LINK_ENTITY_TO_GAME(ammo_M16grenades);

LINK_ENTITY_TO_GAME(ammo_MicroFusionCells);

LINK_ENTITY_TO_GAME(ammo_NGgrenades);

LINK_ENTITY_TO_GAME(ammo_ShockCore);

LINK_ENTITY_TO_GAME(ammo_ShockGrenade);

LINK_ENTITY_TO_GAME(ammo_SunOfGod);

LINK_ENTITY_TO_GAME(ammo_WTELEPORTERgrenades);

LINK_ENTITY_TO_GAME(ammo_adrenaline);

LINK_ENTITY_TO_GAME(ammo_ak101);

LINK_ENTITY_TO_GAME(ammo_ak105);

LINK_ENTITY_TO_GAME(ammo_ak47);

LINK_ENTITY_TO_GAME(ammo_ak47clip);

LINK_ENTITY_TO_GAME(ammo_ak5);

LINK_ENTITY_TO_GAME(ammo_ak74);

LINK_ENTITY_TO_GAME(ammo_ak74clip);

LINK_ENTITY_TO_GAME(ammo_ak74grenades);

LINK_ENTITY_TO_GAME(ammo_anaconda);

LINK_ENTITY_TO_GAME(ammo_ar10clip);

LINK_ENTITY_TO_GAME(ammo_ar33clip);

LINK_ENTITY_TO_GAME(ammo_areaammo);

LINK_ENTITY_TO_GAME(ammo_aug);

LINK_ENTITY_TO_GAME(ammo_awm);

LINK_ENTITY_TO_GAME(ammo_awp);

LINK_ENTITY_TO_GAME(ammo_axes);

LINK_ENTITY_TO_GAME(ammo_bar);

LINK_ENTITY_TO_GAME(ammo_baretta);

LINK_ENTITY_TO_GAME(ammo_bazooka);

LINK_ENTITY_TO_GAME(ammo_beamgunclip);

LINK_ENTITY_TO_GAME(ammo_benelli);

LINK_ENTITY_TO_GAME(ammo_ber92f);

LINK_ENTITY_TO_GAME(ammo_ber93r);

LINK_ENTITY_TO_GAME(ammo_beretta);

LINK_ENTITY_TO_GAME(ammo_berettaclip);

LINK_ENTITY_TO_GAME(ammo_berrett82clip);

LINK_ENTITY_TO_GAME(ammo_bfg);

LINK_ENTITY_TO_GAME(ammo_bizon);

LINK_ENTITY_TO_GAME(ammo_boltrifle);

LINK_ENTITY_TO_GAME(ammo_bow);

LINK_ENTITY_TO_GAME(ammo_bren);

LINK_ENTITY_TO_GAME(ammo_brick);

LINK_ENTITY_TO_GAME(ammo_buckshot);

LINK_ENTITY_TO_GAME(ammo_buffalo);

LINK_ENTITY_TO_GAME(ammo_cannon);

LINK_ENTITY_TO_GAME(ammo_caseless);

LINK_ENTITY_TO_GAME(ammo_cell);

LINK_ENTITY_TO_GAME(ammo_cgrenade);

LINK_ENTITY_TO_GAME(ammo_chemical);

LINK_ENTITY_TO_GAME(ammo_cloaker);

LINK_ENTITY_TO_GAME(ammo_colt);

LINK_ENTITY_TO_GAME(ammo_colt45clip);

LINK_ENTITY_TO_GAME(ammo_coltgov);

LINK_ENTITY_TO_GAME(ammo_colts);

LINK_ENTITY_TO_GAME(ammo_concussion);

LINK_ENTITY_TO_GAME(ammo_cougarclip);

LINK_ENTITY_TO_GAME(ammo_cpc);

LINK_ENTITY_TO_GAME(ammo_crate);

LINK_ENTITY_TO_GAME(ammo_crossbow);

LINK_ENTITY_TO_GAME(ammo_d5kclip);

LINK_ENTITY_TO_GAME(ammo_dbshot);

LINK_ENTITY_TO_GAME(ammo_dd44clip);

LINK_ENTITY_TO_GAME(ammo_de50);

LINK_ENTITY_TO_GAME(ammo_deagle);

LINK_ENTITY_TO_GAME(ammo_deagleclip);

LINK_ENTITY_TO_GAME(ammo_deploygun);

LINK_ENTITY_TO_GAME(ammo_desert);

LINK_ENTITY_TO_GAME(ammo_dmlclip);

LINK_ENTITY_TO_GAME(ammo_doubleshotgun);

LINK_ENTITY_TO_GAME(ammo_dragunovclip);

LINK_ENTITY_TO_GAME(ammo_dsr1);

LINK_ENTITY_TO_GAME(ammo_dualberettas);

LINK_ENTITY_TO_GAME(ammo_dualscorpion);

LINK_ENTITY_TO_GAME(ammo_dyno);

LINK_ENTITY_TO_GAME(ammo_eagleclip);

LINK_ENTITY_TO_GAME(ammo_egonclip);

LINK_ENTITY_TO_GAME(ammo_el_standard);

LINK_ENTITY_TO_GAME(ammo_enfield);

LINK_ENTITY_TO_GAME(ammo_exploder);

LINK_ENTITY_TO_GAME(ammo_explogun);

LINK_ENTITY_TO_GAME(ammo_famas);

LINK_ENTITY_TO_GAME(ammo_famasclip);

LINK_ENTITY_TO_GAME(ammo_fg42);

LINK_ENTITY_TO_GAME(ammo_fiveseven);

LINK_ENTITY_TO_GAME(ammo_flameammo);

LINK_ENTITY_TO_GAME(ammo_flameclip);

LINK_ENTITY_TO_GAME(ammo_flashbang);

LINK_ENTITY_TO_GAME(ammo_fnfal);

LINK_ENTITY_TO_GAME(ammo_fnp90clip);

LINK_ENTITY_TO_GAME(ammo_frag);

LINK_ENTITY_TO_GAME(ammo_freezeclip);

LINK_ENTITY_TO_GAME(ammo_fuel);

LINK_ENTITY_TO_GAME(ammo_g11);

LINK_ENTITY_TO_GAME(ammo_g36);

LINK_ENTITY_TO_GAME(ammo_g36clip);

LINK_ENTITY_TO_GAME(ammo_g36e);

LINK_ENTITY_TO_GAME(ammo_g36k);

LINK_ENTITY_TO_GAME(ammo_g3a3);

LINK_ENTITY_TO_GAME(ammo_galil);

LINK_ENTITY_TO_GAME(ammo_garand);

LINK_ENTITY_TO_GAME(ammo_gatlinbox);

LINK_ENTITY_TO_GAME(ammo_gattlinggun);

LINK_ENTITY_TO_GAME(ammo_gaussclip);

LINK_ENTITY_TO_GAME(ammo_generic_american);

LINK_ENTITY_TO_GAME(ammo_generic_british);

LINK_ENTITY_TO_GAME(ammo_generic_german);

LINK_ENTITY_TO_GAME(ammo_gewehr);

LINK_ENTITY_TO_GAME(ammo_glock17);

LINK_ENTITY_TO_GAME(ammo_glock18);

LINK_ENTITY_TO_GAME(ammo_glockclip);

LINK_ENTITY_TO_GAME(ammo_goldpp7clip);

LINK_ENTITY_TO_GAME(ammo_gp25);

LINK_ENTITY_TO_GAME(ammo_greasegun);

LINK_ENTITY_TO_GAME(ammo_grenades);

LINK_ENTITY_TO_GAME(ammo_handcannon);

LINK_ENTITY_TO_GAME(ammo_helirockets);

LINK_ENTITY_TO_GAME(ammo_hellfire);

LINK_ENTITY_TO_GAME(ammo_hk21);

LINK_ENTITY_TO_GAME(ammo_hk33clip);

LINK_ENTITY_TO_GAME(ammo_hkmp5);

LINK_ENTITY_TO_GAME(ammo_isotope);

LINK_ENTITY_TO_GAME(ammo_jackhammer);

LINK_ENTITY_TO_GAME(ammo_javelins);

LINK_ENTITY_TO_GAME(ammo_k43);

LINK_ENTITY_TO_GAME(ammo_kar);

LINK_ENTITY_TO_GAME(ammo_kf7clip);

LINK_ENTITY_TO_GAME(ammo_knife);

LINK_ENTITY_TO_GAME(ammo_knives);

LINK_ENTITY_TO_GAME(ammo_launcher);

LINK_ENTITY_TO_GAME(ammo_lgclip);

LINK_ENTITY_TO_GAME(ammo_lgrenades);

LINK_ENTITY_TO_GAME(ammo_lightning);

LINK_ENTITY_TO_GAME(ammo_longbow);

LINK_ENTITY_TO_GAME(ammo_longslide);

LINK_ENTITY_TO_GAME(ammo_lrclip);

LINK_ENTITY_TO_GAME(ammo_luger);

LINK_ENTITY_TO_GAME(ammo_m11);

LINK_ENTITY_TO_GAME(ammo_m112);

LINK_ENTITY_TO_GAME(ammo_m134);

LINK_ENTITY_TO_GAME(ammo_m16);

LINK_ENTITY_TO_GAME(ammo_m16a2clip);

LINK_ENTITY_TO_GAME(ammo_m16clip);

LINK_ENTITY_TO_GAME(ammo_m1carbine);

LINK_ENTITY_TO_GAME(ammo_m2);

LINK_ENTITY_TO_GAME(ammo_m203);

LINK_ENTITY_TO_GAME(ammo_m249);

LINK_ENTITY_TO_GAME(ammo_m4);

LINK_ENTITY_TO_GAME(ammo_m40a1clip);

LINK_ENTITY_TO_GAME(ammo_m4a1);

LINK_ENTITY_TO_GAME(ammo_m4clip);

LINK_ENTITY_TO_GAME(ammo_m60);

LINK_ENTITY_TO_GAME(ammo_m60clip);

LINK_ENTITY_TO_GAME(ammo_m79);

LINK_ENTITY_TO_GAME(ammo_m82);

LINK_ENTITY_TO_GAME(ammo_mac);

LINK_ENTITY_TO_GAME(ammo_mac10);

LINK_ENTITY_TO_GAME(ammo_machinegun);

LINK_ENTITY_TO_GAME(ammo_markmusket);

LINK_ENTITY_TO_GAME(ammo_mc51);

LINK_ENTITY_TO_GAME(ammo_mf2clip);

LINK_ENTITY_TO_GAME(ammo_mg34);

LINK_ENTITY_TO_GAME(ammo_mg36);

LINK_ENTITY_TO_GAME(ammo_mg42);

LINK_ENTITY_TO_GAME(ammo_mindrayclip);

LINK_ENTITY_TO_GAME(ammo_minigunClip);

LINK_ENTITY_TO_GAME(ammo_minimissile);

LINK_ENTITY_TO_GAME(ammo_mk23);

LINK_ENTITY_TO_GAME(ammo_mossberg);

LINK_ENTITY_TO_GAME(ammo_mp40);

LINK_ENTITY_TO_GAME(ammo_mp44);

LINK_ENTITY_TO_GAME(ammo_mp5);

LINK_ENTITY_TO_GAME(ammo_mp5a2);

LINK_ENTITY_TO_GAME(ammo_mp5a4);

LINK_ENTITY_TO_GAME(ammo_mp5a5);

LINK_ENTITY_TO_GAME(ammo_mp5clip);

LINK_ENTITY_TO_GAME(ammo_mp5grenades);

LINK_ENTITY_TO_GAME(ammo_mp5k);

LINK_ENTITY_TO_GAME(ammo_mp5pdwclip);

LINK_ENTITY_TO_GAME(ammo_mp5sd);

LINK_ENTITY_TO_GAME(ammo_msg90);

LINK_ENTITY_TO_GAME(ammo_musketoon);

LINK_ENTITY_TO_GAME(ammo_nails);

LINK_ENTITY_TO_GAME(ammo_nato);

LINK_ENTITY_TO_GAME(ammo_nato308);

LINK_ENTITY_TO_GAME(ammo_natoclip);

LINK_ENTITY_TO_GAME(ammo_needle);

LINK_ENTITY_TO_GAME(ammo_nuke);

LINK_ENTITY_TO_GAME(ammo_null);

LINK_ENTITY_TO_GAME(ammo_p99);

LINK_ENTITY_TO_GAME(ammo_p99clip);

LINK_ENTITY_TO_GAME(ammo_pdw);

LINK_ENTITY_TO_GAME(ammo_phantomclip);

LINK_ENTITY_TO_GAME(ammo_piat);

LINK_ENTITY_TO_GAME(ammo_pistol);

LINK_ENTITY_TO_GAME(ammo_pistolet);

LINK_ENTITY_TO_GAME(ammo_pkm);

LINK_ENTITY_TO_GAME(ammo_pkmclip);

LINK_ENTITY_TO_GAME(ammo_powercell);

LINK_ENTITY_TO_GAME(ammo_pschreck);

LINK_ENTITY_TO_GAME(ammo_psg1);

LINK_ENTITY_TO_GAME(ammo_psg1clip);

LINK_ENTITY_TO_GAME(ammo_quad);

LINK_ENTITY_TO_GAME(ammo_railgun);

LINK_ENTITY_TO_GAME(ammo_rcp90clip);

LINK_ENTITY_TO_GAME(ammo_remington);

LINK_ENTITY_TO_GAME(ammo_rifleclip);

LINK_ENTITY_TO_GAME(ammo_rifleshot);

LINK_ENTITY_TO_GAME(ammo_rocket);

LINK_ENTITY_TO_GAME(ammo_rocketlauncher);

LINK_ENTITY_TO_GAME(ammo_rocketpistol);

LINK_ENTITY_TO_GAME(ammo_rockets);

LINK_ENTITY_TO_GAME(ammo_rpgclip);

LINK_ENTITY_TO_GAME(ammo_rs202m2);

LINK_ENTITY_TO_GAME(ammo_rs_grenade);

LINK_ENTITY_TO_GAME(ammo_rs_psi);

LINK_ENTITY_TO_GAME(ammo_ruger);

LINK_ENTITY_TO_GAME(ammo_sa80clip);

LINK_ENTITY_TO_GAME(ammo_saa);

LINK_ENTITY_TO_GAME(ammo_saiga);

LINK_ENTITY_TO_GAME(ammo_sako);

LINK_ENTITY_TO_GAME(ammo_satchels);

LINK_ENTITY_TO_GAME(ammo_sbarrelshot);

LINK_ENTITY_TO_GAME(ammo_scatterclip);

LINK_ENTITY_TO_GAME(ammo_scopedkar);

LINK_ENTITY_TO_GAME(ammo_shells);

LINK_ENTITY_TO_GAME(ammo_shot);

LINK_ENTITY_TO_GAME(ammo_shrapnel);

LINK_ENTITY_TO_GAME(ammo_sig245clip);

LINK_ENTITY_TO_GAME(ammo_silverpp7clip);

LINK_ENTITY_TO_GAME(ammo_sks);

LINK_ENTITY_TO_GAME(ammo_slugs);

LINK_ENTITY_TO_GAME(ammo_smg9);

LINK_ENTITY_TO_GAME(ammo_snUZI);

LINK_ENTITY_TO_GAME(ammo_sniper);

LINK_ENTITY_TO_GAME(ammo_spas12);

LINK_ENTITY_TO_GAME(ammo_spasclip);

LINK_ENTITY_TO_GAME(ammo_spellbook);

LINK_ENTITY_TO_GAME(ammo_spitcarb);

LINK_ENTITY_TO_GAME(ammo_spitclip);

LINK_ENTITY_TO_GAME(ammo_spore);

LINK_ENTITY_TO_GAME(ammo_spring);

LINK_ENTITY_TO_GAME(ammo_srifleclip);

LINK_ENTITY_TO_GAME(ammo_ssg3000);

LINK_ENTITY_TO_GAME(ammo_st_far);

LINK_ENTITY_TO_GAME(ammo_st_heavy);

LINK_ENTITY_TO_GAME(ammo_st_medium);

LINK_ENTITY_TO_GAME(ammo_st_short);

LINK_ENTITY_TO_GAME(ammo_st_special1);

LINK_ENTITY_TO_GAME(ammo_st_special2);

LINK_ENTITY_TO_GAME(ammo_sten);

LINK_ENTITY_TO_GAME(ammo_sterling);

LINK_ENTITY_TO_GAME(ammo_steyr_m40);

LINK_ENTITY_TO_GAME(ammo_stg24);

LINK_ENTITY_TO_GAME(ammo_stoner);

LINK_ENTITY_TO_GAME(ammo_stonerscope);

LINK_ENTITY_TO_GAME(ammo_stonersilencer);

LINK_ENTITY_TO_GAME(ammo_stormcell);

LINK_ENTITY_TO_GAME(ammo_stubcoolant);

LINK_ENTITY_TO_GAME(ammo_stumpbox);

LINK_ENTITY_TO_GAME(ammo_svd);

LINK_ENTITY_TO_GAME(ammo_swarm_xen_secondary);

LINK_ENTITY_TO_GAME(ammo_swarmprimary);

LINK_ENTITY_TO_GAME(ammo_swarmsecondary);

LINK_ENTITY_TO_GAME(ammo_synthol);

LINK_ENTITY_TO_GAME(ammo_tec9);

LINK_ENTITY_TO_GAME(ammo_thompson);

LINK_ENTITY_TO_GAME(ammo_tommy);

LINK_ENTITY_TO_GAME(ammo_tommygun);

LINK_ENTITY_TO_GAME(ammo_trigun);

LINK_ENTITY_TO_GAME(ammo_ump45);

LINK_ENTITY_TO_GAME(ammo_uranium235);

LINK_ENTITY_TO_GAME(ammo_uranium238);

LINK_ENTITY_TO_GAME(ammo_usp);

LINK_ENTITY_TO_GAME(ammo_uspclip);

LINK_ENTITY_TO_GAME(ammo_uzi);

LINK_ENTITY_TO_GAME(ammo_uziclip);

LINK_ENTITY_TO_GAME(ammo_veprclip);

LINK_ENTITY_TO_GAME(ammo_vomitclip);

LINK_ENTITY_TO_GAME(ammo_vulcan);

LINK_ENTITY_TO_GAME(ammo_wa2000);

LINK_ENTITY_TO_GAME(ammo_webley);

LINK_ENTITY_TO_GAME(ammo_whiskey);

LINK_ENTITY_TO_GAME(ammo_winchester);

LINK_ENTITY_TO_GAME(ammo_winchesterclip);

LINK_ENTITY_TO_GAME(ammo_xbolts);

LINK_ENTITY_TO_GAME(ammo_xm4);

LINK_ENTITY_TO_GAME(ammo_xm4slug);

LINK_ENTITY_TO_GAME(ammo_zmgclip);

LINK_ENTITY_TO_GAME(ammobox);

LINK_ENTITY_TO_GAME(antifric_timer);

LINK_ENTITY_TO_GAME(antifriction_grenade);

LINK_ENTITY_TO_GAME(arcade_points);

LINK_ENTITY_TO_GAME(armoury_entity);

LINK_ENTITY_TO_GAME(arms_dealer);

LINK_ENTITY_TO_GAME(arrow);

LINK_ENTITY_TO_GAME(asl_charge_flare);

LINK_ENTITY_TO_GAME(aura);

LINK_ENTITY_TO_GAME(authenticationmanager);

LINK_ENTITY_TO_GAME(autogl_gren);

LINK_ENTITY_TO_GAME(ball_launcher);

LINK_ENTITY_TO_GAME(ball_lightning);

LINK_ENTITY_TO_GAME(balle);

LINK_ENTITY_TO_GAME(banana);

LINK_ENTITY_TO_GAME(bankinterest);

LINK_ENTITY_TO_GAME(barneymine);

LINK_ENTITY_TO_GAME(base_drive_ent);

LINK_ENTITY_TO_GAME(basecurse);

LINK_ENTITY_TO_GAME(bb_ammo);

LINK_ENTITY_TO_GAME(bb_custommission);

LINK_ENTITY_TO_GAME(bb_equipment);

LINK_ENTITY_TO_GAME(bb_escape_final);

LINK_ENTITY_TO_GAME(bb_escape_radar);

LINK_ENTITY_TO_GAME(bb_escapeair);

LINK_ENTITY_TO_GAME(bb_funk);

LINK_ENTITY_TO_GAME(bb_healthkit);

LINK_ENTITY_TO_GAME(bb_mapmission);

LINK_ENTITY_TO_GAME(bb_mission_zone);

LINK_ENTITY_TO_GAME(bb_objective_item);

LINK_ENTITY_TO_GAME(bb_objectives);

LINK_ENTITY_TO_GAME(bb_spawn_player);

LINK_ENTITY_TO_GAME(bb_spawn_zombie);

LINK_ENTITY_TO_GAME(bb_tank);

LINK_ENTITY_TO_GAME(bb_waypoint);

LINK_ENTITY_TO_GAME(bb_zombie_player);

LINK_ENTITY_TO_GAME(bbody);

LINK_ENTITY_TO_GAME(beam);

LINK_ENTITY_TO_GAME(beam_attack);

LINK_ENTITY_TO_GAME(beamshot);

LINK_ENTITY_TO_GAME(bean);

LINK_ENTITY_TO_GAME(beanstalk);

LINK_ENTITY_TO_GAME(bfg10k);

LINK_ENTITY_TO_GAME(big_bang);

LINK_ENTITY_TO_GAME(big_disk);

LINK_ENTITY_TO_GAME(big_lance_proj);

LINK_ENTITY_TO_GAME(bird);

LINK_ENTITY_TO_GAME(black_hole);

LINK_ENTITY_TO_GAME(blade);

LINK_ENTITY_TO_GAME(blaster_beam);

LINK_ENTITY_TO_GAME(blaster_bolt);

LINK_ENTITY_TO_GAME(bmortar);

LINK_ENTITY_TO_GAME(bodyque);

LINK_ENTITY_TO_GAME(bone_follow);

LINK_ENTITY_TO_GAME(boolean_multisource);

LINK_ENTITY_TO_GAME(booty_it);

LINK_ENTITY_TO_GAME(bot);

LINK_ENTITY_TO_GAME(bot_waypoint);

LINK_ENTITY_TO_GAME(bouncy_bolt);

LINK_ENTITY_TO_GAME(bouncy_tripmine);

LINK_ENTITY_TO_GAME(bowie_bolt);

LINK_ENTITY_TO_GAME(bubgren);

LINK_ENTITY_TO_GAME(building_dispenser);

LINK_ENTITY_TO_GAME(building_sentrygun);

LINK_ENTITY_TO_GAME(building_sentrygun_base);

LINK_ENTITY_TO_GAME(building_teleporter);

LINK_ENTITY_TO_GAME(bullet);

LINK_ENTITY_TO_GAME(bumper_car_start);

LINK_ENTITY_TO_GAME(burning_attack);

LINK_ENTITY_TO_GAME(button_aiwallplug);

LINK_ENTITY_TO_GAME(button_target);

LINK_ENTITY_TO_GAME(calc_position);

LINK_ENTITY_TO_GAME(calc_ratio);

LINK_ENTITY_TO_GAME(calc_subvelocity);

LINK_ENTITY_TO_GAME(calc_velocity_path);

LINK_ENTITY_TO_GAME(calc_velocity_polar);

LINK_ENTITY_TO_GAME(candy);

LINK_ENTITY_TO_GAME(cannon_ball);

LINK_ENTITY_TO_GAME(capture_point);

LINK_ENTITY_TO_GAME(carried_book_team1);

LINK_ENTITY_TO_GAME(carried_book_team2);

LINK_ENTITY_TO_GAME(carried_flag_team1);

LINK_ENTITY_TO_GAME(carried_flag_team2);

LINK_ENTITY_TO_GAME(carry_resource);

LINK_ENTITY_TO_GAME(carry_scientist);

LINK_ENTITY_TO_GAME(ce_psi_01);

LINK_ENTITY_TO_GAME(ce_rocket_01);

LINK_ENTITY_TO_GAME(cflag);

LINK_ENTITY_TO_GAME(chain_hurter);

LINK_ENTITY_TO_GAME(charge);

LINK_ENTITY_TO_GAME(charged_bolt);

LINK_ENTITY_TO_GAME(chronoclip);

LINK_ENTITY_TO_GAME(cine_blood);

LINK_ENTITY_TO_GAME(clbaby);

LINK_ENTITY_TO_GAME(clmomma);

LINK_ENTITY_TO_GAME(cm_rocket_01);

LINK_ENTITY_TO_GAME(cocklebur);

LINK_ENTITY_TO_GAME(conc_charge);

LINK_ENTITY_TO_GAME(control_forklift);

LINK_ENTITY_TO_GAME(controller_charge_ball);

LINK_ENTITY_TO_GAME(controller_charged_ball);

LINK_ENTITY_TO_GAME(controller_energy_ball);

LINK_ENTITY_TO_GAME(controller_head_ball);

LINK_ENTITY_TO_GAME(controller_health_ball);

LINK_ENTITY_TO_GAME(controller_throw_ball);

LINK_ENTITY_TO_GAME(counter);

LINK_ENTITY_TO_GAME(crossbow_bolt);

LINK_ENTITY_TO_GAME(crowbar_rocket);

LINK_ENTITY_TO_GAME(crowbarang);

LINK_ENTITY_TO_GAME(crowd);

LINK_ENTITY_TO_GAME(ctb_point_one);

LINK_ENTITY_TO_GAME(ctb_point_two);

LINK_ENTITY_TO_GAME(ctc_capturepoint);

LINK_ENTITY_TO_GAME(ctc_chicken);

LINK_ENTITY_TO_GAME(ctf_blueflag);

LINK_ENTITY_TO_GAME(ctf_bluespawn);

LINK_ENTITY_TO_GAME(ctf_cloak);

LINK_ENTITY_TO_GAME(ctf_crowbar);

LINK_ENTITY_TO_GAME(ctf_cycle);

LINK_ENTITY_TO_GAME(ctf_flag);

LINK_ENTITY_TO_GAME(ctf_flag_follow_blue);

LINK_ENTITY_TO_GAME(ctf_flag_follow_red);

LINK_ENTITY_TO_GAME(ctf_flagzone);

LINK_ENTITY_TO_GAME(ctf_frag);

LINK_ENTITY_TO_GAME(ctf_hook);

LINK_ENTITY_TO_GAME(ctf_lowgrav);

LINK_ENTITY_TO_GAME(ctf_protect);

LINK_ENTITY_TO_GAME(ctf_redflag);

LINK_ENTITY_TO_GAME(ctf_redspawn);

LINK_ENTITY_TO_GAME(ctf_regen);

LINK_ENTITY_TO_GAME(ctf_rune);

LINK_ENTITY_TO_GAME(ctf_superglock);

LINK_ENTITY_TO_GAME(ctf_trap);

LINK_ENTITY_TO_GAME(ctf_vamp);

LINK_ENTITY_TO_GAME(curse_spawner);

LINK_ENTITY_TO_GAME(cust_11);

LINK_ENTITY_TO_GAME(cust_12);

LINK_ENTITY_TO_GAME(cust_13);

LINK_ENTITY_TO_GAME(cust_21);

LINK_ENTITY_TO_GAME(cust_22);

LINK_ENTITY_TO_GAME(cust_24);

LINK_ENTITY_TO_GAME(cust_25);

LINK_ENTITY_TO_GAME(cust_2GaussPistolSniper);

LINK_ENTITY_TO_GAME(cust_2MinigunCooled);

LINK_ENTITY_TO_GAME(cust_31);

LINK_ENTITY_TO_GAME(cust_33);

LINK_ENTITY_TO_GAME(cust_34);

LINK_ENTITY_TO_GAME(cust_41);

LINK_ENTITY_TO_GAME(cust_43);

LINK_ENTITY_TO_GAME(custom_precache);

LINK_ENTITY_TO_GAME(cycler);

LINK_ENTITY_TO_GAME(cycler_prdroid);

LINK_ENTITY_TO_GAME(cycler_sprite);

LINK_ENTITY_TO_GAME(cycler_weapon);

LINK_ENTITY_TO_GAME(cycler_wreckage);

LINK_ENTITY_TO_GAME(davespit);

LINK_ENTITY_TO_GAME(db_spawner);

LINK_ENTITY_TO_GAME(dc_info_objective);

LINK_ENTITY_TO_GAME(dc_info_objectposition);

LINK_ENTITY_TO_GAME(dc_object);

LINK_ENTITY_TO_GAME(dc_trigger_objectcapture);

LINK_ENTITY_TO_GAME(dc_trigger_reset);

LINK_ENTITY_TO_GAME(dc_trigger_team);

LINK_ENTITY_TO_GAME(deadcorpse);

LINK_ENTITY_TO_GAME(deal_columbian_start);

LINK_ENTITY_TO_GAME(deal_italian_start);

LINK_ENTITY_TO_GAME(deal_swat_start);

LINK_ENTITY_TO_GAME(dealer);

LINK_ENTITY_TO_GAME(death_ball);

LINK_ENTITY_TO_GAME(deathball);

LINK_ENTITY_TO_GAME(deathsoul);

LINK_ENTITY_TO_GAME(debris);

LINK_ENTITY_TO_GAME(decore_aicore);

LINK_ENTITY_TO_GAME(decore_asteroid);

LINK_ENTITY_TO_GAME(decore_baboon);

LINK_ENTITY_TO_GAME(decore_bodygib);

LINK_ENTITY_TO_GAME(decore_butterflyflock);

LINK_ENTITY_TO_GAME(decore_cactus);

LINK_ENTITY_TO_GAME(decore_cam);

LINK_ENTITY_TO_GAME(decore_corpse);

LINK_ENTITY_TO_GAME(decore_eagle);

LINK_ENTITY_TO_GAME(decore_explodable);

LINK_ENTITY_TO_GAME(decore_foot);

LINK_ENTITY_TO_GAME(decore_goldskull);

LINK_ENTITY_TO_GAME(decore_gutspile);

LINK_ENTITY_TO_GAME(decore_hatgib);

LINK_ENTITY_TO_GAME(decore_ice);

LINK_ENTITY_TO_GAME(decore_mushroom);

LINK_ENTITY_TO_GAME(decore_mushroom2);

LINK_ENTITY_TO_GAME(decore_nest);

LINK_ENTITY_TO_GAME(decore_pipes);

LINK_ENTITY_TO_GAME(decore_prickle);

LINK_ENTITY_TO_GAME(decore_pteradon);

LINK_ENTITY_TO_GAME(decore_scripted_boulder);

LINK_ENTITY_TO_GAME(decore_sittingtubemortar);

LINK_ENTITY_TO_GAME(decore_spacebebris);

LINK_ENTITY_TO_GAME(decore_swampplants);

LINK_ENTITY_TO_GAME(decore_torch);

LINK_ENTITY_TO_GAME(decore_torchflame);

LINK_ENTITY_TO_GAME(defensechamber);

LINK_ENTITY_TO_GAME(demoman_mine);

LINK_ENTITY_TO_GAME(detpack);

LINK_ENTITY_TO_GAME(dg_streak_spiral);

LINK_ENTITY_TO_GAME(disc);

LINK_ENTITY_TO_GAME(disc_arena);

LINK_ENTITY_TO_GAME(discarena);

LINK_ENTITY_TO_GAME(dispball);

LINK_ENTITY_TO_GAME(displacer_ball);

LINK_ENTITY_TO_GAME(dmgmirrorcurse);

LINK_ENTITY_TO_GAME(dmgscalecurse);

LINK_ENTITY_TO_GAME(dod_ammo);

LINK_ENTITY_TO_GAME(dod_ammo_depot);

LINK_ENTITY_TO_GAME(dod_camera);

LINK_ENTITY_TO_GAME(dod_capture_area);

LINK_ENTITY_TO_GAME(dod_control_point);

LINK_ENTITY_TO_GAME(dod_control_point_master);

LINK_ENTITY_TO_GAME(dod_location);

LINK_ENTITY_TO_GAME(dod_mapmarker);

LINK_ENTITY_TO_GAME(dod_object);

LINK_ENTITY_TO_GAME(dod_object_goal);

LINK_ENTITY_TO_GAME(dod_point_relay);

LINK_ENTITY_TO_GAME(dod_preround);

LINK_ENTITY_TO_GAME(dod_round_timer);

LINK_ENTITY_TO_GAME(dod_score_ent);

LINK_ENTITY_TO_GAME(dod_secondary);

LINK_ENTITY_TO_GAME(dod_stats);

LINK_ENTITY_TO_GAME(dod_trigger_sandbag);

LINK_ENTITY_TO_GAME(dove_flock);

LINK_ENTITY_TO_GAME(dove_flyer);

LINK_ENTITY_TO_GAME(dragonball);

LINK_ENTITY_TO_GAME(drive_agrunt);

LINK_ENTITY_TO_GAME(drive_agrunt_spawner);

LINK_ENTITY_TO_GAME(drive_apache);

LINK_ENTITY_TO_GAME(drive_apache_spawner);

LINK_ENTITY_TO_GAME(drive_asl);

LINK_ENTITY_TO_GAME(drive_assassin);

LINK_ENTITY_TO_GAME(drive_assassin_spawner);

LINK_ENTITY_TO_GAME(drive_bm);

LINK_ENTITY_TO_GAME(drive_bm_spawner);

LINK_ENTITY_TO_GAME(drive_bullsquid);

LINK_ENTITY_TO_GAME(drive_bullsquid_spawner);

LINK_ENTITY_TO_GAME(drive_chumtoad);

LINK_ENTITY_TO_GAME(drive_controller);

LINK_ENTITY_TO_GAME(drive_controller_spawner);

LINK_ENTITY_TO_GAME(drive_garg);

LINK_ENTITY_TO_GAME(drive_garg_spawner);

LINK_ENTITY_TO_GAME(drive_headcrab);

LINK_ENTITY_TO_GAME(drive_houndeye);

LINK_ENTITY_TO_GAME(drive_houndeye_spawner);

LINK_ENTITY_TO_GAME(drive_icky);

LINK_ENTITY_TO_GAME(drive_icky_spawner);

LINK_ENTITY_TO_GAME(drive_panth);

LINK_ENTITY_TO_GAME(drive_panth_spawner);

LINK_ENTITY_TO_GAME(drive_slave_spawner);

LINK_ENTITY_TO_GAME(drive_turret);

LINK_ENTITY_TO_GAME(drive_turret_spawner);

LINK_ENTITY_TO_GAME(drop_bomb);

LINK_ENTITY_TO_GAME(drunkcurse);

LINK_ENTITY_TO_GAME(drv_hvr_rocket);

LINK_ENTITY_TO_GAME(eagle_laser);

LINK_ENTITY_TO_GAME(effect_shield);

LINK_ENTITY_TO_GAME(egon_mirv);

LINK_ENTITY_TO_GAME(emp_cannister);

LINK_ENTITY_TO_GAME(emp_pulse);

LINK_ENTITY_TO_GAME(entity_botcam);

LINK_ENTITY_TO_GAME(entity_clustergod);

LINK_ENTITY_TO_GAME(entity_digitgod);

LINK_ENTITY_TO_GAME(entity_spawn_round);

LINK_ENTITY_TO_GAME(entity_spritegod);

LINK_ENTITY_TO_GAME(entity_volcanospew);

LINK_ENTITY_TO_GAME(env_ClusterExplosion);

LINK_ENTITY_TO_GAME(env_animsprite);

LINK_ENTITY_TO_GAME(env_beam);

LINK_ENTITY_TO_GAME(env_beamtrail);

LINK_ENTITY_TO_GAME(env_beverage);

LINK_ENTITY_TO_GAME(env_blood);

LINK_ENTITY_TO_GAME(env_blowercannon);

LINK_ENTITY_TO_GAME(env_bombglow);

LINK_ENTITY_TO_GAME(env_bubbles);

LINK_ENTITY_TO_GAME(env_customize);

LINK_ENTITY_TO_GAME(env_debris);

LINK_ENTITY_TO_GAME(env_decal);

LINK_ENTITY_TO_GAME(env_dlight);

LINK_ENTITY_TO_GAME(env_electrified_wire);

LINK_ENTITY_TO_GAME(env_elight);

LINK_ENTITY_TO_GAME(env_explosion);

LINK_ENTITY_TO_GAME(env_fade);

LINK_ENTITY_TO_GAME(env_fadeone);

LINK_ENTITY_TO_GAME(env_fire);

LINK_ENTITY_TO_GAME(env_flag);

LINK_ENTITY_TO_GAME(env_fog);

LINK_ENTITY_TO_GAME(env_footsteps);

LINK_ENTITY_TO_GAME(env_fountain);

LINK_ENTITY_TO_GAME(env_funnel);

LINK_ENTITY_TO_GAME(env_gamma);

LINK_ENTITY_TO_GAME(env_genewormcloud);

LINK_ENTITY_TO_GAME(env_genewormspawn);

LINK_ENTITY_TO_GAME(env_global);

LINK_ENTITY_TO_GAME(env_glow);

LINK_ENTITY_TO_GAME(env_laser);

LINK_ENTITY_TO_GAME(env_lensflare);

LINK_ENTITY_TO_GAME(env_lightning);

LINK_ENTITY_TO_GAME(env_message);

LINK_ENTITY_TO_GAME(env_model);

LINK_ENTITY_TO_GAME(env_mp3);

LINK_ENTITY_TO_GAME(env_music);

LINK_ENTITY_TO_GAME(env_particle);

LINK_ENTITY_TO_GAME(env_particleemitter);

LINK_ENTITY_TO_GAME(env_particles);

LINK_ENTITY_TO_GAME(env_particles_custom);

LINK_ENTITY_TO_GAME(env_particule);

LINK_ENTITY_TO_GAME(env_precip);

LINK_ENTITY_TO_GAME(env_push);

LINK_ENTITY_TO_GAME(env_quakefx);

LINK_ENTITY_TO_GAME(env_rain);

LINK_ENTITY_TO_GAME(env_render);

LINK_ENTITY_TO_GAME(env_rocketshooter);

LINK_ENTITY_TO_GAME(env_rope);

LINK_ENTITY_TO_GAME(env_sentence);

LINK_ENTITY_TO_GAME(env_shake);

LINK_ENTITY_TO_GAME(env_shockwave);

LINK_ENTITY_TO_GAME(env_shooter);

LINK_ENTITY_TO_GAME(env_sky);

LINK_ENTITY_TO_GAME(env_smoker);

LINK_ENTITY_TO_GAME(env_snow);

LINK_ENTITY_TO_GAME(env_sound);

LINK_ENTITY_TO_GAME(env_spark);

LINK_ENTITY_TO_GAME(env_spawnereffect);

LINK_ENTITY_TO_GAME(env_sprite);

LINK_ENTITY_TO_GAME(env_spriteshooter);

LINK_ENTITY_TO_GAME(env_spritetrain);

LINK_ENTITY_TO_GAME(env_state);

LINK_ENTITY_TO_GAME(env_sun);

LINK_ENTITY_TO_GAME(env_timer);

LINK_ENTITY_TO_GAME(env_vgui);

LINK_ENTITY_TO_GAME(env_warpball);

LINK_ENTITY_TO_GAME(env_weather);

LINK_ENTITY_TO_GAME(env_xenmaker);

LINK_ENTITY_TO_GAME(equipment_parachute);

LINK_ENTITY_TO_GAME(event_player_die);

LINK_ENTITY_TO_GAME(event_player_join);

LINK_ENTITY_TO_GAME(event_player_kill);

LINK_ENTITY_TO_GAME(event_player_leave);

LINK_ENTITY_TO_GAME(event_player_spawn);

LINK_ENTITY_TO_GAME(event_round_end);

LINK_ENTITY_TO_GAME(event_round_prestart);

LINK_ENTITY_TO_GAME(event_round_reset);

LINK_ENTITY_TO_GAME(event_round_start);

LINK_ENTITY_TO_GAME(event_team_die);

LINK_ENTITY_TO_GAME(event_team_win);

LINK_ENTITY_TO_GAME(explode_bolt);

LINK_ENTITY_TO_GAME(explodingattack);

LINK_ENTITY_TO_GAME(explosive_slug);

LINK_ENTITY_TO_GAME(external_cam);

LINK_ENTITY_TO_GAME(extra_ammo);

LINK_ENTITY_TO_GAME(fa_drop_zone);

LINK_ENTITY_TO_GAME(fa_parachute);

LINK_ENTITY_TO_GAME(fa_push_flag);

LINK_ENTITY_TO_GAME(fa_push_point);

LINK_ENTITY_TO_GAME(fa_sd_object);

LINK_ENTITY_TO_GAME(fa_team_goal);

LINK_ENTITY_TO_GAME(fa_team_item);

LINK_ENTITY_TO_GAME(fading_corpse);

LINK_ENTITY_TO_GAME(fev_cloud);

LINK_ENTITY_TO_GAME(fev_vial);

LINK_ENTITY_TO_GAME(final_flash);

LINK_ENTITY_TO_GAME(finish);

LINK_ENTITY_TO_GAME(fire_ball);

LINK_ENTITY_TO_GAME(fireanddie);

LINK_ENTITY_TO_GAME(firework);

LINK_ENTITY_TO_GAME(flag);

LINK_ENTITY_TO_GAME(flag_capture_point);

LINK_ENTITY_TO_GAME(flag_follow);

LINK_ENTITY_TO_GAME(flag_world);

LINK_ENTITY_TO_GAME(flak_bomb);

LINK_ENTITY_TO_GAME(flak_grenade);

LINK_ENTITY_TO_GAME(flak_shard);

LINK_ENTITY_TO_GAME(flakfrag);

LINK_ENTITY_TO_GAME(flame);

LINK_ENTITY_TO_GAME(flame_ball);

LINK_ENTITY_TO_GAME(flame_chunk);

LINK_ENTITY_TO_GAME(flamme);

LINK_ENTITY_TO_GAME(flare);

LINK_ENTITY_TO_GAME(flare_fire);

LINK_ENTITY_TO_GAME(flying_axe);

LINK_ENTITY_TO_GAME(flying_crowbar);

LINK_ENTITY_TO_GAME(fog);

LINK_ENTITY_TO_GAME(follow_ent);

LINK_ENTITY_TO_GAME(followbot);

LINK_ENTITY_TO_GAME(follower_basic);

LINK_ENTITY_TO_GAME(football);

LINK_ENTITY_TO_GAME(frag_grenade);

LINK_ENTITY_TO_GAME(freeze_beam);

LINK_ENTITY_TO_GAME(freeze_gel);

LINK_ENTITY_TO_GAME(freeze_grenade);

LINK_ENTITY_TO_GAME(freeze_timer);

LINK_ENTITY_TO_GAME(friction_timer);

LINK_ENTITY_TO_GAME(frostball);

LINK_ENTITY_TO_GAME(fruit_sacrifice_point_team_green);

LINK_ENTITY_TO_GAME(fruit_sacrifice_point_team_red);

LINK_ENTITY_TO_GAME(func_airstrike_area);

LINK_ENTITY_TO_GAME(func_ammoprovider);

LINK_ENTITY_TO_GAME(func_ammostripper);

LINK_ENTITY_TO_GAME(func_bankteller);

LINK_ENTITY_TO_GAME(func_base);

LINK_ENTITY_TO_GAME(func_bomb_target);

LINK_ENTITY_TO_GAME(func_breakable);

LINK_ENTITY_TO_GAME(func_breakpoints);

LINK_ENTITY_TO_GAME(func_button);

LINK_ENTITY_TO_GAME(func_buyzone);

LINK_ENTITY_TO_GAME(func_capture_area);

LINK_ENTITY_TO_GAME(func_capturepoint);

LINK_ENTITY_TO_GAME(func_clouds);

LINK_ENTITY_TO_GAME(func_conveyor);

LINK_ENTITY_TO_GAME(func_corruptsecurity);

LINK_ENTITY_TO_GAME(func_ctb);

LINK_ENTITY_TO_GAME(func_destroy_objective);

LINK_ENTITY_TO_GAME(func_disctoggle);

LINK_ENTITY_TO_GAME(func_door);

LINK_ENTITY_TO_GAME(func_door2);

LINK_ENTITY_TO_GAME(func_door_rotating);

LINK_ENTITY_TO_GAME(func_elevator);

LINK_ENTITY_TO_GAME(func_escapezone);

LINK_ENTITY_TO_GAME(func_friction);

LINK_ENTITY_TO_GAME(func_goal_zone);

LINK_ENTITY_TO_GAME(func_goalpoint);

LINK_ENTITY_TO_GAME(func_golem_body);

LINK_ENTITY_TO_GAME(func_golem_controls_attack);

LINK_ENTITY_TO_GAME(func_golem_controls_forward);

LINK_ENTITY_TO_GAME(func_golem_controls_left);

LINK_ENTITY_TO_GAME(func_golem_controls_right);

LINK_ENTITY_TO_GAME(func_golem_leftarm);

LINK_ENTITY_TO_GAME(func_golem_leftleg);

LINK_ENTITY_TO_GAME(func_golem_rightarm);

LINK_ENTITY_TO_GAME(func_golem_rightleg);

LINK_ENTITY_TO_GAME(func_grencatch);

LINK_ENTITY_TO_GAME(func_guntarget);

LINK_ENTITY_TO_GAME(func_healthcharger);

LINK_ENTITY_TO_GAME(func_hostage_rescue);

LINK_ENTITY_TO_GAME(func_illusionary);

LINK_ENTITY_TO_GAME(func_jail_breakable);

LINK_ENTITY_TO_GAME(func_jail_button);

LINK_ENTITY_TO_GAME(func_jail_door);

LINK_ENTITY_TO_GAME(func_jail_door_rotating);

LINK_ENTITY_TO_GAME(func_jail_pushable);

LINK_ENTITY_TO_GAME(func_jail_release);

LINK_ENTITY_TO_GAME(func_ladder);

LINK_ENTITY_TO_GAME(func_laddertype);

LINK_ENTITY_TO_GAME(func_land_transport);

LINK_ENTITY_TO_GAME(func_landtransport);

LINK_ENTITY_TO_GAME(func_modeltank);

LINK_ENTITY_TO_GAME(func_modelvehicle);

LINK_ENTITY_TO_GAME(func_modelvehiclemodel);

LINK_ENTITY_TO_GAME(func_monsterclip);

LINK_ENTITY_TO_GAME(func_mortar_field);

LINK_ENTITY_TO_GAME(func_music);

LINK_ENTITY_TO_GAME(func_needs);

LINK_ENTITY_TO_GAME(func_needs_boredom);

LINK_ENTITY_TO_GAME(func_needs_dirty);

LINK_ENTITY_TO_GAME(func_needs_drink);

LINK_ENTITY_TO_GAME(func_needs_eat);

LINK_ENTITY_TO_GAME(func_needs_fatigue);

LINK_ENTITY_TO_GAME(func_needs_lonely);

LINK_ENTITY_TO_GAME(func_needs_pee);

LINK_ENTITY_TO_GAME(func_needs_poo);

LINK_ENTITY_TO_GAME(func_nobuild);

LINK_ENTITY_TO_GAME(func_nogrenades);

LINK_ENTITY_TO_GAME(func_op4mortarcontroller);

LINK_ENTITY_TO_GAME(func_particle_system);

LINK_ENTITY_TO_GAME(func_pendulum);

LINK_ENTITY_TO_GAME(func_physics);

LINK_ENTITY_TO_GAME(func_pillageable);

LINK_ENTITY_TO_GAME(func_plat);

LINK_ENTITY_TO_GAME(func_plat_toggleremove);

LINK_ENTITY_TO_GAME(func_platrot);

LINK_ENTITY_TO_GAME(func_player_start);

LINK_ENTITY_TO_GAME(func_point);

LINK_ENTITY_TO_GAME(func_possesion);

LINK_ENTITY_TO_GAME(func_protect);

LINK_ENTITY_TO_GAME(func_pushable);

LINK_ENTITY_TO_GAME(func_rain);

LINK_ENTITY_TO_GAME(func_recharge);

LINK_ENTITY_TO_GAME(func_reset);

LINK_ENTITY_TO_GAME(func_resource);

LINK_ENTITY_TO_GAME(func_rot_button);

LINK_ENTITY_TO_GAME(func_rotating);

LINK_ENTITY_TO_GAME(func_searchlight);

LINK_ENTITY_TO_GAME(func_securityzone);

LINK_ENTITY_TO_GAME(func_seethrough);

LINK_ENTITY_TO_GAME(func_seethroughdoor);

LINK_ENTITY_TO_GAME(func_shine);

LINK_ENTITY_TO_GAME(func_sickbay_bed);

LINK_ENTITY_TO_GAME(func_sickbay_psych);

LINK_ENTITY_TO_GAME(func_snow);

LINK_ENTITY_TO_GAME(func_tank);

LINK_ENTITY_TO_GAME(func_tank_gauss);

LINK_ENTITY_TO_GAME(func_tank_of);

LINK_ENTITY_TO_GAME(func_tank_turret);

LINK_ENTITY_TO_GAME(func_tankbase);

LINK_ENTITY_TO_GAME(func_tankcannon);

LINK_ENTITY_TO_GAME(func_tankcontrols);

LINK_ENTITY_TO_GAME(func_tankcontrols_of);

LINK_ENTITY_TO_GAME(func_tankdrive);

LINK_ENTITY_TO_GAME(func_tankgib);

LINK_ENTITY_TO_GAME(func_tankgib_gib);

LINK_ENTITY_TO_GAME(func_tankgoauld);

LINK_ENTITY_TO_GAME(func_tanklaser);

LINK_ENTITY_TO_GAME(func_tanklaser_of);

LINK_ENTITY_TO_GAME(func_tankmg);

LINK_ENTITY_TO_GAME(func_tankmortar);

LINK_ENTITY_TO_GAME(func_tankmortar_of);

LINK_ENTITY_TO_GAME(func_tankrocket);

LINK_ENTITY_TO_GAME(func_tankrocket_of);

LINK_ENTITY_TO_GAME(func_tanktarget);

LINK_ENTITY_TO_GAME(func_tbutton);

LINK_ENTITY_TO_GAME(func_tech_breakable);

LINK_ENTITY_TO_GAME(func_trackautochange);

LINK_ENTITY_TO_GAME(func_trackchange);

LINK_ENTITY_TO_GAME(func_tracktrain);

LINK_ENTITY_TO_GAME(func_train);

LINK_ENTITY_TO_GAME(func_traincontrols);

LINK_ENTITY_TO_GAME(func_trenchcap1);

LINK_ENTITY_TO_GAME(func_trenchcap2);

LINK_ENTITY_TO_GAME(func_trenchcap3);

LINK_ENTITY_TO_GAME(func_trenchcap4);

LINK_ENTITY_TO_GAME(func_use);

LINK_ENTITY_TO_GAME(func_vehicle);

LINK_ENTITY_TO_GAME(func_vehiclecontrols);

LINK_ENTITY_TO_GAME(func_vehicletankcontrols);

LINK_ENTITY_TO_GAME(func_vine);

LINK_ENTITY_TO_GAME(func_vip_safetyzone);

LINK_ENTITY_TO_GAME(func_wall);

LINK_ENTITY_TO_GAME(func_wall_toggle);

LINK_ENTITY_TO_GAME(func_wardrobe);

LINK_ENTITY_TO_GAME(func_water);

LINK_ENTITY_TO_GAME(func_water_up);

LINK_ENTITY_TO_GAME(func_weaponcheck);

LINK_ENTITY_TO_GAME(func_weldable);

LINK_ENTITY_TO_GAME(func_wind);

LINK_ENTITY_TO_GAME(game_clock);

LINK_ENTITY_TO_GAME(game_counter);

LINK_ENTITY_TO_GAME(game_counter_set);

LINK_ENTITY_TO_GAME(game_end);

LINK_ENTITY_TO_GAME(game_endbattle);

LINK_ENTITY_TO_GAME(game_item);

LINK_ENTITY_TO_GAME(game_levelmusic);

LINK_ENTITY_TO_GAME(game_nuke);

LINK_ENTITY_TO_GAME(game_player_die);

LINK_ENTITY_TO_GAME(game_player_equip);

LINK_ENTITY_TO_GAME(game_player_hurt);

LINK_ENTITY_TO_GAME(game_player_message);

LINK_ENTITY_TO_GAME(game_player_team);

LINK_ENTITY_TO_GAME(game_playerdie);

LINK_ENTITY_TO_GAME(game_roundend);

LINK_ENTITY_TO_GAME(game_score);

LINK_ENTITY_TO_GAME(game_team_master);

LINK_ENTITY_TO_GAME(game_team_score);

LINK_ENTITY_TO_GAME(game_team_set);

LINK_ENTITY_TO_GAME(game_team_win);

LINK_ENTITY_TO_GAME(game_text);

LINK_ENTITY_TO_GAME(game_zone_pirscore);

LINK_ENTITY_TO_GAME(game_zone_player);

LINK_ENTITY_TO_GAME(game_zone_territory);

LINK_ENTITY_TO_GAME(game_zone_vikscore);

LINK_ENTITY_TO_GAME(gameclock);

LINK_ENTITY_TO_GAME(garg_hull);

LINK_ENTITY_TO_GAME(garg_stomp);

LINK_ENTITY_TO_GAME(gauss_becon);

LINK_ENTITY_TO_GAME(generic_model);

LINK_ENTITY_TO_GAME(getGlobalStunLevel);

LINK_ENTITY_TO_GAME(ghost);

LINK_ENTITY_TO_GAME(gibable_corpse);

LINK_ENTITY_TO_GAME(gibshooter);

LINK_ENTITY_TO_GAME(glock_gib);

LINK_ENTITY_TO_GAME(glock_stickygib);

LINK_ENTITY_TO_GAME(gluon);

LINK_ENTITY_TO_GAME(gluon2);

LINK_ENTITY_TO_GAME(gonarch_headcrabifier);

LINK_ENTITY_TO_GAME(gonomeguts);

LINK_ENTITY_TO_GAME(gonomespit);

LINK_ENTITY_TO_GAME(gr_shell);

LINK_ENTITY_TO_GAME(grapple_bolt);

LINK_ENTITY_TO_GAME(grapple_hook);

LINK_ENTITY_TO_GAME(grapple_tip);

LINK_ENTITY_TO_GAME(grappletongue);

LINK_ENTITY_TO_GAME(gravity_grenade);

LINK_ENTITY_TO_GAME(gren_projectile);

LINK_ENTITY_TO_GAME(gren_smoke);

LINK_ENTITY_TO_GAME(grenade);

LINK_ENTITY_TO_GAME(guard_info);

LINK_ENTITY_TO_GAME(guidedattack);

LINK_ENTITY_TO_GAME(gunmancycler);

LINK_ENTITY_TO_GAME(hallucgrenade);

LINK_ENTITY_TO_GAME(halo_base);

LINK_ENTITY_TO_GAME(hand_knife);

LINK_ENTITY_TO_GAME(he_charge_flare);

LINK_ENTITY_TO_GAME(he_eye_flare);

LINK_ENTITY_TO_GAME(headcrab_teather);

LINK_ENTITY_TO_GAME(hegrenade);

LINK_ENTITY_TO_GAME(hellfire);

LINK_ENTITY_TO_GAME(hi_bomb_zone);

LINK_ENTITY_TO_GAME(hi_button_load);

LINK_ENTITY_TO_GAME(hi_coop_tango);

LINK_ENTITY_TO_GAME(hi_fog);

LINK_ENTITY_TO_GAME(hi_global);

LINK_ENTITY_TO_GAME(hi_hostage);

LINK_ENTITY_TO_GAME(hi_hostage_rescue);

LINK_ENTITY_TO_GAME(hi_hostage_respawn);

LINK_ENTITY_TO_GAME(hi_nameposition);

LINK_ENTITY_TO_GAME(hi_objective);

LINK_ENTITY_TO_GAME(hi_objective_manager);

LINK_ENTITY_TO_GAME(hi_round_timer);

LINK_ENTITY_TO_GAME(hi_spawn_hostage);

LINK_ENTITY_TO_GAME(hi_spawn_nato);

LINK_ENTITY_TO_GAME(hi_spawn_observer);

LINK_ENTITY_TO_GAME(hi_spawn_tango);

LINK_ENTITY_TO_GAME(hlight);

LINK_ENTITY_TO_GAME(hltr_assaut_boutton);

LINK_ENTITY_TO_GAME(hltr_hostage);

LINK_ENTITY_TO_GAME(hltr_object);

LINK_ENTITY_TO_GAME(hltr_object_tombe);

LINK_ENTITY_TO_GAME(hltr_objectcapture);

LINK_ENTITY_TO_GAME(hltr_objectif);

LINK_ENTITY_TO_GAME(hltr_objectif_num);

LINK_ENTITY_TO_GAME(hltr_passant);

LINK_ENTITY_TO_GAME(hltr_spectator_start);

LINK_ENTITY_TO_GAME(hltr_turret);

LINK_ENTITY_TO_GAME(hltr_waypoint);

LINK_ENTITY_TO_GAME(holo_player);

LINK_ENTITY_TO_GAME(hologram_beak);

LINK_ENTITY_TO_GAME(hologram_damage);

LINK_ENTITY_TO_GAME(holywars_rocket);

LINK_ENTITY_TO_GAME(homing_balls);

LINK_ENTITY_TO_GAME(hornet);

LINK_ENTITY_TO_GAME(hospital);

LINK_ENTITY_TO_GAME(hostage_entity);

LINK_ENTITY_TO_GAME(hud_sprite);

LINK_ENTITY_TO_GAME(human_flashgrenade);

LINK_ENTITY_TO_GAME(hvr_blkop_rocket);

LINK_ENTITY_TO_GAME(hvr_rocket);

LINK_ENTITY_TO_GAME(i_p_t);

LINK_ENTITY_TO_GAME(i_t_g);

LINK_ENTITY_TO_GAME(i_t_t);

LINK_ENTITY_TO_GAME(i_w_g);

LINK_ENTITY_TO_GAME(i_w_t);

LINK_ENTITY_TO_GAME(iflame);

LINK_ENTITY_TO_GAME(impact_sprite);

LINK_ENTITY_TO_GAME(impail_arrow);

LINK_ENTITY_TO_GAME(impailer);

LINK_ENTITY_TO_GAME(impgren);

LINK_ENTITY_TO_GAME(implode_timer);

LINK_ENTITY_TO_GAME(in_shell);

LINK_ENTITY_TO_GAME(incendiary_rocket);

LINK_ENTITY_TO_GAME(indian_arrow);

LINK_ENTITY_TO_GAME(infect);

LINK_ENTITY_TO_GAME(info_Waterdetect);

LINK_ENTITY_TO_GAME(info_administrator);

LINK_ENTITY_TO_GAME(info_airstrike_node);

LINK_ENTITY_TO_GAME(info_alarm);

LINK_ENTITY_TO_GAME(info_alias);

LINK_ENTITY_TO_GAME(info_alien_return);

LINK_ENTITY_TO_GAME(info_alien_start);

LINK_ENTITY_TO_GAME(info_american_line);

LINK_ENTITY_TO_GAME(info_areadef);

LINK_ENTITY_TO_GAME(info_areamarker);

LINK_ENTITY_TO_GAME(info_ball_start);

LINK_ENTITY_TO_GAME(info_banana_green_capture);

LINK_ENTITY_TO_GAME(info_banana_red_capture);

LINK_ENTITY_TO_GAME(info_bananier);

LINK_ENTITY_TO_GAME(info_bg_detect);

LINK_ENTITY_TO_GAME(info_bigmomma);

LINK_ENTITY_TO_GAME(info_bomb_target);

LINK_ENTITY_TO_GAME(info_bone);

LINK_ENTITY_TO_GAME(info_bone_spawn);

LINK_ENTITY_TO_GAME(info_boobytrap);

LINK_ENTITY_TO_GAME(info_botlandmark);

LINK_ENTITY_TO_GAME(info_botnode);

LINK_ENTITY_TO_GAME(info_british_line);

LINK_ENTITY_TO_GAME(info_button_phonebooth);

LINK_ENTITY_TO_GAME(info_caisse_128);

LINK_ENTITY_TO_GAME(info_caisse_16);

LINK_ENTITY_TO_GAME(info_caisse_256);

LINK_ENTITY_TO_GAME(info_caisse_32);

LINK_ENTITY_TO_GAME(info_caisse_64);

LINK_ENTITY_TO_GAME(info_camera);

LINK_ENTITY_TO_GAME(info_cap_rules);

LINK_ENTITY_TO_GAME(info_checkpoint);

LINK_ENTITY_TO_GAME(info_chicken);

LINK_ENTITY_TO_GAME(info_chip);

LINK_ENTITY_TO_GAME(info_civ_start);

LINK_ENTITY_TO_GAME(info_compile_parameters);

LINK_ENTITY_TO_GAME(info_css);

LINK_ENTITY_TO_GAME(info_ctb_controller);

LINK_ENTITY_TO_GAME(info_ctfdetect);

LINK_ENTITY_TO_GAME(info_ctfspawn);

LINK_ENTITY_TO_GAME(info_ctfspawn_powerup);

LINK_ENTITY_TO_GAME(info_curtain);

LINK_ENTITY_TO_GAME(info_deathball_spawn);

LINK_ENTITY_TO_GAME(info_defaultammo);

LINK_ENTITY_TO_GAME(info_displacer_earth_target);

LINK_ENTITY_TO_GAME(info_displacer_xen_target);

LINK_ENTITY_TO_GAME(info_dm );

LINK_ENTITY_TO_GAME(info_doddetect);

LINK_ENTITY_TO_GAME(info_dragonrider_spawn);

LINK_ENTITY_TO_GAME(info_draw_slayerswin);

LINK_ENTITY_TO_GAME(info_draw_vampireswin);

LINK_ENTITY_TO_GAME(info_drive_agrunt_spawn);

LINK_ENTITY_TO_GAME(info_drive_apache_spawn);

LINK_ENTITY_TO_GAME(info_drive_assassin_spawn);

LINK_ENTITY_TO_GAME(info_drive_bm_spawn);

LINK_ENTITY_TO_GAME(info_drive_bullsquid_spawn);

LINK_ENTITY_TO_GAME(info_drive_controller_spawn);

LINK_ENTITY_TO_GAME(info_drive_garg_spawn);

LINK_ENTITY_TO_GAME(info_drive_houndeye_spawn);

LINK_ENTITY_TO_GAME(info_drive_icky_spawn);

LINK_ENTITY_TO_GAME(info_drive_panth_spawn);

LINK_ENTITY_TO_GAME(info_drive_slave_spawn);

LINK_ENTITY_TO_GAME(info_drive_turret_spawn);

LINK_ENTITY_TO_GAME(info_elephant);

LINK_ENTITY_TO_GAME(info_elev_floor);

LINK_ENTITY_TO_GAME(info_escape_start);

LINK_ENTITY_TO_GAME(info_escape_target);

LINK_ENTITY_TO_GAME(info_evil_start);

LINK_ENTITY_TO_GAME(info_firearms_detect);

LINK_ENTITY_TO_GAME(info_firstround_spawn);

LINK_ENTITY_TO_GAME(info_flagspawn);

LINK_ENTITY_TO_GAME(info_flash);

LINK_ENTITY_TO_GAME(info_frontline);

LINK_ENTITY_TO_GAME(info_gameplay);

LINK_ENTITY_TO_GAME(info_gameplaylogic);

LINK_ENTITY_TO_GAME(info_gangsta_dm_start);

LINK_ENTITY_TO_GAME(info_goldeneye);

LINK_ENTITY_TO_GAME(info_good_start);

LINK_ENTITY_TO_GAME(info_group);

LINK_ENTITY_TO_GAME(info_gwGeneral_spawn);

LINK_ENTITY_TO_GAME(info_gwars_map);

LINK_ENTITY_TO_GAME(info_gwbriefing_spawn);

LINK_ENTITY_TO_GAME(info_gwinmap_spawn);

LINK_ENTITY_TO_GAME(info_gwplayer_spawn);

LINK_ENTITY_TO_GAME(info_gwwaiting_spawn);

LINK_ENTITY_TO_GAME(info_hlpaintctfdetect);

LINK_ENTITY_TO_GAME(info_hmctfdetect);

LINK_ENTITY_TO_GAME(info_hostage_rescue);

LINK_ENTITY_TO_GAME(info_hostage_safezone);

LINK_ENTITY_TO_GAME(info_human_return);

LINK_ENTITY_TO_GAME(info_human_start);

LINK_ENTITY_TO_GAME(info_initial_player_allies);

LINK_ENTITY_TO_GAME(info_initial_player_axis);

LINK_ENTITY_TO_GAME(info_intermission);

LINK_ENTITY_TO_GAME(info_italian_start);

LINK_ENTITY_TO_GAME(info_jail_start);

LINK_ENTITY_TO_GAME(info_join_autoassign);

LINK_ENTITY_TO_GAME(info_join_team);

LINK_ENTITY_TO_GAME(info_jumppad_target);

LINK_ENTITY_TO_GAME(info_jwhite_capture);

LINK_ENTITY_TO_GAME(info_jwhite_escape);

LINK_ENTITY_TO_GAME(info_jwhite_rescue);

LINK_ENTITY_TO_GAME(info_kb_deathmatch);

LINK_ENTITY_TO_GAME(info_lacrymo);

LINK_ENTITY_TO_GAME(info_landmark);

LINK_ENTITY_TO_GAME(info_landmine);

LINK_ENTITY_TO_GAME(info_leave_game);

LINK_ENTITY_TO_GAME(info_location);

LINK_ENTITY_TO_GAME(info_map_parameters);

LINK_ENTITY_TO_GAME(info_mapdetect);

LINK_ENTITY_TO_GAME(info_mapinfo);

LINK_ENTITY_TO_GAME(info_marguerite);

LINK_ENTITY_TO_GAME(info_marker);

LINK_ENTITY_TO_GAME(info_minefield);

LINK_ENTITY_TO_GAME(info_model);

LINK_ENTITY_TO_GAME(info_money_start);

LINK_ENTITY_TO_GAME(info_monkey_start_team_green);

LINK_ENTITY_TO_GAME(info_monkey_start_team_red);

LINK_ENTITY_TO_GAME(info_monster_return);

LINK_ENTITY_TO_GAME(info_monster_start);

LINK_ENTITY_TO_GAME(info_morbid);

LINK_ENTITY_TO_GAME(info_morbid_start);

LINK_ENTITY_TO_GAME(info_movewith);

LINK_ENTITY_TO_GAME(info_msdetect);

LINK_ENTITY_TO_GAME(info_multiple);

LINK_ENTITY_TO_GAME(info_next_map);

LINK_ENTITY_TO_GAME(info_node);

LINK_ENTITY_TO_GAME(info_node_air);

LINK_ENTITY_TO_GAME(info_nomines);

LINK_ENTITY_TO_GAME(info_null);

LINK_ENTITY_TO_GAME(info_objective);

LINK_ENTITY_TO_GAME(info_observer);

LINK_ENTITY_TO_GAME(info_observer_start);

LINK_ENTITY_TO_GAME(info_orientation);

LINK_ENTITY_TO_GAME(info_paintball);

LINK_ENTITY_TO_GAME(info_particles);

LINK_ENTITY_TO_GAME(info_physics);

LINK_ENTITY_TO_GAME(info_pitworm);

LINK_ENTITY_TO_GAME(info_pitworm_steam_lock);

LINK_ENTITY_TO_GAME(info_player_allies);

LINK_ENTITY_TO_GAME(info_player_american);

LINK_ENTITY_TO_GAME(info_player_attacker);

LINK_ENTITY_TO_GAME(info_player_axis);

LINK_ENTITY_TO_GAME(info_player_blue);

LINK_ENTITY_TO_GAME(info_player_british);

LINK_ENTITY_TO_GAME(info_player_coop);

LINK_ENTITY_TO_GAME(info_player_deadstart);

LINK_ENTITY_TO_GAME(info_player_deathmatch);

LINK_ENTITY_TO_GAME(info_player_deathmatch_reverse);

LINK_ENTITY_TO_GAME(info_player_defender);

LINK_ENTITY_TO_GAME(info_player_dm2);

LINK_ENTITY_TO_GAME(info_player_equip);

LINK_ENTITY_TO_GAME(info_player_goauld);

LINK_ENTITY_TO_GAME(info_player_jail);

LINK_ENTITY_TO_GAME(info_player_judge);

LINK_ENTITY_TO_GAME(info_player_knight);

LINK_ENTITY_TO_GAME(info_player_marine);

LINK_ENTITY_TO_GAME(info_player_nva_reenforcement);

LINK_ENTITY_TO_GAME(info_player_observer);

LINK_ENTITY_TO_GAME(info_player_perp);

LINK_ENTITY_TO_GAME(info_player_pirate);

LINK_ENTITY_TO_GAME(info_player_red);

LINK_ENTITY_TO_GAME(info_player_respawn);

LINK_ENTITY_TO_GAME(info_player_slayer);

LINK_ENTITY_TO_GAME(info_player_spectator);

LINK_ENTITY_TO_GAME(info_player_start);

LINK_ENTITY_TO_GAME(info_player_start_prox);

LINK_ENTITY_TO_GAME(info_player_start_reverse);

LINK_ENTITY_TO_GAME(info_player_team1);

LINK_ENTITY_TO_GAME(info_player_team2);

LINK_ENTITY_TO_GAME(info_player_team_at);

LINK_ENTITY_TO_GAME(info_player_team_bt);

LINK_ENTITY_TO_GAME(info_player_team_ce);

LINK_ENTITY_TO_GAME(info_player_team_cm);

LINK_ENTITY_TO_GAME(info_player_team_gf);

LINK_ENTITY_TO_GAME(info_player_team_iw);

LINK_ENTITY_TO_GAME(info_player_team_s);

LINK_ENTITY_TO_GAME(info_player_team_sm);

LINK_ENTITY_TO_GAME(info_player_team_st);

LINK_ENTITY_TO_GAME(info_player_team_we);

LINK_ENTITY_TO_GAME(info_player_teamspawn);

LINK_ENTITY_TO_GAME(info_player_torri);

LINK_ENTITY_TO_GAME(info_player_unas);

LINK_ENTITY_TO_GAME(info_player_usmc_reenforcement);

LINK_ENTITY_TO_GAME(info_player_vampire);

LINK_ENTITY_TO_GAME(info_player_viking);

LINK_ENTITY_TO_GAME(info_playerstart_blue);

LINK_ENTITY_TO_GAME(info_playerstart_red);

LINK_ENTITY_TO_GAME(info_pvk);

LINK_ENTITY_TO_GAME(info_radar_block);

LINK_ENTITY_TO_GAME(info_rescue_monster);

LINK_ENTITY_TO_GAME(info_rescue_point);

LINK_ENTITY_TO_GAME(info_reset);

LINK_ENTITY_TO_GAME(info_robot_return);

LINK_ENTITY_TO_GAME(info_robot_start);

LINK_ENTITY_TO_GAME(info_roundx_spawn);

LINK_ENTITY_TO_GAME(info_russian_start);

LINK_ENTITY_TO_GAME(info_sapin);

LINK_ENTITY_TO_GAME(info_scientist);

LINK_ENTITY_TO_GAME(info_scientist_dead);

LINK_ENTITY_TO_GAME(info_scientist_start);

LINK_ENTITY_TO_GAME(info_semi_null);

LINK_ENTITY_TO_GAME(info_sidetect);

LINK_ENTITY_TO_GAME(info_spawn_container);

LINK_ENTITY_TO_GAME(info_spectate);

LINK_ENTITY_TO_GAME(info_sprite);

LINK_ENTITY_TO_GAME(info_stadium);

LINK_ENTITY_TO_GAME(info_status);

LINK_ENTITY_TO_GAME(info_swat_start);

LINK_ENTITY_TO_GAME(info_target);

LINK_ENTITY_TO_GAME(info_team);

LINK_ENTITY_TO_GAME(info_team1_corner0);

LINK_ENTITY_TO_GAME(info_team1_corner1);

LINK_ENTITY_TO_GAME(info_team1_corner_player0);

LINK_ENTITY_TO_GAME(info_team1_corner_player1);

LINK_ENTITY_TO_GAME(info_team1_goalkick0);

LINK_ENTITY_TO_GAME(info_team1_goalkick1);

LINK_ENTITY_TO_GAME(info_team1_penalty_spot);

LINK_ENTITY_TO_GAME(info_team1_player1);

LINK_ENTITY_TO_GAME(info_team1_player10);

LINK_ENTITY_TO_GAME(info_team1_player11);

LINK_ENTITY_TO_GAME(info_team1_player2);

LINK_ENTITY_TO_GAME(info_team1_player3);

LINK_ENTITY_TO_GAME(info_team1_player4);

LINK_ENTITY_TO_GAME(info_team1_player5);

LINK_ENTITY_TO_GAME(info_team1_player6);

LINK_ENTITY_TO_GAME(info_team1_player7);

LINK_ENTITY_TO_GAME(info_team1_player8);

LINK_ENTITY_TO_GAME(info_team1_player9);

LINK_ENTITY_TO_GAME(info_team2_corner0);

LINK_ENTITY_TO_GAME(info_team2_corner1);

LINK_ENTITY_TO_GAME(info_team2_corner_player0);

LINK_ENTITY_TO_GAME(info_team2_corner_player1);

LINK_ENTITY_TO_GAME(info_team2_goalkick0);

LINK_ENTITY_TO_GAME(info_team2_goalkick1);

LINK_ENTITY_TO_GAME(info_team2_penalty_spot);

LINK_ENTITY_TO_GAME(info_team2_player1);

LINK_ENTITY_TO_GAME(info_team2_player10);

LINK_ENTITY_TO_GAME(info_team2_player11);

LINK_ENTITY_TO_GAME(info_team2_player2);

LINK_ENTITY_TO_GAME(info_team2_player3);

LINK_ENTITY_TO_GAME(info_team2_player4);

LINK_ENTITY_TO_GAME(info_team2_player5);

LINK_ENTITY_TO_GAME(info_team2_player6);

LINK_ENTITY_TO_GAME(info_team2_player7);

LINK_ENTITY_TO_GAME(info_team2_player8);

LINK_ENTITY_TO_GAME(info_team2_player9);

LINK_ENTITY_TO_GAME(info_team_base);

LINK_ENTITY_TO_GAME(info_team_ronin);

LINK_ENTITY_TO_GAME(info_team_scavenger);

LINK_ENTITY_TO_GAME(info_team_start);

LINK_ENTITY_TO_GAME(info_team_usmc);

LINK_ENTITY_TO_GAME(info_teamplaydetect);

LINK_ENTITY_TO_GAME(info_teamspawn);

LINK_ENTITY_TO_GAME(info_teleport_destination);

LINK_ENTITY_TO_GAME(info_texlights);

LINK_ENTITY_TO_GAME(info_tf_teamcheck);

LINK_ENTITY_TO_GAME(info_tf_teamset);

LINK_ENTITY_TO_GAME(info_tfdetect);

LINK_ENTITY_TO_GAME(info_tfgoal);

LINK_ENTITY_TO_GAME(info_tfgoal_timer);

LINK_ENTITY_TO_GAME(info_throw_in);

LINK_ENTITY_TO_GAME(info_transistor);

LINK_ENTITY_TO_GAME(info_transport_node);

LINK_ENTITY_TO_GAME(info_tt_gameplay);

LINK_ENTITY_TO_GAME(info_upko_start);

LINK_ENTITY_TO_GAME(info_usa_start);

LINK_ENTITY_TO_GAME(info_vgui_start);

LINK_ENTITY_TO_GAME(info_vip_start);

LINK_ENTITY_TO_GAME(info_wanteddetect);

LINK_ENTITY_TO_GAME(info_waypoint);

LINK_ENTITY_TO_GAME(info_ww_teamcheck);

LINK_ENTITY_TO_GAME(info_ww_teamset);

LINK_ENTITY_TO_GAME(info_wwdetect);

LINK_ENTITY_TO_GAME(info_wwgoal);

LINK_ENTITY_TO_GAME(info_wwgoal_timer);

LINK_ENTITY_TO_GAME(info_zone);

LINK_ENTITY_TO_GAME(info_zone_caisse);

LINK_ENTITY_TO_GAME(infodecal);

LINK_ENTITY_TO_GAME(inout_register);

LINK_ENTITY_TO_GAME(item_NVG);

LINK_ENTITY_TO_GAME(item_PowerShield);

LINK_ENTITY_TO_GAME(item_acme);

LINK_ENTITY_TO_GAME(item_airtank);

LINK_ENTITY_TO_GAME(item_ammo);

LINK_ENTITY_TO_GAME(item_ammoicon);

LINK_ENTITY_TO_GAME(item_ammopack);

LINK_ENTITY_TO_GAME(item_antidote);

LINK_ENTITY_TO_GAME(item_antidotee);

LINK_ENTITY_TO_GAME(item_antigrav);

LINK_ENTITY_TO_GAME(item_armor);

LINK_ENTITY_TO_GAME(item_armor1);

LINK_ENTITY_TO_GAME(item_armor2);

LINK_ENTITY_TO_GAME(item_armor3);

LINK_ENTITY_TO_GAME(item_armorInv);

LINK_ENTITY_TO_GAME(item_armour);

LINK_ENTITY_TO_GAME(item_artifact_envirosuit);

LINK_ENTITY_TO_GAME(item_artifact_invisibility);

LINK_ENTITY_TO_GAME(item_artifact_invulnerability);

LINK_ENTITY_TO_GAME(item_artifact_super_damage);

LINK_ENTITY_TO_GAME(item_assaultsuit);

LINK_ENTITY_TO_GAME(item_atde_object);

LINK_ENTITY_TO_GAME(item_autodoc);

LINK_ENTITY_TO_GAME(item_backpack);

LINK_ENTITY_TO_GAME(item_badge);

LINK_ENTITY_TO_GAME(item_bag1);

LINK_ENTITY_TO_GAME(item_bag2);

LINK_ENTITY_TO_GAME(item_banana);

LINK_ENTITY_TO_GAME(item_bandage);

LINK_ENTITY_TO_GAME(item_bandolier);

LINK_ENTITY_TO_GAME(item_baril);

LINK_ENTITY_TO_GAME(item_baril_bleu);

LINK_ENTITY_TO_GAME(item_baril_rouge);

LINK_ENTITY_TO_GAME(item_battery);

LINK_ENTITY_TO_GAME(item_beans);

LINK_ENTITY_TO_GAME(item_body);

LINK_ENTITY_TO_GAME(item_bonustime);

LINK_ENTITY_TO_GAME(item_bonustime2);

LINK_ENTITY_TO_GAME(item_bonustime3);

LINK_ENTITY_TO_GAME(item_book_team1);

LINK_ENTITY_TO_GAME(item_book_team2);

LINK_ENTITY_TO_GAME(item_bottle);

LINK_ENTITY_TO_GAME(item_briefcase);

LINK_ENTITY_TO_GAME(item_c4);

LINK_ENTITY_TO_GAME(item_cactus);

LINK_ENTITY_TO_GAME(item_caisse_arme);

LINK_ENTITY_TO_GAME(item_caisse_mega);

LINK_ENTITY_TO_GAME(item_caisse_outil);

LINK_ENTITY_TO_GAME(item_caisse_vie);

LINK_ENTITY_TO_GAME(item_canteen);

LINK_ENTITY_TO_GAME(item_capkey);

LINK_ENTITY_TO_GAME(item_cash);

LINK_ENTITY_TO_GAME(item_catalyst);

LINK_ENTITY_TO_GAME(item_cells);

LINK_ENTITY_TO_GAME(item_chainmail);

LINK_ENTITY_TO_GAME(item_chicken);

LINK_ENTITY_TO_GAME(item_claymore);

LINK_ENTITY_TO_GAME(item_cloak);

LINK_ENTITY_TO_GAME(item_cloaker);

LINK_ENTITY_TO_GAME(item_concussion);

LINK_ENTITY_TO_GAME(item_corpse);

LINK_ENTITY_TO_GAME(item_ctf_flag);

LINK_ENTITY_TO_GAME(item_ctfaccelerator);

LINK_ENTITY_TO_GAME(item_ctfbackpack);

LINK_ENTITY_TO_GAME(item_ctfbase);

LINK_ENTITY_TO_GAME(item_ctfflag);

LINK_ENTITY_TO_GAME(item_ctflongjump);

LINK_ENTITY_TO_GAME(item_ctfportablehev);

LINK_ENTITY_TO_GAME(item_ctfregeneration);

LINK_ENTITY_TO_GAME(item_dbldamage);

LINK_ENTITY_TO_GAME(item_dish);

LINK_ENTITY_TO_GAME(item_docbag);

LINK_ENTITY_TO_GAME(item_document);

LINK_ENTITY_TO_GAME(item_dragonball);

LINK_ENTITY_TO_GAME(item_dyno);

LINK_ENTITY_TO_GAME(item_dyno1);

LINK_ENTITY_TO_GAME(item_elixer);

LINK_ENTITY_TO_GAME(item_elixir);

LINK_ENTITY_TO_GAME(item_energy);

LINK_ENTITY_TO_GAME(item_flag);

LINK_ENTITY_TO_GAME(item_flag1);

LINK_ENTITY_TO_GAME(item_flag2);

LINK_ENTITY_TO_GAME(item_flag_atde);

LINK_ENTITY_TO_GAME(item_flag_blue);

LINK_ENTITY_TO_GAME(item_flag_center);

LINK_ENTITY_TO_GAME(item_flag_cf);

LINK_ENTITY_TO_GAME(item_flag_ctf);

LINK_ENTITY_TO_GAME(item_flag_red);

LINK_ENTITY_TO_GAME(item_flag_slayer);

LINK_ENTITY_TO_GAME(item_flag_team1);

LINK_ENTITY_TO_GAME(item_flag_team2);

LINK_ENTITY_TO_GAME(item_flag_vampire);

LINK_ENTITY_TO_GAME(item_flashbang);

LINK_ENTITY_TO_GAME(item_flashlight);

LINK_ENTITY_TO_GAME(item_food);

LINK_ENTITY_TO_GAME(item_frag);

LINK_ENTITY_TO_GAME(item_fruit);

LINK_ENTITY_TO_GAME(item_gascan);

LINK_ENTITY_TO_GAME(item_generic);

LINK_ENTITY_TO_GAME(item_genericammo);

LINK_ENTITY_TO_GAME(item_glass);

LINK_ENTITY_TO_GAME(item_grappin);

LINK_ENTITY_TO_GAME(item_head);

LINK_ENTITY_TO_GAME(item_health);

LINK_ENTITY_TO_GAME(item_healthkit);

LINK_ENTITY_TO_GAME(item_heavyarmor);

LINK_ENTITY_TO_GAME(item_herbs);

LINK_ENTITY_TO_GAME(item_highjump);

LINK_ENTITY_TO_GAME(item_holster);

LINK_ENTITY_TO_GAME(item_infjetpack);

LINK_ENTITY_TO_GAME(item_invincibility);

LINK_ENTITY_TO_GAME(item_invisibility);

LINK_ENTITY_TO_GAME(item_irnvg);

LINK_ENTITY_TO_GAME(item_jetpack);

LINK_ENTITY_TO_GAME(item_kevlar);

LINK_ENTITY_TO_GAME(item_knife);

LINK_ENTITY_TO_GAME(item_laser);

LINK_ENTITY_TO_GAME(item_leather);

LINK_ENTITY_TO_GAME(item_lgboiler);

LINK_ENTITY_TO_GAME(item_longjump);

LINK_ENTITY_TO_GAME(item_mask);

LINK_ENTITY_TO_GAME(item_medboiler);

LINK_ENTITY_TO_GAME(item_megavirus);

LINK_ENTITY_TO_GAME(item_metal);

LINK_ENTITY_TO_GAME(item_mine);

LINK_ENTITY_TO_GAME(item_money);

LINK_ENTITY_TO_GAME(item_moneycase);

LINK_ENTITY_TO_GAME(item_newspaper);

LINK_ENTITY_TO_GAME(item_nightvision);

LINK_ENTITY_TO_GAME(item_nuclearbomb);

LINK_ENTITY_TO_GAME(item_nuclearbombbutton);

LINK_ENTITY_TO_GAME(item_nuclearbombtimer);

LINK_ENTITY_TO_GAME(item_nvg);

LINK_ENTITY_TO_GAME(item_objective);

LINK_ENTITY_TO_GAME(item_papers);

LINK_ENTITY_TO_GAME(item_parachute);

LINK_ENTITY_TO_GAME(item_pickaxe);

LINK_ENTITY_TO_GAME(item_platemail);

LINK_ENTITY_TO_GAME(item_portableHEV);

LINK_ENTITY_TO_GAME(item_portableHealthkit);

LINK_ENTITY_TO_GAME(item_powerarmor);

LINK_ENTITY_TO_GAME(item_powerup);

LINK_ENTITY_TO_GAME(item_pt);

LINK_ENTITY_TO_GAME(item_quad);

LINK_ENTITY_TO_GAME(item_radio);

LINK_ENTITY_TO_GAME(item_random);

LINK_ENTITY_TO_GAME(item_regeneration);

LINK_ENTITY_TO_GAME(item_resource);

LINK_ENTITY_TO_GAME(item_rockets);

LINK_ENTITY_TO_GAME(item_scope);

LINK_ENTITY_TO_GAME(item_security);

LINK_ENTITY_TO_GAME(item_sensubeanbag);

LINK_ENTITY_TO_GAME(item_shells);

LINK_ENTITY_TO_GAME(item_shovel);

LINK_ENTITY_TO_GAME(item_silencer);

LINK_ENTITY_TO_GAME(item_silvercase);

LINK_ENTITY_TO_GAME(item_smboiler);

LINK_ENTITY_TO_GAME(item_sodacan);

LINK_ENTITY_TO_GAME(item_speedburst);

LINK_ENTITY_TO_GAME(item_spikes);

LINK_ENTITY_TO_GAME(item_stealth);

LINK_ENTITY_TO_GAME(item_steerribs);

LINK_ENTITY_TO_GAME(item_steerskull);

LINK_ENTITY_TO_GAME(item_stg24);

LINK_ENTITY_TO_GAME(item_suit);

LINK_ENTITY_TO_GAME(item_swarm_flag);

LINK_ENTITY_TO_GAME(item_tag);

LINK_ENTITY_TO_GAME(item_telegram);

LINK_ENTITY_TO_GAME(item_telegraphkey);

LINK_ENTITY_TO_GAME(item_teleport);

LINK_ENTITY_TO_GAME(item_tfgoal);

LINK_ENTITY_TO_GAME(item_thighpack);

LINK_ENTITY_TO_GAME(item_tombe_bleu);

LINK_ENTITY_TO_GAME(item_tombe_rouge);

LINK_ENTITY_TO_GAME(item_treasurechest);

LINK_ENTITY_TO_GAME(item_upgradevest);

LINK_ENTITY_TO_GAME(item_vengeance);

LINK_ENTITY_TO_GAME(item_vest);

LINK_ENTITY_TO_GAME(item_wagonwheel);

LINK_ENTITY_TO_GAME(item_weapon);

LINK_ENTITY_TO_GAME(item_wheelside);

LINK_ENTITY_TO_GAME(item_wwgoal);

LINK_ENTITY_TO_GAME(jail_execute);

LINK_ENTITY_TO_GAME(jail_team_master);

LINK_ENTITY_TO_GAME(jailer);

LINK_ENTITY_TO_GAME(javelin);

LINK_ENTITY_TO_GAME(judge);

LINK_ENTITY_TO_GAME(jumppad_sign);

LINK_ENTITY_TO_GAME(kamehameha);

LINK_ENTITY_TO_GAME(kb_fireworklauncher);

LINK_ENTITY_TO_GAME(killerrat);

LINK_ENTITY_TO_GAME(knife);

LINK_ENTITY_TO_GAME(knife_bolt);

LINK_ENTITY_TO_GAME(knife_throw);

LINK_ENTITY_TO_GAME(kotm_crown);

LINK_ENTITY_TO_GAME(kotm_hill);

LINK_ENTITY_TO_GAME(kwSpikeProjectile);

LINK_ENTITY_TO_GAME(lance_proj);

LINK_ENTITY_TO_GAME(laser_aimer);

LINK_ENTITY_TO_GAME(laser_dot);

LINK_ENTITY_TO_GAME(laser_spot);

LINK_ENTITY_TO_GAME(laserbolt);

LINK_ENTITY_TO_GAME(law_rocket);

LINK_ENTITY_TO_GAME(lc_init_ent);

LINK_ENTITY_TO_GAME(lc_respawner);

LINK_ENTITY_TO_GAME(lc_spawner);

LINK_ENTITY_TO_GAME(lcs_spawner);

LINK_ENTITY_TO_GAME(lflamme_fire);

LINK_ENTITY_TO_GAME(lgtng_ball);

LINK_ENTITY_TO_GAME(lifesoul);

LINK_ENTITY_TO_GAME(light);

LINK_ENTITY_TO_GAME(light_environment);

LINK_ENTITY_TO_GAME(light_glow);

LINK_ENTITY_TO_GAME(light_spot);

LINK_ENTITY_TO_GAME(lightfader);

LINK_ENTITY_TO_GAME(load_wpt);

LINK_ENTITY_TO_GAME(locus_alias);

LINK_ENTITY_TO_GAME(locus_beam);

LINK_ENTITY_TO_GAME(locus_variable);

LINK_ENTITY_TO_GAME(m203grenade);

LINK_ENTITY_TO_GAME(m61_grenade);

LINK_ENTITY_TO_GAME(m72_rocket);

LINK_ENTITY_TO_GAME(magicattack);

LINK_ENTITY_TO_GAME(maintainer_ent);

LINK_ENTITY_TO_GAME(mapClassName);

LINK_ENTITY_TO_GAME(mapobject_asiancar);

LINK_ENTITY_TO_GAME(mapobject_bluecar);

LINK_ENTITY_TO_GAME(mapobject_copcar);

LINK_ENTITY_TO_GAME(mapobject_jaguar);

LINK_ENTITY_TO_GAME(mapobject_npc_bum);

LINK_ENTITY_TO_GAME(mapobject_npc_bum1);

LINK_ENTITY_TO_GAME(mapobject_npc_bum2);

LINK_ENTITY_TO_GAME(mapobject_npc_bum3);

LINK_ENTITY_TO_GAME(mapobject_npc_crow);

LINK_ENTITY_TO_GAME(mapobject_npc_slavebot);

LINK_ENTITY_TO_GAME(mapobject_redcar);

LINK_ENTITY_TO_GAME(mapobject_zeppelin);

LINK_ENTITY_TO_GAME(marker);

LINK_ENTITY_TO_GAME(marker_flag);

LINK_ENTITY_TO_GAME(master_key);

LINK_ENTITY_TO_GAME(master_living);

LINK_ENTITY_TO_GAME(master_object_home);

LINK_ENTITY_TO_GAME(master_period);

LINK_ENTITY_TO_GAME(master_player);

LINK_ENTITY_TO_GAME(master_relay);

LINK_ENTITY_TO_GAME(master_round_active);

LINK_ENTITY_TO_GAME(master_round_start);

LINK_ENTITY_TO_GAME(master_secret);

LINK_ENTITY_TO_GAME(master_state);

LINK_ENTITY_TO_GAME(master_team);

LINK_ENTITY_TO_GAME(master_tech);

LINK_ENTITY_TO_GAME(mastertoggle);

LINK_ENTITY_TO_GAME(mazo);

LINK_ENTITY_TO_GAME(mazoold);

LINK_ENTITY_TO_GAME(medal);

LINK_ENTITY_TO_GAME(medevac);

LINK_ENTITY_TO_GAME(medivac);

LINK_ENTITY_TO_GAME(meteor_god);

LINK_ENTITY_TO_GAME(meteor_target);

LINK_ENTITY_TO_GAME(mi_capture_zone);

LINK_ENTITY_TO_GAME(mi_fail_objective);

LINK_ENTITY_TO_GAME(mi_func_fog);

LINK_ENTITY_TO_GAME(mi_grass);

LINK_ENTITY_TO_GAME(mi_model);

LINK_ENTITY_TO_GAME(mi_model_names);

LINK_ENTITY_TO_GAME(mi_model_static);

LINK_ENTITY_TO_GAME(mi_obj_icon);

LINK_ENTITY_TO_GAME(mi_obj_switch);

LINK_ENTITY_TO_GAME(mi_objective_attack);

LINK_ENTITY_TO_GAME(mi_objective_bomb);

LINK_ENTITY_TO_GAME(mi_objective_defend);

LINK_ENTITY_TO_GAME(mi_objective_destruct);

LINK_ENTITY_TO_GAME(mi_objective_extraction);

LINK_ENTITY_TO_GAME(mi_objective_hosrescue);

LINK_ENTITY_TO_GAME(mi_objective_hostage);

LINK_ENTITY_TO_GAME(mi_objective_observation);

LINK_ENTITY_TO_GAME(mi_objective_other);

LINK_ENTITY_TO_GAME(mi_objective_repair);

LINK_ENTITY_TO_GAME(mi_objective_team1);

LINK_ENTITY_TO_GAME(mi_objective_team2);

LINK_ENTITY_TO_GAME(mi_respawn);

LINK_ENTITY_TO_GAME(mi_set_objective);

LINK_ENTITY_TO_GAME(mi_set_respawn);

LINK_ENTITY_TO_GAME(mi_set_timer);

LINK_ENTITY_TO_GAME(mi_spawn_zone);

LINK_ENTITY_TO_GAME(mi_team_damage);

LINK_ENTITY_TO_GAME(mi_team_names);

LINK_ENTITY_TO_GAME(mi_team_push);

LINK_ENTITY_TO_GAME(mi_team_skins);

LINK_ENTITY_TO_GAME(mi_timer);

LINK_ENTITY_TO_GAME(mi_trigger_weather);

LINK_ENTITY_TO_GAME(mi_weather);

LINK_ENTITY_TO_GAME(mini_grunt);

LINK_ENTITY_TO_GAME(mini_rocket);

LINK_ENTITY_TO_GAME(mission_modifier);

LINK_ENTITY_TO_GAME(misterx);

LINK_ENTITY_TO_GAME(mode_relay);

LINK_ENTITY_TO_GAME(model_entity);

LINK_ENTITY_TO_GAME(molotov);

LINK_ENTITY_TO_GAME(momentary_door);

LINK_ENTITY_TO_GAME(momentary_jail_door);

LINK_ENTITY_TO_GAME(momentary_rot_button);

LINK_ENTITY_TO_GAME(monster_ShockTrooper_dead);

LINK_ENTITY_TO_GAME(monster_a10);

LINK_ENTITY_TO_GAME(monster_adrian);

LINK_ENTITY_TO_GAME(monster_adrian_dead);

LINK_ENTITY_TO_GAME(monster_alarm);

LINK_ENTITY_TO_GAME(monster_alien_babyvoltigore);

LINK_ENTITY_TO_GAME(monster_alien_controller);

LINK_ENTITY_TO_GAME(monster_alien_grunt);

LINK_ENTITY_TO_GAME(monster_alien_panther);

LINK_ENTITY_TO_GAME(monster_alien_slave);

LINK_ENTITY_TO_GAME(monster_alien_slave_dead);

LINK_ENTITY_TO_GAME(monster_alien_voltigore);

LINK_ENTITY_TO_GAME(monster_allied_barney);

LINK_ENTITY_TO_GAME(monster_allied_grunt);

LINK_ENTITY_TO_GAME(monster_alliedgrunt_dead);

LINK_ENTITY_TO_GAME(monster_annie);

LINK_ENTITY_TO_GAME(monster_annie_dead);

LINK_ENTITY_TO_GAME(monster_apache);

LINK_ENTITY_TO_GAME(monster_archer);

LINK_ENTITY_TO_GAME(monster_assassin_repel);

LINK_ENTITY_TO_GAME(monster_assassin_target);

LINK_ENTITY_TO_GAME(monster_axis_grunt);

LINK_ENTITY_TO_GAME(monster_axisgrunt_dead);

LINK_ENTITY_TO_GAME(monster_babycrab);

LINK_ENTITY_TO_GAME(monster_babygarg);

LINK_ENTITY_TO_GAME(monster_balllightning);

LINK_ENTITY_TO_GAME(monster_barnacle);

LINK_ENTITY_TO_GAME(monster_barney);

LINK_ENTITY_TO_GAME(monster_barney_dead);

LINK_ENTITY_TO_GAME(monster_barniel);

LINK_ENTITY_TO_GAME(monster_barniel_dead);

LINK_ENTITY_TO_GAME(monster_bbturret);

LINK_ENTITY_TO_GAME(monster_beak);

LINK_ENTITY_TO_GAME(monster_bear);

LINK_ENTITY_TO_GAME(monster_beartrap);

LINK_ENTITY_TO_GAME(monster_bigminer);

LINK_ENTITY_TO_GAME(monster_bigminer_dead);

LINK_ENTITY_TO_GAME(monster_bigmomma);

LINK_ENTITY_TO_GAME(monster_blkop_apache);

LINK_ENTITY_TO_GAME(monster_blkop_ospray);

LINK_ENTITY_TO_GAME(monster_blkop_osprey);

LINK_ENTITY_TO_GAME(monster_bloater);

LINK_ENTITY_TO_GAME(monster_bodypart);

LINK_ENTITY_TO_GAME(monster_bomb);

LINK_ENTITY_TO_GAME(monster_buffalo);

LINK_ENTITY_TO_GAME(monster_bullchicken);

LINK_ENTITY_TO_GAME(monster_bullsquid);

LINK_ENTITY_TO_GAME(monster_burnmine);

LINK_ENTITY_TO_GAME(monster_c4);

LINK_ENTITY_TO_GAME(monster_camera);

LINK_ENTITY_TO_GAME(monster_cat);

LINK_ENTITY_TO_GAME(monster_chicken);

LINK_ENTITY_TO_GAME(monster_chumtoad);

LINK_ENTITY_TO_GAME(monster_cine2_crispen);

LINK_ENTITY_TO_GAME(monster_cine2_dave);

LINK_ENTITY_TO_GAME(monster_cine2_hvyweapons);

LINK_ENTITY_TO_GAME(monster_cine2_masala);

LINK_ENTITY_TO_GAME(monster_cine2_nagatow);

LINK_ENTITY_TO_GAME(monster_cine2_scientist);

LINK_ENTITY_TO_GAME(monster_cine2_slave);

LINK_ENTITY_TO_GAME(monster_cine2_tiedcolonel);

LINK_ENTITY_TO_GAME(monster_cine2_townmex);

LINK_ENTITY_TO_GAME(monster_cine2_wtowna);

LINK_ENTITY_TO_GAME(monster_cine2_wtownb);

LINK_ENTITY_TO_GAME(monster_cine3_annie);

LINK_ENTITY_TO_GAME(monster_cine3_barney);

LINK_ENTITY_TO_GAME(monster_cine3_crispen);

LINK_ENTITY_TO_GAME(monster_cine3_dave);

LINK_ENTITY_TO_GAME(monster_cine3_hoss);

LINK_ENTITY_TO_GAME(monster_cine3_masala);

LINK_ENTITY_TO_GAME(monster_cine3_nagatow);

LINK_ENTITY_TO_GAME(monster_cine3_scientist);

LINK_ENTITY_TO_GAME(monster_cine3_tiedcolonel);

LINK_ENTITY_TO_GAME(monster_cine3_townmex);

LINK_ENTITY_TO_GAME(monster_cine3_wtowna);

LINK_ENTITY_TO_GAME(monster_cine3_wtownb);

LINK_ENTITY_TO_GAME(monster_cine_annie);

LINK_ENTITY_TO_GAME(monster_cine_barney);

LINK_ENTITY_TO_GAME(monster_cine_crispen);

LINK_ENTITY_TO_GAME(monster_cine_dave);

LINK_ENTITY_TO_GAME(monster_cine_hoss);

LINK_ENTITY_TO_GAME(monster_cine_masala);

LINK_ENTITY_TO_GAME(monster_cine_nagatow);

LINK_ENTITY_TO_GAME(monster_cine_panther);

LINK_ENTITY_TO_GAME(monster_cine_scientist);

LINK_ENTITY_TO_GAME(monster_cine_tiedcolonel);

LINK_ENTITY_TO_GAME(monster_cine_townmex);

LINK_ENTITY_TO_GAME(monster_cine_wtowna);

LINK_ENTITY_TO_GAME(monster_cine_wtownb);

LINK_ENTITY_TO_GAME(monster_civilian);

LINK_ENTITY_TO_GAME(monster_civilian_dead);

LINK_ENTITY_TO_GAME(monster_cleansuit_scientist);

LINK_ENTITY_TO_GAME(monster_cleansuit_scientist_dead);

LINK_ENTITY_TO_GAME(monster_cockroach);

LINK_ENTITY_TO_GAME(monster_colonel);

LINK_ENTITY_TO_GAME(monster_colonel_dead);

LINK_ENTITY_TO_GAME(monster_cow);

LINK_ENTITY_TO_GAME(monster_cowboy);

LINK_ENTITY_TO_GAME(monster_cowboy_dead);

LINK_ENTITY_TO_GAME(monster_cricket);

LINK_ENTITY_TO_GAME(monster_crispen);

LINK_ENTITY_TO_GAME(monster_crispen_dead);

LINK_ENTITY_TO_GAME(monster_critter);

LINK_ENTITY_TO_GAME(monster_darttrap);

LINK_ENTITY_TO_GAME(monster_dave);

LINK_ENTITY_TO_GAME(monster_dave_dead);

LINK_ENTITY_TO_GAME(monster_demo);

LINK_ENTITY_TO_GAME(monster_dog);

LINK_ENTITY_TO_GAME(monster_dragon);

LINK_ENTITY_TO_GAME(monster_dragonfly);

LINK_ENTITY_TO_GAME(monster_drillsergeant);

LINK_ENTITY_TO_GAME(monster_eagle);

LINK_ENTITY_TO_GAME(monster_eagle_flock);

LINK_ENTITY_TO_GAME(monster_endboss);

LINK_ENTITY_TO_GAME(monster_exp_alien_slave);

LINK_ENTITY_TO_GAME(monster_fakemedkit);

LINK_ENTITY_TO_GAME(monster_fgrunt_repel);

LINK_ENTITY_TO_GAME(monster_flag1);

LINK_ENTITY_TO_GAME(monster_flag2);

LINK_ENTITY_TO_GAME(monster_flag3);

LINK_ENTITY_TO_GAME(monster_flag4);

LINK_ENTITY_TO_GAME(monster_flashlight);

LINK_ENTITY_TO_GAME(monster_flyer);

LINK_ENTITY_TO_GAME(monster_flyer_flock);

LINK_ENTITY_TO_GAME(monster_furniture);

LINK_ENTITY_TO_GAME(monster_garbage);

LINK_ENTITY_TO_GAME(monster_gargantua);

LINK_ENTITY_TO_GAME(monster_gator);

LINK_ENTITY_TO_GAME(monster_generic);

LINK_ENTITY_TO_GAME(monster_generic_dead);

LINK_ENTITY_TO_GAME(monster_geneworm);

LINK_ENTITY_TO_GAME(monster_giantplant);

LINK_ENTITY_TO_GAME(monster_gman);

LINK_ENTITY_TO_GAME(monster_goat);

LINK_ENTITY_TO_GAME(monster_goblin);

LINK_ENTITY_TO_GAME(monster_gonome);

LINK_ENTITY_TO_GAME(monster_gonome_dead);

LINK_ENTITY_TO_GAME(monster_gordon);

LINK_ENTITY_TO_GAME(monster_gordon_dead);

LINK_ENTITY_TO_GAME(monster_gps);

LINK_ENTITY_TO_GAME(monster_grunt_ally_medic_dead);

LINK_ENTITY_TO_GAME(monster_grunt_ally_repel);

LINK_ENTITY_TO_GAME(monster_grunt_ally_torch_dead);

LINK_ENTITY_TO_GAME(monster_grunt_repel);

LINK_ENTITY_TO_GAME(monster_gunner_friendly);

LINK_ENTITY_TO_GAME(monster_hatchetfish);

LINK_ENTITY_TO_GAME(monster_headcrab);

LINK_ENTITY_TO_GAME(monster_helicopter);

LINK_ENTITY_TO_GAME(monster_hevbarn);

LINK_ENTITY_TO_GAME(monster_hevbarn_dead);

LINK_ENTITY_TO_GAME(monster_hevsuit_dead);

LINK_ENTITY_TO_GAME(monster_hfgrunt_dead);

LINK_ENTITY_TO_GAME(monster_hgrunt);

LINK_ENTITY_TO_GAME(monster_hgrunt_dead);

LINK_ENTITY_TO_GAME(monster_hgrunt_shotgun);

LINK_ENTITY_TO_GAME(monster_hiveback);

LINK_ENTITY_TO_GAME(monster_horse);

LINK_ENTITY_TO_GAME(monster_hoss);

LINK_ENTITY_TO_GAME(monster_hoss_dead);

LINK_ENTITY_TO_GAME(monster_hostage);

LINK_ENTITY_TO_GAME(monster_hostage_dead);

LINK_ENTITY_TO_GAME(monster_houndeye);

LINK_ENTITY_TO_GAME(monster_houndeye_dead);

LINK_ENTITY_TO_GAME(monster_human_assassin);

LINK_ENTITY_TO_GAME(monster_human_bandit);

LINK_ENTITY_TO_GAME(monster_human_chopper);

LINK_ENTITY_TO_GAME(monster_human_demoman);

LINK_ENTITY_TO_GAME(monster_human_friendly_grunt);

LINK_ENTITY_TO_GAME(monster_human_grunt);

LINK_ENTITY_TO_GAME(monster_human_grunt_ally);

LINK_ENTITY_TO_GAME(monster_human_grunt_ally_dead);

LINK_ENTITY_TO_GAME(monster_human_gunman);

LINK_ENTITY_TO_GAME(monster_human_medic_ally);

LINK_ENTITY_TO_GAME(monster_human_medic_ally_dead);

LINK_ENTITY_TO_GAME(monster_human_scientist);

LINK_ENTITY_TO_GAME(monster_human_spforce);

LINK_ENTITY_TO_GAME(monster_human_terror);

LINK_ENTITY_TO_GAME(monster_human_torch_ally);

LINK_ENTITY_TO_GAME(monster_human_torch_ally_dead);

LINK_ENTITY_TO_GAME(monster_human_unarmed);

LINK_ENTITY_TO_GAME(monster_hwgrunt);

LINK_ENTITY_TO_GAME(monster_hwgrunt_repel);

LINK_ENTITY_TO_GAME(monster_ichthyosaur);

LINK_ENTITY_TO_GAME(monster_implanted);

LINK_ENTITY_TO_GAME(monster_kaiewi);

LINK_ENTITY_TO_GAME(monster_kaiewi_dead);

LINK_ENTITY_TO_GAME(monster_kate);

LINK_ENTITY_TO_GAME(monster_kate_dead);

LINK_ENTITY_TO_GAME(monster_kid);

LINK_ENTITY_TO_GAME(monster_killerbabycrab);

LINK_ENTITY_TO_GAME(monster_largescorpion);

LINK_ENTITY_TO_GAME(monster_larve);

LINK_ENTITY_TO_GAME(monster_leech);

LINK_ENTITY_TO_GAME(monster_llama);

LINK_ENTITY_TO_GAME(monster_lrocket);

LINK_ENTITY_TO_GAME(monster_m2);

LINK_ENTITY_TO_GAME(monster_male_assassin);

LINK_ENTITY_TO_GAME(monster_manta);

LINK_ENTITY_TO_GAME(monster_masala);

LINK_ENTITY_TO_GAME(monster_masala_dead);

LINK_ENTITY_TO_GAME(monster_massassin_dead);

LINK_ENTITY_TO_GAME(monster_medic_ally_repel);

LINK_ENTITY_TO_GAME(monster_mexbandit);

LINK_ENTITY_TO_GAME(monster_mexbandit_dead);

LINK_ENTITY_TO_GAME(monster_microraptor);

LINK_ENTITY_TO_GAME(monster_mine);

LINK_ENTITY_TO_GAME(monster_miniturret);

LINK_ENTITY_TO_GAME(monster_missle);

LINK_ENTITY_TO_GAME(monster_monkey);

LINK_ENTITY_TO_GAME(monster_mortar);

LINK_ENTITY_TO_GAME(monster_morter);

LINK_ENTITY_TO_GAME(monster_mouton);

LINK_ENTITY_TO_GAME(monster_myself);

LINK_ENTITY_TO_GAME(monster_myself_dead);

LINK_ENTITY_TO_GAME(monster_nagatow);

LINK_ENTITY_TO_GAME(monster_nagatow_dead);

LINK_ENTITY_TO_GAME(monster_nihilanth);

LINK_ENTITY_TO_GAME(monster_op4loader);

LINK_ENTITY_TO_GAME(monster_osprey);

LINK_ENTITY_TO_GAME(monster_otis);

LINK_ENTITY_TO_GAME(monster_otis_dead);

LINK_ENTITY_TO_GAME(monster_ourano);

LINK_ENTITY_TO_GAME(monster_parachute);

LINK_ENTITY_TO_GAME(monster_parrot);

LINK_ENTITY_TO_GAME(monster_penguin);

LINK_ENTITY_TO_GAME(monster_penta);

LINK_ENTITY_TO_GAME(monster_pig);

LINK_ENTITY_TO_GAME(monster_pilot);

LINK_ENTITY_TO_GAME(monster_pipebomb);

LINK_ENTITY_TO_GAME(monster_pitdrone);

LINK_ENTITY_TO_GAME(monster_pitworm);

LINK_ENTITY_TO_GAME(monster_pitworm_up);

LINK_ENTITY_TO_GAME(monster_player);

LINK_ENTITY_TO_GAME(monster_poison);

LINK_ENTITY_TO_GAME(monster_powderkeg);

LINK_ENTITY_TO_GAME(monster_prisoner);

LINK_ENTITY_TO_GAME(monster_prop_apache);

LINK_ENTITY_TO_GAME(monster_proximity);

LINK_ENTITY_TO_GAME(monster_puma);

LINK_ENTITY_TO_GAME(monster_ramone);

LINK_ENTITY_TO_GAME(monster_ramone_repel);

LINK_ENTITY_TO_GAME(monster_raptor);

LINK_ENTITY_TO_GAME(monster_rat);

LINK_ENTITY_TO_GAME(monster_recruit);

LINK_ENTITY_TO_GAME(monster_replic);

LINK_ENTITY_TO_GAME(monster_replicateur);

LINK_ENTITY_TO_GAME(monster_replicator);

LINK_ENTITY_TO_GAME(monster_robogrunt);

LINK_ENTITY_TO_GAME(monster_robogrunt_dead);

LINK_ENTITY_TO_GAME(monster_robogrunt_repel);

LINK_ENTITY_TO_GAME(monster_robogruntrepel);

LINK_ENTITY_TO_GAME(monster_rustbattery);

LINK_ENTITY_TO_GAME(monster_rustbit);

LINK_ENTITY_TO_GAME(monster_rustbit_friendly);

LINK_ENTITY_TO_GAME(monster_rustflier);

LINK_ENTITY_TO_GAME(monster_rustgunr);

LINK_ENTITY_TO_GAME(monster_sarge_allies);

LINK_ENTITY_TO_GAME(monster_satchel);

LINK_ENTITY_TO_GAME(monster_scientist);

LINK_ENTITY_TO_GAME(monster_scientist_dead);

LINK_ENTITY_TO_GAME(monster_scorpion);

LINK_ENTITY_TO_GAME(monster_sentry);

LINK_ENTITY_TO_GAME(monster_sentry_mini);

LINK_ENTITY_TO_GAME(monster_sentryx);

LINK_ENTITY_TO_GAME(monster_sheep);

LINK_ENTITY_TO_GAME(monster_shockroach);

LINK_ENTITY_TO_GAME(monster_shocktrooper);

LINK_ENTITY_TO_GAME(monster_shocktrooper_repel);

LINK_ENTITY_TO_GAME(monster_shotgun);

LINK_ENTITY_TO_GAME(monster_sitting_civilian);

LINK_ENTITY_TO_GAME(monster_sitting_cleansuit_scientist);

LINK_ENTITY_TO_GAME(monster_sitting_colonel);

LINK_ENTITY_TO_GAME(monster_sitting_crispen);

LINK_ENTITY_TO_GAME(monster_sitting_dave);

LINK_ENTITY_TO_GAME(monster_sitting_masala);

LINK_ENTITY_TO_GAME(monster_sitting_nagatow);

LINK_ENTITY_TO_GAME(monster_sitting_scientist);

LINK_ENTITY_TO_GAME(monster_sitting_townmex);

LINK_ENTITY_TO_GAME(monster_sitting_twnwesta);

LINK_ENTITY_TO_GAME(monster_sitting_twnwestb);

LINK_ENTITY_TO_GAME(monster_sitting_villager);

LINK_ENTITY_TO_GAME(monster_skeleton);

LINK_ENTITY_TO_GAME(monster_skeleton_dead);

LINK_ENTITY_TO_GAME(monster_skelly);

LINK_ENTITY_TO_GAME(monster_skellydance);

LINK_ENTITY_TO_GAME(monster_skull);

LINK_ENTITY_TO_GAME(monster_skunk);

LINK_ENTITY_TO_GAME(monster_smallminer);

LINK_ENTITY_TO_GAME(monster_smallminer_dead);

LINK_ENTITY_TO_GAME(monster_snake);

LINK_ENTITY_TO_GAME(monster_snark);

LINK_ENTITY_TO_GAME(monster_spforce_dead);

LINK_ENTITY_TO_GAME(monster_spforce_repel);

LINK_ENTITY_TO_GAME(monster_sphere);

LINK_ENTITY_TO_GAME(monster_spider);

LINK_ENTITY_TO_GAME(monster_spiritdragon);

LINK_ENTITY_TO_GAME(monster_spiritwiz);

LINK_ENTITY_TO_GAME(monster_srocket);

LINK_ENTITY_TO_GAME(monster_super_snark);

LINK_ENTITY_TO_GAME(monster_tac);

LINK_ENTITY_TO_GAME(monster_tank);

LINK_ENTITY_TO_GAME(monster_target);

LINK_ENTITY_TO_GAME(monster_targetrocket);

LINK_ENTITY_TO_GAME(monster_tentacle);

LINK_ENTITY_TO_GAME(monster_tentaclemaw);

LINK_ENTITY_TO_GAME(monster_terror_dead);

LINK_ENTITY_TO_GAME(monster_terror_repel);

LINK_ENTITY_TO_GAME(monster_test);

LINK_ENTITY_TO_GAME(monster_thornbush);

LINK_ENTITY_TO_GAME(monster_tied_colonel);

LINK_ENTITY_TO_GAME(monster_torch_ally_repel);

LINK_ENTITY_TO_GAME(monster_tornado);

LINK_ENTITY_TO_GAME(monster_townmex);

LINK_ENTITY_TO_GAME(monster_townmex_dead);

LINK_ENTITY_TO_GAME(monster_trainingbot);

LINK_ENTITY_TO_GAME(monster_tripmine);

LINK_ENTITY_TO_GAME(monster_troop);

LINK_ENTITY_TO_GAME(monster_tube);

LINK_ENTITY_TO_GAME(monster_tube_embryo);

LINK_ENTITY_TO_GAME(monster_turret);

LINK_ENTITY_TO_GAME(monster_turretbase);

LINK_ENTITY_TO_GAME(monster_twnwesta);

LINK_ENTITY_TO_GAME(monster_twnwesta_dead);

LINK_ENTITY_TO_GAME(monster_twnwestb);

LINK_ENTITY_TO_GAME(monster_twnwestb_dead);

LINK_ENTITY_TO_GAME(monster_valve_turret);

LINK_ENTITY_TO_GAME(monster_villager);

LINK_ENTITY_TO_GAME(monster_villager_dead);

LINK_ENTITY_TO_GAME(monster_vortigaunt);

LINK_ENTITY_TO_GAME(monster_wizardclone);

LINK_ENTITY_TO_GAME(monster_wombat);

LINK_ENTITY_TO_GAME(monster_worker);

LINK_ENTITY_TO_GAME(monster_worker_dead);

LINK_ENTITY_TO_GAME(monster_wwmine);

LINK_ENTITY_TO_GAME(monster_xenome);

LINK_ENTITY_TO_GAME(monster_xenome_embryo);

LINK_ENTITY_TO_GAME(monster_zbarney);

LINK_ENTITY_TO_GAME(monster_zombie);

LINK_ENTITY_TO_GAME(monster_zombie2);

LINK_ENTITY_TO_GAME(monster_zombie_barney);

LINK_ENTITY_TO_GAME(monster_zombie_soldier);

LINK_ENTITY_TO_GAME(monster_zombie_soldier_dead);

LINK_ENTITY_TO_GAME(monstermaker);

LINK_ENTITY_TO_GAME(monsterpoint);

LINK_ENTITY_TO_GAME(monsterpoint_backup);

LINK_ENTITY_TO_GAME(mortar_shell);

LINK_ENTITY_TO_GAME(mortarshell);

LINK_ENTITY_TO_GAME(motion_manager);

LINK_ENTITY_TO_GAME(motion_thread);

LINK_ENTITY_TO_GAME(movementchamber);

LINK_ENTITY_TO_GAME(moving_camera);

LINK_ENTITY_TO_GAME(multi_alias);

LINK_ENTITY_TO_GAME(multi_gate_1);

LINK_ENTITY_TO_GAME(multi_gate_10);

LINK_ENTITY_TO_GAME(multi_gate_11);

LINK_ENTITY_TO_GAME(multi_gate_12);

LINK_ENTITY_TO_GAME(multi_gate_13);

LINK_ENTITY_TO_GAME(multi_gate_14);

LINK_ENTITY_TO_GAME(multi_gate_15);

LINK_ENTITY_TO_GAME(multi_gate_16);

LINK_ENTITY_TO_GAME(multi_gate_17);

LINK_ENTITY_TO_GAME(multi_gate_18);

LINK_ENTITY_TO_GAME(multi_gate_19);

LINK_ENTITY_TO_GAME(multi_gate_2);

LINK_ENTITY_TO_GAME(multi_gate_20);

LINK_ENTITY_TO_GAME(multi_gate_21);

LINK_ENTITY_TO_GAME(multi_gate_22);

LINK_ENTITY_TO_GAME(multi_gate_23);

LINK_ENTITY_TO_GAME(multi_gate_24);

LINK_ENTITY_TO_GAME(multi_gate_25);

LINK_ENTITY_TO_GAME(multi_gate_26);

LINK_ENTITY_TO_GAME(multi_gate_27);

LINK_ENTITY_TO_GAME(multi_gate_28);

LINK_ENTITY_TO_GAME(multi_gate_29);

LINK_ENTITY_TO_GAME(multi_gate_3);

LINK_ENTITY_TO_GAME(multi_gate_30);

LINK_ENTITY_TO_GAME(multi_gate_31);

LINK_ENTITY_TO_GAME(multi_gate_32);

LINK_ENTITY_TO_GAME(multi_gate_33);

LINK_ENTITY_TO_GAME(multi_gate_34);

LINK_ENTITY_TO_GAME(multi_gate_35);

LINK_ENTITY_TO_GAME(multi_gate_36);

LINK_ENTITY_TO_GAME(multi_gate_37);

LINK_ENTITY_TO_GAME(multi_gate_38);

LINK_ENTITY_TO_GAME(multi_gate_39);

LINK_ENTITY_TO_GAME(multi_gate_4);

LINK_ENTITY_TO_GAME(multi_gate_40);

LINK_ENTITY_TO_GAME(multi_gate_41);

LINK_ENTITY_TO_GAME(multi_gate_42);

LINK_ENTITY_TO_GAME(multi_gate_43);

LINK_ENTITY_TO_GAME(multi_gate_44);

LINK_ENTITY_TO_GAME(multi_gate_45);

LINK_ENTITY_TO_GAME(multi_gate_46);

LINK_ENTITY_TO_GAME(multi_gate_47);

LINK_ENTITY_TO_GAME(multi_gate_48);

LINK_ENTITY_TO_GAME(multi_gate_49);

LINK_ENTITY_TO_GAME(multi_gate_5);

LINK_ENTITY_TO_GAME(multi_gate_6);

LINK_ENTITY_TO_GAME(multi_gate_7);

LINK_ENTITY_TO_GAME(multi_gate_8);

LINK_ENTITY_TO_GAME(multi_gate_9);

LINK_ENTITY_TO_GAME(multi_manager);

LINK_ENTITY_TO_GAME(multi_watcher);

LINK_ENTITY_TO_GAME(multisource);

LINK_ENTITY_TO_GAME(my_monster);

LINK_ENTITY_TO_GAME(nail);

LINK_ENTITY_TO_GAME(neutrinobeam);

LINK_ENTITY_TO_GAME(nggrenade);

LINK_ENTITY_TO_GAME(nihilanth_energy_ball);

LINK_ENTITY_TO_GAME(node_viewer);

LINK_ENTITY_TO_GAME(node_viewer_fly);

LINK_ENTITY_TO_GAME(node_viewer_human);

LINK_ENTITY_TO_GAME(node_viewer_large);

LINK_ENTITY_TO_GAME(npc);

LINK_ENTITY_TO_GAME(npc_skrunk);

LINK_ENTITY_TO_GAME(nuclear_missile);

LINK_ENTITY_TO_GAME(nuke);

LINK_ENTITY_TO_GAME(nuke_rocket);

LINK_ENTITY_TO_GAME(object);

LINK_ENTITY_TO_GAME(object_beacon);

LINK_ENTITY_TO_GAME(object_follow);

LINK_ENTITY_TO_GAME(object_maker);

LINK_ENTITY_TO_GAME(object_superchip);

LINK_ENTITY_TO_GAME(offensechamber);

LINK_ENTITY_TO_GAME(ol_bell);

LINK_ENTITY_TO_GAME(ol_bluecapture);

LINK_ENTITY_TO_GAME(ol_blueflag);

LINK_ENTITY_TO_GAME(ol_bottle);

LINK_ENTITY_TO_GAME(ol_ctf_blueteam);

LINK_ENTITY_TO_GAME(ol_ctf_redteam);

LINK_ENTITY_TO_GAME(ol_furniture);

LINK_ENTITY_TO_GAME(ol_lantern);

LINK_ENTITY_TO_GAME(ol_redcapture);

LINK_ENTITY_TO_GAME(ol_redflag);

LINK_ENTITY_TO_GAME(ol_spitoon);

LINK_ENTITY_TO_GAME(old_rocket);

LINK_ENTITY_TO_GAME(oldbot);

LINK_ENTITY_TO_GAME(op4mortar);

LINK_ENTITY_TO_GAME(oz_logo);

LINK_ENTITY_TO_GAME(oz_rune);

LINK_ENTITY_TO_GAME(paintball);

LINK_ENTITY_TO_GAME(palm_tree);

LINK_ENTITY_TO_GAME(para_roundtimer);

LINK_ENTITY_TO_GAME(particle);

LINK_ENTITY_TO_GAME(particle_shooter);

LINK_ENTITY_TO_GAME(particle_weatherfx);

LINK_ENTITY_TO_GAME(path_corner);

LINK_ENTITY_TO_GAME(path_monster);

LINK_ENTITY_TO_GAME(path_track);

LINK_ENTITY_TO_GAME(pballblue);

LINK_ENTITY_TO_GAME(pballred);

LINK_ENTITY_TO_GAME(pe_escapezone);

LINK_ENTITY_TO_GAME(pe_light);

LINK_ENTITY_TO_GAME(pe_light_ref);

LINK_ENTITY_TO_GAME(pe_object_case);

LINK_ENTITY_TO_GAME(pe_object_htool);

LINK_ENTITY_TO_GAME(pe_objectclip);

LINK_ENTITY_TO_GAME(pe_radar_mark);

LINK_ENTITY_TO_GAME(pe_rain);

LINK_ENTITY_TO_GAME(pe_spawn_corps);

LINK_ENTITY_TO_GAME(pe_spawn_syndicate);

LINK_ENTITY_TO_GAME(pe_terminal);

LINK_ENTITY_TO_GAME(phase_pulse);

LINK_ENTITY_TO_GAME(phasegate);

LINK_ENTITY_TO_GAME(pickup_drive_agrunt);

LINK_ENTITY_TO_GAME(pickup_drive_apache);

LINK_ENTITY_TO_GAME(pickup_drive_assassin);

LINK_ENTITY_TO_GAME(pickup_drive_bm);

LINK_ENTITY_TO_GAME(pickup_drive_bullsquid);

LINK_ENTITY_TO_GAME(pickup_drive_controller);

LINK_ENTITY_TO_GAME(pickup_drive_garg);

LINK_ENTITY_TO_GAME(pickup_drive_houndeye);

LINK_ENTITY_TO_GAME(pickup_drive_icky);

LINK_ENTITY_TO_GAME(pickup_drive_panth);

LINK_ENTITY_TO_GAME(pickup_drive_slave);

LINK_ENTITY_TO_GAME(pickup_drive_turret);

LINK_ENTITY_TO_GAME(pineapple);

LINK_ENTITY_TO_GAME(pipe_bomb);

LINK_ENTITY_TO_GAME(pipebomb);

LINK_ENTITY_TO_GAME(pitdronespike);

LINK_ENTITY_TO_GAME(pitworm_gib);

LINK_ENTITY_TO_GAME(pitworm_gibshooter);

LINK_ENTITY_TO_GAME(plasma);

LINK_ENTITY_TO_GAME(plasma2);

LINK_ENTITY_TO_GAME(plasma_ball);

LINK_ENTITY_TO_GAME(player);

LINK_ENTITY_TO_GAME(player_beartrapstrip);

LINK_ENTITY_TO_GAME(player_corpse);

LINK_ENTITY_TO_GAME(player_flame);

LINK_ENTITY_TO_GAME(player_freeze);

LINK_ENTITY_TO_GAME(player_giveitems);

LINK_ENTITY_TO_GAME(player_glasses);

LINK_ENTITY_TO_GAME(player_helmet);

LINK_ENTITY_TO_GAME(player_loadsaved);

LINK_ENTITY_TO_GAME(player_respawn_zone);

LINK_ENTITY_TO_GAME(player_roach);

LINK_ENTITY_TO_GAME(player_speaker);

LINK_ENTITY_TO_GAME(player_togglehud);

LINK_ENTITY_TO_GAME(player_weaponship);

LINK_ENTITY_TO_GAME(player_weaponstrip);

LINK_ENTITY_TO_GAME(playerhornet);

LINK_ENTITY_TO_GAME(point_win);

LINK_ENTITY_TO_GAME(poison_injected);

LINK_ENTITY_TO_GAME(potatoc);

LINK_ENTITY_TO_GAME(power_bag);

LINK_ENTITY_TO_GAME(power_ballbag);

LINK_ENTITY_TO_GAME(power_coco);

LINK_ENTITY_TO_GAME(power_gloves);

LINK_ENTITY_TO_GAME(power_pogo);

LINK_ENTITY_TO_GAME(power_shield);

LINK_ENTITY_TO_GAME(power_shoes);

LINK_ENTITY_TO_GAME(powerstruggle);

LINK_ENTITY_TO_GAME(powerup_candy);

LINK_ENTITY_TO_GAME(proj_bird);

LINK_ENTITY_TO_GAME(proj_blaster);

LINK_ENTITY_TO_GAME(proj_bursatchel);

LINK_ENTITY_TO_GAME(proj_cocklebur);

LINK_ENTITY_TO_GAME(proj_comet);

LINK_ENTITY_TO_GAME(proj_doublemagicmissle);

LINK_ENTITY_TO_GAME(proj_dragonfire);

LINK_ENTITY_TO_GAME(proj_earthquakesatchel);

LINK_ENTITY_TO_GAME(proj_fireball);

LINK_ENTITY_TO_GAME(proj_firesatchel);

LINK_ENTITY_TO_GAME(proj_firespiral);

LINK_ENTITY_TO_GAME(proj_fissure);

LINK_ENTITY_TO_GAME(proj_flame);

LINK_ENTITY_TO_GAME(proj_flyingskull);

LINK_ENTITY_TO_GAME(proj_healhurtsatchel);

LINK_ENTITY_TO_GAME(proj_icepoke);

LINK_ENTITY_TO_GAME(proj_iceshard);

LINK_ENTITY_TO_GAME(proj_lightningcloud);

LINK_ENTITY_TO_GAME(proj_lightningsatchel);

LINK_ENTITY_TO_GAME(proj_magicmissle);

LINK_ENTITY_TO_GAME(proj_meteor);

LINK_ENTITY_TO_GAME(proj_mindmissle);

LINK_ENTITY_TO_GAME(proj_missile);

LINK_ENTITY_TO_GAME(proj_poisonsatchel);

LINK_ENTITY_TO_GAME(proj_rollingstone);

LINK_ENTITY_TO_GAME(proj_sporepod);

LINK_ENTITY_TO_GAME(proj_stone);

LINK_ENTITY_TO_GAME(proj_suctionsatchel);

LINK_ENTITY_TO_GAME(proj_tcrystal);

LINK_ENTITY_TO_GAME(proj_throwingbone);

LINK_ENTITY_TO_GAME(proj_toothsatchel);

LINK_ENTITY_TO_GAME(proj_whirlwind);

LINK_ENTITY_TO_GAME(proj_wyvern);

LINK_ENTITY_TO_GAME(promagcurse);

LINK_ENTITY_TO_GAME(pshield_det);

LINK_ENTITY_TO_GAME(pt_bomb_zone);

LINK_ENTITY_TO_GAME(pt_campaign_end);

LINK_ENTITY_TO_GAME(pt_change_attribute);

LINK_ENTITY_TO_GAME(pt_control_area);

LINK_ENTITY_TO_GAME(pt_defeat_victory);

LINK_ENTITY_TO_GAME(pt_entity_reset);

LINK_ENTITY_TO_GAME(pt_hostage);

LINK_ENTITY_TO_GAME(pt_mission_target);

LINK_ENTITY_TO_GAME(pt_model);

LINK_ENTITY_TO_GAME(pt_parachute_area);

LINK_ENTITY_TO_GAME(pt_rescue_zone);

LINK_ENTITY_TO_GAME(pt_round_info);

LINK_ENTITY_TO_GAME(pt_score_award);

LINK_ENTITY_TO_GAME(pt_startpoint);

LINK_ENTITY_TO_GAME(pt_timer);

LINK_ENTITY_TO_GAME(pt_toggle_respawnmode);

LINK_ENTITY_TO_GAME(pt_trigger_bomb);

LINK_ENTITY_TO_GAME(pt_trigger_loader);

LINK_ENTITY_TO_GAME(pt_trigger_zone);

LINK_ENTITY_TO_GAME(pulse);

LINK_ENTITY_TO_GAME(pulsefrag);

LINK_ENTITY_TO_GAME(punch_tripmine);

LINK_ENTITY_TO_GAME(push_point1);

LINK_ENTITY_TO_GAME(push_point2);

LINK_ENTITY_TO_GAME(push_point3);

LINK_ENTITY_TO_GAME(push_point4);

LINK_ENTITY_TO_GAME(push_point5);

LINK_ENTITY_TO_GAME(quake_nail);

LINK_ENTITY_TO_GAME(quake_rocket);

LINK_ENTITY_TO_GAME(race_controller);

LINK_ENTITY_TO_GAME(race_path);

LINK_ENTITY_TO_GAME(rage);

LINK_ENTITY_TO_GAME(rain_modify);

LINK_ENTITY_TO_GAME(rain_settings);

LINK_ENTITY_TO_GAME(random_ammo);

LINK_ENTITY_TO_GAME(random_speaker);

LINK_ENTITY_TO_GAME(random_trigger);

LINK_ENTITY_TO_GAME(random_weapon);

LINK_ENTITY_TO_GAME(redflag_follow);

LINK_ENTITY_TO_GAME(reetou_proj);

LINK_ENTITY_TO_GAME(remove_all_wpts);

LINK_ENTITY_TO_GAME(remove_wpt);

LINK_ENTITY_TO_GAME(resourcetower);

LINK_ENTITY_TO_GAME(respawn_chest);

LINK_ENTITY_TO_GAME(return_axe);

LINK_ENTITY_TO_GAME(reversecurse);

LINK_ENTITY_TO_GAME(rm_rocket);

LINK_ENTITY_TO_GAME(roach_charge);

LINK_ENTITY_TO_GAME(rocket_air_strike);

LINK_ENTITY_TO_GAME(rocket_croix);

LINK_ENTITY_TO_GAME(rocket_mine);

LINK_ENTITY_TO_GAME(rocket_napalm);

LINK_ENTITY_TO_GAME(rocket_phoenix);

LINK_ENTITY_TO_GAME(rocket_pigeon);

LINK_ENTITY_TO_GAME(rocket_snipe);

LINK_ENTITY_TO_GAME(rocketpistol_rocket);

LINK_ENTITY_TO_GAME(rope_sample);

LINK_ENTITY_TO_GAME(rope_segment);

LINK_ENTITY_TO_GAME(rpg7_rocket);

LINK_ENTITY_TO_GAME(rpg_rocket);

LINK_ENTITY_TO_GAME(rr_shell);

LINK_ENTITY_TO_GAME(rrp);

LINK_ENTITY_TO_GAME(rs_effect);

LINK_ENTITY_TO_GAME(rs_fog);

LINK_ENTITY_TO_GAME(rs_grenade);

LINK_ENTITY_TO_GAME(rs_if);

LINK_ENTITY_TO_GAME(rs_message);

LINK_ENTITY_TO_GAME(rs_particle_emitter);

LINK_ENTITY_TO_GAME(rs_roundend);

LINK_ENTITY_TO_GAME(rs_teamslot);

LINK_ENTITY_TO_GAME(rs_timer);

LINK_ENTITY_TO_GAME(rs_val);

LINK_ENTITY_TO_GAME(rs_victorypoints);

LINK_ENTITY_TO_GAME(sams_shower);

LINK_ENTITY_TO_GAME(sanity);

LINK_ENTITY_TO_GAME(satelite_laser);

LINK_ENTITY_TO_GAME(save_wpt);

LINK_ENTITY_TO_GAME(scan);

LINK_ENTITY_TO_GAME(scatterattack);

LINK_ENTITY_TO_GAME(scattersplit);

LINK_ENTITY_TO_GAME(sci_spawner);

LINK_ENTITY_TO_GAME(scientist_mine);

LINK_ENTITY_TO_GAME(scientistbomb);

LINK_ENTITY_TO_GAME(score_multiplier);

LINK_ENTITY_TO_GAME(scoreboard);

LINK_ENTITY_TO_GAME(scripted);

LINK_ENTITY_TO_GAME(scripted_action);

LINK_ENTITY_TO_GAME(scripted_sentence);

LINK_ENTITY_TO_GAME(scripted_sequence);

LINK_ENTITY_TO_GAME(scripted_tanksequence);

LINK_ENTITY_TO_GAME(scripted_trainsequence);

LINK_ENTITY_TO_GAME(sdcurse);

LINK_ENTITY_TO_GAME(secondary_point);

LINK_ENTITY_TO_GAME(securitymanager);

LINK_ENTITY_TO_GAME(sell_ak101);

LINK_ENTITY_TO_GAME(sell_ak47);

LINK_ENTITY_TO_GAME(sell_baretta);

LINK_ENTITY_TO_GAME(sell_beretta);

LINK_ENTITY_TO_GAME(sell_dblshot);

LINK_ENTITY_TO_GAME(sell_dbshot);

LINK_ENTITY_TO_GAME(sell_glock17);

LINK_ENTITY_TO_GAME(sell_longslide);

LINK_ENTITY_TO_GAME(sell_mac);

LINK_ENTITY_TO_GAME(sell_mac10);

LINK_ENTITY_TO_GAME(sell_molotov_cocktail);

LINK_ENTITY_TO_GAME(sell_mossberg);

LINK_ENTITY_TO_GAME(sell_psg1);

LINK_ENTITY_TO_GAME(sell_steyr_m40);

LINK_ENTITY_TO_GAME(sell_tec9);

LINK_ENTITY_TO_GAME(sell_tommy);

LINK_ENTITY_TO_GAME(sell_tommygun);

LINK_ENTITY_TO_GAME(sell_uzi);

LINK_ENTITY_TO_GAME(sensorychamber);

LINK_ENTITY_TO_GAME(sgvortex);

LINK_ENTITY_TO_GAME(shell_bazooka);

LINK_ENTITY_TO_GAME(shell_piat);

LINK_ENTITY_TO_GAME(shell_pschreck);

LINK_ENTITY_TO_GAME(shenlong);

LINK_ENTITY_TO_GAME(shipAI_shipmate);

LINK_ENTITY_TO_GAME(shock);

LINK_ENTITY_TO_GAME(shockH);

LINK_ENTITY_TO_GAME(shock_beam);

LINK_ENTITY_TO_GAME(shockcore);

LINK_ENTITY_TO_GAME(shotgun_crate);

LINK_ENTITY_TO_GAME(shotgun_forklift);

LINK_ENTITY_TO_GAME(shotgun_hev);

LINK_ENTITY_TO_GAME(shotgun_sci);

LINK_ENTITY_TO_GAME(show_wpt);

LINK_ENTITY_TO_GAME(shrapnel);

LINK_ENTITY_TO_GAME(shrink_timer);

LINK_ENTITY_TO_GAME(shrinker_project);

LINK_ENTITY_TO_GAME(siegeturret);

LINK_ENTITY_TO_GAME(skull);

LINK_ENTITY_TO_GAME(skunk_fart);

LINK_ENTITY_TO_GAME(slAGruntTakeCoverFromEnemy);

LINK_ENTITY_TO_GAME(slAGruntThreatDisplay);

LINK_ENTITY_TO_GAME(slAssassinTakeCoverFromBestSound);

LINK_ENTITY_TO_GAME(slAssassinTakeCoverFromEnemy);

LINK_ENTITY_TO_GAME(slAssassinTakeCoverFromEnemy2);

LINK_ENTITY_TO_GAME(slBaFaceTarget);

LINK_ENTITY_TO_GAME(slControllerTakeCover);

LINK_ENTITY_TO_GAME(slFaceTarget);

LINK_ENTITY_TO_GAME(slFaceTargetScared);

LINK_ENTITY_TO_GAME(slGruntTakeCover);

LINK_ENTITY_TO_GAME(slGruntTakeCoverFromBestSound);

LINK_ENTITY_TO_GAME(slGruntTossGrenadeCover);

LINK_ENTITY_TO_GAME(slIdleTrigger);

LINK_ENTITY_TO_GAME(slRunToScript);

LINK_ENTITY_TO_GAME(slTakeCoverFromBestSound);

LINK_ENTITY_TO_GAME(slTakeCoverFromEnemy);

LINK_ENTITY_TO_GAME(slTakeCoverFromOrigin);

LINK_ENTITY_TO_GAME(slTlkIdleEyecontact);

LINK_ENTITY_TO_GAME(slTlkIdleWatchClient);

LINK_ENTITY_TO_GAME(slTwitchDie);

LINK_ENTITY_TO_GAME(slWalkToScript);

LINK_ENTITY_TO_GAME(slowcurse);

LINK_ENTITY_TO_GAME(slowrefirecurse);

LINK_ENTITY_TO_GAME(sm_project);

LINK_ENTITY_TO_GAME(sm_rocket_01);

LINK_ENTITY_TO_GAME(smoking_grenade);

LINK_ENTITY_TO_GAME(snark_cam);

LINK_ENTITY_TO_GAME(snark_project);

LINK_ENTITY_TO_GAME(snowball);

LINK_ENTITY_TO_GAME(soundent);

LINK_ENTITY_TO_GAME(spark_shower);

LINK_ENTITY_TO_GAME(spawn_base_team1);

LINK_ENTITY_TO_GAME(spawn_base_team2);

LINK_ENTITY_TO_GAME(spawn_observer);

LINK_ENTITY_TO_GAME(spawn_player_custom);

LINK_ENTITY_TO_GAME(spawn_player_team1);

LINK_ENTITY_TO_GAME(spawn_player_team2);

LINK_ENTITY_TO_GAME(spawnfar);

LINK_ENTITY_TO_GAME(speaker);

LINK_ENTITY_TO_GAME(spear_bolt);

LINK_ENTITY_TO_GAME(specailbc);

LINK_ENTITY_TO_GAME(special_teleport_in);

LINK_ENTITY_TO_GAME(sphere_explosion);

LINK_ENTITY_TO_GAME(sphere_mine);

LINK_ENTITY_TO_GAME(spirit_bomb);

LINK_ENTITY_TO_GAME(spitgunprojectile);

LINK_ENTITY_TO_GAME(spore);

LINK_ENTITY_TO_GAME(sporegrenade);

LINK_ENTITY_TO_GAME(sporegunprojectile);

LINK_ENTITY_TO_GAME(spy_camera);

LINK_ENTITY_TO_GAME(spy_escape);

LINK_ENTITY_TO_GAME(squadmaker);

LINK_ENTITY_TO_GAME(squidspit);

LINK_ENTITY_TO_GAME(sstrike);

LINK_ENTITY_TO_GAME(st_rocket);

LINK_ENTITY_TO_GAME(streak_spiral);

LINK_ENTITY_TO_GAME(struct_goa);

LINK_ENTITY_TO_GAME(struct_goa_actual);

LINK_ENTITY_TO_GAME(struct_goa_actual_sub);

LINK_ENTITY_TO_GAME(struct_tau);

LINK_ENTITY_TO_GAME(struct_tau_actual);

LINK_ENTITY_TO_GAME(struct_tau_actual_sub);

LINK_ENTITY_TO_GAME(sturmbody);

LINK_ENTITY_TO_GAME(suck_grenade);

LINK_ENTITY_TO_GAME(suck_mine);

LINK_ENTITY_TO_GAME(sunofgod);

LINK_ENTITY_TO_GAME(swarm_garg_stomp);

LINK_ENTITY_TO_GAME(swarm_spawnpt_acontroller);

LINK_ENTITY_TO_GAME(swarm_spawnpt_agarg);

LINK_ENTITY_TO_GAME(swarm_spawnpt_aheadcrab);

LINK_ENTITY_TO_GAME(swarm_spawnpt_aslave);

LINK_ENTITY_TO_GAME(swarm_spawnpt_atripod);

LINK_ENTITY_TO_GAME(swarm_spawnpt_hassassin);

LINK_ENTITY_TO_GAME(swarm_spawnpt_hcommander);

LINK_ENTITY_TO_GAME(swarm_spawnpt_hengineer);

LINK_ENTITY_TO_GAME(swarm_spawnpt_hgrunt);

LINK_ENTITY_TO_GAME(swarm_spawnpt_hscientist);

LINK_ENTITY_TO_GAME(swarm_spawnpt_human);

LINK_ENTITY_TO_GAME(swarm_team_alien);

LINK_ENTITY_TO_GAME(swarm_team_human);

LINK_ENTITY_TO_GAME(tag_camp);

LINK_ENTITY_TO_GAME(tag_crouch_jump);

LINK_ENTITY_TO_GAME(tag_door);

LINK_ENTITY_TO_GAME(tag_duck);

LINK_ENTITY_TO_GAME(tag_jump);

LINK_ENTITY_TO_GAME(tag_run);

LINK_ENTITY_TO_GAME(tag_snipe);

LINK_ENTITY_TO_GAME(tag_walk);

LINK_ENTITY_TO_GAME(tango_nomad);

LINK_ENTITY_TO_GAME(tango_sentinel);

LINK_ENTITY_TO_GAME(tango_sniper);

LINK_ENTITY_TO_GAME(targ_speaker);

LINK_ENTITY_TO_GAME(target_all_players);

LINK_ENTITY_TO_GAME(target_animation);

LINK_ENTITY_TO_GAME(target_cdaudio);

LINK_ENTITY_TO_GAME(target_changemaster);

LINK_ENTITY_TO_GAME(target_changetarget);

LINK_ENTITY_TO_GAME(target_give);

LINK_ENTITY_TO_GAME(target_gravity);

LINK_ENTITY_TO_GAME(target_help);

LINK_ENTITY_TO_GAME(target_hurt);

LINK_ENTITY_TO_GAME(target_kill);

LINK_ENTITY_TO_GAME(target_mp3audio);

LINK_ENTITY_TO_GAME(target_print);

LINK_ENTITY_TO_GAME(target_push);

LINK_ENTITY_TO_GAME(target_random);

LINK_ENTITY_TO_GAME(target_random_player);

LINK_ENTITY_TO_GAME(target_relay);

LINK_ENTITY_TO_GAME(target_removeobject);

LINK_ENTITY_TO_GAME(target_reset);

LINK_ENTITY_TO_GAME(target_respawn);

LINK_ENTITY_TO_GAME(target_score);

LINK_ENTITY_TO_GAME(target_server_command);

LINK_ENTITY_TO_GAME(target_set_cvar);

LINK_ENTITY_TO_GAME(target_spawnitem);

LINK_ENTITY_TO_GAME(target_spectate);

LINK_ENTITY_TO_GAME(target_team);

LINK_ENTITY_TO_GAME(target_team_score);

LINK_ENTITY_TO_GAME(target_teamdie);

LINK_ENTITY_TO_GAME(target_teleport);

LINK_ENTITY_TO_GAME(target_tripmine);

LINK_ENTITY_TO_GAME(target_weaponstrip);

LINK_ENTITY_TO_GAME(target_win);

LINK_ENTITY_TO_GAME(tcontroller_cball);

LINK_ENTITY_TO_GAME(tcontroller_tball);

LINK_ENTITY_TO_GAME(team_advarmory);

LINK_ENTITY_TO_GAME(team_advturretfactory);

LINK_ENTITY_TO_GAME(team_armory);

LINK_ENTITY_TO_GAME(team_armslab);

LINK_ENTITY_TO_GAME(team_chemlab);

LINK_ENTITY_TO_GAME(team_command);

LINK_ENTITY_TO_GAME(team_goal);

LINK_ENTITY_TO_GAME(team_hive);

LINK_ENTITY_TO_GAME(team_infportal);

LINK_ENTITY_TO_GAME(team_medlab);

LINK_ENTITY_TO_GAME(team_nukeplant);

LINK_ENTITY_TO_GAME(team_observatory);

LINK_ENTITY_TO_GAME(team_prototypelab);

LINK_ENTITY_TO_GAME(team_turretfactory);

LINK_ENTITY_TO_GAME(team_webstrand);

LINK_ENTITY_TO_GAME(telebecon);

LINK_ENTITY_TO_GAME(teledeath);

LINK_ENTITY_TO_GAME(teleenter);

LINK_ENTITY_TO_GAME(temp_eject);

LINK_ENTITY_TO_GAME(teslagren);

LINK_ENTITY_TO_GAME(test_effect);

LINK_ENTITY_TO_GAME(testhull);

LINK_ENTITY_TO_GAME(testparticles);

LINK_ENTITY_TO_GAME(tf_ammo_rpgclip);

LINK_ENTITY_TO_GAME(tf_flame);

LINK_ENTITY_TO_GAME(tf_flamethrower_burst);

LINK_ENTITY_TO_GAME(tf_gl_grenade);

LINK_ENTITY_TO_GAME(tf_ic_rocket);

LINK_ENTITY_TO_GAME(tf_nailgun_nail);

LINK_ENTITY_TO_GAME(tf_rpg_rocket);

LINK_ENTITY_TO_GAME(tf_weapon_ac);

LINK_ENTITY_TO_GAME(tf_weapon_autorifle);

LINK_ENTITY_TO_GAME(tf_weapon_axe);

LINK_ENTITY_TO_GAME(tf_weapon_caltrop);

LINK_ENTITY_TO_GAME(tf_weapon_caltropgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_concussiongrenade);

LINK_ENTITY_TO_GAME(tf_weapon_empgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_flamethrower);

LINK_ENTITY_TO_GAME(tf_weapon_gasgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_genericprimedgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_gl);

LINK_ENTITY_TO_GAME(tf_weapon_ic);

LINK_ENTITY_TO_GAME(tf_weapon_knife);

LINK_ENTITY_TO_GAME(tf_weapon_medikit);

LINK_ENTITY_TO_GAME(tf_weapon_mirvbomblet);

LINK_ENTITY_TO_GAME(tf_weapon_mirvgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_nailgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_napalmgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_ng);

LINK_ENTITY_TO_GAME(tf_weapon_normalgrenade);

LINK_ENTITY_TO_GAME(tf_weapon_pl);

LINK_ENTITY_TO_GAME(tf_weapon_railgun);

LINK_ENTITY_TO_GAME(tf_weapon_rpg);

LINK_ENTITY_TO_GAME(tf_weapon_shotgun);

LINK_ENTITY_TO_GAME(tf_weapon_sniperrifle);

LINK_ENTITY_TO_GAME(tf_weapon_spanner);

LINK_ENTITY_TO_GAME(tf_weapon_superng);

LINK_ENTITY_TO_GAME(tf_weapon_supershotgun);

LINK_ENTITY_TO_GAME(tf_weapon_tranq);

LINK_ENTITY_TO_GAME(tgrenade);

LINK_ENTITY_TO_GAME(thornbush);

LINK_ENTITY_TO_GAME(throwing_knife);

LINK_ENTITY_TO_GAME(thrown_katana);

LINK_ENTITY_TO_GAME(thrown_knife);

LINK_ENTITY_TO_GAME(thrown_object);

LINK_ENTITY_TO_GAME(thrown_sledge);

LINK_ENTITY_TO_GAME(timed_grenade);

LINK_ENTITY_TO_GAME(timedevent);

LINK_ENTITY_TO_GAME(timer);

LINK_ENTITY_TO_GAME(timer_add);

LINK_ENTITY_TO_GAME(tlAGruntTakeCoverFromEnemy);

LINK_ENTITY_TO_GAME(tlAGruntThreatDisplay);

LINK_ENTITY_TO_GAME(tlAssassinTakeCoverFromBestSound);

LINK_ENTITY_TO_GAME(tlAssassinTakeCoverFromEnemy);

LINK_ENTITY_TO_GAME(tlAssassinTakeCoverFromEnemy2);

LINK_ENTITY_TO_GAME(tlBaFaceTarget);

LINK_ENTITY_TO_GAME(tlControllerTakeCover);

LINK_ENTITY_TO_GAME(tlFaceTarget);

LINK_ENTITY_TO_GAME(tlFaceTargetScared);

LINK_ENTITY_TO_GAME(tlGruntTakeCover1);

LINK_ENTITY_TO_GAME(tlGruntTakeCoverFromBestSound);

LINK_ENTITY_TO_GAME(tlGruntTossGrenadeCover1);

LINK_ENTITY_TO_GAME(tlTakeCoverFromBestSound);

LINK_ENTITY_TO_GAME(tlTakeCoverFromEnemy);

LINK_ENTITY_TO_GAME(tlTakeCoverFromOrigin);

LINK_ENTITY_TO_GAME(tlTlkIdleEyecontact);

LINK_ENTITY_TO_GAME(tlTlkIdleWatchClient);

LINK_ENTITY_TO_GAME(tlTlkIdleWatchClientStare);

LINK_ENTITY_TO_GAME(tlTwitchDie);

LINK_ENTITY_TO_GAME(tod_control_area);

LINK_ENTITY_TO_GAME(tod_control_point);

LINK_ENTITY_TO_GAME(tod_notripmine_area);

LINK_ENTITY_TO_GAME(tod_roundend_trigger);

LINK_ENTITY_TO_GAME(tod_roundstart_trigger);

LINK_ENTITY_TO_GAME(tod_roundwon_trigger);

LINK_ENTITY_TO_GAME(tombstone);

LINK_ENTITY_TO_GAME(translocator_disc);

LINK_ENTITY_TO_GAME(transphasic_gib);

LINK_ENTITY_TO_GAME(transport_apc);

LINK_ENTITY_TO_GAME(transport_blackhawk);

LINK_ENTITY_TO_GAME(treasure_chest);

LINK_ENTITY_TO_GAME(trenches_allied_start);

LINK_ENTITY_TO_GAME(trenches_axis_start);

LINK_ENTITY_TO_GAME(trenches_central_start);

LINK_ENTITY_TO_GAME(trigger);

LINK_ENTITY_TO_GAME(trigger_GoalLine);

LINK_ENTITY_TO_GAME(trigger_PenaltyBox);

LINK_ENTITY_TO_GAME(trigger_SideLine);

LINK_ENTITY_TO_GAME(trigger_adv_teleport);

LINK_ENTITY_TO_GAME(trigger_armory);

LINK_ENTITY_TO_GAME(trigger_assault);

LINK_ENTITY_TO_GAME(trigger_auto);

LINK_ENTITY_TO_GAME(trigger_autosave);

LINK_ENTITY_TO_GAME(trigger_ball);

LINK_ENTITY_TO_GAME(trigger_baril);

LINK_ENTITY_TO_GAME(trigger_bounce);

LINK_ENTITY_TO_GAME(trigger_camera);

LINK_ENTITY_TO_GAME(trigger_capturetimer);

LINK_ENTITY_TO_GAME(trigger_captureupdate);

LINK_ENTITY_TO_GAME(trigger_cdaudio);

LINK_ENTITY_TO_GAME(trigger_changealias);

LINK_ENTITY_TO_GAME(trigger_changecvar);

LINK_ENTITY_TO_GAME(trigger_changelevel);

LINK_ENTITY_TO_GAME(trigger_changetarget);

LINK_ENTITY_TO_GAME(trigger_changevalue);

LINK_ENTITY_TO_GAME(trigger_coldbreath);

LINK_ENTITY_TO_GAME(trigger_comet);

LINK_ENTITY_TO_GAME(trigger_command);

LINK_ENTITY_TO_GAME(trigger_control);

LINK_ENTITY_TO_GAME(trigger_coop);

LINK_ENTITY_TO_GAME(trigger_counter);

LINK_ENTITY_TO_GAME(trigger_course);

LINK_ENTITY_TO_GAME(trigger_crash);

LINK_ENTITY_TO_GAME(trigger_ctf);

LINK_ENTITY_TO_GAME(trigger_ctfgeneric);

LINK_ENTITY_TO_GAME(trigger_deathmatch);

LINK_ENTITY_TO_GAME(trigger_deliveryzone);

LINK_ENTITY_TO_GAME(trigger_die);

LINK_ENTITY_TO_GAME(trigger_discerturn);

LINK_ENTITY_TO_GAME(trigger_discreturn);

LINK_ENTITY_TO_GAME(trigger_endround);

LINK_ENTITY_TO_GAME(trigger_endsection);

LINK_ENTITY_TO_GAME(trigger_env_hurt);

LINK_ENTITY_TO_GAME(trigger_execcommand);

LINK_ENTITY_TO_GAME(trigger_fall);

LINK_ENTITY_TO_GAME(trigger_flagcheck);

LINK_ENTITY_TO_GAME(trigger_geneworm_hit);

LINK_ENTITY_TO_GAME(trigger_goal);

LINK_ENTITY_TO_GAME(trigger_golem);

LINK_ENTITY_TO_GAME(trigger_gravity);

LINK_ENTITY_TO_GAME(trigger_gunmanteleport);

LINK_ENTITY_TO_GAME(trigger_hevcharge);

LINK_ENTITY_TO_GAME(trigger_hold);

LINK_ENTITY_TO_GAME(trigger_hurt);

LINK_ENTITY_TO_GAME(trigger_hurt_bleu);

LINK_ENTITY_TO_GAME(trigger_hurt_rouge);

LINK_ENTITY_TO_GAME(trigger_inout);

LINK_ENTITY_TO_GAME(trigger_jail);

LINK_ENTITY_TO_GAME(trigger_jail_release);

LINK_ENTITY_TO_GAME(trigger_jetpack);

LINK_ENTITY_TO_GAME(trigger_jump);

LINK_ENTITY_TO_GAME(trigger_jumppad);

LINK_ENTITY_TO_GAME(trigger_katehealth);

LINK_ENTITY_TO_GAME(trigger_kill_nogib);

LINK_ENTITY_TO_GAME(trigger_killmonster);

LINK_ENTITY_TO_GAME(trigger_lightstyle);

LINK_ENTITY_TO_GAME(trigger_loadhazard);

LINK_ENTITY_TO_GAME(trigger_mine);

LINK_ENTITY_TO_GAME(trigger_monsterjump);

LINK_ENTITY_TO_GAME(trigger_motion);

LINK_ENTITY_TO_GAME(trigger_multiple);

LINK_ENTITY_TO_GAME(trigger_music);

LINK_ENTITY_TO_GAME(trigger_noclaymores);

LINK_ENTITY_TO_GAME(trigger_npcarea);

LINK_ENTITY_TO_GAME(trigger_objectcapture);

LINK_ENTITY_TO_GAME(trigger_once);

LINK_ENTITY_TO_GAME(trigger_once_round);

LINK_ENTITY_TO_GAME(trigger_onsight);

LINK_ENTITY_TO_GAME(trigger_particles);

LINK_ENTITY_TO_GAME(trigger_playerfreeze);

LINK_ENTITY_TO_GAME(trigger_point_controle);

LINK_ENTITY_TO_GAME(trigger_presence);

LINK_ENTITY_TO_GAME(trigger_push);

LINK_ENTITY_TO_GAME(trigger_random);

LINK_ENTITY_TO_GAME(trigger_random_group);

LINK_ENTITY_TO_GAME(trigger_random_time);

LINK_ENTITY_TO_GAME(trigger_random_unique);

LINK_ENTITY_TO_GAME(trigger_regen);

LINK_ENTITY_TO_GAME(trigger_reinforcements);

LINK_ENTITY_TO_GAME(trigger_relay);

LINK_ENTITY_TO_GAME(trigger_respawn);

LINK_ENTITY_TO_GAME(trigger_rndbased);

LINK_ENTITY_TO_GAME(trigger_rottest);

LINK_ENTITY_TO_GAME(trigger_rs_hurt);

LINK_ENTITY_TO_GAME(trigger_script);

LINK_ENTITY_TO_GAME(trigger_secret);

LINK_ENTITY_TO_GAME(trigger_section);

LINK_ENTITY_TO_GAME(trigger_setflag);

LINK_ENTITY_TO_GAME(trigger_setorigin);

LINK_ENTITY_TO_GAME(trigger_shutup);

LINK_ENTITY_TO_GAME(trigger_sound);

LINK_ENTITY_TO_GAME(trigger_speed);

LINK_ENTITY_TO_GAME(trigger_sponly);

LINK_ENTITY_TO_GAME(trigger_startpatrol);

LINK_ENTITY_TO_GAME(trigger_teampush);

LINK_ENTITY_TO_GAME(trigger_teleport);

LINK_ENTITY_TO_GAME(trigger_transition);

LINK_ENTITY_TO_GAME(trigger_war);

LINK_ENTITY_TO_GAME(trigger_xen_return);

LINK_ENTITY_TO_GAME(trigger_zone);

LINK_ENTITY_TO_GAME(trigger_zone_bleu);

LINK_ENTITY_TO_GAME(trigger_zone_rouge);

LINK_ENTITY_TO_GAME(trip_beam);

LINK_ENTITY_TO_GAME(ts_bomb);

LINK_ENTITY_TO_GAME(ts_dmhill);

LINK_ENTITY_TO_GAME(ts_groundweapon);

LINK_ENTITY_TO_GAME(ts_hack);

LINK_ENTITY_TO_GAME(ts_mapglobals);

LINK_ENTITY_TO_GAME(ts_model);

LINK_ENTITY_TO_GAME(ts_objective_manager);

LINK_ENTITY_TO_GAME(ts_objective_ptr);

LINK_ENTITY_TO_GAME(ts_powerup);

LINK_ENTITY_TO_GAME(ts_slowmotion);

LINK_ENTITY_TO_GAME(ts_slowmotionpoint);

LINK_ENTITY_TO_GAME(ts_teamescape);

LINK_ENTITY_TO_GAME(ts_trigger);

LINK_ENTITY_TO_GAME(ts_vipescape);

LINK_ENTITY_TO_GAME(ts_wingiver);

LINK_ENTITY_TO_GAME(turf_zone);

LINK_ENTITY_TO_GAME(turret);

LINK_ENTITY_TO_GAME(ultima_grenade);

LINK_ENTITY_TO_GAME(umbracloud);

LINK_ENTITY_TO_GAME(umbraprojectile);

LINK_ENTITY_TO_GAME(uw_monster_hybrid);

LINK_ENTITY_TO_GAME(uw_monster_michael);

LINK_ENTITY_TO_GAME(uw_player_lycan);

LINK_ENTITY_TO_GAME(uw_player_vamp);

LINK_ENTITY_TO_GAME(uw_player_view);

LINK_ENTITY_TO_GAME(vehicle);

LINK_ENTITY_TO_GAME(vehicle_dreadnaught);

LINK_ENTITY_TO_GAME(vehicle_dreadnaught_weapon);

LINK_ENTITY_TO_GAME(vehicle_tank);

LINK_ENTITY_TO_GAME(vehicle_thing);

LINK_ENTITY_TO_GAME(vehicle_wraithlord);

LINK_ENTITY_TO_GAME(vehicle_wraithlord_weapon);

LINK_ENTITY_TO_GAME(victim_scientist);

LINK_ENTITY_TO_GAME(vipescape);

LINK_ENTITY_TO_GAME(virtual_hull);

LINK_ENTITY_TO_GAME(voltigoreshock);

LINK_ENTITY_TO_GAME(vote_llama);

LINK_ENTITY_TO_GAME(vscrossbow_bolt);

LINK_ENTITY_TO_GAME(watcher);

LINK_ENTITY_TO_GAME(watcher_count);

LINK_ENTITY_TO_GAME(weapmortar);

LINK_ENTITY_TO_GAME(weapon_1100);

LINK_ENTITY_TO_GAME(weapon_1445lda);

LINK_ENTITY_TO_GAME(weapon_1911);

LINK_ENTITY_TO_GAME(weapon_30cal);

LINK_ENTITY_TO_GAME(weapon_30mmsg);

LINK_ENTITY_TO_GAME(weapon_357);

LINK_ENTITY_TO_GAME(weapon_40gl);

LINK_ENTITY_TO_GAME(weapon_44sw);

LINK_ENTITY_TO_GAME(weapon_50cal);

LINK_ENTITY_TO_GAME(weapon_556AR);

LINK_ENTITY_TO_GAME(weapon_90mm);

LINK_ENTITY_TO_GAME(weapon_92d);

LINK_ENTITY_TO_GAME(weapon_9mmAR);

LINK_ENTITY_TO_GAME(weapon_9mmhandgun);

LINK_ENTITY_TO_GAME(weapon_9mmm41a);

LINK_ENTITY_TO_GAME(weapon_BritGrenMelee);

LINK_ENTITY_TO_GAME(weapon_BritGrenNade);

LINK_ENTITY_TO_GAME(weapon_BrownBess);

LINK_ENTITY_TO_GAME(weapon_CEnfield);

LINK_ENTITY_TO_GAME(weapon_CEnfieldMelee);

LINK_ENTITY_TO_GAME(weapon_CEnfieldNade);

LINK_ENTITY_TO_GAME(weapon_Charleville);

LINK_ENTITY_TO_GAME(weapon_FT);

LINK_ENTITY_TO_GAME(weapon_G11);

LINK_ENTITY_TO_GAME(weapon_GMauserG98);

LINK_ENTITY_TO_GAME(weapon_GMauserG98Melee);

LINK_ENTITY_TO_GAME(weapon_GMauserG98Nade);

LINK_ENTITY_TO_GAME(weapon_GermGrenMelee);

LINK_ENTITY_TO_GAME(weapon_GermGrenNade);

LINK_ENTITY_TO_GAME(weapon_Knife);

LINK_ENTITY_TO_GAME(weapon_Pennsylvania);

LINK_ENTITY_TO_GAME(weapon_PistolA);

LINK_ENTITY_TO_GAME(weapon_PistolB);

LINK_ENTITY_TO_GAME(weapon_Revolutionnaire);

LINK_ENTITY_TO_GAME(weapon_SPchemicalgun);

LINK_ENTITY_TO_GAME(weapon_Sabre);

LINK_ENTITY_TO_GAME(weapon_a5);

LINK_ENTITY_TO_GAME(weapon_aandagger);

LINK_ENTITY_TO_GAME(weapon_acannon);

LINK_ENTITY_TO_GAME(weapon_acidrocket);

LINK_ENTITY_TO_GAME(weapon_acidrocketgun);

LINK_ENTITY_TO_GAME(weapon_adrenaline);

LINK_ENTITY_TO_GAME(weapon_aicore);

LINK_ENTITY_TO_GAME(weapon_air_strike);

LINK_ENTITY_TO_GAME(weapon_ak);

LINK_ENTITY_TO_GAME(weapon_ak101);

LINK_ENTITY_TO_GAME(weapon_ak47);

LINK_ENTITY_TO_GAME(weapon_ak5);

LINK_ENTITY_TO_GAME(weapon_ak5s);

LINK_ENTITY_TO_GAME(weapon_ak74);

LINK_ENTITY_TO_GAME(weapon_akimbo_glocks);

LINK_ENTITY_TO_GAME(weapon_akimbob);

LINK_ENTITY_TO_GAME(weapon_akimboberettas);

LINK_ENTITY_TO_GAME(weapon_akimbocolts);

LINK_ENTITY_TO_GAME(weapon_akimbodeagles);

LINK_ENTITY_TO_GAME(weapon_akimbogun);

LINK_ENTITY_TO_GAME(weapon_akimbosawedoffs);

LINK_ENTITY_TO_GAME(weapon_akimcolt);

LINK_ENTITY_TO_GAME(weapon_akimsaa);

LINK_ENTITY_TO_GAME(weapon_aks74u);

LINK_ENTITY_TO_GAME(weapon_alarm);

LINK_ENTITY_TO_GAME(weapon_aliencannon);

LINK_ENTITY_TO_GAME(weapon_amerknife);

LINK_ENTITY_TO_GAME(weapon_ammo);

LINK_ENTITY_TO_GAME(weapon_amp);

LINK_ENTITY_TO_GAME(weapon_anaconda);

LINK_ENTITY_TO_GAME(weapon_angel);

LINK_ENTITY_TO_GAME(weapon_ar10);

LINK_ENTITY_TO_GAME(weapon_ar33);

LINK_ENTITY_TO_GAME(weapon_assaultminigun);

LINK_ENTITY_TO_GAME(weapon_aug);

LINK_ENTITY_TO_GAME(weapon_autococker);

LINK_ENTITY_TO_GAME(weapon_autogl);

LINK_ENTITY_TO_GAME(weapon_automag);

LINK_ENTITY_TO_GAME(weapon_autoshotgun);

LINK_ENTITY_TO_GAME(weapon_awm);

LINK_ENTITY_TO_GAME(weapon_awp);

LINK_ENTITY_TO_GAME(weapon_axe);

LINK_ENTITY_TO_GAME(weapon_axes);

LINK_ENTITY_TO_GAME(weapon_b93r);

LINK_ENTITY_TO_GAME(weapon_babblergun);

LINK_ENTITY_TO_GAME(weapon_babblerprojectile);

LINK_ENTITY_TO_GAME(weapon_balllightningspell);

LINK_ENTITY_TO_GAME(weapon_banana);

LINK_ENTITY_TO_GAME(weapon_banane);

LINK_ENTITY_TO_GAME(weapon_bandage);

LINK_ENTITY_TO_GAME(weapon_bandsaw);

LINK_ENTITY_TO_GAME(weapon_bang);

LINK_ENTITY_TO_GAME(weapon_bar);

LINK_ENTITY_TO_GAME(weapon_barett);

LINK_ENTITY_TO_GAME(weapon_baretta);

LINK_ENTITY_TO_GAME(weapon_barnacle);

LINK_ENTITY_TO_GAME(weapon_barney9mmar);

LINK_ENTITY_TO_GAME(weapon_barney9mmhg);

LINK_ENTITY_TO_GAME(weapon_barneyhandgrenade);

LINK_ENTITY_TO_GAME(weapon_barneyshotgun);

LINK_ENTITY_TO_GAME(weapon_barrett82);

LINK_ENTITY_TO_GAME(weapon_baseballbat);

LINK_ENTITY_TO_GAME(weapon_bat);

LINK_ENTITY_TO_GAME(weapon_baton);

LINK_ENTITY_TO_GAME(weapon_batsup);

LINK_ENTITY_TO_GAME(weapon_batte);

LINK_ENTITY_TO_GAME(weapon_battleaxe);

LINK_ENTITY_TO_GAME(weapon_bayonet);

LINK_ENTITY_TO_GAME(weapon_bazooka);

LINK_ENTITY_TO_GAME(weapon_beamgun);

LINK_ENTITY_TO_GAME(weapon_beanstalkspell);

LINK_ENTITY_TO_GAME(weapon_bearbite);

LINK_ENTITY_TO_GAME(weapon_bearclaw);

LINK_ENTITY_TO_GAME(weapon_beartrap);

LINK_ENTITY_TO_GAME(weapon_benelli);

LINK_ENTITY_TO_GAME(weapon_ber92f);

LINK_ENTITY_TO_GAME(weapon_ber92fs);

LINK_ENTITY_TO_GAME(weapon_ber93r);

LINK_ENTITY_TO_GAME(weapon_beretta);

LINK_ENTITY_TO_GAME(weapon_beretta_a);

LINK_ENTITY_TO_GAME(weapon_bfg);

LINK_ENTITY_TO_GAME(weapon_bigaxe);

LINK_ENTITY_TO_GAME(weapon_bigbang);

LINK_ENTITY_TO_GAME(weapon_biggun);

LINK_ENTITY_TO_GAME(weapon_bilebomb);

LINK_ENTITY_TO_GAME(weapon_bilebombgun);

LINK_ENTITY_TO_GAME(weapon_binoculars);

LINK_ENTITY_TO_GAME(weapon_binos);

LINK_ENTITY_TO_GAME(weapon_birdspell);

LINK_ENTITY_TO_GAME(weapon_bite2gun);

LINK_ENTITY_TO_GAME(weapon_bitegun);

LINK_ENTITY_TO_GAME(weapon_bizon);

LINK_ENTITY_TO_GAME(weapon_blaster);

LINK_ENTITY_TO_GAME(weapon_blazer);

LINK_ENTITY_TO_GAME(weapon_blink);

LINK_ENTITY_TO_GAME(weapon_blowpipe);

LINK_ENTITY_TO_GAME(weapon_bodypart);

LINK_ENTITY_TO_GAME(weapon_bola);

LINK_ENTITY_TO_GAME(weapon_boltrifle);

LINK_ENTITY_TO_GAME(weapon_bomb);

LINK_ENTITY_TO_GAME(weapon_bomber);

LINK_ENTITY_TO_GAME(weapon_bow);

LINK_ENTITY_TO_GAME(weapon_bren);

LINK_ENTITY_TO_GAME(weapon_brickcannon);

LINK_ENTITY_TO_GAME(weapon_briefcase);

LINK_ENTITY_TO_GAME(weapon_bsword);

LINK_ENTITY_TO_GAME(weapon_buffalo);

LINK_ENTITY_TO_GAME(weapon_burningattack);

LINK_ENTITY_TO_GAME(weapon_bush);

LINK_ENTITY_TO_GAME(weapon_bushmaster);

LINK_ENTITY_TO_GAME(weapon_c4);

LINK_ENTITY_TO_GAME(weapon_cal50);

LINK_ENTITY_TO_GAME(weapon_camera);

LINK_ENTITY_TO_GAME(weapon_candy);

LINK_ENTITY_TO_GAME(weapon_canister);

LINK_ENTITY_TO_GAME(weapon_cannon);

LINK_ENTITY_TO_GAME(weapon_case);

LINK_ENTITY_TO_GAME(weapon_cat);

LINK_ENTITY_TO_GAME(weapon_cattleprod);

LINK_ENTITY_TO_GAME(weapon_caws);

LINK_ENTITY_TO_GAME(weapon_ce_lasgun);

LINK_ENTITY_TO_GAME(weapon_ce_powersword);

LINK_ENTITY_TO_GAME(weapon_ce_psistaff);

LINK_ENTITY_TO_GAME(weapon_ce_rocketlauncher);

LINK_ENTITY_TO_GAME(weapon_ce_shurikencatapult);

LINK_ENTITY_TO_GAME(weapon_ce_shurikenpistol);

LINK_ENTITY_TO_GAME(weapon_cellphone);

LINK_ENTITY_TO_GAME(weapon_chaingun);

LINK_ENTITY_TO_GAME(weapon_chainsaw);

LINK_ENTITY_TO_GAME(weapon_charge);

LINK_ENTITY_TO_GAME(weapon_charged);

LINK_ENTITY_TO_GAME(weapon_chronosceptor);

LINK_ENTITY_TO_GAME(weapon_claw);

LINK_ENTITY_TO_GAME(weapon_claws);

LINK_ENTITY_TO_GAME(weapon_claymore);

LINK_ENTITY_TO_GAME(weapon_clip_generic);

LINK_ENTITY_TO_GAME(weapon_cloak);

LINK_ENTITY_TO_GAME(weapon_cloaker);

LINK_ENTITY_TO_GAME(weapon_cluster);

LINK_ENTITY_TO_GAME(weapon_cm_autocannon);

LINK_ENTITY_TO_GAME(weapon_cm_bolter);

LINK_ENTITY_TO_GAME(weapon_cm_bolterflamer);

LINK_ENTITY_TO_GAME(weapon_cm_boltpistol);

LINK_ENTITY_TO_GAME(weapon_cm_chainaxe);

LINK_ENTITY_TO_GAME(weapon_cm_chainsword);

LINK_ENTITY_TO_GAME(weapon_cm_flamer);

LINK_ENTITY_TO_GAME(weapon_cm_fraggrenade);

LINK_ENTITY_TO_GAME(weapon_cm_heavybolter);

LINK_ENTITY_TO_GAME(weapon_cm_lascannon);

LINK_ENTITY_TO_GAME(weapon_cm_missilelauncher);

LINK_ENTITY_TO_GAME(weapon_cm_stormbolter);

LINK_ENTITY_TO_GAME(weapon_cocker);

LINK_ENTITY_TO_GAME(weapon_cocogrenade);

LINK_ENTITY_TO_GAME(weapon_cocolauncher);

LINK_ENTITY_TO_GAME(weapon_colt);

LINK_ENTITY_TO_GAME(weapon_colt45);

LINK_ENTITY_TO_GAME(weapon_coltgov);

LINK_ENTITY_TO_GAME(weapon_colts);

LINK_ENTITY_TO_GAME(weapon_combatknife);

LINK_ENTITY_TO_GAME(weapon_combospell);

LINK_ENTITY_TO_GAME(weapon_cometspell);

LINK_ENTITY_TO_GAME(weapon_concussion);

LINK_ENTITY_TO_GAME(weapon_controlled);

LINK_ENTITY_TO_GAME(weapon_cougar);

LINK_ENTITY_TO_GAME(weapon_coujaf);

LINK_ENTITY_TO_GAME(weapon_couteau);

LINK_ENTITY_TO_GAME(weapon_croix);

LINK_ENTITY_TO_GAME(weapon_crossbow);

LINK_ENTITY_TO_GAME(weapon_crowbar);

LINK_ENTITY_TO_GAME(weapon_crowbar_electric);

LINK_ENTITY_TO_GAME(weapon_cutlass);

LINK_ENTITY_TO_GAME(weapon_cz75);

LINK_ENTITY_TO_GAME(weapon_d5k);

LINK_ENTITY_TO_GAME(weapon_dagger);

LINK_ENTITY_TO_GAME(weapon_daystick);

LINK_ENTITY_TO_GAME(weapon_dblpistolet);

LINK_ENTITY_TO_GAME(weapon_dblshot);

LINK_ENTITY_TO_GAME(weapon_dbshot);

LINK_ENTITY_TO_GAME(weapon_dd44);

LINK_ENTITY_TO_GAME(weapon_de50);

LINK_ENTITY_TO_GAME(weapon_deagle);

LINK_ENTITY_TO_GAME(weapon_deathball);

LINK_ENTITY_TO_GAME(weapon_deathrayspell);

LINK_ENTITY_TO_GAME(weapon_demo);

LINK_ENTITY_TO_GAME(weapon_deploygun);

LINK_ENTITY_TO_GAME(weapon_desert);

LINK_ENTITY_TO_GAME(weapon_deserteagle);

LINK_ENTITY_TO_GAME(weapon_destructodisc);

LINK_ENTITY_TO_GAME(weapon_devastator);

LINK_ENTITY_TO_GAME(weapon_devour);

LINK_ENTITY_TO_GAME(weapon_disc);

LINK_ENTITY_TO_GAME(weapon_disclauncher);

LINK_ENTITY_TO_GAME(weapon_discoballs);

LINK_ENTITY_TO_GAME(weapon_disintegrator);

LINK_ENTITY_TO_GAME(weapon_displacer);

LINK_ENTITY_TO_GAME(weapon_divinewind);

LINK_ENTITY_TO_GAME(weapon_dm4);

LINK_ENTITY_TO_GAME(weapon_dml);

LINK_ENTITY_TO_GAME(weapon_double);

LINK_ENTITY_TO_GAME(weapon_doublemagicmisslespell);

LINK_ENTITY_TO_GAME(weapon_doubleshotgun);

LINK_ENTITY_TO_GAME(weapon_dragonball);

LINK_ENTITY_TO_GAME(weapon_dragonbreathspell);

LINK_ENTITY_TO_GAME(weapon_dragonspell);

LINK_ENTITY_TO_GAME(weapon_dragunov);

LINK_ENTITY_TO_GAME(weapon_dsr1);

LINK_ENTITY_TO_GAME(weapon_dualberettas);

LINK_ENTITY_TO_GAME(weapon_dualscorpion);

LINK_ENTITY_TO_GAME(weapon_dummy);

LINK_ENTITY_TO_GAME(weapon_dynamite);

LINK_ENTITY_TO_GAME(weapon_dynomite);

LINK_ENTITY_TO_GAME(weapon_eagle);

LINK_ENTITY_TO_GAME(weapon_eballblaster);

LINK_ENTITY_TO_GAME(weapon_egon);

LINK_ENTITY_TO_GAME(weapon_el_standard);

LINK_ENTITY_TO_GAME(weapon_electro);

LINK_ENTITY_TO_GAME(weapon_elite);

LINK_ENTITY_TO_GAME(weapon_emag);

LINK_ENTITY_TO_GAME(weapon_empcannon);

LINK_ENTITY_TO_GAME(weapon_emsniper);

LINK_ENTITY_TO_GAME(weapon_energy);

LINK_ENTITY_TO_GAME(weapon_enfield);

LINK_ENTITY_TO_GAME(weapon_excal);

LINK_ENTITY_TO_GAME(weapon_experimental);

LINK_ENTITY_TO_GAME(weapon_expgrenade);

LINK_ENTITY_TO_GAME(weapon_exploder);

LINK_ENTITY_TO_GAME(weapon_explogun);

LINK_ENTITY_TO_GAME(weapon_eyelaser);

LINK_ENTITY_TO_GAME(weapon_famas);

LINK_ENTITY_TO_GAME(weapon_fev);

LINK_ENTITY_TO_GAME(weapon_fg42);

LINK_ENTITY_TO_GAME(weapon_finalflash);

LINK_ENTITY_TO_GAME(weapon_fingerlaser);

LINK_ENTITY_TO_GAME(weapon_finishingbuster);

LINK_ENTITY_TO_GAME(weapon_fireballspell);

LINK_ENTITY_TO_GAME(weapon_fishingrod);

LINK_ENTITY_TO_GAME(weapon_fist);

LINK_ENTITY_TO_GAME(weapon_fists);

LINK_ENTITY_TO_GAME(weapon_fiveseven);

LINK_ENTITY_TO_GAME(weapon_flail);

LINK_ENTITY_TO_GAME(weapon_flakcannon);

LINK_ENTITY_TO_GAME(weapon_flame);

LINK_ENTITY_TO_GAME(weapon_flamegrenade);

LINK_ENTITY_TO_GAME(weapon_flamegun);

LINK_ENTITY_TO_GAME(weapon_flamelickspell);

LINK_ENTITY_TO_GAME(weapon_flamer);

LINK_ENTITY_TO_GAME(weapon_flamethrower);

LINK_ENTITY_TO_GAME(weapon_flaregun);

LINK_ENTITY_TO_GAME(weapon_flarepistol);

LINK_ENTITY_TO_GAME(weapon_flashbang);

LINK_ENTITY_TO_GAME(weapon_fleshgrenade);

LINK_ENTITY_TO_GAME(weapon_flintlock);

LINK_ENTITY_TO_GAME(weapon_flute);

LINK_ENTITY_TO_GAME(weapon_fnfal);

LINK_ENTITY_TO_GAME(weapon_fnp90);

LINK_ENTITY_TO_GAME(weapon_forcespell);

LINK_ENTITY_TO_GAME(weapon_frag);

LINK_ENTITY_TO_GAME(weapon_fraggrenade);

LINK_ENTITY_TO_GAME(weapon_freeze);

LINK_ENTITY_TO_GAME(weapon_freezer);

LINK_ENTITY_TO_GAME(weapon_freezerayspell);

LINK_ENTITY_TO_GAME(weapon_friezadisc);

LINK_ENTITY_TO_GAME(weapon_froster);

LINK_ENTITY_TO_GAME(weapon_fusil);

LINK_ENTITY_TO_GAME(weapon_g11);

LINK_ENTITY_TO_GAME(weapon_g36);

LINK_ENTITY_TO_GAME(weapon_g36c);

LINK_ENTITY_TO_GAME(weapon_g36cs);

LINK_ENTITY_TO_GAME(weapon_g36e);

LINK_ENTITY_TO_GAME(weapon_g36k);

LINK_ENTITY_TO_GAME(weapon_g3a3);

LINK_ENTITY_TO_GAME(weapon_g3sg1);

LINK_ENTITY_TO_GAME(weapon_ga2uss);

LINK_ENTITY_TO_GAME(weapon_gacgoa);

LINK_ENTITY_TO_GAME(weapon_galil);

LINK_ENTITY_TO_GAME(weapon_gallitgun);

LINK_ENTITY_TO_GAME(weapon_garand);

LINK_ENTITY_TO_GAME(weapon_gasgrenade);

LINK_ENTITY_TO_GAME(weapon_gatlin);

LINK_ENTITY_TO_GAME(weapon_gatling);

LINK_ENTITY_TO_GAME(weapon_gattlinggun);

LINK_ENTITY_TO_GAME(weapon_gauss);

LINK_ENTITY_TO_GAME(weapon_gausspistol);

LINK_ENTITY_TO_GAME(weapon_generic);

LINK_ENTITY_TO_GAME(weapon_genericbeam);

LINK_ENTITY_TO_GAME(weapon_gerknife);

LINK_ENTITY_TO_GAME(weapon_germanknife);

LINK_ENTITY_TO_GAME(weapon_gewehr);

LINK_ENTITY_TO_GAME(weapon_giantplantspell);

LINK_ENTITY_TO_GAME(weapon_glauncher);

LINK_ENTITY_TO_GAME(weapon_glock);

LINK_ENTITY_TO_GAME(weapon_glock17);

LINK_ENTITY_TO_GAME(weapon_glock18);

LINK_ENTITY_TO_GAME(weapon_glock_auto);

LINK_ENTITY_TO_GAME(weapon_glock_auto_a);

LINK_ENTITY_TO_GAME(weapon_glstaff);

LINK_ENTITY_TO_GAME(weapon_gluongun);

LINK_ENTITY_TO_GAME(weapon_goaregen);

LINK_ENTITY_TO_GAME(weapon_goldpp7);

LINK_ENTITY_TO_GAME(weapon_gps);

LINK_ENTITY_TO_GAME(weapon_grappin);

LINK_ENTITY_TO_GAME(weapon_grapple);

LINK_ENTITY_TO_GAME(weapon_grease);

LINK_ENTITY_TO_GAME(weapon_greasegun);

LINK_ENTITY_TO_GAME(weapon_gren);

LINK_ENTITY_TO_GAME(weapon_grenade);

LINK_ENTITY_TO_GAME(weapon_grenadegun);

LINK_ENTITY_TO_GAME(weapon_grenadelauncher);

LINK_ENTITY_TO_GAME(weapon_guardian);

LINK_ENTITY_TO_GAME(weapon_hache);

LINK_ENTITY_TO_GAME(weapon_hammer);

LINK_ENTITY_TO_GAME(weapon_hand);

LINK_ENTITY_TO_GAME(weapon_handcannon);

LINK_ENTITY_TO_GAME(weapon_handgrenade);

LINK_ENTITY_TO_GAME(weapon_handgrenade_ex);

LINK_ENTITY_TO_GAME(weapon_harpoongun);

LINK_ENTITY_TO_GAME(weapon_he);

LINK_ENTITY_TO_GAME(weapon_healingspray);

LINK_ENTITY_TO_GAME(weapon_heavymachinegun);

LINK_ENTITY_TO_GAME(weapon_hegrenade);

LINK_ENTITY_TO_GAME(weapon_helico);

LINK_ENTITY_TO_GAME(weapon_hk21);

LINK_ENTITY_TO_GAME(weapon_hk33);

LINK_ENTITY_TO_GAME(weapon_hk33ka2);

LINK_ENTITY_TO_GAME(weapon_hkmp5);

LINK_ENTITY_TO_GAME(weapon_holybsword);

LINK_ENTITY_TO_GAME(weapon_horar1);

LINK_ENTITY_TO_GAME(weapon_horar2);

LINK_ENTITY_TO_GAME(weapon_horg1);

LINK_ENTITY_TO_GAME(weapon_horg2);

LINK_ENTITY_TO_GAME(weapon_hormg1);

LINK_ENTITY_TO_GAME(weapon_hormg2);

LINK_ENTITY_TO_GAME(weapon_hornetgun);

LINK_ENTITY_TO_GAME(weapon_hyperblaster);

LINK_ENTITY_TO_GAME(weapon_icepokespell);

LINK_ENTITY_TO_GAME(weapon_impulse);

LINK_ENTITY_TO_GAME(weapon_incendiary);

LINK_ENTITY_TO_GAME(weapon_infector);

LINK_ENTITY_TO_GAME(weapon_invis);

LINK_ENTITY_TO_GAME(weapon_islavezap);

LINK_ENTITY_TO_GAME(weapon_ithaca);

LINK_ENTITY_TO_GAME(weapon_jackhammer);

LINK_ENTITY_TO_GAME(weapon_javelin);

LINK_ENTITY_TO_GAME(weapon_jetfist);

LINK_ENTITY_TO_GAME(weapon_jetpack);

LINK_ENTITY_TO_GAME(weapon_jumpjet);

LINK_ENTITY_TO_GAME(weapon_k43);

LINK_ENTITY_TO_GAME(weapon_kamakazi);

LINK_ENTITY_TO_GAME(weapon_kameha);

LINK_ENTITY_TO_GAME(weapon_kamehameha);

LINK_ENTITY_TO_GAME(weapon_kametorpedo);

LINK_ENTITY_TO_GAME(weapon_kamikaze);

LINK_ENTITY_TO_GAME(weapon_kar);

LINK_ENTITY_TO_GAME(weapon_katana);

LINK_ENTITY_TO_GAME(weapon_kawauso);

LINK_ENTITY_TO_GAME(weapon_kbar);

LINK_ENTITY_TO_GAME(weapon_kf7);

LINK_ENTITY_TO_GAME(weapon_kiblast);

LINK_ENTITY_TO_GAME(weapon_killsaw);

LINK_ENTITY_TO_GAME(weapon_kmedkit);

LINK_ENTITY_TO_GAME(weapon_knife);

LINK_ENTITY_TO_GAME(weapon_lacrymo);

LINK_ENTITY_TO_GAME(weapon_lance);

LINK_ENTITY_TO_GAME(weapon_lanceflamme);

LINK_ENTITY_TO_GAME(weapon_larve);

LINK_ENTITY_TO_GAME(weapon_laser);

LINK_ENTITY_TO_GAME(weapon_laser_rifle);

LINK_ENTITY_TO_GAME(weapon_lasergatling);

LINK_ENTITY_TO_GAME(weapon_launcher);

LINK_ENTITY_TO_GAME(weapon_law);

LINK_ENTITY_TO_GAME(weapon_lawgiver);

LINK_ENTITY_TO_GAME(weapon_lawrod);

LINK_ENTITY_TO_GAME(weapon_leap);

LINK_ENTITY_TO_GAME(weapon_levitationspell);

LINK_ENTITY_TO_GAME(weapon_lflamme);

LINK_ENTITY_TO_GAME(weapon_lgrenades);

LINK_ENTITY_TO_GAME(weapon_lightning);

LINK_ENTITY_TO_GAME(weapon_lightningboltspell);

LINK_ENTITY_TO_GAME(weapon_lightningcloudspell);

LINK_ENTITY_TO_GAME(weapon_lightsaber);

LINK_ENTITY_TO_GAME(weapon_lightsabre);

LINK_ENTITY_TO_GAME(weapon_lngun);

LINK_ENTITY_TO_GAME(weapon_longbow);

LINK_ENTITY_TO_GAME(weapon_longslide);

LINK_ENTITY_TO_GAME(weapon_longsword);

LINK_ENTITY_TO_GAME(weapon_lr300);

LINK_ENTITY_TO_GAME(weapon_lr300s);

LINK_ENTITY_TO_GAME(weapon_lrifle);

LINK_ENTITY_TO_GAME(weapon_luger);

LINK_ENTITY_TO_GAME(weapon_m11);

LINK_ENTITY_TO_GAME(weapon_m11sd);

LINK_ENTITY_TO_GAME(weapon_m134);

LINK_ENTITY_TO_GAME(weapon_m14);

LINK_ENTITY_TO_GAME(weapon_m16);

LINK_ENTITY_TO_GAME(weapon_m16a2);

LINK_ENTITY_TO_GAME(weapon_m1carbine);

LINK_ENTITY_TO_GAME(weapon_m2);

LINK_ENTITY_TO_GAME(weapon_m21);

LINK_ENTITY_TO_GAME(weapon_m249);

LINK_ENTITY_TO_GAME(weapon_m3);

LINK_ENTITY_TO_GAME(weapon_m4);

LINK_ENTITY_TO_GAME(weapon_m40a1);

LINK_ENTITY_TO_GAME(weapon_m41);

LINK_ENTITY_TO_GAME(weapon_m4a1);

LINK_ENTITY_TO_GAME(weapon_m60);

LINK_ENTITY_TO_GAME(weapon_m61frag);

LINK_ENTITY_TO_GAME(weapon_m67);

LINK_ENTITY_TO_GAME(weapon_m72);

LINK_ENTITY_TO_GAME(weapon_m76);

LINK_ENTITY_TO_GAME(weapon_m79);

LINK_ENTITY_TO_GAME(weapon_m82);

LINK_ENTITY_TO_GAME(weapon_m86);

LINK_ENTITY_TO_GAME(weapon_m92s);

LINK_ENTITY_TO_GAME(weapon_m96);

LINK_ENTITY_TO_GAME(weapon_m98);

LINK_ENTITY_TO_GAME(weapon_mac);

LINK_ENTITY_TO_GAME(weapon_mac10);

LINK_ENTITY_TO_GAME(weapon_mace);

LINK_ENTITY_TO_GAME(weapon_machete);

LINK_ENTITY_TO_GAME(weapon_machinegun);

LINK_ENTITY_TO_GAME(weapon_mag);

LINK_ENTITY_TO_GAME(weapon_magicmisslespell);

LINK_ENTITY_TO_GAME(weapon_main);

LINK_ENTITY_TO_GAME(weapon_markmusket);

LINK_ENTITY_TO_GAME(weapon_masenko);

LINK_ENTITY_TO_GAME(weapon_mat49);

LINK_ENTITY_TO_GAME(weapon_match);

LINK_ENTITY_TO_GAME(weapon_matrix);

LINK_ENTITY_TO_GAME(weapon_maverick);

LINK_ENTITY_TO_GAME(weapon_mc51);

LINK_ENTITY_TO_GAME(weapon_medic);

LINK_ENTITY_TO_GAME(weapon_medikit);

LINK_ENTITY_TO_GAME(weapon_medkit);

LINK_ENTITY_TO_GAME(weapon_melee);

LINK_ENTITY_TO_GAME(weapon_metabolize);

LINK_ENTITY_TO_GAME(weapon_meteorspell);

LINK_ENTITY_TO_GAME(weapon_mf2);

LINK_ENTITY_TO_GAME(weapon_mg34);

LINK_ENTITY_TO_GAME(weapon_mg36);

LINK_ENTITY_TO_GAME(weapon_mg42);

LINK_ENTITY_TO_GAME(weapon_microuzi);

LINK_ENTITY_TO_GAME(weapon_microuzi_a);

LINK_ENTITY_TO_GAME(weapon_mindmisslespell);

LINK_ENTITY_TO_GAME(weapon_mindray);

LINK_ENTITY_TO_GAME(weapon_mine);

LINK_ENTITY_TO_GAME(weapon_minicanon);

LINK_ENTITY_TO_GAME(weapon_minigun);

LINK_ENTITY_TO_GAME(weapon_mk23);

LINK_ENTITY_TO_GAME(weapon_model98);

LINK_ENTITY_TO_GAME(weapon_molotov);

LINK_ENTITY_TO_GAME(weapon_molotov_cocktail);

LINK_ENTITY_TO_GAME(weapon_mop);

LINK_ENTITY_TO_GAME(weapon_mortar);

LINK_ENTITY_TO_GAME(weapon_mortier);

LINK_ENTITY_TO_GAME(weapon_morve);

LINK_ENTITY_TO_GAME(weapon_mosin);

LINK_ENTITY_TO_GAME(weapon_mossberg);

LINK_ENTITY_TO_GAME(weapon_mouthblast);

LINK_ENTITY_TO_GAME(weapon_mouton);

LINK_ENTITY_TO_GAME(weapon_mp40);

LINK_ENTITY_TO_GAME(weapon_mp44);

LINK_ENTITY_TO_GAME(weapon_mp5);

LINK_ENTITY_TO_GAME(weapon_mp510);

LINK_ENTITY_TO_GAME(weapon_mp5a2);

LINK_ENTITY_TO_GAME(weapon_mp5a4);

LINK_ENTITY_TO_GAME(weapon_mp5a5);

LINK_ENTITY_TO_GAME(weapon_mp5k);

LINK_ENTITY_TO_GAME(weapon_mp5ktac);

LINK_ENTITY_TO_GAME(weapon_mp5navy);

LINK_ENTITY_TO_GAME(weapon_mp5pdw);

LINK_ENTITY_TO_GAME(weapon_mp5sd);

LINK_ENTITY_TO_GAME(weapon_mp5sd5);

LINK_ENTITY_TO_GAME(weapon_msg90);

LINK_ENTITY_TO_GAME(weapon_musket);

LINK_ENTITY_TO_GAME(weapon_musketoon);

LINK_ENTITY_TO_GAME(weapon_mustardgrenade);

LINK_ENTITY_TO_GAME(weapon_mwgun);

LINK_ENTITY_TO_GAME(weapon_nailgun);

LINK_ENTITY_TO_GAME(weapon_napalm);

LINK_ENTITY_TO_GAME(weapon_narcogrenade);

LINK_ENTITY_TO_GAME(weapon_ndagger);

LINK_ENTITY_TO_GAME(weapon_needle);

LINK_ENTITY_TO_GAME(weapon_nervegrenade);

LINK_ENTITY_TO_GAME(weapon_ngun);

LINK_ENTITY_TO_GAME(weapon_nightstick);

LINK_ENTITY_TO_GAME(weapon_not_in_use_grenade);

LINK_ENTITY_TO_GAME(weapon_nukegun);

LINK_ENTITY_TO_GAME(weapon_nuker);

LINK_ENTITY_TO_GAME(weapon_null);

LINK_ENTITY_TO_GAME(weapon_nva_grenade);

LINK_ENTITY_TO_GAME(weapon_oicw);

LINK_ENTITY_TO_GAME(weapon_omen);

LINK_ENTITY_TO_GAME(weapon_orbital);

LINK_ENTITY_TO_GAME(weapon_orracle);

LINK_ENTITY_TO_GAME(weapon_p225);

LINK_ENTITY_TO_GAME(weapon_p226);

LINK_ENTITY_TO_GAME(weapon_p226mp);

LINK_ENTITY_TO_GAME(weapon_p228);

LINK_ENTITY_TO_GAME(weapon_p90);

LINK_ENTITY_TO_GAME(weapon_p99);

LINK_ENTITY_TO_GAME(weapon_paintball);

LINK_ENTITY_TO_GAME(weapon_paintgrenade);

LINK_ENTITY_TO_GAME(weapon_paralysis);

LINK_ENTITY_TO_GAME(weapon_parasite);

LINK_ENTITY_TO_GAME(weapon_parrot);

LINK_ENTITY_TO_GAME(weapon_pdw);

LINK_ENTITY_TO_GAME(weapon_penguin);

LINK_ENTITY_TO_GAME(weapon_pgp);

LINK_ENTITY_TO_GAME(weapon_phantom);

LINK_ENTITY_TO_GAME(weapon_phoenix);

LINK_ENTITY_TO_GAME(weapon_photongun);

LINK_ENTITY_TO_GAME(weapon_piat);

LINK_ENTITY_TO_GAME(weapon_pick);

LINK_ENTITY_TO_GAME(weapon_pigeon);

LINK_ENTITY_TO_GAME(weapon_pipebomb);

LINK_ENTITY_TO_GAME(weapon_pipewrench);

LINK_ENTITY_TO_GAME(weapon_pistol);

LINK_ENTITY_TO_GAME(weapon_pistolet);

LINK_ENTITY_TO_GAME(weapon_pkm);

LINK_ENTITY_TO_GAME(weapon_plasma);

LINK_ENTITY_TO_GAME(weapon_plasmarifle);

LINK_ENTITY_TO_GAME(weapon_plauncher);

LINK_ENTITY_TO_GAME(weapon_pmine);

LINK_ENTITY_TO_GAME(weapon_poison);

LINK_ENTITY_TO_GAME(weapon_poolstick);

LINK_ENTITY_TO_GAME(weapon_powderkeg);

LINK_ENTITY_TO_GAME(weapon_powerbeam);

LINK_ENTITY_TO_GAME(weapon_powerblade);

LINK_ENTITY_TO_GAME(weapon_prifle);

LINK_ENTITY_TO_GAME(weapon_primalscream);

LINK_ENTITY_TO_GAME(weapon_proximity);

LINK_ENTITY_TO_GAME(weapon_pschreck);

LINK_ENTITY_TO_GAME(weapon_psg);

LINK_ENTITY_TO_GAME(weapon_psg1);

LINK_ENTITY_TO_GAME(weapon_pt);

LINK_ENTITY_TO_GAME(weapon_pulserifle);

LINK_ENTITY_TO_GAME(weapon_pushhands);

LINK_ENTITY_TO_GAME(weapon_python);

LINK_ENTITY_TO_GAME(weapon_quakegun);

LINK_ENTITY_TO_GAME(weapon_radio);

LINK_ENTITY_TO_GAME(weapon_railgun);

LINK_ENTITY_TO_GAME(weapon_ralkek);

LINK_ENTITY_TO_GAME(weapon_rallyhp);

LINK_ENTITY_TO_GAME(weapon_rbull);

LINK_ENTITY_TO_GAME(weapon_rcp90);

LINK_ENTITY_TO_GAME(weapon_redeemer);

LINK_ENTITY_TO_GAME(weapon_reetou);

LINK_ENTITY_TO_GAME(weapon_regeneration);

LINK_ENTITY_TO_GAME(weapon_rem1100);

LINK_ENTITY_TO_GAME(weapon_remington);

LINK_ENTITY_TO_GAME(weapon_renzoku);

LINK_ENTITY_TO_GAME(weapon_resource);

LINK_ENTITY_TO_GAME(weapon_rgd5);

LINK_ENTITY_TO_GAME(weapon_rifle);

LINK_ENTITY_TO_GAME(weapon_rmine);

LINK_ENTITY_TO_GAME(weapon_rocketlauncher);

LINK_ENTITY_TO_GAME(weapon_rocketpistol);

LINK_ENTITY_TO_GAME(weapon_rollingstonespell);

LINK_ENTITY_TO_GAME(weapon_rpd);

LINK_ENTITY_TO_GAME(weapon_rpg);

LINK_ENTITY_TO_GAME(weapon_rpg7);

LINK_ENTITY_TO_GAME(weapon_rs202m2);

LINK_ENTITY_TO_GAME(weapon_rs_meta);

LINK_ENTITY_TO_GAME(weapon_ruche);

LINK_ENTITY_TO_GAME(weapon_ruger);

LINK_ENTITY_TO_GAME(weapon_rumgun);

LINK_ENTITY_TO_GAME(weapon_runner);

LINK_ENTITY_TO_GAME(weapon_sa80);

LINK_ENTITY_TO_GAME(weapon_saa);

LINK_ENTITY_TO_GAME(weapon_saber);

LINK_ENTITY_TO_GAME(weapon_sabers);

LINK_ENTITY_TO_GAME(weapon_saiga);

LINK_ENTITY_TO_GAME(weapon_sainte);

LINK_ENTITY_TO_GAME(weapon_sako);

LINK_ENTITY_TO_GAME(weapon_satchel);

LINK_ENTITY_TO_GAME(weapon_satelitelaser);

LINK_ENTITY_TO_GAME(weapon_satellite);

LINK_ENTITY_TO_GAME(weapon_sawed);

LINK_ENTITY_TO_GAME(weapon_sawedoff);

LINK_ENTITY_TO_GAME(weapon_sawgun);

LINK_ENTITY_TO_GAME(weapon_sbarrel);

LINK_ENTITY_TO_GAME(weapon_scatterbeam);

LINK_ENTITY_TO_GAME(weapon_scattergun);

LINK_ENTITY_TO_GAME(weapon_scattershot);

LINK_ENTITY_TO_GAME(weapon_scopedenfield);

LINK_ENTITY_TO_GAME(weapon_scopedfg42);

LINK_ENTITY_TO_GAME(weapon_scopedkar);

LINK_ENTITY_TO_GAME(weapon_scorpion);

LINK_ENTITY_TO_GAME(weapon_scout);

LINK_ENTITY_TO_GAME(weapon_seburo);

LINK_ENTITY_TO_GAME(weapon_sensu);

LINK_ENTITY_TO_GAME(weapon_seringue);

LINK_ENTITY_TO_GAME(weapon_sg550);

LINK_ENTITY_TO_GAME(weapon_sg552);

LINK_ENTITY_TO_GAME(weapon_sgenerator);

LINK_ENTITY_TO_GAME(weapon_shield);

LINK_ENTITY_TO_GAME(weapon_shieldattack);

LINK_ENTITY_TO_GAME(weapon_shieldgun);

LINK_ENTITY_TO_GAME(weapon_shieldspell);

LINK_ENTITY_TO_GAME(weapon_shocker);

LINK_ENTITY_TO_GAME(weapon_shocker2k);

LINK_ENTITY_TO_GAME(weapon_shockrifle);

LINK_ENTITY_TO_GAME(weapon_shockroach);

LINK_ENTITY_TO_GAME(weapon_shotgun);

LINK_ENTITY_TO_GAME(weapon_shrinker);

LINK_ENTITY_TO_GAME(weapon_sig);

LINK_ENTITY_TO_GAME(weapon_sig245);

LINK_ENTITY_TO_GAME(weapon_sig550);

LINK_ENTITY_TO_GAME(weapon_sigpro);

LINK_ENTITY_TO_GAME(weapon_silverpp7);

LINK_ENTITY_TO_GAME(weapon_sixshooter);

LINK_ENTITY_TO_GAME(weapon_skeletonspell);

LINK_ENTITY_TO_GAME(weapon_sks);

LINK_ENTITY_TO_GAME(weapon_skullspell);

LINK_ENTITY_TO_GAME(weapon_skunk);

LINK_ENTITY_TO_GAME(weapon_sl68);

LINK_ENTITY_TO_GAME(weapon_slappers);

LINK_ENTITY_TO_GAME(weapon_sledge);

LINK_ENTITY_TO_GAME(weapon_sledgehammer);

LINK_ENTITY_TO_GAME(weapon_sm_assaultcannon);

LINK_ENTITY_TO_GAME(weapon_sm_bolter);

LINK_ENTITY_TO_GAME(weapon_sm_boltpistol);

LINK_ENTITY_TO_GAME(weapon_sm_combatknife);

LINK_ENTITY_TO_GAME(weapon_sm_flamer);

LINK_ENTITY_TO_GAME(weapon_sm_fraggrenade);

LINK_ENTITY_TO_GAME(weapon_sm_heavybolter);

LINK_ENTITY_TO_GAME(weapon_sm_heavyflamer);

LINK_ENTITY_TO_GAME(weapon_sm_lascannon);

LINK_ENTITY_TO_GAME(weapon_sm_missilelauncher);

LINK_ENTITY_TO_GAME(weapon_sm_poweraxe);

LINK_ENTITY_TO_GAME(weapon_sm_powersword);

LINK_ENTITY_TO_GAME(weapon_sm_stormbolter);

LINK_ENTITY_TO_GAME(weapon_smartgun);

LINK_ENTITY_TO_GAME(weapon_smaw);

LINK_ENTITY_TO_GAME(weapon_smg9);

LINK_ENTITY_TO_GAME(weapon_smkgrenade);

LINK_ENTITY_TO_GAME(weapon_smoke);

LINK_ENTITY_TO_GAME(weapon_smokegrenade);

LINK_ENTITY_TO_GAME(weapon_snUZI);

LINK_ENTITY_TO_GAME(weapon_snark);

LINK_ENTITY_TO_GAME(weapon_snipe);

LINK_ENTITY_TO_GAME(weapon_sniper);

LINK_ENTITY_TO_GAME(weapon_sniper3);

LINK_ENTITY_TO_GAME(weapon_sniperrifle);

LINK_ENTITY_TO_GAME(weapon_snowball);

LINK_ENTITY_TO_GAME(weapon_socom);

LINK_ENTITY_TO_GAME(weapon_socomtac);

LINK_ENTITY_TO_GAME(weapon_solarflare);

LINK_ENTITY_TO_GAME(weapon_soniccannon);

LINK_ENTITY_TO_GAME(weapon_sonicgrenade);

LINK_ENTITY_TO_GAME(weapon_spade);

LINK_ENTITY_TO_GAME(weapon_spas);

LINK_ENTITY_TO_GAME(weapon_spas12);

LINK_ENTITY_TO_GAME(weapon_spear);

LINK_ENTITY_TO_GAME(weapon_specialbeamcannon);

LINK_ENTITY_TO_GAME(weapon_spectre);

LINK_ENTITY_TO_GAME(weapon_spider);

LINK_ENTITY_TO_GAME(weapon_spiderbomb);

LINK_ENTITY_TO_GAME(weapon_spikegun);

LINK_ENTITY_TO_GAME(weapon_spiritbomb);

LINK_ENTITY_TO_GAME(weapon_spiritwizspell);

LINK_ENTITY_TO_GAME(weapon_spit);

LINK_ENTITY_TO_GAME(weapon_spitcarbine);

LINK_ENTITY_TO_GAME(weapon_spitpistol);

LINK_ENTITY_TO_GAME(weapon_spore);

LINK_ENTITY_TO_GAME(weapon_sporelauncher);

LINK_ENTITY_TO_GAME(weapon_spotboltspell);

LINK_ENTITY_TO_GAME(weapon_spring);

LINK_ENTITY_TO_GAME(weapon_spy);

LINK_ENTITY_TO_GAME(weapon_spycam);

LINK_ENTITY_TO_GAME(weapon_spyder);

LINK_ENTITY_TO_GAME(weapon_srifle);

LINK_ENTITY_TO_GAME(weapon_ssg3000);

LINK_ENTITY_TO_GAME(weapon_sshotgun);

LINK_ENTITY_TO_GAME(weapon_st_far);

LINK_ENTITY_TO_GAME(weapon_st_heavy);

LINK_ENTITY_TO_GAME(weapon_st_medium);

LINK_ENTITY_TO_GAME(weapon_st_melee);

LINK_ENTITY_TO_GAME(weapon_st_power);

LINK_ENTITY_TO_GAME(weapon_st_psi);

LINK_ENTITY_TO_GAME(weapon_st_short);

LINK_ENTITY_TO_GAME(weapon_st_special);

LINK_ENTITY_TO_GAME(weapon_st_throw);

LINK_ENTITY_TO_GAME(weapon_staff);

LINK_ENTITY_TO_GAME(weapon_star);

LINK_ENTITY_TO_GAME(weapon_sten);

LINK_ENTITY_TO_GAME(weapon_sterling);

LINK_ENTITY_TO_GAME(weapon_steyr_m40);

LINK_ENTITY_TO_GAME(weapon_stg24);

LINK_ENTITY_TO_GAME(weapon_stickgrenade);

LINK_ENTITY_TO_GAME(weapon_stickgrenade_ex);

LINK_ENTITY_TO_GAME(weapon_stinger);

LINK_ENTITY_TO_GAME(weapon_stingray2);

LINK_ENTITY_TO_GAME(weapon_stomp);

LINK_ENTITY_TO_GAME(weapon_stoner);

LINK_ENTITY_TO_GAME(weapon_stonerscope);

LINK_ENTITY_TO_GAME(weapon_stonersilencer);

LINK_ENTITY_TO_GAME(weapon_stubgun);

LINK_ENTITY_TO_GAME(weapon_stumpgun);

LINK_ENTITY_TO_GAME(weapon_stupid);

LINK_ENTITY_TO_GAME(weapon_super_mouton);

LINK_ENTITY_TO_GAME(weapon_supernailgun);

LINK_ENTITY_TO_GAME(weapon_supershotgun);

LINK_ENTITY_TO_GAME(weapon_svd);

LINK_ENTITY_TO_GAME(weapon_swarm_gargflame);

LINK_ENTITY_TO_GAME(weapon_swipe);

LINK_ENTITY_TO_GAME(weapon_sword);

LINK_ENTITY_TO_GAME(weapon_syringe);

LINK_ENTITY_TO_GAME(weapon_tacgun);

LINK_ENTITY_TO_GAME(weapon_taucannon);

LINK_ENTITY_TO_GAME(weapon_tavor);

LINK_ENTITY_TO_GAME(weapon_tdagger);

LINK_ENTITY_TO_GAME(weapon_tec9);

LINK_ENTITY_TO_GAME(weapon_telekinesis);

LINK_ENTITY_TO_GAME(weapon_teleporter);

LINK_ENTITY_TO_GAME(weapon_teleporteur);

LINK_ENTITY_TO_GAME(weapon_teslagun);

LINK_ENTITY_TO_GAME(weapon_thompson);

LINK_ENTITY_TO_GAME(weapon_thornblastspell);

LINK_ENTITY_TO_GAME(weapon_throwaxe);

LINK_ENTITY_TO_GAME(weapon_throwing_knife);

LINK_ENTITY_TO_GAME(weapon_throwingknife);

LINK_ENTITY_TO_GAME(weapon_timmy);

LINK_ENTITY_TO_GAME(weapon_tknife);

LINK_ENTITY_TO_GAME(weapon_tmine);

LINK_ENTITY_TO_GAME(weapon_tmp);

LINK_ENTITY_TO_GAME(weapon_tnt);

LINK_ENTITY_TO_GAME(weapon_toad);

LINK_ENTITY_TO_GAME(weapon_tokarev);

LINK_ENTITY_TO_GAME(weapon_tomahawk);

LINK_ENTITY_TO_GAME(weapon_tommy);

LINK_ENTITY_TO_GAME(weapon_tommygun);

LINK_ENTITY_TO_GAME(weapon_tornadospell);

LINK_ENTITY_TO_GAME(weapon_transistor);

LINK_ENTITY_TO_GAME(weapon_translocator);

LINK_ENTITY_TO_GAME(weapon_treasure);

LINK_ENTITY_TO_GAME(weapon_trident);

LINK_ENTITY_TO_GAME(weapon_trigun);

LINK_ENTITY_TO_GAME(weapon_triplaser);

LINK_ENTITY_TO_GAME(weapon_tripmine);

LINK_ENTITY_TO_GAME(weapon_trooper);

LINK_ENTITY_TO_GAME(weapon_tsgun);

LINK_ENTITY_TO_GAME(weapon_tucan);

LINK_ENTITY_TO_GAME(weapon_turret);

LINK_ENTITY_TO_GAME(weapon_twosword);

LINK_ENTITY_TO_GAME(weapon_u2);

LINK_ENTITY_TO_GAME(weapon_umbra);

LINK_ENTITY_TO_GAME(weapon_umbrella);

LINK_ENTITY_TO_GAME(weapon_ump45);

LINK_ENTITY_TO_GAME(weapon_updraftspell);

LINK_ENTITY_TO_GAME(weapon_us_grenade);

LINK_ENTITY_TO_GAME(weapon_usas);

LINK_ENTITY_TO_GAME(weapon_usas12);

LINK_ENTITY_TO_GAME(weapon_usp);

LINK_ENTITY_TO_GAME(weapon_uspmp);

LINK_ENTITY_TO_GAME(weapon_uzi);

LINK_ENTITY_TO_GAME(weapon_uziakimbo);

LINK_ENTITY_TO_GAME(weapon_vepr);

LINK_ENTITY_TO_GAME(weapon_viksword);

LINK_ENTITY_TO_GAME(weapon_vomit);

LINK_ENTITY_TO_GAME(weapon_vsbike);

LINK_ENTITY_TO_GAME(weapon_vsclaw);

LINK_ENTITY_TO_GAME(weapon_vscolt);

LINK_ENTITY_TO_GAME(weapon_vscrossbow);

LINK_ENTITY_TO_GAME(weapon_vscue);

LINK_ENTITY_TO_GAME(weapon_vsdbshotgun);

LINK_ENTITY_TO_GAME(weapon_vsmp5);

LINK_ENTITY_TO_GAME(weapon_vsshotgun);

LINK_ENTITY_TO_GAME(weapon_vsstake);

LINK_ENTITY_TO_GAME(weapon_vsthunderfive);

LINK_ENTITY_TO_GAME(weapon_vswinchester);

LINK_ENTITY_TO_GAME(weapon_vulcan);

LINK_ENTITY_TO_GAME(weapon_wa2000);

LINK_ENTITY_TO_GAME(weapon_warhammer);

LINK_ENTITY_TO_GAME(weapon_webley);

LINK_ENTITY_TO_GAME(weapon_webspinner);

LINK_ENTITY_TO_GAME(weapon_welder);

LINK_ENTITY_TO_GAME(weapon_whirlwindspell);

LINK_ENTITY_TO_GAME(weapon_whiskey);

LINK_ENTITY_TO_GAME(weapon_whiterayspell);

LINK_ENTITY_TO_GAME(weapon_whl);

LINK_ENTITY_TO_GAME(weapon_winchester);

LINK_ENTITY_TO_GAME(weapon_wombatspell);

LINK_ENTITY_TO_GAME(weapon_wyvernspell);

LINK_ENTITY_TO_GAME(weapon_xbow);

LINK_ENTITY_TO_GAME(weapon_xm1014);

LINK_ENTITY_TO_GAME(weapon_xm4);

LINK_ENTITY_TO_GAME(weapon_zat);

LINK_ENTITY_TO_GAME(weapon_zatarc);

LINK_ENTITY_TO_GAME(weapon_zmg);

LINK_ENTITY_TO_GAME(weaponbox);

LINK_ENTITY_TO_GAME(weather_genie);

LINK_ENTITY_TO_GAME(weather_litnode);

LINK_ENTITY_TO_GAME(webgunprojectile);

LINK_ENTITY_TO_GAME(wh);

LINK_ENTITY_TO_GAME(wheel);

LINK_ENTITY_TO_GAME(wheel_ent);

LINK_ENTITY_TO_GAME(whiskey_whisk);

LINK_ENTITY_TO_GAME(whshard);

LINK_ENTITY_TO_GAME(world_areacontrol);

LINK_ENTITY_TO_GAME(world_artefact);

LINK_ENTITY_TO_GAME(world_capturelocation);

LINK_ENTITY_TO_GAME(world_items);

LINK_ENTITY_TO_GAME(worldspawn);

LINK_ENTITY_TO_GAME(ww_ammo_satchels);

LINK_ENTITY_TO_GAME(ww_ammo_spellbook);

LINK_ENTITY_TO_GAME(ww_bearbite);

LINK_ENTITY_TO_GAME(ww_emitter);

LINK_ENTITY_TO_GAME(ww_monstermaker);

LINK_ENTITY_TO_GAME(ww_satchel);

LINK_ENTITY_TO_GAME(ww_satchel_archmage);

LINK_ENTITY_TO_GAME(ww_satchel_death);

LINK_ENTITY_TO_GAME(ww_satchel_dragon);

LINK_ENTITY_TO_GAME(ww_satchel_earth);

LINK_ENTITY_TO_GAME(ww_satchel_fire);

LINK_ENTITY_TO_GAME(ww_satchel_ice);

LINK_ENTITY_TO_GAME(ww_satchel_life);

LINK_ENTITY_TO_GAME(ww_satchel_lightning);

LINK_ENTITY_TO_GAME(ww_satchel_nature);

LINK_ENTITY_TO_GAME(ww_satchel_wind);

LINK_ENTITY_TO_GAME(ww_seal);

LINK_ENTITY_TO_GAME(ww_seal_death);

LINK_ENTITY_TO_GAME(ww_seal_dragon);

LINK_ENTITY_TO_GAME(ww_seal_earth);

LINK_ENTITY_TO_GAME(ww_seal_fire);

LINK_ENTITY_TO_GAME(ww_seal_ice);

LINK_ENTITY_TO_GAME(ww_seal_life);

LINK_ENTITY_TO_GAME(ww_seal_lightning);

LINK_ENTITY_TO_GAME(ww_seal_nature);

LINK_ENTITY_TO_GAME(ww_seal_wind);

LINK_ENTITY_TO_GAME(ww_sentrycrystal);

LINK_ENTITY_TO_GAME(ww_spell_beanstalk);

LINK_ENTITY_TO_GAME(ww_spell_bird);

LINK_ENTITY_TO_GAME(ww_spell_deathray);

LINK_ENTITY_TO_GAME(ww_spell_doublemissile);

LINK_ENTITY_TO_GAME(ww_spell_dragonbreath);

LINK_ENTITY_TO_GAME(ww_spell_fireball);

LINK_ENTITY_TO_GAME(ww_spell_flamelick);

LINK_ENTITY_TO_GAME(ww_spell_forceblast);

LINK_ENTITY_TO_GAME(ww_spell_freezeray);

LINK_ENTITY_TO_GAME(ww_spell_iceshard);

LINK_ENTITY_TO_GAME(ww_spell_levitate);

LINK_ENTITY_TO_GAME(ww_spell_lightningbolt);

LINK_ENTITY_TO_GAME(ww_spell_missile);

LINK_ENTITY_TO_GAME(ww_spell_pebbleblast);

LINK_ENTITY_TO_GAME(ww_spell_seal);

LINK_ENTITY_TO_GAME(ww_spell_shield);

LINK_ENTITY_TO_GAME(ww_spell_skull);

LINK_ENTITY_TO_GAME(ww_spell_spotbolt);

LINK_ENTITY_TO_GAME(ww_spell_stone);

LINK_ENTITY_TO_GAME(ww_spell_thornblast);

LINK_ENTITY_TO_GAME(ww_spell_updraft);

LINK_ENTITY_TO_GAME(ww_spell_whiteray);

LINK_ENTITY_TO_GAME(ww_spell_wyvern);

LINK_ENTITY_TO_GAME(ww_staff);

LINK_ENTITY_TO_GAME(wyvern);

LINK_ENTITY_TO_GAME(xbow_teather);

LINK_ENTITY_TO_GAME(xbowbolt);

LINK_ENTITY_TO_GAME(xen_hair);

LINK_ENTITY_TO_GAME(xen_hull);

LINK_ENTITY_TO_GAME(xen_plantlight);

LINK_ENTITY_TO_GAME(xen_spore_large);

LINK_ENTITY_TO_GAME(xen_spore_medium);

LINK_ENTITY_TO_GAME(xen_spore_small);

LINK_ENTITY_TO_GAME(xen_tree);

LINK_ENTITY_TO_GAME(xen_ttrigger);

LINK_ENTITY_TO_GAME(zatarc_proj);

LINK_ENTITY_TO_GAME(zone);

LINK_ENTITY_TO_GAME(zone_bomb_target);

LINK_ENTITY_TO_GAME(zone_capture);

LINK_ENTITY_TO_GAME(zone_gatetravel);

LINK_ENTITY_TO_GAME(zone_nodamage);

LINK_ENTITY_TO_GAME(zone_shelter);

*/