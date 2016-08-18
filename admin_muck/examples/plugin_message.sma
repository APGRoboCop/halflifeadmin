/* This plugin will show a message to everyone who connects,
 * and will show another message every ten minutes. 
 */

/* $Id: plugin_message.sma,v 1.4 2001/06/02 18:30:37 darope Exp $ */

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
