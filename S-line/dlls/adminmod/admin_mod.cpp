/*
 * ===========================================================================
 *
 * $Id: admin_mod.cpp,v 1.38 2004/08/21 19:44:06 darope Exp $
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
 * The guts of Admin Mod
 *
 */

//#include <string.h> //Deprecated [APG]RoboCop[CL]
#include <cstring>

#ifdef _WIN32
#define stricmp _stricmp
#endif

#define QUEUEING_DEBUGF(s) 

#ifdef USE_MYSQL
#  include <mysql.h>
#  include <errmsg.h>
#endif

#ifdef USE_PGSQL
#  include <libpq-fe.h>
#endif

#include "extdll.h"
#define SDK_UTIL_H	// extdll.h already #include's util.h

#include "meta_api.h"
#include "amlibc.h"
#include "amutil.h"
#include "users.h"
#include "statics.h"

AMXINIT amx_Init;
AMXREGISTER amx_Register;
AMXFINDPUBLIC amx_FindPublic;
AMXEXEC amx_Exec;
AMXGETADDR amx_GetAddr;
AMXSTRLEN amx_StrLen;
AMXRAISEERROR amx_RaiseError;
AMXSETSTRING amx_SetString;
AMXGETSTRING amx_GetString;

#ifdef CS_TEAMINFO
CGameRules* g_pGameRules = 0;
FIV myCountTeams = 0;
FII myCountTeamPlayers = 0;
#endif

AMX amx;
void *program=nullptr;

DLL_GLOBAL edict_t *pAdminEnt;
DLL_GLOBAL edict_t *pTimerEnt;

extern  AMX_NATIVE_INFO admin_Natives[];
extern  enginefuncs_t   g_engfuncs;
extern  globalvars_t*   gpGlobals;
extern  auth_struct     g_AuthArray[MAX_PLAYERS + 1];

#include <deque>   // deque
#include <memory>  // auto_ptr
using namespace std;
struct AMConnectingPlayer {
	edict_t* pEntity;
	char acName[USERNAME_SIZE];
	char acIPPort[IPPORT_SIZE];
};
deque <AMConnectingPlayer*> g_ovcPendingPlayers;


extern DLL_GLOBAL BOOL		g_fInitialized;
extern DLL_GLOBAL BOOL      g_fIPsLoaded;
extern DLL_GLOBAL BOOL      g_fModelsLoaded;
extern DLL_GLOBAL BOOL      g_fUsersLoaded;    
extern DLL_GLOBAL BOOL      g_fVaultLoaded;
extern DLL_GLOBAL BOOL      g_fWordsLoaded;
extern DLL_GLOBAL BOOL      g_fRunPlugins;
extern DLL_GLOBAL BOOL      g_fRunScripts;

cvar_t admin_balance_teams = {"admin_balance_teams","0",FCVAR_EXTDLL, 0};
cvar_t admin_bot_protection = {"admin_bot_protection","0",FCVAR_EXTDLL, 0};
cvar_t admin_connect_msg = {"admin_connect_msg","Welcome to the Real World...",FCVAR_EXTDLL};
cvar_t admin_cs_restrict = {"admin_cs_restrict","0",FCVAR_EXTDLL, 0};
cvar_t admin_debug = {"admin_debug","0",FCVAR_EXTDLL, 0};
cvar_t admin_devel = {"admin_devel","0",FCVAR_EXTDLL, 0};
cvar_t admin_fun_mode = {"admin_fun_mode","0",FCVAR_EXTDLL, 0};
cvar_t admin_fx= {"admin_fx","0",FCVAR_EXTDLL, 0};
cvar_t admin_gag_name = {"admin_gag_name","0",FCVAR_EXTDLL, 0};
cvar_t admin_gag_sayteam = {"admin_gag_sayteam","0",FCVAR_EXTDLL, 0};
cvar_t admin_highlander = {"admin_highlander","0",FCVAR_SERVER, 0};
cvar_t admin_ignore_immunity = {"admin_ignore_immunity","0",FCVAR_SERVER, 0};
cvar_t admin_parse_logs = {"admin_parse_logs","0",FCVAR_EXTDLL, 0};
cvar_t admin_plugin_file = {"admin_plugin_file","0",FCVAR_EXTDLL};
cvar_t admin_reconnect_timeout = {"admin_reconnect_timeout","300",FCVAR_EXTDLL};
cvar_t admin_reject_msg = {"admin_reject_msg","0",FCVAR_EXTDLL};
cvar_t admin_repeat_msg = {"admin_repeat_msg","This server is using Admin Mod",FCVAR_EXTDLL};
cvar_t admin_repeat_freq = {"admin_repeat_freq","600",FCVAR_EXTDLL, 600};
cvar_t admin_quiet = {"admin_quiet","0",FCVAR_SERVER};
cvar_t admin_vault_file = {"admin_vault_file","0",FCVAR_EXTDLL};
cvar_t admin_version = {"admin_mod_version",MOD_VERSION,FCVAR_SERVER};    
cvar_t admin_vote_autostart = {"admin_vote_autostart","0",FCVAR_EXTDLL};
cvar_t admin_vote_echo = {"admin_vote_echo","0",FCVAR_EXTDLL};
cvar_t admin_vote_freq = {"admin_vote_freq","600",FCVAR_EXTDLL};
cvar_t admin_vote_maxextend = {"admin_vote_maxextend","0",FCVAR_EXTDLL};
cvar_t admin_vote_ratio = {"admin_vote_ratio","0",FCVAR_EXTDLL};
cvar_t ami_sv_maxplayers = {"ami_sv_maxplayers","",FCVAR_SERVER, 0};
cvar_t amv_anti_cheat_options = {"amv_anti_cheat_options","",FCVAR_EXTDLL, 0};
cvar_t amv_default_config_dir = {"amv_default_config_dir", "addons/adminmod/config", FCVAR_EXTDLL, 1};
cvar_t amv_enable_beta = {"amv_enable_beta","",FCVAR_EXTDLL, 0};
cvar_t amv_hide_reserved_slots = {"amv_hide_reserved_slots","1",FCVAR_EXTDLL, 1};
cvar_t amv_log_passwords = {"amv_log_passwords","0",FCVAR_EXTDLL, 0};
cvar_t amv_private_server = {"amv_private_server","0",FCVAR_SERVER, 0};
cvar_t amv_prvt_kick_message = {"amv_prvt_kick_message","0",FCVAR_EXTDLL, 0};
cvar_t amv_reconnect_time = {"amv_reconnect_time","0",FCVAR_EXTDLL, 0};
cvar_t amv_register_cmds = {"amv_register_cmds","",FCVAR_EXTDLL, 0};
cvar_t amv_vote_duration = {"amv_vote_duration","30",FCVAR_EXTDLL, 30};
cvar_t allow_client_exec = {"allow_client_exec","0",FCVAR_SERVER};
cvar_t default_access = {"default_access","0",FCVAR_SERVER};
cvar_t encrypt_password = {"encrypt_password","1",FCVAR_EXTDLL}; // use encrypted ones by default
cvar_t file_access_read = {"file_access_read","0",FCVAR_EXTDLL};
cvar_t file_access_write = {"file_access_write","0",FCVAR_EXTDLL};
cvar_t help_file = {"help_file","0",FCVAR_EXTDLL};
cvar_t ips_file = {"ips_file","0",FCVAR_EXTDLL};
cvar_t kick_ratio = {"kick_ratio","0",FCVAR_EXTDLL};
cvar_t map_ratio = {"map_ratio","0",FCVAR_EXTDLL};
cvar_t maps_file = {"maps_file","0",FCVAR_EXTDLL}; 
cvar_t models_file  = {"models_file","0",FCVAR_EXTDLL};
cvar_t models_kick_msg = {"models_kick_msg","0",FCVAR_EXTDLL};
cvar_t nicks_kick_msg = {"nicks_kick_msg","0",FCVAR_EXTDLL}; 
cvar_t password_field = {"password_field","0",FCVAR_EXTDLL};
cvar_t pretty_say = {"pretty_say","1",FCVAR_EXTDLL};
cvar_t public_slots_free = {"public_slots_free","0",FCVAR_SERVER};
cvar_t reserve_slots = {"reserve_slots","0",FCVAR_SERVER};
cvar_t reserve_slots_msg = {"reserve_slots_msg","0",FCVAR_EXTDLL};
cvar_t reserve_type = {"reserve_type","0",FCVAR_SERVER};
cvar_t script_file = {"script_file","0",FCVAR_EXTDLL};
cvar_t use_regex= {"use_regex","0",FCVAR_EXTDLL};
cvar_t users_file = {"users_file","0",FCVAR_EXTDLL}; 
cvar_t vote_freq = {"vote_freq","0",FCVAR_EXTDLL};
cvar_t words_file = {"words_file","0",FCVAR_EXTDLL};
#ifdef USE_MYSQL
    cvar_t mysql_database = {"mysql_database","0",FCVAR_EXTDLL};  // Database to connect to
	cvar_t mysql_dbtable_ips = {"mysql_dbtable_ips","0",FCVAR_EXTDLL};
 	cvar_t mysql_dbtable_models = {"mysql_dbtable_models","0",FCVAR_EXTDLL};
	cvar_t mysql_dbtable_plugins = {"mysql_dbtable_plugins","0",FCVAR_EXTDLL};
	cvar_t mysql_dbtable_users = {"mysql_dbtable_users","0",FCVAR_EXTDLL};
	cvar_t mysql_dbtable_tags = {"mysql_dbtable_tags","0",FCVAR_EXTDLL};

	cvar_t mysql_dbtable_words = {"mysql_dbtable_words","0",FCVAR_EXTDLL};
	cvar_t mysql_host = {"mysql_host","0",FCVAR_EXTDLL};
	cvar_t am_mysql_port = {"mysql_port","0", FCVAR_EXTDLL, 0};
	cvar_t mysql_pass = {"mysql_pass","0",FCVAR_EXTDLL};
	cvar_t mysql_user = {"mysql_user","0",FCVAR_EXTDLL};

	cvar_t mysql_preload = {"mysql_preload","1",FCVAR_EXTDLL};

	cvar_t mysql_users_sql = {"mysql_users_sql","SELECT pass,access FROM %s where nick='%s' or nick='%s'",FCVAR_EXTDLL};
	cvar_t mysql_tags_sql = {"mysql_tags_sql","SELECT pass,access FROM %s where '%s' REGEXP nick or nick='%s'",FCVAR_EXTDLL};

#endif

#ifdef USE_PGSQL
	cvar_t pgsql_database = {"pgsql_database","0",FCVAR_EXTDLL};  // Database to connect to
	cvar_t pgsql_dbtable_ips = {"pgsql_dbtable_ips","0",FCVAR_EXTDLL};
 	cvar_t pgsql_dbtable_models = {"pgsql_dbtable_models","0",FCVAR_EXTDLL};
	cvar_t pgsql_dbtable_plugins = {"pgsql_dbtable_plugins","0",FCVAR_EXTDLL};
	cvar_t pgsql_dbtable_users = {"pgsql_dbtable_users","0",FCVAR_EXTDLL};
	cvar_t pgsql_dbtable_tags = {"pgsql_dbtable_tags","0",FCVAR_EXTDLL};

	cvar_t pgsql_dbtable_words = {"pgsql_dbtable_words","0",FCVAR_EXTDLL};
	cvar_t pgsql_host = {"pgsql_host","0",FCVAR_EXTDLL};
	cvar_t pgsql_port = {"pgsql_port","5432", FCVAR_EXTDLL, 5432};
	cvar_t pgsql_pass = {"pgsql_pass","0",FCVAR_EXTDLL};
	cvar_t pgsql_user = {"pgsql_user","0",FCVAR_EXTDLL};

	cvar_t pgsql_preload = {"pgsql_preload","1",FCVAR_EXTDLL};

	cvar_t pgsql_users_sql = {"pgsql_users_sql","SELECT pass,access FROM %s where nick='%s' or nick='%s'",FCVAR_EXTDLL};
	cvar_t pgsql_tags_sql = {"pgsql_tags_sql","SELECT pass,access FROM %s where '%s' ~ nick or nick='%s'",FCVAR_EXTDLL};

