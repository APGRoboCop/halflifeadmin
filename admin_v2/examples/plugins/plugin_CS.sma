/*
 * $Id: plugin_CS.sma,v 1.3 2001/09/27 20:33:16 darope Exp $
 *
 *
 * Copyright (c) 1999-2001 Alfred Reynolds, Florian Zschocke, Magua
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
 * ===========================================================================
 *
 * Comments:
 *
 * This plugin contains commands specific for the Counterstrike mod
 *
 * Restart round voting by Nathan O'Sullivan
 * Weapon restriction by GIJoe (CSController@yahoo.com) 
 *
 */

#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

#define ACCESS_VOTE_RESTART 1
#define ACCESS_RESTART 2
#define ACCESS_CHANGE_TEAM 8192
#define ACCESS_RESTRICT 8192

#define ITEM_INVALID -1

#define MAX_MENU_CHOICES 10
#define MAX_MENUS 10

#define MENU_EQUIPMENT 8
#define MENU_INVALID -1
#define MENU_TOP 0

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.0";

new RestrictMenu[MAX_MENUS][MAX_MENU_CHOICES];
new UserMenu[MAX_PLAYERS];

AllowAllWeapons(AllowWeapon) {
	new i;
	new j;
	
	for(i=0;i<MAX_MENUS;i++)
		for(j=0;j<MAX_MENU_CHOICES;j++)
			RestrictMenu[i][j]=AllowWeapon;
}

AllowMenu(Menu,AllowWeapon) {
	new i;
	
	for(i=0;i<MAX_MENU_CHOICES;i++)
		RestrictMenu[Menu][i]=AllowWeapon;
}

AllowWeapon(Menu,Weapon,AllowWeapon) {
	RestrictMenu[Menu][Weapon]=AllowWeapon;
}

public admin_changeteam(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
		
	convert_string(HLCommand,Command,MAX_NAME_LENGTH);
	convert_string(HLData,Data,MAX_NAME_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	
	if (check_user(Data) == 1) {
		get_username(Data,TargetName,MAX_NAME_LENGTH);
		say_command(User,Command,TargetName);
		if (check_immunity(TargetName) != 0) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't move %s onto another team, you silly bear.", TargetName);
			say(Text);
		} else if (streq(Command,"admin_ct")==1) {
			execclient(TargetName, "chooseteam;menuselect 2;menuselect 3"); 
		} else if (streq(Command,"admin_t")==1) {
			execclient(TargetName, "chooseteam;menuselect 1;menuselect 3"); 
		}
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(Data);
	}

	return PLUGIN_HANDLED;
}

