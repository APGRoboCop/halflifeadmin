/*
 * ===========================================================================
 *
 * $Id: dll.cpp,v 1.12 2006/01/03 21:05:26 darope Exp $
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
 * Stubs for the various dll functions
 *
 */   

#include "version.h"
#include "amutil.h"
#include "extdll.h"
#include "users.h"
#define SDK_UTIL_H	// extdll.h already #include's util.h
#ifdef USE_METAMOD
#  include "meta_api.h"
#endif

static constexpr char vstring[] = "\n$Pg: Admin Mod -- " MOD_VERSION " | " __DATE__ " - " __TIME__ " $\n";

extern AMXINIT amx_Init;
extern AMXREGISTER amx_Register;
extern AMXFINDPUBLIC amx_FindPublic;
extern AMXEXEC amx_Exec;
extern AMXGETADDR amx_GetAddr;
extern AMXSTRLEN amx_StrLen;
extern AMXRAISEERROR amx_RaiseError;
extern AMXSETSTRING amx_SetString;
extern AMXGETSTRING amx_GetString;

#ifdef _WIN32
extern HINSTANCE h_AMX;
#else
extern void *h_AMX;
#include <dlfcn.h>
#define GetProcAddress dlsym    
#endif

#ifdef _WIN32
  typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
  typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
# define EXPORT  _declspec( dllexport )
#else
  typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int); 
  typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
# define EXPORT
#endif

#ifndef USE_METAMOD
  extern GETENTITYAPI other_GetEntityAPI;
  extern GETNEWDLLFUNCTIONS other_GetNewDLLFunctions;
  static DLL_FUNCTIONS other_gFunctionTable;
  static NEW_DLL_FUNCTIONS other_gNewFunctionTable;
#endif

extern DLL_GLOBAL edict_t *pTimerEnt;
extern DLL_GLOBAL enginefuncs_t my_engfuncs;
extern DLL_GLOBAL enginefuncs_t my_pre_engfuncs;


#ifdef USE_METAMOD

/*********** Metamod stuff ***********/
// Global vars from metamod.  These variable names are referenced by
// various macros.
meta_globals_t *gpMetaGlobals;		// metamod globals
gamedll_funcs_t *gpGamedllFuncs;	// gameDLL function tables
mutil_funcs_t *gpMetaUtilFuncs;		// metamod utility functions

// Description of plugin
plugin_info_t Plugin_info = {
  META_INTERFACE_VERSION,	// ifvers
  "Adminmod",	// name
  MOD_VERSION,	// version
  VDATE,	// date
  VAUTHOR,	// author
  VURL,	        // url
  "ADMIN",	// logtag
  PT_STARTUP,	// (when) loadable
  PT_ANYTIME	// (when) unloadable
};

// Must provide at least one of these..
static META_FUNCTIONS gMetaFunctionTable = {
  GetEntityAPI,		// pfnGetEntityAPI		HL SDK; called before game DLL
  GetEntityAPI_Post,  // pfnGetEntityAPI_Post		META; called after game DLL
  nullptr,			// pfnGetEntityAPI2		HL SDK2; called before game DLL
  nullptr,			// pfnGetEntityAPI2_Post	META; called after game DLL
  GetNewDLLFunctions, // pfnGetNewDLLFunctions	HL SDK2; called before game DLL
  nullptr,			// pfnGetNewDLLFunctions_Post	META; called after game DLL
  nullptr,			// pfnGetEngineFunctions	META; called before HL engine
  GetEngineFunctions_Post, // pfnGetEngineFunctions_Post META; called after HL engine
};

// Metamod attaching plugin to the server.
//  now		(given) current phase, ie during map, during changelevel, or at startup
//  pFunctionTable	(requested) table of function tables this plugin catches
//  pMGlobals	(given) global vars from metamod
//  pGamedllFuncs	(given) copy of function tables from game dll
C_DLLEXPORT int Meta_Attach(PLUG_LOADTIME now, META_FUNCTIONS *pFunctionTable, 
		meta_globals_t *pMGlobals, gamedll_funcs_t *pGamedllFuncs)
{
  if(now > Plugin_info.loadable) {
    LOG_ERROR(PLID, "Can't load plugin right now");
    return(FALSE);
  }
  if(!pMGlobals) {
    LOG_ERROR(PLID, "Meta_Attach called with null pMGlobals");
    return(FALSE);
  }
  gpMetaGlobals=pMGlobals;
  if(!pFunctionTable) {
    LOG_ERROR(PLID, "Meta_Attach called with null pFunctionTable");
    return(FALSE);
  }
  memcpy(pFunctionTable, &gMetaFunctionTable, sizeof(META_FUNCTIONS));
  gpGamedllFuncs = pGamedllFuncs;


  //-- load the AMX engine
  char script_dll[2048];
  const char* pcPlPath = GET_PLUGIN_PATH( PLID );


  memset( script_dll, 0, 2048 );
  snprintf( script_dll, sizeof(script_dll)-strlen(SCRIPT_DLL)-1,"%s", pcPlPath );

  char* pcTmp = strrchr(script_dll, '/');
  if ( pcTmp == nullptr ) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (%s): incorrect engine path.\n",script_dll );
	am_exit(1);
  } else {
	  pcTmp++;
  }  // if
  sprintf( pcTmp, "%s", SCRIPT_DLL );

  LOG_MESSAGE( PLID, "Loading scripting engine %s.", script_dll );


