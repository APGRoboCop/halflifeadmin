/***
*
*	Copyright (c) 2000, Alfred Reynolds
****/

/*

===== users.cpp ========================================================

  routines for handling user functions for the admin_ modules

*/
#include <string.h>

#ifndef _WIN32
 #include <regex.h>
#endif

#ifndef _WIN32
#include <crypt.h>     
#endif

#ifdef USE_MYSQL
	#include <mysql.h>
	#include <errmsg.h>
#endif


#include "extdll.h"
#include "users.h"

user_struct users[USERS_SIZE]; // hardcode 128 users for the moment
user_struct nicks[NICKS_SIZE]; 
user_struct models[MODELS_SIZE]; 


ip_struct p_ips[MAX_IPS+1];	// priority ips

extern enginefuncs_t  g_engfuncs;
extern globalvars_t                               *gpGlobals;
DLL_GLOBAL BOOL  g_fLoadUsers;
DLL_GLOBAL BOOL  g_fLoadNicks;
DLL_GLOBAL BOOL  g_fLoadModels;
DLL_GLOBAL BOOL  g_fLoadIPs;
DLL_GLOBAL BOOL   g_fGameOver;

#ifdef USE_MYSQL
	extern MYSQL mysql;
	extern BOOL mysql_ok;
#endif

// typedef for list of admin commands
typedef struct users_commands_s
{   
	char *command;
	void	(*func)	(edict_t *pEntity);
	int access;
} users_commands_t;


/*

1 - vote map, vote kick,
2, - map, timelimit, fraglimit. (if they can change the map.. it
doesn't make sense to lock these out)
4 - prematch, reload,restartround
8 - pause, unpause
16 - pass, no_pass
32 - teamplay, Friendly-Fire
64 - admin_change other server variables.. (gravity, fall damage,
etc.. ), admin_say
128 - admin_kick
256 - admin_ban, admin_unban
512 - admin_status, admin_cfg
leaves some space for other stuff.
*/



users_commands_t user_commands[] = {
 {"admin_help",admin_help,ALL},
 {"admin_userlist",admin_userlist,ALL},
 {"admin_nextmap",admin_nextmap,ALL},
 {"admin_vote_kick",admin_vote_kick,LEV1},
 {"admin_vote_map",admin_vote_map,LEV1},
 {"admin_map",admin_map,LEV2},
 {"admin_pass",admin_pass,LEV16},
 {"admin_nopass",admin_nopass,LEV16},
 {"admin_say",admin_say,LEV64},
 {"admin_friendlyfire",admin_friendlyfire,LEV32},
 {"admin_teamplay",admin_teamplay,LEV32},
 {"admin_fraglimit",admin_fraglimit,LEV2},
 {"admin_timelimit",admin_timelimit,LEV2},
 {"admin_kick",admin_kick,LEV128},
 {"admin_status",admin_status,LEV512},
 {"admin_cfg",admin_cfg,LEV512},
 {"admin_ban",admin_ban,LEV256},
 {"admin_unban",admin_unban,LEV256},
 {"admin_reload",admin_reload,LEV4},
 {"admin_pause",admin_pause,LEV8},
 {"admin_unpause",admin_unpause,LEV8},
 {"admin_version",admin_version,ALL},
 {"admin_prematch",admin_prematch,LEV4},
 {"admin_timeleft",admin_timeleft,ALL},
 {"admin_restartround",admin_restartround,LEV4},
 {NULL,NULL,0}
};


void ShowMenu (edict_t* pev, int bitsValidSlots, int nDisplayTime, BOOL
fNeedMore, char pszText[500])

{
int gmsgShowMenu = 0;  
 gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 ); 
MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pev);

WRITE_SHORT( bitsValidSlots);
WRITE_CHAR( nDisplayTime );
WRITE_BYTE( fNeedMore );
WRITE_STRING (pszText);

MESSAGE_END();

}





/*********************************************************************************************
*
*
* match - return 1 if the strings match, 0 otherwise. Matching is either a 
* straight string match OR a regex expression, depending on the users choice.
*
*
**********************************************************************************************/
int  match(const char *string, char *pattern) {
#ifdef _WIN32

return (!strcmp(string,pattern));

#else

if ((int)CVAR_GET_FLOAT("use_regex")!=1)  {

	return (!strcmp(string,pattern));
}
else { 
	 int    status;
         regex_t    re;

		
         if ( regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB|REG_ICASE) != 0) {
             return(0);      // report error 
         }
         status = regexec(&re, string, (size_t) 0, NULL, 0);

         regfree(&re);
         if (status != 0) {
             return(0);      /* report error */
         }

	//if ( (int)CVAR_GET_FLOAT("admin_debug")!=0) {
	//	printf("matched %s to %s\n\n",string,pattern);
	//} 
         return(1);
}
#endif
}




/*********************************************************************************************
*
*
* check_user - check if a users password matches their name
*
*
**********************************************************************************************/

int check_user(const char *name,const char *pass) {
	int i=0,valid=0;

#ifndef USE_MYSQL
	char *encrypt;

	while(users[i].username[0]!=0 && i<USERS_SIZE) {
   if (match(name,users[i].username)==1) {    
	//	if (!strnicmp(name,users[i].username,32)) {
			valid=1;
			break;
		}
		i++;
	}

 
	if(valid==1) {
	// check the password
	valid=0;
#ifndef _WIN32
	encrypt=crypt(pass,users[i].username);
#else
	encrypt=(char *)&pass[0];
#endif
	if (!strncmp(encrypt,users[i].pass,PASSWORD_SIZE)) valid=1;
	} 

#else

if (mysql_ok==TRUE)
	{
	char query[4096];
	int res;
	MYSQL_RES *result;
	MYSQL_ROW row=NULL;


	sprintf(query, "SELECT pass FROM %s WHERE nick=\"%s\"", CVAR_GET_STRING("mysql_dbtable_users"), name);
	res=mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	result=mysql_use_result(&mysql);
	if (result)
		{
		if (row=mysql_fetch_row(result))
			{
			valid=0;
			if (strcasecmp(row[0], pass)==0) valid=1;
			}
		mysql_free_result(result);
		}
	}
#endif

return valid;
}


/*********************************************************************************************
*
*
* check_nick - check if a nicks password matches pass
*
*
**********************************************************************************************/
int check_nick_pass(const char *name,const char *pass) {
	int i=0,valid=2;

if ( (int)CVAR_GET_FLOAT("admin_debug")!=0) {
		UTIL_LogPrintf("[ADMIN] DEBUG: %s %s\n",name,pass);
}
#ifndef USE_MYSQL
	char *encrypt;
	while(nicks[i].username[0]!=0 && i<NICKS_SIZE) {
	  if (match(name,nicks[i].username)==1) { 
			valid=1;
			break;
		}
		i++;
	}

if ( (int)CVAR_GET_FLOAT("admin_debug")!=0) {
		UTIL_LogPrintf("[ADMIN] DEBUG: %s %s %s %i\n",name,pass,nicks[i].pass,i);
}
	if(valid==1) {
	// check the password
	valid=0;
#ifndef _WIN32
	encrypt=crypt(pass,nicks[i].username);
#else
	encrypt=(char *)&pass[0];
#endif
	if (!strcmp(encrypt,nicks[i].pass)) valid=1;
	}
#else

if (mysql_ok==TRUE)
	{
	char query[4096];
	int res;
	MYSQL_RES *result;
	MYSQL_ROW row=NULL;

	sprintf(query, "SELECT pass FROM %s WHERE nick=\"%s\"", CVAR_GET_STRING("mysql_dbtable_nicks"), name);
	res=mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	result=mysql_use_result(&mysql);
	if (result)
		{
		if (row=mysql_fetch_row(result))
			{
			valid=0;
			if (strcasecmp(row[0], pass)==0) valid=1;
			}
		mysql_free_result(result);
		}
	}
#endif

return valid;
}





