/*
Admin mod script. Copyright (C) 2000, Alfred Reynolds.

 Parts by: them@clanwho.com (http://www.clanwho.com) (admin_rcon)
 Other parts by: Nathan O'Sullivan (numtostr and admin_vote_restart)
 	         Jaguar  <magua@wedgewood.net>  (new script functions)
*/

#include <core>
#include <console>
#include <admin>
#include <string>

#define ALL 0
#define LEV1 1
#define LEV2 1<<1
#define LEV4  1<<2
#define LEV8 1<<3
#define LEV16 1<<4
#define LEV32 1<<5 /* 32 */
#define LEV64 1<<6 /* 64 */
#define LEV128 1<<7
#define LEV256 1<<8
#define LEV512 1<<9
#define LEV1024 1<<10
#define LEV2048  1<< 11
#define LEV4096  1<<12
#define LEV8192 1<<13
#define LEV16384 1<<14
#define LEV32768 1<<15

enum PRINT_TYPE {
	print_console=0,
        print_center,
        print_chat,
	print_tty
  };


/*new NULL_CHAR = '^0';*/
#define MAX_NAME_LENGTH 50
#define MAX_DATA_LENGTH 200

new user[50]; /* the username of the person calling the script*/

/*****************************************************************************
			HELPER FUNCTIONS
******************************************************************************/



/* returns 1 if two strings are EXACTLY the same in the first n (or less if one is smaller than n) digits, 0 otherwise */

streq(a[],b[],n) {
	
 	new i;
	
	// be careful about how much we check
	if (n > strlen(a)) n=strlen(a);
	if (n > strlen(b)) n=strlen(b);

	for(i=0;i<n;i++) 
		if (a[i]!=b[i]) return 0;

	return 1;
}


min(a,b) {
	if (a<b) return a;
	else return b;
	return 1;
}






/*****************************************************************************
			END OF HELPER FUNCTIONS
******************************************************************************/


admin_say(msg[]) {
  
  new pmsg[100];
  
  if ( auth("") != 1 || access(LEV64,"")!=1) {
    selfmessage("You don't have access to this command");
    return;
  }                                    
  
  snprintf( pmsg, 100, "Message from Admin (%s)", user);
  say(pmsg);
  say(msg);
}

