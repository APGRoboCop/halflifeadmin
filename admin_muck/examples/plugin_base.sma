/*
 * Admin mod script. Copyright (C) 2000, Alfred Reynolds.
 * $Id: plugin_base.sma,v 1.16 2001/09/19 22:35:39 darope Exp $
 *
*/

/* This is a base plugin for Admin Mod; it doesn't do anything fancy. */

#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.2";

public ChangeMap(Timer,Repeat,HLUser,HLParam) {
	new NewMap[MAX_NAME_LENGTH];
	
	convert_string(HLParam,NewMap,MAX_NAME_LENGTH);
	if(strlen(NewMap) != 0) {
		changelevel(NewMap);
	}
}

public admin_ban(HLCommand,HLData,HLUserName,UserIndex) {
	new ban_user[MAX_DATA_LENGTH];
	new BanTime = 0;
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new strTime[MAX_NUMBER_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new User[MAX_NAME_LENGTH];
	
 	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strbreak(Data,ban_user,strTime, MAX_DATA_LENGTH);
	if(strlen(strTime) != 0) 
		BanTime = strtonum(strTime);	
	
	if (check_user(ban_user)==1) {
		get_username(ban_user,TargetName,MAX_NAME_LENGTH);
		say_command(User,Command,TargetName);
		if(check_immunity(ban_user)==1) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't ban '%s', you silly bear.", TargetName);
			say(Text);
		} else {
			ban(ban_user,BanTime);
		}
	} else {
		say_command(User,Command,ban_user);
		ban(ban_user,BanTime);
	}
	return PLUGIN_HANDLED;
}

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

public admin_chat(HLCommand,HLData,HLUserName,UserIndex) {
	new i;
	new maxplayers = maxplayercount();
	new Data[MAX_DATA_LENGTH];
	new Name[MAX_NAME_LENGTH];
	new SessionID;
	new Team;
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	new WONID;
	
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	snprintf(Text, MAX_TEXT_LENGTH, "(Admin) %s: %s", User, Data);
	for(i=1; i<=maxplayers; i++) {
		strinit(Name);
		if(playerinfo(i,Name,MAX_NAME_LENGTH,SessionID,WONID,Team)==1) {
			if(access(ACCESS_CHAT,Name)!=0) {
				messageex(Name, Text, print_chat);
			}
		}
	}
	log(Text);
	return PLUGIN_HANDLED;
}