#endif

#ifdef USE_MYSQL
  MYSQL mysql;
  BOOL g_fUseMySQL = FALSE;
#endif

#ifdef USE_PGSQL
  PGconn *pgsql;
  BOOL g_fUsePgSQL = FALSE;
#endif

/* direct access to cvars. They get registered in h_export.cpp */
cvar_t* ptAM_devel = nullptr;
cvar_t* ptAM_debug = nullptr;
//cvar_t* ptAM_autoban = 0;
cvar_t* ptAM_botProtection = nullptr;
cvar_t* ptAM_reserve_slots = nullptr;
cvar_t* ptAM_hide_reserved_slots = nullptr;


int g_NameCrashAction = 0;
int g_PreventClientExploit = 1;
int g_SpectatorCheatAction = 0;
int g_msgTextMsg = 0;


// file scope variables
static bool s_bInitCheck = true;
static bool s_bMECheck = true;
static bool s_bConfigRead = false;
static unsigned int s_uiFCount = 0;
static unsigned int s_uiDequeueTimeout = 0;


// Forward declaration of functions.
unsigned int me_log_fix( bool Log, bool Fix );


void AM_AdminCommand() {  
  AM_ClientCommand(nullptr);
}

int AM_ClientCommand( edict_t *pEntity ) {
  const char *pcmd = CMD_ARGV(0);
  int iIndex;
  cell cReturn = 0;
  char commandline[255];	
  char* admin_command = commandline;

  pAdminEnt=pEntity;
  
  if (!g_fInitialized)
    AM_ClientStart(pEntity);  

  DEBUG_LOG(4, ("ClientCommand: '%s' - '%s'", pcmd, CMD_ARGS()) );

  const char* pcAuthId = GETPLAYERAUTHID( pEntity ); // pcAuthId not used? [APG]RoboCop[CL]

  commandline[0] = 0;
  if ( CMD_ARGS() ) strncpy( commandline, CMD_ARGS(), 255 );
 
	handle_unprintables( commandline );
 
  /* Do the admin_command check - strip out admin_command if it
     prepends the command.  This allows us to capture commands from
     the hlds console, etc. */
  if( FStrEq(pcmd,"admin_command") || FStrEq(pcmd,"admin_cmd") ) {
	  
	  pcmd=CMD_ARGV(1);
	  if ( (admin_command = strchr( commandline, ' ' )) == nullptr ) {
		  admin_command = strchr( commandline, 0 );
	  } else {
		  admin_command++;
	  }  // if-else
#if 0
	  int i;
	  strcpy(admin_command,"");
	  for(i=2;i<CMD_ARGC();i++) {
		  if((strlen(admin_command)+strlen(CMD_ARGV(i)))>255) 
			  break;
		  if(i!=2) 
			  strcat(admin_command," ");
		  strcat(admin_command,CMD_ARGV(i));
		  DEBUG_LOG(4, ("Arg %i: %s", i, CMD_ARGV(i)) );
	  }
#endif
  } else {
#if 0
	  /* TODO: This could probably be replaced with a simple strncpy,
		 but it's late, and I'm tired.  If you want, feel free. */
	  int i;
	  
	  strcpy(admin_command,"");
	  for(i=1;i<CMD_ARGC();i++) {
		  if((strlen(admin_command)+strlen(CMD_ARGV(i)))>255) 
			  break;
		  if(i!=1) 
			  strcat(admin_command," ");
		  strcat(admin_command,CMD_ARGV(i));
	  }
#endif // 0
  }
  
  /* Do our hardcoded checks (admin_password, admin_help, admin_version...)...
     note that admin_help and admin_version are only checked if we're using
     plugins */  
  if ( FStrEq(pcmd, "admin_login") || FStrEq(pcmd, "admin_password") ) {
    if (pAdminEnt == nullptr) {
      UTIL_LogPrintf("Laf. Why are you trying to enter a password?  You're at the console!\n");
    } else if ( CMD_ARGC() == 2) {
      SetUserPassword(STRING(pEntity->v.netname),const_cast<char*>(CMD_ARGV(1)),pEntity);
      VerifyUserAuth(STRING(pEntity->v.netname), pEntity);
    } else {
      CLIENT_PRINTF( pEntity, print_console,"Bad admin_password format, try: admin_password <password>\n");
    }
    return RESULT_HANDLED;
  } else if (g_fRunPlugins && !stricmp(pcmd, "admin_help")) {
    /* This never gets passed back to the engine,
       so ignore the return value. */
    HandleHelp(pAdminEnt,admin_command,0);
    return RESULT_HANDLED;
	/* Thrown in for HLRat.  admin_help, but formatted a bit nicer for machines,
	as opposed to humans. */
  } else if (g_fRunPlugins && !stricmp(pcmd, "admin_help_hlrat")) {
    /* This never gets passed back to the engine,
       so ignore the return value. */
    HandleHelp(pAdminEnt,admin_command,1);
    return RESULT_HANDLED;
  } else if ( stricmp(pcmd, "admin_version") == 0) {
	System_Response(UTIL_VarArgs( "Admin Mod version %s\n",MOD_VERSION), pEntity);
    /* This never gets passed back to the engine,
       so ignore the return value. */
    if ( cvar_file_is_set("admin_plugin_file") ) HandleVersion(pAdminEnt);
	else System_Response( "Admin Mod plugins are disabled.\n", pEntity );

    return RESULT_HANDLED;
  } else if ( 0 == stricmp(pcmd, "admin_adm")) {
    /* This never gets passed back to the engine,
       so ignore the return value. */
	  if ( admin_command != nullptr && strcasecmp( admin_command, "mefix") == 0 ) {
		  const int ret = me_log_fix( false, true );
		  if ( ret ) {
			  System_Response( UTIL_VarArgs("Attempted to fix %i suspicious entities.\n", ret), pAdminEnt );
		  } else {
			  System_Response( "No suspicious entities found to be fixed.\n", pAdminEnt );
		  }  // if
	  }  // if
    return RESULT_HANDLED;
  } else if ( 0 == stricmp(pcmd, "admin_status")) {
	  // print some information on the requested user
	  int i_indx;
	  if ( admin_command == nullptr || pAdminEnt != nullptr || *admin_command == '\0' || !stricmp(admin_command,"am i") ) {
		  i_indx = ENTINDEX( pEntity );
	  } else {
		  i_indx = GetPlayerIndex( admin_command );
	  }  // if-else
	  char acString[1024];
	  if ( i_indx == 0 ) {
		  snprintf( acString, sizeof(acString), "[ADMIN] Status: No such player: '%s'\n", admin_command );
	  } else {
		  snprintf( acString, sizeof(acString), "[ADMIN] Status: Name: '%s', Access: %i \n", g_AuthArray[i_indx].sUserName, g_AuthArray[i_indx].iAccess );
	  }  // if-else
	  System_Response( acString, pAdminEnt );
    return RESULT_HANDLED;
  }
  
  if (g_fRunPlugins) {
    /* A non-zero return indicates that we should stop processing...
       anything else means keep on truckin'. */
	// This script is maybe considered a bug [APG]RoboCop[CL]
	  DEBUG_LOG(4, ("'%s' - '%s'", pcmd, admin_command) );
    const plugin_result tprResult = HandleCommand(pAdminEnt, const_cast<char*>(pcmd), admin_command);
	  if ( tprResult == PLUGIN_HANDLED || tprResult == PLUGIN_NO_ACCESS ) {
		  return RESULT_HANDLED;
	  }  // if
  } else {
	  const char *program_file=const_cast<char *>(CVAR_GET_STRING("script_file"));
    
    if(program_file==nullptr|| FStrEq(program_file,"0") || !g_fRunScripts) {
      UTIL_LogPrintf( "[ADMIN] Scripting is disabled. (No mono-script file defined. (cvar script_file))\n");			
    } else {
	    int iError = amx_FindPublic(&amx, "client_commands", &iIndex);
      if (iError != AMX_ERR_NONE) {
	UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't find 'client_commands' proc, error #%i\n",iError);
      } else { 
	if(pEntity!=nullptr) 
	  iError = amx_Exec(&amx, &cReturn, iIndex, 5, pcmd, CMD_ARGS(), STRING(pEntity->v.netname), GETPLAYERUSERID(pEntity), GETPLAYERWONID(pEntity));
	else
	  iError = amx_Exec(&amx, &cReturn, iIndex, 5, pcmd, admin_command,"Admin",-1,-1); 
	
	if (iError != AMX_ERR_NONE)
	  UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't run 'client_commands' proc, error #%i\n",iError);
	
	if ( cReturn==1) {
	  return RESULT_HANDLED;
	}
      }
    }
  }
  
  if  (FStrEq( pcmd, "menuselect") && CMD_ARGC() >= 2) {
    CTimer *pTimer = static_cast<CTimer *>(GET_PRIVATE(pTimerEnt));
    if (pTimer->VoteInProgress()) {
      int i_index;
      const int iVote = atoi(CMD_ARGV(1));
      
      if (pAdminEnt == nullptr) {
				i_index = 0;
      } else {
				i_index = ENTINDEX(pAdminEnt);
      }
      if (pTimer->GetPlayerVote(i_index) != 0) {
				// This person has either already voted, or
				// was not shown the vote.  Either way, we ignore it.
				// Do nothing.
      } else if (iVote > pTimer->GetMaxVoteChoice()) {
				// The vote is out of range.  Ignore it.
			} else {
				pTimer->SetPlayerVote(i_index, iVote);
				System_Response(UTIL_VarArgs("[ADMIN] Vote entered for option #%i\n",atoi(CMD_ARGV(1))),pAdminEnt);
	
				if(static_cast<int>(CVAR_GET_FLOAT("admin_vote_echo")) != 0) {
					if (pAdminEnt == nullptr) {
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%s voted for option #%i\n","Admin",atoi(CMD_ARGV(1))));
					} else {
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%s voted for option #%i\n",STRING(pAdminEnt->v.netname),atoi(CMD_ARGV(1))));
					}
				}
				return RESULT_HANDLED;
      }
    }
  }
  
  // If we got this from console (via admin_command), don't send it on anywhere else.
  if (pAdminEnt == nullptr) {
    return RESULT_HANDLED;
  } else {
    return RESULT_CONTINUE;
  }
}

//
// A static global to transfer the return value from ClientConnect() to ClientConnect_Post()
static BOOL bAM_ClientConnectRetval;
// A macro to return and store the return value
#define CLIENT_CONNECT_RETURN( a ) bAM_ClientConnectRetval = (a); return a