/*********************************************************************************************
*
*
* check_nick - check if a nick is in the registration db and act upon it

*
*
**********************************************************************************************/

int check_nick(char *kick_name,edict_t *pEntity) {
// check if the nick is registered and passes
char *infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity);

char name[256];

if (kick_name==NULL) strncpy(name,g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ),256);
else strncpy(name,kick_name,256);

 if (    g_engfuncs.pfnInfoKeyValue( infobuffer, (char *)CVAR_GET_STRING("password_field"))!=NULL && !FStrEq(CVAR_GET_STRING("nicks_file" ),"0") ) {   // nicks_file is NOT "0"     

  int ans= check_nick_pass(g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ),g_engfuncs.pfnInfoKeyValue( infobuffer, (char *)CVAR_GET_STRING("password_field")));
 if  ( ans == 1 ) {
CLIENT_PRINTF( pEntity, print_console,"[ADMIN] Nick password accepted\n");
} else if (ans == 2) {
//CLIENT_PRINTF( pEntity, print_console,"[ADMIN] Nick not in database\n");
} else {

// ans == 0
if (! FStrEq(CVAR_GET_STRING("nicks_kick_msg"),"0") ) { // nicks_file != "0"
CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] %s\n",CVAR_GET_STRING("nicks_kick_msg" )));
}else {

CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] You either entered the wrong password or this nick is taken by someone else.\n Please choose another nick and connect again\nuse \"setinfo %s <password>\" to set your nick password\n ",(char *)CVAR_GET_STRING("password_field")));
}

return 0;
}

} else if ( !FStrEq(CVAR_GET_STRING("nicks_file" ),"0") ) { // nicks_file is NOT "0"
if ( !FStrEq(CVAR_GET_STRING("nicks_kick_msg"),"0") ) { // the msg is NOT "0"
CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] %s\n",CVAR_GET_STRING("nicks_kick_msg" )));
}else {

CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] You either entered the wrong password or this nick is taken by someone else.\n Please choose another nick and connect again\nuse \"setinfo %s <password>\" to set your nick password\n",(char *)CVAR_GET_STRING("password_field")));
}

return 0;
}
return 1;
}





/******************************************************************************************
*
*
* check_model - check if a users has the right to use a model
*
*
**********************************************************************************************/

int check_model_pass(const char *model,const char *pass) {
	int i=0,valid=0;

#ifndef USE_MYSQL
	char *encrypt;

	while(models[i].username[0]!=0 && i<MODELS_SIZE) {
	   if (match(model,models[i].username)==1) {    
			valid=1;
			break;
		}
		i++;
	}

	
	if ( i==MODELS_SIZE ||models[i].username[0]==0  ) return 2; // not protected
 
	if(valid==1) {
	// check the password
	valid=0;
#ifndef _WIN32
	encrypt=crypt(pass,models[i].username);
#else
	encrypt=(char *)&pass[0];
#endif
	if (!strcmp(encrypt,models[i].pass)) valid=1;
	}

#else

if (mysql_ok==TRUE)
	{
	char query[4096];
	int res;
	MYSQL_RES *result;
	MYSQL_ROW row=NULL;


	sprintf(query, "SELECT pass FROM %s WHERE model=\"%s\"", CVAR_GET_STRING("mysql_dbtable_models"), model);
	res=mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	result=mysql_use_result(&mysql);
	if (result)
		{
		if (row=mysql_fetch_row(result))
			{
			valid=0;
			if (strcasecmp(row[0], pass)==0) valid=1;
			}
		mysql_free_result(result);
		}
	}
#endif

return valid;
}


int check_model(char *model_name,edict_t *pEntity) {
// check if the nick is registered and passes
char *infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity);

char name[256];

if (model_name==NULL) strncpy(name,g_engfuncs.pfnInfoKeyValue( infobuffer, "model" ),256);
else strncpy(name,model_name,256);

 if (    g_engfuncs.pfnInfoKeyValue( infobuffer, (char *)CVAR_GET_STRING("models_password_field"))!=NULL && !FStrEq(CVAR_GET_STRING("models_file" ),"0") ) {   // models_file is NOT "0"     

  int ans= check_model_pass(g_engfuncs.pfnInfoKeyValue( infobuffer, "model" ),g_engfuncs.pfnInfoKeyValue( infobuffer, (char *)CVAR_GET_STRING("models_password_field")));
 if  ( ans == 1 ) {
CLIENT_PRINTF( pEntity, print_console,"[ADMIN] Model password accepted\n");
} else if (ans == 2) {
//CLIENT_PRINTF( pEntity, print_console,"[ADMIN] Model not in database\n");
} else {

// ans == 0
if (! FStrEq(CVAR_GET_STRING("models_kick_msg"),"0") ) { //models_message != "0"
CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] %s\n",CVAR_GET_STRING("models_kick_msg" )));
}else {

CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] You either entered the wrong password or this model is passworded.\n Please choose another model and connect again\nuse \"setinfo %s <password>\" to set your model password\n ",(char *)CVAR_GET_STRING("models_password_field")));
}

return 0;
}

} else if ( !FStrEq(CVAR_GET_STRING("models_file" ),"0") ) { // nicks_file is NOT "0"
if ( !FStrEq(CVAR_GET_STRING("models_kick_msg"),"0") ) { // the msg is NOT "0"
CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] %s\n",CVAR_GET_STRING("models_kick_msg" )));
}else {

CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("[ADMIN] You either entered the wrong password or this model is passworded.\n Please choose another model and connect again\nuse \"setinfo %s <password>\" to set your model password\n ",(char *)CVAR_GET_STRING("models_password_field")));

}

return 0;
}
return 1;
}












/*********************************************************************************************
*
*
* auth_user - return 1 is name and access level match for a users entry

*
*
**********************************************************************************************/

int auth_user(char *name,int access) {
	int valid=0;
	int axx=0;

	if(access==ALL) return 1;

	int default_access = (int)CVAR_GET_FLOAT("default_access");	
	
	if (  (default_access!=0) && ((default_access & access) != 0) ) return 1; // default access allowed

#ifndef USE_MYSQL
	int i=0;

	while(users[i].username[0]!=0 && i<USERS_SIZE) {
 if (match(name,users[i].username)==1) {     
	//	if (!strnicmp(name,users[i].username,32)) {
			valid=1;
			break;
		}
		i++;
	}
	axx=users[i].access;
#else

if (mysql_ok==TRUE)
	{
	char query[4096];
	int res;
	MYSQL_RES *result;
	MYSQL_ROW row=NULL;

	sprintf(query, "SELECT access FROM %s WHERE nick=\"%s\"", CVAR_GET_STRING("mysql_dbtable_users"), name);
	res=mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	result=mysql_use_result(&mysql);
	if (result)
		{
		if (row=mysql_fetch_row(result))
			{
			valid=1;
			axx=atoi(row[0]);
			}
		mysql_free_result(result);
		}
	}
#endif

//	UTIL_LogPrintf( "[ADMIN] User:%i  Access:%i %i\n",users[i].access,access,access & users[i].access);

	if ( valid == 1) { // this is a valid user
		if ( (access & axx) ==0 ) valid=0;
		else valid=1;
	}
return valid;
}

