#  ifdef _WIN32
  h_AMX = LoadLibrary(script_dll);
#  else
  h_AMX = dlopen(script_dll, RTLD_NOW); 
#  endif


  if ( h_AMX == nullptr) {
#  ifdef WIN32
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (%s) Error: %i\n",script_dll, GetLastError());
#  else
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (%s) %s\n",script_dll,dlerror());
#  endif    
    am_exit(1);
  }
  
  amx_Init= (AMXINIT)GetProcAddress(h_AMX, "amx_Init");
  if (amx_Init == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_Init)\n");
    am_exit(1);
  }
  
  amx_Register= (AMXREGISTER)GetProcAddress(h_AMX, "amx_Register");
  if (amx_Register == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_Register)\n");
    am_exit(1);
  }
  
  amx_FindPublic= (AMXFINDPUBLIC)GetProcAddress(h_AMX, "amx_FindPublic");
  if (amx_FindPublic == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_FindPublic)\n");
    am_exit(1);
  }
  
  amx_Exec= (AMXEXEC)GetProcAddress(h_AMX, "amx_Exec");
  if (amx_Exec == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_Exec)\n");
    am_exit(1);
  }
  
  amx_GetAddr= (AMXGETADDR)GetProcAddress(h_AMX, "amx_GetAddr");
  if (amx_GetAddr == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_GetAddr)\n");
    am_exit(1);
  }
  
  amx_StrLen= (AMXSTRLEN)GetProcAddress(h_AMX, "amx_StrLen");
  if (amx_StrLen == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_StrLen)\n");
    am_exit(1);
  }
  
  amx_RaiseError= (AMXRAISEERROR)GetProcAddress(h_AMX, "amx_RaiseError");
  if (amx_RaiseError == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_RaiseError)\n");
    am_exit(1);
  }
  
  amx_SetString= (AMXSETSTRING)GetProcAddress(h_AMX, "amx_SetString");
  if (amx_SetString == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_SetString)\n");
    am_exit(1);
  }
  
  amx_GetString= (AMXGETSTRING)GetProcAddress(h_AMX, "amx_GetString");
  if (amx_GetString == nullptr ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_GetString)\n");
    am_exit(1);
  }


#ifdef EXTMM
  const gamedll_t* pGameDLL = (gamedll_t*)GET_GAME_DLL();
  if ( strcmp(pGameDLL->name, "cstrike") == 0 ) {
      UTIL_LogPrintf( "[ADMIN] INFO: Loading extended Counter-Strike functionality.\n");

    if (pGameDLL->handle == nullptr) {
      UTIL_LogPrintf( "[ADMIN] INFO: Unable to open Counter-Strike DLL for extended functionality.\n");
    } else {

      g_pflTimeLimit = (float*)GetProcAddress( pGameDLL->handle, "g_flTimeLimit" );
      if ( g_pflTimeLimit == nullptr ) {
        UTIL_LogPrintf( "[ADMIN] INFO: Could not find CS' TimeLimit. AM's timeleft may differ from CS' timeleft.\n");
      } else {
      }  // if-else

    }  // if-else
  } // if
#else
  // Disabled since this overwrites the setting in h_export.cpp
  // g_pflTimeLimit = nullptr;
#endif // EXTMM



  return(TRUE);
}

// Metamod detaching plugin from the server.
// now		(given) current phase, ie during map, etc
// reason	          (given) why detaching (refresh, console unload, forced unload, etc)
C_DLLEXPORT int Meta_Detach(PLUG_LOADTIME now, PL_UNLOAD_REASON reason) {
  if(now > Plugin_info.unloadable && reason != PNL_CMD_FORCED) {
    LOG_ERROR(PLID, "[%s] ERROR: Can't unload plugin right now");
    return(FALSE);
  }

  AM_Stop();

  return(TRUE);
}

