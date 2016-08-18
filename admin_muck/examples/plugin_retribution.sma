/* This plugin contains commands for getting back at llamas, 
 * cheaters, and the like.
 */

/* $Id: plugin_retribution.sma,v 1.11 2001/06/09 02:43:16 darope Exp $ */

#include <core>
#include <console>
#include <string>
#include <admin>
#include <adminlib>

#define ACCESS_SLAP 128
#define ACCESS_SLAY 128
#define ACCESS_GAG 2048
#define ACCESS_EXECCLIENT 65536
#define ACCESS_LLAMA 8192

#define FLAG_INVALID 0
#define FLAG_LLAMA 1
#define FLAG_IGNORE_NAME 2

#define GAG_INVALID -1

new UserFlags[MAX_PLAYERS] = {FLAG_INVALID,...};
new UserGagTime[MAX_PLAYERS] = {GAG_INVALID,...};

new LastSlappedName[MAX_NAME_LENGTH];
new LastSlapperIndex = 0;

new STRING_VERSION[MAX_DATA_LENGTH] = "2.50.0";

AddUserFlag(UserIndex, Flag) {
	new Name[MAX_NAME_LENGTH];
	new strWONID[MAX_NUMBER_LENGTH];
	new UserID;
	new VaultData[MAX_DATA_LENGTH];
	new WONID;
	
	UserFlags[UserIndex] = UserFlags[UserIndex] | Flag;
	if (Flag == FLAG_LLAMA && getvar("sv_lan") == 0) {
		if (playerinfo(UserIndex,Name,MAX_NAME_LENGTH,UserID,WONID) != 0) {
			numtostr(WONID,strWONID);
			if(get_vaultdata(strWONID,VaultData,MAX_DATA_LENGTH) != 0) {
				if (strcasestr(VaultData," llama") == -1) {
					strcat(VaultData," llama", MAX_DATA_LENGTH);
					set_vaultdata(strWONID,VaultData);
				}
			} else {
				strcpy(VaultData, " llama", MAX_DATA_LENGTH);				
				set_vaultdata(strWONID,VaultData);
			}
		}
	}
}

CheckVaultFlags(UserIndex) {
	new Name[MAX_NAME_LENGTH];
	new strWONID[MAX_NUMBER_LENGTH];
	new UserID;
	new VaultData[MAX_DATA_LENGTH];
	new WONID;

	if (getvar("sv_lan") == 0) {
		if (playerinfo(UserIndex,Name,MAX_NAME_LENGTH,UserID,WONID) != 0) {
			numtostr(WONID,strWONID);
			if(get_vaultdata(strWONID,VaultData,MAX_DATA_LENGTH) != 0) {
				if (strcasestr(VaultData," llama") != -1) {
					UserFlags[UserIndex] = UserFlags[UserIndex] | FLAG_LLAMA;
				}
			}
		}
	}
}

PlayThunderclap() {
  new i;
  new iMaxPlayers = maxplayercount();
  new Name[MAX_NAME_LENGTH];
  new UserID;
  new WONID;

  if (getvar("admin_fx") != 0) {  
    for (i = 1; i <= iMaxPlayers; i++) {
  		if (playerinfo(i,Name,MAX_NAME_LENGTH,UserID,WONID) != 0) {
        playsound(Name, "ambience/thunder_clap.wav");
      }
    }
  }
}

RemoveUserFlag(UserIndex, Flag) {
	new Name[MAX_NAME_LENGTH];
	new strWONID[MAX_NUMBER_LENGTH];
	new UserID;
	new VaultData[MAX_DATA_LENGTH];
	new WONID;

	UserFlags[UserIndex] = UserFlags[UserIndex] & ~Flag;
	if (Flag == FLAG_LLAMA && getvar("sv_lan") == 0) {
		if (playerinfo(UserIndex,Name,MAX_NAME_LENGTH,UserID,WONID) != 0) {
			numtostr(WONID,strWONID);
			if(get_vaultdata(strWONID,VaultData,MAX_DATA_LENGTH) != 0) {
				if (strcasestr(VaultData," llama") != -1) {
					strsubst(VaultData," llama", "", MAX_DATA_LENGTH);
					set_vaultdata(strWONID,VaultData);
				}
			}
		}
	}
}

