/* This file implements the admin mod commands needed for user interaction
 *
 *  Copyright (c) Alfred Reynolds, 2000
 *  This file is covered by the GPL.
 *
 * $Id: admin_commands.cpp,v 1.37 2001/08/28 08:09:14 alfred Exp $
 *
 */

#include <string.h>

#ifndef _WIN32
  #include <regex.h>
#else
  #include "gnuregex.h"
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
#include "amutil.h"

#include <sys/types.h>
#include <sys/stat.h>

#ifndef WIN32
  #include <unistd.h>  
#endif

#ifdef _WIN32
  #include <io.h>
  #include <direct.h>
#endif

/* OMFG...amx.h includes a #pragma pack directive that caused it to
screw with the entvars_t structure. It should always be included
_AFTER_ extdll.h */
#include "amx.h"
#include "statics.h"

#define print_tty 3


extern AMXINIT amx_Init;
extern AMXREGISTER amx_Register;
extern AMXFINDPUBLIC amx_FindPublic;
extern AMXEXEC amx_Exec;
extern AMXGETADDR amx_GetAddr;
extern AMXSTRLEN amx_StrLen;
extern AMXRAISEERROR amx_RaiseError;
extern AMXSETSTRING amx_SetString;
extern AMXGETSTRING amx_GetString;

extern DLL_GLOBAL edict_t *pTimerEnt;
extern DLL_GLOBAL edict_t *pAdminEnt;

#ifdef CS_TEAMINFO
extern FIV myCountTeams;
extern FII myCountTeamPlayers;
#endif

extern DLL_GLOBAL mapcycle_t mapcycle;
extern DLL_GLOBAL CLinkList<word_struct>* m_pWordList;
extern auth_struct g_AuthArray[MAX_PLAYERS + 1];

extern AMX_NATIVE_INFO fixed_Natives[];
extern AMX_NATIVE_INFO admin_Natives[];

extern DLL_GLOBAL BOOL  g_fIPsLoaded;
extern DLL_GLOBAL BOOL  g_fModelsLoaded;
extern DLL_GLOBAL BOOL  g_fUsersLoaded;
extern DLL_GLOBAL BOOL  g_fVaultLoaded;
extern DLL_GLOBAL BOOL  g_fWordsLoaded;
extern DLL_GLOBAL BOOL  g_fRunScripts;
extern DLL_GLOBAL BOOL  g_fRunPlugins;

extern int GameDispatchSpawn( edict_t *pent );

// CEM - 12/27/2000 - Not really necessary, but nice to have.
char* INVALID_TIMER_PARAM = NULL;

// Using this macro requires the following variable declarations:
//  int iNumParams = params[0] / sizeof(cell);
//
// 'Required' should be the required number of params; eg, 2.
#define CHECK_AMX_PARAMS(Required) \
	if (iNumParams < Required) { \
		System_Error(UTIL_VarArgs("[ADMIN] ERROR: Missing required parameters.  Required %i, has %i.\n", Required, iNumParams),pAdminEnt); \
		amx_RaiseError(amx,AMX_ERR_NATIVE); \
		return 0; \
	}

// Using this macro requires the following variable declarations:
//	int iLength;
//  cell* cStr;    
//
// 'Param' should be an integer (referring to an element of the
// params array), MaxLength should be the maximum length of the
// character array to be written to (to check for overrun), and
// String should be a character array.
#define GET_AMX_STRING(Param,MaxLength,String) \
	amx_GetAddr(amx,params[Param],&cStr); \
	amx_StrLen(cStr,&iLength); \
	if (iLength >= MaxLength) { \
		System_Error(UTIL_VarArgs("[ADMIN] ERROR: AMX string too large.  Max size %i, is %i.\n", MaxLength, iLength),pAdminEnt); \
		amx_RaiseError(amx,AMX_ERR_NATIVE); \
		return 0; \
	} \
	amx_GetString(String,cStr);

#define SET_AMX_STRING(Param,MaxLength,String) \
	amx_GetAddr(amx,params[Param],&cStr); \
	if (String == NULL) { \
		amx_SetString(cStr, "", 0); \
	} else if((int)strlen(String) > MaxLength) { \
		System_Error(UTIL_VarArgs("[ADMIN] ERROR: AMX string too small.  Required size %i, is %i.\n", strlen(String), MaxLength),pAdminEnt); \
		amx_RaiseError(amx,AMX_ERR_NATIVE); \
		return 0; \
	} else { \
		amx_SetString(cStr,const_cast<char*>(String),0); \
	}


// access(access,target = "") -> returns whether 'target' has 'access' or not.
// if 'target' is empty, use the user who executed the command.
static cell access(AMX *amx, cell *params) {
  int iAccess;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sUser[BUF_SIZE];
  cell* cStr;    
  
  CHECK_AMX_PARAMS(1);
  iAccess = params[1];
  if (iNumParams >= 2) {
    GET_AMX_STRING(2,BUF_SIZE,sUser);
  } else {
    sUser[0] = '\0';
  }
  
  if(strlen(sUser) > 0) {
    int iPlayerIndex = GetPlayerIndex(sUser);
    if (iPlayerIndex == 0) {
      System_Response(UTIL_VarArgs("[ADMIN] (access) Unable to find player: %s\n", sUser),pAdminEnt);
      return 0;
    }
    CBaseEntity *pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
    if (IsPlayerValid(pPlayer)) {
      if ((GetUserAccess(pPlayer->edict()) & iAccess) == iAccess) {
	return 1;
      }
    }
  } else {	  
    // If we're console, we have full access
    if(pAdminEnt==NULL) 
      return 1;
    if ((GetUserAccess(pAdminEnt) & iAccess) == iAccess) {
      return 1;
    }
  }
  return 0;
}

// auth(target) -> returns 1 if 'target' has a matching user record.
// if 'target' is empty, use the user who executed the command.
static cell auth(AMX *amx, cell *params) {
  int iIndex;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  cell* cStr;
  char sUser[BUF_SIZE];
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,BUF_SIZE,sUser);
  
  if(sUser == NULL || strlen(sUser)==0) {
    // The console should always be considered authed.
    if(pAdminEnt==NULL) 
      return 1; 
    iIndex = ENTINDEX(pAdminEnt);
  } else {
    iIndex = GetPlayerIndex((char*)sUser);
  }
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    System_Response(UTIL_VarArgs("[ADMIN] (auth) Unable to find player: %s\n", sUser),pAdminEnt);
    return 0;
  }
  return ((g_AuthArray[iIndex].iUserIndex != 0) ? 1 : 0);
}

// ban(user,time) -> Bans 'user' for 'time' (0 is permanent)
static cell ban(AMX *amx, cell *params) {
  int iIsIP = 0;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  int iPlayerIndex = 0;
  int iTime;
  unsigned int uWONID = 0;
  char sCommand[COMMAND_SIZE];
  char sUser[BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(2);
  GET_AMX_STRING(1,BUF_SIZE,sUser);
  iTime = params[2];
  if (sUser == NULL || strlen(sUser) == 0) {
    System_Response("[ADMIN] (ban) You must enter a name.\n",pAdminEnt);
    return 0;
  } else if (iTime < 0) {
    System_Response("[ADMIN] (ban) You cannot enter a negative time.\n",pAdminEnt);
    return 0;
  }
  
  iPlayerIndex = GetPlayerIndex(sUser);
  if (iPlayerIndex > 0) {
    CBaseEntity* pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
    if (IsPlayerValid(pPlayer)) {
      uWONID = GETPLAYERWONID(pPlayer->edict());
    }
  } else if (atoi(sUser) != 0) {
    System_Response("[ADMIN] (ban) User recognized as WON ID.\n",pAdminEnt);
    uWONID = atoi(sUser);
  } else if (IsIPValid(sUser)) {
    System_Response("[ADMIN] (ban) User recognized as IP address.\n",pAdminEnt);
    iIsIP = 1;
  } else {
    System_Response(UTIL_VarArgs("[ADMIN] (ban) Unable to find player: %s\n", sUser),pAdminEnt);
    return 0;
  }
  if (iIsIP) {
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"addip %s\n",sUser);
#else
    sprintf(sCommand,"addip %s\n",sUser);	
#endif	
    SERVER_COMMAND(sCommand);
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"writeip\n");
#else
    sprintf(sCommand,"writeip\n");
#endif
  } else {
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"banid %i %u kick\n",iTime,uWONID);
#else
    sprintf(sCommand,"banid %i %u kick\n",iTime,uWONID);	
#endif	
    SERVER_COMMAND(sCommand);
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"writeid\n");
#else
    sprintf(sCommand,"writeid\n");
#endif
    SERVER_COMMAND(sCommand);
  }
  UTIL_LogPrintf("[ADMIN] Banned: %s\n",sUser);
  return 1;
}

// centersay(data, time, R, G, B) -> puts 'data' in the center of
// the screen for 'time' seconds in the (R,G,B) color.  Uses the
// pretty_say cvar; if enabled, uses special effects like 'fade in',
// etc.
static cell centersay(AMX *amx, cell *params) {
  int i;
  int iLength;
  int iLineFeed = -1;
  int iNumParams = params[0] / sizeof(cell);
  char sText[CENTER_SAY_SIZE];
  cell* cStr;    
  hudtextparms_t m_textParms;
  
  CHECK_AMX_PARAMS(5);
  GET_AMX_STRING(1,CENTER_SAY_SIZE,sText);
  
  if (sText == NULL || strlen(sText) == 0) {
    System_Response("[ADMIN] (centersay) You must say something.\n",pAdminEnt);
    return 1;
  }	
  FormatLine(sText);
  for (i = 0; i < (int)strlen(sText); i++) {
    if ((i - iLineFeed) > CENTER_SAY_LINE_SIZE) {
      System_Response(UTIL_VarArgs("[ADMIN] (centersay) The maximum size for any one line in a centersay is %i.\n",CENTER_SAY_LINE_SIZE),pAdminEnt);
      if (pAdminEnt != NULL) {
	UTIL_LogPrintf("[ADMIN] WARNING: Centersay line limit size exceeded: %s\n", sText);
      }
      return 0;
    }
    if (sText[i] == '\n') {
      iLineFeed = i;
    }
  }
  
  if ((int)CVAR_GET_FLOAT("pretty_say") != 0) {
    m_textParms.r1 = params[3];
    m_textParms.g1 = params[4];
    m_textParms.b1 = params[5];
    m_textParms.a1 = 0;
    m_textParms.r2 = 0;
    m_textParms.g2 = 0;
    m_textParms.b2 = 0;
    m_textParms.a2 = 0;   
    m_textParms.fadeinTime = 0;
    m_textParms.fadeoutTime = 0;
    m_textParms.holdTime = params[2];   
    m_textParms.fxTime = params[2];
    m_textParms.x = -1;
    m_textParms.y = 0.25;
    m_textParms.effect=2;
    m_textParms.channel=1;
    
    UTIL_HudMessageAll( m_textParms,sText );
  } else {
    UTIL_ClientPrintAll(HUD_PRINTCENTER, UTIL_VarArgs("%s\n",sText));    
  }
  UTIL_LogPrintfFNL("[ADMIN] (centersay) %s\n",sText);
  return 1;
}



// changelevel(map) -> changes level to 'map'
static cell changelevel(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char szNextMap[32];
  cell* cStr;    
  char sMap[BUF_SIZE];
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,BUF_SIZE,sMap);
  
  if (sMap == NULL || strlen(sMap) == 0 ) { 
    System_Response("[ADMIN] (changelevel) You must enter a map name.\n",pAdminEnt);
    return 0;
  }
  strncpy( szNextMap, sMap,32);
  fix_string(szNextMap,strlen(szNextMap));
  
  if (!IS_MAP_VALID(szNextMap) ) {
    System_Response(UTIL_VarArgs("[ADMIN] (changelevel) Invalid map name: '%s'\n", sMap),pAdminEnt);
    return 0;
  }
  UTIL_LogPrintf( "[ADMIN] (changelevel) Changing map to %s\n",sMap);
  CHANGE_LEVEL( szNextMap, NULL );
  return 1;
}



// check_user(user) -> Returns 1 if 'user' can be found on the server
static cell check_user_amx(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sUser[BUF_SIZE];
  cell* cStr;    
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,BUF_SIZE,sUser);
  return (GetPlayerIndex(sUser) ? 1 : 0);
}



// convert_string(hlstring,amxstring,maxlength) -> Grabs the data from the
// 'hlstring' pointer and puts it into 'amxstring'.
static cell convert_string(AMX *amx, cell *params) {
  int iLength;
  cell* cStr;
  char* sHLString;
  int iNumParams = params[0] / sizeof(cell);
  
  CHECK_AMX_PARAMS(3);
  sHLString=(char *)params[1];
  iLength = params[3];
  SET_AMX_STRING(2, iLength, sHLString);
  return 1;
}



