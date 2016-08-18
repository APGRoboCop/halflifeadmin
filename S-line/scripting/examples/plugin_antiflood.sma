/*
 * ===========================================================================
 *
 * $Id: plugin_antiflood.sma,v 1.5 2003/09/11 19:41:07 darope Exp $
 *
 *
 * Copyright (c) 1999-2003 Alfred Reynolds, Florian Zschocke, Magua
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
 * ===========================================================================
 *
 * Comments:
 *
 * Original author and copyright holder: Luke Sankey 
 *
 * This is a flooding filter script for adminmod that controls the
 * number of say/team_say/admin_ commands a client can do in a 
 * short amount of time. It determines that a user is flooding the 
 * server if they say 5 things in less than 2 seconds (default values).
*/


#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>


// The plugin will kick a player if they say more than MAX_LIMIT things
// in less than MAX_SECS seconds.  The plugin will simply warn a
// player if they say more than WARN_LIMIT things in less than MAX_SECS
// seconds.
#define MAX_LIMIT 5
#define WARN_LIMIT 4
#define MAX_SECS 2

new AM_VERSION_STRING[] = "2.50.05";

// Keep track of when the last thing was said
new LastTime[MAX_PLAYERS];
// Keep track of the number of things said for each second in the window
new NumSaid[MAX_PLAYERS][MAX_SECS];
// Remember if we kicked the person
new WasKicked[MAX_PLAYERS] = {0,...};
// Keep track of users's IP address for logging
new UserIP[MAX_PLAYERS][MAX_TEXT_LENGTH];

// Status codes
#define FLOOD_WARN 1
#define FLOOD_KICK 2

/****************************************************************************/

public plugin_connect(HLUserName, HLIP, UserIndex) 
{
	new strIP[MAX_TEXT_LENGTH];
	convert_string(HLIP, strIP, MAX_TEXT_LENGTH);

	// Save IP for later in case we have to log something
	strcpy(UserIP[UserIndex], strIP, MAX_TEXT_LENGTH);

	// Reset the user's data when they connect
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS)
	{
		new i;
		for (i = 0; i < MAX_SECS; i++)
			NumSaid[UserIndex][i] = 0;
		LastTime[UserIndex] = 0;
		WasKicked[UserIndex] = false;
	}

	return PLUGIN_CONTINUE;
}

/****************************************************************************/

public plugin_command(HLCommand, HLData, HLUserName, UserIndex)
{
	new Status;
	new Text[MAX_TEXT_LENGTH];
	new Date[MAX_TEXT_LENGTH];
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	new sAuthID[MAX_AUTHID_LENGTH];
	convert_string(HLCommand, Command, MAX_COMMAND_LENGTH);
	convert_string(HLData, Data, MAX_DATA_LENGTH);
	convert_string(HLUserName, User, MAX_NAME_LENGTH);

	// Filter say, say_team, and admin_ commands
	if ((strncasecmp(Command, "say", 3) == 0) || (strncasecmp(Command, "admin_", 6) == 0))
	{
		get_userAuthID(User, sAuthID);
		if (strcmp(User, "Admin") == 0 && strcmp(sAuthID, "") == 0)
			return PLUGIN_CONTINUE; // ignore the server console

		// Check to see if the user is flooding the server or not
		Status = IsFlooding(User, UserIndex);

		if (Status == FLOOD_WARN)
		{
			messageex(User, "You are flooding the server. Stop.", print_center);
			messageex(User, "You are flooding the server. Stop.", print_console);

			return PLUGIN_HANDLED;
		}
		else if (Status == FLOOD_KICK)
		{
			if (WasKicked[UserIndex])
			{
				return PLUGIN_HANDLED;
			}
			WasKicked[UserIndex] = true;
			message(User, "You were warned, but did not stop flooding the server.");
			servertime(Date, MAX_NUMBER_LENGTH, "%m/%d/%y %H:%M:%S");
			snprintf(Text, MAX_TEXT_LENGTH, "%s - %s<%s><%s> was kicked using command ^"%s %s^"",
				Date, User, UserIP[UserIndex], sAuthID, Command, Data);
			writefile("FloodLog.txt", Text);
			log(Text);
			kick(User);

			return PLUGIN_HANDLED;
		}
	}

	return PLUGIN_CONTINUE;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
// Returns 0 if the user is allowed to say things
// Returns FLOOD_WARN if the user needs to be warned
// Returns FLOOD_KICK if the user needs to be kicked
///////////////////////////////////////////////////////////////////////////////
IsFlooding(User[], UserIndex)
{
	if(check_immunity(User) == 0)
	{
		new i, j;
		new Total;
		new CurTime;
		new SecsAgo;

		Total = 0;
		CurTime = systemtime();

		// Special case if this is the first time the user has said
		// anything since they joined
		if (LastTime[UserIndex] == 0)
		{
			LastTime[UserIndex] = CurTime;
			return 0;
		}

		// Determine how many seconds ago the last thing was said
		SecsAgo = CurTime - LastTime[UserIndex];

		// Do some bookkeeping
		if (SecsAgo > MAX_SECS)
		{
			for (i = 0; i < MAX_SECS; i++)
				NumSaid[UserIndex][i] = 0;
		}
		else
		{
			for (j = 0; j < SecsAgo; j++)
			{
				for (i = 1; i < MAX_SECS; i++)
					NumSaid[UserIndex][i] = NumSaid[UserIndex][i-1];
				NumSaid[UserIndex][0] = 0;
			}
		}

		// Update User Data
		NumSaid[UserIndex][0] = NumSaid[UserIndex][0] + 1;
		LastTime[UserIndex] = CurTime;

		// Calculate the number of things the player has said in
		// the last MAX_SECS
		for (i = 0; i < MAX_SECS; i++)
			Total += NumSaid[UserIndex][i];

		// If they've said too much, then kick them.
		if (Total > MAX_LIMIT)
			return FLOOD_KICK;

		// Or, maybe warn them.
		else if (Total > WARN_LIMIT)
			return FLOOD_WARN;

	} // end if not immune

	return 0;
}

/****************************************************************************/

public plugin_init() 
{
	plugin_registerinfo("Admin Anti-Flood Plugin", "Auto kicks flooders", AM_VERSION_STRING);
	
	return PLUGIN_CONTINUE;
}