/****************************************************************************************************
Alfreds new commands
*****************************************************************************************************/


//int vote,map_menu=0,num_yes_map=0,num_no_map=0,num_no=0;
//int name,num_yes=0,vote_time_kick=0,vote_time_map=0,total_num=0;
vote_struct votes[VOTE_LENGTH]; // struct holding vote data

 CBaseEntity *pPlayer;

/*********************************************************************************************
*
*
* admin_help - print out help on a topic
*
*
**********************************************************************************************/

void admin_help(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;

	if (auth_user(STRING(pev->netname),LEV2)==1) {

CLIENT_PRINTF( pEntity, print_console,"admin_map <map_name>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  change the level to map_name\n");
}
	if (auth_user(STRING(pev->netname),LEV128)==1) {

CLIENT_PRINTF( pEntity, print_console," admin_kick <username> [message]\n");
CLIENT_PRINTF( pEntity, print_console,"                                  kick username and send them the message \"message\" (optional)\n");
}
	if (auth_user(STRING(pev->netname),LEV16)==1) {

CLIENT_PRINTF( pEntity, print_console," admin_pass <server_passowrd>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  put a password on the server\n");
CLIENT_PRINTF( pEntity, print_console," admin_nopass\n");
CLIENT_PRINTF( pEntity, print_console,"                                  turn off the server password\n");
}
	if (auth_user(STRING(pev->netname),LEV2) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_timelimit <time>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  set how long to play maps for to time minutes\n");
}
	if (auth_user(STRING(pev->netname),LEV2) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_fraglimit <x>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  change levels when x frags have been reached\n");
}
	if (auth_user(STRING(pev->netname),LEV128) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_say <message>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  say this message to all users\n");
}
	if (auth_user(STRING(pev->netname),LEV32) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_friendlyfire <x>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  set friendlyfire to x (0=Off,1=On)\n");
}
	if (auth_user(STRING(pev->netname),LEV32) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_teamplay <x>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  set teamplay to x (0=Off,1=On)\n");
}
	if (auth_user(STRING(pev->netname),LEV512) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_status\n");
CLIENT_PRINTF( pEntity, print_console,"                                  print some server information (currently unimplemented)\n");
}
	if (auth_user(STRING(pev->netname),LEV512) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_cfg <script_name>\n");
CLIENT_PRINTF( pEntity, print_console,"                                 execute the script .\n");
}
	if (auth_user(STRING(pev->netname),LEV4) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_prematch <pre_match>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  set the prematch variable to <pre_match> (FOR TFC ONLY)\n");
CLIENT_PRINTF( pEntity, print_console," admin_restartround <x>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  change the sv_restartround variable to x\n");
}
#ifndef USE_MYSQL
	if (auth_user(STRING(pev->netname),LEV4) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_reload\n");
CLIENT_PRINTF( pEntity, print_console,"                                  reload the users and nick file\n");
}
#endif
	if (auth_user(STRING(pev->netname),LEV128) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_ban <username> <mins>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  ban and kick username from the server for min minutes\n");
CLIENT_PRINTF( pEntity, print_console," admin_unban <username>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  remove the ban on userid\n");
}
	if (auth_user(STRING(pev->netname),LEV1) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_vote_map <map_name>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  vote to change to map <map_name>\n");
}
	if (auth_user(STRING(pev->netname),LEV1) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_vote_kick <player_name>\n");
CLIENT_PRINTF( pEntity, print_console,"                                  vote to kick player <player_name>\n");
}

// anyone can access these three functions
CLIENT_PRINTF( pEntity, print_console," admin_userlist\n");
CLIENT_PRINTF( pEntity, print_console,"                                  list the users on the system\n");
CLIENT_PRINTF( pEntity, print_console," admin_help\n");
CLIENT_PRINTF( pEntity, print_console,"                                  print out a list of commands available to you\n");
CLIENT_PRINTF( pEntity, print_console," admin_timeleft\n");
CLIENT_PRINTF( pEntity, print_console,"                                  print out the time remaining on the current map\n");
CLIENT_PRINTF( pEntity, print_console," admin_nextmap\n");
CLIENT_PRINTF( pEntity, print_console,"                                  print out the next map in the map cycle\n");




	if (auth_user(STRING(pev->netname),LEV8) ==1){
CLIENT_PRINTF( pEntity, print_console," admin_pause\n");
CLIENT_PRINTF( pEntity, print_console,"                                  enable pausing of the server\n");
CLIENT_PRINTF( pEntity, print_console," admin_unpause\n");
CLIENT_PRINTF( pEntity, print_console,"                                  disable pausing of the server\n");
}

CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs("To be able to use restricted commands you must enter your password using \"setinfo %s <password>\" before using the command\n",CVAR_GET_STRING("password_field")));
}


/*********************************************************************************************
*
*
* admin_userlist
*
*
**********************************************************************************************/

void admin_userlist(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	    int             i;
                                                                                        // loop through all players
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "Name     Server ID\n"));
        for ( i = 1; i <= gpGlobals->maxClients; i++ )
        {
                CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );


                if ( pPlayer)
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "%s <%i> WON:%u\n",STRING(pPlayer->pev->netname), GETPLAYERUSERID( pPlayer->edict() ), GETPLAYERWONID( pPlayer->edict()) ));
			
	}

		UTIL_LogPrintf( "[ADMIN:%s] executed userlist\n",STRING(pev->netname) );
}



/*********************************************************************************************
*
*
* admin_vote_kick
*
*
**********************************************************************************************/


void admin_vote_kick(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
		int vote_freq_kick = (int)CVAR_GET_FLOAT("vote_freq_kick" );
		float kick_ratio = (float)CVAR_GET_FLOAT("kick_ratio")*100;
		int k; 
		vote_struct *vote=&votes[KICK_VOTE];

		if ( vote_freq_kick==0 || kick_ratio==0) return; // not enabled 

		if ( ((int)time(NULL)-vote_freq_kick) > vote->time ) { 
		strncpy(vote->name, CMD_ARGV(1),32 );	// Make a copy of the new map name
		int num=atoi(vote->name);
                         // printf("IN HERE");                                                              // loop through all players

        for ( k = 1; k <= gpGlobals->maxClients; k++ )
        {
                pPlayer = UTIL_PlayerByIndex( k );
                if ( pPlayer) {
		  if (num==0 && FStrEq(STRING(pPlayer->pev->netname),vote->name)) break;
		  else if ( num== GETPLAYERUSERID( pPlayer->edict() )) break;
		}	
	}

	
		if (k>= gpGlobals->maxClients) {
				CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "kick error, unable to find user %s\n",vote->name));
			vote=0;
			return;
		}

		vote->enabled=1;
		vote->time=(int)time(NULL);
		char tus[500];

		sprintf(tus,"Kick %s?:\n1: Yes\n2: No\n",STRING(pPlayer->pev->netname));
		int             i;
                // loop through all players

		vote->num_yes=vote->num_no=vote->total_num=0;
	 	for ( i = 1; i <= gpGlobals->maxClients; i++ ) {
               		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
                	if ( pPlayer) {
				ShowMenu (ENT(pPlayer->pev), 3, MENU_SHOW, 0, tus);
				vote->total_num++;
			}
		}
       
		UTIL_LogPrintf( "[ADMIN] Kick vote initiated from user %s for user %s\n",STRING(pev->netname),vote->name);
	} else {
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must wait for %i minutes between votes\n",vote_freq_kick/60));
	
	}
}








