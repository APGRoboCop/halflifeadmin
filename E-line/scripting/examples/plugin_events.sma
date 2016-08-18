 /* $Id: plugin_events.sma,v 1.4 2003/08/20 23:01:59 bugblatter Exp $ */

#include <core>
#include <string>
#include <admin>
#include <adminlib>
#include <plugin>

#include <events_cstrike>

new AM_VERSION_STRING[] = "2.51.05";

new g_Name[MAX_NAME_LENGTH];

GetName(UserIndex) {
	playerinfo(UserIndex,g_Name,MAX_NAME_LENGTH);
}

public plugin_init(){

	plugin_registerinfo("Event Test Plugin","A plugin to test and debug adminmod events",AM_VERSION_STRING);

	plugin_registerevt("game_start","EvtGameStart");
	plugin_registerevt("round_start","EvtRoundStart");
	plugin_registerevt("round_end","EvtRoundEnd");
	plugin_registerevt("player_spawn","EvtPlayerSpawn");
	plugin_registerevt("player_health","EvtPlayerHealth");
	plugin_registerevt("player_death","EvtPlayerDeath");
	plugin_registerevt("player_score","EvtPlayerScore");
	plugin_registerevt("hostage_spawned","EvtHostageSpawned");
	plugin_registerevt("hostage_moved","EvtHostageMoved");
	plugin_registerevt("hostage_removed","EvtHostageRemoved");
	plugin_registerevt("hostage_allrescued","EvtHostageARescued");
	plugin_registerevt("hostage_notrescued","EvtHostageNRescued");
	plugin_registerevt("bomb_spawn","EvtBombSpawn");
	plugin_registerevt("bomb_zone","EvtBombZone");
	plugin_registerevt("bomb_dropped","EvtBombDropped");
	plugin_registerevt("bomb_pickup","EvtBombPickup");
	plugin_registerevt("bomb_planting","EvtBombPlanting");
	plugin_registerevt("bomb_plantfailed","EvtBombPlantFailed");
	plugin_registerevt("bomb_planted","EvtBombPlanted");
	plugin_registerevt("bomb_defusing","EvtBombDefusing");
	plugin_registerevt("bomb_defused","EvtBombDefused");
	plugin_registerevt("bomb_detonated","EvtBombDetonated");
	plugin_registerevt("bomb_notdetonated","EvtBombNotDetonated");
	plugin_registerevt("vip_change","EvtVIPChange");
	plugin_registerevt("vip_escaped","EvtVIPEscaped");
	plugin_registerevt("vip_notescaped","EvtVIPNotEscaped");
	plugin_registerevt("vip_death","EvtVIPDeath");
	plugin_registerevt("terrorist_escaped","EvtTerroristEscaped");
	plugin_registerevt("buy_zone","EvtBuyZone");
	plugin_registerevt("money_change","EvtMoneyChange");
	plugin_registerevt("weapon_pickup","EvtWeaponPickup");
	plugin_registerevt("weapon_change","EvtWeaponChange");
	plugin_registerevt("weapon_stored","EvtWeaponStored");
	plugin_registerevt("ammo_pickup","EvtAmmoPickup");
	plugin_registerevt("item_pickup","EvtItemPickup");
	plugin_registerevt("nightvision","EvtNightVision");

	return PLUGIN_CONTINUE;
}


public EvtGameStart() {
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"GAME START: Woo Yay!");
	say(Msg);
	return PLUGIN_CONTINUE;

}


public EvtRoundStart() {
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"ROUND START");
	say(Msg);
	return PLUGIN_CONTINUE;

}


public EvtRoundEnd(WinningTeam) {
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"ROUND END: WinningTeam %i",WinningTeam);
	say(Msg);
	return PLUGIN_CONTINUE;

}


public EvtPlayerTeam(Team,HLUserName,Automatic) {
	new Name[MAX_NAME_LENGTH];
	convert_string(HLUserName,Name,MAX_NAME_LENGTH);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"TEAM CHANGE: %s joined team %i - Automatic = %i",Name,Team,Automatic);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtPlayerSpawn(UserIndex) {
	new Msg[MAX_DATA_LENGTH];
	new Dummy;
	new Dead;
	GetName(UserIndex);
	playerinfo(UserIndex,Msg,MAX_DATA_LENGTH,Dummy,Dummy,Dummy,Dead);
	snprintf(Msg,MAX_DATA_LENGTH,"PLAYER SPAWN: %s has spawned - dead %i",g_Name,Dead);
	say(Msg);
	return PLUGIN_CONTINUE;

}


public EvtPlayerHealth(UserIndex,Health) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"HEALTH: %s health is %i",g_Name,Health);
	say(Msg);
	return PLUGIN_CONTINUE;

}


public EvtPlayerDeath(UserIndex,HLReason,KillerIndex,Headshot) {
	GetName(UserIndex);
	new Reason[MAX_DATA_LENGTH];
	convert_string(HLReason,Reason,MAX_DATA_LENGTH);

	new Killer[MAX_NAME_LENGTH];
	playerinfo(KillerIndex,Killer,MAX_NAME_LENGTH);

	new Msg[MAX_DATA_LENGTH];
	if (KillerIndex==UserIndex) {
		snprintf(Msg,MAX_DATA_LENGTH,"DEATH: %s died due to %s",g_Name,Reason);
	}
	else {
		snprintf(Msg,MAX_DATA_LENGTH,"DEATH: %s killed %s with %s (headshot %i)",Killer,g_Name,Reason,Headshot);
	}
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtPlayerScore(UserIndex,Kills,Deaths) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"SCORE: %s has %i kills and %i deaths",g_Name,Kills,Deaths);
	say(Msg);
	return PLUGIN_CONTINUE;
}