BOOL AM_ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ], bool bForce ) {    
  char* infobuffer = nullptr;
  char sIP[IPPORT_SIZE], *p;
  char sModel[MODELNAME_SIZE];
  char sName[USERNAME_SIZE];
  const int iIndex = ENTINDEX(pEntity);
  user_struct tUser;
  // lazy man's pointer :)
  unique_ptr<AMConnectingPlayer> poPlayerData = std::make_unique<AMConnectingPlayer>(); //TODO: Experimental [APG]RoboCop[CL]
 
  pAdminEnt = pEntity;  
  poPlayerData->pEntity = pEntity;

  memset( sIP, 0, sizeof(sIP) );
  memset( sModel, 0, MODELNAME_SIZE );
  memset( sName, 0, USERNAME_SIZE );
  memset( &tUser, 0, sizeof(user_struct) );

  if ( pEntity ) infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity);  
  if ( infobuffer ) am_strncpy(sModel,g_engfuncs.pfnInfoKeyValue(infobuffer,"model"),MODELNAME_SIZE);
  if ( pszName ) {
	am_strncpy( sName, pszName, USERNAME_SIZE );
	am_strncpy( poPlayerData->acName, pszName, USERNAME_SIZE );
 } 
 if ( pszAddress ) {
	if ( 0 == strcmp(pszAddress, "loopback") ) pszAddress = "127.0.0.1:27005";
	am_strncpy( sIP, pszAddress, sizeof(sIP) );
	am_strncpy( poPlayerData->acIPPort, pszAddress, IPPORT_SIZE );
 }

  DEBUG_LOG(1, ("AM_ClientConnect: '%s'  from %s slot:%i %s", sName, sIP, iIndex, bForce?"  (forced)":"") );


  if ( make_friendly(sName, TRUE) == 2 ) {
    strcpy(szRejectReason,"\n[ADMIN] Your name has invalid characters. Connection denied.");
	CLIENT_PRINTF( pAdminEnt, print_console, UTIL_VarArgs("%s\n", szRejectReason) );
    UTIL_LogPrintf("[ADMIN] Client with non printing characters in name connected: '%s'.\n", sName);
    UTIL_LogPrintf("[ADMIN] Client dropped for security reasons.\n");
    CLIENT_CONNECT_RETURN(FALSE);
  };

  if (!g_fInitialized) {
    AM_ClientStart(pEntity);
  }  // if

  const char* pcAuthId = GETPLAYERAUTHID( pEntity );
  AMAuthId oaiAuthID( pcAuthId );
  // In case this is the listenserver user, we set the authid to STEAM_ID_LOOPBACK
  // unless we already have a loopback id, i.e. VALVE_ID_LOOPBACK
  if ( (! IS_DEDICATED_SERVER()) && (!oaiAuthID.is_loopid()) && nullptr != pszAddress) {
	  if ( 0 == strcmp("127.0.0.1:27005", pszAddress) && 1 == GETPLAYERUSERID(pEntity) ) {
		  oaiAuthID = "STEAM_ID_LOOPBACK";
	  }
  }

  QUEUEING_DEBUGF( ("|---> Queueing check: sv_lan is %u, player authid %s [%s] is found to be %spending.\n", (int)CVAR_GET_FLOAT("sv_lan"), pcAuthId, (const char*)oaiAuthID, AMAuthId::is_pending(pcAuthId)?"":"not ") );

  if ( (!static_cast<int>(CVAR_GET_FLOAT("sv_lan"))) &&  AMAuthId::is_pending(pcAuthId) ) {
	  QUEUEING_DEBUGF( ("|---> Player Steam Id (ClientConnect) is pending.\n") );
	  // check if we have him already queued
	  if ( !g_ovcPendingPlayers.empty() ) {
		  bool bQueued = false;
		  deque<AMConnectingPlayer*>::iterator it = g_ovcPendingPlayers.begin();
		  const deque<AMConnectingPlayer*>::iterator itEnd = g_ovcPendingPlayers.end();
		  for ( it; it < itEnd; ++it ) {
			  if ( (*it)->pEntity == poPlayerData->pEntity ) {
				  bQueued = true;
				  break;
			  }  // if
		  }  // for

          if ( !bQueued ) {
              // Enqueue player
              g_ovcPendingPlayers.emplace_back( poPlayerData.release() );
              // Set timeout during which we do not dequeue players
              s_uiDequeueTimeout = s_uiFCount + 100000;
        	  QUEUEING_DEBUGF( ("|---> Player Steam Id (ClientConnect) Enqueued player (%p).\n", pEntity) );
          }
	  } else {
          // Enqueue player
          g_ovcPendingPlayers.emplace_back( poPlayerData.release() );
          // Set timeout during which we do not dequeue players.
          s_uiDequeueTimeout = s_uiFCount + 100000;
	      QUEUEING_DEBUGF( ("|---> Player Steam Id (ClientConnect) Enqueued player (%p).\n", pEntity) );
	  }  // if-else

	// Don't do any checks if the user's id isn't validated, yet.
	CLIENT_CONNECT_RETURN(TRUE);
  }  // if

  AddUserAuth( sName, sIP, pEntity );

  if ( p = strchr(sIP,':') ) {
    *p = '\0';
  }  // if

  char szCommand[128];
  const char* sPasswordField = CVAR_GET_STRING("password_field");

  snprintf(szCommand,128,"setinfo %s \"\"\n",sPasswordField);
  CLIENT_COMMAND(pEntity,szCommand);


  // check if we restrict access only to users who are in users.ini or ips.ini
  // Listenserver users are not restricted
  if ( !oaiAuthID.is_loopid() && static_cast<int>(CVAR_GET_FLOAT("amv_private_server")) != 0 ) {

	  // check if the user is listed in our user list
	  // or if his IP is listed in our IPs list
	  if ( !(GetUserRecord(sName, oaiAuthID, sIP, g_AuthArray[iIndex].sPassword, nullptr))
		   && !(IsIPReserved(sIP)) ) { 

		  const char* sRsvdRejectMsg = get_cvar_string_value( "amv_prvt_kick_message", true );
		  
		  if ( sRsvdRejectMsg == nullptr ) {
			  strcpy(szRejectReason,"\n[ADMIN] Only registered users are allowed on this server.");
		  } else {
			  snprintf( szRejectReason, 128, "\n%s", sRsvdRejectMsg);
		  }
		  DEBUG_LOG( 1, ("Access to private server denied for user '%s'", sName) );
			CLIENT_PRINTF( pAdminEnt, print_console, UTIL_VarArgs("%s\n", szRejectReason) );
		  CLIENT_CONNECT_RETURN(FALSE);
	  }  // if
		  DEBUG_LOG( 1, ("Access to private server granted to user '%s'", sName) );
  }  // if


  // check if the name is reserved, does not apply to listenserver users
  if ( !oaiAuthID.is_loopid() && IsNameReserved(sName, oaiAuthID, sIP, &tUser) ) {
	  if ( ( (tUser.iAccess >= 0) && (((GetUserAccess(pEntity) & ACCESS_RESERVE_NICK) != ACCESS_RESERVE_NICK)
		   	|| !(pass_compare(tUser.sPassword, g_AuthArray[iIndex].sPassword))) )
		   || ( (tUser.iAccess < 0) && (strcmp(tUser.sPassword,pcAuthId) != 0) )  ){

		  const char* sNickRejectMsg = get_cvar_string_value( "nicks_kick_msg", true );
		  
		  if ( sNickRejectMsg == nullptr ) {
			  strcpy(szRejectReason,"\n[ADMIN] Your nickname or WONID is reserved on this server." );
		  } else {
			  snprintf( szRejectReason, 128, "\n%s", sNickRejectMsg);
		  }  // if-else
		  DEBUG_LOG( 1, ("Access to nickname denied for user '%s'", sName) );
			CLIENT_PRINTF( pAdminEnt, print_console, UTIL_VarArgs("%s\n", szRejectReason) );
		  CLIENT_CONNECT_RETURN(FALSE);
	  }  // if
		  DEBUG_LOG( 1, ("Access to nickname granted to user '%s'", sName) );
  }  // if
  

  // check if the model is reserved, does not apply to listenserver users
  if ( !oaiAuthID.is_loopid() && IsModelReserved(sModel) ) {
    if (GetModelAccess(sModel, pEntity)==0) {
      const char* sModelRejectMsg = get_cvar_string_value( "models_kick_msg", true );
      
      if ( sModelRejectMsg == nullptr ) {
		strcpy(szRejectReason,"\n[ADMIN] That model is reserved on this server.");
      } else {
		snprintf( szRejectReason, 128, "\n%s", sModelRejectMsg);
      }
	  DEBUG_LOG( 1, ("Access to model '%s' denied for user '%s'", sModel, sName) );
      CLIENT_PRINTF( pAdminEnt, print_console, UTIL_VarArgs("%s\n", szRejectReason) );
      CLIENT_CONNECT_RETURN(FALSE);
    }
  }

  const int iFreeSlots = GetFreeSlots( pEntity );
  const int iResType = static_cast<int>(CVAR_GET_FLOAT("reserve_type"));
	int iResTaken = 0;
	DEBUG_LOG(1, ("%i players / %i free spots / %i max spots.", GetPlayerCount(), iFreeSlots, gpGlobals->maxClients) );

	if (iFreeSlots > 0 && iResType != 2) {
		// We don't need to bother checking for anything; reserve type
		// isn't 2 (where you take a reserve slot no matter what),
		// and we have a free slot.
	} else {

		// Otherwise, check to see if this person has reserve spot access.
		if (IsIPReserved(sIP) || ((GetUserAccess(pEntity) & ACCESS_RESERVE_SPOT) == ACCESS_RESERVE_SPOT)) {
			// They do.  Tell them they're taking a reserve spot.
			CLIENT_PRINTF( pAdminEnt, print_console,"[ADMIN] Reserved spot taken.\n");
			UTIL_LogPrintf("[ADMIN] Reserved spot taken by: %s\n",sName);
			iResTaken = 1;
		} else if (iFreeSlots <= 0) {
			// They don't have reserve access.  Check the free spots (we
			// still might get here with some free spots open, if we have
			// reserve type 2).  If there are no free spots, kick 'em.
			const char* sReserveMsg = get_cvar_string_value( "reserve_slots_msg", true );
			const int iResSlots = static_cast<int>(CVAR_GET_FLOAT("reserve_slots"));
			
			if ( sReserveMsg == nullptr ) {
				snprintf(szRejectReason, 128, "\n[ADMIN] %d of the %d slots on this server are reserved,\n[ADMIN] and no unreserved slots are left. Try again later.\n", iResSlots, gpGlobals->maxClients);
			} else {
				snprintf( szRejectReason, 128, "\n%s", sReserveMsg);
			}  // if-else

      		CLIENT_PRINTF( pAdminEnt, print_console, UTIL_VarArgs("%s\n", szRejectReason) );
			CLIENT_CONNECT_RETURN(FALSE);
		}  // if-else
	}  // if-else


  // If we have reserve type one, and this person took the reserve spot,
  // we need to kick the person with the highest ping.
  if (iResType == 1 && iResTaken == 1) {
	DEBUG_LOG(3, ("KickHighestPinger() triggered in ClientConnect()") );
	  KickHighestPinger(sName,sIP,pEntity);
  }  // if
  
  if(g_fRunPlugins) {
    // We should never not pass this back to the engine,
    // so ignore the return value.
    HandleConnect(pEntity, sName, const_cast<char*>(pszAddress));
  } else if (g_fRunScripts) {
    // now run the script
    char *program_file=const_cast<char *>(CVAR_GET_STRING("script_file"));
    
    if(program_file==nullptr|| FStrEq(program_file,"0")) {
      UTIL_LogPrintf( "[ADMIN] Scripting is disabled. (No mono-script file defined. (cvar script_file))\n");
    } else {
	    int i_index;
      cell cReturn = 0;
      
      int iError = amx_FindPublic(&amx, "client_connect", &i_index);
      if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't find 'client_connect' proc, error #%i\n",iError);
      } else {
		iError = amx_Exec(&amx, &cReturn, i_index, 2, sName,pszAddress);	    
		if (iError != AMX_ERR_NONE) {
		  UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't run 'client_connect' proc, error #%i\n",iError);
		}
      }
    }
  }
  
  if (iResTaken==1) {
    CVAR_SET_FLOAT("public_slots_free",iFreeSlots);
    //CVAR_SET_FLOAT("sv_visiblemaxplayers",iFreeSlots);
  } else {
    CVAR_SET_FLOAT("public_slots_free",iFreeSlots - 1);
    //CVAR_SET_FLOAT("sv_visiblemaxplayers",iFreeSlots-1);
  }

  DEVEL_LOG(1, ("AM_ClientConnect finished.\n", sName) );

  CLIENT_CONNECT_RETURN(TRUE);
}

