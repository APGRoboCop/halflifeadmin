/*
 * $Id: plugin_message.sma,v 1.3 2001/09/27 20:33:16 darope Exp $
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
 * This plugin will show a message to everyone who connects,
 * and will show another message every ten minutes. 
 * 
 */


#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.0";

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

public plugin_connect(HLName,HLIP,UserIndex) {
	set_timer("say_hello",45,0);
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Auto-Messaging Plugin","Shows message on connect, as well as a message every x minutes.",STRING_VERSION);
	new iRepeatInterval = getvar("admin_repeat_freq");
	if ( iRepeatInterval >= 15 ) {
	  set_timer("say_stuff", iRepeatInterval, 99999);
	} 
	return PLUGIN_CONTINUE;
}
