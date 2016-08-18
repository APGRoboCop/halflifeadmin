/* Stubs for the various dll functions
 *
 *  Copyright (c) Alfred Reynolds, 2000
 *  This file is covered by the GPL.
 *
 * $Id: dll.cpp,v 1.11 2001/09/19 22:29:24 darope Exp $
 *
 */   

const char vstring[] = "$Id: dll.cpp,v 1.11 2001/09/19 22:29:24 darope Exp $";

#include "extdll.h"
#define SDK_UTIL_H	// extdll.h already #include's util.h
#ifdef USE_METAMOD
  #include "meta_api.h"
#endif

#ifdef _WIN32
  typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
  typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
  #define EXPORT  _declspec( dllexport )
#else
  typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int); 
  typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
  #define EXPORT
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
  "19-09-2001",	// date
  "Alfred Reynolds <alfred@mazuma.net.au>",	// author
  "http://www.adminmod.org/",	// url
  "ADMIN",	// logtag
  PT_STARTUP,	// (when) loadable
  PT_STARTUP,	// (when) unloadable
};

// Must provide at least one of these..
static META_FUNCTIONS gMetaFunctionTable = {
  GetEntityAPI,		// pfnGetEntityAPI		HL SDK; called before game DLL
  NULL,			// pfnGetEntityAPI_Post		META; called after game DLL
  NULL,			// pfnGetEntityAPI2		HL SDK2; called before game DLL
  NULL,			// pfnGetEntityAPI2_Post	META; called after game DLL
  GetNewDLLFunctions, // pfnGetNewDLLFunctions	HL SDK2; called before game DLL
  NULL,			// pfnGetNewDLLFunctions_Post	META; called after game DLL
  NULL,			// pfnGetEngineFunctions	META; called before HL engine
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
  return(TRUE);
}

// Metamod requesting info about this plugin
//  ifvers		(given) interface_version metamod is using
//  pPlugInfo		(requested) struct with info about plugin
//  pMetaUtilFuncs	(given) table of utility functions provided by metamod
C_DLLEXPORT int Meta_Query(char *ifvers, plugin_info_t **pPlugInfo,
		mutil_funcs_t *pMetaUtilFuncs) 
{
  if ((int) CVAR_GET_FLOAT("developer") != 0) 
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
    LOG_MESSAGE(PLID, "WARNING: meta-interface version mismatch; requested=%s ours=%s", ifvers, Plugin_info.ifvers);
    sscanf(ifvers, "%d:%d", &mmajor, &mminor);
    sscanf(META_INTERFACE_VERSION, "%d:%d", &pmajor, &pminor);
    // If plugin has later interface version, it's incompatible (update 
    // metamod).
    if(pmajor > mmajor || (pmajor==mmajor && pminor > mminor)) {
      LOG_ERROR(PLID, "metamod version is too old for plugin %s v.%s; update metamod", Plugin_info.name, Plugin_info.version);
      return(FALSE);
    }
    // If plugin has older major interface version, it's incompatible (update 
    // plugin).
    else if(pmajor < mmajor) {
      LOG_ERROR(PLID, "metamod version is incompatible with plugin %s v.%s; please find a newer version of %s", Plugin_info.name, Plugin_info.version, Plugin_info.name);
      return(FALSE);
    }
    // Minor interface is older, but this is guaranteed to be backwards
    // compatible, so we warn, but we still accept it.
    else if(pmajor==mmajor && pminor < mminor)
      LOG_MESSAGE(PLID, "WARNING: metamod version is newer than expected; consider finding a newer version of plugin %s", Plugin_info.name);
    else
      LOG_ERROR(PLID, "unexpected version comparision; metavers=%s, mmajor=%d, mminor=%d; plugvers=%s, pmajor=%d, pminor=%d", ifvers, mmajor, mminor, META_INTERFACE_VERSION, pmajor, pminor); }
  return(TRUE);
}