// Metamod requesting info about this plugin
//  ifvers		(given) interface_version metamod is using
//  pPlugInfo		(requested) struct with info about plugin
//  pMetaUtilFuncs	(given) table of utility functions provided by metamod
C_DLLEXPORT int Meta_Query(char *ifvers, plugin_info_t **pPlugInfo,
		mutil_funcs_t *pMetaUtilFuncs) 
{
  if (static_cast<int>(CVAR_GET_FLOAT("developer")) != 0) 
    UTIL_LogPrintf("[%s] DEBUG: called: Meta_Query; version=%s, ours=%s\n", 
		   Plugin_info.logtag, ifvers, Plugin_info.ifvers);

  // Check for valid pMetaUtilFuncs before we continue.
  if(!pMetaUtilFuncs) {
    UTIL_LogPrintf("[%s] ERROR: Meta_Query called with null pMetaUtilFuncs\n", Plugin_info.logtag);
    return(FALSE);
  }
  gpMetaUtilFuncs=pMetaUtilFuncs;
  
  // Give metamod our plugin_info struct.
  *pPlugInfo=&Plugin_info;
  
  if(!FStrEq(ifvers, Plugin_info.ifvers)) {
    int mmajor=0, mminor=0, pmajor=0, pminor=0;
    LOG_MESSAGE(PLID, "INFO: meta-interface version mismatch; Metamod=%s Admin Mod=%s", ifvers, Plugin_info.ifvers);
    sscanf(ifvers, "%d:%d", &mmajor, &mminor);
    sscanf(META_INTERFACE_VERSION, "%d:%d", &pmajor, &pminor);
    // If plugin has later interface version, it's incompatible (update 
    // metamod).
    if(pmajor > mmajor || (pmajor==mmajor && pminor > mminor)) {
      LOG_ERROR(PLID, "Metamod version is too old for plugin %s v.%s; update Metamod", Plugin_info.name, Plugin_info.version);
      return(FALSE);
    }
    // If plugin has older major interface version, it's incompatible (update 
    // plugin).
    else if(pmajor < mmajor) {
      LOG_ERROR(PLID, "Metamod version is incompatible with plugin %s v.%s; please find a newer version of %s", Plugin_info.name, Plugin_info.version, Plugin_info.name);
      return(FALSE);
    }
    // Minor interface is older, but this is guaranteed to be backwards
    // compatible, so we warn, but we still accept it.
    else if(pmajor==mmajor && pminor < mminor)
      LOG_MESSAGE(PLID, "WARNING: Metamod version is newer than expected; consider finding a newer version of plugin %s", Plugin_info.name);
    else
      LOG_ERROR(PLID, "unexpected version comparision; metavers=%s, mmajor=%d, mminor=%d; plugvers=%s, pmajor=%d, pminor=%d", ifvers, mmajor, mminor, META_INTERFACE_VERSION, pmajor, pminor); }
  return(TRUE);
}

C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion ) 
{
  if (static_cast<int>(CVAR_GET_FLOAT("developer")) != 0)
    UTIL_LogPrintf("[%s] DEBUG: called: GetEngineFunctions_Post; version=%d\n", Plugin_info.logtag, *interfaceVersion);
  if(!pengfuncsFromEngine) {
    UTIL_LogPrintf("[%s] ERROR: GetEngineFunctions_Post called with null pengfuncsFromEngine\n", Plugin_info.logtag);
    return(FALSE);
  }
  else if(*interfaceVersion != ENGINE_INTERFACE_VERSION) {
    UTIL_LogPrintf("[%s] WARNING: GetEngineFunctions_Post version mismatch; requested=%d ours=%d\n", Plugin_info.logtag, *interfaceVersion, ENGINE_INTERFACE_VERSION);
    // Tell metamod what version we had, so it can figure out who is out of date.
    *interfaceVersion = ENGINE_INTERFACE_VERSION;
    return(FALSE);
  }
  memcpy(pengfuncsFromEngine, &my_engfuncs, sizeof(enginefuncs_t));
  return TRUE;
}

/*********** /Metamod stuff ***********/

void SetMetaResult(int iResult) {
  if (iResult == RESULT_CONTINUE) {
    SET_META_RESULT(MRES_HANDLED);
  } else {
    SET_META_RESULT(MRES_SUPERCEDE);
  }
}
#endif