/*********************************************************************************************
*
*
* check_map - check if we are allowed to vote for that map
*
*
**********************************************************************************************/
int allowed_map(char *map) { //is this map in maps.ini ? 1=yes, 0=no

	char *mapcfile = (char*)CVAR_GET_STRING( "maps_file" );
	if (mapcfile==NULL || FStrEq(mapcfile,"0")) return 0;
	int length;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( mapcfile, &length );
	if ( pFileList && length )
	{


		// keep pulling mapnames out of the list until we find "map", else return 0
		while ( 1 )
		{
			while ( *pFileList && isspace( *pFileList ) ) pFileList++; // skip over any whitespace
			if ( !(*pFileList) )
				break;

			char cBuf[32];
			int ret = sscanf( pFileList, " %32s", cBuf );
			// Check the map name is valid
			if ( ret != 1 || *cBuf < 13 )
				break;

			if ( FStrEq( cBuf, map ) )
			{  // we've found our map;  they can do it!
				return 1;
			}

			pFileList += strlen( cBuf );
		}

		FREE_FILE( aFileList );
	}
return 0;

}


int check_map(char *map)
{
	
//	char szNextMap[32];
	// find the map to change to

	char *mapcfile = (char*)CVAR_GET_STRING( "maps_file" );
		
	if ( FStrEq(map,"next_map")) { // next map in the cycle 

	int length;
	char *pFileList;
	char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
	if (mapcfile==NULL) return 0;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( mapcfile, &length );
	if ( pFileList && length )
	{
		// the first map name in the file becomes the default
		sscanf( pFileList, " %32s", map);

		// keep pulling mapnames out of the list until the current mapname
		// if the current mapname isn't found,  load the first map in the list
		BOOL next_map_is_it = FALSE;
		while ( 1 )
		{
			while ( *pFileList && isspace( *pFileList ) ) pFileList++; // skip over any whitespace
			if ( !(*pFileList) )
				break;

			char cBuf[32];
			int ret = sscanf( pFileList, " %32s", cBuf );
			// Check the map name is valid
			if ( ret != 1 || *cBuf < 13 )
				break;

			if ( next_map_is_it )
			{
				// check that it is a valid map file
				if ( IS_MAP_VALID( cBuf ) )
				{
					strcpy( map, cBuf );
					break;
				}
			}

			if ( FStrEq( cBuf, STRING(gpGlobals->mapname) ) )
			{  // we've found our map;  next map is the one to change to
				next_map_is_it = TRUE;
			}

			pFileList += strlen( cBuf );
		}

		FREE_FILE( aFileList );
	}

	// you can always vote for the next map on the cycle
		//strncpy(map,szNextMap,32);
		return (IS_MAP_VALID(map));


	} else { // they have defined the maps.ini file for us

	char *mapcfile = (char*)CVAR_GET_STRING( "maps_file" );
	if (mapcfile==NULL || FStrEq(mapcfile,"0") ) { // no maps.ini file defined, just check it is good
		return (IS_MAP_VALID(map));
	}
	if (!IS_MAP_VALID(map)) return 0; // bad map name
	return(allowed_map(map));

	} // end of maps.ini else check


	
return 0;

}

	
/*********************************************************************************************
*
*
* admin_vote_map
*
*
**********************************************************************************************/


void admin_vote_map(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	int vote_freq_map = (int)CVAR_GET_FLOAT("vote_freq_map" );	
	float map_ratio = (float)CVAR_GET_FLOAT("map_ratio")*100;
	vote_struct *vote=&votes[MAP_VOTE];
		
	if ( vote_freq_map==0 || map_ratio==0) return; // not enabled 	
		if ( ((int)time(NULL)-vote_freq_map) > vote->time ) { 
		strncpy(vote->name, CMD_ARGV(1),32 );	// Make a copy of the new map name		
		fix_string(vote->name,strlen(vote->name));


		if (check_map(vote->name)==0 ){
		CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "%s is not on the allowed map list\n",vote->name));
			return;
		}
	if ( !IS_MAP_VALID(vote->name)){
		CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "%s is not a valid map name\n",vote->name));
			return;
		}

		vote->enabled=1;
		vote->time=(int)time(NULL);
		char tus[500];

		sprintf(tus,"Change map to %s?:\n1: Yes\n2: No\n",vote->name);
		 int   i;
                  // loop through all players
		vote->total_num=vote->num_yes=vote->num_no=0;

        for ( i = 1; i <= gpGlobals->maxClients; i++ )
        {
                CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );
                if ( pPlayer) {
			ShowMenu (ENT(pPlayer->pev), 3, MENU_SHOW, 0, tus);
			vote->total_num++;
		}
	}
	UTIL_LogPrintf( "[ADMIN] Map vote initiated from user %s for map %s\n",STRING(pev->netname),vote->name);	
	} else {
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must wait for %i minutes between votes\n",vote_freq_map/60));
	}
		
}

/*********************************************************************************************
*
*
* admin_map
*
*
**********************************************************************************************/

void admin_map(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	char szNextMap[32];
	
	const char *iszItem =CMD_ARGV(1) ;	// Make a copy of the new map name
if (iszItem==NULL  ) { 
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must enter a map name"));
	return;
}
	strncpy( szNextMap, iszItem,32);
	fix_string(szNextMap,strlen(szNextMap));

	if ( !IS_MAP_VALID(szNextMap) ) {
		CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "bad map name %s\n",iszItem));
		UTIL_LogPrintf( "[ADMIN:%s] Bad map %s\n",STRING(pev->netname),iszItem);
		return;
	}

	g_fGameOver = TRUE;

	ALERT( at_console, "CHANGE LEVEL: %s\n", szNextMap );
	UTIL_LogPrintf( "[ADMIN:%s] Map change to %s\n", STRING(pev->netname),iszItem);	
	CHANGE_LEVEL( szNextMap, NULL );
}

/*********************************************************************************************
*
*
* admin_pass
*
*
**********************************************************************************************/

void admin_pass(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	const char *iszItem = CMD_ARGV(1) ;
if (iszItem==NULL  ) { 
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must enter a password"));
	return;
}
	 		CVAR_SET_STRING( "sv_password", iszItem);
			CVAR_SET_FLOAT( "password",1);
			UTIL_LogPrintf( "[ADMIN:%s] Password put on server(%s)\n", STRING(pev->netname),iszItem);
}

/*********************************************************************************************
*
*
* admin_nopass
*
*
**********************************************************************************************/

void admin_nopass(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
			CVAR_SET_STRING( "sv_password", "");
			CVAR_SET_FLOAT( "password",0);
			UTIL_LogPrintf( "[ADMIN:%s] Password reset\n", STRING(pev->netname));
}



/*********************************************************************************************
*
*
* admin_say
*
*
**********************************************************************************************/

void admin_say(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;


	char *iszItem=(char *)  CMD_ARGS() ;
	if ( iszItem==NULL ) {
		CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must say something\n"));
	 	return;
	}	

		 UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs(
"Message from the Admin: %s\n",iszItem));    

			UTIL_LogPrintf( "[ADMIN:%s] Message from the Admin: %s\n", STRING(pev->netname),iszItem);
}