// currentmap(map, length) -> If 'length' is 0, prints current map to console.  Otherwise,
// puts it in 'map' var.
static cell currentmap(AMX *amx, cell *params) {
  int iMaxLength;
  int iNumParams = params[0] / sizeof(cell);
  cell* cStr;
  
  CHECK_AMX_PARAMS(2);
  iMaxLength = params[2];
  
  if(iMaxLength == 0) {
    System_Response(UTIL_VarArgs( "[ADMIN] Current map: %s\n",STRING(gpGlobals->mapname)),pAdminEnt);
  } else {
    SET_AMX_STRING(1,iMaxLength,STRING(gpGlobals->mapname));
  }
  return 1;
}



// exec(cmd) -> causes 'cmd' to be exec'ed, as if by 'rcon cmd'
static cell exec(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sCommand[COMMAND_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,COMMAND_SIZE,sCommand);
  
  if (sCommand == NULL || strlen(sCommand) == 0) {
    System_Response("[ADMIN] (exec) You must include a command.\n", pAdminEnt);
    return 0;
  }
  
  // We need a newline character, or the command won't get executed.
  if (sCommand[strlen(sCommand)-1]!='\n')	
    strcat(sCommand,"\n");    
  SERVER_COMMAND(sCommand);
  // However, we don't want a newline character in the logs.
  if (sCommand[strlen(sCommand)-1]=='\n') 
    sCommand[strlen(sCommand)-1] = '\0';
  UTIL_LogPrintf( "[ADMIN] Executing command: %s\n",sCommand);
  return 1;
}



// get_vaultdata(key,data,length) -> Sets 'data' to the data associated 
// with 'key'.  
// Returns 1 on success, 0 on failure.
static cell get_vaultdata(AMX *amx, cell *params) {
	int iLength;
	int iMaxLength;
	int iNumParams = params[0] / sizeof(cell);
	cell* cStr;
	char* sData;
	char sKey[BUF_SIZE];

	CHECK_AMX_PARAMS(3);
	GET_AMX_STRING(1,BUF_SIZE,sKey);

	iMaxLength = params[3];

	if (strlen(sKey) > 0) {
		sData = GetVaultData(sKey);
		if (sData == NULL) 
			return 0;
		SET_AMX_STRING(2,iMaxLength,sData);
		return 1;
	}
	return 0;
}



// kick(user) -> kicks user
static cell kick(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  int iPlayerIndex = 0;
  char sCommand[COMMAND_SIZE];
  char sUser[BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,BUF_SIZE,sUser);
  
  if (sUser == NULL || strlen(sUser) == 0) {
    System_Response(  "[ADMIN] (kick) You must enter a name to kick\n",pAdminEnt);
    return 0;
  }
  iPlayerIndex = GetPlayerIndex(sUser);
  if (iPlayerIndex == 0) {
    System_Response( UTIL_VarArgs("[ADMIN] (kick) Unable to find player: %s\n", sUser),pAdminEnt);
    return 0;
  }
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
  if (IsPlayerValid(pPlayer)) {
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"kick # %i\n",GETPLAYERUSERID( pPlayer->edict()));
#else
    sprintf(sCommand,"kick # %i\n",GETPLAYERUSERID(pPlayer->edict()));
#endif
    SERVER_COMMAND(sCommand);
    UTIL_LogPrintf( "[ADMIN] Kicked %s\n",sUser);   
  }
  return 1;
}



// kill_timer(timer id) -> stops the specified timer
static cell kill_timer(AMX *amx, cell *params) {
  int iNumParams = params[0] / sizeof (cell);
  int iTimer;
  
  CHECK_AMX_PARAMS(1);
  // set_timer gives us an off by one index, so compensate
  iTimer = params[1] - 1;
  
  CTimer *pEntity = (CTimer *)GET_PRIVATE(pTimerEnt);
  if (!pEntity->ValidTimerIndex(iTimer)) {
    UTIL_LogPrintf("[ADMIN] ERROR: Timer index %i is not valid.\n", iTimer);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  pEntity->DeleteTimer(iTimer, 1);
  return 1;
}



// listmaps() -> lists the maps in the mapcycle.txt
static cell list_maps(AMX *amx, cell *params) {
  listmaps(pAdminEnt);
  return 1;
}



// log(data) -> writes 'data' to the log
static cell log(AMX *amx, cell *params) {
  int i;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sText[LARGE_BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,LARGE_BUF_SIZE,sText);
  
  if(sText == NULL || strlen(sText) == 0) {
    System_Response("[ADMIN] (log) You must say something.\n",pAdminEnt);
    return 0;
  }
  
  for(i = 0; i < (int)strlen(sText); i++) {
    if(sText[i] == 10 || sText[i] == 13) {
      sText[i] = '@';
    }
  }
  
  UTIL_LogPrintf( "[ADMIN] %s\n",sText);
  return 1;
}



// maxplayercount() -> returns the maximum players allowed on the server
static cell maxplayercount(AMX *amx, cell *params) {
  return gpGlobals->maxClients;
}



// message(target,data) -> Displays 'data' to 'target' in the console
static cell message(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sText[LARGE_BUF_SIZE];
  char sUser[BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(2);
  GET_AMX_STRING(1,BUF_SIZE,sUser);
  GET_AMX_STRING(2,LARGE_BUF_SIZE,sText);
  
  int iPlayerIndex = GetPlayerIndex(sUser);
  if (iPlayerIndex == 0) {
    System_Response( UTIL_VarArgs("[ADMIN] (message) Unable to find player: %s\n", sUser),pAdminEnt);
    return 0;
  }
  CBaseEntity* pPlayer = UTIL_PlayerByIndex(iPlayerIndex);


  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( IsPlayerValid(pPlayer) && GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      System_Response("Cannot message this player: client is a bot.\n",pAdminEnt);
      return 0; 
    }  // if 
  }  // if

  if (!IsPlayerValid(pPlayer)) {
    System_Response(UTIL_VarArgs( "[ADMIN] (message) Unable to message user %s\n",sUser),pAdminEnt);
    return 0;
  } else {
    CLIENT_PRINTF( pPlayer->edict(), print_console, UTIL_VarArgs("%s\n",sText));
  }
  return 1;
}



// messageex(user,data,type) -> like message, but accepts third parameter to
// determine where to display to (console, chat, center, etc)
static cell messageex(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  int iType;
  char sText[LARGE_BUF_SIZE];
  char sUser[BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(3);
  GET_AMX_STRING(1,BUF_SIZE,sUser);
  GET_AMX_STRING(2,LARGE_BUF_SIZE,sText);
  iType = params[3];
  
  int iPlayerIndex = GetPlayerIndex(sUser);
  if (iPlayerIndex == 0) {
    System_Response( UTIL_VarArgs("[ADMIN] (messageex) Unable to find player: %s\n",sUser),pAdminEnt);
    return 0;
  }

  CBaseEntity* pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( IsPlayerValid(pPlayer) && GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      System_Response("Cannot messageex this player: client is a bot.\n",pAdminEnt);
      return 0; 
    }  // if 
  }  // if

  FormatLine( sText );
  
  if (!IsPlayerValid(pPlayer)) {
    System_Response(UTIL_VarArgs( "[ADMIN] (messageex) Unable to message user %s\n",sUser),pAdminEnt);
    return 0;
  } else if (iType == print_center) {
    CLIENT_PRINTF(pPlayer->edict(), print_center, UTIL_VarArgs("%s\n",sText));
  } else if (iType == print_chat) {
    // we need to print to ourselves unreliably otherwise we crash the server
    entvars_t *pev = &pPlayer->edict()->v;
    if (pPlayer->edict() != pAdminEnt) {
      ClientPrint(pev, HUD_PRINTTALK, UTIL_VarArgs("%s\n", sText) );
    } else {
      UTIL_ClientPrint_UR(pev, HUD_PRINTTALK, UTIL_VarArgs("%s\n", sText),0,0,0,0 );
    }
  } else if (iType == print_tty) {
    // Print using the teletype effect
    hudtextparms_t m_textParms;
    m_textParms.r1 = 255;
    m_textParms.g1 = 250;
    m_textParms.b1 = 0;
    m_textParms.a1 = 0;
    m_textParms.r2 = 255;
    m_textParms.g2 = 255;
    m_textParms.b2 = 250;                                                            
    m_textParms.a2 = 0;   
    m_textParms.fadeinTime = 0.02;
    m_textParms.fadeoutTime = 0;
    m_textParms.holdTime = 7;
    m_textParms.fxTime = 0.06;
    m_textParms.effect = 2;
    m_textParms.x = 0.05;
    m_textParms.y = 0.7;
    m_textParms.channel = 1;
    UTIL_HudMessage( pPlayer, m_textParms, sText );	
  } else {
    // Default is to the console
    CLIENT_PRINTF(pPlayer->edict(), print_console, UTIL_VarArgs( "%s\n",sText));
  }
  return 1;
}



// nextmap(map, length) -> If 'length' is 0, prints next map to console.  Otherwise,
// puts it in 'map' var.
static cell nextmap(AMX *amx, cell *params) {
  int iMaxLength;
  int iNumParams = params[0] / sizeof(cell);
  cell* cStr;
  
  CHECK_AMX_PARAMS(2);
  iMaxLength = params[2];
  
  
  mapcycle_item_s *item;
  char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
  DestroyMapCycle( &mapcycle );
  ReloadMapCycleFile( mapcfile, &mapcycle );
  item = CurrentMap(&mapcycle);
  
  if(iMaxLength == 0) {
    System_Response(UTIL_VarArgs( "[ADMIN] Next map in cycle: %s\n",item->mapname),pAdminEnt);
  } else {
    SET_AMX_STRING(1,iMaxLength,item->mapname);
  }
  return 1;
}



// playercount() -> returns the current number of players on the server
static cell playercount(AMX *amx, cell *params) {
  return GetPlayerCount();
}



// playerinfo(index,name,length,userid,wonid,team) -> Given 'index', fills the other
// parameters with the appropriate player info.
static cell playerinfo(AMX *amx, cell *params) {
  int iMaxLength;
  int iNumParams = params[0] / sizeof(cell);
  int iPlayerIndex;
  char sName[BUF_SIZE];
  cell *cParam;
  cell* cStr;
  BOOL bIsBot = FALSE;

  CHECK_AMX_PARAMS(3);
  iPlayerIndex = params[1];
  if (iPlayerIndex < 1) {
    System_Error(UTIL_VarArgs("[ADMIN] ERROR: Request for player info for invalid index %d \n", iPlayerIndex, gpGlobals->maxClients),pAdminEnt);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  if (iPlayerIndex > gpGlobals->maxClients) {
    System_Error(UTIL_VarArgs("[ADMIN] ERROR: Request for player info index %d exceeds max player limit %d\n", iPlayerIndex, gpGlobals->maxClients),pAdminEnt);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(iPlayerIndex);
  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( IsPlayerValid(pPlayer) && GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      DEBUG_LOG( 1, ("Getting playerinfo for a bot.\n") );
      //return 0;
      bIsBot = TRUE;
    }  // if 
  }  // if

  if (IsPlayerValid(pPlayer)) {
    strncpy(sName,STRING(pPlayer->pev->netname), BUF_SIZE);
    iMaxLength = params[3];
    SET_AMX_STRING(2, iMaxLength, sName);
    if (iNumParams >= 4) {
      amx_GetAddr(amx,params[4],&cParam);
      *cParam = (cell)GETPLAYERUSERID(pPlayer->edict());
    }
    if (iNumParams >= 5) {
      amx_GetAddr(amx,params[5],&cParam);
      *cParam = (cell)GETPLAYERWONID(pPlayer->edict());
    }
    if (iNumParams >= 6) {
      amx_GetAddr(amx,params[6],&cParam);
      *cParam = (cell)get_player_team( pPlayer );
    }
    if (iNumParams >= 7) {
      amx_GetAddr(amx,params[7],&cParam);
      *cParam = (pPlayer->edict()->v.deadflag == DEAD_NO ? 0 : 1);
    }

    return 1;
  } else {
    return 0;
  }
}



// playsound(user, sound) -> plays 'sound' for 'user' (does nothing
// if user doesn't have that sound.  Works just like execclient(), but
// ignores allow_client_exec.
static cell playsound(AMX *amx, cell *params) {
  cell* cStr;    
  int iLength;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char sCmd[BUF_SIZE];
  char sPlayer[BUF_SIZE];
  
  CHECK_AMX_PARAMS(2);
  GET_AMX_STRING(1,BUF_SIZE,sPlayer);
  GET_AMX_STRING(2,BUF_SIZE,sCmd);
  
  PlayerIndex = GetPlayerIndex(sPlayer);
  if(PlayerIndex==0) 
  	return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if (pPlayer==NULL) 
  	return 0;
    
  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      System_Response("Cannot play sound to client: client is a bot.\n",pAdminEnt);
      return 0; // they don't allow client exec on this server
    }  // if 
  }  // if
  
  CLIENT_COMMAND ( pPlayer->edict(), UTIL_VarArgs("play %s\n", sCmd) );
  return 1;
}