public admin_execall(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new i;
	new maxplayers = maxplayercount();
	new SessionID;
	new Target[MAX_NAME_LENGTH];
	new Team;
	new User[MAX_NAME_LENGTH];
	new WONID;
		
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	for(i=1; i<=maxplayers; i++) {
		strinit(Target);
		if(playerinfo(i,Target,MAX_NAME_LENGTH,SessionID,WONID,Team)==1) {
			if(check_immunity(Target)==0 && i != UserIndex) {
				execclient(Target,Data);
			}
		}
	}
	say_command(User,Command,Data);
	return PLUGIN_HANDLED;
}

public admin_execclient(HLCommand,HLData,HLUserName,UserIndex) {
	new Cmd[MAX_DATA_LENGTH];
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new Target[MAX_NAME_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strbreak(Data, Target, Cmd, MAX_TEXT_LENGTH);
	if (strlen(Cmd) == 0) {
		selfmessage( "Unparsable format: no command found.");
		return PLUGIN_HANDLED;
	} else if(check_user(Target)==0) {
		selfmessage("Unrecognized player: ");
		selfmessage(Target);
		return PLUGIN_HANDLED;
	}
	get_username(Target,TargetName,MAX_NAME_LENGTH);
	say_command(User,Command,TargetName);
	if(check_immunity(TargetName)!=0) {
		snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't control %s, you silly bear.", TargetName);
		say(Text);
	} else {
		if (execclient(TargetName,Cmd)==0) {
			selfmessage("Failed.");
		} else {
			selfmessage("Succeeded.");
		}
	}
	return PLUGIN_HANDLED;
}

public admin_execteam(HLCommand,HLData,HLUserName,UserIndex) {
	new Cmd[MAX_TEXT_LENGTH];
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new i;
	new maxplayers = maxplayercount();
	new SessionID;
	new ExecTeam = 0;
	new strTeam[MAX_TEXT_LENGTH];
	new Target[MAX_NAME_LENGTH];
	new Team;
	new User[MAX_NAME_LENGTH];
	new WONID;
		
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strbreak(Data, strTeam, Cmd, MAX_TEXT_LENGTH);
	if (strlen(Cmd) == 0) {
		selfmessage( "Unparsable format: no command found.");
		return PLUGIN_HANDLED;
	}
	ExecTeam = strtonum(strTeam);

	if(ExecTeam > 0) {
		for(i=1; i<=maxplayers; i++) {
			strinit(Target);
			if(playerinfo(i,Target,MAX_NAME_LENGTH,SessionID,WONID,Team)==1) {
				if(Team==ExecTeam && i != UserIndex) {
					if(check_immunity(Target)==0) {
						execclient(Target,Cmd);
					}
				}
			}
		}
		say_command(User,Command,Data);
	} else {
		selfmessage("The team to exec must be a number.");
	}
	return PLUGIN_HANDLED;
}

public admin_gag(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new GagTime = 0;
	new iIndex;
	new Target[MAX_NAME_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new strTime[MAX_DATA_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	strbreak(Data, Target, strTime, MAX_TEXT_LENGTH);
	if (strlen(strTime) != 0) {
		new Time = systemtime();
		GagTime = strtonum(strTime) * 60;
		GagTime += Time;
	}

	if(check_user(Target)==0) {
		selfmessage( "Unrecognized player: ");
		selfmessage( Target);
		return PLUGIN_HANDLED;
	}
	get_username(Target,TargetName,MAX_NAME_LENGTH);
	if(check_immunity(TargetName)!=0) {
		snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't gag %s, you silly bear.", TargetName);
		say(Text);
	} else {
		get_userindex(TargetName, iIndex);
		RemoveUserFlag(iIndex, FLAG_IGNORE_NAME);
		UserGagTime[iIndex] = GagTime;
	}
	return PLUGIN_HANDLED;
}

public admin_llama(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iIndex;
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (check_user(Data) == 1) {
		get_username(Data,TargetName,MAX_NAME_LENGTH);
		say_command(User,Command,TargetName);
		if(check_immunity(TargetName)!=0) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't transform %s into a llama, you silly bear.", TargetName);
			say(Text);
		} else {
			get_userindex(TargetName, iIndex);
			AddUserFlag(iIndex, FLAG_LLAMA);
			execclient(TargetName, "name Llama");
		}
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

public admin_slap(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (check_user(Data) == 1) {
		get_username(Data,TargetName,MAX_NAME_LENGTH);
		if (UserIndex != LastSlapperIndex || streq(LastSlappedName, TargetName) == 0) {
		  say_command(User,Command,TargetName);
		}
		if(check_immunity(TargetName)!=0) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't slap %s, you silly bear.", TargetName);
			say(Text);
		} else {				
			slap(TargetName);
		}
		LastSlapperIndex = UserIndex;
		strcpy(LastSlappedName, TargetName, MAX_NAME_LENGTH);
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

public admin_slay(HLCommand,HLData,HLUserName,UserIndex) {
  new Command[MAX_COMMAND_LENGTH];
  new Data[MAX_DATA_LENGTH];
  new TargetName[MAX_NAME_LENGTH];
  new Text[MAX_TEXT_LENGTH];
  new User[MAX_NAME_LENGTH];
  
  convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
  convert_string(HLData,Data,MAX_DATA_LENGTH);
  convert_string(HLUserName,User,MAX_NAME_LENGTH);
  if (check_user(Data) == 1) {
    get_username(Data,TargetName,MAX_NAME_LENGTH);
    if(check_immunity(TargetName)!=0) {
      snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't slay %s, you silly bear.", TargetName);
      say(Text);
    } else {
      if ( slay(TargetName) ) {
	PlayThunderclap();
    /* Since we have our own unique message, we have to handle
       admin_quiet ourselves. */
	if (getvar("admin_quiet") == 0) {
	  snprintf(Text, MAX_TEXT_LENGTH, "[ADMIN] %s was struck down by %s's wrath.", TargetName, User);
	  say(Text);
	} else if (getvar("admin_quiet") == 1) {
	  snprintf(Text, MAX_TEXT_LENGTH, "[ADMIN] %s was struck down by the admin's wrath.", TargetName);
	  say(Text);
	} else {
	  log_command(User, Command, Data);
	}
      }
    }
  } else {
    selfmessage("Unrecognized player: ");
    selfmessage(Data);
  }
  return PLUGIN_HANDLED;
}

public admin_slayteam(HLCommand,HLData,HLUserName,UserIndex) {
  new Command[MAX_COMMAND_LENGTH];
  new Data[MAX_DATA_LENGTH];
  new i;
  new maxplayers = maxplayercount();
  new SessionID;
  new SlayTeam;
  new TargetName[MAX_NAME_LENGTH];
  new Team;
  new Text[MAX_TEXT_LENGTH];
  new User[MAX_NAME_LENGTH];
  new WONID;
  
  convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
  convert_string(HLData,Data,MAX_DATA_LENGTH);
  convert_string(HLUserName,User,MAX_NAME_LENGTH);
  SlayTeam = strtonum(Data);	
  if(SlayTeam > 0) {
    for(i=1; i<=maxplayers; i++) {
      strinit(TargetName);
      if(playerinfo(i,TargetName,MAX_NAME_LENGTH,SessionID,WONID,Team)==1) {
	if(Team==SlayTeam && i != UserIndex) {
	  if(check_immunity(TargetName)==0) {
	    slay(TargetName);
	  }
	}
      }
    }
    PlayThunderclap();
    /* Since we have our own unique message, we have to handle
       admin_quiet ourselves. */
    if (getvar("admin_quiet") == 0) {
      snprintf(Text, MAX_TEXT_LENGTH, "[ADMIN] %s was struck down by %s's wrath.", TargetName, User);
      say(Text);
    } else if (getvar("admin_quiet") == 1) {
      snprintf(Text, MAX_TEXT_LENGTH, "[ADMIN] %s was struck down by the admin's wrath.", TargetName);
      say(Text);
    } else {
      log_command(User, Command, Data);
    }
  } else {
    selfmessage("The team to slay must be a number.");
  }
  return PLUGIN_HANDLED;
}

public admin_ungag(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iIndex;
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (check_user(Data) == 1) {
		get_username(Data,TargetName,MAX_NAME_LENGTH);
		say_command(User,Command,TargetName);
		/* How can someone with immunity be gagged anyway?
		Better safe than sorry, though. */
		if(check_immunity(TargetName)!=0) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't ungag %s, you silly bear.", TargetName);
			say(Text);
		} else {
			get_userindex(TargetName, iIndex);
			RemoveUserFlag(iIndex, FLAG_IGNORE_NAME);
			UserGagTime[iIndex] = GAG_INVALID;
		}
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

public admin_unllama(HLCommand,HLData,HLUserName,UserIndex) {
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new iIndex;
	new TargetName[MAX_NAME_LENGTH];
	new Text[MAX_TEXT_LENGTH];
	new User[MAX_NAME_LENGTH];
	
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if ( check_user(Data) == 1) {
		get_username(Data,TargetName,MAX_NAME_LENGTH);
		say_command(User,Command,TargetName);
		/* How can someone with immunity be a llama anyway?
		Better safe than sorry, though. */
		if(check_immunity(TargetName)!=0) {
			snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't transform %s back from being a llama, you silly bear.", TargetName);
			say(Text);
		} else {
			get_userindex(TargetName, iIndex);
			RemoveUserFlag(iIndex, FLAG_LLAMA);
			execclient(TargetName, "name NotALlama");
		}
	} else {
		selfmessage("Unrecognized player: ");
		selfmessage(Data);
	}
	return PLUGIN_HANDLED;
}

public HandleSay(HLCommand,HLData,HLUserName,UserIndex) {
	new iIgnoreGag = 0;
	new Command[MAX_COMMAND_LENGTH];
	new Data[MAX_DATA_LENGTH];
	new GagTime;
	new User[MAX_NAME_LENGTH];
	
	/* Ignore messages that come from the console */
	if (UserIndex == 0) {
		return PLUGIN_CONTINUE;
	}
	
	/* Check to see if this is a say_team message, and whether or not
	we should be trapping those. If not, leave.*/
	convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
	convert_string(HLData,Data,MAX_DATA_LENGTH);
	convert_string(HLUserName,User,MAX_NAME_LENGTH);
	if (getvar("admin_gag_sayteam")==0) {
		if (streq(Command,"say_team")==1) {
			return PLUGIN_CONTINUE;
		}
	}

	/* Check to see if this person is gagged */
	GagTime = UserGagTime[UserIndex];
	if (GagTime != GAG_INVALID) {
		if (GagTime != 0) {
			new Time = systemtime();
			if (Time > GagTime) {
				RemoveUserFlag(UserIndex, FLAG_IGNORE_NAME);
				UserGagTime[UserIndex] = GAG_INVALID;
				iIgnoreGag = 1;
			}
		}
		if (!iIgnoreGag) {
			messageex(User, "[ADMIN] You are currently gagged, and cannot speak.", print_chat);
			return PLUGIN_HANDLED;
		}
	}
	/* Check to see if this person is llama-fied. */
	strstripquotes(Data);
	if ((UserFlags[UserIndex] & FLAG_LLAMA) == FLAG_LLAMA) {
		/* They're a llama.  Check to see if they're saying something llama-ish.
		If they're not, eat this message, make them. */
		/* I tried using this:
		
		if(streq(data,"Ooorgle!")==0 && streq(data,"Bleeeat!")==0 && streq(data,"Brawwrr!")==0) {
			
		but it never worked.  Don't know why.  But this does.  So I use it. */
		new Match = FALSE;						
		if(streq(Data,"Ooorgle!")==1) {
			Match = TRUE;
		} else if (streq(Data, "Bleeeat!")==1) {
			Match = TRUE;
		} else if (streq(Data, "Brawwrr!")==1) {
			Match = TRUE;
		}
		
		if(Match==FALSE) {
			new LlamaMsg = random(3);
			if(LlamaMsg==0) {
				/* Orgling is the sound llamas purportedly make when they mate. */
				execclient(User, "say Ooorgle!");
			} else if (LlamaMsg==1) {
				execclient(User, "say Bleeeat!");
			} else {
				execclient(User, "say Brawwrr!");
			}
			return PLUGIN_HANDLED;
		}		
	}
	
	/* Check to see if the person is swearing */
	if ( check_words(Data)==0) {
		/* Compliments of PetitMorte */
		if (check_immunity(User)==0) {
			messageex(User, "Swearing not allowed on this server", print_center);
			new SwearMsg = random(5); /* Don't let em swear, but still let 'em say something. */
			if(SwearMsg==0) {
				execclient(User, "say Oy! I should have my mouth washed out with soap!");
			} else if (SwearMsg==1) {
				execclient(User, "say Sazza frazza rassin frazzer!!!");
			} else if (SwearMsg==2) {
				execclient(User, "say Can you believe I kiss my mother with this mouth?!?");
			} else if (SwearMsg==3) {
				execclient(User, "say No swearing please!  It's *icky* and will hurt my dainty little ears!");
			} else if (SwearMsg==4) {
				execclient(User, "say I have a potty mouth!");
			}
			return PLUGIN_HANDLED;
		}
	}	
	
	/* Otherwise, let this pass. */
	return PLUGIN_CONTINUE;
}

public plugin_connect(HLUserName, HLIP, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		UserFlags[UserIndex] = FLAG_INVALID;
		UserGagTime[UserIndex] = GAG_INVALID;
		CheckVaultFlags(UserIndex);
	}
	return PLUGIN_CONTINUE;
}

public plugin_disconnect(HLUserName, UserIndex) {
	if (UserIndex >= 1 && UserIndex <= MAX_PLAYERS) {
		UserFlags[UserIndex] = FLAG_INVALID;
		UserGagTime[UserIndex] = GAG_INVALID;
	}
	return PLUGIN_CONTINUE;
}

public plugin_info(HLOldName,HLNewName,UserIndex) {
	new iIgnoreGag = 0;
	new GagTime;
	new Command[MAX_TEXT_LENGTH];
	new NewName[MAX_NAME_LENGTH];
	new OldName[MAX_NAME_LENGTH];

	convert_string(HLNewName, NewName, MAX_NAME_LENGTH);
	convert_string(HLOldName, OldName, MAX_NAME_LENGTH);
	
	/* Only bother if the name has changed. */
	if(streq(OldName,NewName)==0) {
		/* Check for llama-isation */
		if((UserFlags[UserIndex] & FLAG_LLAMA) == FLAG_LLAMA) {
			execclient(OldName, "name Llama");
			return PLUGIN_HANDLED;
		/* Check for gag, only if we're gagging the name */
		} else if(getvar("admin_gag_name")==1) {
			GagTime = UserGagTime[UserIndex];
			if (GagTime != GAG_INVALID) {
				if (GagTime != 0) {
					new Time = systemtime();
					if (Time > GagTime) {
						RemoveUserFlag(UserIndex, FLAG_IGNORE_NAME);
						UserGagTime[UserIndex] = GAG_INVALID;
						iIgnoreGag = 1;
					}
				}
				if (!iIgnoreGag) {
					if ((UserFlags[UserIndex] & FLAG_IGNORE_NAME) == FLAG_IGNORE_NAME) {
						RemoveUserFlag(UserIndex, FLAG_IGNORE_NAME);
					} else {
						AddUserFlag(UserIndex, FLAG_IGNORE_NAME);
						snprintf(Command, MAX_TEXT_LENGTH, "name %s", OldName);
						execclient(OldName,Command);
					}
					return PLUGIN_HANDLED;
				}
			}			
		}
	}

	/* Don't allow them to take a name with a banned word in it */	
	if(check_words(NewName)==0) {
		execclient(OldName,"name NewName");
		return PLUGIN_HANDLED;
	}
	return PLUGIN_CONTINUE;
}

public plugin_init() {
	plugin_registerinfo("Admin Retribution Plugin","Commands for getting back at llamas and cheaters.",STRING_VERSION);
	
	plugin_registercmd("admin_execall","admin_execall",ACCESS_EXECCLIENT,"admin_execall <command>: Force everyone to execute command.");
	plugin_registercmd("admin_execclient","admin_execclient",ACCESS_EXECCLIENT,"admin_execclient <target> <command>: Force target to execute command.");
	plugin_registercmd("admin_execteam","admin_execteam",ACCESS_EXECCLIENT,"admin_execteam <team> <command>: Force everyone on team to execute command.");
	plugin_registercmd("admin_gag","admin_gag",ACCESS_GAG,"admin_gag <target> [<minutes>]: Gag target.  0 minutes is a permanent gag.");
	plugin_registercmd("admin_llama","admin_llama",ACCESS_LLAMA,"admin_llama <target>: Llama-fy target.");
	plugin_registercmd("admin_slap","admin_slap",ACCESS_SLAP,"admin_slap <target>: Slaps target.");
	plugin_registercmd("admin_slay","admin_slay",ACCESS_SLAY,"admin_slay <target>: Slays target.");
	plugin_registercmd("admin_slayteam","admin_slayteam",ACCESS_SLAY,"admin_slayteam <team>: Slays everyone on team.");
	plugin_registercmd("admin_ungag","admin_ungag",ACCESS_GAG,"admin_ungag <target>: Ungag target.");
	plugin_registercmd("admin_unllama","admin_unllama",ACCESS_LLAMA,"admin_unllama <target>: Unllama-fy target.");
	plugin_registercmd("say","HandleSay",ACCESS_ALL);
	plugin_registercmd("say_team","HandleSay",ACCESS_ALL);
		
	return PLUGIN_CONTINUE;	
}