/*********************************************************************************************
*
*
* admin_friendlyfire
*
*
**********************************************************************************************/

void admin_friendlyfire(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	if (atoi( CMD_ARGV(1)) > 0 ) {
		 		CVAR_SET_FLOAT( "mp_friendlyfire", 1);
				UTIL_LogPrintf( "[ADMIN:%s] Friendly fire set to 1\n",STRING(pev->netname));
			} else {
				CVAR_SET_FLOAT( "mp_friendlyfire", 0);
				UTIL_LogPrintf( "[ADMIN:%s] Friendly fire set to 0\n",STRING(pev->netname));
			}
}




/*********************************************************************************************
*
*
* admin_fraglimit
*
*
**********************************************************************************************/

void admin_fraglimit(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	if (atoi( CMD_ARGV(1)) > 0 ) {
	 			CVAR_SET_FLOAT( "mp_fraglimit", atoi( CMD_ARGV(1) ));
				UTIL_LogPrintf( "[ADMIN:%s] Fraglimit set to %i\n",STRING(pev->netname),atoi( CMD_ARGV(1)));
			}

}

/*********************************************************************************************
*
*
* admin_restartround
*
*
**********************************************************************************************/

void admin_restartround(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	if (atoi( CMD_ARGV(1)) > 0 ) {
	 			CVAR_SET_FLOAT( "sv_restartround", atoi( CMD_ARGV(1) ));
				UTIL_LogPrintf( "[ADMIN:%s] Restartround set to %i\n",STRING(pev->netname),atoi( CMD_ARGV(1)));
			}

}

/*********************************************************************************************
*
*
* admin_prematch
*
*
**********************************************************************************************/

void admin_prematch(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	if (atoi( CMD_ARGV(1)) > 0 ) {
	 			CVAR_SET_FLOAT( "tfc_clanbattle_prematch", atoi( CMD_ARGV(1) ));
				UTIL_LogPrintf( "[ADMIN:%s] Prematch set to %i\n",STRING(pev->netname),atoi( CMD_ARGV(1)));
			}

}

/*********************************************************************************************
*
*
* admin_timelimit
*
*
**********************************************************************************************/

void admin_timelimit(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	if (atoi( CMD_ARGV(1)) > 0 ) {
	 			CVAR_SET_FLOAT( "mp_timelimit", atoi( CMD_ARGV(1) ));
				UTIL_LogPrintf( "[ADMIN:%s] Timelimit set to %i\n",STRING(pev->netname),atoi( CMD_ARGV(1)));
			}

}

/*********************************************************************************************
*
*
* admin_teamplay
*
*
**********************************************************************************************/

void admin_teamplay(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	if (atoi( CMD_ARGV(1)) > 0 ) {
	 			CVAR_SET_FLOAT( "mp_teamplay", atoi( CMD_ARGV(1) ));
				UTIL_LogPrintf( "[ADMIN:%s] Teamplay set to %i\n",STRING(pev->netname),atoi( CMD_ARGV(1)));
			}

}


/*********************************************************************************************
*
*
* admin_kick
*
*
**********************************************************************************************/

void admin_kick(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;

	char szCommand[256];
			const char *iszItem = CMD_ARGV(1);
			const char *isz2Item = CMD_ARGV(2) ;
 			CBaseEntity *pKick;

	if (iszItem==NULL || isz2Item==NULL ) { 
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must enter a name and a time"));
	return;
}
	if (atoi(iszItem)!=0) {
	#ifndef _WIN32
			snprintf(szCommand,256,"kick # %i\n",atoi(iszItem) );
#else
	sprintf(szCommand,"kick # %i\n",atoi(iszItem) );
#endif

} else {
// DO NUMBERS AND NAMES 
	pKick=UTIL_PlayerByName((char *)iszItem);

	if ( pKick==NULL) {
			CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "Unable to find player %s\n",iszItem));
		return;
	}


	if (!GETPLAYERUSERID( pKick->edict())) { // whoops, can't find wonid
		CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "Unable to find player %s's WONID (%u)\n",iszItem,GETPLAYERWONID( pKick->edict())));
	}

#ifndef _WIN32
			snprintf(szCommand,256,"kick # %i\n", GETPLAYERUSERID( pKick->edict()));
#else
	sprintf(szCommand,"kick # %i\n",GETPLAYERUSERID( pKick->edict()) );
#endif
}


			if ( strlen(isz2Item)!=0) {
			int i;
			CBaseEntity *pPlayer=NULL;
			// send them a message first
			 for (i = 1; i <= gpGlobals->maxClients; i++) {
        			pPlayer = UTIL_PlayerByIndex(i);

        		if (pPlayer == NULL)
            			continue;

        		if (FNullEnt(pPlayer->pev))
            			continue;

        		if (FStrEq(STRING(pPlayer->pev->netname),iszItem))
            			break;
			}
			if (pPlayer!=NULL) {
				edict_t *pPlayerEdict = INDEXENT( i );    
				CLIENT_PRINTF( pPlayerEdict, print_console,UTIL_VarArgs( "%s\n",isz2Item));

			}
			

			}
			SERVER_COMMAND(szCommand);
			UTIL_LogPrintf( "[ADMIN:%s] kicked %s\n",STRING(pev->netname),iszItem );		


}

/*********************************************************************************************
*
*
* admin_status
*
*
**********************************************************************************************/

void admin_status(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;

	UTIL_LogPrintf( "[ADMIN:%s] Status %i\n",STRING(pev->netname),NUMBER_OF_ENTITIES());
			//gpGlobals->mapname
}

/*********************************************************************************************
*
*
* admin_version
*
*
**********************************************************************************************/

void admin_version(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;

	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "Admin mod version %s\n",MOD_VERSION));
			//gpGlobals->mapname
}


/*********************************************************************************************
*
*
* admin_cfg
*
*
**********************************************************************************************/

void admin_cfg(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	char szCommand[256];
	const char *iszItem =  CMD_ARGV(1) ;
		if ( iszItem==NULL) {
CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must enter a file to run"));
	return;
}
#ifndef _WIN32
			snprintf(szCommand,256,"exec %s\n",iszItem );
#else
	sprintf(szCommand,"exec %s\n",iszItem );
#endif
			SERVER_COMMAND(szCommand);
			UTIL_LogPrintf( "[ADMIN:%s] executed exec %s\n",STRING(pev->netname),iszItem );
}


/*********************************************************************************************
*
*
* admin_ban
*
*
**********************************************************************************************/

void admin_ban(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	char szCommand[256];
	const char *iszItem = CMD_ARGV(1);
	const char *isz2Item = CMD_ARGV(2);
//	int i;
	CBaseEntity *pKick;

	if (iszItem==NULL || isz2Item==NULL ) { 
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must enter a name and a time"));
	return;
}

	int time_out=atoi(isz2Item);
	if (time_out==0) {
	CLIENT_PRINTF( pEntity, print_console,"You must specify a time to ban for (a non zero positive number)\n");	
		return;
         }

if (atoi(iszItem)!=0) {
	#ifndef _WIN32
			snprintf(szCommand,256,"banid %i #%i kick\n",time_out,atoi(iszItem) );
#else
	sprintf(szCommand,"banid %i #%i kick\n",time_out,atoi(iszItem) );
#endif

} else {

	pKick=UTIL_PlayerByName((char *)iszItem);

	if (pKick==NULL) {
			CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "Unable to find player %s\n",iszItem));
		return;
	}


	if (!GETPLAYERUSERID( pKick->edict()) || !GETPLAYERWONID( pKick->edict())) { // whoops, can't find wonid
		CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "Unable to find player %s's WONID (%u)\n",iszItem,GETPLAYERWONID( pKick->edict())));
}