//
// plugin_exec( Command, Data )
// Calls a registered Command from another plugin with the specified Data.
// User access check is the same as if the command was called from the client.
// Returns 1 on successful execution and 0 on failure.
//
static cell plugin_exec( AMX* amx, cell* params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sCmd[BUF_SIZE];
  char sData[LARGE_BUF_SIZE];
  cell* cStr;

  CHECK_AMX_PARAMS( 2 );
  GET_AMX_STRING( 1, BUF_SIZE, sCmd );
  GET_AMX_STRING( 2, LARGE_BUF_SIZE, sData );

  DEBUG_LOG( 1, ("plugin_exec: Command: '%s' Args: '%s'", sCmd, sData) );

  /* Do our hardcoded checks (admin_password, admin_help, admin_version...)...
     We don't call them via plugin_exec()*/  
  if ( FStrEq(sCmd, "admin_password" )) {
    return 0;
  } else if ( !stricmp(sCmd, "admin_help")) {
    return 0;
  } else if ( !stricmp(sCmd, "admin_help_hlrat")) {
    return 0;
  } else if ( !stricmp(sCmd, "admin_version")) {
    return 0;
  }
  
  // execute the command via HandleCommand() if we are using plugins
  if (g_fRunPlugins) {

    int iRetVal = HandleCommand(pAdminEnt, sCmd, sData);
    if ( iRetVal == PLUGIN_ERROR || iRetVal == PLUGIN_NO_ACCESS ) {
      return 0;
    } else {
      return 1;
    }  // if-else
   
  } else {
    // there is no sense in using plugin_exec() if we don't use plugins, is there?
    /*
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
    */
    return 0;
  }

  return 1;
}  //plugin_exec()


// plugin_message(Text) -> Pretty useless test function that returns
// 'Text', much like message(), but identifies the plugin as well.
static cell plugin_message(AMX* amx, cell* params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sBuffer[BUF_SIZE];
  cell* cStr;
  CPlugin* pPlugin = GetPlugin(amx);
  
  if (pPlugin == NULL) {
		System_Error("[ADMIN] ERROR: plugin_registerinfo: Could not find matching amx.\n",pAdminEnt);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
	CHECK_AMX_PARAMS(1);
	GET_AMX_STRING(1,BUF_SIZE,sBuffer);
  
  if (pPlugin->Name() == NULL) {
    System_Response(UTIL_VarArgs("%s: %s\n", pPlugin->File(), sBuffer),pAdminEnt);
  } else {
    System_Response(UTIL_VarArgs("%s: %s\n", pPlugin->Name(), sBuffer),pAdminEnt);
  }
  return 1;
}

// plugin_registercmd(Command, Function, Access, [Help]) -> Registers 'Command' so that
// when a matching string is received, 'Function' will be called, but only if the
// user has 'Access'.  'Help' is an optional entry to add into the help system.
static cell plugin_registercmd(AMX* amx, cell* params) {
  int iAccess;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sCmd[BUF_SIZE];
  char sFunction[BUF_SIZE];
  char sHelp[PLUGIN_HELP_SIZE];
  cell* cStr;
  CPlugin* pPlugin = GetPlugin(amx);
  
  if (pPlugin == NULL) {
    System_Error("[ADMIN] ERROR: plugin_registercmd: Could not find matching amx.\n",pAdminEnt);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  CHECK_AMX_PARAMS(3);
  GET_AMX_STRING(1,PLUGIN_CMD_SIZE,sCmd);
  GET_AMX_STRING(2,BUF_SIZE,sFunction);
  iAccess = params[3];
  if (iNumParams >= 4) {
    GET_AMX_STRING(4,PLUGIN_HELP_SIZE,sHelp);    
    if (strlen(sHelp) > 0) {
      AddHelpEntry(sCmd,sHelp,iAccess);
    }
  }
  
  return (pPlugin->AddCommand(sCmd, sFunction, iAccess) ? 1: 0);
}

// plugin_registerhelp(Command, Access, Help) -> Adds a help entry for 'Command',
// with the text 'Help'.  Shown only to those people with 'Access'.
static cell plugin_registerhelp(AMX* amx, cell* params) {
  int iAccess;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sCmd[PLUGIN_CMD_SIZE];
  char sHelp[PLUGIN_HELP_SIZE];
  cell* cStr;
  CPlugin* pPlugin = GetPlugin(amx);
  
  if (pPlugin == NULL) {
    System_Error("[ADMIN] ERROR: plugin_registerhelp: Could not find matching amx.\n",pAdminEnt);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  CHECK_AMX_PARAMS(3);
  GET_AMX_STRING(1,PLUGIN_CMD_SIZE,sCmd);  
  iAccess = params[2];
  GET_AMX_STRING(3,PLUGIN_HELP_SIZE,sHelp);
  
  if(AddHelpEntry(sCmd,sHelp,iAccess)) {
    return 1;
  } else {
    return 0;
  }
}

// plugin_registerinfo(Name, Description, Version) -> Sets up the data
// that admin_version will return.
static cell plugin_registerinfo(AMX* amx, cell* params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sDesc[BUF_SIZE];
  char sName[BUF_SIZE];
  char sVersion[BUF_SIZE];
  cell* cStr;
  CPlugin* pPlugin = GetPlugin(amx);
  
  if (pPlugin == NULL) {
    System_Error("[ADMIN] ERROR: plugin_registerinfo: Could not find matching amx.\n",pAdminEnt);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  CHECK_AMX_PARAMS(3);
  GET_AMX_STRING(1,BUF_SIZE,sName);
  GET_AMX_STRING(2,BUF_SIZE,sDesc);
  GET_AMX_STRING(3,BUF_SIZE,sVersion);
  
  pPlugin->SetName(sName);
  pPlugin->SetDesc(sDesc);
  pPlugin->SetVersion(sVersion);
  return 1;
}


// reload() -> Reloads the AM data files
static cell reload(AMX *amx, cell *params) {
  if (pAdminEnt != NULL) 
    UTIL_LogPrintf("[ADMIN] Reloading files.\n" );
  InitAdminModData(FALSE, TRUE);
  return 1;
}



// say(data) -> says 'data' as if said by 'rcon say'
static cell say(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sText[LARGE_BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,LARGE_BUF_SIZE,sText);
  
  if (sText == NULL || strlen(sText) == 0) {
    System_Response("[ADMIN] (say) You must say something.\n",pAdminEnt);
    return 1;
  }

  FormatLine( sText );

  UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%s\n",sText));
  UTIL_LogPrintf("[ADMIN] (say) %s\n",sText);
  return 1;
}



// selfmessage(data) -> like message, but always to the user who executed the command.
// Good for users who are connecting, and can't be looked up yet...can also display
// to the console, which message can't, etc.
static cell selfmessage(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sText[BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,BUF_SIZE,sText);
  System_Response(UTIL_VarArgs("%s\n",sText),pAdminEnt);
  return 1;
}



// set_timer(function,wait,repeatcount,parameter = "") -> Sets a timer to call 'function'
// every 'wait' seconds for 'repeatcount' times.
static cell set_timer(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  int iRepeat;
  int iWait;
  char sFunction[BUF_SIZE];
  char sParam[BUF_SIZE];
  cell* cStr;    
  
  CHECK_AMX_PARAMS(3);
  GET_AMX_STRING(1,BUF_SIZE,sFunction);
  iWait = params[2];
  iRepeat = params[3];
  if(iNumParams >= 4) {
    GET_AMX_STRING(4,BUF_SIZE,sParam);
  } else {
    sParam[0] = '\0';
  }	
  
  // Update the timer to make sure it calls the correct 
  // timer next
  CTimer *pEntity = (CTimer *)GET_PRIVATE(pTimerEnt);  
  int iTimer = pEntity->AddTimer(amx,iWait,iRepeat,sFunction,sParam,pAdminEnt);
  if (iTimer == INVALID_TIMER)  {
    UTIL_LogPrintf("[ADMIN] ERROR: set_timer returned invalid timer index for function %s, wait %i, repeat %i.\n", sFunction,iWait,iRepeat);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } 
  
  // Add one here, so we can differentiate from an error result
  DEBUG_LOG(1, ("Timer for function %s, wait %i, repeat %i added as index %i.\n", sFunction, iWait, iRepeat, iTimer) );

  return (iTimer + 1);
}



// set_vaultdata(key,data) -> Sets the data associated with 'key' to 'data'.
// Returns 1 on success, 0 on failure.
static cell set_vaultdata(AMX *amx, cell *params) {
	int iLength;
	int iNumParams = params[0] / sizeof(cell);
	cell* cStr;
	char sData[BUF_SIZE];
	char sKey[BUF_SIZE];

	CHECK_AMX_PARAMS(2);
	GET_AMX_STRING(1,BUF_SIZE,sKey);
	GET_AMX_STRING(2,BUF_SIZE,sData);
	SetVaultData(sKey,sData);
	return 1;
}



// timeleft(printconsole = 1) -> Returns the time left on the map in seconds.  If 
// printconsole is 1, prints out to the console, too.
static cell timeleft(AMX *amx, cell *params) {
  int iNumParams = params[0] / sizeof(cell);
  
  float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60; // map timelimit in sec
  if (iNumParams == 0 || params[1] == 1) {
    int timeleft = flTimeLimit - gpGlobals->time;
    int minleft = (int)(timeleft) / 60;
    int secleft = (int)(timeleft) % 60;
    System_Response(UTIL_VarArgs( "[ADMIN] Time left on map: %d:%d.\n",minleft, secleft), pAdminEnt);
  }  // if
  return (cell)(flTimeLimit - gpGlobals->time);
}           



// typesay(data, time, R, G, B) -> types 'data' out just above the
// char area for 'time' seconds in the (R,G,B) color.  
static cell typesay(AMX *amx, cell *params) {
	int iLength;
	int iNumParams = params[0] / sizeof(cell);
	char sText[LARGE_BUF_SIZE];
	cell* cStr;
	hudtextparms_t m_textParms;

	CHECK_AMX_PARAMS(5);
	GET_AMX_STRING(1,LARGE_BUF_SIZE,sText);

	if (sText == NULL || strlen(sText) == 0) {
	  System_Response("[ADMIN] (typesay) You must say something.\n",pAdminEnt);
	  return 1;
	}

  	if ( strlen(sText) > CENTER_SAY_LINE_SIZE) {
      		System_Response(UTIL_VarArgs("[ADMIN] (centersay) The maximum size for any one line in a centersay is %i.\n",CENTER_SAY_LINE_SIZE),pAdminEnt);
      	if (pAdminEnt != NULL) {
        	UTIL_LogPrintf("[ADMIN] WARNING: Centersay line limit size exceeded: %s\n", sText);
      	}
      	return 0;
    }


	FormatLine( sText );

	m_textParms.r1 = params[3];
	m_textParms.g1 = params[4];
	m_textParms.b1 = params[5];
	m_textParms.a1 = 0;
	m_textParms.r2 = 255;
	m_textParms.g2 = 255;
	m_textParms.b2 = 250;
	m_textParms.a2 = 0;
	m_textParms.fadeinTime = 0.02;
	m_textParms.fadeoutTime = 0;
	m_textParms.holdTime = params[2];
	m_textParms.fxTime = 0.06;
	m_textParms.effect = 2;
	m_textParms.x = 0.05;
	m_textParms.y = 0.65;
	m_textParms.channel=1;
	UTIL_HudMessageAll( m_textParms,sText );
	UTIL_LogPrintfFNL( "[ADMIN] (typesay) %s\n", sText);
	return 1;
}

 


// unban(user) -> unbans user
static cell unban(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sCommand[COMMAND_SIZE];
  char sUser[BUF_SIZE];
  cell* cStr;
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,BUF_SIZE,sUser);
  
  if (sUser == NULL || strlen(sUser) == 0) {
    System_Response( "[ADMIN] (unban) You must enter a name to unban",pAdminEnt);
    return 0;
  }
  
  if (IsIPValid(sUser)) {
    System_Response("[ADMIN] (unban) User recognized as IP address.\n",pAdminEnt);
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"removeip %s\n",sUser);
#else
    sprintf(sCommand,"removeip %s\n",sUser);
#endif
    SERVER_COMMAND(sCommand);
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"writeip\n");
#else
    sprintf(sCommand,"writeip\n");
#endif
    SERVER_COMMAND(sCommand);
  } else {
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"removeid %s\n",sUser);
#else
    sprintf(sCommand,"removeid %s\n",sUser);
#endif
    SERVER_COMMAND(sCommand);
#ifdef HAS_SNPRINTF
    snprintf(sCommand,COMMAND_SIZE,"writeid\n");
#else
    sprintf(sCommand,"writeid\n");
#endif
    SERVER_COMMAND(sCommand);
  }
  
  UTIL_LogPrintf( "[ADMIN] Unbanned %s\n",sUser);
  return 1;
}



// valid_map(map) -> returns 1 if 'map' is a valid map
static cell valid_map(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sMap[BUF_SIZE];
  cell* cStr;    
  
  CHECK_AMX_PARAMS(1);
  GET_AMX_STRING(1,BUF_SIZE,sMap);
  return(check_map(sMap)) ;
}



// version() -> prints the version to the console
static cell version(AMX *amx, cell *params) {
  System_Response(UTIL_VarArgs("[ADMIN] Admin Mod version %s\n",MOD_VERSION),pAdminEnt);
  return 1;
}