void DispatchObjectCollsionBox( edict_t *pent ) {

  if(pent==pTimerEnt) {
    if (static_cast<int>(CVAR_GET_FLOAT("admin_debug")) >= 4)
      UTIL_LogPrintf("[ADMIN] DEBUG: Hiding timer entity from DispatchObjectCollsionBox\n");

#ifdef USE_METAMOD
   RETURN_META(MRES_SUPERCEDE);
#else
   return;
#endif
  }
#ifdef USE_METAMOD
  RETURN_META(MRES_IGNORED);
#else
  other_gFunctionTable.pfnSetAbsBox(pent);
#endif


}  

// Possible crash cause for 3rd Party HL Mods for 2.50.61 linux? [APG]RoboCop[CL]
void ClientCommand( edict_t *pEntity ) {
	const int iResult = AM_ClientCommand(pEntity);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnClientCommand(pEntity);
#endif
}

BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ) {
	const int iResult = AM_ClientConnect(pEntity,pszName,pszAddress,szRejectReason, false);
#ifdef USE_METAMOD
  if (iResult == TRUE) {
    RETURN_META_VALUE(MRES_HANDLED,TRUE);
  } else {
    RETURN_META_VALUE(MRES_SUPERCEDE,FALSE);
  }
#else
  if (iResult == TRUE) {
    return(other_gFunctionTable.pfnClientConnect(pEntity, pszName, pszAddress, szRejectReason, false));
  } else {
    return FALSE;
  }
#endif
}

#ifdef USE_METAMOD
BOOL ClientConnect_Post( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ) {
	const BOOL ret=META_RESULT_ORIG_RET( BOOL );
	const BOOL iResult = AM_ClientConnect_Post( pEntity, pszName, pszAddress, szRejectReason );
  if (iResult == ret) {
    RETURN_META_VALUE(MRES_IGNORED,ret);
  } else {
    RETURN_META_VALUE(MRES_OVERRIDE,iResult);
  }
}
#endif

void ClientDisconnect( edict_t *pEntity ) {
	const int iResult = AM_ClientDisconnect(pEntity);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnClientDisconnect(pEntity);
#endif
}

void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer ) {
	const int iResult = AM_ClientUserInfoChanged(pEntity,infobuffer, false);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnClientUserInfoChanged(pEntity, infobuffer);
#endif
}


void DispatchThink( edict_t *pent ) {
	const int iResult = AM_DispatchThink(pent);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE) 
    other_gFunctionTable.pfnThink(pent);
#endif
}

void GameDLLInit() {
	const int iResult = AM_GameDLLInit();
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE) 
    other_gFunctionTable.pfnGameInit();
#endif
}

void OnFreeEntPrivateData( edict_t *pEnt ) {
	const int iResult = AM_OnFreeEntPrivateData(pEnt);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE) 
    other_gNewFunctionTable.pfnOnFreeEntPrivateData(pEnt);
#endif
}

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ) {
	const int iResult = AM_Initialize();

  //TODO: What would be even better, would be to store the first map of the mapcycle and start with
  //TODO: a forced map change since the first map set via +map does is not played in the regular cycle.

  // Check if we had a forced or a normal map change.
  if ( g_iForcedMapChange != 0 ) --g_iForcedMapChange;
  // If we had a normal map change, reset the stored nextmap.
  if ( g_iForcedMapChange == 0 ) g_pcNextMap = nullptr;


#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnServerActivate(pEdictList, edictCount, clientMax);
#endif
}

const char *GetGameDescription() {
  const char* pcDescription = nullptr;
  const int iResult = AM_GetGameDescription( pcDescription );

#ifdef USE_METAMOD
  SetMetaResult(iResult);
  return pcDescription;
#else
  if (iResult == RESULT_CONTINUE) {
	  return(other_gFunctionTable.pfnGetGameDescription());
  } else {
	  return ( pcDescription );
  }
#endif
}

/*
int	InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message ) {

	int iResult = RESULT_CONTINUE;

	if ( ptAM_keyfiles->value > 0 ) {
		iResult = AM_InconsistentFile( player, filename, disconnect_message );
	}  // if

#ifdef USE_METAMOD
	SetMetaResult( iResult );
	if ( iResult == RESULT_CONTINUE ) {
		return 0;
	} else {
		return 1;
	}  // if-else
#else
	if ( iResult == RESULT_CONTINUE ) {
		return (other_gFunctionTable.pfnInconsistentFile(player,filename,disconnect_message));
	} else {
		return 1;
	}  // if-else
#endif
}
*/


/* Functions that we need to send back to the engine,
   regardless of whether we're using metamod or not */
