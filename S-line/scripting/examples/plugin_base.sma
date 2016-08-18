/*
 * ===========================================================================
 *
 * $Id: plugin_base.sma,v 1.12 2004/07/23 21:30:47 darope Exp $
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
 * This is a base script for Admin Mod; it doesn't do anything fancy.
 * It provides some of the core commands for Admin Mod. 
 *
*/

#include <core>
#include <string>
#include <admin>
#include <adminlib>

new AM_VERSION_STRING[] = "2.50.13";
new g_AbortVote=0;
new g_VoteInProgress=0;

/* admin_ban <target or WONID or IP> [<minutes>] ['ip']
	 admin_banip <target or WONID or IP> [<minutes>] */
public admin_ban(HLCommand,HLData,HLUserName,UserIndex) {
	new ban_user[MAX_DATA_LENGTH];
	new BanTime = 0;
	new iBanType = bBanByID;
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new strTime[MAX_NUMBER_LENGTH];
	new strType[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	strbreak(Data,ban_user,strTime, MAX_DATA_LENGTH);
	strbreak(strTime, strTime, strType, MAX_DATA_LENGTH);

	if(strlen(strTime) != 0) BanTime = strtonum(strTime);

	if((strcasecmp(strType, "ip")==0) || (getvar("sv_lan") != 0) || (strcasecmp(Command, "admin_banip")==0)) iBanType = bBanByIP;

	if (check_user(ban_user)==1) {
		get_username(ban_user,TargetName,MAX_NAME_LENGTH);
		say_command(User,Command,TargetName);
		if(check_immunity(ban_user)==1) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't ban '%s', you silly bear.", TargetName);
			messageex(User,Text,print_chat);
		} else {
			ban(ban_user,BanTime,iBanType);
		}
	} else {
		say_command(User,Command,ban_user);
		ban(ban_user,BanTime,iBanType);
	}
	return PLUGIN_HANDLED;
}

/* admin_cfg <config file> */
public admin_cfg(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"exec",Data);
	return PLUGIN_HANDLED;
}