// vote_multiple(msg,option1,option2...optionN,function,data) -> Call vote for 'msg',
// displayiong 'option1' through 'optionN'.  When done, call 'function', passing in
// param 'data'.  
static cell vote_multiple(AMX *amx, cell *params) {
  int i;
  int iBits = 1;
  int iChoiceCount = 0;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  char sFunction[BUF_SIZE];
  char sOption[BUF_SIZE];
  char sParam[BUF_SIZE];
  char sText[VOTE_SIZE];
  cell* cStr;    
  
  CHECK_AMX_PARAMS(5);
  GET_AMX_STRING(1,VOTE_SIZE,sText);
  GET_AMX_STRING(iNumParams - 1,BUF_SIZE,sFunction);
  GET_AMX_STRING(iNumParams,BUF_SIZE,sParam);

  int iCharLeft = VOTE_SIZE - strlen(sText);
  char* pcNext = sText + strlen(sText);
  // Flesh out the message with all the allowed choices
  for(i = 2; i < (iNumParams - 1) && (i < 11/*(9+2)*/) && (iCharLeft > 0); i++) { // step through each param
    GET_AMX_STRING(i,BUF_SIZE,sOption);
    snprintf( pcNext, iCharLeft,"\n%i: %s", i - 1, sOption );
    iCharLeft -= (iLength + 4);
    pcNext += (iLength + 4);
    //strcat(sText, UTIL_VarArgs("\n%i: %s", i - 1, sOption));
    iBits |= 1 << (i-2); // activate this choice in the menu
    iChoiceCount++;
  }
  
  DEVEL_LOG( 3, ("Vote called: \"%s\"", sText) );
  // Add a new timer for this vote
  CTimer *pEntity = (CTimer *)GET_PRIVATE(pTimerEnt);  
  if (!pEntity->StartVote(amx,sText,iChoiceCount,iBits,sFunction,sParam,pAdminEnt)) {
    System_Error("[ADMIN] ERROR: CTimer::StartVote failed.\n",pAdminEnt);
    //amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } 
  return 1;
}



// vote_allowed() -> Returns 1 if a call to vote_multiple is allowed right now.
static cell vote_allowed(AMX *amx, cell *params) {
  int iVoteFreq = (int)CVAR_GET_FLOAT("vote_freq" );
  
  if (iVoteFreq <= 0) 
    return 0;
  CTimer *pEntity = (CTimer *)GET_PRIVATE(pTimerEnt);
  return (pEntity->VoteAllowed() ? 1 : 0);
}



static cell userlist(AMX *amx, cell *params) {
  int i;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  cell *cString;
  char name[BUF_SIZE];
  
  name[0] = NULL;
  if(iNumParams>=1) {
    amx_GetAddr(amx,params[1],&cString); 
    amx_StrLen(cString,&iLength);
    if (iLength>=BUF_SIZE) {
      amx_RaiseError(amx,AMX_ERR_NATIVE);
      return 0;
    } /* if */ 
    amx_GetString(name,cString);    
  }
  
  iLength = strlen(name);
  // loop through all players
  System_Response("                 Name         Server ID            WON\n",pAdminEnt);
  
  for ( i = 1; i <= gpGlobals->maxClients; i++ ) {
    CBaseEntity *pPlayer = UTIL_PlayerByIndex(i );
    if (pPlayer) {
      if(iLength==0 || strnicmp(name, STRING(pPlayer->pev->netname), iLength)==0) {
	System_Response(UTIL_VarArgs( "%30s    <%i>          WON:%u\n",STRING(pPlayer->pev->netname), GETPLAYERUSERID( pPlayer->edict() ), GETPLAYERWONID( pPlayer->edict()) ),pAdminEnt);
      }
    }
  }
  return 1;
}




static cell getvar(AMX *amx, cell *params) {
  cell* cStr;    
  int len;
  char name[BUF_SIZE];
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */ 
  amx_GetString(name,cStr);    
  char *iszItem=name;
  
  int value = (int)CVAR_GET_FLOAT(name);
  return (cell)value;
}



static cell getstrvar(AMX *amx, cell *params) {
  
  cell* cStr;    
  int len;
  char name[BUF_SIZE];
  char buf[BUF_SIZE];
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */ 
  amx_GetString(name,cStr);    
  char *iszItem=name;
  
  
  strncpy(buf,CVAR_GET_STRING(name),BUF_SIZE);
  
  amx_GetAddr(amx,params[2],&cStr);
  
  if (params[3] < (int)strlen(buf)) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } else
    amx_SetString(cStr,buf,0);
  return 1;
  
}




static cell help(AMX *amx, cell *params) {
  
  cell* cStr;    
  int len;
  char name[BUF_SIZE];
  char *help_file,*help_item,*next_item;
  int length;
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */ 
  amx_GetString(name+1,cStr);    
  name[0]=':';
  char *iszItem=name;
  help_file=(char *)CVAR_GET_STRING("help_file");
  
  if(help_file==NULL || FStrEq(help_file,"0")) return 0;
  
  char *pFileList;
  char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(help_file, &length );
  
  
  if ( pFileList && length )
    {
      DEBUG_LOG( 1, ("help: Loaded help file '%s' of size %d\n",
		       help_file,length) );
      if ( strlen(iszItem)> 2) {
	
	help_item=strstr(aFileList,iszItem);
	
	if(help_item!=NULL) {
	  next_item=strstr(help_item+strlen(name),":");
	  if(next_item!=NULL) (*next_item)='\0';
	  System_Response(UTIL_VarArgs( "%s\n",help_item+1),pAdminEnt);
	  
	}
      } else {
	char *next=aFileList,*last=aFileList;
	
	while(next!=NULL) {
	  
	  next=strchr(last,':');
	  if(next!=NULL) {
	    *next='\0';
	    System_Response(UTIL_VarArgs( "%s\n",last),pAdminEnt);
	  }
	  last=next+1;
	}
	
      }
      
    }
  
  
  
  
  FREE_FILE( aFileList );
  aFileList = 0;
  return 1;
}




static cell strtonum(AMX *amx, cell *params) {
  
  entvars_t *pev = &pAdminEnt->v;
  
  cell* cStr;    
  int len;
  char name[BUF_SIZE];
  
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */ 
  amx_GetString(name,cStr);  
  len=atoi(name); 
  
  return (cell)len;
}



static cell censor_words(AMX *amx, cell *params) {
  int i;
  int iLength;
  int iOffset;
  int iRetVal = 1;
  char sBuffer[BUF_SIZE];
  char sReturnBuffer[BUF_SIZE];
  char* sWord;
  cell* cStr;
  CLinkItem<word_struct>* pLink;
  word_struct* tWord;
  
  // admin can swear all they want ;)
  if(pAdminEnt==NULL) 
    return 1; 
  
  if (m_pWordList == NULL)
    return 1;
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&iLength);
  if (iLength>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(sBuffer,cStr);
  strcpy(sReturnBuffer, sBuffer);
  
  for (i = 0; i < (int)strlen(sBuffer); i++)
    sBuffer[i] = tolower(sBuffer[i]);
  
  pLink = m_pWordList->FirstLink();
  while (pLink != NULL) {
    tWord = pLink->Data();
    sWord = strstr(sBuffer,tWord->sWord);
    while (sWord != NULL) {
      iOffset = (sWord - &sBuffer[0]) - 1;
      for (i = 1; i <= (int)strlen(tWord->sWord); i++) {
	sReturnBuffer[iOffset + i] = '*';
      }
      sWord = strstr((char*)(sWord + 1), tWord->sWord);
      iRetVal = 0;
    }
    pLink = pLink->NextLink();
  }
  amx_SetString(cStr,sReturnBuffer,0);
  return iRetVal;
}



static cell check_words(AMX *amx, cell *params) {
  int i;
  int iLength;
  char sBuffer[BUF_SIZE];
  cell* cStr;
  CLinkItem<word_struct>* pLink;
  word_struct* tWord;
  
  // admin can swear all they want ;)
  if(pAdminEnt==NULL) 
    return 1; 
  
  if (m_pWordList == NULL)
    return 1;
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&iLength);
  if (iLength>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(sBuffer,cStr);  
  
  for (i = 0; i < (int)strlen(sBuffer); i++)
    sBuffer[i] = tolower(sBuffer[i]);
  
  pLink = m_pWordList->FirstLink();
  while (pLink != NULL) {
    tWord = pLink->Data();
    if (strstr(sBuffer, tWord->sWord) != NULL ) {
      DEBUG_LOG( 1, ("check_words: User '%s' matched swear word '%s'\n",STRING(pAdminEnt->v.netname),tWord->sWord) );
      return 0;
    }
    pLink = pLink->NextLink();
  }
  return 1;
}



static cell get_userindex(AMX *amx, cell *params) {
  cell* cStr;    
  cell *Param;
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  amx_GetAddr(amx,params[2],&Param);
  *Param = (cell)PlayerIndex;
  return 1;
}




static cell get_username(AMX *amx, cell *params) {
  cell *cptr;
  cell* cStr;    
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 3) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  amx_GetAddr(amx,params[2],&cptr);
  if(params[3] < (int)strlen(STRING(pPlayer->pev->netname))) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_SetString(cptr,const_cast<char*>(STRING(pPlayer->pev->netname)),0);
  return 1;
}



static cell getteamcount(AMX *amx, cell *params) {
  int i;
  int iCS;
  int iTeam;
  int iTeamCount = 0;
  
  iTeam = params[1];
  
  char* pcMod = GetModDir();
  if (strcmp(pcMod, "cstrike") == 0) {
    iCS = 1;
  } else {
    iCS = 0;
  }
  
  /* Attempt to use the super-duper CS team count function */
#ifdef CS_TEAMINFO
  if (iCS == 1) {
    if ( (myCountTeamPlayers != 0) && (iTeam == 1 || iTeam == 2) ) {
      iTeamCount = myCountTeamPlayers( iTeam );
      DEBUG_LOG( 1, ("getteamcount: CountTeamPlayers returned  '%d' players for team '%d'\n",iTeamCount, iTeam) );
      return iTeamCount;
    }
  }
#endif
  
  /* Use the normal, boring team count function. */
  for (i = 1; i <= gpGlobals->maxClients; i++ ) {
    CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
    if (IsPlayerValid(pPlayer)) {
      /* This is largely irrelevant: if the mod is not CS, then get_player_team
	 will call pPlayer->pev->team anyway...but, y'know, whatever.  It's better than
	 the two totally seperate for loops that were here before. */
      if (iCS == 1) {
	if(get_player_team(pPlayer) == iTeam) iTeamCount++;
      } else {
	if(pPlayer->pev->team==iTeam) iTeamCount++;
      }
    }  // if
  }  // for
  

  DEBUG_LOG( 1, ("getteamcount: Found  '%d' players in team '%d'\n",iTeamCount, iTeam) );

  return iTeamCount;
}

static cell get_userorigin(AMX *amx, cell *params) {
  cell* cStr;    
  cell *Param;
  int len;
  int intValue;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 4) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  if ( (int)CVAR_GET_FLOAT("admin_debug")!=0) {
    System_Response(UTIL_VarArgs( "[ADMIN] DEBUG: Origin For %s: X: %f Y: %f Z: %f\n", 
				  PlayerText, floor(pPlayer->edict()->v.origin.x), 
				  floor(pPlayer->edict()->v.origin.y), 
				  floor(pPlayer->edict()->v.origin.z)),pAdminEnt);
  }
  
  amx_GetAddr(amx,params[2],&Param);
  intValue = floor(pPlayer->edict()->v.origin.x);
  *Param = (cell)intValue;
  
  amx_GetAddr(amx,params[3],&Param);
  intValue = floor(pPlayer->edict()->v.origin.y);
  *Param = (cell)intValue;
  
  amx_GetAddr(amx,params[4],&Param);
  intValue = floor(pPlayer->edict()->v.origin.z);
  *Param = (cell)intValue;
  
  return 1;
}



static cell get_userSessionID(AMX *amx, cell *params) {
  cell* cStr;    
  cell *Param;
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  amx_GetAddr(amx,params[2],&Param);
  *Param = (cell)GETPLAYERUSERID(pPlayer->edict());
  return 1;
}



static cell get_userWONID(AMX *amx, cell *params) {
  cell* cStr;    
  cell *Param;
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  amx_GetAddr(amx,params[2],&Param);
  *Param = (cell)GETPLAYERWONID(pPlayer->edict());
  return 1;
}