int GameDispatchSpawn( edict_t *pent ) {
#if 0
	if( ptAM_keyfiles->value > 0 ) {
		float min = 0.0;
		float max = 2.0;
		PRECACHE_GENERIC( "test.akf" );
		FORCE_UNMODIFIED( force_exactfile, &min, &max, "test.akf" );
	}  // if
#endif
  //if ( pent )
	  //UTIL_LogPrintf( "classanme %s globalname %s\n", STRING(pent->v.classname), STRING(pent->v.globalname) );
#ifdef USE_METAMOD
	SetMetaResult(RESULT_CONTINUE);
	//return(gpGamedllFuncs->dllapi_table->pfnSpawn(pent));
	return 1;
#else
	return(other_gFunctionTable.pfnSpawn(pent));
#endif
}


void PlayerPreThink( edict_t *pEntity ) {
	const int iResult = AM_PlayerPreThink( pEntity );
#ifdef USE_METAMOD
	SetMetaResult(iResult);
#else
	if ( iResult == RESULT_CONTINUE )
		other_gFunctionTable.pfnPlayerPreThink(pEntity);
#endif
}

void ClientPutInServer( edict_t *pEntity ) {
	const int iResult = AM_ClientPutInServer( pEntity );
  SetMetaResult( iResult );
}
 
void ServerDeactivate() {
	const int iResult = AM_ServerDeactivate();
  SetMetaResult( iResult );
} 


void StartFrame() {
	const int iResult = AM_StartFrame();
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnStartFrame();
#endif
}



/* This stuff isn't needed...but might be some day (the function prototypes
   are useful all by the themselves...so I just can't bear to delete it. 
   
   So let's just keep it in this never-to-be-compiled area, shall we? */
#if 0
void DispatchUse( edict_t *pentUsed, edict_t *pentOther ) {
  other_gFunctionTable.pfnUse(pentUsed, pentOther);
}

int DispatchSpawn( edict_t *pent ) {
  return(other_gFunctionTable.pfnSpawn(pent));
}

void DispatchTouch( edict_t *pentTouched, edict_t *pentOther ) {
  other_gFunctionTable.pfnTouch(pentTouched, pentOther);
}

void DispatchBlocked( edict_t *pentBlocked, edict_t *pentOther ) {
  other_gFunctionTable.pfnBlocked(pentBlocked, pentOther);
}

void DispatchKeyValue( edict_t *pentKeyvalue, KeyValueData *pkvd ) {
  other_gFunctionTable.pfnKeyValue(pentKeyvalue, pkvd);
}

void DispatchSave( edict_t *pent, SAVERESTOREDATA *pSaveData ) {
  other_gFunctionTable.pfnSave(pent, pSaveData);
}

int DispatchRestore( edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity ) {
  return(other_gFunctionTable.pfnRestore(pent, pSaveData, globalEntity));
}

//void DispatchObjectCollsionBox( edict_t *pent ) {
//  other_gFunctionTable.pfnSetAbsBox(pent);
//}

void SaveWriteFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount ) {
  other_gFunctionTable.pfnSaveWriteFields(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveReadFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount ) {
  other_gFunctionTable.pfnSaveReadFields(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveGlobalState( SAVERESTOREDATA *pSaveData ) {
  other_gFunctionTable.pfnSaveGlobalState(pSaveData);
}

void RestoreGlobalState( SAVERESTOREDATA *pSaveData ) {
  other_gFunctionTable.pfnRestoreGlobalState(pSaveData);
}

void ResetGlobalState() {
  other_gFunctionTable.pfnResetGlobalState;
}

void ClientKill( edict_t *pEntity ) {
  other_gFunctionTable.pfnClientKill(pEntity);
}

void ClientPutInServer( edict_t *pEntity ) {
  other_gFunctionTable.pfnClientPutInServer(pEntity);
}

void ServerDeactivate() {
  other_gFunctionTable.pfnServerDeactivate;
}

//void PlayerPreThink( edict_t *pEntity ) {
//  other_gFunctionTable.pfnPlayerPreThink(pEntity);
//}

void PlayerPostThink( edict_t *pEntity ) {
  other_gFunctionTable.pfnPlayerPostThink(pEntity);
}

//void StartFrame() {
//  other_gFunctionTable.pfnStartFrame;
//}

void ParmsNewLevel() {
  other_gFunctionTable.pfnParmsNewLevel;
}

void ParmsChangeLevel() {
  other_gFunctionTable.pfnParmsChangeLevel;
}


void PlayerCustomization( edict_t *pEntity, customization_t *pCust ) {
  other_gFunctionTable.pfnPlayerCustomization(pEntity, pCust);
}

void SpectatorConnect( edict_t *pEntity ) {
  other_gFunctionTable.pfnSpectatorConnect(pEntity);
}

void SpectatorDisconnect( edict_t *pEntity ) {
  other_gFunctionTable.pfnSpectatorDisconnect(pEntity);
}

void SpectatorThink( edict_t *pEntity ) {
  other_gFunctionTable.pfnSpectatorThink(pEntity);
}

void Sys_Error( const char *error_string ) {
  other_gFunctionTable.pfnSys_Error(error_string);
}

void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas ) {
  other_gFunctionTable.pfnSetupVisibility(pViewEntity,pClient,pvs,pas);
}

void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd ) {
  other_gFunctionTable.pfnUpdateClientData(ent,sendweapons,cd);
}

int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet ) {
  return (other_gFunctionTable.pfnAddToFullPack(state,e,ent,host,hostflags,player,pSet));
}