BOOL AM_ClientConnect_Post( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ) {    
  
  pAdminEnt = pEntity;  

  DEBUG_LOG(3, ("AM_ClientConnect_Post") );

  // Notify connecting players when allow_client_exec is set
  if ( TRUE == bAM_ClientConnectRetval && static_cast<int>(CVAR_GET_FLOAT("allow_client_exec")) == 1 ) {
	  //System_Response( "**\n", pAdminEnt );	  
	  System_Response( "************** NOTICE >>>>>>>>>>>>\n", pAdminEnt );	  
	  System_Response( const_cast<char*>(get_am_string(nullptr,0,statstr[9],statstr_table)), pAdminEnt );
	  System_Response( "************** NOTICE <<<<<<<<<<<<\n", pAdminEnt );	  
  }  // if


  DEBUG_LOG(3, ("AM_ClientConnect_Post finished.\n") );

  return TRUE;
}

int AM_ClientDisconnect( edict_t* pEntity ) {
    
  DEBUG_LOG(1, ("AM_ClientDisconnect: %s\n", STRING(pEntity->v.netname)) );

  const int iIndex = ENTINDEX( pEntity ); 
  const char* pcAuthId = GETPLAYERAUTHID( pEntity ); // pcAuthId not used? [APG]RoboCop[CL]

  if(g_fRunPlugins) {
    // We should never not pass this back to the engine,
    // so ignore the return value.
    HandleDisconnect(pEntity);
  }
  
  UpdateUserAuth(pEntity);

  // flag this client as dirty so CUIC() will not do checks
  g_AuthArray[iIndex].state = auth_struct::dirty; 

  // Reassign the Highlander in case the current Highlander is leaving.
  GetHighlanderIndex( pEntity );
  
  CVAR_SET_FLOAT("public_slots_free",GetFreeSlots(pEntity));
  //CVAR_SET_FLOAT("sv_visiblemaxplayers",GetFreeSlots(pEntity));
  return RESULT_CONTINUE;
}


