/*
 * $Id: plugin_chat.sma,v 1.3 2001/09/27 20:33:16 darope Exp $
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
 * This plugin contains stuff that responds to 'say' commands
 *
 */


#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.2";

new MessageMode[MAX_PLAYERS][MAX_DATA_LENGTH];

SayCurrentMap() {
	new Text[MAX_TEXT_LENGTH];
	new CurrentMap[MAX_NAME_LENGTH];
	
	currentmap(CurrentMap,MAX_NAME_LENGTH);
	snprintf(Text, MAX_TEXT_LENGTH, "The current map is: %s", CurrentMap);
	say(Text);
}

SayNextMap() {
	new Text[MAX_TEXT_LENGTH];
	new NextMap[MAX_NAME_LENGTH];
	
	nextmap(NextMap,MAX_NAME_LENGTH);
	snprintf(Text, MAX_TEXT_LENGTH, "The next map will be: %s", NextMap);
	say(Text);
}

SayTimeleft() {
	new Text[MAX_TEXT_LENGTH];
	new Seconds = timeleft(0);
	
	Seconds /= 60;	
	snprintf(Text, MAX_TEXT_LENGTH, "Time remaining on map: %i minutes", Seconds);
	say(Text);
}

public admin_messagemode(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	if (UserIndex == 0) {
		selfmessage("This command cannot be used from the console.");
		return PLUGIN_HANDLED;
	}
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
		
	if (streq(Command,"admin_nomessagemode")==1) {
		strinit(MessageMode[UserIndex]);
		selfmessage("Message mode off. Your 'say' commands will be treated as normal.");
	} else if (strlen(Data)==0) {
		strinit(MessageMode[UserIndex]);
		selfmessage("Message mode off. Your 'say' commands will be treated as normal.");
	} else if (strmatch(Data,"say",3)==1) {
		strinit(MessageMode[UserIndex]);
		selfmessage("Message mode off. Your 'say' commands will be treated as normal.");
	} else {
		strcpy(MessageMode[UserIndex], Data, MAX_DATA_LENGTH);
		snprintf(Text, MAX_TEXT_LENGTH,"Message mode on. Your 'say' commands will be treated as '%s'.", MessageMode[UserIndex]);
		selfmessage(Text);
	}
	return PLUGIN_HANDLED;
}

public HandleSay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	strstripquotes(Data);
	if (strlen(MessageMode[UserIndex]) != 0 && UserIndex != 0) {
	  if ( strcasestr(MessageMode[UserIndex], "admin_") >= 0 ) { // admin_* command
	    plugin_exec( MessageMode[UserIndex], Data );
	    return PLUGIN_HANDLED;
	  } else {   // HL or MOD command
	    snprintf(Text, MAX_TEXT_LENGTH, "%s %s", MessageMode[UserIndex], Data);
	    execclient(User,Text);
	    return PLUGIN_HANDLED;
	  }
	}
	if (streq(Data, "timeleft")==1) {
		SayTimeleft();
	} else if (streq(Data, "version")==1) {
		say("Type 'admin_version' in the console for version information.");
	} else if (streq(Data, "nextmap")==1) {
		SayNextMap();
	} else if (streq(Data, "currentmap")==1) {
		SayCurrentMap();
	}

	return PLUGIN_CONTINUE;
}

public plugin_connect(HLUserName, HLIP, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		strinit(MessageMode[UserIndex]);
	}
	return PLUGIN_CONTINUE;
}

public plugin_disconnect(HLUserName, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		strinit(MessageMode[UserIndex]);
	}
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Admin Chat Plugin","Commands for responding to chat.",STRING_VERSION);
	
	plugin_registercmd("admin_messagemode","admin_messagemode",ACCESS_ALL,"admin_messagemode <command>: Will treat 'say' as command.");
	plugin_registercmd("admin_nomessagemode","admin_messagemode",ACCESS_ALL,"admin_nomessagemode: Will treat 'say' as 'say'.");
	plugin_registercmd("say","HandleSay",ACCESS_ALL);
	plugin_registerhelp("say",ACCESS_ALL,"say currentmap: Will respond with the current map's name.");
	plugin_registerhelp("say",ACCESS_ALL,"say nextmap: Will respond with the next map in the cycle.");
	plugin_registerhelp("say",ACCESS_ALL,"say timeleft: Will respond with the time left.");
	
	return PLUGIN_CONTINUE;
}
