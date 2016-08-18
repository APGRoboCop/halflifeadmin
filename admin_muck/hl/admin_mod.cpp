/* The guts of Admin Mod
 *
 *  Copyright (c) Alfred Reynolds, 2000
 *  This file is covered by the GPL.
 *
 * $Id: admin_mod.cpp,v 1.28 2001/09/19 22:29:24 darope Exp $
 *
 */   

#ifdef USE_MYSQL
  #include <mysql.h>
  #include <errmsg.h>
#endif

#include "extdll.h"
#define SDK_UTIL_H	// extdll.h already #include's util.h

#include "amutil.h"
#include "users.h"

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
void *program=NULL;

DLL_GLOBAL edict_t *pAdminEnt;
DLL_GLOBAL edict_t *pTimerEnt;

extern  AMX_NATIVE_INFO fixed_Natives[];
extern  AMX_NATIVE_INFO admin_Natives[];
extern  enginefuncs_t   g_engfuncs;
extern  globalvars_t*   gpGlobals;
extern  auth_struct     g_AuthArray[MAX_PLAYERS + 1];

extern DLL_GLOBAL BOOL		  g_fInitialized;
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
cvar_t amv_autoban = {"amv_autoban","0",FCVAR_EXTDLL, 0};
cvar_t amv_hide_reserved_slots = {"amv_hide_reserved_slots","1",FCVAR_EXTDLL, 1};
cvar_t allow_client_exec = {"allow_client_exec","0",FCVAR_SERVER};
cvar_t default_access = {"default_access","0",FCVAR_SERVER};
cvar_t encrypt_password = {"encrypt_password","1",FCVAR_EXTDLL}; // use encrypted ones by default
cvar_t file_access_read = {"file_access_read","0",FCVAR_EXTDLL};
cvar_t file_access_write = {"file_access_write","0",FCVAR_EXTDLL};
cvar_t help_file = {"help_file","0",FCVAR_EXTDLL};
cvar_t ips_file = {"ips_file","ips.ini",FCVAR_EXTDLL};
cvar_t kick_ratio = {"kick_ratio","0",FCVAR_EXTDLL};
cvar_t map_ratio = {"map_ratio","0",FCVAR_EXTDLL};
cvar_t maps_file = {"maps_file","0",FCVAR_EXTDLL}; 
cvar_t models_file  = {"models_file","0",FCVAR_EXTDLL};
cvar_t models_kick_msg = {"models_kick_msg","0",FCVAR_EXTDLL};
cvar_t nicks_kick_msg = {"nicks_kick_msg","0",FCVAR_EXTDLL}; 
cvar_t password_field = {"password_field","pw",FCVAR_EXTDLL};
cvar_t password_timeout = {"password_timeout","7200",FCVAR_EXTDLL};  // not used anymore
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
	cvar_t mysql_dbtable_ips = {"mysql_dbtable_ips","0",FCVAR_EXTDLL};
 	cvar_t mysql_dbtable_models = {"mysql_dbtable_models","0",FCVAR_EXTDLL};
	cvar_t mysql_dbtable_plugins = {"mysql_dbtable_plugins","0",FCVAR_EXTDLL};
	cvar_t mysql_dbtable_users = {"mysql_dbtable_users","0",FCVAR_EXTDLL};
	cvar_t mysql_dbtable_tags = {"mysql_dbtable_tags","0",FCVAR_EXTDLL};

	cvar_t mysql_dbtable_words = {"mysql_dbtable_words","0",FCVAR_EXTDLL};
	cvar_t mysql_host = {"mysql_host","0",FCVAR_EXTDLL};
	cvar_t mysql_pass = {"mysql_pass","0",FCVAR_EXTDLL};
	cvar_t mysql_user = {"mysql_user","0",FCVAR_EXTDLL};

	cvar_t mysql_preload = {"mysql_preload","1",FCVAR_EXTDLL};

	cvar_t mysql_users_sql = {"mysql_users_sql","SELECT pass,access FROM %s where nick='%s' or nick='%i'",FCVAR_EXTDLL};
	cvar_t mysql_tags_sql = {"mysql_tags_sql","SELECT pass,access FROM %s where '%s' REGEXP nick or nick='%i''",FCVAR_EXTDLL};