static cell execclient(AMX *amx, cell *params) {
  cell* cStr;    
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char CmdText[BUF_SIZE];
  char CmdBuf[BUF_SIZE];
  char PlayerText[BUF_SIZE];
  
  if ((int)CVAR_GET_FLOAT("allow_client_exec")!=1) {
    System_Response("Cannot execute client command: allow_client_exec is not set to 1.\n",pAdminEnt);
    return 0; // they don't allow client exec on this server
  }
  
  if (iNumParams < 2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if (pPlayer==NULL) return 0;
  
  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      System_Response("Cannot execute client command: client is a bot.\n",pAdminEnt);
      return 0; // they don't allow client exec on this server
    }  // if 
  }  // if

  amx_GetAddr(amx,params[2],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(CmdText,cStr);
    

  // prepare the command to be executed
  BOOL bMoreCmds = FALSE;
  apat* pCmd = 0;
  char* pcStart = 0;
  char* pcEnd = 0;
  char* pcEOB = 0;
  memcpy( CmdBuf, CmdText, BUF_SIZE );
  pcEOB = CmdBuf + strlen( CmdBuf );
  pcStart = CmdBuf;
  while ( pcStart < pcEOB ) {
    pcStart = pcStart + strspn( pcStart, " ;\"" );
    if ( (pcEnd = pcStart + strcspn(pcStart, " ;\"")) != pcStart ) {
      if ( *pcEnd == ';' ) bMoreCmds = TRUE;
      *pcEnd = 0;
    }  // if
    if ( !(pCmd = pat_search_key(eclist, pcStart)) ) {
      System_Response( UTIL_VarArgs("Sorry, the server is unable to execute command \"%s\" on that client.\n", pcStart), pAdminEnt );
      return 0;
    } // if
    if ( pcEnd++ != pcStart ) {
      if ( pCmd->atAttribute == 1 ) { // bind
	pcStart = pcEnd + strcspn( pcEnd, " \";" );
	continue;
      }  // if
      if ( bMoreCmds ) {
	pcStart = pcEnd;
	bMoreCmds = FALSE;
	continue;
      }  // if
      if ( (pcStart = strchr(pcEnd, ';')) ) {
	pcStart++;
	continue;
      } else {
	break;
      }  // if-else
    } else {
      break;
    }  // if-else
  }  // while

				

  if (!stricmp(CmdText,"kill")) {
    if (pPlayer->edict()->v.deadflag != DEAD_NO) {
      System_Response("That player is not currently alive.  The 'kill' command would have no effect.\n",pAdminEnt);
      return 0;
    }

    if((int)CVAR_GET_FLOAT("admin_fx") != 0) {
    	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pPlayer->pev->origin);
    	WRITE_BYTE(TE_TAREXPLOSION);
    	WRITE_COORD(pPlayer->edict()->v.origin.x);
    	WRITE_COORD(pPlayer->edict()->v.origin.y);
    	WRITE_COORD(pPlayer->edict()->v.origin.z);
    	MESSAGE_END();
    
    	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pPlayer->pev->origin);
    	WRITE_BYTE(TE_LAVASPLASH);
    	WRITE_COORD(pPlayer->edict()->v.origin.x);
    	WRITE_COORD(pPlayer->edict()->v.origin.y);
    	WRITE_COORD(pPlayer->edict()->v.origin.z);
    	MESSAGE_END();
    }
  }
  
  CLIENT_PRINTF( pPlayer->edict(), print_console,UTIL_VarArgs("%s %s.\n",get_am_string(0,0,statstr[4],statstr_table),CmdText));
  CLIENT_COMMAND ( pPlayer->edict(), UTIL_VarArgs("%s\n", CmdText) );
  return 1;
}


static cell slay(AMX *amx, cell *params) {
  cell* cStr;    
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 1) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if (pPlayer==NULL) return 0;
  
  
  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      System_Response("Cannot execute slay command: client is a bot.\n",pAdminEnt);
      return 0; // they don't allow client exec on this server
    }  // if 
  }  // if
  

  if (pPlayer->edict()->v.deadflag != DEAD_NO) {
    System_Response("That player is not currently alive.  The 'slay' command would have no effect.\n",pAdminEnt);
    return 0;
  }
  
  if((int)CVAR_GET_FLOAT("admin_fx") != 0) {
    MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pPlayer->pev->origin);
    WRITE_BYTE(TE_TAREXPLOSION);
    WRITE_COORD(pPlayer->edict()->v.origin.x);
    WRITE_COORD(pPlayer->edict()->v.origin.y);
    WRITE_COORD(pPlayer->edict()->v.origin.z);
    MESSAGE_END();
    
    MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pPlayer->pev->origin);
    WRITE_BYTE(TE_LAVASPLASH);
    WRITE_COORD(pPlayer->edict()->v.origin.x);
    WRITE_COORD(pPlayer->edict()->v.origin.y);
    WRITE_COORD(pPlayer->edict()->v.origin.z);
    MESSAGE_END();
  }
  
 
  CLIENT_COMMAND ( pPlayer->edict(), "kill\n" );
  //other_gFunctionTable.pfnClientKill( pPlayer->edict() );
  return 1;
}




static cell teleport(AMX *amx, cell *params) {
  cell* cStr;    
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 4) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  BOOL bFx = TRUE;

  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      bFx = FALSE;
    }  // if
  }  // if

  if ( bFx ) {
      // Show teleport effect at where the teleportee is (code courtesy of Abraxus)  
      if ((int)CVAR_GET_FLOAT("admin_fx") != 0) {
	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pPlayer->pev->origin);
	WRITE_BYTE(TE_TELEPORT);
	WRITE_COORD(pPlayer->edict()->v.origin.x);
	WRITE_COORD(pPlayer->edict()->v.origin.y);
	WRITE_COORD(pPlayer->edict()->v.origin.z);
	MESSAGE_END();
      }

      CLIENT_PRINTF( pPlayer->edict(), print_console,"You have been teleported by the server administrator.\n");
    }
  
  pPlayer->edict()->v.origin.x = (float)params[2];
  pPlayer->edict()->v.origin.y = (float)params[3];
  pPlayer->edict()->v.origin.z = (float)params[4];

  if ( bFx ) {
    // Show teleport effect at where the teleportee goes
    if ((int)CVAR_GET_FLOAT("admin_fx") != 0) {
      MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, pPlayer->pev->origin);
      WRITE_BYTE(TE_TELEPORT);
      WRITE_COORD(pPlayer->edict()->v.origin.x);
      WRITE_COORD(pPlayer->edict()->v.origin.y);
      WRITE_COORD(pPlayer->edict()->v.origin.z);
      MESSAGE_END();
    }
  }

  return 1;
}

/****************************************************************************************
 *              FILE  ACCESS  FUNCTIONS                                                 *
 ****************************************************************************************/

static cell fileexists(AMX *amx, cell *params) {
  
  cell* cStr;    
  int iLen;
  int iNumParams = params[0]/sizeof(cell);
  char acFilename[BUF_SIZE];
  FILE* fFile;
  
  if ( iNumParams < 1 ) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }  // if
  
  amx_GetAddr( amx, params[1], &cStr ); 
  amx_StrLen( cStr, &iLen );
  if ( iLen >= BUF_SIZE ) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }  // if
  amx_GetString( acFilename, cStr ); 
  
  char acFilePath[PATH_MAX];
  if ( get_file_path(acFilePath, acFilename, PATH_MAX, 0) > 0 ) {  
    DEBUG_LOG( 1, ("fileexists looking for file %s\n", acFilePath) );
    
    if( (fFile = fopen(acFilePath,"r")) == 0 ) {
      return 0;
    }  // if
    
    fclose( fFile );
  } else {
    return 0;
  }  // if-else

  return 1; 
  
}  // fileexists()



static cell filesize(AMX *amx, cell *params) {
  
#define FSUNIT_BYTES 0
#define FSUNIT_LINES 1

  cell* cStr;    
  int iLen;
  int iNumParams = params[0]/sizeof(cell);
  int iRetVal = 0;
  char acFilename[BUF_SIZE];
  FILE* fFile;
  
  if ( iNumParams < 2 ) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }  // if

  amx_GetAddr( amx, params[1], &cStr ); 
  amx_StrLen( cStr, &iLen );
  if (iLen >= BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }  // if
  amx_GetString( acFilename, cStr ); 

  int iUnits = params[2];

  char acLine[BUF_SIZE];
  char acFilePath[PATH_MAX];

  int iNumLines = 0;
  int iNumBytes = 0;
  char* pcLine = NULL;

  if ( get_file_path(acFilePath, acFilename, PATH_MAX, "file_access_read") > 0 ) {  
    
#ifdef LINUX
    // we don't have to read the file to see how large it is
    if ( iUnits == FSUNIT_BYTES ) {
      struct stat file;
      iRetVal = stat( acFilePath, &file );

      if ( iRetVal >= 0 ) {
      // if the file is empty we don't print anything
	return  (cell)file.st_size;
      } else {
	return 0;
      }  // if-else
    }  // if
#endif

    if( (fFile = fopen(acFilePath,"r")) == NULL ) {
      return 0;
    }  // if

    pcLine = fgets( acLine, BUF_SIZE, fFile );
    while ( pcLine != NULL ) {
      pcLine = fgets( acLine, BUF_SIZE, fFile );
      iNumLines++;
      if ( pcLine != 0 ) {
	iNumBytes += strlen( pcLine );
      }  // if
    }  // while

    fclose( fFile );
    
    switch ( iUnits ) {
    case FSUNIT_BYTES:
      return iNumBytes;
      break;
    case FSUNIT_LINES:
      return iNumLines;
      break;
    default:
      break;
    }  // switch

  }  // if

  return 0;
}  // filesize()




static cell readfile(AMX *amx, cell *params) {
  
  cell *cptr;
  cell* cStr;    
  int iLen;
  int iNumParams = params[0]/sizeof(cell);
  char acFilename[BUF_SIZE];
  FILE* fFile;
  char acLine[BUF_SIZE];
  char* pcLine = NULL;

  if (iNumParams < 4) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }  // if
  
  amx_GetAddr( amx, params[1], &cStr ); 
  amx_StrLen( cStr, &iLen );
  if (iLen >= BUF_SIZE ) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }  // if
  amx_GetString( acFilename, cStr ); 
  
  int line_no=(int)params[3];     
  
  memset(acLine, 0x0, BUF_SIZE );
  
  char acFilePath[PATH_MAX];
  int i;

  if ( get_file_path(acFilePath, acFilename, PATH_MAX, "file_access_read") > 0 ) {  
    
    if( (fFile = fopen(acFilePath,"r")) == NULL ) {
      return 0;
    }  // if
    
    for ( i = 0; i < line_no; i++ ) {                
      pcLine = fgets( acLine, BUF_SIZE, fFile );
      if( pcLine == NULL ) {
	fclose( fFile );
	return 0;
      }  // if
    }  // for

    fclose( fFile );

    pcLine = strrchr( acLine, '\n' );
    if ( pcLine ) *pcLine = 0;
    pcLine = strrchr( acLine, '\r' );
    if ( pcLine ) *pcLine = 0;

    if ( strlen(acLine) >= 0 ) {
      amx_GetAddr( amx, params[2], &cptr );
      if( params[4] < (cell)strlen(acLine) ) {
	amx_RaiseError( amx, AMX_ERR_NATIVE );
	return 0;
      }  // if
      amx_SetString( cptr, acLine, 0 );
    }  // if

  } else {
    return 0;
  }  // if-else

  return 1; 
  
}  //readfile()




static cell writefile(AMX *amx, cell *params) {
  
  cell* cStr;    
  int iLen;
  int iNumParams = params[0]/sizeof(cell);
  char acFilename[BUF_SIZE];
  char acData[BUF_SIZE];
  FILE* fFile;
#ifdef _WIN32
  const char* pcEOL = "\r\n";
#else
  const char* pcEOL = "\n";
#endif

  if (iNumParams < 3) {
    DEBUG_LOG( 2, ("ERROR:writefile: requires 3 parameters, called with %d.", iNumParams) );
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }
  
  amx_GetAddr( amx, params[1], &cStr ); 
  amx_StrLen( cStr, &iLen );
  if ( iLen >= BUF_SIZE ) {
    DEBUG_LOG( 2, ("ERROR:writefile: max. filename size is BUF_SIZE, provided size is %d..", iLen) ); 
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }
  amx_GetString( acFilename, cStr ); 
  
  amx_GetAddr( amx, params[2], &cStr ); 
  amx_StrLen( cStr, &iLen );
  if ( iLen >= BUF_SIZE ) {
    DEBUG_LOG( 2, ("ERROR:writefile: max. line size is BUF_SIZE, provided size is %d..", iLen) ); 
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }
  amx_GetString( acData, cStr ); 
  
  char acFilePath[PATH_MAX];
  char acLine[BUF_SIZE];

  if ( get_file_path(acFilePath, acFilename, PATH_MAX, "file_access_write") > 0 ) {  
    
      
    if( (fFile = fopen(acFilePath,"a+")) == NULL ) {
      DEBUG_LOG( 2, ("ERROR:writefile: fopen failed on filename %s: %s.", acFilePath, strerror(errno)) ); 
      amx_RaiseError( amx, AMX_ERR_NATIVE );
      return 0;
    }
    
    if( params[3] == -1 ) { // default is to just append
      fprintf( fFile, "%s%s", acData, pcEOL);
      DEBUG_LOG( 1, ("Appending to file") );
      fclose( fFile );
    } else {
      FILE *tmp_file;
      char acTempFilePath[PATH_MAX];
    
      /*
      // We used the open above to create the file if it isn't there. 
      // We have to close it again before we can write to it.
      fclose( fFile );

      if( (fFile = fopen(acFilePath,"r")) == NULL ) {
	DEBUG_LOG( 2, ("ERROR:writefile: fopen failed on filename %s: %s.", acFilePath, strerror(errno)) ); 
	amx_RaiseError( amx, AMX_ERR_NATIVE );
	return 0;
      }
      */

      rewind( fFile );

      get_file_path( acTempFilePath, "adminmod.tmp", PATH_MAX, "file_access_write" );
      
      if( (tmp_file = fopen(acTempFilePath,"w+")) == NULL ) {
	DEBUG_LOG( 2, ("ERROR:writefile: failed to create temporary file.") ); 
	amx_RaiseError( amx, AMX_ERR_NATIVE );
	fclose( fFile );
	return 0;
      }
      
      int i=0;
      while ( !feof( fFile ) ) {
	memset( acLine, 0, BUF_SIZE );
	i++;
	if ( fgets(acLine, BUF_SIZE, fFile) != NULL) {
	  if( i == params[3] )
	    fprintf( tmp_file, "%s%s", acData, pcEOL);
	  else
	    fputs( acLine, tmp_file );
	}  // if
      }  // while
      // if the line to write to is greater than the last line, append it to the file
      if ( params[3] >= i ) {
	fprintf( tmp_file, "%s%s", acData, pcEOL);
      }	 // if


      // copied to tmp file, now copy back
      fclose( fFile );
      if( (fFile = fopen(acFilePath,"w")) == NULL ) {
	DEBUG_LOG( 2, ("ERROR:writefile: fopen failed on filename %s.", acFilePath) ); 
	amx_RaiseError( amx, AMX_ERR_NATIVE );
	fclose( tmp_file );
	return 0;
      }  // if

      rewind( tmp_file );
      
      
      while ( !feof(tmp_file) ) {
	memset( acLine, 0, BUF_SIZE );
	fgets( acLine, BUF_SIZE, tmp_file);
	fputs( acLine, fFile);
      }  // while
      
      fclose( tmp_file );
      unlink( acTempFilePath );
      fclose( fFile );
       
    }  // if
    
  } else {
    DEBUG_LOG( 2, ("writefile: file access failed for file %s.", acFilename) ); 
    return 0;
  }  // if-else

  return 1; 
  
}  //writefile()