#ifndef _WIN32
			snprintf(szCommand,256,"banid %i %u kick\n",time_out,GETPLAYERWONID( pKick->edict()));
#else
	sprintf(szCommand,"banid %i %u kick\n",time_out,GETPLAYERWONID( pKick->edict()));	
#endif
}


			SERVER_COMMAND(szCommand);
#ifndef _WIN32
			snprintf(szCommand,256,"writeid\n",iszItem );
#else
			sprintf(szCommand,"writeid\n",iszItem );
#endif
			SERVER_COMMAND(szCommand);
			UTIL_LogPrintf( "[ADMIN:%s] executed baned for %s\n",STRING(pev->netname),isz2Item);

}


/*********************************************************************************************
*
*
* admin_unban
*
*
**********************************************************************************************/

void admin_unban(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	char szCommand[256];
	const char *iszItem = CMD_ARGV(1) ;
if (iszItem==NULL  ) { 
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You must enter a name to unban"));
	return;
}


#ifndef _WIN32
			snprintf(szCommand,256,"removeid %s\n",iszItem);
#else
sprintf(szCommand,"removeid %s\n",iszItem);
#endif
			SERVER_COMMAND(szCommand);
#ifndef _WIN32
			snprintf(szCommand,256,"writeid\n",iszItem );
#else
sprintf(szCommand,"writeid\n",iszItem );
#endif
			SERVER_COMMAND(szCommand);
			UTIL_LogPrintf( "[ADMIN:%s] executed removeid %s\n",STRING(pev->netname),iszItem );
}


/*********************************************************************************************
*
*
* admin_reload
*
*
**********************************************************************************************/

void admin_reload(edict_t *pEntity) {
#ifndef USE_MYSQL
	entvars_t *pev = &pEntity->v;

	g_fLoadUsers=TRUE;
	g_fLoadNicks=TRUE;
	g_fLoadModels=TRUE;
 	g_fLoadIPs=TRUE;
	load_users();
	load_nicks();
	load_models();
	load_ips();
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "Reloaded users/nicks/ips file\n"));
	UTIL_LogPrintf( "[ADMIN:%s] reloading users/nicks/ips file\n",STRING(pev->netname) );
#endif
}

/*********************************************************************************************
*
*
* admin_pause
*
*
**********************************************************************************************/

void admin_pause(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;
	char szCommand[256];

#ifndef _WIN32
			snprintf(szCommand,256,"pausable 1\n");
#else
			sprintf(szCommand,"pausable 1\n");
#endif
			SERVER_COMMAND(szCommand);


	UTIL_LogPrintf( "[ADMIN:%s] paused server\n",STRING(pev->netname) );
}

/*********************************************************************************************
*
*
* admin_unpause
*
*
**********************************************************************************************/

void admin_unpause(edict_t *pEntity) {
	entvars_t *pev = &pEntity->v;

	char szCommand[256];

#ifndef _WIN32
			snprintf(szCommand,256,"pausable 0\n");
#else
			sprintf(szCommand,"pausable 0\n");
#endif
			SERVER_COMMAND(szCommand);


	UTIL_LogPrintf( "[ADMIN:%s] unpaused server\n",STRING(pev->netname) );
}




/*********************************************************************************************
*
*
* admin_timeleft
*
*
**********************************************************************************************/

void admin_timeleft(edict_t *pEntity) {

 float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60; // map timelimit in sec


	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "timeleft on map:%0.0f minutes\n",(flTimeLimit-gpGlobals->time)/60 ));

}

/*********************************************************************************************
*
*
* admin_nextmap - print out the next map on the cycle
*
*
**********************************************************************************************/

void admin_nextmap(edict_t *pEntity) {

	char map[32];
	int length;
	char *pFileList;
	char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
	if (mapcfile==NULL) return;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( mapcfile, &length );
	if ( pFileList && length )
	{
		// the first map name in the file becomes the default
		sscanf( pFileList, " %32s", map);

		// keep pulling mapnames out of the list until the current mapname
		// if the current mapname isn't found,  load the first map in the list
		BOOL next_map_is_it = FALSE;
		while ( 1 )
		{
			while ( *pFileList && isspace( *pFileList ) ) pFileList++; // skip over any whitespace
			if ( !(*pFileList) )
				break;

			char cBuf[32];
			int ret = sscanf( pFileList, " %32s", cBuf );
			// Check the map name is valid
			if ( ret != 1 || *cBuf < 13 )
				break;

			if ( next_map_is_it )
			{
				// check that it is a valid map file
				if ( IS_MAP_VALID( cBuf ) )
				{
					strcpy( map, cBuf );
					break;
				}
			}

			if ( FStrEq( cBuf, STRING(gpGlobals->mapname) ) )
			{  // we've found our map;  next map is the one to change to
				next_map_is_it = TRUE;
			}

			pFileList += strlen( cBuf );
		}

		FREE_FILE( aFileList );
	}
	CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "next map in cycle: %s\n",map));
}


















/*********************************************************************************************
*
*
* do_admin_commands - do some client commands 
*
*
**********************************************************************************************/



void do_admin_commands(edict_t *pEntity) {

	const char *pcmd = CMD_ARGV(0);
	entvars_t *pev = &pEntity->v;
	int i=0;

 	while(user_commands[i].command!=NULL) {
	if (FStrEq(user_commands[i].command,pcmd)) {


	int default_access = (int)CVAR_GET_FLOAT("default_access");	
	
	if (  !((default_access!=0) && ((default_access & user_commands[i].access) != 0)) && (user_commands[i].access!=ALL)) {

			 char *infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity); 
			
			if (check_user(STRING(pev->netname),g_engfuncs.pfnInfoKeyValue( infobuffer, (char *)CVAR_GET_STRING("password_field") ))==0) {
				CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "bad password\n"));
				UTIL_LogPrintf( "[ADMIN:%s] Bad password\n", STRING(pev->netname));
				i++;
				continue; // go back to looking for commands to do
			}
		} // access

		if (auth_user(STRING(pev->netname),user_commands[i].access)==1) 		{
			user_commands[i].func(pEntity);
		} else {
			CLIENT_PRINTF( pEntity, print_console,UTIL_VarArgs( "You do not have access to this command\n"));
		}
		
	} // fstreq
	i++;
       } // while
 }












/******************************************************************************************************
*********************************************************************************************************/



/*********************************************************************************************
*
*
* load_users - load the users file into the users struct 
*
*
**********************************************************************************************/