public admin_csay(HLCommand,HLData,HLUserName,UserIndex) {
	new Color[MAX_DATA_LENGTH];
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Message[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	strbreak(Data,Color,Message,MAX_DATA_LENGTH);
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
	} else {
		centersay(Data,10,15,250,10);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

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
			say(Text);
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

public admin_listmaps(HLCommand,HLData,HLUserName,UserIndex) {
	new curmap[100];
	
	selfmessage("The maps on the mapcycle are:");
	list_maps();
	selfmessage("and the current map is:");
	currentmap(curmap,100);
	selfmessage(curmap);
	return PLUGIN_HANDLED;
}

public admin_map(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	new sTimelimit[10];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	getstrvar("mp_timelimit", sTimelimit, 10); 

	if (valid_map(Data)==1) {
		say_command(User,Command,Data);
		exec("mp_timelimit 1");
		set_serverinfo("config_mp_timelimit", sTimelimit);
		set_timer("ChangeMap",2,1,Data);
	} else {
		selfmessage("Bad map name");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

public admin_nextmap(HLCommand,HLData,HLUserName,UserIndex) {
	new strDummy[1];
	nextmap(strDummy,0);
	return PLUGIN_HANDLED;
}

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
	if (strmatch(Data, "rcon_password", strlen("rcon_password"))==1) {
		reject_message();
		return PLUGIN_HANDLED;
	}
	execute_command(User,Command,"",Data);
	return PLUGIN_HANDLED;
}

public admin_reload(HLCommand,HLData,HLUserName,UserIndex) {
	reload();
	return PLUGIN_HANDLED;
}

public admin_say(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);           

	snprintf(Text, MAX_TEXT_LENGTH, "Message from Admin (%s): %s", User, Data);
	say(Text);
	return PLUGIN_HANDLED;
}

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
public admin_ssay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);           
	say(Data);
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

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

public admin_timeleft(HLCommand,HLData,HLUserName,UserIndex) {
	timeleft();
	return PLUGIN_HANDLED;
}

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

public admin_tsay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	new Color[MAX_NAME_LENGTH];
	new Message[MAX_DATA_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

	strbreak(Data,Color,Message,MAX_DATA_LENGTH);

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
	} else {
		typesay(Data,10,220,220,220);
	}
	log_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

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

public admin_userlist(HLCommand,HLData,HLUserName,UserIndex) {
	new Data[MAX_DATA_LENGTH];
	
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	userlist(Data);
	return PLUGIN_HANDLED;
}

public admin_vote_kick(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	new WONID;
	new strWONID[MAX_NUMBER_LENGTH];

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
			say(Text);
		} else {
			snprintf(Text, MAX_TEXT_LENGTH, "Kick %s?", real_user);
			if(getvar("sv_lan")==1) {
				vote(Text,"Yes","No","HandleKickVote",real_user);
			} else {
				get_userWONID(real_user,WONID);
				numtostr(WONID,strWONID);
				vote(Text,"Yes","No","HandleKickVote",strWONID);
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
	
	if (WinningOption == 1) {
		new Ratio = getvar("kick_ratio");
		if (VoteCount >= Ratio*UserCount/100) {
			snprintf(Text, MAX_TEXT_LENGTH, "%s was kicked due to a vote.", VoteUser);
			say(Text);
			message(VoteUser,"You have been kicked due to a vote.");
			kick(VoteUser);
		} else {
			numtostr(Ratio*UserCount/100,strNumber);
			snprintf(Text, MAX_TEXT_LENGTH, "Kick vote succeeded, but not enough votes for kick (needed %s)",strNumber);
			say(Text);
		}
	} else {
		say("Kick vote failed.");
	}
}

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
		vote(Text,"Yes","No","HandleMapVote",Data);
	} else {
		selfmessage("Bad map name");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

/* Handle a map vote's results. */
public HandleMapVote(WinningOption,HLMap,VoteCount,UserCount) {
	new strNumber[MAX_NUMBER_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new VoteMap[MAX_DATA_LENGTH];
	convert_string(HLMap,VoteMap,MAX_DATA_LENGTH);
	new sTimelimit[10];
	getstrvar("mp_timelimit", sTimelimit, 10);
	
	if (WinningOption == 1) {
		new Ratio = getvar("map_ratio");
		if (VoteCount >= Ratio*UserCount/100) {
			snprintf(Text, MAX_TEXT_LENGTH, "Changing map to %s due to vote.", VoteMap);
			say(Text);
			exec("mp_timelimit 1");
			set_serverinfo( "config_mp_timelimit", sTimelimit );
			set_timer("ChangeMap",2,1,VoteMap);
		} else {
			numtostr(Ratio*UserCount/100,strNumber);
			snprintf(Text, MAX_TEXT_LENGTH, "Map vote succeeded, but not enough votes for change (needed %s)", strNumber);
			say(Text);
		}
	} else {
		say("Map vote failed.");
	}
}

public admin_vsay(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];

	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);

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

public plugin_init() {
	plugin_registerinfo("Admin Base Plugin","Basic Admin Mod commands.",STRING_VERSION);
	
	plugin_registercmd("admin_ban","admin_ban",ACCESS_BAN,"admin_ban <target or WONID or IP> [<minutes>]: Bans target. 0 minutes is a permanent ban.");
	plugin_registercmd("admin_cfg","admin_cfg",ACCESS_CONFIG,"admin_cfg <config file>: Executes config file on server.");
	plugin_registercmd("admin_chat","admin_chat",ACCESS_CHAT,"admin_chat <msg>: Shows message only to other admins.");
	plugin_registercmd("admin_csay","admin_csay",ACCESS_SAY,"admin_csay [color] <msg>: Shows message in center of screen.");
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
	
	new strMap[MAX_DATA_LENGTH];
	new ExecCommand[MAX_DATA_LENGTH];
	new sStoredTimelimit [10];		

	get_serverinfo ("config_mp_timelimit", sStoredTimelimit, 10);
	if (strlen(sStoredTimelimit ) > 0) {
	  snprintf(ExecCommand, MAX_DATA_LENGTH, "mp_timelimit %s", sStoredTimelimit);
	  exec (ExecCommand);
	  set_serverinfo ("config_mp_timelimit", "");
	}

	currentmap(strMap, MAX_DATA_LENGTH);
	snprintf(ExecCommand, MAX_DATA_LENGTH, "%s.cfg", strMap);
	if ( fileexists(ExecCommand) ) {
	  snprintf(ExecCommand, MAX_DATA_LENGTH, "exec %s.cfg", strMap);
	  log(ExecCommand);
	  exec(ExecCommand);
	}
	
	return PLUGIN_CONTINUE;
}