#endif

#ifdef USE_MYSQL
  MYSQL mysql;
  BOOL g_fUseMySQL = FALSE;
#endif

/* direct access to cvars. They get registered in h_export.cpp */
cvar_t* ptAM_devel = 0;
cvar_t* ptAM_debug = 0;
cvar_t* ptAM_autoban = 0;
cvar_t* ptAM_botProtection = 0;
cvar_t* ptAM_reserve_slots = 0;
cvar_t* ptAM_hide_reserved_slots = 0;


void AM_AdminCommand(void) {  
  AM_ClientCommand(NULL);
}

int AM_ClientCommand( edict_t *pEntity ) {
  const char *pcmd = CMD_ARGV(0);
  int iError;
  int iIndex;
  cell cReturn = 0;
  char admin_command[255];	
  
  pAdminEnt=pEntity;
  
  if (!g_fInitialized)
    AM_ClientStart(pEntity);
  
  /* Do the admin_command check - strip out admin_command if it
     prepends the command.  This allows us to capture commands from
     the hlds console, etc. */
  if(FStrEq(pcmd,"admin_command")) {
    int i;
    
    pcmd=CMD_ARGV(1);
    strcpy(admin_command,"");
    for(i=2;i<CMD_ARGC();i++) {
      if((strlen(admin_command)+strlen(CMD_ARGV(i)))>255) 
	break;
      if(i!=2) 
	strcat(admin_command," ");
      strcat(admin_command,CMD_ARGV(i));
    }
  } else {
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
  }
  
  /* Do our hardcoded checks (admin_password, admin_help, admin_version...)...
     note that admin_help and admin_version are only checked if we're using
     plugins */  
  if ( FStrEq(pcmd, "admin_password" )) {
    if (pAdminEnt == NULL) {
      UTIL_LogPrintf("Laf. Why are you trying to enter a password?  You're at the console!\n");
    } else if ( CMD_ARGC() == 2) {
      SetUserPassword(STRING(pEntity->v.netname),(char*)CMD_ARGV(1),pEntity);
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
  } else if (g_fRunPlugins && !stricmp(pcmd, "admin_version")) {
    /* This never gets passed back to the engine,
       so ignore the return value. */
    HandleVersion(pAdminEnt);
    return RESULT_HANDLED;
  }
  
  if (g_fRunPlugins) {
    /* A non-zero return indicates that we should stop processing...
       anything else means keep on truckin'. */
    if (HandleCommand(pAdminEnt,(char*)pcmd,admin_command) == PLUGIN_HANDLED) {
      return RESULT_HANDLED;
    }
  } else {
    char *program_file=(char *) CVAR_GET_STRING("script_file");
    
    if(program_file==NULL|| FStrEq(program_file,"0") || !g_fRunScripts) {
      UTIL_LogPrintf( "[ADMIN] Unable to find program_file cvar, scripting is disabled\n");			
    } else {
      iError = amx_FindPublic(&amx,"client_commands",&iIndex);
      if (iError != AMX_ERR_NONE) {
	UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't find 'client_commands' proc, error #%i\n",iError);
      } else { 
	if(pEntity!=NULL) 
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
    CTimer *pTimer = (CTimer *)GET_PRIVATE(pTimerEnt);
    if (pTimer->VoteInProgress()) {
      int iIndex;
      int iVote = atoi(CMD_ARGV(1));
      
      if (pAdminEnt == NULL) {
				iIndex = 0;
      } else {
				iIndex = ENTINDEX(pAdminEnt);
      }
      if (pTimer->GetPlayerVote(iIndex) != 0) {
				// This person has either already voted, or
				// was not shown the vote.  Either way, we ignore it.
				// Do nothing.
      } else if (iVote > pTimer->GetMaxVoteChoice()) {
				// The vote is out of range.  Ignore it.
			} else {
				pTimer->SetPlayerVote(iIndex, iVote);
				System_Response(UTIL_VarArgs("[ADMIN] Vote entered for option #%i\n",atoi(CMD_ARGV(1))),pAdminEnt);
	
				if((int)CVAR_GET_FLOAT("admin_vote_echo") != 0) {
					if (pAdminEnt == NULL) {
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
  if (pAdminEnt == NULL) {
    return RESULT_HANDLED;
  } else {
    return RESULT_CONTINUE;
  }

  if ((int)CVAR_GET_FLOAT("developer") != 0) {
    UTIL_LogPrintf("[ADMIN] DEVEL: AM_ClientCommand finished.\n");
  }

}

BOOL AM_ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ) {    
  char* infobuffer = 0;
  char sIP[IP_SIZE], *p = 0;
  char sModel[BUF_SIZE];
  char sName[BUF_SIZE];
  user_struct tUser;
  
  
  memset( sIP, 0, IP_SIZE );
  memset( sModel, 0, BUF_SIZE );
  memset( sName, 0, BUF_SIZE );
  memset( &tUser, 0, sizeof(user_struct) );

  if ( pEntity ) infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity);  
  if ( pszName ) strncpy(sName, pszName,BUF_SIZE);
  if ( make_friendly(sName, TRUE) == 2 ) {
    strcpy(szRejectReason,"[ADMIN] Your name has invalid characters. Connection denied.");
    UTIL_LogPrintf("[ADMIN] Client with non printing characters in name connected: %s.\n", sName);
    UTIL_LogPrintf("[ADMIN] Client dropped for security reasons.\n");
    return FALSE;
  };
  
  if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf("[ADMIN] AM_ClientConnect: %s\n", sName);
  }
  

  if ( infobuffer ) strncpy(sModel,g_engfuncs.pfnInfoKeyValue(infobuffer,"model"),BUF_SIZE);
  if ( pszAddress ) strncpy(sIP,pszAddress,IP_SIZE);
  
  if ( p = strchr(sIP,':') ) 
    *p = '\0';
  
  if (!g_fInitialized)
    AM_ClientStart(pEntity);
  
  AddUserAuth( sName, sIP, pEntity );

  /*
  if ((int)CVAR_GET_FLOAT("developer") != 0) {
    UTIL_LogPrintf("[ADMIN] AM_ClientConnect done.\n", sName);
  }
  return TRUE;
  */

  if (IsNameReserved(sName, GETPLAYERWONID(pEntity), &tUser)) {
    if ((GetUserAccess(pEntity) & ACCESS_RESERVE_NICK) != ACCESS_RESERVE_NICK) {
      char* sNickRejectMsg = (char*)CVAR_GET_STRING("nicks_kick_msg");
      
      if (sNickRejectMsg == NULL || FStrEq(sNickRejectMsg,"0")) {
		strcpy(szRejectReason,"[ADMIN] That name is reserved on this server.");
      } else {
		strcpy(szRejectReason, sNickRejectMsg);
      }
      return FALSE;
    }
  }
  
  if (IsModelReserved(sModel)) {
    if (GetModelAccess(sModel, pEntity)==0) {
      char* sModelRejectMsg = (char*)CVAR_GET_STRING("models_kick_msg");
      
      if (sModelRejectMsg == NULL || FStrEq(sModelRejectMsg,"0")) {
		strcpy(szRejectReason,"[ADMIN] That model is reserved on this server.");
      } else {
		strcpy(szRejectReason, sModelRejectMsg);
      }
      return FALSE;
    }
  }
  
  int iFreeSlots = GetFreeSlots(pEntity);
  int iResType = (int)CVAR_GET_FLOAT("reserve_type");
  int iResTaken = 0;
  DEBUG_LOG(1, ("%i free spots / %i max spots.",iFreeSlots,gpGlobals->maxClients) );

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
      char* sReserveMsg = (char*)CVAR_GET_STRING("reserve_slots_msg");
      int iResSlots = (int)CVAR_GET_FLOAT("reserve_slots");
      
      if (sReserveMsg==NULL || FStrEq(CVAR_GET_STRING("reserve_slots_msg"),"0")) {
	sprintf(szRejectReason,"[ADMIN] %d of the %d slots on this server are reserved, and no unreserved slots are left. Try again later.\n",iResSlots, gpGlobals->maxClients);
      } else {
		strcpy(szRejectReason,sReserveMsg);
      }
      return FALSE;
    }
  }
  
  // If we have reserve type one, and this person took the reserve spot,
  // we need to kick the person with the highest ping.
  if (iResType == 1 && iResTaken == 1) {
    KickHighestPinger(sName,sIP,pEntity);
  }
  
  if(g_fRunPlugins) {
    // We should never not pass this back to the engine,
    // so ignore the return value.
    HandleConnect(pEntity, sName, (char*)pszAddress);
  } else if (g_fRunScripts) {
    // now run the script
    char *program_file=(char *) CVAR_GET_STRING("script_file");
    
    if(program_file==NULL|| FStrEq(program_file,"0")) {
      UTIL_LogPrintf( "[ADMIN] ERROR: Unable to find program_file cvar, scripting is disabled\n");
    } else {
      int iError;
      int iIndex;
      cell cReturn = 0;
      
      iError = amx_FindPublic(&amx,"client_connect",&iIndex);
      if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't find 'client_connect' proc, error #%i\n",iError);
      } else {
		iError = amx_Exec(&amx, &cReturn, iIndex, 2, sName,pszAddress);	    
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

  if ((int)CVAR_GET_FLOAT("developer") != 0) {
    UTIL_LogPrintf("[ADMIN] DEVEL: AM_ClientConnect finished.\n", sName);
  }
  
  return TRUE;
}

int AM_ClientDisconnect( edict_t* pEntity ) {
  if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf("[ADMIN] AM_ClientDisconnect: %s\n", STRING(pEntity->v.netname));
  }
  
  if(g_fRunPlugins) {
    // We should never not pass this back to the engine,
    // so ignore the return value.
    HandleDisconnect(pEntity);
  }
  
  UpdateUserAuth(pEntity);
  
  CVAR_SET_FLOAT("public_slots_free",GetFreeSlots(pEntity));
  //CVAR_SET_FLOAT("sv_visiblemaxplayers",GetFreeSlots(pEntity));
  return RESULT_CONTINUE;
}

void AM_ClientStart(edict_t *pEntity) {
  int iError;
  int iIndex;
  cell cReturn = 0;
  
  if ( g_fInitialized == TRUE) 
    return;
  
  g_fInitialized = TRUE;
  
#ifdef USE_MYSQL
  const char* sHost = CVAR_GET_STRING("mysql_host");
  const char* sPassword = CVAR_GET_STRING("mysql_pass");
  const char* sUser = CVAR_GET_STRING("mysql_user");
  
  if (g_fUseMySQL == FALSE && strlen(sHost) != 0 && strlen(sUser) != 0 && FStrEq(sHost,"0") == 0 && FStrEq(sUser,"0") == 0) {
    g_fUseMySQL = TRUE;
    mysql_init(&mysql);
    mysql_real_connect(&mysql, sHost, sUser, sPassword,NULL,0,NULL,0);
    
    if (mysql_errno(&mysql)) {
      UTIL_LogPrintf("[ADMIN] ERROR: MySQL Error: %s\n", mysql_error(&mysql));
      mysql_close(&mysql);
      g_fUseMySQL = FALSE;
      exit(1);
    }
  } else if (g_fUseMySQL == TRUE) {
    UTIL_LogPrintf("[ADMIN] MySQL connection already established.\n");
  } else {
    UTIL_LogPrintf("[ADMIN] Not establishing MySQL connection: either host or user name is blank.\n");
  }
#endif
  
  InitSpawnEntityList();
  
  LoadIPs();
  LoadModels();
  LoadUsers();
  LoadVault();
  LoadWords();
  
  pAdminEnt=pEntity;
  
  if(program!=NULL)
    free(program);
  
  int istr = MAKE_STRING("adminmod_timer");
  
  pTimerEnt = CREATE_NAMED_ENTITY(istr);
  if ( FNullEnt( pTimerEnt ) ) {
    UTIL_LogPrintf("[ADMIN] ERROR: NULL Ent for adminmod_timer\n" );
    exit(1);
  }
  
  CBaseEntity *pTimer = (CBaseEntity *)GET_PRIVATE(pTimerEnt);
  if (pTimer) { // run the ptimer spawn
    pTimer->Spawn();   
  }
  
  pTimer->edict()->v.owner = NULL;
  pTimer->edict()->v.origin.x = 0;
  pTimer->edict()->v.origin.y = 0;
  pTimer->edict()->v.origin.z = 0;
  pTimer->edict()->v.angles.x = 0;
  pTimer->edict()->v.angles.y = 0;
  pTimer->edict()->v.angles.z = 0;
  pTimerEnt->v.spawnflags |= SF_NORESPAWN;
  
  char* sPluginFile = (char*)CVAR_GET_STRING("admin_plugin_file");
  if (sPluginFile == NULL || FStrEq(sPluginFile,"") || FStrEq(sPluginFile, "0")) {
    char *program_file=(char *) CVAR_GET_STRING("script_file");
    
    if(program_file==NULL|| FStrEq(program_file,"0")) {
#ifdef WIN32
      MessageBox(NULL,"[ADMIN] ERROR:\n\nYou must define either \"admin_plugin_file\" or \"script_file\" in your server.cfg (listenserver.cfg) before you can use Admin Mod.\nGo to http://www.adminmod.org/ for more details\n","ERROR",MB_OK | MB_ICONSTOP | MB_DEFBUTTON1 | MB_SYSTEMMODAL);
#else
      fprintf(stderr,"[ADMIN] ERROR: **********************************\n\nYou must define either \"admin_plugin_file\" or \"script_file\" in your server.cfg (listenserver.cfg) before you can use Admin Mod.\nGo to http://www.adminmod.org/ for more details\n");
#endif
      exit(1);
    }	
    
    // reload the script file  
    program = LoadScript(&amx,program_file);
    if (program == NULL) {
      UTIL_LogPrintf( "[ADMIN] ERROR: Unable to load script file '%s'\n",program_file);
      exit(1);
    }
    
    /* Ignore these errors - sometimes, the VM will return error 19
       when there's actually nothing wrong.  Dunno why. */
    iError = amx_Register(&amx, fixed_Natives, -1);
    iError = amx_Register(&amx, admin_Natives, -1);
    
    iError = amx_FindPublic(&amx,"client_start",&iIndex);
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
  } else {
    // Load the plugin file and get our actual plugin names.
    if (!LoadPlugins()) {
      UTIL_LogPrintf("[ADMIN] ERROR: LoadPlugins() failed.\n");
      exit(1);
    }
    UTIL_LogPrintf("[ADMIN] Plugins loaded successfully.\n");
    g_fRunPlugins = TRUE;
  }
  g_fRunScripts = TRUE;
}

int AM_ClientUserInfoChanged( edict_t *pEntity, char *infobuffer ) {
  int iKick = 0;
  char szCommand[256];
  char sModel[BUF_SIZE];
  char sName[BUF_SIZE];
  user_struct tUser;
  memset(szCommand, 0x0, 256);
  memset(sModel, 0x0, BUF_SIZE);
  memset(sName, 0x0, BUF_SIZE);
  memset(&tUser, 0x0,sizeof(user_struct));
  
  strncpy(sName, g_engfuncs.pfnInfoKeyValue(infobuffer,"name"),BUF_SIZE);

  if ( make_friendly(sName, TRUE) == 2 ) {
    // the little brat tries to crash the clients
    UTIL_LogPrintf("[ADMIN] Player %s <%d> tried to crash clients with bad name.\n",STRING(pEntity->v.netname), GETPLAYERWONID(pEntity) );
    strcpy(g_engfuncs.pfnInfoKeyValue(infobuffer,"name"), "shithead");
    if ( (int)CVAR_GET_FLOAT("amv_autoban") ) {
      SERVER_COMMAND( UTIL_VarArgs("banid 1440 %u\n", GETPLAYERWONID(pEntity)) );
      SERVER_COMMAND( UTIL_VarArgs("writeid\n") );
      UTIL_LogPrintf( "[ADMIN] Banned player with WONID %u for 24h\n", GETPLAYERWONID(pEntity) );
    }  // if
    CLIENT_COMMAND ( pEntity, "quit\n" );
    return RESULT_HANDLED;
  }  // if
  
  if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf("[ADMIN] AM_ClientUserInfoChanged: %s\n", sName);
  }
  
  if (!g_fInitialized)
    AM_ClientStart(pEntity);

  strncpy(sModel, g_engfuncs.pfnInfoKeyValue(infobuffer,"model"), BUF_SIZE);
  
  // If the name has changed, we need to check it
  if(!FStrEq(sName, STRING(pEntity->v.netname))) {
    SetUserPassword(sName, NULL, pEntity);
    VerifyUserAuth(sName, pEntity);
    if (IsNameReserved(sName, GETPLAYERWONID(pEntity), &tUser)) {
      if ((GetUserAccess(pEntity) & ACCESS_RESERVE_NICK) != ACCESS_RESERVE_NICK) {
	char* sNickRejectMsg = (char*)CVAR_GET_STRING("nicks_kick_msg");
	
	if (sNickRejectMsg == NULL || FStrEq(sNickRejectMsg,"0")) {
	  CLIENT_PRINTF(pEntity, print_console, "[ADMIN] That name is reserved on this server.\n");
	} else {
	  CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("%s\n",sNickRejectMsg));
	}
	iKick = 1;
      }
    }
  }
  
  // If the model has changed, we need to check it
  if(!FStrEq(sModel, STRING(pEntity->v.model))) {
    if (IsModelReserved(sModel)) {
      if (GetModelAccess(sModel, pEntity)==0) {
	char* sModelRejectMsg = (char*)CVAR_GET_STRING("models_kick_msg");
	
	if (sModelRejectMsg == NULL || FStrEq(sModelRejectMsg,"0")) {
	  CLIENT_PRINTF(pEntity, print_console, "[ADMIN] That model is reserved on this server.\n");
	} else {
	  CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("%s\n",sModelRejectMsg));
	}
	iKick = 1;
      }
    }
  }
  
  if (iKick == 1) {
    CBaseEntity *pKick=UTIL_PlayerByName(STRING(pEntity->v.netname));
    if (pKick==NULL) {
      UTIL_LogPrintf( "[ADMIN] Unable to find player %s\n",STRING(pEntity->v.netname));
      return RESULT_HANDLED;
    }
    
    if (!GETPLAYERUSERID( pKick->edict())) { // whoops, can't find wonid
      UTIL_LogPrintf( "[ADMIN] Unable to find player %s's WONID (%u)\n",STRING(pEntity->v.netname),GETPLAYERWONID( pKick->edict()));
    }
    UTIL_LogPrintf( "[ADMIN] Kicked %s due to bad nickname info for user %s\n",STRING(pEntity->v.netname), g_engfuncs.pfnInfoKeyValue( infobuffer, "name"));
#ifndef _WIN32
    snprintf(szCommand,256,"kick # %i\n", GETPLAYERUSERID( pKick->edict()));
#else
    sprintf(szCommand,"kick # %i\n",GETPLAYERUSERID( pKick->edict()) );
#endif
    SERVER_COMMAND(szCommand);
  }
  
  // run the script callback for info
  pAdminEnt=pEntity;

  if (g_fRunPlugins) {
    
    /* A non-zero return indicates that we don't pass
       this back to the engine. */
    if (HandleInfo(pAdminEnt,sName) == PLUGIN_HANDLED) {
      return RESULT_HANDLED;
    }
  } else if (g_fRunScripts) {
    int iError;
    int iIndex;
    cell cReturn = 0;
    
    char *program_file=(char *) CVAR_GET_STRING("script_file");
    //if(g_fRunScripts==TRUE) {
    if(program_file==NULL|| FStrEq(program_file,"0")) {
      UTIL_LogPrintf( "[ADMIN] ERROR: Unable to find program_file cvar, scripting is disabled\n");		
    } else {
      iError = amx_FindPublic(&amx,"client_info",&iIndex);
      if (iError != AMX_ERR_NONE) {
	UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't find 'client_info' proc, error #%i\n",iError);
      } else {
	iError = amx_Exec(&amx, &cReturn, iIndex, 4, STRING(pEntity->v.netname), sName, GETPLAYERUSERID(pEntity), GETPLAYERWONID(pEntity));
	
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
  if(pent==pTimerEnt && pTimerEnt!=NULL) { // intercept the timer
    CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pTimerEnt);
    if (pEntity) pEntity->Think();
    return RESULT_HANDLED;
  } 
  
  return RESULT_CONTINUE;
}

int AM_GetGameDescription( const char* _pcDescription ) {

  static int s_iResSlots = -1;
  static int s_iHideSlots = -1;

  _pcDescription = 0;

  int iResSlots = (int)CVAR_GET_FLOAT("reserve_slots"); 
  int iHideSlots = (int)CVAR_GET_FLOAT("amv_hide_reserved_slots");

  // we have to act only if either reserve_slots or hide_reserve_slots changed
  if ( (iResSlots != s_iResSlots) || (iHideSlots != s_iHideSlots) ) {
	// store changed values
	s_iResSlots = iResSlots;
	s_iHideSlots = iHideSlots;

	// check if reserve slots values are sensible
	// i.e. larger than 0 but smaller than maxplayers
	if ( iResSlots <= 0 ) {
	  iResSlots = 0;
	} else if ( iResSlots >= gpGlobals->maxClients ) {
	  iResSlots =gpGlobals->maxClients;
	}  // if-else
 
	if ( iHideSlots != 0 && iResSlots > 0 ) {
	  // if we hide the reserved slots set the visiblemaxplayers
	  CVAR_SET_FLOAT( "sv_visiblemaxplayers", (gpGlobals->maxClients - iResSlots) );
	  DEBUG_LOG(1, ("Reported maxplayers set to %i", (gpGlobals->maxClients - iResSlots)) );
	} else if ( iHideSlots == 0 ) {
	  // else disable the maxplayers override
	  CVAR_SET_FLOAT( "sv_visiblemaxplayers", -1.0 );
	  DEBUG_LOG(1, ("Reporting normal maxplayer setting") );
	}  // if-else
  }  // if

  return RESULT_CONTINUE;
}  //  AM_GetGameDescription()


int AM_GameDLLInit( void ) {
 
  /* CVars missing here get registered in h_export.cpp::GiveFnptrsToDll() */

  CVAR_REGISTER(&admin_balance_teams);
  /* CVAR_REGISTER(&admin_bot_protection);   /* registered in h_export.cpp */
  CVAR_REGISTER(&admin_cs_restrict);
  CVAR_REGISTER(&admin_connect_msg);
  /* CVAR_REGISTER(&admin_debug);   /* registered in h_export.cpp */
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
  // CVAR_REGISTER(&amv_autoban);        /* registered in h_export.cpp */
  // CVAR_REGISTER(&amv_hide_reserved_slots);        /* registered in h_export.cpp */
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
  CVAR_REGISTER(&password_timeout);
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
  
  REG_SVR_COMMAND("admin_command", AM_AdminCommand);
  
  AM_Initialize();
  
  return RESULT_CONTINUE;
}

int AM_Initialize() {
  pTimerEnt = NULL;
  InitAdminModData(TRUE, FALSE);
  return RESULT_CONTINUE;
}

int AM_OnFreeEntPrivateData( edict_t *pent ) {
  if(pent==pTimerEnt && pTimerEnt!=NULL) { // intercept the timer
    //This seems to crash.  Prolly a bad idea.
    //CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pTimerEnt);
    //if (pEntity) delete(pent);
    return RESULT_HANDLED;
  } 
  
  return RESULT_CONTINUE;
}

void KickHighestPinger( const char *pszName,char *ip,edict_t *pEntity) {
  int iAccess;
  int iLoss = 0;
  int iMaxPing = 0;
  int iPing = 0;
  int iPlayerIndex = 0;
  char name[BUF_SIZE];
  char szCommand[BUF_SIZE];
  CBaseEntity* pPlayer;
  
  for (int i = 1; i <= gpGlobals->maxClients; i++) {
    pPlayer = UTIL_PlayerByIndex(i);
    
    if(!IsPlayerValid(pPlayer)) 
      continue;
    
    PLAYER_CNX_STATS(pPlayer->edict(),&iPing,&iLoss);
    if ( iPing > iMaxPing) {
      iAccess = GetUserAccess(pPlayer->edict());
      if (!(IsIPReserved(g_AuthArray[i].sIP) 
	    || ((iAccess & ACCESS_RESERVE_SPOT) == ACCESS_RESERVE_SPOT) 
	    || ((iAccess & ACCESS_IMMUNITY) == ACCESS_IMMUNITY 
		&& (int)CVAR_GET_FLOAT("admin_ignore_immunity") == 0))) {
	iPlayerIndex = i;
	iMaxPing = iPing;
	strcpy(name,STRING(pPlayer->pev->netname));
      }
    }
  }
  
  if ( iPlayerIndex != 0) {
    pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
    
#ifndef _WIN32
    snprintf(szCommand,BUF_SIZE,"kick # %i\n", GETPLAYERUSERID( pPlayer->edict()));
#else
    sprintf(szCommand,"kick # %i\n",GETPLAYERUSERID( pPlayer->edict()) );
#endif
    
    char* sReserveKickMsg = (char*)CVAR_GET_STRING("reserve_slots_msg");
    
    if (sReserveKickMsg == NULL || FStrEq(sReserveKickMsg,"0")) {
      CLIENT_PRINTF( pPlayer->edict(), print_console, "[ADMIN] The Server Admin has enabled reserved slots. You have been kicked due to another player taking a reserved slot\n");
    } else {
      CLIENT_PRINTF( pPlayer->edict(), print_console, sReserveKickMsg);
    }
    
    SERVER_COMMAND(szCommand);
    UTIL_LogPrintf( "[ADMIN] kicked %s due to reserve_slots\n",STRING(pPlayer->pev->netname) );
  }
}

void* LoadScript(AMX *amx,char *filename) {
  FILE* fp;
  AMX_HEADER hdr;
  void* program;
  
  if ( (fp = fopen( filename, "rb" )) != NULL ) {
    fread(&hdr, sizeof hdr, 1, fp);
    if ( (program = calloc(1, (int)hdr.stp)) != NULL ) {
      rewind( fp );
      fread( program, 1, (int)hdr.size, fp );
      fclose( fp );
      memset(amx, 0, sizeof(AMX));
      if ( amx_Init( amx, program ) != AMX_ERR_NONE ) {
	free(program);
	return NULL;
      }
      return program;
    }
  }
  return NULL;
}