public EvtHostageSpawned(HostageIndex,X,Y,Z) {
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"HOSTAGE SPAWNED: hostage %i to %i,%i,%i",HostageIndex,X,Y,Z);
	say(Msg);
	return PLUGIN_CONTINUE;
}


public EvtHostageMoved(HostageIndex,X,Y,Z) {
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"HOSTAGE MOVED: hostage %i to %i,%i,%i",HostageIndex,X,Y,Z);
	say(Msg);
	return PLUGIN_CONTINUE;
}




public EvtHostageRemoved(UserIndex,HostageIndex,Killed) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	if (Killed) {
		snprintf(Msg,MAX_DATA_LENGTH,"HOSTAGE DEATH: %s killed hostage %i",g_Name,HostageIndex);
	}
	else {
		snprintf(Msg,MAX_DATA_LENGTH,"HOSTAGE RESCUE: %s rescued hostage %i",g_Name,HostageIndex);
	}
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtRescueZone(UserIndex,Enabled) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"RESCUE ZONE: %s in zone %i",g_Name,Enabled);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtHostageARescued(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"ALL HOSTAGE RESCUED: %s rescued last hostage",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;

}

public EvtHostageNRescued() {
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"T WIN - HOSTAGES NOT RESCUED");
	say(Msg);
	return PLUGIN_CONTINUE;

}















public EvtBombSpawn(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB SPAWN: %s has it",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombZone(UserIndex,Enabled) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB ZONE: %s in zone %i",g_Name,Enabled);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombDropped(UserIndex,X,Y,Z) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB DROPPED: %s dropped it at co-ordinates %i,%i,%i",g_Name,X,Y,Z);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombPickup(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB PICKUP: %s picked up the bomb",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombPlantFailed(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB PLANT: %s tried to plant outside the bomb zone",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombPlanting(UserIndex,Time) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB PLANTING: %s is planting - will take %i",g_Name,Time);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombPlanted(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB PLANTED: by %s",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;

}

public EvtBombDefusing(UserIndex,Time,Kit) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB PLANT/DEFUSE: %s is defusing - will take %i seconds, kit= %i",g_Name,Time,Kit);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombDefused(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB DEFUSED: %s was defusing (others may be defusing too)",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombDetonated(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB DETONATION: Planted by %s",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtBombNotDetonated() {
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BOMB NOT DETONATED");
	say(Msg);
	return PLUGIN_CONTINUE;
}









public EvtVIPChange(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"VIP SPAWNED: Player %s",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtVIPEscaped(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"CT WIN - VIP ESCAPED: %s was VIP",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtVIPNotEscaped(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"T WIN - VIP NOT ESCAPED: %s was VIP",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtVIPDeath(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"T WIN - VIP KILLED: %s was VIP",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}




public EvtTerroristEscaped(UserIndex) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"TERRORIST ESCAPED %s",g_Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}









public EvtBuyZone(UserIndex,Enabled) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"BUY ZONE: %s in zone %i",g_Name,Enabled);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtDefuserPurchased(UserIndex,Enabled) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"DEFUSER PURCHASE: %s purchase %i",g_Name,Enabled);
	say(Msg);
	return PLUGIN_CONTINUE;
}


public EvtMoneyChange(UserIndex,Money) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"MONEY CHANGE: %s has %i credits",g_Name,Money);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtWeaponPickup(UserIndex,Weapon) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"WEAPON PICKUP: %s has picked up / bought a %i",g_Name,Weapon);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtWeaponChange(UserIndex,Weapon,Ammo) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"WEAPON CHANGE / FIRED: %s is using %i, bullets %i",g_Name,Weapon,Ammo);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtWeaponStored(UserIndex,Weapon,Ammo) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"WEAPON STORED: %s stored a %i, bullets %i",g_Name,Weapon,Ammo);
	say(Msg);
	return PLUGIN_CONTINUE;
}
public EvtAmmoPickup(UserIndex,Type,Quantity) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"AMMO PICKUP: %s has picked up/bought %i of %i",g_Name,Quantity,Type);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtItemPickup(UserIndex,HLName) {
	GetName(UserIndex);
	new Name[MAX_DATA_LENGTH];
	convert_string(HLName,Name,MAX_DATA_LENGTH);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"ITEM PICKUP: %s has picked up/bought a %s",g_Name,Name);
	say(Msg);
	return PLUGIN_CONTINUE;
}

public EvtNightVision(UserIndex,Enabled) {
	GetName(UserIndex);
	new Msg[MAX_DATA_LENGTH];
	snprintf(Msg,MAX_DATA_LENGTH,"NIGHT VISION: %s has it %i",g_Name,Enabled);
	say(Msg);
	return PLUGIN_CONTINUE;
}
