/*
 * ===========================================================================
 *
 * $Id: plugin_fun.sma,v 1.4 2003/03/26 20:44:32 darope Exp $
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
 * A script for the silly 
 *
 */


#include <core>
#include <string>
#include <admin>
#include <adminlib>

#define ACCESS_FUN 8192

new AM_VERSION_STRING[] = "2.50.05";

new iDiscoCount = 0;
new iDiscoTimer = 0;

new iLastAYB = 0;

AllYourBaseAreBelongToUs(User[]) {
  new i;
  new iMaxPlayers = maxplayercount();
  new Name[MAX_NAME_LENGTH];

  if (systemtime() < iLastAYB) {
    messageex(User, "I think we should wait a little bit longer, don't you?", print_chat);
  } else {
    for (i = 1; i <= iMaxPlayers; i++) {
      if (playerinfo(i,Name,MAX_NAME_LENGTH) == 1) {
        speakto(Name, "(p96) all(t20) your(t20) base(t10) are(t20) be(t40) lock(e50) handling(s70) to us");
      }
    }
    
    set_timer("ayb1",5,1);
    set_timer("ayb2",25,1); 
    set_timer("ayb3",40,1); 
    iLastAYB = systemtime();
    iLastAYB += 600;
  }
}

public ayb1(Timer,Repeat,HLName,HLParam) { 
  centersay("In A.D. 2101^nWar was beginning.^nCaptain: What happen?^nMechanic: Somebody set up us the bomb.^nOperator: We get signal.^nCaptain: What!",15,255,255,255); 
} 

public ayb2(Timer,Repeat,HLName,HLParam) { 
  centersay("Operator: Main screen turn on.^nCaptain: It's You!!^nCats: How are you gentlemen !!^nCats: All your base are belong to us.^nCats: You are on the way to destruction.^nCaptain: What you say !!",15,255,255,255); 
} 

public ayb3(Timer,Repeat,HLName,HLParam) { 
  centersay("Cats: You have no chance to survive make your time.^nCats: HA HA HA HA ....^nCaptain: Take off every 'zig'!!^nCaptain: You know what you doing.^nCaptain: Move 'zig'.^nCaptain: For great justice.",15,255,255,255); 
} 

GlowHelper(User[], Color[]) {
	new iGoodColor = 1;
	
	if (streq(Color,"red")==1) {
		glow(User,250,10,10);
	} else if ( streq(Color, "blue")==1) {
		glow(User,10,10,250);
	} else if ( streq(Color, "green")==1) {
		glow(User,10,250,10);
	} else if ( streq(Color, "white")==1) {
		glow(User,250,250,250);
	} else if ( streq(Color, "yellow")==1) {
		glow(User,250,250,10);
	} else if ( streq(Color, "purple")==1) {
		glow(User,250,10,250);
	} else if ( streq(Color, "off")==1) {
		glow(User,0,0,0);
	} else {
		iGoodColor = 0;
	}
	return iGoodColor;
}

KillGlow() {
       new i;
       new iMaxPlayers = maxplayercount();
       new Name[MAX_NAME_LENGTH];
               
       for (i = 1; i <= iMaxPlayers; i++) {
               if(playerinfo(i,Name,MAX_NAME_LENGTH)==1) {
                   glow(Name,0,0,0);
               }
       }
}

KillDisco() {
	new i;
	new iMaxPlayers = maxplayercount();
	new Name[MAX_NAME_LENGTH];
		
	for (i = 1; i <= iMaxPlayers; i++) {
		if(playerinfo(i,Name,MAX_NAME_LENGTH)==1) {
			glow(Name,0,0,0);
		}
	}
	centersay("Disco . . . Is Dead.",10,0,255,0);
	kill_timer(iDiscoTimer);
	iDiscoCount = 0;
	iDiscoTimer = 0;
}

public DiscoInferno(Timer,Repeat,HLName,HLParam) {
	new i;
	new iDiscoMsg;
	new iMaxPlayers = maxplayercount();
	new Blue;
	new Green;
	new Name[MAX_NAME_LENGTH];
	new Red;
	
	if(getvar("admin_fun_mode")==0) {
		KillDisco();
	} else {
		for (i = 1; i <= iMaxPlayers; i++) {
			if(playerinfo(i,Name,MAX_NAME_LENGTH)==1) {
				Red = random(256);
				Green = random(256);
				Blue = random(256);
				glow(Name,Red,Green,Blue);
			}
		}
		if (iDiscoCount == 0) {
			iDiscoMsg = random(5);
			if (iDiscoMsg == 0) {
				centersay("Uh Uh Uh Uh . . . Staying Alive!",10,Red,Green,Blue);
			} else if (iDiscoMsg == 1) {
				centersay("Disco Inferno, Baby!",10,Red,Green,Blue);
			} else if (iDiscoMsg == 2) {
				centersay("Just Talkin' 'Bout Shaft!",10,Red,Green,Blue);
			} else if (iDiscoMsg == 3) {
				centersay("So . . . What's Your Sign?",10,Red,Green,Blue);
			} else {
				centersay("Ooga Shaka!",10,Red,Green,Blue);
			}
		}
		iDiscoCount++;
		if (iDiscoCount > 20)
			iDiscoCount = 0;
	}
}