admin_map(map[]) {

  if ( auth("") != 1 || access(LEV2,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  if (valid_map(map)==1) {
    say("Changing level to :");
    say(map);
    changelevel(map);
  } else {
    selfmessage("Bad map name");
    selfmessage(map);
  }
}





admin_vote_kick(kick_user[]) {

  if ( access(LEV1,"")!=1) {
    selfmessage("You don't have access to this command");	
    return;	
  }
  
  if (vote_allowed()!=1) {
    selfmessage("Vote not allowed at this time.");
    return;
  }
  if (check_user(kick_user) == 1) {
    new real_user[MAX_NAME_LENGTH];
    get_username(kick_user,real_user,MAX_NAME_LENGTH);
    new msg[MAX_DATA_LENGTH] = "Kick ";
    strcat(msg,real_user, MAX_DATA_LENGTH);
    strcat(msg,"?", MAX_DATA_LENGTH);
    vote(msg,"Yes","No","kick_vote",real_user);
  } else {
    selfmessage("Unrecognized user name ");
    selfmessage(kick_user);
  } 
}


public kick_vote(a,b,c,d) {
 
  new vote_kick_username[100];
  convert_string(b,vote_kick_username,100);
  
  if ( a == 1 ) {
    new ratio = getvar("kick_ratio");
    if ( ratio == 0 ) {
      say("Kick vote is not enabled");
      return;
    }
    if ( c > (ratio/100)*d ) {
      new rc;
      rc=check_user(vote_kick_username);
      if( rc>0 ) { 
	new real_user[32];
	if ( rc == 2 || rc == 3 ) {
	  get_username( vote_kick_username, real_user,32);
	} else {
	  strcpy(real_user ,vote_kick_username,32);
	}
	
	new msg[200];			
	snprintf( msg, 200, "%s kicked due to a vote", real_user );
	say(msg);
	message(real_user,"You have been kicked due to a vote");
	kick(real_user);
      }
    } else {
      say("Not enough votes for kick");
    }
  } else {
    say("Not enough votes for kick");
  }
}


admin_vote_map(map[]) {
  
  if ( vote_allowed()==1 && access(LEV1,"")==1) {
    if ( valid_map(map) == 1) {
      new msg[100];
      if (strlen(map)<1) {
	selfmessage("Incorrect commandline");
      }
      
      snprintf( msg, 100, "Change map to %s", map );
      log(msg);
      vote(msg,"Yes","No","map_vote",map);
    } else
      selfmessage("Map does not exist");
  } else 
    selfmessage("Vote not allowed at this time");
  
}


public map_vote(a,b,c,d) {
 
  new vote_map[100];
  new tell[500];
  convert_string(b,vote_map,100);
  
  if ( a == 1 ) {
    new ratio = getvar("map_ratio");
    if ( ratio == 0 ) {
      say("Map vote is not enabled");
      return;
    }
    if ( c > (ratio/100)*d ) {
      snprintf( tell, 500, "Change of map to %s due to vote" , vote_map );
      centersay(tell,10,0,255,0);
      changelevel(vote_map);
    } else {
      say("Not enough votes for map change");	
    }
  } else {
    say("Not enough votes for map change");
  }
}


// FROM Nathan O'Sullivan
/*
 *****
 admin_vote_restart

 To use, add the following in client_commands():

  else if (streq(command,"admin_vote_restart",strlen(command))==1)
                admin_vote_restart(data);

 Requires: numtostr()
 *****
*/

admin_vote_restart(unused[]) {
  if ( vote_allowed()==1 && access(LEV1,"")==1) {
    new msg[100];
    strcpy(msg,"Restart map & begin play?", 100 );
    vote(msg,"Yes","No","restart_vote",unused);
  } else
    selfmessage("Vote not allowed at this time");
}

public restart_vote(a,b,c,d) {
  
  new vote_map[100];
  convert_string(b,vote_map,100);
  
  if ( a == 1 ) {
    new ratio = getvar("map_ratio");
    if ( ratio == 0 ) {
      say("Restart vote is not enabled");
      return;
    }
    if ( c > (ratio/100)*d ) {
      new tleft, tlimit;
      tleft = timeleft(0) - 10; // # seconds till restart!
      tlimit = getvar("mp_timelimit");
      tlimit = tlimit + (tlimit - tleft/60);
      
      new str[100];
      snprintf( str, 100, "mp_timelimit %d", tlimit );
      exec (str);
      exec ("sv_restartround 10");
    } else {
      say("Not enough votes for map restart");
    }
  } else {
    say("Not enough votes for map restart");
  }
}


/* END */



admin_gravity(a[]) {

  if ( auth("") != 1 || access(LEV32,"")!=1) {
    selfmessage("You don't have access to this command");	
    return;	
  }
  
  new msg[100];
  strcpy(msg,"sv_gravity ",100);
  strcat(msg,a, 100);
  say("Changing gravity to:");
  say(a);
  exec(msg);
}

admin_pause(a) {

  if ( auth("") != 1 || access(LEV8,"")!=1) {
    selfmessage("You don't have access to this command");	
    return;	
  }
  
  
  new msg[100];
  strcpy(msg,"pausable ",100);
  
  if ( a == 1) {
    strcat(msg," 1", 100);
    log("setting pausable to 1");
  } else {
    strcat(msg," 0", 100);
    log("setting pausable to 0");
  }
  
  exec(msg);
  
}

admin_pass(a[]) {
  
  if ( auth("") != 1 || access(LEV16,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  new msg[100];
  snprintf( msg, 100, "sv_password %s", a );
  /* a[strlen(a)-1]=0; */
  log("setting password");
  
  exec(msg);
  
}

admin_friendlyfire(a[]) {

  if ( auth("") != 1 || access(LEV32,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  new msg[100];
  snprintf( msg, 100, "mp_friendlyfire %s", a );
  log("setting mp_friendlyfire");
  
  exec(msg);
  
}

admin_teamplay(a[]) {
  
  if ( auth("") != 1 || access(LEV32,"")!=1) {
    selfmessage("You don't have access to this command");	
    return;	
  }
  
  new msg[100];
  snprintf( msg, 100, "mp_teamplay %s", a );
  log("setting mp_teamplay");
  
  exec(msg);
  
}

admin_fraglimit(a[]) {

  if ( auth("") != 1 || access(LEV2,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  new msg[100];
  snprintf( msg, 100, "mp_fraglimit %s", a );
  log("setting mp_fraglimit");
  
  exec(msg);
  
}

admin_timelimit(a[]) {
  
  if ( auth("") != 1 || access(LEV2,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }	
  
  new msg[100];
  snprintf( msg, 100, "mp_timelimit %s", a );
  log("setting mp_timelimit");
  
  exec(msg);
  
}

admin_kick(kick_user[]) {

  if ( auth("") != 1 || access(LEV128,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  if ( check_user(kick_user) == 1) {
    new real_user[MAX_NAME_LENGTH];
    get_username(kick_user,real_user,MAX_NAME_LENGTH);
    message(real_user,"You have been kicked by ");
    message(real_user,user);
    kick(real_user);
  } else {
    selfmessage("Unrecognized player: ");
    selfmessage(kick_user);
  }
  
}

admin_cfg(a[]) {
  
  if ( auth("") != 1 || access(LEV512,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  new msg[100];
  strcpy(msg,"exec ", 100);
  strcat(msg,a, 100);
  log("Executing script");
  
  exec(msg);
}

admin_restartround(a[]) {

  if ( auth("") != 1 || access(LEV4,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  new msg[100];
  strcpy(msg,"sv_restartround ",100);
  strcat(msg,a, 100);
  log("setting sv_restartround");
  
  exec(msg);
  
}


admin_prematch(a[]) {
  
  if ( auth("") != 1 || access(LEV4,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  new msg[100];
  strcpy(msg,"tfc_clanbattle_prematch ",100);
  strcat(msg,a, 100);
  log("setting tfc_clanbattle_prematch");
  
  exec(msg);
  
}

admin_unban(a[]) {
  
  if ( auth("") != 1 || access(LEV256,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  unban(a);
  log("Unbanning user:");
  log(a);
}

admin_ban(ban_user[]) {
  
  if ( auth("") != 1 || access(LEV256,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  
  if (check_user(ban_user)==1) {
    new real_user[MAX_NAME_LENGTH];
    get_username(ban_user,real_user,MAX_NAME_LENGTH);
    ban(real_user,0);
  } else {
    selfmessage("Unrecognized player: ");
    selfmessage(ban_user);
  }
  
}

admin_reload() {
  
  if ( auth("") != 1 || access(LEV4,"")!=1) {
    selfmessage("You don't have access to this command");
    return;	
  }
  reload();
}






admin_rcon(a[]) {
  
  new tellout[500];
  if ( auth("") != 1 || access(LEV512,"") != 1) {
    selfmessage("Access to this command is not allowed");
    return;
  }
  
  
  if ( strncasecmp(a, "rcon_password", strlen("rcon_password") == 0) ) {
    selfmessage("Denied, command logged and red flagged for review");
    snprintf(tellout, 500, "WARNING: %s has attempted to change the rcon_password, BANNING", user );
    ban(user, 0);
    kick(user);
    say(tellout)
    return;
  }
  snprintf( tellout, 500, "ADMIN COMMAND: %s just issued an rcon command.", user );
  say(tellout);
  exec(a);
}


admin_listmaps(user_here[]) {
  new curmap[100];
  
  message(user_here,"The maps on the mapcycle are:");
  list_maps();
  message(user_here,"and the current map is:");
  currentmap(curmap,100);
  message(user_here,curmap);  
}

public say_stuff() {
  centersay("This server is using Admin Mod",10,0,255,0);
}


/*
 *
 * Called when the server first loads up a level (when the first client connects)
 *
 */


public client_start() { 
  set_timer("say_stuff",600,99999); // repeat forever	
}


new new_user[40];

public say_hello() {
  messageex(new_user,"Welcome to the Real World...",print_center);
}


/*
 *
 *
 * Called when a clients info details change (like their name,model,...)
 *   type "setinfo" into the client console to get a full list
 *
 */

public client_info(HLname,HLSessionID,HLWONID) {
  new name[40],model[40];
  
  convert_string(HLname,name,40); // the username 
  
  get_userinfo(name,"model",model,40); // get the model the user has
  
  //set_serverinfo("ALFRED","this is what I store");
  //message(name,model); // now tell them what it is ;)
  //message(name,"user details changed :)");
}




/*
 *
 * Called when each player joins the server
 *
 */

public client_connect(a,b) {
  new name[40],ip[200];
  // NOTE - you cannot message this user as they don't exists yet (really ...) 
  // You can set a timer to message them in 30 sec or so tho
  // DOUBLE NOTE - the timer here won't work right when more than 1 person enters in a 30 sec period. An array would have to be made, but you bright scripters can do that :)
  
  
  
  convert_string(a,name,40); // the username
  convert_string(b,ip,40); // the ip of the user
  strcpy(new_user,name,40);
  min(2,1);
  set_timer("say_hello",60,0); // say hello to the user
  
}




/*
 *
 * Main function called when a client enters a command into the console
 *
 */
public client_commands(HLCommand,HLData,HLUser,HLSessionID,HLWONID) {

	new command[40],data[100];

	convert_string(HLCommand,command,40);
        convert_string(HLData,data,100);
        convert_string(HLUser,user,40);

  		
	// check_words needs to be above he strlen check
	if ( check_words(data)==0) {
		selfmessage("You are not allowed to swear on this server");
		return 1;
	}

	//new tst[100];
	//get_serverinfo("ALFRED",tst,100);     // get the value stored in ALFRED
	//log(tst);

	if ( strlen(command) < strlen("admin") ) return 0;

	if( streq(command,"admin_say",strlen(command))==1 ) 
		admin_say(data);
	else if( streq(command,"admin_map",strlen(command))==1) 
		admin_map(data);
	else if( streq(command,"admin_vote_kick",strlen(command))==1) 
		admin_vote_kick(data);
	else if( streq(command,"admin_vote_map",strlen(command))==1) 
		admin_vote_map(data);
	else if( streq(command,"admin_vote_restart",strlen(command))==1) 
		admin_vote_restart(data);
	else if( streq(command,"admin_gravity",strlen(command))==1)
		admin_gravity(data);
	else if( streq(command,"admin_pause",strlen(command))==1)
		admin_pause(1);
	else if( streq(command,"admin_unpause",strlen(command))==1)
		admin_pause(0);
	else if( streq(command,"admin_pass",strlen(command))==1)
		admin_pass(data);
	else if( streq(command,"admin_nopass",strlen(command))==1)
		admin_pass("^"^"");
	else if( streq(command,"admin_friendlyfire",strlen(command))==1) 
		admin_friendlyfire(data);
	else if( streq(command,"admin_fraglimit",strlen(command))==1)
		admin_fraglimit(data);
	else if( streq(command,"admin_teamplay",strlen(command))==1) 
		admin_teamplay(data);
	else if( streq(command,"admin_timelimit",strlen(command))==1) 
		admin_timelimit(data);
	else if( streq(command,"admin_kick",strlen(command))==1) 
		admin_kick(data);
	else if( streq(command,"admin_cfg",strlen(command))==1) 
		admin_cfg(data);
	else if( streq(command,"admin_prematch",strlen(command))==1) 
		admin_prematch(data);
	else if( streq(command,"admin_restartround",strlen(command))==1) 
		admin_restartround(data);
	else if( streq(command,"admin_unban",strlen(command))==1) 
		admin_unban(data);
	else if( streq(command,"admin_ban",strlen(command))==1) 
		admin_ban(data);
	else if( streq(command,"admin_reload",strlen(command))==1) 
		admin_reload();
	else if( streq(command,"admin_timeleft",strlen(command))==1) 
		timeleft();
 	else if( streq(command,"admin_nextmap",strlen(command))==1) 
		nextmap(data,0);
	else if( streq(command,"admin_version",strlen(command))==1)
		version();
	else if( streq(command,"admin_userlist",strlen(command))==1)
		userlist();
	else if( streq(command,"admin_help",strlen(command)) ==1)
		help(data);
	else if( streq(command,"admin_rcon",strlen(command))==1 )
        	admin_rcon(data);
	 else if( streq(command,"admin_listmaps",strlen(command))==1 )
        	admin_listmaps(user);

	// return 1 to say we got a command and acted upon it
	if ( streq(command,"admin_",strlen("admin_")) ==1) 
		return 1;
	
	return 0;
}

