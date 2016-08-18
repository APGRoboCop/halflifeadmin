/* This plugin contains various cheating-type commands */

/* $Id: plugin_cheat.sma,v 1.5 2001/03/28 11:43:43 darope Exp $ */

#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

#define ACCESS_GODMODE 8192
#define ACCESS_NOCLIP 8192
#define ACCESS_TELEPORT 8192

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.0";

public admin_godmode(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_DATA_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iGodMode;
	new strGodMode[MAX_DATA_LENGTH];
	new strGodModeUser[MAX_DATA_LENGTH];
	new User[MAX_DATA_LENGTH];
	
	convert_string(HLCommand,Command,MAX_DATA_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_DATA_LENGTH);
	strbreak(Data,strGodModeUser,strGodMode, MAX_DATA_LENGTH);
	iGodMode = check_param(strGodMode);
	
	if (check_user(strGodModeUser)==1) {
		say_command(User,Command,Data,1);
		godmode(strGodModeUser,iGodMode);
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(strGodModeUser);
	}
	return PLUGIN_HANDLED;
}

public admin_noclip(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_DATA_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iNoclip;
	new strNoclip[MAX_DATA_LENGTH];
	new strNoclipUser[MAX_DATA_LENGTH];
	new User[MAX_DATA_LENGTH];
	
	convert_string(HLCommand,Command,MAX_DATA_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_DATA_LENGTH);
	strbreak(Data,strNoclipUser,strNoclip, MAX_DATA_LENGTH);
	iNoclip = check_param(strNoclip);
	
	if (check_user(strNoclipUser)==1) {
		say_command(User,Command,Data,1);
		noclip(strNoclipUser,iNoclip);
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(strNoclipUser);
	}
	return PLUGIN_HANDLED;
}

public admin_stack(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_DATA_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Dead;
	new i;
	new maxplayers = maxplayercount();
	new Name[MAX_NAME_LENGTH];
	new Result;
	new SessionID;
	new Team;
	new User[MAX_NAME_LENGTH];
	new WONID;
	new X;
	new Y;
	new Z;
	
	convert_string(HLCommand,Command,MAX_DATA_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_DATA_LENGTH);
	get_userorigin(User,X,Y,Z);
	for (i=1; i<=maxplayers; i++) {
		Result = playerinfo(i,Name,MAX_NAME_LENGTH,SessionID,WONID,Team,Dead);
		if(Result==1) {
			if (check_immunity(Name)==0 && i != UserIndex && Dead == 0) {
				Z += 96;
				teleport(Name, X, Y, Z);
			}
		}
	}
	say_command(User,Command,Data,1);
	return PLUGIN_HANDLED;
}

/* Usage of admin_teleport:

admin_teleport <player> <X> <Y> <Z>

<player> = Player, of course.  Can be name, WON ID, Session ID, etc.
<X>, <Y>, <Z> = Integer numbers.  Use admin_userorigin to find these.

eg:
admin_teleport jaguar 30 400 -350

*/
public admin_teleport(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new strX[MAX_NUMBER_LENGTH];
	new strY[MAX_NUMBER_LENGTH];
	new strZ[MAX_NUMBER_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new TeleportUser[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	new X = 0;
	new Y = 0;
	new Z = 0;
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	
	strgsep(Data, " ", "^"", TeleportUser, MAX_NAME_LENGTH, strX, MAX_NUMBER_LENGTH, strY, MAX_NUMBER_LENGTH, strZ, MAX_NUMBER_LENGTH);
	
	if (check_user(TeleportUser)==0) {
		selfmessage("Unrecognized player: ");
		selfmessage(TeleportUser);
		return PLUGIN_HANDLED;
	}	
	get_username(TeleportUser, TargetName, MAX_NAME_LENGTH);
	
	if(strlen(strX)==0) {
		selfmessage( "Unparsable format: no X value found.");
		return PLUGIN_HANDLED;
	} 
	X = strtonum(strX);
	
	if(strlen(strY)==0) {
		selfmessage( "Unparsable format: no Y value found.");
		return PLUGIN_HANDLED;
	} 
	Y = strtonum(strY);

	if(strlen(strZ)==0) {
		selfmessage( "Unparsable format: no Z value found.");
		return PLUGIN_HANDLED;
	} 
	Z = strtonum(strZ);
	
	snprintf(Text, MAX_TEXT_LENGTH, "Attempting teleport of '%s' to X %i, Y %i, Z %i.", TargetName, X, Y, Z);
	selfmessage(Text);
	
	say_command(User,Command,TargetName,1);
	if (streq(TargetName, User)==0) {
		if (check_immunity(TargetName)==1) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't teleport %s, you silly bear.", TargetName);
			say(Text);
			return PLUGIN_HANDLED;
		}
	}
	if(teleport(TargetName,X,Y,Z)==1) {
		selfmessage("Succeeded.");
	} else {
		selfmessage("Failed.");
	}
	return PLUGIN_HANDLED;
}

/* Usage:

admin_userorigin <player>

<Player> = Player.  May be name, WON ID, Session ID, etc.

eg, 

admin_userorigin jaguar

Will show you the X, Y, and Z coordinates of the player.
*/
public admin_userorigin(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new X = 0;
	new Y = 0;
	new Z = 0;
	
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	if (check_user(Data)==1) {
		if (get_userorigin(Data, X, Y, Z)==1) {
			snprintf(Text, MAX_TEXT_LENGTH, "Success.  X %i, Y %i, Z %i", X, Y, Z);
			selfmessage(Text);			
		} else {
			selfmessage("Failed.");
		}
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

public plugin_init() {
	plugin_registerinfo("Admin Cheating Plugin","Commands for noclip, teleporting, godmode, etc.",STRING_VERSION);

	plugin_registercmd("admin_godmode","admin_godmode",ACCESS_GODMODE,"admin_godmode <target> <^"on^" | ^"off^">: Sets godmode on target.");
	plugin_registercmd("admin_noclip","admin_noclip",ACCESS_NOCLIP,"admin_noclip <target> <^"on^" | ^"off^">: Sets noclip on target.");
	plugin_registercmd("admin_stack","admin_stack",ACCESS_TELEPORT,"admin_stack: Will stack everyone on top of you.");
	plugin_registercmd("admin_teleport","admin_teleport",ACCESS_TELEPORT,"admin_teleport <target> <X> <Y> <Z>: Teleports target to the given coordinates. See admin_userorigin.");
	plugin_registercmd("admin_userorigin","admin_userorigin",ACCESS_TELEPORT,"admin_userorigin <target>: Returns the X, Y, Z coordinates of target.");
	return PLUGIN_CONTINUE;
}