void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs ) {
  other_gFunctionTable.pfnCreateBaseline(player,eindex,baseline,entity,playermodelindex,player_mins,player_maxs);
}

void RegisterEncoders() {
  other_gFunctionTable.pfnRegisterEncoders;
}

int GetWeaponData( struct edict_s *player, struct weapon_data_s *info ) {
  return(other_gFunctionTable.pfnGetWeaponData(player,info));
}

void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed ) {
  other_gFunctionTable.pfnCmdStart(player,cmd,random_seed);
}

void CmdEnd ( const edict_t *player ) {
  other_gFunctionTable.pfnCmdEnd(player);
}

int ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size ) {
  return (other_gFunctionTable.pfnConnectionlessPacket(net_from,args,response_buffer,response_buffer_size));
}


int GetHullBounds( int hullnumber, float *mins, float *maxs ) {
  return (other_gFunctionTable.pfnGetHullBounds(hullnumber,mins,maxs));
}

void CreateInstancedBaselines () {
  other_gFunctionTable.pfnCreateInstancedBaselines;
}


int	InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message ) {
  return (other_gFunctionTable.pfnInconsistentFile(player,filename,disconnect_message));
}


void PM_Move ( struct playermove_s *ppmove, int server ) {
  other_gFunctionTable.pfnPM_Move(ppmove,server);
}

void PM_Init( struct playermove_s *ppmove ) {
  other_gFunctionTable.pfnPM_Init(ppmove);
}

char PM_FindTextureType( char *name ) {
  return(other_gFunctionTable.pfnPM_FindTextureType(name));
}

static DLL_FUNCTIONS gFunctionTable = 
  {
    GameDLLInit,				//pfnGameInit
    DispatchSpawn,				//pfnSpawn
    DispatchThink,				//pfnThink
    DispatchUse,				//pfnUse
    DispatchTouch,				//pfnTouch
    DispatchBlocked,			//pfnBlocked
    DispatchKeyValue,			//pfnKeyValue
    DispatchSave,				//pfnSave
    DispatchRestore,			//pfnRestore
    DispatchObjectCollsionBox,	//pfnAbsBox
    
    SaveWriteFields,			//pfnSaveWriteFields
    SaveReadFields,				//pfnSaveReadFields
    
    SaveGlobalState,			//pfnSaveGlobalState
    RestoreGlobalState,			//pfnRestoreGlobalState
    ResetGlobalState,			//pfnResetGlobalState
    
    ClientConnect,				//pfnClientConnect
    ClientDisconnect,			//pfnClientDisconnect
    ClientKill,					//pfnClientKill
    ClientPutInServer,			//pfnClientPutInServer
    ClientCommand,				//pfnClientCommand
    ClientUserInfoChanged,		//pfnClientUserInfoChanged
    ServerActivate,				//pfnServerActivate
    ServerDeactivate,			//pfnServerDeactivate
    
    PlayerPreThink,				//pfnPlayerPreThink
    PlayerPostThink,			//pfnPlayerPostThink
    
    StartFrame,					//pfnStartFrame
    ParmsNewLevel,				//pfnParmsNewLevel
    ParmsChangeLevel,			//pfnParmsChangeLevel
    
    GetGameDescription,         //pfnGetGameDescription    Returns string describing current .dll game.
    PlayerCustomization,        //pfnPlayerCustomization   Notifies .dll of new customization for player.
    
    SpectatorConnect,			//pfnSpectatorConnect      Called when spectator joins server
    SpectatorDisconnect,        //pfnSpectatorDisconnect   Called when spectator leaves the server
    SpectatorThink,				//pfnSpectatorThink        Called when spectator sends a command packet (usercmd_t)
    
    Sys_Error,					//pfnSys_Error				Called when engine has encountered an error
    
    PM_Move,					//pfnPM_Move
    PM_Init,					//pfnPM_Init				Server version of player movement initialization
    PM_FindTextureType,			//pfnPM_FindTextureType
    
    SetupVisibility,			//pfnSetupVisibility        Set up PVS and PAS for networking for this client
    UpdateClientData,			//pfnUpdateClientData       Set up data sent only to specific client
    AddToFullPack,				//pfnAddToFullPack
    CreateBaseline,				//pfnCreateBaseline			Tweak entity baseline for network encoding, allows setup of player baselines, too.
    RegisterEncoders,			//pfnRegisterEncoders		Callbacks for network encoding
    GetWeaponData,				//pfnGetWeaponData
    CmdStart,					//pfnCmdStart
    CmdEnd,						//pfnCmdEnd
    ConnectionlessPacket,		//pfnConnectionlessPacket
    GetHullBounds,				//pfnGetHullBounds
    CreateInstancedBaselines,   //pfnCreateInstancedBaselines
    InconsistentFile,			//pfnInconsistentFile
  };