static cell resetfile(AMX *amx, cell *params) {
  
  cell* cStr;    
  int iLen;
  int iNumParams = params[0]/sizeof(cell);
  char acFilename[BUF_SIZE];
  FILE* fFile;
  
  if (iNumParams < 1) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetAddr( amx, params[1], &cStr ); 
  amx_StrLen( cStr, &iLen );
  if (iLen >= BUF_SIZE) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }
  amx_GetString( acFilename, cStr ); 
  
  char acFilePath[PATH_MAX];
  if ( get_file_path(acFilePath, acFilename, PATH_MAX, "file_access_write") > 0 ) {  

#ifdef _WIN32
    if( (fFile = fopen(acFilePath,"w")) == NULL ) {
      amx_RaiseError( amx, AMX_ERR_NATIVE );
      return 0;
    }
    fclose( fFile );
#else
    
    return ( !truncate(acFilePath, 0) );
#endif

  } else {
    return 0;
  }  // if-else

  return 1; 
  
}  // resetfile()




static cell deletefile(AMX *amx, cell *params) {
  
  cell* cStr;    
  int iLen;
  int iNumParams = params[0]/sizeof(cell);
  int iRetVal = 0;
  char acFilename[BUF_SIZE];

  if (iNumParams < 1) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }  // if
  
  amx_GetAddr( amx, params[1], &cStr ); 
  amx_StrLen( cStr, &iLen );
  if ( iLen >= BUF_SIZE ) {
    amx_RaiseError( amx, AMX_ERR_NATIVE );
    return 0;
  }  // if
  amx_GetString( acFilename, cStr ); 
  
  char acFilePath[PATH_MAX];

  if ( get_file_path(acFilePath, acFilename, PATH_MAX, "file_access_write") > 0) {  
    return ( !unlink( acFilePath ) );
  } else {
    return 0;
  }  // if-else

}  // deletefile()




static cell get_userinfo(AMX *amx, cell *params) {
  
  cell *cptr;
  cell* cStr;
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerName[BUF_SIZE];
  char InfoName[BUF_SIZE];
  
  if (iNumParams < 4) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerName,cStr);
  PlayerIndex = GetPlayerIndex(PlayerName);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  amx_GetAddr(amx,params[2],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(InfoName,cStr);
  
  amx_GetAddr(amx,params[3],&cptr);
  
  char *infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pPlayer->edict());
  char *value=g_engfuncs.pfnInfoKeyValue( infobuffer,InfoName );
  
  if(params[4] < (int)strlen(value)) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_SetString(cptr,value,0);
  return 1;
}



static cell get_serverinfo(AMX *amx, cell *params) {
  cell *cptr;
  cell* cStr;
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char InfoName[BUF_SIZE];
  
  if (iNumParams < 3) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetString(InfoName,cStr);
  
  amx_GetAddr(amx,params[2],&cptr);
  
  char *infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(NULL);
  char *value=g_engfuncs.pfnInfoKeyValue( infobuffer,InfoName );
  
  if(params[3] < (int)strlen(value)) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_SetString(cptr,value,0);
  return 1;
}




static cell set_serverinfo(AMX *amx, cell *params) {
  cell* cStr;
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char value[BUF_SIZE];
  char InfoName[BUF_SIZE];
  
  if (iNumParams < 2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetString(InfoName,cStr);
  
  amx_GetAddr(amx,params[2],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetString(value,cStr);
  
  printf("setting KEY %s to %s\n",InfoName,value);
  char *infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(NULL);
  g_engfuncs.pfnSetKeyValue(infobuffer,InfoName,value);
  
  return 1;
}




static cell slap(AMX *amx, cell *params) {
  cell* cStr;    
  int len;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 1) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  
  PlayerIndex = GetPlayerIndex(PlayerText);
  
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if ( !IsPlayerValid(pPlayer) ) return 0;

  /*
  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      System_Response("Cannot slap this player: client is a bot.\n",pAdminEnt);
      return 0; // they don't allow client exec on this server
    }  // if 
  }  // if
  */

  /* Dock 'em five health, if they can stand it */
  if(pPlayer->edict()->v.health > 5) {
    pPlayer->edict()->v.health -= 5;
  }
  
  /* Play a random sound, varying the sound based on the mod */
  float	flRndSound;
  flRndSound = RANDOM_FLOAT ( 0 , 1 ); 
  
  char strMod[32];
  strcpy(strMod, GetModDir());
  if(!stricmp(strMod, "tfc")) {
    // TFC pain sounds
    if ( flRndSound <= 0.33 ) {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "player/pain2.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    } else if ( flRndSound <= 0.66 ) {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "player/pain4.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    } else {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "player/pain6.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    }
  } else if (!stricmp(strMod, "cstrike")) {
    // Counterstrike pain sounds, suggested by Da Rope
    if ( flRndSound <= 0.33 ) {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-3.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    } else if ( flRndSound <= 0.66 ) {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "player/bhit_flesh-2.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    } else {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "player/pl_die1.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    }
  } else {
    // Default sounds - crowbar hitting flesh sounds
    if ( flRndSound <= 0.33 ) {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "weapons/cbar_hitbod1.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    } else if ( flRndSound <= 0.66 ) {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "weapons/cbar_hitbod2.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    } else {
      EMIT_SOUND_DYN2(ENT(pPlayer->pev), CHAN_VOICE, "weapons/cbar_hitbod3.wav", 1, ATTN_NORM, 0, PITCH_NORM);
    }
  }
  
  /* Send 'em sprawling */
  pPlayer->edict()->v.velocity.x += floor(RANDOM_FLOAT(-800,800));
  pPlayer->edict()->v.velocity.y += floor(RANDOM_FLOAT(-800,800));
  if (pPlayer->edict()->v.velocity.z >= 200) {
    pPlayer->edict()->v.velocity.z -= floor(RANDOM_FLOAT(100,200));
  } else {
    pPlayer->edict()->v.velocity.z += floor(RANDOM_FLOAT(100,200));
  }
  return 1;
}




static cell godmode(AMX *amx, cell *params) {
  cell* cStr;    
  int len;
  int iGodMode;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  iGodMode = (int)params[2];
  if(iGodMode!=0) {
    pPlayer->edict()->v.solid = SOLID_NOT;
    pPlayer->edict()->v.takedamage = DAMAGE_NO;
  UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs(const_cast<char*>(get_am_string(0,0,statstr[0],statstr_table)),STRING(pPlayer->pev->netname))); 
  } else {
    pPlayer->edict()->v.solid = SOLID_SLIDEBOX;
    pPlayer->edict()->v.takedamage = DAMAGE_AIM;
  UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs(const_cast<char*>(get_am_string(0,0,statstr[1],statstr_table)),STRING(pPlayer->pev->netname))); 
  }
  return 1;
}




static cell noclip(AMX *amx, cell *params) {
  cell* cStr;    
  int len;
  int iMoveType;
  int iNumParams = params[0]/sizeof(cell);
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) return 0;
  
  CBaseEntity* pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( GETPLAYERWONID(pPlayer->edict()) == 0 ) {
      System_Response("Cannot noclip this player: client is a bot.\n",pAdminEnt);
      return 0; 
    }  // if 
  }  // if

  iMoveType = (int)params[2];
  if(iMoveType!=0) {
    pPlayer->edict()->v.movetype = MOVETYPE_NOCLIP;
  UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs(const_cast<char*>(get_am_string(0,0,statstr[2],statstr_table)),STRING(pPlayer->pev->netname))); 
  } else {
    pPlayer->edict()->v.movetype = MOVETYPE_WALK;
  UTIL_ClientPrintAll( HUD_PRINTTALK, UTIL_VarArgs(const_cast<char*>(get_am_string(0,0,statstr[3],statstr_table)),STRING(pPlayer->pev->netname))); 
  }

  return 1;
}




static cell spawn(AMX *amx, cell *params) {
  int i;
  int iIdentity;
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  cell* cString;
  char szClassname[BUF_SIZE];
  edict_t *pEnt;
  CBaseEntity *pCheckEntity;
  CBaseEntity *pEntity;
  
  if (iNumParams < 7) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetAddr(amx,params[1],&cString); 
  amx_StrLen(cString,&iLength);
  if (iLength>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(szClassname,cString);  
  
  for(i = 0; i < (int)strlen(szClassname); i++) 
    szClassname[i] = tolower(szClassname[i]);
  
  pCheckEntity = UTIL_FindEntityByClassname(NULL, szClassname);
  if(pCheckEntity==NULL) {
    System_Response( "[ADMIN] spawn: The entity to be spawned was not cached at map load, so cannot be spawned.\n",pAdminEnt);
    return 0;
  }
  pEnt = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));
  if(FNullEnt(pEnt)) {
    System_Response("[ADMIN] spawn: Null entity in spawn.\n",pAdminEnt);
    return 0;
  }
  
  pEntity = (CBaseEntity *)GET_PRIVATE(pEnt);
  pEntity->edict()->v.owner = pAdminEnt;
  pEntity->edict()->v.origin.x = (float)params[2];
  pEntity->edict()->v.origin.y = (float)params[3];
  pEntity->edict()->v.origin.z = (float)params[4];
  pEntity->edict()->v.angles.x = (float)params[5];
  pEntity->edict()->v.angles.y = (float)params[6];
  pEntity->edict()->v.angles.z = (float)params[7];
  GameDispatchSpawn( pEntity->edict() );
  
  iIdentity = AddSpawnEntity(szClassname, pEntity);
  if(iIdentity==0) {
    System_Response( "[ADMIN] spawn: AddSpawnEntity returned 0.\n",pAdminEnt);
    return 0;
  }
  return iIdentity;
}




static cell listspawn(AMX *amx, cell *params) {
  int iLength;
  int iNumParams = params[0] / sizeof(cell);
  cell* cString;
  char szClassname[BUF_SIZE];
  
  szClassname[0] = NULL;
  
  if(iNumParams>=1) {
    amx_GetAddr(amx,params[1],&cString); 
    amx_StrLen(cString,&iLength);
    if (iLength>=BUF_SIZE) {
      amx_RaiseError(amx,AMX_ERR_NATIVE);
      return 0;
    }
  }
  amx_GetString(szClassname,cString);  
  ListSpawnEntities(pAdminEnt,szClassname);
  return 1;
}




static cell movespawn(AMX *amx, cell *params) {
  int iIdentity;
  int iNumParams = params[0] / sizeof(cell);
  spawn_struct* pSpawnEntity;
  CBaseEntity *pEntity;
  
  if (iNumParams < 7) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  iIdentity = (int)params[1];
  pSpawnEntity = FindSpawnEntity(iIdentity);
  if(pSpawnEntity==NULL) {
    System_Response( "[ADMIN] movespawn: Could not find matching spawned entity.\n",pAdminEnt);
    return 0;
  }
  pEntity = pSpawnEntity->pEntity;
  pEntity->edict()->v.origin.x = (float)params[2];
  pEntity->edict()->v.origin.y = (float)params[3];
  pEntity->edict()->v.origin.z = (float)params[4];
  pEntity->edict()->v.angles.x = (float)params[5];
  pEntity->edict()->v.angles.y = (float)params[6];
  pEntity->edict()->v.angles.z = (float)params[7];
  return 1;
}




