/*
 * ===========================================================================
 *
 * $Id: plugin_message.sma,v 1.3 2003/05/07 21:07:35 darope Exp $
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
 * This script will show a message to everyone who connects,
 * and will show another message every ten minutes. 
 *
 */


#include <core>
#include <string>
#include <admin>
#include <adminlib>

new AM_VERSION_STRING[] = "2.51.03";

public say_hello(Timer,Repeat,HLName,HLParam) {
	new User[MAX_NAME_LENGTH];
	new Msg[MAX_TEXT_LENGTH];
	
	getstrvar("admin_connect_msg",Msg,MAX_TEXT_LENGTH);
	if ( Msg[0] == '0' && Msg[1] == '^0' ) return;
	convert_string(HLName,User,MAX_NAME_LENGTH);
	messageex(User,Msg,print_center);
}

public say_stuff(Timer,Repeat,HLName,HLParam) {
	new Msg[MAX_TEXT_LENGTH];
	
	getstrvar("admin_repeat_msg",Msg,MAX_TEXT_LENGTH);
	if ( Msg[0] == '0' && Msg[1] == '^0' ) return;
	centersay(Msg,10,0,255,0);
}

public plugin_connect(HLUserName, HLIP, UserIndex) {
	set_timer("say_hello",45,0);
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Auto-Messaging Plugin","Shows message on connect, as well as a message every x minutes.",AM_VERSION_STRING);
	new iRepeatInterval = getvar("admin_repeat_freq");
	if ( iRepeatInterval >= 15 ) {
	  set_timer("say_stuff", iRepeatInterval, 99999);
	} 
	return PLUGIN_CONTINUE;
}