void AM_ClientStart(edict_t *pEntity) {
	int iIndex;
  cell cReturn = 0;

  if ( g_fInitialized == TRUE) {
    return;
  }  // if

  const char* pcAuthId = GETPLAYERAUTHID( pEntity );// pcAuthId not used? [APG]RoboCop[CL]

  g_fInitialized = TRUE;
 
 
#ifdef USE_MYSQL
  const char* sDatabase = get_cvar_string_value( "mysql_database" ); 
  const char* sHost = get_cvar_string_value( "mysql_host" );
  const unsigned int port = (unsigned int)CVAR_GET_FLOAT("mysql_port");
  const char* sPassword = CVAR_GET_STRING("mysql_pass");
  const char* sUser = get_cvar_string_value( "mysql_user" );
  

  if ( g_fUseMySQL == FALSE && sHost != nullptr && sUser != nullptr) {
    g_fUseMySQL = TRUE;
    mysql_init( &mysql );
    mysql_real_connect( &mysql, sHost, sUser, sPassword, sDatabase, port, nullptr, 0 );
    
    if ( mysql_errno(&mysql) ) {
      UTIL_LogPrintf( "[ADMIN] ERROR: MySQL Error: %s\n", mysql_error(&mysql) );
      mysql_close( &mysql );
      g_fUseMySQL = FALSE;
      am_exit(1);
    }
  } else if ( g_fUseMySQL == TRUE ) {
    UTIL_LogPrintf( "[ADMIN] MySQL connection already established.\n" );
  } else {
    UTIL_LogPrintf( "[ADMIN] Not establishing MySQL connection: either host or user name is blank.\n" );
  }
#endif
  
#ifdef USE_PGSQL
  const char* psDatabase = get_cvar_string_value( "pgsql_database" ); 
  const char* psHost = get_cvar_string_value( "pgsql_host" );
  const char* psPort = CVAR_GET_STRING("pgsql_port");
  const char* psPassword = CVAR_GET_STRING("pgsql_pass");
  const char* psUser = get_cvar_string_value( "pgsql_user" );
  

  if ( g_fUsePgSQL == FALSE && psHost != nullptr && psUser != nullptr ) {
    g_fUsePgSQL = TRUE;
    pgsql = PQsetdbLogin(psHost, psPort, nullptr, nullptr, psDatabase, psUser, psPassword );
    
    if (PQstatus(pgsql) == CONNECTION_BAD) {
      UTIL_LogPrintf( "[ADMIN] ERROR: PgSQL Error: %s\n", PQerrorMessage(pgsql) );
      PQfinish( pgsql );
      g_fUsePgSQL = FALSE;
      am_exit(1);
    }
  } else if ( g_fUsePgSQL == TRUE ) {
    UTIL_LogPrintf( "[ADMIN] PgSQL connection already established.\n" );
  } else {
    UTIL_LogPrintf( "[ADMIN] Not establishing PgSQL connection: either host or user name is blank.\n" );
  }
#endif


  /*
  // parse the special optins cvar
  const char* pcOptions = CVAR_GET_STRING("amv_anti_cheat_options");
  const char* pcOpt = strstr( pcOptions, "nc" );
  if ( pcOpt != 0 && (pcOpt == pcOptions || *(pcOpt-1) == ':') ) {
	  g_NameCrashAction = atoi( pcOpt + 2 );
  }  // if
  pcOpt = strstr( pcOptions, "sp" );
  if ( pcOpt != 0 && (pcOpt == pcOptions || *(pcOpt-1) == ':') ) {
	  g_SpectatorCheatAction = atoi( pcOpt + 2 );
  }  // if
  */
  g_NameCrashAction = get_option_cvar_value( "amv_anti_cheat_options", "nc", 0 );
  g_SpectatorCheatAction = get_option_cvar_value( "amv_anti_cheat_options", "sp", 0 );
  g_PreventClientExploit = get_option_cvar_value( "amv_anti_cheat_options", "prex", 1 ); 

  if ( strcmp("tfc", GetModDir()) != 0 ) {
	  g_SpectatorCheatAction = 0;
  }  // if

  DEBUG_LOG(2, ("NameCrashAction: %d, SpectatorCheatAction: %d", g_NameCrashAction, g_SpectatorCheatAction) );


  InitSpawnEntityList();
  
  LoadIPs();
  LoadModels();
  LoadUsers();
  LoadVault();
  LoadWords();
  
  pAdminEnt=pEntity;
  
  if(program!=nullptr)
    free(program);

  const int istr = MAKE_STRING("adminmod_timer");
  
  pTimerEnt = CREATE_NAMED_ENTITY(istr);
  if ( FNullEnt( pTimerEnt ) ) {
    UTIL_LogPrintf("[ADMIN] ERROR: nullptr Ent for adminmod_timer\n" );
    am_exit(1);
  }
  
  DispatchSpawn(pTimerEnt);  

    pTimerEnt->v.origin =  Vector(0,0,0);
    pTimerEnt->v.euser1 = nullptr;
    pTimerEnt->v.angles = Vector(0,0,0);
    pTimerEnt->v.velocity = Vector(0,0,0);
        pTimerEnt->v.takedamage = DAMAGE_NO;
        pTimerEnt->v.health = 100000;
        pTimerEnt->v.movetype = MOVETYPE_NONE;
        pTimerEnt->v.sequence = 0;
        pTimerEnt->v.framerate = 1.0;   
        pTimerEnt->v.solid = SOLID_NOT;  
   // SET_MODEL(pMediFlag, "models/??");
    UTIL_SetSize(VARS(pTimerEnt), Vector(0, 0, 0), Vector(0, 0, 0));
    pTimerEnt->v.classname = MAKE_STRING("adminmod_timer");


  CBaseEntity *pTimer = static_cast<CBaseEntity *>(GET_PRIVATE(pTimerEnt));
  if (pTimer) { // run the ptimer spawn
    pTimer->Spawn();   
  }
  
  //pTimer->edict()->v.owner = nullptr;
  //pTimerEnt->v.origin.x = 9000;
  //pTimerEnt->v.origin.y = 9000;
  //pTimerEnt->v.origin.z = 9000;
  //pTimer->edict()->v.angles.x = 0;
  //pTimer->edict()->v.angles.y = 0;
  //pTimer->edict()->v.angles.z = 0;
  //pTimerEnt->v.spawnflags |= SF_NORESPAWN;


  //(*g_engfuncs.pfnSetOrigin)(pTimerEnt, pTimerEnt->v.origin);
  

  // make sure that the password_field cvar is set and is prefixed with an underscore
  const char *passwd_field = get_cvar_string_value( "password_field", true );

  if( passwd_field == nullptr  ) {  // Default value "0", i.e. config file not read.
	UTIL_LogPrintf("\n[ADMIN] ERROR: ********************************************************\nThe configuration file for Admin Mod (default: adminmod.cfg) could not be read.\nMake sure that the Admin Mod configuration file is executed from server.cfg\nwhen you use Admin Mod.\nGo to http://www.adminmod.org/manual/ for more details.\n");
#ifdef WIN32
	MessageBox(nullptr,"[ADMIN] ERROR:\n\nThe configuration file for Admin Mod could not be read.\nMake sure that the Admin Mod configuration file (default: adminmod.cfg) is executed from server.cfg when you use Admin Mod.\nGo to http://www.adminmod.org/manual/ for more details.\n","ERROR",MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
#else
	fprintf(stderr,"\n[ADMIN] ERROR: ********************************************************\nThe configuration file for Admin Mod (default: adminmod.cfg) could not be read.\nMake sure that the Admin Mod configuration file is executed from server.cfg\nwhen you use Admin Mod.\nGo to http://www.adminmod.org/manual/ for more details.\n");
#endif
	am_exit(1);

  } else if ( passwd_field[0] == '\0' ) {  // Empty password_field.
	  UTIL_LogPrintf( "[ADMIN] WARNING: You have not set a password_field value. This is needed for your admins to securely authenticate with the server.\n");

  } else if ( passwd_field[0] != '_' ) {  // Not starting with an underscore.
	  UTIL_LogPrintf( "\n[ADMIN] ERROR: *********************************************************\nFor your own safety you are required to set the password_field cvar to a value\nwhich is prefixed with an underscore. This is to protect your admins' passwords\nfrom being distributed to other clients.\n\nChange the value for password_field in your adminmod.cfg file\nfrom '%s' to '_%s'.\nGo to http://www.adminmod.org/manual/ for more details.\n", passwd_field, passwd_field );
#ifdef WIN32
	  char acErrMsg[500];
	  snprintf( acErrMsg, 500, "[ADMIN] ERROR:\n\nFor your own safety you are required to set the password_field cvar to a value which is prefixed with an underscore. This is to protect your admins' passwords from being distributed to other clients.\n\nChange the value for password_field in your adminmod.cfg file from '%s' to '_%s'.\nGo to http://www.adminmod.org/manual/ for more details.\n", passwd_field, passwd_field );
	  MessageBox(nullptr, acErrMsg, "ERROR", MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
#else
	  fprintf(stderr,"\n[ADMIN] ERROR: *********************************************************\nFor your own safety you are required to set the password_field cvar to a value\nwhich is prefixed with an underscore. This is to protect your admins' passwords\nfrom being distributed to other clients.\n\nChange the value for password_field in your adminmod.cfg file\nfrom '%s' to '_%s'.\nGo to http://www.adminmod.org/manual/ for more details.\n", passwd_field, passwd_field );
#endif
	  am_exit(1);
  }  // if-else		

  const char* sPluginFile = get_cvar_file_value( "admin_plugin_file" );
  if ( sPluginFile == nullptr ) {
    const char *program_file = get_cvar_file_value( "script_file" );
    if ( program_file != nullptr ) {
	    /*TBR This isn't used anymore, we allow not using any plugins now
    if( program_file == nullptr ) {
#ifdef WIN32
      MessageBox(nullptr,"[ADMIN] ERROR:\n\nYou must define \"admin_plugin_file\" in your adminmod.cfg file before you can use Admin Mod.\nGo to http://www.adminmod.org/help/online/ for more details.\n","ERROR",MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
#else
      fprintf(stderr,"[ADMIN] ERROR: **********************************\n\nYou must define \"admin_plugin_file\" in your adminmod.cfg file before you can use Admin Mod.\nGo to http://www.adminmod.org/help/online/ for more details.\n");
#endif
      am_exit(1);
    }	
  */

		// reload the script file  
		program = LoadScript(&amx,program_file);
		if (program == nullptr) {
			UTIL_LogPrintf( "[ADMIN] ERROR: Unable to load script file '%s'\n",program_file);
		}


	    int iError = amx_FindPublic(&amx, "client_start", &iIndex);
		if (iError != AMX_ERR_NONE) {
			UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't find 'client_start' proc, error #%i\n", iError);
			return;
		}
		
		iError = amx_Exec(&amx, &cReturn, iIndex, 0);
		if (iError != AMX_ERR_NONE) {
			UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't run 'client_start' proc, error #%i\n", iError);
		} else {
			UTIL_LogPrintf("[ADMIN] Client start successful.\n");
		}
		g_fRunPlugins = FALSE;
	}
  } else {
    // Load the plugin file and get our actual plugin names.
    if ( !LoadPlugins() ) {
		UTIL_LogPrintf("[ADMIN] WARNING: No plugins were loaded. This may not be what you intended!\n");
    } else {
    	UTIL_LogPrintf("[ADMIN] Plugins loaded successfully.\n");
    	g_fRunPlugins = TRUE;
	}
  }
  g_fRunScripts = TRUE;
}

void AM_Stop() {

  if ( g_fInitialized != TRUE) {
    return;
  }  // if


  g_fInitialized = FALSE;
 
 
#ifdef USE_MYSQL
  if ( g_fUseMySQL == TRUE ) {
    mysql_close( &mysql );
    if ( mysql_errno(&mysql) ) {
      UTIL_LogPrintf( "[ADMIN] ERROR: MySQL Error: %s\n", mysql_error(&mysql) );
    }
    g_fUseMySQL = FALSE;
  }
#endif
  
#ifdef USE_PGSQL
  if ( g_fUsePgSQL == TRUE ) {
    PQfinish( pgsql );
    //if (PQstatus(pgsql) == CONNECTION_BAD) {
    //  UTIL_LogPrintf( "[ADMIN] ERROR: PgSQL Error: %s\n", PQerrorMessage(pgsql) );
    //}
    g_fUsePgSQL = FALSE;
  }
#endif

  DeleteSpawnEntityList();
  
  UnloadIPs();
  UnloadModels();
  UnloadUsers();
  UnloadVault();
  UnloadWords();
  
  UnloadPlugins(); 
  
  if(program!=nullptr)
    free(program);
  
  
  if ( ! FNullEnt( pTimerEnt ) ) {
  	REMOVE_ENTITY( pTimerEnt );
	DEBUG_LOG(2, ("Removing timer entity.") );
  }
  


  g_fRunPlugins = FALSE;
  g_fRunScripts = FALSE;
}


int AM_ClientUserInfoChanged( edict_t *pEntity, char *infobuffer, bool bForce ) {

  const int c_KICK_MSG_LEN = 250;
  const int c_SERVER_CMD_LEN= 256;

  int iKick = 0;
  const int iIndex = ENTINDEX(pEntity);
  char szCommand[c_SERVER_CMD_LEN];
  char acKickMsg[c_KICK_MSG_LEN]; 
  char sModel[MODELNAME_SIZE];
  char sName[USERNAME_SIZE];
  user_struct tUser;

  memset(szCommand, 0x0, c_SERVER_CMD_LEN);
  memset(sModel, 0x0, MODELNAME_SIZE);
  memset(sName, 0x0, USERNAME_SIZE);
  memset(&tUser, 0x0,sizeof(user_struct));

  am_strncpy(sName, g_engfuncs.pfnInfoKeyValue(infobuffer,"name"),USERNAME_SIZE);

  DEBUG_LOG(1, ("AM_ClientUserInfoChanged: %s slot: %i %s\n", sName, iIndex, bForce?"  (forced)":"") );


  // if the player has not been put into the server yet, we do not run anything in here
  if ( g_AuthArray[iIndex].state == auth_struct::dirty || pEntity->v.netname == 0 || FStrEq(STRING(pEntity->v.netname), "") ) {
    DEVEL_LOG(2, ("AM_ClientUserInfoChanged() called on unintialized player (state==%d), (netname==%i). Call ignored", g_AuthArray[iIndex].
state, pEntity->v.netname) );
    return RESULT_CONTINUE;
  }

  const char* pcAuthId = GETPLAYERAUTHID( pEntity );
  const AMAuthId oaiAuthID( pcAuthId );


  int retval = make_friendly(sName, TRUE);
  if ( retval == 2 ) {
    // the little brat tries to crash the clients
    UTIL_LogPrintf("[ADMIN] Player '%s' <%s> tried to crash clients with bad name.\n",STRING(pEntity->v.netname),
                   static_cast<const char*>(oaiAuthID));
    if ( g_NameCrashAction == 2 && static_cast<bool>(oaiAuthID) ) {
      SERVER_COMMAND( UTIL_VarArgs("banid 1440 %s\n", static_cast<const char*>(oaiAuthID)) );
      SERVER_COMMAND( UTIL_VarArgs("writeid\n") );
      UTIL_LogPrintf( "[ADMIN] Banned player with AuthID %s for 24h\n", static_cast<const char*>(oaiAuthID) );
    }  // if
    CLIENT_COMMAND ( pEntity, "quit\n" );
    return RESULT_HANDLED;
  }  // if
  

  if (!g_fInitialized) AM_ClientStart(pEntity);



  if ( (!static_cast<int>(CVAR_GET_FLOAT("sv_lan"))) && AMAuthId::is_pending(pcAuthId) ) {
/*TBR: Don't queue anything in here. We simply return and let CLientConnect handle the queueing.
	  //UTIL_LogPrintf( "|---> Player Steam Id (ClientUserInfoChanged) is pending.\n" );
	  // check if we have him already queued
	  if ( !g_ovcPendingPlayers.empty() ) {
		  bool bQueued = false;
		  deque<edict_t*>::iterator it = g_ovcPendingPlayers.begin();
		  deque<edict_t*>::iterator itEnd = g_ovcPendingPlayers.end();
		  for ( it; it < itEnd; ++it ) {
			  if ( *it == pEntity ) {
				  bQueued = true;
				  break;
			  }  // if
		  }  // for
          if ( !bQueued ) {
              // Enqueue the player
              g_ovcPendingPlayers.emplace_back( pEntity );
              // Set a timeout during which we do not dequeue players.
              s_uiDequeueTimeout = s_uiFCount + 100000;
              //UTIL_LogPrintf( "|---> Player Steam Id (ClientUserInfoChanged) Enqueued player. (%p)", pEntity );
              }
	  } else {
          // Enqueue the player
          g_ovcPendingPlayers.emplace_back( pEntity );
          // Set a timeout during which we do not dequeue players.
          s_uiDequeueTimeout = s_uiFCount + 100000;
          //UTIL_LogPrintf( "|---> Player Steam Id (ClientUserInfoChanged) Enqueued player. (%p)", pEntity );
	  }  // if-else
*/

	QUEUEING_DEBUGF( ("|---> Player Steam Id (ClientUserInfoChanged) is pending.\n") );
	// Don't do any checks if the user's id isn't validated, yet.
	return RESULT_CONTINUE;
  }  // if



  am_strncpy(sModel, g_engfuncs.pfnInfoKeyValue(infobuffer,"model"), MODELNAME_SIZE);
  
  // If the name has changed, we need to check it
  if(!FStrEq(sName, STRING(pEntity->v.netname)) || bForce) {
      // check if we restrict access only to users who are in users.ini or ips.ini
      // Listenserver users are not restricted
      if ( !oaiAuthID.is_loopid() && static_cast<int>(CVAR_GET_FLOAT("amv_private_server")) != 0 ) {

	      // check if the user is listed in our user list
	      // or if his IP is listed in our IPs list
	      if ( !(GetUserRecord(sName, oaiAuthID, g_AuthArray[iIndex].sIP, g_AuthArray[iIndex].sPassword, nullptr))
		       && !(IsIPReserved(g_AuthArray[iIndex].sIP)) ) { 

		      const char* sRsvdRejectMsg = get_cvar_string_value( "amv_prvt_kick_message", true );
		  
		      if ( sRsvdRejectMsg == nullptr ) {
				  am_strncpy( acKickMsg, "[ADMIN] Only registered nicknames are allowed on this server.", c_KICK_MSG_LEN );
		      } else {
				  am_strncpy( acKickMsg, sRsvdRejectMsg, c_KICK_MSG_LEN );
		      }
		      DEBUG_LOG( 1, ("Access to private server denied for user '%s'", sName) );
			  CLIENT_PRINTF( pEntity, print_console, UTIL_VarArgs("%s\n", acKickMsg) );
		      iKick = 1;
	      }  // if
	  	    DEBUG_LOG( 1, ("Access to private server granted to user '%s'", sName) );
      }  // if


      if ( 0 == iKick ) {
	      VerifyUserAuth(sName, pEntity);
	      if (IsNameReserved(sName, oaiAuthID, nullptr, &tUser)) {
		      if ( !oaiAuthID.is_loopid() &&
			       ( ( (tUser.iAccess >= 0) && 
			           ( ((GetUserAccess(pEntity) & ACCESS_RESERVE_NICK) != ACCESS_RESERVE_NICK) ||
				        !(pass_compare(tUser.sPassword, g_AuthArray[iIndex].sPassword)) 
				       )
				     ) ||
		             ( (tUser.iAccess < 0) && (strcmp(tUser.sPassword,GETPLAYERAUTHID(pEntity)) != 0) ) 
			        ) 
			      ) {
		  
			      const char* sNickRejectMsg = get_cvar_string_value( "nicks_kick_msg", true );
			  

			      if ( sNickRejectMsg == nullptr ) {
				      CLIENT_PRINTF(pEntity, print_console, "[ADMIN] Your name or WON ID is reserved on this server.\n");
				      am_strncpy( acKickMsg, "[ADMIN] Your name or WON ID is reserved on this server.", c_KICK_MSG_LEN );
			      } else {
				      CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("%s\n",sNickRejectMsg));
				      am_strncpy( acKickMsg, sNickRejectMsg, c_KICK_MSG_LEN );
			      }
			      DEBUG_LOG( 1, ("Access to nickname denied for user '%s'", sName) );
			      iKick = 1;
		      } else {
			      DEBUG_LOG( 1, ("Access to nickname granted to user '%s'", sName) );
                  // Update the auth record with the new name
                  am_strncpy( g_AuthArray[iIndex].sUserName, sName, USERNAME_SIZE );
		      }
	      }
      }
  }
  
  // If the model has changed, we need to check it
  if(!FStrEq(sModel, STRING(pEntity->v.model)) || bForce ) {
	  if ( !oaiAuthID.is_loopid() && IsModelReserved(sModel) ) {
		  if (GetModelAccess(sModel, pEntity)==0) {
			  const char* sModelRejectMsg = get_cvar_string_value( "models_kick_msg", true );
			  
			  if ( sModelRejectMsg == nullptr ) {
				  CLIENT_PRINTF(pEntity, print_console, "[ADMIN] That model is reserved on this server.\n");
				  am_strncpy( acKickMsg, "[ADMIN] That model is reserved on this server.", c_KICK_MSG_LEN );
			  } else {
				  CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("%s\n",sModelRejectMsg));
				  am_strncpy( acKickMsg, sModelRejectMsg, c_KICK_MSG_LEN );
			  }
			  iKick = 2;
			  DEBUG_LOG( 1, ("Access to model '%s' denied for user '%s'", sModel, sName) );
		  } else {
			  DEBUG_LOG( 1, ("Access to model '%s' granted to user '%s'", sModel, sName) );
		  }
	  }
  }
  
  if (iKick >= 1) {
    if ( !GETPLAYERUSERID(pEntity) ) { // whoops, can't find seesionid
		UTIL_LogPrintf( "[ADMIN] Unable to find player \"%s\"'s session id (%i)\n", STRING(pEntity->v.netname), GETPLAYERUSERID(pEntity) );
    } else {
    	UTIL_LogPrintf( "[ADMIN] Kicked %s due to bad %s info for user %s\n", STRING(pEntity->v.netname), 
						(iKick==1)?"nickname":"model", g_engfuncs.pfnInfoKeyValue( infobuffer, "name"));
    	util_kick_player( GETPLAYERUSERID(pEntity), acKickMsg );
	}

	// He got kicked. No need to run a script on him
	return RESULT_CONTINUE;
  }
  
  // run the script callback for info
  pAdminEnt=pEntity;

  GetHighlanderIndex();

  if (g_fRunPlugins) {
    
    /* A non-zero return indicates that we don't pass
       this back to the engine. */
    if (HandleInfo(pAdminEnt,sName) == PLUGIN_HANDLED) {
      return RESULT_HANDLED;
    }
  } else if (g_fRunScripts) {
	  int i_index;
    cell cReturn = 0;

	  const char *program_file=const_cast<char *>(CVAR_GET_STRING("script_file"));
    //if(g_fRunScripts==TRUE) {
    if(program_file==nullptr|| FStrEq(program_file,"0")) {
      UTIL_LogPrintf( "[ADMIN] Scripting is disabled. (No mono-script file defined. (cvar script_file))\n");		
    } else {
      int iError = amx_FindPublic(&amx, "client_info", &i_index);
      if (iError != AMX_ERR_NONE) {
	UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't find 'client_info' proc, error #%i\n",iError);
      } else {
	iError = amx_Exec(&amx, &cReturn, i_index, 4, STRING(pEntity->v.netname), sName, GETPLAYERUSERID(pEntity), GETPLAYERAUTHID(pEntity));
	
	if (iError != AMX_ERR_NONE) {
	  UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't run 'client_info' proc, error %i\n",iError);
	}
	
	if (cReturn==1) {
	  return RESULT_HANDLED;
	}
      }
    }
  }
  
  return RESULT_CONTINUE;
}