/* admin_chat <msg> */
public admin_chat(HLCommand,HLData,HLUserName,UserIndex) {
	new i;
	new maxplayers = maxplayercount();
	new Data[MAX_DATA_LENGTH];
	new Name[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strstripquotes(Data);
	snprintf(Text, MAX_TEXT_LENGTH, "(Admin) %s: %s", User, Data);
	for(i=1; i<=maxplayers; i++) {
		strinit(Name);
		if(playerinfo(i,Name,MAX_NAME_LENGTH)==1) {
			if(access(ACCESS_CHAT,Name)!=0) {
				messageex(Name, Text, print_chat);
			}
		}
	}
	log(Text);
	return PLUGIN_HANDLED;
}

/* admin_csay [color] <msg> */
public admin_csay(HLCommand,HLData,HLUserName,UserIndex) {
	new Color[MAX_NAME_LENGTH];
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Message[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	strgsep( Data, " ", "^"'", Color, MAX_NAME_LENGTH, Message, MAX_DATA_LENGTH );

	if (streq(Color,"red")==1) {
		centersay(Message,10,250,10,10);
	} else if ( streq(Color, "blue")==1) {
		centersay(Message,10,10,10,250);
	} else if ( streq(Color, "green")==1) {
		centersay(Message,10,10,250,10);
	} else if ( streq(Color, "white")==1) {
		centersay(Message,10,250,250,250);
	} else if ( streq(Color, "yellow")==1) {
		centersay(Message,10,250,250,10);
	} else if ( streq(Color, "purple")==1) {
		centersay(Message,10,250,10,250);
	} else if ( streq(Color, "random")==1) {
		centersay(Message,10,random(100),random(100),random(100));
	} else {
		strstripquotes(Data);
		centersay(Data,10,15,250,10);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

/* admin_dmesg <idtype> <userid> <message> */
public admin_dmesg( HLCommand, HLData, HLUserName, UserIndex ) {
	new sUser[MAX_NUMBER_LENGTH];
	new sType[10];
	new iUid;
	new uid:tType;
	new sMessage[MAX_TEXT_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iNumTokens;

	convert_string(HLData,Data,MAX_DATA_LENGTH);


	iNumTokens = strsep( Data, " ", sType, 10, sUser, MAX_NUMBER_LENGTH, sMessage, MAX_TEXT_LENGTH );
	if ( iNumTokens < 2) {
		directmessage( "Incorrect usage, arguments missing." );
		//printf( "num tokens: %d ^n", iNumTokens );
		return PLUGIN_CONTINUE;
	}	// if

	iUid = strtonum( sUser );
	switch( sType[0] ) {
	case 'i':
		tType = uid_index;
	case 's':
		tType = uid_sessionID;
	case 'w':
		tType = uid_wonID;
	default:
		tType = uid_invalid;
	}	// switch()


	strstripquotes(sMessage);
	directmessage( sMessage, iUid, tType );

	return PLUGIN_CONTINUE;
}	// admin_dmesg()

/* admin_fraglimit <fraglimit> */
public admin_fraglimit(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"mp_fraglimit",Data);
	return PLUGIN_HANDLED;
}

/* admin_friendlyfire <friendlyfire> */
public admin_friendlyfire(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"mp_friendlyfire",Data);
	return PLUGIN_HANDLED;
}

/* admin_gravity <gravity> */
public admin_gravity(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"sv_gravity",Data);
	return PLUGIN_HANDLED;
}

/* admin_hostname <name> */
public admin_hostname(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new sHostName[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	snprintf(sHostName, MAX_DATA_LENGTH, "^"%s^"", Data);
	execute_command(User,Command,"hostname",sHostName);
	return PLUGIN_HANDLED;
}

/* admin_kick <target> [<reason>] */
public admin_kick(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new kick_user[MAX_DATA_LENGTH];
	new Reason[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strbreak(Data,kick_user,Reason,MAX_DATA_LENGTH);
	if ( check_user(kick_user) == 1) {
		new real_user[MAX_NAME_LENGTH];
		get_username(kick_user,real_user,MAX_NAME_LENGTH);
		say_command(User,Command,real_user);
		if(check_immunity(kick_user)!=0) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't kick '%s', you silly bear.", real_user);
			messageex(User, Text, print_chat);
		} else {
			if (strlen(Reason) != 0) {
				snprintf(Text, MAX_TEXT_LENGTH, "You have been kicked because %s", Reason);
				message(real_user, Text);
			}
			kick(real_user);
		}
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(kick_user);
	}
	return PLUGIN_HANDLED;
}

/* admin_listmaps */
public admin_listmaps(HLCommand,HLData,HLUserName,UserIndex) {
	new curmap[100];

	selfmessage("The maps on the mapcycle are:");
	list_maps();
	selfmessage("and the current map is:");
	currentmap(curmap,100);
	selfmessage(curmap);
	return PLUGIN_HANDLED;
}

/* admin_map <map> */
public admin_map(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	if (valid_map(Data)==1) {
		say_command(User,Command,Data);
		changelevel(Data, 4);
		//set_timer("ChangeMap",4,0,Data);
	} else {
		selfmessage("Bad map name");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

/* admin_nextmap */
public admin_nextmap(HLCommand,HLData,HLUserName,UserIndex) {
	new strDummy[1];
	nextmap(strDummy,0);
	return PLUGIN_HANDLED;
}

/* admin_pass <password>
	 admin_nopass */
public admin_pass(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Msg[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];

	/* Don't use execute_command here, unless you want your password to
	be publicly broadcast... instead we use our own message */
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (streq(Command, "admin_nopass")==1) {
		strcpy(Data,"^"^"",MAX_DATA_LENGTH);
	} else {
		convert_string(HLData,Data,MAX_DATA_LENGTH);
		/* If they didn't supply a password, show them the current one. */
		if (strlen(Data) == 0 || streq(Data, "^"^"") == 1) {
			new sPasswd[MAX_NAME_LENGTH];
			getstrvar( "sv_password", sPasswd, MAX_NAME_LENGTH );
			snprintf(Text, MAX_TEXT_LENGTH, "The password is: %s.", sPasswd);
			selfmessage(Text);
			return PLUGIN_HANDLED;
		}
	}
	if (streq(Data,"^"^"")) {
		snprintf(Text, MAX_TEXT_LENGTH, "ADMIN Command: %s has removed the server's password.", User);
	} else {
		snprintf(Text, MAX_TEXT_LENGTH, "ADMIN Command: %s has set the server's password.", User);
	}
	say(Text);

	log_command(User,Command,Data);
	snprintf(Msg, MAX_DATA_LENGTH, "sv_password %s", Data);
	exec(Msg);
	return PLUGIN_HANDLED;
}

/* admin_pause
	 admin_unpause */
public admin_pause(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (streq(Command, "admin_unpause")==1) {
		execute_command(User,Command,"pausable","0");
	} else {
		execute_command(User,Command,"pausable","1");
	}
	return PLUGIN_HANDLED;
}

/* admin_psay <target> <msg> */
public admin_psay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iMsgToAdmin = 0;
	new Msg[MAX_TEXT_LENGTH];
	new Target[MAX_TEXT_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strbreak(Data, Target, Msg, MAX_TEXT_LENGTH);
	if (strlen(Msg) == 0) {
		selfmessage( "Unparsable format: no message found.");
		return PLUGIN_HANDLED;
	} else if (check_user(Target) == 0) {
		if (streq(Target, "admin")==1) {
			iMsgToAdmin = 1;
		} else {
			selfmessage("Unrecognized player: ");
			selfmessage(Target);
			return PLUGIN_HANDLED;
		}
	}
	if (iMsgToAdmin == 1) {
		strcpy(TargetName, "Admin", MAX_NAME_LENGTH);
	} else {
		get_username(Target,TargetName,MAX_NAME_LENGTH);
	}

	snprintf(Text, MAX_TEXT_LENGTH, "(Private Msg From %s): %s", User, Msg);
	if (iMsgToAdmin == 1) {
		log(Text);
	} else {
		messageex(TargetName, Text, print_chat);
	}
	snprintf(Text, MAX_TEXT_LENGTH, "(Private Msg To %s): %s", TargetName, Msg);
	if (UserIndex == 0) {
		log(Text);
	} else {
		messageex(User, Text, print_chat);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

/* admin_rcon <cmd> */
public admin_rcon(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	if (check_auth(ACCESS_RCON)==0) {
		selfmessage("Laf. Silly bear.");
		return PLUGIN_HANDLED;
	}

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (strstr(Data, "rcon_password") >= 0) {
		reject_message();
		return PLUGIN_HANDLED;
	}
	execute_command(User,Command,"",Data);
	return PLUGIN_HANDLED;
}

/* admin_reload */
public admin_reload(HLCommand,HLData,HLUserName,UserIndex) {
	reload();
	return PLUGIN_HANDLED;
}

/* admin_say <msg> */
public admin_say(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strstripquotes(Data);

	snprintf(Text, MAX_TEXT_LENGTH, "Message from Admin (%s): %s", User, Data);
	say(Text);
	return PLUGIN_HANDLED;
}

/* admin_servercfg <config file> */
public admin_servercfg(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"servercfgfile",Data);
	return PLUGIN_HANDLED;
}

/* Silent say: does an admin_say, but without showing the user name; exactly as if you did a
'say' from console, or via rcon. */
/* admin_ssay <msg> */
public admin_ssay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strstripquotes(Data);
	say(Data);
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

/* admin_teamplay <teamplay> */
public admin_teamplay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"mp_teamplay",Data);
	return PLUGIN_HANDLED;
}


/* admin_timeleft */
public admin_timeleft(HLCommand,HLData,HLUserName,UserIndex) {
	timeleft();
	return PLUGIN_HANDLED;
}

/* admin_timelimit <timelimit> */
public admin_timelimit(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	execute_command(User,Command,"mp_timelimit",Data);
	return PLUGIN_HANDLED;
}

/* admin_tsay [color] <msg> */
public admin_tsay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	new Color[MAX_NAME_LENGTH];
	new Message[MAX_DATA_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	strgsep( Data, " ", "^"'", Color, MAX_NAME_LENGTH, Message, MAX_DATA_LENGTH );

	if (streq(Color,"red")==1) {
		typesay(Message,10,250,10,10);
	} else if ( streq(Color, "blue")==1) {
		typesay(Message,10,10,10,250);
	} else if ( streq(Color, "green")==1) {
		typesay(Message,10,10,250,10);
	} else if ( streq(Color, "white")==1) {
		typesay(Message,10,250,250,250);
	} else if ( streq(Color, "yellow")==1) {
		typesay(Message,10,250,250,10);
	} else if ( streq(Color, "purple")==1) {
		typesay(Message,10,250,10,250);
	} else if ( streq(Color, "random")==1) {
		typesay(Message,10,random(100),random(100),random(100));
	} else {
		strstripquotes(Data);
		typesay(Data,10,220,220,220);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

/* admin_unban <WONID or IP> */
public admin_unban(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	unban(Data);
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

/* admin_userlist [<name>] */
public admin_userlist(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];

	convert_string(HLData,Data,MAX_DATA_LENGTH);
	userlist(Data);
	return PLUGIN_HANDLED;
}

/* admin_vote_kick <target> */
public admin_vote_kick(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	new sAuthID[MAX_AUTHID_LENGTH];

	if (vote_allowed()!=1) {
		selfmessage( "Vote not allowed at this time.");
		return PLUGIN_HANDLED;
	}

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (check_user(Data) == 1) {
		new real_user[MAX_NAME_LENGTH];
		get_username(Data,real_user,MAX_NAME_LENGTH);
		say_command(User,Command,Data);
		if(check_immunity(real_user)!=0) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't kick %s, you silly bear.", real_user);
			messageex(User, Text, print_chat);
		} else {
			snprintf(Text, MAX_TEXT_LENGTH, "Kick %s?", real_user);
			if(getvar("sv_lan")==1) {
				if (vote(Text,"Yes","No","HandleKickVote",real_user)) {
					g_VoteInProgress = 1;
					g_AbortVote = 0;
				}
			} else {
				get_userAuthID(real_user,sAuthID);
				if (vote(Text,"Yes","No","HandleKickVote",sAuthID)) {
					g_VoteInProgress = 1;
					g_AbortVote = 0;
				}
			}
		}
	} else {
		selfmessage("Unrecognized user name ");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

/* Handle a kick vote's results. */
public HandleKickVote(WinningOption,HLUser,VoteCount,UserCount) {
	new strNumber[MAX_NUMBER_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new VoteUser[MAX_DATA_LENGTH];
	convert_string(HLUser,VoteUser,MAX_DATA_LENGTH);

	g_VoteInProgress = 0;
	if (WinningOption == 1) {
		new Ratio = getvar("kick_ratio");
		if (VoteCount >= Ratio*UserCount/100) {
			if (g_AbortVote) {
				say("Kick vote was aborted by an admin");
			} else {
				set_timer("KickUser",10,1,VoteUser);
			}
		} else {
			numtostr(Ratio*UserCount/100,strNumber);
			snprintf(Text, MAX_TEXT_LENGTH, "Kick vote succeeded, but not enough votes for kick (needed %s)",strNumber);
			say(Text);
		}
	} else {
		say("Kick vote failed.");
	}
}

/* Actually kick the user, unless aborted. */
public KickUser(Timer,Repeat,HLUserName,HLParam) {
	new VoteUser[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	convert_string(HLParam,VoteUser,MAX_DATA_LENGTH);

	if (g_AbortVote) {
		say("Kick vote was Aborted by an admin");
	}
	else {
		snprintf(Text, MAX_TEXT_LENGTH, "%s was kicked due to a vote.", VoteUser);
		say(Text);
		message(VoteUser,"You have been kicked due to a vote.");
		kick(VoteUser);
	}
}

/* admin_vote_map <map> */
public admin_vote_map(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];

	if (vote_allowed()!=1) {
		selfmessage( "Vote not allowed at this time.");
		return PLUGIN_HANDLED;
	}

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (valid_map(Data) == 1) {
		snprintf(Text, MAX_TEXT_LENGTH, "Change map to %s?", Data);
		log(Text);
		if (vote(Text,"Yes","No","HandleMapVote",Data)) {
			g_VoteInProgress = 1;
			g_AbortVote=0;
		}

	} else {
		snprintf(Text, MAX_TEXT_LENGTH, "You cannot vote for map %s on this server",Data);
		selfmessage(Text);
	}
	return PLUGIN_HANDLED;
}

/* Handle a map vote's results. */
public HandleMapVote(WinningOption,HLMap,VoteCount,UserCount) {
	new strNumber[MAX_NUMBER_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new VoteMap[MAX_DATA_LENGTH];
	convert_string(HLMap,VoteMap,MAX_DATA_LENGTH);
	g_VoteInProgress = 0;

	if (WinningOption == 1) {
		new Ratio = getvar("map_ratio");
		if (VoteCount >= Ratio*UserCount/100) {
			if (g_AbortVote) {
				say("Map vote was aborted by an admin");
			} else {
				set_timer("VoteChangeMap",10,1,VoteMap);
			}
		} else {
			numtostr(Ratio*UserCount/100,strNumber);
			snprintf(Text, MAX_TEXT_LENGTH, "Map vote succeeded, but not enough votes for change (needed %s)", strNumber);
			say(Text);
		}
	} else {
		say("Map vote failed.");
	}
}

/* Actually kick the user, unless aborted. */
public VoteChangeMap(Timer,Repeat,HLUserName,HLParam) {
	new Text[MAX_TEXT_LENGTH];
	new VoteMap[MAX_DATA_LENGTH];
	convert_string(HLParam,VoteMap,MAX_DATA_LENGTH);

	if (g_AbortVote) {
		say("Map vote aborted by an admin");
	}
	else {
		snprintf(Text, MAX_TEXT_LENGTH, "Changing map to %s due to vote.", VoteMap);
		say(Text);
		changelevel(VoteMap, 4);
	}
}

/* admin_vsay <question> */
public admin_vsay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strstripquotes(Data);

	if (vote_allowed()!=1) {
		selfmessage( "Vote not allowed at this time.");
		return PLUGIN_HANDLED;
	}

	log_command(User,Command,Data);
	vote(Data,"Yes","No","HandleVsay", Data);
	return PLUGIN_HANDLED;
}

public HandleVsay(WinningOption,HLData,VoteCount,UserCount) {
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];

	convert_string(HLData,Data,MAX_DATA_LENGTH);
	snprintf(Text, MAX_TEXT_LENGTH, "^"%s^": Option #%i won with %i / %i votes.", Data, WinningOption, VoteCount, UserCount);
	say(Text);
}

/* Allows a vote to be Aborted */
public admin_abort_vote(HLCommand,HLData,HLUserName,UserIndex) {
	if (g_VoteInProgress) {
		g_AbortVote=1;
		selfmessage("The vote in progress will be aborted.");
	}
	else {
		selfmessage("There are no votes in progress that can be aborted.");
	}
	return PLUGIN_HANDLED;
}

/* admin_motd <msg> */
public admin_motd(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[1550];
	new Msg[1500];
	new Target[MAX_TEXT_LENGTH];
	new TargetName[MAX_NAME_LENGTH];

	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	strbreak(Data, Target, Msg, MAX_TEXT_LENGTH);
	if (strlen(Msg) == 0) {
		selfmessage( "Unparsable format: no message found.");
		return PLUGIN_HANDLED;
	} else if (check_user(Target) == 0) {
		selfmessage("Unrecognized player: ");
		selfmessage(Target);
		return PLUGIN_HANDLED;
	}
	get_username(Target,TargetName,MAX_NAME_LENGTH);

	motd( TargetName, Msg );

	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}


public plugin_init() {
	plugin_registerinfo("Admin Base Plugin","Basic Admin Mod commands.",AM_VERSION_STRING);

	plugin_registercmd("admin_ban","admin_ban",ACCESS_BAN,"admin_ban <target or WONID or IP> [<minutes>] ['ip']: Bans target. 0 minutes is a permanent ban.");
	plugin_registercmd("admin_banip","admin_ban",ACCESS_BAN,"admin_banip <target or WONID or IP> [<minutes>]: Bans targets ip address. 0 minutes is a permanent ban. ");
	plugin_registercmd("admin_cfg","admin_cfg",ACCESS_CONFIG,"admin_cfg <config file>: Executes config file on server.");
	plugin_registercmd("admin_chat","admin_chat",ACCESS_CHAT,"admin_chat <msg>: Shows message only to other admins.");
	plugin_registercmd("admin_csay","admin_csay",ACCESS_SAY,"admin_csay [color] <msg>: Shows message in center of screen.");
	plugin_registercmd("admin_dmesg","admin_dmesg",ACCESS_CONFIG,"admin_dmesg <idtype> <userid> <message>: Direct message");
	plugin_registercmd("admin_fraglimit","admin_fraglimit",ACCESS_FRAGLIMIT,"admin_fraglimit <fraglimit>: Sets the mp_fraglimit cvar.");
	plugin_registercmd("admin_friendlyfire","admin_friendlyfire",ACCESS_FRIENDLY_FIRE,"admin_friendlyfire <friendlyfire>: Sets the mp_friendlyfire cvar.");
	plugin_registercmd("admin_gravity","admin_gravity",ACCESS_GRAVITY,"admin_gravity <gravity>: Sets the sv_gravity cvar.");
	plugin_registercmd("admin_hostname","admin_hostname",ACCESS_CONFIG,"admin_hostname <name>: Sets the hostname cvar.");
	plugin_registercmd("admin_kick","admin_kick",ACCESS_KICK,"admin_kick <target> [<reason>]: Kicks target.");
	plugin_registercmd("admin_listmaps","admin_listmaps",ACCESS_ALL,"admin_listmaps: Shows maps in mapcycle.");
	plugin_registercmd("admin_map","admin_map",ACCESS_MAP,"admin_map <map>: Changes map.");
	plugin_registercmd("admin_nextmap","admin_nextmap",ACCESS_ALL,"admin_nextmap: Shows next map in cycle.");
	plugin_registercmd("admin_nopass","admin_pass",ACCESS_PASS,"admin_nopass: Clears the server's password.");
	plugin_registercmd("admin_pass","admin_pass",ACCESS_PASS,"admin_pass <password>: Sets the server's password.");
	plugin_registercmd("admin_pause","admin_pause",ACCESS_PAUSE,"admin_pause: Sets the pausable cvar to 1.");
	plugin_registercmd("admin_psay","admin_psay",ACCESS_SAY,"admin_psay <target> <msg>: Sends a private msg to target.");
	plugin_registercmd("admin_rcon","admin_rcon",ACCESS_RCON,"admin_rcon <cmd>: Executes rcon command.");
	plugin_registercmd("admin_reload","admin_reload",ACCESS_RELOAD,"admin_reload: Reloads Admin Mod files.");
	plugin_registercmd("admin_say","admin_say",ACCESS_SAY,"admin_say <msg>: Shows a message from you as admin.");
	plugin_registercmd("admin_servercfg","admin_servercfg",ACCESS_CONFIG,"admin_servercfg <config file>: Sets the config file as the server's default.");
	plugin_registercmd("admin_ssay","admin_ssay",ACCESS_SAY,"admin_ssay <msg>: Shows a message admin without identification.");
	plugin_registercmd("admin_teamplay","admin_teamplay",ACCESS_TEAMPLAY,"admin_teamplay <teamplay>: Sets the mp_teamplay cvar.");
	plugin_registercmd("admin_timeleft","admin_timeleft",ACCESS_TIMELEFT,"admin_timeleft: Shows the time left.");
	plugin_registercmd("admin_timelimit","admin_timelimit",ACCESS_TIMELIMIT,"admin_timelimit <timelimit>: Sets the mp_timelimit cvar.");
	plugin_registercmd("admin_tsay","admin_tsay",ACCESS_SAY,"admin_tsay [color] <msg>: Prints msg on lower left of screen.");
	plugin_registercmd("admin_unban","admin_unban",ACCESS_BAN,"admin_unban <WONID or IP>: Unbans target.");
	plugin_registercmd("admin_unpause","admin_pause",ACCESS_PAUSE,"admin_unpause: Sets the pausable cvar to 0.");
	plugin_registercmd("admin_userlist","admin_userlist",ACCESS_USERLIST,"admin_userlist [<name>]: Shows a list of users.");
	plugin_registercmd("admin_vote_kick","admin_vote_kick",ACCESS_VOTE_KICK,"admin_vote_kick <target>: Starts a vote to kick target.");
	plugin_registercmd("admin_vote_map","admin_vote_map",ACCESS_VOTE_MAP,"admin_vote_map <map>: Starts a vote to change the map.");
	plugin_registercmd("admin_vsay","admin_vsay",ACCESS_SAY,"admin_vsay <question>: Presents question as a vote.");
	plugin_registercmd("admin_abort_vote","admin_abort_vote",ACCESS_ABORT_VOTE,"admin_abort_vote: Aborts a vote in progress.");
	plugin_registercmd("admin_motd","admin_motd",ACCESS_SAY,"admin_motd: Displays a MOTD style window on the client with a msg of your choice.");

	new strMap[MAX_DATA_LENGTH];
	new ExecCommand[MAX_DATA_LENGTH];

	currentmap(strMap, MAX_DATA_LENGTH);
	snprintf(ExecCommand, MAX_DATA_LENGTH, "%s.cfg", strMap);
	if ( fileexists(ExecCommand) ) {
		snprintf(ExecCommand, MAX_DATA_LENGTH, "exec %s.cfg", strMap);
		log(ExecCommand);
		exec(ExecCommand);
	}

	return PLUGIN_CONTINUE;
}