#endif

DLL_FUNCTIONS gFunctionTable = 
  {
    GameDLLInit,	//pfnGameInit
    GameDispatchSpawn,		//pfnSpawn
    DispatchThink,				//pfnThink
    nullptr,	// DispatchUse,			//pfnUse
    nullptr,	// DispatchTouch,		//pfnTouch
    nullptr,	// DispatchBlocked,		//pfnBlocked
    nullptr,	// DispatchKeyValue,		//pfnKeyValue
    nullptr,	// DispatchSave,		//pfnSave
    nullptr,	// DispatchRestore,		//pfnRestore
    DispatchObjectCollsionBox,	// DispatchObjectCollsionBox,	//pfnAbsBox

    nullptr,	// SaveWriteFields,		//pfnSaveWriteFields
    nullptr,	// SaveReadFields,		//pfnSaveReadFields

    nullptr,	// SaveGlobalState,		//pfnSaveGlobalState
    nullptr,	// RestoreGlobalState,		//pfnRestoreGlobalState
    nullptr,	// ResetGlobalState,		//pfnResetGlobalState
    
    ClientConnect,				//pfnClientConnect
    ClientDisconnect,	// ClientDisconnect,		//pfnClientDisconnect
    nullptr,	// ClientKill,			//pfnClientKill
    ClientPutInServer,		//pfnClientPutInServer
    ClientCommand,				//pfnClientCommand
    ClientUserInfoChanged,			//pfnClientUserInfoChanged
    ServerActivate,				//pfnServerActivate
    ServerDeactivate,		//pfnServerDeactivate
    
    PlayerPreThink,		//pfnPlayerPreThink
    nullptr,	// PlayerPostThink,		//pfnPlayerPostThink
    
    StartFrame,			//pfnStartFrame
    nullptr,	// ParmsNewLevel,		//pfnParmsNewLevel
    nullptr,	// ParmsChangeLevel,		//pfnParmsChangeLevel
    
    GetGameDescription,		//pfnGetGameDescription    Returns string describing current .dll game.
    nullptr,	// PlayerCustomization,		//pfnPlayerCustomization   Notifies .dll of new customization for player.

    nullptr,	// SpectatorConnect,		//pfnSpectatorConnect      Called when spectator joins server
    nullptr,	// SpectatorDisconnect,		//pfnSpectatorDisconnect   Called when spectator leaves the server
    nullptr,	// SpectatorThink,		//pfnSpectatorThink        Called when spectator sends a command packet (usercmd_t)

    nullptr,	// Sys_Error,			//pfnSys_Error				Called when engine has encountered an error

    nullptr,	// PM_Move,			//pfnPM_Move
    nullptr,	// PM_Init,			//pfnPM_Init				Server version of player movement initialization
    nullptr,	// PM_FindTextureType,		//pfnPM_FindTextureType

    nullptr,	// SetupVisibility,		//pfnSetupVisibility        Set up PVS and PAS for networking for this client
    nullptr,	// UpdateClientData,		//pfnUpdateClientData       Set up data sent only to specific client
    nullptr,	// AddToFullPack,		//pfnAddToFullPack
    nullptr,	// CreateBaseline,		//pfnCreateBaseline			Tweak entity baseline for network encoding, allows setup of player baselines, too.
    nullptr,	// RegisterEncoders,		//pfnRegisterEncoders		Callbacks for network encoding
    nullptr,	// GetWeaponData,		//pfnGetWeaponData
    nullptr,	// CmdStart,			//pfnCmdStart
    nullptr,	// CmdEnd,			//pfnCmdEnd
    nullptr,	// ConnectionlessPacket,	//pfnConnectionlessPacket
    nullptr,	// GetHullBounds,		//pfnGetHullBounds
    nullptr,	// CreateInstancedBaselines,	//pfnCreateInstancedBaselines
    nullptr,   //InconsistentFile,		//pfnInconsistentFile
  };