int AM_DispatchThink( edict_t *pent ) {
  if(pent==pTimerEnt && pTimerEnt!=nullptr) { // intercept the timer
    CBaseEntity *pEntity = static_cast<CBaseEntity *>(GET_PRIVATE(pTimerEnt));
    if (pEntity) pEntity->Think();
    return RESULT_HANDLED;
  } 
  
  return RESULT_CONTINUE;
}

int AM_GetGameDescription( const char* _pcDescription ) {

  static int s_iResSlots  = -1;
  static int s_iResType   = -1;
  static int s_iHideSlots = -1;
  static int s_iPlayerCnt = -1;
  static int s_iVisMaxPlayers = -1;
  static int s_iFreeSlots = -1;

  _pcDescription = nullptr;

  int iResSlots = static_cast<int>(CVAR_GET_FLOAT("reserve_slots"));
  const int iResType = static_cast<int>(CVAR_GET_FLOAT("reserve_type"));
  const int iHideSlots = static_cast<int>(CVAR_GET_FLOAT("amv_hide_reserved_slots"));
  const int iPlayerCnt = GetPlayerCount();
  const int iFreeSlots = static_cast<int>(CVAR_GET_FLOAT("public_slots_free"));
  const int iVisMaxPlayers = static_cast<int>(CVAR_GET_FLOAT("sv_visiblemaxplayers"));

  if ( iResType == 1 ) iResSlots = 1;

  // we have to act only if a relevant cvar changed
  if ( (iFreeSlots != s_iFreeSlots) || (iPlayerCnt != s_iPlayerCnt)
       || (iVisMaxPlayers != s_iVisMaxPlayers) || (iHideSlots != s_iHideSlots)
       || (iResSlots != s_iResSlots) || (iResType != s_iResType) ) {

	//DEVEL_LOG(1, ("Some stored variable changed.") );
	//DEVEL_LOG(1, ("reserve_slots          stored:%d new:%d", s_iResSlots, iResSlots) );
	//DEVEL_LOG(1, ("reserve_type           stored:%d new:%d", s_iResType, iResType) );
	//DEVEL_LOG(1, ("amv_hide_reserve_slots stored:%d new:%d", s_iHideSlots, iHideSlots) );
	//DEVEL_LOG(1, ("sv_visiblemaxplayers   stored:%d new:%d", s_iVisMaxPlayers, iVisMaxPlayers) );
	//DEVEL_LOG(1, ("player count           stored:%d new:%d", s_iPlayerCnt, iPlayerCnt) );
	//DEVEL_LOG(1, ("public_slots_free      stored:%d new:%d", s_iFreeSlots, iFreeSlots) );

	// store changed values
	s_iResSlots  = iResSlots;
	s_iResType   = iResType;
	s_iHideSlots = iHideSlots;
	s_iPlayerCnt = iPlayerCnt;
	s_iVisMaxPlayers = iVisMaxPlayers;
	s_iFreeSlots = iFreeSlots;

	// check if reserve slots values are sensible
	// i.e. larger than 0 but smaller than maxplayers
	if ( iResSlots <= 0 ) {
	  iResSlots = 0;
	} else if ( iResSlots >= gpGlobals->maxClients ) {
	  iResSlots =gpGlobals->maxClients;
	}  // if-else
 
	if ( iHideSlots != 0 && iResSlots > 0 ) {
	  // if we hide the reserved slots set the visiblemaxplayers
		if ( iResType == 2 ) {
			CVAR_SET_FLOAT( "sv_visiblemaxplayers", (iFreeSlots + iPlayerCnt) );
			DEBUG_LOG(1, ("Reported maxplayers set to %i", (iFreeSlots + iPlayerCnt)) );
		} else {
			CVAR_SET_FLOAT( "sv_visiblemaxplayers", (gpGlobals->maxClients - iResSlots) );
			DEBUG_LOG(1, ("Reported maxplayers set to %i", (gpGlobals->maxClients - iResSlots)) );
		}  // if-else
	} else if ( iHideSlots == 0 ) {
	  // else disable the maxplayers override
	  CVAR_SET_FLOAT( "sv_visiblemaxplayers", -1.0 );
	  DEBUG_LOG(1, ("Reporting normal maxplayer setting") );
	}  // if-else
  }  // if

  return RESULT_CONTINUE;
}  //  AM_GetGameDescription()


/*
int AM_InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message ) {
	DEBUG_LOG(2, ("Inconsistent file: %s", filename) );
	if ( strcmp("test.akf", filename) == 0 ) {
		DEBUG_LOG(1, ("Keyfile mismatch") );
		strcpy( disconnect_message, "keyfile mismatch" );
		return RESULT_HANDLED;
	} // if
	return PLUGIN_CONTINUE;
	
}  // AM_InconsistentFile()
*/




int AM_PlayerPreThink( edict_t* pEntity ) {

	if ( g_SpectatorCheatAction == 1 ) {
		if((pEntity->v.iuser1 == 3) 
		   && ((pEntity->v.team != 0) || (pEntity->v.playerclass != 0)) 
		   && !pEntity->v.deadflag) {                                               
			CLIENT_COMMAND(pEntity, "spectate;");
			DEBUG_LOG(1, ("Player '%s' is suspected to use the spectator cheat.", STRING(pEntity->v.netname)) );
		}  // if
	}  // if

	return RESULT_CONTINUE;
}  // AM_PlayerPreThink()




int AM_ClientPutInServer( edict_t* pEntity ) {
	const int iIndex = ENTINDEX( pEntity );
 
    // flag this client as clean so CUIC() will do checks
    g_AuthArray[iIndex].state = auth_struct::clean;
    DEVEL_LOG(2, ("AM_ClientPutInServer(): '%s', slot %i", STRING(pEntity->v.netname), iIndex) );
    return RESULT_CONTINUE;
}


 
int AM_ServerDeactivate() {
 
    DEVEL_LOG(1, ("AM_ServerDeactivate(): reseting backup auth buffer") );
    clear_auth_bak_array();
    return RESULT_CONTINUE;
} 