static cell removespawn(AMX *amx, cell *params) {
  int iIdentity;
  int iNumParams = params[0] / sizeof(cell);
  
  if (iNumParams < 1) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  iIdentity = (int)params[1];
  return (RemoveSpawnEntity(iIdentity) ? 1 : 0);
}



//CEM - 12/27/2000 - System time. Useful for timestamps, more stable than timeleft.
static cell systemtime(AMX *amx, cell *params) {
  return (cell)time(NULL);
}

/************************************************************************************************
 *
 *  FZ ADDED
 *
 *  maptime( whichtime, printconsole = 0 )
 *
 * whichtime == 0 : return the time into the map
 *           == 1 : return the time left on the map
 *
 * printconsole == 0 : don't print the time to the console
 *              == 1 : print the time to the console
 *
 ************************************************************************************************/

static cell maptime(AMX *amx, cell *params) {
  
  int iNumParams = params[0]/sizeof(cell);
  
  if ( iNumParams<2) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }  // if

  float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60; // map timelimit in sec

  // if we should return the time left
  if ( params[1] == 1 ) {
    float timeleft = flTimeLimit - gpGlobals->time;

    if ( params[2] == 1 ) {
      int minleft = (int)(timeleft) / 60;
      int secleft = (int)(timeleft) % 60;
      System_Response(UTIL_VarArgs( "timeleft on map: %d:%d\n",minleft, secleft),pAdminEnt);
    }  // if

    return (cell)(timeleft);
  }  else {
    // we return the map time

    if ( params[2] == 1 ) {
      int min = (int)(gpGlobals->time / 60);
      int sec = (int)(gpGlobals->time) % 60;
      System_Response(UTIL_VarArgs( "time into map: %d:%d\n",min, sec),pAdminEnt);
    }  // if

    return (cell)(gpGlobals->time);
  }  // if-else
    
}  // maptime()





/* FZ ADDED 31.12.2000  
 * servertime(timestring, maxlen, format). 
 * Even better, as it also returns the time in a formated string for printing.
 */

static cell servertime(AMX *amx, cell *params) {
  cell* cStr; 
  int iFormatFound = 0;
  int len;
  int iNumParams = params[0]/sizeof(cell);
  char acFormatString[BUF_SIZE];
  char* pcTimeString = 0;

  time_t myTime = time(NULL);
  struct tm *tmMyTime = localtime( &myTime );
  myTime = mktime( tmMyTime );

  // just return time in seconds if no parameters are given
  if (iNumParams  < 1) {
    return (cell)myTime;
  }  // if


  // raise an error if we have not enough parameters
  if (iNumParams < 3) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  int maxlen = params[2];

  // get the format string
  amx_GetAddr(amx,params[3],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }

  amx_GetString(acFormatString,cStr);

  // get the address for the return string
  amx_GetAddr(amx,params[1],&cStr);


  //-- if the format string is omitted (default "none" used) use asctime
  if ( strcmp(acFormatString, "none") == 0 ) {
    iFormatFound = 0;
    pcTimeString = asctime( tmMyTime );
  } else {
    iFormatFound = 1;
    //-- else use strftime to format the output string
    pcTimeString = new char[maxlen];
    for ( int i = 0; i < maxlen; i++) pcTimeString[i] = 0;
    strftime( pcTimeString, maxlen, acFormatString, tmMyTime );
  }  // if-else

  if(maxlen < (int)strlen(pcTimeString)) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }  // if

  // chomp the newline
  char* nl = strrchr( pcTimeString, '\n' );
  if ( nl != 0 ) {
    *nl = '\0';
  }  // if

  amx_SetString(cStr,pcTimeString,0);

  if ( iFormatFound ) {
    delete pcTimeString;
    pcTimeString = 0;
  }  // if

  return (cell) myTime;

}  // servertime()
/* /FZ */




static cell consgreet(AMX *amx, cell *params) {

  cell *cellstr = 0;
  int len = 0;
  int iNumParams = params[0] / sizeof(cell);
  char text[LARGE_BUF_SIZE];
  

  if ( iNumParams < 1 ) {
    return 0;
  }

  amx_GetAddr( amx, params[1], &cellstr );
  amx_StrLen( cellstr, &len );
  if ( len > LARGE_BUF_SIZE ) {
    return 0;
  }

  amx_GetString( text, cellstr );
  char* pcText = text;

  char* extension = ".txt";  // this could also be read from a cvar
  char* pcExtension = strstr( pcText, extension );  // locate extension in string

  int iMsgLength = 0;          // length of the message in a file
  BOOL bReadFromFile = FALSE;  

  char* pcMsgFile = 0;        // the message read from a file
  char strGameDir[2048];  
  (*g_engfuncs.pfnGetGameDir)(strGameDir); 

  // check if the string provided ends with ".txt"
  if ( pcExtension != 0 && *(pcExtension + strlen(extension)) == '\0' ) {
    // skip spaces at the beginning
    while ( *pcText == ' ' || *pcText == '\t' ) pcText++;

#ifdef LINUX
    //-- get the full pathname
    char* pcFilePath = 0;
    int iPathLen = 0;
    if  ((strlen( pcText )+ 2 + strlen( strGameDir )) < 2048 ) {
      //-- the filename will still fit into the gamedir string
      pcFilePath = strGameDir + strlen( strGameDir );
      *pcFilePath = '/';
      pcFilePath++;
      strcpy( pcFilePath, pcText );
      pcFilePath = strGameDir;
    } else {
      //-- allocate a new string for the full path
      iPathLen = strlen( strGameDir ) + 2 + strlen( pcText );
      pcFilePath = new char[iPathLen];
      if ( pcFilePath == 0 ) return 0;   // always wear security helmets
      for ( int i = 0; i < iPathLen; i++ ) pcFilePath[i] = 0;

      strcat( pcFilePath, strGameDir );
      strcat( pcFilePath, "/" );
      strcat( pcFilePath, pcText );
    }  // if-else

    //-- see if it is a file
    struct stat file;
    int retval = stat( pcFilePath, &file );
    if ( iPathLen ) {
      delete[] pcFilePath;
      pcFilePath = 0;
    }  // if
    
    if ( retval >= 0 ) {
      // if the file is empty we don't print anything
      if ( file.st_size <= 1 ) {
	return 1;  // this may be improved later for reading from pipes
      }  // if
      // read the file to pcMsgFile if it is readable
      if ( (file.st_mode & S_IRUSR) == S_IRUSR ) {
	bReadFromFile = TRUE;
	pcMsgFile = (char*)LOAD_FILE_FOR_ME( pcText, &iMsgLength ) ;
      }  // if
    } else {
    }  // if-else
#else
    pcMsgFile = (char*)LOAD_FILE_FOR_ME( pcText, &iMsgLength );
    if ( pcMsgFile != 0 ) {
      if ( iMsgLength <= 1 ) {
	return 1;
      } else {
	bReadFromFile = TRUE;
      }  // if-else
    }  // if
#endif
  } else {

    // print the text to the console
    System_Response(UTIL_VarArgs( "%s\n", pcText),pAdminEnt);
  }  // if-else

  
  if ( bReadFromFile == TRUE ) {
    // safety first
    if ( pcMsgFile == 0 ) {
      FREE_FILE( pcMsgFile ) ;
      pcMsgFile = 0;
      return 0;
    }  // if
 
    // delete \r from the text
    char* pC = strchr( pcMsgFile, '\r' );
    while ( pC != 0 ) {
      *pC = ' ';
      pC = strchr( pC, '\r' );
    }  // while
    
    // print the file content to the console
    System_Response( pcMsgFile,pAdminEnt);
  }  // if

  FREE_FILE( pcMsgFile ) ;
  pcMsgFile = 0;

  return 1;
}  // consgreet




static cell rainbow(AMX *amx, cell *params)
{
  /*
   *   params[1] = string to say
   *   params[2] = time to say it for
   *   params[3,4,5] = r,g,b colour
   *   params[6,7,8] = r,g,b colour
   */
  cell result = 1;
  int len;
  char *iszItem;
  char buf[CENTER_SAY_SIZE];
  cell* cStr;    
  hudtextparms_t m_textParms;
  
  entvars_t *pev = &pAdminEnt->v;
  amx_GetAddr(amx,params[1],&cStr);
  amx_StrLen(cStr,&len);
  if (len>=CENTER_SAY_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  } /* if */     
  amx_GetString(buf,cStr);    
  iszItem=buf;
  
  if ( iszItem==NULL ) {
    System_Response( "You must say something\n", pAdminEnt );
    return result;
  }    
  
  m_textParms.r1 = params[6];
  m_textParms.g1 = params[7];
  m_textParms.b1 = params[8];
  m_textParms.a1 = 0;
  m_textParms.r2 = params[3];
  m_textParms.g2 = params[4];
  m_textParms.b2 = params[5];
  m_textParms.a2 = 0;
  
  m_textParms.fadeinTime = 0;
  m_textParms.fadeoutTime = 0;
  
  m_textParms.holdTime = params[2];   
  m_textParms.fxTime = params[2];
  
  m_textParms.x = -1;
  m_textParms.y = 0.25;
  
  m_textParms.effect=2;
  m_textParms.channel=1;
  
  UTIL_HudMessageAll( m_textParms,iszItem );
  UTIL_LogPrintfFNL( "[ADMIN:%s] %s\n", STRING(pev->netname),iszItem);
  
  return result;
}



