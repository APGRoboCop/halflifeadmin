//This include file contains template event
//handler functions for all counter-strike 1.5
//events
//
//NOTE 1: Where an event argument is prefixed with "HL" it
//        is a string and you need to use convert_string to
//        access it.
//
//NOTE 2: The comment describing each event contains the line you
//        need to put in plug-init to trap it.
//




////////////////////////////////////////////////////////////////////////////////////////
// General Game Events

//"Game Commencing" message displayed
//
//plugin_registerevt("game_start","EvtGameStart");
//
forward EvtGameStart();

//A round has started. Note this event occurs at the point the cameras switch back to the
//spawn point:
// - The players begin spawning immediately but sometimes take a second or two
// - The players won't be able to move for another mp_freezetime seconds.
// - The round will end in mp_roundtime*60 + mp_freezetime seconds
//
//NOTE: Sometimes the first player spawn event occurs before the round start
//
//plugin_registerevt("round_start","EvtRoundStart");
//
forward EvtRoundStart();

//A round has ended. This will be followed by a goal-specific round end event
//giving the reason unless the reason is all of one team died.
//
//plugin_registerevt("round_end","EvtRoundEnd");
//
forward EvtRoundEnd(WinningTeam);

//Player has joined a team on connect or they have swapped teams
//Automatic is 1 if the player was swapped teams by the game to balance
//them. It is not 1 for choosing autoselect when connecting/swapping.
//
//plugin_registerevt("player_team","EvtPlayerTeam");
//
forward EvtPlayerTeam(Team,HLUserName,Automatic);

//Player spawns after joining and choosing a team or after a round start
//
//Players can spawn dead if the are too slow changing team, join
//the spectator team or join late in the round. Use playerinfo()
//to determine if they have actuallys spawned.
//
//Players can spawn twice on joining - once when they first enter the
//game world - this will be a dead spawn. Then if they joined at a time
//where they can enter the game immediately they will spawn again alive.
//
//
//Players on the spectator team do not spawn
//
//plugin_registerevt("player_spawn","EvtPlayerSpawn");
//
forward EvtPlayerSpawn(UserIndex);

//Player's Health changed due to respawn or damage
//
//plugin_registerevt("player_health","EvtPlayerHealth");
//
forward EvtPlayerHealth(UserIndex,Health);

//Player Died. For suicide, KillerIndex == Victim index and HLWeapon contains
//"world" or "worldspawn"
//
//plugin_registerevt("player_death","EvtPlayerDeath");
//
forward EvtPlayerDeath(UserIndex,HLReason,KillerIndex,Headshot);

//Player score changed
//
//plugin_registerevt("player_score","EvtPlayerScore");
//
forward EvtPlayerScore(UserIndex,Kills,Deaths);






////////////////////////////////////////////////////////////////////////////////////////
// Hostage / Rescue Events


//Hostage spawned
//
//plugin_registerevt("hostage_spawned","EvtHostageSpawned");
//
forward EvtHostageSpawned(HostageIndex,X,Y,Z);

//Hostage moved due to being used or pushed. Also fires once after spawn
//
//plugin_registerevt("hostage_moved","EvtHostageMoved");
//
forward EvtHostageMoved(HostageIndex,X,Y,Z);

//Hostage removed from game due to rescue or killed by player.
//
//Killed = 1 for a kill, 0 for a rescue
//
//plugin_registerevt("hostage_removed","EvtHostageRemoved");
//
forward EvtHostageRemoved(UserIndex,HostageIndex,Killed);

//Player from either team entered/left a rescue zone (due to movement or death/respawn)
//Enabled is 1 on entering the zone, and 0 on leaving it
//
//plugin_registerevt("rescue_zone","EvtRescueZone");
//
forward EvtRescueZone(UserIndex,Enabled);


//CT Win - All unkilled hostages have been rescued - occurs after round end
//Returns the index of the last player to rescue a hostage
//Look at the previous hostage removed events to determine which player's rescued each hostage
//
//plugin_registerevt("hostage_allrescued","EvtHostageARescued");
//
forward EvtHostageARescued(UserIndex);

//T Win - Timer expired without all unkilled hostages being rescued - occurs after round end
//
//plugin_registerevt("hostage_notrescued","EvtHostageNRescued");
//
forward EvtHostageNRescued();







////////////////////////////////////////////////////////////////////////////////////////
// Bomb / Defusal Events


//A player has spawned with the bomb
//
//plugin_registerevt("bomb_spawn","EvtBombSpawn");
//
forward EvtBombSpawn(UserIndex);

//A player with the bomb has entered/left the bomb zone
//
//plugin_registerevt("bomb_zone","EvtBombZone");
//
//This occurs with Enabled=0 when the player drops/plants
//This occurs with Enabled=1 when the player spawns with the bomb or leaves a bomb zone
//This occurs with Enabled=2 when the player enters he bomb zone (bomb icon flashing)
//
forward EvtBombZone(UserIndex,Enabled);

//A player has dropped the bomb voluntarily or through death
//
//plugin_registerevt("bomb_dropped","EvtBombDropped");
//
forward EvtBombDropped(UserIndex,X,Y,Z);