DLL_FUNCTIONS gFunctionTable_Post;


NEW_DLL_FUNCTIONS gNewFunctionTable = 
  {
    OnFreeEntPrivateData,		//! pfnOnFreeEntPrivateData()	Called right before the object's memory is freed.  Calls its destructor.
    nullptr,	// pfnGameShutdown()
    nullptr,	// pfnShouldCollide()
  };

#ifndef _WIN32
extern "C" EXPORT int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion );  
int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion ) {
#else
extern "C" _declspec( dllexport) int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion ) {
#endif
	// this is just so that the version string doesn't get optimised away.
	if ( strncmp(vstring+1, "Pg", 2) != 0) printf("");

#ifdef USE_METAMOD
  if ( !pFunctionTable || interfaceVersion != INTERFACE_VERSION ) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Bad interface version\n" );
    return FALSE;
  }
#else
  if (!other_GetEntityAPI(&other_gFunctionTable, INTERFACE_VERSION)) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Unable to get GetEntityAPI function table\n" );
    return FALSE;  
  }
  memcpy(&gFunctionTable , &other_gFunctionTable, sizeof( DLL_FUNCTIONS ) );
  gFunctionTable.pfnClientCommand = ClientCommand; // add the admin_commands and parse the vote commands
  gFunctionTable.pfnClientConnect = ClientConnect; // load the users file on startup
  gFunctionTable.pfnGameInit = GameDLLInit; // register our extra cvars
  gFunctionTable.pfnClientUserInfoChanged = ClientUserInfoChanged; // check for name changes to reserved nicks
  gFunctionTable.pfnServerActivate = ServerActivate;// reset load files
  gFunctionTable.pfnClientDisconnect = ClientDisconnect;
  gFunctionTable.pfnThink = DispatchThink; // used to intercept the timer
  gFunctionTable.pfnGetGameDescription = GetGameDescription; // used to hide reserved slots
  //gFunctionTable.pfnInconsistentFile = InconsistentFile;
  gFunctionTable.pfnPlayerPreThink = PlayerPreThink; // used against the spectator cheat
  gFunctionTable.pfnStartFrame = StartFrame;
  /* gFunctionTable.pfnSetAbsBox = DispatchObjectCollsionBox; // needed? */
#endif
  memcpy( pFunctionTable, &gFunctionTable, sizeof( DLL_FUNCTIONS ) );
  return TRUE;
}
  

#ifdef USE_METAMOD
#  ifndef _WIN32
extern "C" EXPORT int GetEntityAPI_Post( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion );  
int GetEntityAPI_Post( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion ) {
#  else
extern "C" _declspec( dllexport) int GetEntityAPI_Post( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion ) {
#  endif

  if ( !pFunctionTable || interfaceVersion != INTERFACE_VERSION ) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Bad interface version\n" );
    return FALSE;
  }
  memset(&gFunctionTable_Post, 0, sizeof(DLL_FUNCTIONS) );
  gFunctionTable_Post.pfnClientConnect = ClientConnect_Post;

  memcpy( pFunctionTable, &gFunctionTable_Post, sizeof(DLL_FUNCTIONS) );
  return TRUE;
}
#endif // USE_METAMOD  


#ifndef _WIN32
extern "C" EXPORT int GetNewDLLFunctions( NEW_DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion );
int GetNewDLLFunctions( NEW_DLL_FUNCTIONS *pFunctionTable, int* interfaceVersion ) {
#else
extern "C" _declspec( dllexport) int GetNewDLLFunctions( NEW_DLL_FUNCTIONS *pFunctionTable, int *interfaceVersion ) {
#endif
#ifdef USE_METAMOD
  if ( !pFunctionTable ) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Bad interface version\n" );
    return FALSE;
  }
#else
  if( other_GetNewDLLFunctions == nullptr) 
    return FALSE;
  
  if (!other_GetNewDLLFunctions(&other_gNewFunctionTable, interfaceVersion)) { 
    UTIL_LogPrintf( "[ADMIN] ERROR: Unable to get GetNewDLLFunctions function table\n" );
    return FALSE;  // error initializing function table!!! 
  } 
  memcpy(&gNewFunctionTable , &other_gNewFunctionTable, sizeof( NEW_DLL_FUNCTIONS ) );
  gNewFunctionTable.pfnOnFreeEntPrivateData = OnFreeEntPrivateData;
#endif
  memcpy( pFunctionTable, &gNewFunctionTable, sizeof( NEW_DLL_FUNCTIONS ) );
  return TRUE; 
}
    