/* admin_disco */
public admin_disco(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_NAME_LENGTH);
	convert_string(HLData,Data,MAX_NAME_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (getvar("admin_fun_mode")==0) {
		selfmessage("The discotheque can only be turned on if admin_fun_mode is 1.");
	} else if (iDiscoTimer == 0) {
		iDiscoTimer = set_timer("DiscoInferno",5,99999);
		selfmessage("Let the disco begin!");
	} else {
		KillDisco();
	}
	say_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

/* admin_fun <"on" | "off"> */
public admin_fun(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_NAME_LENGTH);
	convert_string(HLData,Data,MAX_NAME_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if(check_param(Data)==1) {
		execute_command(User,Command,"admin_fun_mode","1");
	} else {
		execute_command(User,Command,"admin_fun_mode","0");
		KillGlow();
	}
	return PLUGIN_HANDLED;
}

/* admin_glow <color | "off"> */
public admin_glow(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_NAME_LENGTH);
	convert_string(HLData,Data,MAX_NAME_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (GlowHelper(User,Data)==1) {
		selfmessage("Success.");
	} else {
		selfmessage("Unrecognized color.");
	}
	return PLUGIN_HANDLED;
}

public HandleSay(HLCommand,HLData,HLUserName,UserIndex) {
  new i;
  new Command[MAX_COMMAND_LENGTH];
  new Data[MAX_DATA_LENGTH];
  new Length;
  new strGlow[MAX_DATA_LENGTH];
  new Text[MAX_TEXT_LENGTH];
  new User[MAX_NAME_LENGTH];

  /* Ignore the console */
  if (UserIndex < 1) 
    return PLUGIN_CONTINUE;
    	
  convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
  convert_string(HLData,Data,MAX_DATA_LENGTH);
  convert_string(HLUserName,User,MAX_NAME_LENGTH);
  
  strstripquotes(Data);
  
  if (strmatch(Data, "glow ", strlen("glow "))==1) {
    if (getvar("admin_fun_mode") == 0) {
      messageex(User, "Glowing is only allowed during fun mode.", print_chat);
    } else {
			/* we need to strip out 'glow ' (5 characters */
			Length = strlen(Data);
			for(i=5;i<Length+1;i++)
				strGlow[i-5] = Data[i];
			strGlow[i-5] = NULL_CHAR;
			
			if (GlowHelper(User, strGlow) == 1) {
				if (streq(strGlow,"off")==1) {
			    	snprintf(Text, MAX_TEXT_LENGTH, "%s is no longer glowing.", User);
				} else {
			  		snprintf(Text, MAX_TEXT_LENGTH, "%s begins glowing %s.", User, strGlow);
			 	}
			  	say(Text);
			} else {
			  snprintf(Text, MAX_TEXT_LENGTH, "Sorry, %s, but I don't recognize that 'glow' color.", User);
			  messageex(User, Text, print_chat);
			}
			
  	}
  } else if (streq(Data, "movezig") == 1 && getvar("admin_fun_mode")==1) {
    AllYourBaseAreBelongToUs(User);
  }
  return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Fun Plugin","Does silly stuff.",AM_VERSION_STRING);

	plugin_registercmd("admin_disco","admin_disco",ACCESS_FUN,"admin_disco: Starts disco fever. Fun mode only.");	
	plugin_registercmd("admin_fun","admin_fun",ACCESS_FUN,"admin_fun <^"on^" | ^"off^">: Turns fun mode on or off.");	
	plugin_registercmd("admin_glow","admin_glow",ACCESS_FUN,"admin_glow <color | ^"off^">: Causes you to glow that color.");
	plugin_registercmd("say","HandleSay",ACCESS_ALL, "say glow <color | ^"off^">: Causes you to glow that color.  Fun mode only.");
	
	return PLUGIN_CONTINUE;
}