//A player has picked up a dropped bomb
//
//plugin_registerevt("bomb_pickup","EvtBombPickup");
//
forward EvtBombPickup(UserIndex);

//A player has tried to plant the bomb outside a bomb zone
//
//plugin_registerevt("bomb_plantfailed","EvtBombPlantFailed");
//
forward EvtBombPlantFailed(UserIndex);

//A player has started planting the bomb
//
//plugin_registerevt("bomb_planting","EvtBombPlanting");
//
forward EvtBombPlanting(UserIndex,Time);

//A player has planted the bomb. Look at the previous bomb_planting
//event to determine who. Time = seconds it will take to plant
//
//plugin_registerevt("bomb_planted","EvtBombPlanted");
//
forward EvtBombPlanted(UserIndex);

//A player has started defusing the bomb
//
//plugin_registerevt("bomb_defusing","EvtBombDefusing");
//
//Time = seconds it will take to defuse
//Kit = 1 when player has a defusal kit
//
forward EvtBombDefusing(UserIndex,Time,Kit);

//A player has completed defusing the bomb - occurs after round end.
//No reliable way to determine which player - you can use the previous
//defusing messages and the time, but 2 players may be defusing simulateously
//
//plugin_registerevt("bomb_defused","EvtBombDefused");
//
forward EvtBombDefused(UserIndex);

//T Win - The bomb has exploded - occurs after round end
//
//plugin_registerevt("bomb_detonated","EvtBombDetonated");
//
forward EvtBombDetonated(UserIndex);

//CT Win - The bomb has not exploded at round end. This occurs regardless
//of whether it was planted or not.
//
//plugin_registerevt("bomb_notdetonated","EvtBombNotDetonated");
//
forward EvtBombNotDetonated();






////////////////////////////////////////////////////////////////////////////////////////
// VIP / Terrorist Escape events

//Player has been selected as the VIP. NOTE: This can occur mid-round while dead
//if the previous VIP left and a new player joins, but uusally occurs at the start
//of a round. This event doesn't fire every round - only when the VIP is a different
//player.
//
//plugin_registerevt("vip_change","EvtVIPChange");
//
forward EvtVIPChange(UserIndex);

//CT Win - VIP has escaped - use the prevous vip_change event to determine who
//
//plugin_registerevt("vip_escaped","EvtVIPEscaped");
//
forward EvtVIPEscaped(UserIndex);

//T Win - VIP has not escaped at end of round - fires after end of round
//
//plugin_registerevt("vip_escaped","EvtVIPEscaped");
//
forward EvtVIPNotEscaped(UserIndex);

//T Win - VIP was killed
//If you need to know who killed them, track the "vip_change" and "death" events so you
//get a killer index for the vip.
//
//plugin_registerevt("vip_death","EvtVIPDeath");
//
forward EvtVIPDeath(UserIndex);

//A single terrorist has escape on an "es" map.
//
//plugin_registerevt("terrorist_escaped","EvtTerroristEscaped");
//
forward EvtTerroristEscaped(UserIndex);

//CT Win - End of round and not enough terrorist have escaped - fires after end of round.
//
//plugin_registerevt("terrorist_notescaped","EvtTNotEscaped");
//
forward EvtTNotEscaped();





////////////////////////////////////////////////////////////////////////////////////////
// Gun / Ammo / Item events

//A player has entered/left their teams buy zone through movement/death/respawning
//
//plugin_registerevt("buy_zone","EvtBuyZone");
//
forward EvtBuyZone(UserIndex,Enabled);

//A players money has changed through spending or rewards
//
//plugin_registerevt("money_change","EvtMoneyChange");
//
forward EvtMoneyChange(UserIndex,Money);

//A player has purchased or picked up a new weapon
//
//plugin_registerevt("weapon_pickup","EvtWeaponPickup");
//
forward EvtWeaponPickup(UserIndex,Weapon);

//A player has purchased or picked up ammunition
//
//plugin_registerevt("ammo_pickup","EvtAmmoPickup");
//
forward EvtAmmoPickup(UserIndex,Type,Quantity);

//A player has purchased or picked up equipment - works for armour (kevlar) and
//helmet+armour (assaultsuit) and defusal kit (defuser). Night vision purchase
//is not detected.
//
//Quirk: If you already have a defuser at the start of a round, this will fire
//       If you already have Armour/Helmet it won't.
//
//
//plugin_registerevt("item_pickup","EvtItemPickup");
//
forward EvtItemPickup(UserIndex,HLName);

//A player has selected a new weapon slot, fired the selected weapon or reloaded the selected weapon.
//You can determine which from comparing two of these events for the same player:
//
//Weapon or nothing at all changes = changed slot (could change to the same one)
//Ammo goes down = bullets fired
//Ammo goes up = reloaded gun.
//
//plugin_registerevt("weapon_change","EvtWeaponChange");
//
forward EvtWeaponStored(UserIndex,Weapon,Ammo);

//A player has switched weapon without droping the old one. This is information about
//the previous weapon.
//
//plugin_registerevt("weapon_stored","EvtWeaponStored");
//
forward EvtWeaponStored(UserIndex,Weapon,Ammo);



//A player has turned night vision on or off
//
//plugin_registerevt("money_change","EvtMoneyChange");
//
forward EvtNightVision(UserIndex,Enabled);