C_DLLEXPORT int GetEngineFunctions_Post(enginefuncs_t *pengfuncsFromEngine, int *interfaceVersion ) 
{
  if ((int) CVAR_GET_FLOAT("developer") != 0)
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
    if ((int)CVAR_GET_FLOAT("admin_debug") > 2)
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


void ClientCommand( edict_t *pEntity ) {
  int iResult = AM_ClientCommand(pEntity);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
const char *GetGameDescription( void ) {
  return(other_gFunctionTable.pfnGetGameDescription());
}
    other_gFunctionTable.pfnClientCommand(pEntity);
#endif
}

BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] ) {
  int iResult = AM_ClientConnect(pEntity,pszName,pszAddress,szRejectReason);
#ifdef USE_METAMOD
  if (iResult == TRUE) {
    RETURN_META_VALUE(MRES_HANDLED,TRUE);
  } else {
    RETURN_META_VALUE(MRES_SUPERCEDE,FALSE);
  }
#else
  if (iResult == TRUE) {
    return(other_gFunctionTable.pfnClientConnect(pEntity, pszName, pszAddress, szRejectReason));
  } else {
    return FALSE;
  }
#endif
}

void ClientDisconnect( edict_t *pEntity ) {
  int iResult = AM_ClientDisconnect(pEntity);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnClientDisconnect(pEntity);
#endif
}

void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer ) {
  int iResult = AM_ClientUserInfoChanged(pEntity,infobuffer);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnClientUserInfoChanged(pEntity, infobuffer);
#endif
}

void DispatchThink( edict_t *pent ) {
  int iResult = AM_DispatchThink(pent);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE) 
    other_gFunctionTable.pfnThink(pent);
#endif
}

void GameDLLInit( void ) {
  int iResult = AM_GameDLLInit();
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE) 
    other_gFunctionTable.pfnGameInit();
#endif
}

void OnFreeEntPrivateData( edict_t *pEnt ) {
  int iResult = AM_OnFreeEntPrivateData(pEnt);
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE) 
    other_gNewFunctionTable.pfnOnFreeEntPrivateData(pEnt);
#endif
}

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax ) {
  int iResult = AM_Initialize();
#ifdef USE_METAMOD
  SetMetaResult(iResult);
#else
  if (iResult == RESULT_CONTINUE)
    other_gFunctionTable.pfnServerActivate(pEdictList, edictCount, clientMax);
#endif
}