void load_users(void)
{

#ifdef USE_MYSQL
	return;
#else
	int i=0;
	char *usersfile = (char*)CVAR_GET_STRING("users_file" );
	
	if ( g_fLoadUsers==FALSE) return;
	

	g_fLoadUsers=FALSE; // get set to true on a world spawn 

	memset(users,0x0,sizeof(user_struct)*USERS_SIZE); 
      
 if (FStrEq(usersfile,"0") || usersfile==NULL) return; // no users file, thats bad!
	UTIL_LogPrintf( "[ADMIN] Loading users from file %s\n",(char*)CVAR_GET_STRING("users_file" ));

	int length;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(usersfile, &length );
	

	if ( pFileList && length )
	{
	
//		int k,len;
		while ( *pFileList && isspace( *pFileList ) ) pFileList++; // skip over any whitespace
		while ( (*pFileList)) { // while some string is left
			char cBuf[USERNAME_SIZE+PASSWORD_SIZE+10]; // room for the name + password + access value
			char username[USERNAME_SIZE],pass[PASSWORD_SIZE];
			int j;
			
			memset(cBuf,0x0,USERNAME_SIZE+PASSWORD_SIZE+10);
			memset(username,0x0,USERNAME_SIZE);
			memset(pass,0x0,PASSWORD_SIZE);

			j=0;
			while ( pFileList[j]!='\n' && j < length && j<(USERNAME_SIZE+PASSWORD_SIZE+10))
			{
				cBuf[j]=pFileList[j];
				j++;
			}
		//	if ( ret==0) strcpy(cBuf,pFileList);
			pFileList=pFileList+strlen(cBuf)+1;
			length=length-strlen(cBuf)-1;

			// we have the string, tokenize it
			char *next=strchr(cBuf,':');

			if (next==NULL) continue; // whoops, bad line
			if ( *(next-1)=='\\') {
				strcpy(next-1,next);
				next=strchr(next,':');
			}
	if (next==NULL) continue; // whoops, bad line


			if ( strlen(cBuf)-strlen(next) > USERNAME_SIZE) 
				strncpy(username,cBuf,USERNAME_SIZE);
			else
				strncpy(username,cBuf,strlen(cBuf)-strlen(next));

			strncpy(cBuf,next+1,strlen(next));

			next=strchr(cBuf,':');
			if (next==NULL) continue; // whoops, bad line
			if ( *(next-1)=='\\') {
				strcpy(next-1,next);
				next=strchr(next,':');
			}
	if (next==NULL) continue; // whoops, bad line


			if ( strlen(cBuf)-strlen(next) > PASSWORD_SIZE) 
				strncpy(pass,cBuf,PASSWORD_SIZE);
			else
				strncpy(pass,cBuf,strlen(cBuf)-strlen(next));

			if(next[strlen(next)-1]=='\r') next[strlen(next)-1]='\0'; // remove that damned \r char!
			users[i].access=atoi(next+1);
			strcpy(users[i].username,username);
			strcpy(users[i].pass,pass);
			UTIL_LogPrintf( "[ADMIN] User:%s Access:%i\n",users[i].username,users[i].access);
			i++;
		}

	users[i].username[0]=0;
	FREE_FILE( aFileList );
}



#endif
}





/*********************************************************************************************
*
*
* load_nicks - load the nicks file into the nicks struct 
*
*
**********************************************************************************************/

void load_nicks(void)
{

#ifdef USE_MYSQL
	return;
#else
	int i=0;
	char *usersfile = (char*)CVAR_GET_STRING("nicks_file" );
//	ASSERT( usersfile != NULL );
	
	if ( g_fLoadNicks==FALSE) return;

	g_fLoadNicks=FALSE; // get set to true on a world spawn 
	memset(nicks,0x0,sizeof(user_struct)*NICKS_SIZE); 

	if ( FStrEq(usersfile,"0") || usersfile==NULL) return;
	
	UTIL_LogPrintf( "[ADMIN] Loading nicks from file %s\n",(char*)CVAR_GET_STRING("nicks_file" ));	



	int length;
	char *pFileList;
//	char *p,*p1;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(usersfile, &length );

	if ( pFileList && length )
	{
	//	int k,len;
		while ( *pFileList && isspace( *pFileList ) ) pFileList++; // skip over any whitespace
		while ( (*pFileList)) { // while some string is left
			char cBuf[USERNAME_SIZE+PASSWORD_SIZE+10]; // room for the name + password + access value
			char username[USERNAME_SIZE],pass[PASSWORD_SIZE];
			int j;			

			memset(cBuf,0x0,USERNAME_SIZE+PASSWORD_SIZE+10);
			memset(username,0x0,USERNAME_SIZE);
			memset(pass,0x0,PASSWORD_SIZE);

			j=0;
			while ( pFileList[j]!='\n' && j < length && j<(USERNAME_SIZE+PASSWORD_SIZE+10)) {
				cBuf[j]=pFileList[j];
				j++;
			}
		//	int ret=sscanf( pFileList, "%s\n", cBuf);
		//	if ( ret==0) strcpy(cBuf,pFileList);
			pFileList=pFileList+strlen(cBuf)+1;
			length=length-strlen(cBuf)-1;

			// we have the string, tokenize it
			char *next=strchr(cBuf,':');

			if (next==NULL) continue; // whoops, bad line
			if ( *(next-1)=='\\') {
				strcpy(next-1,next);
				next=strchr(next,':');
			}
	if (next==NULL) continue; // whoops, bad line

			if ( strlen(cBuf)-strlen(next) > USERNAME_SIZE) 
				strncpy(username,cBuf,USERNAME_SIZE);
			else
				strncpy(username,cBuf,strlen(cBuf)-strlen(next));

			strcpy(nicks[i].username,username);
			if(next[strlen(next)-1]=='\r') next[strlen(next)-1]='\0'; // remove that damned \r char!

			
			strncpy(nicks[i].pass,next+1,PASSWORD_SIZE);
			UTIL_LogPrintf( "[ADMIN] Nick:%s Pass:%s\n",nicks[i].username,nicks[i].pass);
			i++;	
		}
		nicks[i].username[0]=0;
		FREE_FILE( aFileList );
	}
#endif
}


/*********************************************************************************************
*
*
* load_models - load the models password file into the models struct 
*
*
**********************************************************************************************/

void load_models(void)
{

#ifdef USE_MYSQL
	return;
#else
	int i=0;
	char *usersfile = (char*)CVAR_GET_STRING("models_file" );
//	ASSERT( usersfile != NULL );
	

	if ( g_fLoadModels==FALSE) return;

	g_fLoadModels=FALSE; // get set to true on a world spawn 
	memset(models,0x0,sizeof(user_struct)*MODELS_SIZE); 

	if ( FStrEq(usersfile,"0") || usersfile==NULL) return;
	
	UTIL_LogPrintf( "[ADMIN] Loading models from file %s\n",(char*)CVAR_GET_STRING("models_file" ));	


	int length;
	char *pFileList;
//	char *p,*p1;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(usersfile, &length );

	if ( pFileList && length )
	{
	//	int k,len;
		while ( *pFileList && isspace( *pFileList ) ) pFileList++; // skip over any whitespace
		while ( (*pFileList)) { // while some string is left
			char cBuf[USERNAME_SIZE+PASSWORD_SIZE+10]; // room for the name + password + access value
			char username[USERNAME_SIZE],pass[PASSWORD_SIZE];
			int j;			

			memset(cBuf,0x0,USERNAME_SIZE+PASSWORD_SIZE+10);
			memset(username,0x0,USERNAME_SIZE);
			memset(pass,0x0,PASSWORD_SIZE);

			j=0;
			while ( pFileList[j]!='\n' && j < length && j<(USERNAME_SIZE+PASSWORD_SIZE+10)) {
				cBuf[j]=pFileList[j];
				j++;
			}
			pFileList=pFileList+strlen(cBuf)+1;
			length=length-strlen(cBuf)-1;

			// we have the string, tokenize it
			char *next=strchr(cBuf,':');
	if (next==NULL) continue; // whoops, bad line

			if ( *(next-1)=='\\') {
				strcpy(next-1,next);
				next=strchr(next,':');
			}
	if (next==NULL) continue; // whoops, bad line

		
			if ( strlen(cBuf)-strlen(next) > USERNAME_SIZE) 
				strncpy(username,cBuf,USERNAME_SIZE);
			else
				strncpy(username,cBuf,strlen(cBuf)-strlen(next));

			strcpy(models[i].username,username);
			if(next[strlen(next)-1]=='\r') next[strlen(next)-1]='\0'; // remove that damned \r char!

			
			strncpy(models[i].pass,next+1,PASSWORD_SIZE);
			UTIL_LogPrintf( "[ADMIN] Model:%s Pass:%s\n",models[i].username,models[i].pass);
			i++;	
		}
		models[i].username[0]=0;
		FREE_FILE( aFileList );
	}
#endif
}