unsigned int me_log_fix( bool _bLog, bool _bFix ) {
	unsigned int uiMECount = 0;

	if ( _bLog || _bFix ) {
		unsigned int uiECount = 0;
		edict_t *pent = nullptr;

		DEBUG_LOG(2, ("Checking for broken entities (frame %u).", s_uiFCount) );
		
		//while ((pent = FIND_ENTITY_BY_STRING( pent, "classname","trigger_multiple" )) != nullptr && (!FNullEnt(pent)))
		while ( (pent = FIND_ENTITY_IN_SPHERE(pent, Vector(0,0,0),8000)) != nullptr && (!FNullEnt(pent)) ) {
			
			
			++uiECount;
			
			if ( pent->v.absmin.x==-1 && pent->v.absmin.y==-1 && pent->v.absmin.z==-1 ) {
				if ( pent->v.absmax.x==1 && pent->v.absmax.y==1 && pent->v.absmax.z==1 ) {
					
					// If we should log it to the log, do so.
					if ( _bLog ) {
						UTIL_LogPrintf( "[ADMIN] Found suspicious entity '%s/%s/%s' at (%i,%i,%i) in map %s.\n",
										STRING(pent->v.classname), STRING(pent->v.targetname), STRING(pent->v.model),
										static_cast<int>(pent->v.origin.x), static_cast<int>(pent->v.origin.y), static_cast<int>(pent->v.origin.z),
										STRING(gpGlobals->mapname) );
					}  // if
					
					++uiMECount;
					//UTIL_SavePent(pent);
					
					// Try to fix the entity.
					if ( _bFix ) {
						
						if ( strcmp(STRING(pent->v.classname),"info_tfgoal") !=0 ) {
							pent->v.absmin = pent->v.absmin + pent->v.mins;
							pent->v.absmax = pent->v.absmax + pent->v.maxs;
						} else {
							int max=0;

							int t = pent->v.mins.x;
							if ( t<0 ) t=-t;
							if ( t>max ) max=t;
							
							t = pent->v.mins.y;
							if ( t<0 ) t=-t;
							if ( t>max ) max=t;
							
							t = pent->v.mins.z;
							if ( t<0 ) t=-t;
							if ( t>max ) max=t;
							
							t = pent->v.maxs.x;
							if ( t<0 ) t=-t;
							if ( t>max ) max=t;
							
							t = pent->v.maxs.y;
							if ( t<0 ) t=-t;
							if ( t>max ) max=t;
							
							t = pent->v.maxs.z;
							if ( t<0 ) t=-t;
							if ( t>max ) max=t;
							
							pent->v.absmin = pent->v.absmin - Vector(max,max,max);
							pent->v.absmax = pent->v.absmax + Vector(max,max,max);
							pent->v.nextthink = 0;
							MDLL_Spawn(pent);
							//UTIL_SavePent(pent);
						}  // if-else
					}  // if
				}  // if
			}  // if
			//UTIL_SavePent(pent);
		}  // while
		
		if ( _bLog && uiMECount ) {
			UTIL_LogPrintf( "[ADMIN] %u suspicious entities found out of %u.\n", uiMECount, uiECount );
		}  // if
	}  // if

	return uiMECount;
}