static cell look_in_dir(AMX *amx, cell *params) {
#ifndef WIN32
  return 0;
#else
  
  cell *cptr;
  cell *dir;    
  cell *fileselection;
  
  
  int len_dir;
  int len_selection;
  int iNumParams = params[0]/sizeof(cell);
  
  if (iNumParams < 4) { // wrong #of arguments
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  int PlayerIndex = 0;
  
  amx_GetAddr(amx,params[1],&dir); // get the dir where to look in
  amx_StrLen(dir,&len_dir);
  
  amx_GetAddr(amx,params[2],&fileselection); // get the file selection criteria
  amx_StrLen(fileselection,&len_selection);
  
  amx_GetAddr(amx,params[3],&cptr);
  
  if (len_dir >= BUF_SIZE || len_selection >= BUF_SIZE) {
    amx_SetString(cptr, "", 0);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  char array_dir[BUF_SIZE];
  amx_GetString(array_dir,dir); // getting stringname in array
  
  char array_selection[BUF_SIZE];
  amx_GetString(array_selection, fileselection); // getting fileselection criteria in array
  
  int map_place = (int)params[4]; 
  
  char game_dir[200];
  int i=0;
  
  (*g_engfuncs.pfnGetGameDir)(game_dir); // ==> c:\\sierra\\half-life
  
#ifdef WIN32
  strcat(game_dir,"\\");
#else
  strcat(game_dir,"/");
#endif
  
  strcat(game_dir,array_dir); // ==> c:\\sierra\\half-life\\cstrike\\maps
  
#ifdef WIN32
  strcat(game_dir,"\\");
#else
  strcat(game_dir,"/");
#endif
  
  strcat(game_dir, array_selection);
  
  struct _finddata_t filestruct;
  long hnd;
  //        char buffer[_MAX_PATH];
  
  hnd = _findfirst(game_dir,&filestruct);
  
  if((hnd == -1)) {
    amx_SetString(cptr, "", 0);
    return 0;   // 0 -> not found
  }
  
  for (i=0; i<map_place; i++) {
    if(_findnext(hnd,&filestruct) != 0) { // result = 0 => successfull
      amx_SetString(cptr, "", 0);
      return 0;
    }
  }
  
  amx_SetString(cptr, filestruct.name, 0);
  _findclose(hnd);
  return 1; 
#endif
}

/* CEM - 02/06/01 - Karel's glowing and gettarget functions */
static cell glow(AMX *amx, cell *params) {
  cell* cStr;    
  int iBlue;
  int iGreen;
  int iLength;
  int iNumParams = params[0]/sizeof(cell);
  int iRed;
  int PlayerIndex = 0;
  char PlayerText[BUF_SIZE];
  
  if (iNumParams < 4) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetAddr(amx,params[1],&cStr); 
  amx_StrLen(cStr,&iLength);
  if (iLength <= 0 || iLength >= BUF_SIZE) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  amx_GetString(PlayerText,cStr);  
  
  PlayerIndex = GetPlayerIndex(PlayerText);
  if(PlayerIndex==0) 
    return 0;
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(PlayerIndex);
  if( !IsPlayerValid(pPlayer) ) return 0;

  iRed = (int)params[2];
  iGreen = (int)params[3];
  iBlue = (int)params[4];
  
  if (iRed < 0)
    iRed = 0;
  if (iRed > 255)
    iRed = 255;
  if (iGreen < 0)
    iGreen = 0;
  if (iGreen > 255)
    iGreen = 255;
  if (iBlue < 0)
    iBlue = 0;
  if (iBlue > 255)
    iBlue = 255;
  
  if(iRed != 0 || iGreen != 0 || iBlue != 0) {
    pPlayer->pev->rendermode = kRenderNormal;
    pPlayer->pev->renderfx = kRenderFxGlowShell;
    
    pPlayer->pev->renderamt = 25;  // glow shell distance from entity
    pPlayer->pev->rendercolor.x = iRed;
    pPlayer->pev->rendercolor.y = iGreen;
    pPlayer->pev->rendercolor.z = iBlue;
  } else {
    pPlayer->pev->rendermode = kRenderNormal;
    pPlayer->pev->renderfx = kRenderFxNone;
    pPlayer->pev->renderamt = 0;
  }
  return 1;
}
/* /CEM */




static cell gettarget(AMX *amx, cell *params) {
  /*
   * param 1: username to check target for 
   * param 2: returned target name
   * param 3: maxlen of target name array
   * param 4: max # range to look at, default 1024
   */
  cell *cpUserName;
  int len;
  cell *cpReturnedUser;
  
  int iNumParams = params[0]/sizeof(cell);
  
  if (iNumParams < 4) { // wrong #of arguments
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  amx_GetAddr(amx,params[1],&cpUserName); // get the dir where to look in
  amx_StrLen(cpUserName,&len);
  
  if (len >= BUF_SIZE) {
    //amx_SetString(cpUserName, "", 0);
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  CBaseEntity *pPlayer = 0;
  int PlayerIndex = 0;
 
  if ( len > 0 ) {
    char speler[BUF_SIZE];
    amx_GetString(speler,cpUserName); // getting stringname in array
    
    PlayerIndex = GetPlayerIndex(speler);
    if ( PlayerIndex == 0 )
      DEBUG_LOG( 1, ("Could not find user '%s'.", speler) );
      return 0;
    
  } else {
    if ( pAdminEnt == 0 ) {
      UTIL_LogPrintf("[ADMIN] gettarget: You cannot use this function from the server console.\n");
      return 0;
    } // if
    
    PlayerIndex = ENTINDEX( pAdminEnt );
    if (PlayerIndex < 1) {
      return 0;
    }  // if
  
  }  // if-else

  pPlayer = UTIL_PlayerByIndex(PlayerIndex); 
  if ( pPlayer == NULL ) 
    return 0; 

  TraceResult tr;
  MAKE_VECTORS( pPlayer->pev->v_angle + pPlayer->pev->punchangle );
  
  Vector vecSrc = pPlayer->pev->origin + pPlayer->pev->view_ofs;
  Vector vecEnd = vecSrc + (gpGlobals->v_forward * (int)params[4]);
  TRACE_LINE( vecSrc, vecEnd, dont_ignore_monsters, pPlayer->edict(), &tr);
  
  if (tr.flFraction != 1.0 && !FNullEnt( tr.pHit ) ) {
    CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );
    
    if ( params[3] > 0 ) {
      if ( params[3] <= (int)strlen(STRING(pEntity->pev->netname)) ) {
	   amx_GetAddr(amx,params[2],&cpReturnedUser);
	   amx_SetString(cpReturnedUser,const_cast<char*>(STRING(pEntity->pev->netname)),0);
      }  else {
	   amx_GetAddr(amx,params[2],&cpReturnedUser);
	   amx_SetString(cpReturnedUser,"",0);
	   amx_RaiseError( amx, AMX_ERR_NATIVE );
	   return 0;
      }  // if-else
    }  // if

  return GETPLAYERUSERID(pEntity->edict());
  }  // if

  if ( params[3] > 0 ) {
    amx_GetAddr(amx,params[2],&cpReturnedUser);
    amx_SetString(cpReturnedUser,"",0);
    amx_RaiseError( amx, AMX_ERR_NATIVE );
  }  // if
  return 0;
}





static cell pointto(AMX *amx, cell *params) { 
  int iIndex = 0;
  int iNumParams = params[0]/sizeof(cell); 
  
  if ( pAdminEnt == 0 ) {
    UTIL_LogPrintf("[ADMIN] pointto: You cannot use this function from the server console.\n");
    return 0;
  } // if

  iIndex = ENTINDEX( pAdminEnt );
  if (iNumParams < 1) { 
    amx_RaiseError(amx,AMX_ERR_NATIVE); 
    return 0; 
  } else if (iIndex < 1) {
    amx_RaiseError(amx,AMX_ERR_NATIVE);
    return 0;
  }
  
  CBaseEntity *pPlayer = UTIL_PlayerByIndex(iIndex); 
  if ( pPlayer == NULL ) 
    return 0; 
  
  TraceResult tr; 
  MAKE_VECTORS( pPlayer->pev->v_angle + pPlayer->pev->punchangle ); 
  

  Vector vecSrc = pPlayer->pev->origin + pPlayer->pev->view_ofs;

  Vector vecEnd = vecSrc + (gpGlobals->v_forward * (int)params[1]); 

  TRACE_LINE( vecSrc, vecEnd, dont_ignore_monsters, pPlayer->edict(), &tr); 
  

  if (tr.flFraction != 1.0 && !FNullEnt( tr.pHit ) ) { 
    CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit ); 
    return GETPLAYERUSERID(pEntity->edict());
  } else {
    return 0;
  }
} 



AMX_NATIVE_INFO admin_Natives[] = {
  { "access", access},
  { "auth",auth},
  { "ban",ban},
  { "censor_words",censor_words},
  { "centersay",centersay},
  { "changelevel", changelevel},
  { "check_words",check_words},
  { "check_user",check_user_amx},
  { "consgreet",consgreet},
  { "convert_string", convert_string},
  { "currentmap",currentmap},
  { "deletefile",deletefile},
  { "exec", exec},
  { "execclient",execclient},
  { "fileexists",fileexists},
  { "filesize",filesize},
  { "getstrvar",getstrvar},
  { "gettarget", gettarget},
  { "getteamcount",getteamcount},
  { "getvar",getvar},
  { "get_serverinfo",get_serverinfo},
  { "get_userindex",get_userindex},
  { "get_userinfo",get_userinfo},
  { "get_username",get_username},
  { "get_userorigin",get_userorigin},
  { "get_userSessionID",get_userSessionID},
  { "get_userWONID",get_userWONID},
  { "get_vaultdata",get_vaultdata},
  { "glow", glow}, 
  { "godmode", godmode},
  { "help",help},
  { "kick", kick},
  { "kill_timer",kill_timer},
  { "listspawn", listspawn},
  { "list_maps",list_maps},
  { "log",log},
  { "look_in_dir",look_in_dir},
  { "maptime",maptime},
  { "maxplayercount",maxplayercount},
  { "message", message},
  { "messageex", messageex},
  { "movespawn", movespawn},
  { "nextmap", nextmap},
  { "noclip", noclip},
  { "playercount",playercount},
  { "playerinfo", playerinfo},
  { "playsound",playsound},
  { "plugin_message", plugin_message },
  { "plugin_exec", plugin_exec },
  { "plugin_registercmd", plugin_registercmd},
  { "plugin_registerhelp", plugin_registerhelp},
  { "plugin_registerinfo", plugin_registerinfo},
  { "pointto", pointto },
  { "rainbow",rainbow},
  { "readfile",readfile},
  { "reload",reload},
  { "removespawn", removespawn},
  { "resetfile",resetfile},
  { "say",  say },
  { "selfmessage", selfmessage},
  { "servertime",servertime },
  { "set_serverinfo",set_serverinfo},
  { "set_timer", set_timer},
  { "set_vaultdata",set_vaultdata},
  { "slap",slap},
  { "slay",slay},
  { "spawn", spawn},
  { "strtonum",strtonum},
  { "systemtime",systemtime},
  { "teleport",teleport},
  { "timeleft", timeleft},
  { "typesay", typesay},
  { "unban",unban},
  { "userlist", userlist},
  { "valid_map", valid_map},
  { "version",version},
  { "vote", vote_multiple},
  { "vote_allowed",vote_allowed},
  { "writefile",writefile},
  // Terminator
  { 0, 0 }
};



/*******************************************************************************************
 *
 * I added the CVS log here to see if this helps us. 
 * For now it's just a test. Can be removed if we don't like it.
 *
 * $Log: admin_commands.cpp,v $
 * Revision 1.37  2001/08/28 08:09:14  alfred
 * talksay() string size check added
 * slight change to default tag sql statement
 *
 * Revision 1.36  2001/06/17 23:20:27  darope
 * Enabled playerinfo with bots again.
 *
 * Revision 1.35  2001/06/12 23:55:46  darope
 * -- I broke the fx in teleport() when adding bot_protection. That has been
 *    corrected.
 * -- Added the plugin_exec() function to call commands from other plugins.
 *
 * Revision 1.34  2001/06/10 12:57:36  darope
 * Added IsPlayerValid() checks after each UTIL_PlayerByIndex() call to
 * protect against referencing Null pointers. This also fixes a bug crashing
 * the server in playerinfo() when cycling through player indices.
 *
 * Revision 1.33  2001/06/04 13:28:13  darope
 * Using DEBUG_LOG in timer.cpp. Fixed debug messages in admin_command.cpp
 *
 * Revision 1.32  2001/06/03 23:03:24  darope
 * Fixed an error in the static table
 *
 * Revision 1.31  2001/06/03 21:27:36  darope
 * -- Debug messages now use the DEBUG_LOG macro
 * -- writefile() now creates a new file also if it isn't called with the
 *    append parameter (-1).
 *
 * Revision 1.30  2001/06/03 18:37:47  darope
 * -- Extended bot protection to more commands, like message() and messageex().
 * -- Replaced a few strings to use the compressed string table
 * -- Added a check in execclient() to make sure the command is valid.
 *
 * Revision 1.29  2001/06/03 04:07:28  darope
 * admin_mod.cpp
 *
 * Revision 1.28  2001/06/02 18:13:42  darope
 * -- Added a check for names with non-printable characters that would crash
 *    other clients. Added to ClientConnect() and ClientUserInfoChanged().
 *    In the second case the user will be banned for 24h if the cvar
 *    amv_autoban is set to 1
 * -- Added cvar admin_repeat_freq to control the frequency of admin_repeat_msg
 * -- Added util function UTIL_LogPrintfFNL(). It will print to the log but
 *    replace newlines within the string with \n or, if the string is too
 *    long for that, with a space.
 *
 * Revision 1.27  2001/05/28 10:32:16  darope
 * Used const_casts where necessary in order to use the standard STRING
 * macro from the SDK instead of redefining it non-const. This is better anyway.
 *
 * Revision 1.26  2001/05/28 03:54:38  darope
 * -- Adapted to new template classes.
 * -- Added slay() function to be independant from allow_client_exec
 * -- Modified teleport to work with bots even if admin_fx is turned on
 *
 * Revision 1.25  2001/05/27 16:30:51  darope
 * Ooops. :) Don't you love those fixes fixing fixes?
 *
 * Revision 1.24  2001/05/24 22:37:43  darope
 * Redone the vote_multiple fix to prevent buffor overflows
 *
 * Revision 1.23  2001/05/13 18:13:58  darope
 * - Fixed the crashes of pointto() under Linux. CS was being arsey again,
 *   crashing on CBaseEntity::EyePosition().
 * - Replaced a CLIENT_PRINTF with a System_Response.
 *
 * Revision 1.22  2001/05/11 02:58:17  darope
 * = admin_commands.cpp:
 * - fixed debug info in plugin_registercmd and plugin_registerhelp
 *
 * = CLinkedList.cpp:
 * - fixed bug that would not allow insertion before first element.
 *   untested, but Mr. Warlock is very serious about this.
 *
 * Revision 1.21  2001/05/10 01:42:43  magua
 * getteamcount() fixed. --Jaguar
 *
 * Revision 1.20  2001/04/29 09:46:55  magua
 * Added in admin_parse_logs cvar. --Jaguar
 *
 * Revision 1.15  2001/04/10 21:23:39  darope
 * -- Fixed a bug in ban() that would use arbitrary values for the
 *    ban time.
 * -- Fixed a debig message for gettarget().
 *
 * Revision 1.14  2001/04/04 17:02:42  magua
 * Expanded size of log() from BUF_SIZE to LARGE_BUF_SIZE --Jaguar
 *
 * Revision 1.13  2001/03/31 20:36:17  darope
 * -- say(), messageex() and tsay() respect "^n" and "\n"
 * -- Buffer size increased for say(), message(), tsay(), and messageex()
 *
 * Revision 1.12  2001/03/30 13:21:21  darope
 * -- Added checking for bots by WONID 0 in functions execclient(), slap()
 *    and playsound().
 *
 * -- Swapped pointto() and gettarget() and improved behaviour (or so I hope)
 *
 * Revision 1.11  2001/03/28 10:36:22  darope
 * -- noclip and godmode print messages according to their mode of operation
 *
 * -- file functions return 0 on error or failure. writefile() lineendings
 *    corrected. readfile() strips '\r' in addition to '\n'. fileexists()
 *    does not need file_access_read 1 for operation
 *
 * -- consgreet doesn't print the name of the file if it thinks it should
 *    print a file but doesn't find the file.
 *
 * -- playerinfo() returns the Team for CS.
 *
 * -- getteamcount() returns the Team Count for CS. Either the team is
 *    looked up for each player or the CS internal function count function
 *    is used
 *
 *
 ********************************************************************************************/
