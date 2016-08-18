/*
 * ===========================================================================
 *
 * $Id: plugin_spawn.sma,v 1.4 2003/03/26 20:44:34 darope Exp $
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
 * This script contains commands for playing with spawn objects 
 * Please note that spawning object in a server is not what you would
 * normally do and that it can make your server become unstable and crash.
 * You have been warned!
 *
 */


#include <core>
#include <string>
#include <admin>
#include <adminlib>

#define ACCESS_SPAWN 8192

new AM_VERSION_STRING[] = "2.50.04";

/* Usage:

admin_listspawn [name]

Eg,

admin_listspawn
admin_listspawn xen_tree

Returns all spawned items matching [name] (or all items, if name is given. Used
for getting the identities. 
*/
public admin_listspawn(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	listspawn(Data);
	return PLUGIN_HANDLED;
}

/* Usage:

admin_movespawn (identity) (X) (Y) (Z) (XAngle) (YAngle) (ZAngle)

Eg,

admin_movespawn 3 0 0 0 0 0 0

Moves the spawned item indicated by identity (gotten from 
admin_listspawn) to the given coordinates. 
*/
public admin_movespawn(HLCommand,HLData,HLUserName,UserIndex) {
	new iIdentity;
	new X;
	new XAngle;
	new Y;
	new YAngle;
	new Z;
	new ZAngle;
	new strIdentity[MAX_NUMBER_LENGTH];
	new strX[MAX_NUMBER_LENGTH];
	new strXAngle[MAX_NUMBER_LENGTH];
	new strY[MAX_NUMBER_LENGTH];
	new strYAngle[MAX_NUMBER_LENGTH];
	new strZ[MAX_NUMBER_LENGTH];
	new strZAngle[MAX_NUMBER_LENGTH];
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	
	strgsep(Data, " ", "^"", strIdentity, MAX_NUMBER_LENGTH, strX, MAX_NUMBER_LENGTH, strY, MAX_NUMBER_LENGTH, strZ, MAX_NUMBER_LENGTH, strXAngle, MAX_NUMBER_LENGTH, strYAngle, MAX_NUMBER_LENGTH, strZAngle, MAX_NUMBER_LENGTH);

	iIdentity = strtonum(strIdentity);
	
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

	if(strlen(strXAngle)==0) {
		selfmessage( "Unparsable format: no XAngle value found.");
		return PLUGIN_HANDLED;
	} 
	XAngle = strtonum(strXAngle);

	if(strlen(strYAngle)==0) {
		selfmessage( "Unparsable format: no YAngle value found.");
		return PLUGIN_HANDLED;
	} 
	YAngle = strtonum(strYAngle);

	if(strlen(strZAngle)==0) {
		selfmessage( "Unparsable format: no ZAngle value found.");
		return PLUGIN_HANDLED;
	} 
	ZAngle = strtonum(strZAngle);

	snprintf(Text, MAX_TEXT_LENGTH, "Attempting spawn move to X: %i, Y: %i, Z: %i, XAngle: %i, YAngle: %i, ZAngle: %i", X, Y, Z, XAngle, YAngle, ZAngle);
	selfmessage(Text);
	if(movespawn(iIdentity,X,Y,Z,XAngle,YAngle,ZAngle)==1) {
		selfmessage("Success.");
		say_command(User,Command,Data);
	} else {
		selfmessage("Failed.");
	}
	return PLUGIN_HANDLED;
}

/* Usage:

admin_removespawn (identity)

Removes a spawned item.  Use admin_listspawn to get identities.
*/
public admin_removespawn(HLCommand,HLData,HLUserName,UserIndex) {
	new iIdentity;
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	iIdentity = strtonum(Data);
	if(removespawn(iIdentity)==1) {
		say_command(User,Command,Data);
		selfmessage( "Success.");
	} else {
		selfmessage( "Failed.");
	}
	return PLUGIN_HANDLED;
}

/* Usage:

admin_spawn (thing) (X) (Y) (Z) (XAngle) (YAngle) (ZAngle)

Eg,

admin_spawn monster_miniturret -400 330 -40 0 0 0

Attempts to spawn (thing) at the coordinates, with the given angle.
*/
public admin_spawn(HLCommand,HLData,HLUserName,UserIndex) {
	new iIdentity;
	new X;
	new XAngle;
	new Y;
	new YAngle;
	new Z;
	new ZAngle;
	new strClass[MAX_NAME_LENGTH];
	new strX[MAX_NUMBER_LENGTH];
	new strXAngle[MAX_NUMBER_LENGTH];
	new strY[MAX_NUMBER_LENGTH];
	new strYAngle[MAX_NUMBER_LENGTH];
	new strZ[MAX_NUMBER_LENGTH];
	new strZAngle[MAX_NUMBER_LENGTH];
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	
	strgsep(Data, " ", "^"", strClass, MAX_NAME_LENGTH, strX, MAX_NUMBER_LENGTH, strY, MAX_NUMBER_LENGTH, strZ, MAX_NUMBER_LENGTH, strXAngle, MAX_NUMBER_LENGTH, strYAngle, MAX_NUMBER_LENGTH, strZAngle, MAX_NUMBER_LENGTH);

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

	if(strlen(strXAngle)==0) {
		selfmessage( "Unparsable format: no XAngle value found.");
		return PLUGIN_HANDLED;
	} 
	XAngle = strtonum(strXAngle);

	if(strlen(strYAngle)==0) {
		selfmessage( "Unparsable format: no YAngle value found.");
		return PLUGIN_HANDLED;
	} 
	YAngle = strtonum(strYAngle);

	if(strlen(strZAngle)==0) {
		selfmessage( "Unparsable format: no ZAngle value found.");
		return PLUGIN_HANDLED;
	} 
	ZAngle = strtonum(strZAngle);

	snprintf(Text, MAX_TEXT_LENGTH, "Attempting spawn at X: %i, Y: %i, Z: %i, XAngle: %i, YAngle: %i, ZAngle: %i", X, Y, Z, XAngle, YAngle, ZAngle);
	selfmessage(Text);
	iIdentity = spawn(strClass,X,Y,Z,XAngle,YAngle,ZAngle);
	if (iIdentity != 0) {
		snprintf(Text, MAX_TEXT_LENGTH, "Success.  Spawn created with ID %i.", iIdentity);
		selfmessage(Text);
		say_command(User,Command,Data);
	} else {
		selfmessage("Failed.");
	}
	return PLUGIN_HANDLED;
}

public plugin_init() {
	plugin_registerinfo("Admin Spawn Plugin","Commands for spawning things.",AM_VERSION_STRING);
	
	plugin_registercmd("admin_listspawn","admin_listspawn",ACCESS_SPAWN,"admin_listspawn: Lists all spawned entities.");
	plugin_registercmd("admin_movespawn","admin_movespawn",ACCESS_SPAWN,"admin_movespawn <identity> <X> <Y> <Z> <XAngle> <YAngle> <ZAngle>: Moves a spawned item.");
	plugin_registercmd("admin_removespawn","admin_removespawn",ACCESS_SPAWN,"admin_removespawn <identity>: Removes a spawned item.");
	plugin_registercmd("admin_spawn","admin_spawn",ACCESS_SPAWN,"admin_spawn <class> <X> <Y> <Z> <XAngle> <YAngle> <ZAngle>: Spawns a new item.");
	
	return PLUGIN_CONTINUE;
}