int AM_StartFrame() {
  

	++s_uiFCount;

	if ( s_uiFCount % 500 == 0 ) {  // execute every X frames

		if ( !g_ovcPendingPlayers.empty() ) {
		    QUEUEING_DEBUGF( ("|---> (Start Frame) Checking pending player queue. Queue length: %d\n", g_ovcPendingPlayers.size()) );
			// take the first player in the queue
			AMConnectingPlayer* pePPlayer = g_ovcPendingPlayers.front();
			g_ovcPendingPlayers.pop_front();

			// If this player is not (or no longer) valid, do away with the queue entry.
            // But we have to have a pause after the last player was added or we directly dequeue him here
            if ( (s_uiFCount > s_uiDequeueTimeout) && !IsPlayerValid(pePPlayer->pEntity) ) {
				delete pePPlayer;
				QUEUEING_DEBUGF( ("|---> (Start Frame) Dequeueing invalid player. New queue length: %d\n", g_ovcPendingPlayers.size()) );
				return RESULT_CONTINUE;
			}  // if

			const char* pcAuthId = GETPLAYERAUTHID( pePPlayer->pEntity );
		    const int iIndex = ENTINDEX( pePPlayer->pEntity );
			QUEUEING_DEBUGF( ("|---> Player Auth Id (StartFrame %lu) %s  -- (%d queued)\n", s_uiFCount, pcAuthId, g_ovcPendingPlayers.size()) );

			if ( AMAuthId::is_pending(pcAuthId) ) {
				// Steamid still pending, add him to the queue again
				g_ovcPendingPlayers.emplace_back(pePPlayer);

			} else {
				// Update the auth entry with the validated auth id
				const AMAuthId oaiAuthID( pcAuthId );
				g_AuthArray[iIndex].oaiAuthID = oaiAuthID;
				QUEUEING_DEBUGF( ("|---> (Start Frame %lu) Auth id validated (%s), calling ClientUserInfoChanged()\n", s_uiFCount, pcAuthId) );

				// Run SetUserPassword() since we might have an invalid entry from a first connect
				QUEUEING_DEBUGF( ("|----> Call SetUserPassword( %s, nullptr, %p )\n", pePPlayer->acName, pePPlayer->pEntity) );
				SetUserPassword( pePPlayer->acName, nullptr, pePPlayer->pEntity );

				// call AM_ClientConnect() to make an entry for him and check his access
			    char acRejectMsg[128];
				QUEUEING_DEBUGF( ("|----> Call  AM_ClientConnect( %p, %s, %s, acRejectMsg, true ) \n", pePPlayer->pEntity, pePPlayer->acName, pePPlayer->acIPPort) );
				const BOOL retval = AM_ClientConnect( pePPlayer->pEntity, pePPlayer->acName, pePPlayer->acIPPort, acRejectMsg, true );
				QUEUEING_DEBUGF( ("|----> AM-ClientConnect() returned %s\n", retval?"TRUE":"FALSE") );
				if ( retval == FALSE ) {
					// oopsie, need to kick the poor bastard off for some reason
					CLIENT_PRINTF( pePPlayer->pEntity, print_console, UTIL_VarArgs("%s\n", acRejectMsg) );
    				if ( !GETPLAYERUSERID(pePPlayer->pEntity) ) { // whoops, can't find wonid
						UTIL_LogPrintf( "[ADMIN] Unable to find player %s's user id (%s), cannot kick him!\n", STRING(pePPlayer->pEntity->v.netname), GETPLAYERUSERID(pePPlayer->pEntity) );

    				} else {
						// the first byte of acRejectMessage is skipped since that is a \n which would result in
						// the server command being terminated (and thus run) prematurely and the kick reason
						// would not get printed.
    					UTIL_LogPrintf( "[ADMIN] Kicked '%s', reason: %s\n", STRING(pePPlayer->pEntity->v.netname), &acRejectMsg[1] );
						QUEUEING_DEBUGF( ("|----> Call SERVER_COMMAND( kick #%i %s )\n",  GETPLAYERUSERID(pePPlayer->pEntity), &acRejectMsg[1]) );
    					util_kick_player( GETPLAYERUSERID(pePPlayer->pEntity), &acRejectMsg[1] );
					}

				} else {
					// call AM_ClientUserInfoChanged() just in case and to call the script callback
					QUEUEING_DEBUGF( ("|----> Call AM_ClientUserInfoChanged( %p, infobuf, true )\n",  pePPlayer->pEntity) );
					AM_ClientUserInfoChanged( pePPlayer->pEntity, g_engfuncs.pfnGetInfoKeyBuffer(pePPlayer->pEntity), true );
				}

				QUEUEING_DEBUGF( ("|---> Player Auth Id  %s dequeued -- (%d queued)  (StartFrame %lu)\n", pcAuthId, g_ovcPendingPlayers.size(), s_uiFCount) );
				
				// Delete the player data object
				delete pePPlayer;
			}  // if-else
		} else {
			//QUEUEING_DEBUGF( ("|---> Player Auth Id (StartFrame) --\n") );
		}  // if-else
	}  // if


	/*
	if ( (s_uiFCount % 1000) == 0 ) {
		UTIL_LogPrintf( "|---> MARK Frame %lu\n", s_uiFCount );
	}  // if
	*/


	//-- Every Y frames check if all slots are full despite of reserve_type 1
	//-- being set. This is an invalid configuration.
	if ( (s_uiFCount % 4000) == 0 ) {
		if ( static_cast<int>(CVAR_GET_FLOAT("reserve_type")) == 1 ) {
			//DEBUG_LOG(4, ("Checking for overfull server on reserve type 1. Player count is %i vs maxplayers %i", 
			//				GetPlayerCount(), gpGlobals->maxClients) );
			if ( GetPlayerCount() >= gpGlobals->maxClients ) {
				DEBUG_LOG(3, ("KickHighestPinger() triggered in frame %lu", s_uiFCount) );
				KickHighestPinger( nullptr, nullptr, nullptr);
			}  // if
		}  // if
	}  // if



	if ( (s_bInitCheck || s_bMECheck) && s_uiFCount > 50 ) {

		if ( s_bMECheck ) {
			// On mapchange check for missing entities.

			const int iLog = get_option_cvar_value( "amv_enable_beta", "melog", nullptr, 0 );
			const int iFix = get_option_cvar_value( "amv_enable_beta", "mefix", nullptr, 0 );
			me_log_fix( (iLog != 0), (iFix != 0) );
			
			s_bMECheck = false;
		}  // if


        if ( s_bInitCheck ) {

			// It is a bit of a dirty hack, that we check the password_field here.
			// It is a timing thing and I don't have the time now to find out which other function we could
			// use to make these checks. But having them when the first user connects (ClientStart) is
			// too late IMHO.

/*TBR: This is not used anymore, we now allow to not use any plugins.
			// Check that either admin_plugin_file or script_file is defined.
			if ( !cvar_file_is_set("admin_plugin_file") ) {
				
				if( !cvar_file_is_set("script_file") ) {
					// If neither is set we probably have not yet read the adminmod.cfg file. 
					// Try to read it from a default location and return so that this check is run again
					if ( !s_bConfigRead ) {
						char cfgfile[PATH_MAX];
						int pos;
						snprintf( cfgfile, PATH_MAX-1, "exec %s/adminmod.cfg", CVAR_GET_STRING("amv_default_config_dir") );
						DEBUG_LOG( 1, ("Trying to load config file: %s.", cfgfile) );
						pos = strlen(cfgfile);
						cfgfile[pos] = '\n';
						cfgfile[pos+1] = '\0';
						SERVER_COMMAND( cfgfile );
						s_bConfigRead = true;
						return RESULT_CONTINUE;
					}  // if
 
					UTIL_LogPrintf("[ADMIN] ERROR: ********************************************************\n\nYou must define \"admin_plugin_file\" in your\nadminmod.cfg file before you can use Admin Mod.\nGo to http://www.adminmod.org/help/online/ for more details.\n");
#ifdef WIN32
					MessageBox(nullptr,"[ADMIN] ERROR:\n\nYou must define \"admin_plugin_file\" in your adminmod.cfg file before you can use Admin Mod.\nGo to http://www.adminmod.org/help/online/ for more details.\n","ERROR",MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
#else
					fprintf(stderr,"[ADMIN] ERROR: ********************************************************\n\nYou must define \"admin_plugin_file\" in your\nadminmod.cfg file before you can use Admin Mod.\nGo to http://www.adminmod.org/help/online/ for more details.\n");
#endif
					am_exit(1);
				}  // if
			}  // if
*/


			// make sure that the password_field cvar is set and is prefixed with an underscore
			const char *passwd_field = get_cvar_string_value( "password_field", true );
    
            if( passwd_field == nullptr  ) {
				// If it is set to "0" we probably have not yet read the adminmod.cfg file. 
				// Try to read it from a default location and return so that this check is run again
				if ( !s_bConfigRead ) {
					char cfgfile[PATH_MAX];
					snprintf( cfgfile, PATH_MAX-1, "exec %s/adminmod.cfg", CVAR_GET_STRING("amv_default_config_dir") );
					UTIL_LogPrintf( "Trying to load default Admin Mod config file: %s.\n", cfgfile );
					const int pos = strlen(cfgfile);
					cfgfile[pos] = '\n';
					cfgfile[pos+1] = '\0';
					SERVER_COMMAND( cfgfile );
					s_bConfigRead = true;
					return RESULT_CONTINUE;
				}  // if

				UTIL_LogPrintf("\n[ADMIN] ERROR: ********************************************************\nThe configuration file for Admin Mod (default: adminmod.cfg) could not be read.\nMake sure that the Admin Mod configuration file is executed from server.cfg\nwhen you use Admin Mod.\nGo to http://www.adminmod.org/manual/ for more details.\n");
#ifdef WIN32
				MessageBox(nullptr,"[ADMIN] ERROR:\n\nYThe configuration file for Admin Mod could not be read.\nMake sure that the Admin Mod configuration file (default: adminmod.cfg) is executed from server.cfg when you use Admin Mod.\nGo to http://www.adminmod.org/manual/ for more details.\n","ERROR",MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
#else
				fprintf(stderr,"\n[ADMIN] ERROR: ********************************************************\nThe configuration file for Admin Mod (default: adminmod.cfg) could not be read.\nMake sure that the Admin Mod configuration file is executed from server.cfg\nwhen you use Admin Mod.\nGo to http://www.adminmod.org/manual/ for more details.\n");
#endif
				am_exit(1);

			} else if ( passwd_field[0] == '\0' ) {  // Empty password_field.
				UTIL_LogPrintf( "[ADMIN] WARNING: You have not set a password_field value. This is needed for your admins to securely authenticate with the server.\n");

			} else if ( passwd_field[0] != '_' ) {  // Not starting with an underscore.
				UTIL_LogPrintf( "[ADMIN] ERROR: *********************************************************\nFor your own safety you are required to set the password_field cvar to a value\nwhich is prefixed with an underscore. This is to protect your admins' passwords\nfrom being distributed to other clients.\n\nChange the value for password_field in your adminmod.cfg file\nfrom '%s' to '_%s'.\nGo to http://www.adminmod.org/manual/ for more details.\n", passwd_field, passwd_field );
#ifdef WIN32
				char acErrMsg[500];
				snprintf( acErrMsg, 500, "[ADMIN] ERROR:\n\nFor your own safety you are required to set the password_field cvar to a value which is prefixed with an underscore. This is to protect your admins' passwords from being distributed to other clients.\n\nChange the value for password_field in your adminmod.cfg file from '%s' to '_%s'.\nGo to http://www.adminmod.org/manual/ for more details.\n", passwd_field, passwd_field );
				MessageBox(nullptr, acErrMsg, "ERROR", MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
#else
				fprintf(stderr,"[ADMIN] ERROR: *********************************************************\nFor your own safety you are required to set the password_field cvar to a value\nwhich is prefixed with an underscore. This is to protect your admins' passwords\nfrom being distributed to other clients.\n\nChange the value for password_field in your adminmod.cfg file\nfrom '%s' to '_%s'.\nGo to http://www.adminmod.org/manual/ for more details.\n", passwd_field, passwd_field );
#endif
				am_exit(1);
			}  // if-else		



			// Set the public_slots_free cvar. We need to do it here because we don't have
			// the info on reserved slots from the config file earlier.
			CVAR_SET_FLOAT( "public_slots_free", GetFreeSlots(nullptr) );


			s_bInitCheck = false;
		}  // if

	}  // if



	return RESULT_CONTINUE;
}








int AM_GameDLLInit() {
 
  /* CVars missing here get registered in h_export.cpp::GiveFnptrsToDll() */

  CVAR_REGISTER(&admin_balance_teams);
  // CVAR_REGISTER(&admin_bot_protection);   /* registered in h_export.cpp */
  CVAR_REGISTER(&admin_cs_restrict);
  CVAR_REGISTER(&admin_connect_msg);
  // CVAR_REGISTER(&admin_debug);   /* registered in h_export.cpp */
  // CVAR_REGISTER(&admin_devel);   /* registered in h_export.cpp */
  CVAR_REGISTER(&admin_fun_mode);
  CVAR_REGISTER(&admin_fx);
  CVAR_REGISTER(&admin_gag_name);
  CVAR_REGISTER(&admin_gag_sayteam);
  CVAR_REGISTER(&admin_highlander);
  CVAR_REGISTER(&admin_ignore_immunity);
  CVAR_REGISTER(&admin_parse_logs);
  CVAR_REGISTER(&admin_plugin_file);
  CVAR_REGISTER(&admin_quiet);
  CVAR_REGISTER(&admin_reconnect_timeout);
  CVAR_REGISTER(&admin_reject_msg);
  CVAR_REGISTER(&admin_repeat_msg);
  CVAR_REGISTER(&admin_repeat_freq);
  CVAR_REGISTER(&admin_vault_file);
  CVAR_REGISTER(&admin_version);
  CVAR_REGISTER(&admin_vote_autostart);
  CVAR_REGISTER(&admin_vote_echo);
  CVAR_REGISTER(&admin_vote_freq);
  CVAR_REGISTER(&admin_vote_maxextend);
  CVAR_REGISTER(&admin_vote_ratio);
  CVAR_REGISTER(&allow_client_exec);
  CVAR_REGISTER(&ami_sv_maxplayers);
  CVAR_REGISTER(&amv_anti_cheat_options);
  CVAR_REGISTER(&amv_enable_beta);
  CVAR_REGISTER(&amv_default_config_dir);
  // CVAR_REGISTER(&amv_hide_reserved_slots);        /* registered in h_export.cpp */
  CVAR_REGISTER(&amv_log_passwords);
  CVAR_REGISTER(&amv_private_server);
  CVAR_REGISTER(&amv_prvt_kick_message);
  CVAR_REGISTER(&amv_reconnect_time);
  CVAR_REGISTER(&amv_register_cmds);
  CVAR_REGISTER(&amv_vote_duration);
  CVAR_REGISTER(&default_access);
  CVAR_REGISTER(&encrypt_password);
  CVAR_REGISTER(&file_access_read);
  CVAR_REGISTER(&file_access_write);
  CVAR_REGISTER(&help_file);
  CVAR_REGISTER(&ips_file);
  CVAR_REGISTER(&kick_ratio);
  CVAR_REGISTER(&map_ratio);
  CVAR_REGISTER(&maps_file);
  CVAR_REGISTER(&models_file);
  CVAR_REGISTER(&models_kick_msg);
  CVAR_REGISTER(&nicks_kick_msg);
  CVAR_REGISTER(&password_field);
  CVAR_REGISTER(&pretty_say);
  CVAR_REGISTER(&public_slots_free);
  //CVAR_REGISTER(&reserve_slots);         /* registered in h_export.cpp */
  CVAR_REGISTER(&reserve_slots_msg);
  CVAR_REGISTER(&reserve_type);
  CVAR_REGISTER(&script_file);
  CVAR_REGISTER(&use_regex);
  CVAR_REGISTER(&users_file);
  CVAR_REGISTER(&vote_freq);
  CVAR_REGISTER(&words_file);
  
#ifdef USE_MYSQL
  CVAR_REGISTER(&mysql_database);
  CVAR_REGISTER(&mysql_dbtable_ips);
  CVAR_REGISTER(&mysql_dbtable_models);
  CVAR_REGISTER(&mysql_dbtable_plugins);
  CVAR_REGISTER(&mysql_dbtable_users);
  CVAR_REGISTER(&mysql_dbtable_tags);
  CVAR_REGISTER(&mysql_dbtable_words);
  CVAR_REGISTER(&mysql_host);
  CVAR_REGISTER(&mysql_pass);
  CVAR_REGISTER(&mysql_user);
  CVAR_REGISTER(&mysql_preload);
  CVAR_REGISTER(&mysql_users_sql);
  CVAR_REGISTER(&mysql_tags_sql);


#endif
  
#ifdef USE_PGSQL
  CVAR_REGISTER(&pgsql_database);
  CVAR_REGISTER(&pgsql_dbtable_ips);
  CVAR_REGISTER(&pgsql_dbtable_models);
  CVAR_REGISTER(&pgsql_dbtable_plugins);
  CVAR_REGISTER(&pgsql_dbtable_users);
  CVAR_REGISTER(&pgsql_dbtable_tags);
  CVAR_REGISTER(&pgsql_dbtable_words);
  CVAR_REGISTER(&pgsql_host);
  CVAR_REGISTER(&pgsql_port);
  CVAR_REGISTER(&pgsql_pass);
  CVAR_REGISTER(&pgsql_user);
  CVAR_REGISTER(&pgsql_preload);
  CVAR_REGISTER(&pgsql_users_sql);
  CVAR_REGISTER(&pgsql_tags_sql);


#endif

  REG_SVR_COMMAND("admin_command", AM_AdminCommand);
  REG_SVR_COMMAND("admin_cmd", AM_AdminCommand);
  REG_SVR_COMMAND("admin_version", AM_AdminCommand);
  
  AM_Initialize();

  return RESULT_CONTINUE;
}

int AM_Initialize() {
  pTimerEnt = nullptr;
  s_bMECheck = true;
  s_uiFCount = 0;

  InitAdminModData(TRUE, FALSE);  

  // Set the external maxplayer info to the maxplayers set
  CVAR_SET_FLOAT( "ami_sv_maxplayers", gpGlobals->maxClients );

  return RESULT_CONTINUE;
}

int AM_OnFreeEntPrivateData( edict_t *pent ) {
  if(pent==pTimerEnt && pTimerEnt!=nullptr) { // intercept the timer
    //This seems to crash.  Prolly a bad idea.
    //CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pTimerEnt);
    //if (pEntity) delete(pent);
    return RESULT_HANDLED;
  } 
  
  return RESULT_CONTINUE;
}

void KickHighestPinger( const char *pszName,char *ip,edict_t *pEntity) {
	int iLoss = 0;
  int iMaxPing = -1;
  int iPing = 0;
  int iPlayerIndex = 0;
	CBaseEntity* pPlayer;
  
  for (int i = 1; i <= gpGlobals->maxClients; i++) {
    pPlayer = UTIL_PlayerByIndex(i);
    
    if(!IsPlayerValid(pPlayer)) 
      continue;
    
    PLAYER_CNX_STATS(pPlayer->edict(),&iPing,&iLoss);
    if ( iPing > iMaxPing) {
	    const int iAccess = GetUserAccess(pPlayer->edict());
      if (!(IsIPReserved(g_AuthArray[i].sIP) 
	    || ((iAccess & ACCESS_RESERVE_SPOT) == ACCESS_RESERVE_SPOT) 
	    || ((iAccess & ACCESS_IMMUNITY) == ACCESS_IMMUNITY 
		&& static_cast<int>(CVAR_GET_FLOAT("admin_ignore_immunity")) == 0))) {
	      char name[USERNAME_SIZE];
	      iPlayerIndex = i;
	iMaxPing = iPing;
	strcpy(name,STRING(pPlayer->pev->netname));
      }
    }
  }

  
  if ( iPlayerIndex != 0) {
    pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
    
    
    const char* sReserveKickMsg = get_cvar_string_value( "reserve_slots_msg", true );
    
    if ( sReserveKickMsg == nullptr ) {
      sReserveKickMsg = "[ADMIN] The Server Admin has enabled reserved slots. You have been kicked due to another player taking a reserved slot.";
	}

    CLIENT_PRINTF( pPlayer->edict(), print_console, sReserveKickMsg);
   	util_kick_player( GETPLAYERUSERID(pPlayer->edict()), sReserveKickMsg );
    
    UTIL_LogPrintf( "[ADMIN] kicked %s due to reserve_slots\n",STRING(pPlayer->pev->netname) );
  }
}

void* LoadScript(AMX *amx, const char *filename) {
  FILE* fp;
  AMX_HEADER hdr;
  void* program;
  
  if ( (fp = fopen( filename, "rb" )) != nullptr ) {
    fread(&hdr, sizeof hdr, 1, fp);
    if ( (program = calloc(1, hdr.stp)) != nullptr ) {
      rewind( fp );
      fread( program, 1, hdr.size, fp );
      fclose( fp );
      memset(amx, 0, sizeof(AMX));
      if ( amx_Init( amx, program ) != AMX_ERR_NONE ) {
	free(program);
	return nullptr;
      }
      return program;
    }
  }
  return nullptr;
}