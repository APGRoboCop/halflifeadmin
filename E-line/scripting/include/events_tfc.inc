//This include file contains template event
//handler functions for all team fortress classic 1.5
//events
//
//NOTE 1: Where an event argument is prefixed with "HL" it
//        is a string and you need to use convert_string to
//        access it.
//
//NOTE 2: The comment describing each event contains the line you
//        need to put in plug-init to trap it.
//



/* Events Common to TFC and Counter-Strike */

//A player chooses a new team
//
//plugin_registerevt("player_team","EvtPlayerTeam");
//
forward EvtPlayerTeam(Team,HLUserName,Automatic);

//A player spawns into the game. The first spawn after connecting
//is false - this is the player entering the game world and seeing
//the motd.
//
//plugin_registerevt("player_spawn","EvtPlayerSpawn");
//
forward EvtPlayerSpawn(UserIndex);


//Players health changes for any reason. This includes reseting the
//health on spawning.
//
//plugin_registerevt("player_health","EvtPlayerHealth");
//
forward EvtPlayerHealth(UserIndex,Health);

//Player dies. HLReason is a string containing the weapon / cause of
//dead. NOTE: Headshot detection is flakey at the moment.
//
//plugin_registerevt("player_death","EvtPlayerDeath");
//
forward EvtPlayerDeath(UserIndex,HLReason,KillerIndex,Headshot);

//Player score has changed.
//
//plugin_registerevt("player_score","EvtPlayerScore");
//
forward EvtPlayerScore(UserIndex,Kills,Deaths) ;

//Player has been damaged by an attacker. Amount is the amount of
//damage inflicted, which may be more than the players health
//
//NOTE: AttackerIndex is unreliable at the moment.
//
//plugin_registerevt("player_damage","EvtPlayerDamage");
//
public EvtPlayerDamage(UserIndex,AttackerIndex,Amount) {

//A player has picked up / spawned with a weapon. Weapons
//are identified by number not name.
//
//plugin_registerevt("weapon_pickup","EvtWeaponPickup");
//
forward EvtWeaponPickup(UserIndex,Weapon);

//A player has made a weapon active. For some weapons this
//event also occurs when the weapon is fired (e.g. shotgun)
//
//plugin_registerevt("weapon_change","EvtWeaponChange");
//
forward EvtWeaponChange(UserIndex,Weapon,Ammo);

//A player is putting a weapon away for later use
//
//plugin_registerevt("weapon_stored","EvtWeaponStored");
//
forward EvtWeaponStored(UserIndex,Weapon,Ammo);

//A player has picked up some ammunition / grenades.
//ammo is identified by numeric type.
//
//plugin_registerevt("ammo_pickup","EvtAmmoPickup");
//
forward EvtAmmoPickup(UserIndex,Type,Quantity);


/* TFC Only Events */

//A player has fired a weapon or spawned with a ammunition for a weapon
//or picked up ammo
//
//plugin_registerevt("ammo_change","EvtAmmoChange");
//
forward EvtAmmoChange(UserIndex,Weapon,Ammo);

//A player has picked up a flag.
//
//plugin_registerevt("flag_pickup","EvtFlagPickup");
//
forward EvtFlagPickup(UserIndex,Team);

//A player has dropped or captured a flag (If captured
//a TeamScore event will also occur).
//
//plugin_registerevt("flag_dropped","EvtFlagDropped");
//
forward EvtFlagDropped(UserIndex,Team);

//Player has pressed the grenade key. Type is a string
//containing the grenade type
//
//plugin_registerevt("grenade_held","EvtGrenadeHeld");
//
forward EvtGrenadeHeld(UserIndex,HLType);

//Player has thrown the grenade. Type is a string
//containing the grenade type
//
//plugin_registerevt("grenade_thrown","EvtGrenadeThrown");
//
forward EvtGrenadeThrown(UserIndex,HLType);




//Demoman is planting a detpack
//
//plugin_registerevt("detpack_planting","EvtDetpackPlanting");
//
forward EvtDetpackPlanting(UserIndex,Duration);

//Demoman stopped planting a detpack
//
//plugin_registerevt("detpack_aborted","EvtDetpackAborted");
//
forward EvtDetpackAborted(UserIndex);

//Demoman planted
//
//plugin_registerevt("detpack_planted","EvtDetpackPlanted");
//
forward EvtDetpackPlanted(UserIndex);

//detpack has exploded
//
//plugin_registerevt("detpack_exploded","EvtDetpackExploded");
//
forward EvtDetpackExploded(UserIndex);






//Engineer has tried to start building something but wasn't allowed to
//
//plugin_registerevt("build_illegal","EvtBuildIllegal");
//
forward EvtBuildIllegal(UserIndex);

//Engineer has started building something
//
//plugin_registerevt("build_start","EvtBuildStart");
//
forward EvtBuildStart(UserIndex);

//Engineer has aborted building something
//
//plugin_registerevt("build_aborted","EvtBuildAborted");
//
forward EvtBuildAborted(UserIndex);

//Engineer has completed building some thing. HLType is
//a string containing "sentry" or "dispenser"
//
//plugin_registerevt("build_complete","EvtBuildComplete");
//
forward EvtBuildComplete(UserIndex,HLType);

//Engineer has dismantled something
//
//plugin_registerevt("build_dismantle","EvtBuildDismantle");
//
forward EvtBuildDismantle(UserIndex,HLType);

//Someone has destoryed / detonated something an engineer built
//
//plugin_registerevt("build_destroy","EvtBuildDestroy");
//
forward EvtBuildDestroy(UserIndex,HLType);

//An engineer has upgraded a sentry gun
//
//plugin_registerevt("build_upgrade","EvtBuildUpgrade");
//
forward EvtBuildUpgrade(UserIndex,HLType,Level);





//Team score has changed.
//
//plugin_registerevt("team_score","EvtTeamScore");
//
forward EvtTeamScore(Team,Score,Deaths);



