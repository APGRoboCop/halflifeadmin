/*
 * $Id: plugin_TFC.sma,v 1.3 2001/09/27 20:33:16 darope Exp $
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
 * This plugin contains commands specific for the Team Fortress Classic mod
 *
 */

#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

#define ACCESS_PREMATCH 4
#define ACCESS_BALANCE 32
#define ACCESS_CHANGE_TEAM 8192

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.0";

public admin_balance(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_NAME_LENGTH);
	convert_string(HLData,Data,MAX_NAME_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if(check_param(Data)==1) {
		execute_command(User,Command,"admin_balance_teams","1");
	} else {
		execute_command(User,Command,"admin_balance_teams","0");
	}
	return PLUGIN_HANDLED;
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
		} else if (streq(Command,"admin_blue")==1) {
			execclient(TargetName, "jointeam 1");
		} else if (streq(Command,"admin_red")==1) {
			execclient(TargetName, "jointeam 2");
		} else if (streq(Command,"admin_yellow")==1) {
			execclient(TargetName, "jointeam 3");
		} else if (streq(Command,"admin_green")==1) {
			execclient(TargetName, "jointeam 4");
		}
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(Data);
	}

	return PLUGIN_HANDLED;
}
 
public admin_prematch(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
		
	convert_string(HLCommand,Command,MAX_NAME_LENGTH);
	convert_string(HLData,Data,MAX_NAME_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"tfc_clanbattle_prematch", Data);
	return PLUGIN_HANDLED;
}

public HandleJoinTeam(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	new iNewTeam;
	new iNewTeamCount;
	new iOldTeamCount;
	new iTeam;
	new Name[MAX_NAME_LENGTH];
	new SessionID;
	new strNewTeam[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	new WONID;
	
	if (UserIndex < 1)
		return PLUGIN_CONTINUE;
		
	if(getvar("admin_balance_teams")==1) {
		
		convert_string(HLData,Data,MAX_DATA_LENGTH);
		convert_string(HLUserName,User,MAX_NAME_LENGTH);
		
		playerinfo(UserIndex, Name, MAX_NAME_LENGTH, SessionID, WONID, iTeam);
		/* If they're not currently on a team, ignore it. */
		if(iTeam <= 0)
			return PLUGIN_CONTINUE;
		strcpy(strNewTeam,Data,MAX_DATA_LENGTH);
		iNewTeam = strtonum(strNewTeam);
		/* If they're trying to switch to the team they're already on, ignore it. */
		if (iTeam == iNewTeam)
			return PLUGIN_CONTINUE;
			
		iOldTeamCount = getteamcount(iTeam);
		/* If they're the only one on their current team, ignore it (this is for instances
		like 'Hunted', where the hunted team has only one person on it) */
		if(iOldTeamCount<=1)
			return PLUGIN_CONTINUE;
		iNewTeamCount = getteamcount(iNewTeam);
		/* If there's only one person on the other team, ignore it. */
		if(iNewTeamCount<=1)
			return PLUGIN_CONTINUE;

		/* Otherwise, only allow the switch if the new team has less people than the old team. */
		if(iNewTeamCount >= iOldTeamCount) {
			execclient(User, "speak ^"Team switch denied^"");

			snprintf(Text, MAX_TEXT_LENGTH, "Old Team: %i New Team: %i -- Team Switch Denied", iOldTeamCount, iNewTeamCount);
			selfmessage(Text);
			return PLUGIN_HANDLED;
		}
	}
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Admin TFC Plugin","Commands specific to the Team Fortress Classic mod.",STRING_VERSION);

	plugin_registercmd("admin_balance","admin_balance",ACCESS_BALANCE,"admin_balance <^"on^" | ^"off^">: Sets the forced team balancing.");
	plugin_registercmd("admin_blue","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_blue <target>: Changes target to the blue team.");
	plugin_registercmd("admin_green","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_green <target>: Changes target to the green team.");
	plugin_registercmd("admin_prematch","admin_prematch",ACCESS_PREMATCH,"admin_prematch <prematch>: Sets the tfc_clanbattle_prematch cvar.");
	plugin_registercmd("admin_red","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_red <target>: Changes target to the red team.");
	plugin_registercmd("admin_yellow","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_yellow <target>: Changes target to the yellow team.");
	plugin_registercmd("jointeam","HandleJoinTeam",ACCESS_ALL);
	
	return PLUGIN_CONTINUE;
}