void spit_entry(char *heading, ip_struct *ip)
{
#define B1(x) (int)((x>>24)&0xFF)
#define B2(x) (int)((x>>16)&0xFF)
#define B3(x) (int)((x>>8 )&0xFF)
#define B4(x) (int)((x    )&0xFF)
	char *slash = "", addr[IP_SIZE+1], mask[IP_SIZE+1];

	if ( ip->nstr[0] ) slash = "/";
	sprintf(addr, "%d.%d.%d.%d",
	        B1(ip->addr),B2(ip->addr),B3(ip->addr),B4(ip->addr));
	sprintf(mask, "%d.%d.%d.%d",
	        B1(ip->mask),B2(ip->mask),B3(ip->mask),B4(ip->mask));

	UTIL_LogPrintf("[ADMIN] IP%s <%s%s%s> MASK=%s/%s\n",
	               heading, ip->str, slash, ip->nstr, addr, mask);
}
/*********************************************************************************************
*
* make_nbo  -  returns an ulong in nbo from the dotted ip string 'str',
*              also if default_netmaskp is non-null it is set to the 
*              default netmask in nbo for the dotted ip.
*              This acts differently from inet_aton etc as, e.g.
*              203.9 is valid and will return you 203.9.0.0 (in nbo) and
*              also set default_netmask to 255.255.0.0 (in nbo)
*
**********************************************************************************************/
ulong make_nbo(char *str, ulong *default_netmaskp)
{
	int shift;
	ulong addr, addr_byte;
	char *q = str;

	addr = 0x00000000;
	for( shift=24; *q && shift>=0; shift-=8) {
		addr_byte = atoi(q);
		if (addr_byte>255) addr_byte = 255;
		addr |= addr_byte << shift;
		if (default_netmaskp)
			*default_netmaskp |= 0xFF << shift;
		while (isdigit(*q)) q++;
		if (*q == '.') q++;
	}
	return addr;
}

/*********************************************************************************************
*
*
* parse_ip_and_mask  - parse string 'p' into dotted-ip[\(bits|dotted-netmask)]
*                      ip->str      string copy of ip
*                      ip->nstr     string copy of netmask/bits
*                      ip->addr     addr as ulong in nbo
*                      ip->mask     netmask as ulong in nbo
*
*                      return pointer to next char after what was parsed
**********************************************************************************************/
char *parse_ip_and_mask(char *p, ip_struct *ip)
{
	int i;
	char *saved;
	ulong  bits;

	ip->addr = 0x00000000;

	for(i=0, saved=p; i<IP_SIZE; p++, i++){
		if ( ! ( isdigit(*p) || *p == '.' ) ) 
			break;
	}
	if (!i) return p;			// invalid dotted-ip

	strncpy(ip->str,saved,i);
	// now make mask of dotted ip string, and set default netmask
	ip->addr = make_nbo(ip->str, &ip->mask);

	if ( *p == '/' ) {			// a netmask/bits was specified
		for(i=0, saved=++p; i<IP_SIZE; p++, i++){
			if ( ! ( isdigit(*p) || *p == '.' ) ) 
				break;
		}
		if (!i) return p;		// invalid netmask/bits

		strncpy(ip->nstr,saved,i);
		if (strchr(ip->nstr,'.')) {	// dotted-ip netmask
			ip->mask = make_nbo(ip->nstr, NULL);
		} else {			// bits
			bits = atoi(ip->nstr);
			if (bits>32) bits = 32;
			ip->mask = 0xFFFFFFFF << (32 - bits);
		}
	}
	return p;
}

/*********************************************************************************************
*
*
* load_ips - load the ips file into the p_ips array
*
**********************************************************************************************/

void load_ips(void)
{

#ifdef USE_MYSQL
	return;
#else
	int  i=0;
	char *ipfile = (char*)CVAR_GET_STRING("ips_file");
//	ASSERT( ipfile != NULL );
	
	if ( ipfile==NULL || FStrEq(ipfile,"0") ) return;
	if ( g_fLoadIPs==FALSE) return;

	g_fLoadIPs=FALSE; 		// get set to true on a world spawn 
	memset(p_ips,0,sizeof(p_ips)); 

	UTIL_LogPrintf( "[ADMIN] Loading Priority IPs from %s\n",ipfile);

	int length;
	char *pFileList, *p, *newline;
	char *aFileList= pFileList= (char*)LOAD_FILE_FOR_ME(ipfile,&length);
	
	if ( pFileList && length ) {
		while ( 1 ) {
			while ( isspace(*pFileList) ) pFileList++;
			if ( !(*pFileList) ) break;
			if ((newline=strchr(pFileList,'\n'))==NULL) break;

			p = parse_ip_and_mask(p=pFileList,&p_ips[i]); 
			if (p_ips[i].addr) {
				spit_entry("LOAD", &p_ips[i]);  // log ip
				if (++i == MAX_IPS) break;
			}
			pFileList = newline;
		}
		FREE_FILE( aFileList );
		if ((int)CVAR_GET_FLOAT("reserve_slots")==0)
			UTIL_LogPrintf( "[ADMIN] Priority IPs wont work until reserve_slots is set\n");
	} else
		UTIL_LogPrintf( "[ADMIN] No IPs loaded from file=%s\n",ipfile);
#endif
}
 
/*********************************************************************************************
*
*
* priority_ip
*            return TRUE if ip is a priority ip
*
**********************************************************************************************/

int priority_ip(char *ip) {

	int i=0;
	ulong addr;
#ifndef USE_MYSQL
	addr = make_nbo(ip,NULL);
	while ( p_ips[i].addr ) {
		if ((p_ips[i].addr & p_ips[i].mask) == (addr & p_ips[i].mask)) {
			spit_entry("MATCH", &p_ips[i]);
			return TRUE;
		}
		i++;
	}
	return FALSE;
#else
	int found=FALSE;
if (mysql_ok==TRUE)
	{
	char query[4096];
	int res;
	MYSQL_RES *result;
	MYSQL_ROW row=NULL;


	sprintf(query, "SELECT ip FROM %s WHERE ip like \"%s%\"", CVAR_GET_STRING("mysql_dbtable_ips"), ip);
	res=mysql_real_query(&mysql, query, (unsigned int)strlen(query));
	result=mysql_use_result(&mysql);
	if (result)
		{
		found=TRUE;
		mysql_free_result(result);
		}
	}
#endif
}



