/* This plugin will show a message to everyone who connects,
 * and will show another message every ten minutes. 
 */

/* $Id: plugin_logtest.sma,v 1.1 2001/04/29 08:50:09 magua Exp $ */

#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.0";

public plugin_log(HLLog) {
	new sLog[MAX_TEXT_LENGTH];
	
	convert_string(HLLog,sLog,MAX_TEXT_LENGTH);
	if (strcasestr(sLog, "ping") != -1) {
		say("Pong!");
	}
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Log Test Plugin","Totally useless testing plugin.",STRING_VERSION);
	return PLUGIN_CONTINUE;
}