/* Weapon restriction commands by GIJoe. Usage:

admin_allowweapon <menu number> <weapon number>
admin_restrictweapon <menu number> <weapon number>

<menu number> = Number of menu weapon is in.
<weapon number> = Number of weapon to be restricted.

Ex,

admin_restrictweapon 1 2  would disallow anyone to purchase the number 2 pistol.
admin_allowweapon 1 2 would allow anyone to purchase the number 2 pistol

admin_allowmenu <menu>
admin_restrictmenu <menu>

admin_allowallweapons
admin_restrictallweapons

*/
public admin_handleall(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (streq(Command, "admin_enableallweapons")==1) {
		AllowAllWeapons(1);
	} else {
		AllowAllWeapons(0);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

public admin_handleequip(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (streq(Command, "admin_enableequipment")==1) {
		AllowMenu(MENU_EQUIPMENT,1);
	} else {
		AllowMenu(MENU_EQUIPMENT,0);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

public admin_handlemenu(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iMenu;
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	iMenu = strtonum(Data);
	if(iMenu <= 0 || iMenu >= MAX_MENUS) {
		selfmessage( "Invalid menu number.");
		return PLUGIN_HANDLED;
	} else if (streq(Command, "admin_enablemenu")==1) {
		AllowMenu(iMenu,1);
	} else {
		AllowMenu(iMenu,0);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

public admin_handleweapon(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iItem;
	new iMenu;
	new strItem[MAX_DATA_LENGTH];
	new strMenu[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strbreak(Data, strMenu, strItem, MAX_DATA_LENGTH);
	iItem = strtonum(strItem);
	iMenu = strtonum(strMenu);
	if(iMenu <= 0 || iMenu >= MAX_MENUS) {
		selfmessage( "Invalid menu number.");
		return PLUGIN_HANDLED;
	} else if (iItem <= 0 || iItem >= MAX_MENU_CHOICES) {
		selfmessage( "Invalid item number.");
		return PLUGIN_HANDLED;
	} else if (streq(Command, "admin_enableweapon")==1) {
		AllowWeapon(iMenu,iItem,1);
	} else {
		AllowWeapon(iMenu,iItem,0);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

public admin_restartround(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"sv_restartround",Data);
	return PLUGIN_HANDLED;
}

/* Restart vote by Nathan O'Sullivan */
public admin_vote_restart(HLCommand,HLData,HLUserName,UserIndex) {
	new unused[MAX_NAME_LENGTH];
	
	if (vote_allowed()!=1) {
		selfmessage( "Vote not allowed at this time.");
		return PLUGIN_HANDLED;
	}

	vote("Restart map & begin play?","Yes","No","restart_vote",unused);
	return PLUGIN_HANDLED;
}

public restart_vote(a,b,c,d) {
	new Text[MAX_TEXT_LENGTH];
	
	if ( a == 1 ) {
		new ratio = getvar("map_ratio");
		if ( ratio == 0 ) {
			say("Restart vote is not enabled");
			return;
		}
		if ( c > (ratio/100)*d ) {
			new tleft, tlimit;
			tleft = timeleft() - 10; // # seconds till restart!
			tlimit = getvar("mp_timelimit");
			tlimit = tlimit + (tlimit - tleft/60);
			
			snprintf(Text, MAX_TEXT_LENGTH, "mp_timelimit %i", tlimit);
			exec(Text);
			exec ("sv_restartround 10");
		} else {
			say("Not enough votes for map restart");
		}
	} else {
		say("Not enough votes for map restart");
	}
}

public admin_weaponscheck(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_NAME_LENGTH);
	convert_string(HLData,Data,MAX_NAME_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if(check_param(Data)==1) {
		execute_command(User,Command,"admin_cs_restrict","1");
	} else {
		execute_command(User,Command,"admin_cs_restrict","0");
	}
	return PLUGIN_HANDLED;
}

public ClearMenuData(HLCommand,HLData,HLUserName,UserIndex) {
	if (getvar("admin_cs_restrict")==1) {
		UserMenu[UserIndex] = MENU_INVALID;
	}
	return PLUGIN_CONTINUE;
}

public SetMenuData(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	
	if (getvar("admin_cs_restrict")==1) {
		convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
		if (streq(Command, "buyequip")==1) {
			UserMenu[UserIndex] = MENU_EQUIPMENT;
		} else if (streq(Command, "buy")==1) {
			UserMenu[UserIndex] = MENU_TOP;
		/* Sank's showbriefing fix */
		} else if (streq(Command, "showbriefing")==1) {
			if (UserMenu[UserIndex] == MENU_INVALID) {
				UserMenu[UserIndex] = 1;
			}
		}
	}
	return PLUGIN_CONTINUE;
}

public SetSelectData(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	new iMenu;

	if (getvar("admin_cs_restrict")==1) {
		convert_string(HLData,Data,MAX_DATA_LENGTH);
		convert_string(HLUserName,User,MAX_NAME_LENGTH);

		iMenu = UserMenu[UserIndex];
		/* If they're at the top menu, look at the data and set that as their new menu.
		6, 7, and 10 are exceptions, because those exit the menu process. For them, set
		them at no menu. */
		if (iMenu == MENU_TOP) {
			new iNewMenu = strtonum(Data);
			if (iNewMenu == 6 || iNewMenu == 7 || iNewMenu == 10) {
				UserMenu[UserIndex] = MENU_INVALID;
			} else {
				UserMenu[UserIndex] = iNewMenu;
			}
		/* Otherwise, check to see if we should allow their purchase to happen.  If so,
		happy days...otherwise, eat the message and reset their menu level. */
		} else if (iMenu > MENU_INVALID) {
			new iNewItem = strtonum(Data);
			
			if (iNewItem==10) {
				UserMenu[UserIndex] = MENU_INVALID;
			} else if (RestrictMenu[iMenu][iNewItem]==0) {
				messageex(User, "Weapon currently not allowed", print_center);
				/* Sank's showbriefing fix. 
				UserMenu[UserIndex] = MENU_INVALID;
				
				Instead of setting UserMenu just to negative 1,
				negate UserMenu so that the showbriefing intercept works */
				UserMenu[UserIndex] = -UserMenu[UserIndex];
				return PLUGIN_HANDLED;
			} else {
				UserMenu[UserIndex] = MENU_INVALID;
			}
		} else if (iMenu < MENU_INVALID) {
			/* Sank's showbriefing fix 
			Only Showbriefing or some other menu-bypassing method should get in here */
			new iNewItem = strtonum(Data);
			
			if((RestrictMenu[0 - iMenu][iNewItem]) == 0)
			{
				messageex(User, "SERVER CURRENTLY DOES NOT ALLOW THAT WEAPON", print_center);
				return PLUGIN_HANDLED;
			}
		} else {
			UserMenu[UserIndex] = MENU_INVALID;
		}
	}
	return PLUGIN_CONTINUE;
}

public plugin_connect(HLUserName, HLIP, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		UserMenu[UserIndex] = MENU_INVALID;
	}
	return PLUGIN_CONTINUE;
}

public plugin_disconnect(HLUserName, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		UserMenu[UserIndex] = MENU_INVALID;
	}
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Admin Counterstrike Plugin","Commands specific to the Counterstrike mod.",STRING_VERSION);

	plugin_registercmd("admin_ct","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_ct <target>: Changes target to the counter-terrorist team.");
	plugin_registercmd("admin_enableallweapons","admin_handleall",ACCESS_RESTRICT,"admin_enableallweapons: Allows anyone to buy anything.");
	plugin_registercmd("admin_enableequipment","admin_handleequip",ACCESS_RESTRICT,"admin_enableequipment: Allows anyone to buy any equipment.");
	plugin_registercmd("admin_enablemenu","admin_handlemenu",ACCESS_RESTRICT,"admin_enablemenu <menu>: Allows anyone to buy anything on the specified menu.");
	plugin_registercmd("admin_enableweapon","admin_handleweapon",ACCESS_RESTRICT,"admin_enableweapon <menu> <item>: Allows anyone to buy the specified item on the specified menu.");
	plugin_registercmd("admin_restartround","admin_restartround",ACCESS_RESTART,"admin_restart <seconds>: Restarts the round.");
	plugin_registercmd("admin_restrictallweapons","admin_handleall",ACCESS_RESTRICT,"admin_restrictallweapons: Forbids anyone from buying anything.");
	plugin_registercmd("admin_restrictequipment","admin_handleequip",ACCESS_RESTRICT,"admin_restrictequipment: Forbids anyone from buying any equipment.");
	plugin_registercmd("admin_restrictmenu","admin_handlemenu",ACCESS_RESTRICT,"admin_restrictmenu <menu>: Forbids anyone from buying anything on the specified menu.");
	plugin_registercmd("admin_restrictweapon","admin_handleweapon",ACCESS_RESTRICT,"admin_restrictweapon <menu> <item>: Forbids anyone from buying the specified item on the specifed menu.");
	plugin_registercmd("admin_t","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_t <target>: Changes target to the terrorist team.");
	plugin_registercmd("admin_vote_restart","admin_vote_restart",ACCESS_VOTE_RESTART,"admin_vote_restart: Starts a vote to restart the round.");
	plugin_registercmd("admin_weaponscheck","admin_weaponscheck",ACCESS_RESTRICT,"admin_weaponscheck <^"on^" | ^"off^">: Toggles whether or not weapon restrictions are used.");
	plugin_registercmd("chooseteam","ClearMenuData",ACCESS_ALL);
	plugin_registercmd("radio","ClearMenuData",ACCESS_ALL);
	plugin_registercmd("buyequip","SetMenuData",ACCESS_ALL);
	plugin_registercmd("buy","SetMenuData",ACCESS_ALL);
	/* Sank's showbriefing fix */
	plugin_registercmd("showbriefing","SetMenuData",ACCESS_ALL);
	plugin_registercmd("menuselect","SetSelectData",ACCESS_ALL);
	
	AllowAllWeapons(1);
	
	return PLUGIN_CONTINUE;
}