const char *GetGameDescription( void ) {
  const char* pcDescription = 0;
  int iResult = AM_GetGameDescription( pcDescription );
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



/* Functions that we need to send back to the engine,
   regardless of whether we're using metamod or not */
int GameDispatchSpawn( edict_t *pent ) {
#ifdef USE_METAMOD
  return(gpGamedllFuncs->dllapi_table->pfnSpawn(pent));
#else
  return(other_gFunctionTable.pfnSpawn(pent));
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

void ResetGlobalState( void ) {
  other_gFunctionTable.pfnResetGlobalState;
}

void ClientKill( edict_t *pEntity ) {
  other_gFunctionTable.pfnClientKill(pEntity);
}

void ClientPutInServer( edict_t *pEntity ) {
  other_gFunctionTable.pfnClientPutInServer(pEntity);
}

void ServerDeactivate( void ) {
  other_gFunctionTable.pfnServerDeactivate;
}

void PlayerPreThink( edict_t *pEntity ) {
  other_gFunctionTable.pfnPlayerPreThink(pEntity);
}

void PlayerPostThink( edict_t *pEntity ) {
  other_gFunctionTable.pfnPlayerPostThink(pEntity);
}

void StartFrame( void ) {
  other_gFunctionTable.pfnStartFrame;
}

void ParmsNewLevel( void ) {
  other_gFunctionTable.pfnParmsNewLevel;
}

void ParmsChangeLevel( void ) {
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

void RegisterEncoders( void ) {
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

void CreateInstancedBaselines ( void ) {
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
    NULL,	// DispatchSpawn,		//pfnSpawn
    DispatchThink,				//pfnThink
    NULL,	// DispatchUse,			//pfnUse
    NULL,	// DispatchTouch,		//pfnTouch
    NULL,	// DispatchBlocked,		//pfnBlocked
    NULL,	// DispatchKeyValue,		//pfnKeyValue
    NULL,	// DispatchSave,		//pfnSave
    NULL,	// DispatchRestore,		//pfnRestore
    DispatchObjectCollsionBox,	// DispatchObjectCollsionBox,	//pfnAbsBox
    
    NULL,	// SaveWriteFields,		//pfnSaveWriteFields
    NULL,	// SaveReadFields,		//pfnSaveReadFields
    
    NULL,	// SaveGlobalState,		//pfnSaveGlobalState
    NULL,	// RestoreGlobalState,		//pfnRestoreGlobalState
    NULL,	// ResetGlobalState,		//pfnResetGlobalState
    
    ClientConnect,				//pfnClientConnect
    ClientDisconnect,	// ClientDisconnect,		//pfnClientDisconnect
    NULL,	// ClientKill,			//pfnClientKill
    NULL,	// ClientPutInServer,		//pfnClientPutInServer
    ClientCommand,				//pfnClientCommand
    ClientUserInfoChanged,			//pfnClientUserInfoChanged
    ServerActivate,				//pfnServerActivate
    NULL,	// ServerDeactivate,		//pfnServerDeactivate
    
    NULL,	// PlayerPreThink,		//pfnPlayerPreThink
    NULL,	// PlayerPostThink,		//pfnPlayerPostThink
    
    NULL,	// StartFrame,			//pfnStartFrame
    NULL,	// ParmsNewLevel,		//pfnParmsNewLevel
    NULL,	// ParmsChangeLevel,		//pfnParmsChangeLevel
    
    GetGameDescription,		//pfnGetGameDescription    Returns string describing current .dll game.
    NULL,	// PlayerCustomization,		//pfnPlayerCustomization   Notifies .dll of new customization for player.
    
    NULL,	// SpectatorConnect,		//pfnSpectatorConnect      Called when spectator joins server
    NULL,	// SpectatorDisconnect,		//pfnSpectatorDisconnect   Called when spectator leaves the server
    NULL,	// SpectatorThink,		//pfnSpectatorThink        Called when spectator sends a command packet (usercmd_t)
    
    NULL,	// Sys_Error,			//pfnSys_Error				Called when engine has encountered an error
    
    NULL,	// PM_Move,			//pfnPM_Move
    NULL,	// PM_Init,			//pfnPM_Init				Server version of player movement initialization
    NULL,	// PM_FindTextureType,		//pfnPM_FindTextureType
    
    NULL,	// SetupVisibility,		//pfnSetupVisibility        Set up PVS and PAS for networking for this client
    NULL,	// UpdateClientData,		//pfnUpdateClientData       Set up data sent only to specific client
    NULL,	// AddToFullPack,		//pfnAddToFullPack
    NULL,	// CreateBaseline,		//pfnCreateBaseline			Tweak entity baseline for network encoding, allows setup of player baselines, too.
    NULL,	// RegisterEncoders,		//pfnRegisterEncoders		Callbacks for network encoding
    NULL,	// GetWeaponData,		//pfnGetWeaponData
    NULL,	// CmdStart,			//pfnCmdStart
    NULL,	// CmdEnd,			//pfnCmdEnd
    NULL,	// ConnectionlessPacket,	//pfnConnectionlessPacket
    NULL,	// GetHullBounds,		//pfnGetHullBounds
    NULL,	// CreateInstancedBaselines,	//pfnCreateInstancedBaselines
    NULL,	// InconsistentFile,		//pfnInconsistentFile
  };

NEW_DLL_FUNCTIONS gNewFunctionTable = 
  {
    OnFreeEntPrivateData,		//! pfnOnFreeEntPrivateData()	Called right before the object's memory is freed.  Calls its destructor.
    NULL,	// pfnGameShutdown()
    NULL,	// pfnShouldCollide()
  };

#ifndef _WIN32
extern "C" EXPORT int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion );  
int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion ) {
#else
extern "C" _declspec( dllexport) int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion ) {
#endif
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
  /* gFunctionTable.pfnSetAbsBox = DispatchObjectCollsionBox; // needed? */
#endif
  memcpy( pFunctionTable, &gFunctionTable, sizeof( DLL_FUNCTIONS ) );
  return TRUE;
}
  
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
  if( other_GetNewDLLFunctions == NULL) 
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
    
