/*
 * $Id: h_export.cpp,v 1.19 2001/09/27 20:33:16 darope Exp $
 *
 *
 * Copyright (c) 1999-2001 Alfred Reynolds, Florian Zschocke, Magua
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
 * Dll interface
 *
 */   

#include <stdio.h>			// vsnprintf, etc

#include "version.h"
#include "extdll.h"
#include "users.h"
#include "amutil.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H      // extdll.h already #include's util.h
#include "meta_api.h"
#endif

/*
#ifdef _WIN32
#define DLLEXPORT (__declspec ( export ))
#else
#define DLLEXPORT
#endif
*/

enginefuncs_t  g_engfuncs;
globalvars_t *gpGlobals;
          
#ifdef CS_TEAMINFO
extern CGameRules* g_pGameRules;
extern FIV myCountTeams;
extern FII myCountTeamPlayers;
#endif

extern AMXINIT amx_Init;
extern AMXREGISTER amx_Register;
extern AMXFINDPUBLIC amx_FindPublic;
extern AMXEXEC amx_Exec;
extern AMXGETADDR amx_GetAddr;
extern AMXSTRLEN amx_StrLen;
extern AMXRAISEERROR amx_RaiseError;
extern AMXSETSTRING amx_SetString;
extern AMXGETSTRING amx_GetString;

extern DLL_GLOBAL BOOL  g_fUsersLoaded;  
extern auth_struct g_AuthArray[MAX_PLAYERS + 1];

DLL_GLOBAL enginefuncs_t my_engfuncs;
extern DLL_GLOBAL edict_t *pTimerEnt;


// this structure contains a list of supported mods and their dlls names
// To add support for another mod add an entry here, and add all the 
// exported entities to link_func.cpp

#ifndef USE_METAMOD
mod_struct_type mod_struct[] = {
  {"cstrike","cstrike\\dlls\\mp.dll","cstrike/dlls/cs_i386.so"},
  {"valve","valve\\dlls\\hl.dll","valve/dlls/hl_i386.so"},
  {"action","action\\dlls\\mp.dll","action/dlls/mp_i386.so"},
  {"tfc","tfc\\dlls\\tfc.dll","tfc/dlls/tfc_i386.so"},
  {"phineas","phineas\\dlls\\phineas.dll","phineas/dlls/phineas_i386.so"},
  {"bot","bot\\dlls\\bot.dll","bot/dlls/bot_i386.so"},
  {"freeze","freeze\\dlls\\mp.dll","freeze/dlls/mp_i386.so"},
  {"firearms","firearms\\dlls\\firearms.dll","firearms/dlls/fa_i386.so"},
  {"goldeneye","goldeneye\\dlls\\mp.dll","goldeneye/dlls/golden_i386.so"},
  {"oz","oz\\dlls\\mp.dll","Oz/dlls/mp_i386.so"},
  {"svencoop","svencoop\\dlls\\hl.dll","svencoop/dlls/hl_i386.so"},
  { "si","si\\dlls\\si.dll","si/dlls/si_i386.so"},
  { "frontline","frontline\\dlls\\frontline.dll","frontline/dlls/front_i386.so"},
  { "arg","arg\\dlls\\hl.dll","arg/dlls/arg_i386.so"},
  { "gangstawars","gangstawars\\dlls\\hl.dll","gangstawars/dlls/gansta_i386.so"},
  { "wizwars","wizwars\\dlls\\hl.dll","wizwars/dlls/mp_i386.so"},
  { "swarm","swarm\\dlls\\swarm.dll","swarm/dlls/swarm_i386.so"},
  { "gearbox","gearbox\\dlls\\opfor.dll","gearbox/dlls/opfor_i386.so"},
  { "dod","dod\\dlls\\hl.dll","dod/dlls/dod_i386.so"},
  { "wasteland","wasteland\\dlls\\hl.dll","wasteland/dlls/whl_linux.so"},
  //???? what is the win32 dll name?
  {NULL,NULL,NULL}
};
#endif

/*
#ifdef _WIN32
typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
#else
typedef void (*GIVEFNPTRSTODLL) ( enginefuncs_t* , globalvars_t *  );  
#endif
*/

#ifdef _WIN32
HINSTANCE h_Library = NULL;
HINSTANCE h_AMX = NULL;
#else
void *h_Library=NULL;
void *h_AMX=NULL;
#include <dlfcn.h>
#define GetProcAddress dlsym    
#endif

#ifndef USE_METAMOD
#  ifdef _WIN32
typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
typedef int (DLLEXPORT *GETENTITYAPI2)( DLL_FUNCTIONS *, int *);
typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
#  else
typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
//typedef int (*GIVESPAWN)(char *);
typedef int (*GETENTITYAPI2)( DLL_FUNCTIONS *, int * );
typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
#  endif
GETENTITYAPI other_GetEntityAPI;
GIVEFNPTRSTODLL other_GiveFnptrsToDll;
GETENTITYAPI2 other_GetEntityAPI2;      
GETNEWDLLFUNCTIONS other_GetNewDLLFunctions=NULL;
#endif

#ifdef _WIN32
// Required DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
  if (fdwReason == DLL_PROCESS_ATTACH) {
  } else if (fdwReason == DLL_PROCESS_DETACH) {
    if (h_Library!=NULL)
      FreeLibrary(h_Library);
  }
  return TRUE;
}
#endif



/* Engine functions we need to catch */
#ifdef WITH_LOGPARSING
void am_AlertMessage(ALERT_TYPE atype, char *szFmt, ...) {
  int iParse = (int)CVAR_GET_FLOAT("admin_parse_logs");
  plugin_result eResult = PLUGIN_CONTINUE;
  // If we're using metamod, we don't need to worry about sending
  // the alert message on to the other DLL; MM does that for us.
  // If we're not using metamod, we _do_ need to do this.
#  ifdef USE_METAMOD
  if (iParse == 0) {
    RETURN_META(MRES_IGNORED);
  }
#  endif
  
  char buf[MAX_STRBUF_LEN];
  va_list ap;
  va_start(ap, szFmt);
  vsprintf(buf, szFmt, ap);
  va_end(ap);
  if (iParse != 0) {
    eResult = HandleLog(buf);
  }
#  ifdef USE_METAMOD
  if (eResult == PLUGIN_HANDLED) {
    RETURN_META(MRES_HANDLED);
  } else {
    RETURN_META(MRES_IGNORED);
  }
#  else
  ALERT(atype,buf);
#  endif
}
#endif

edict_t *am_FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, 
                float rad)
{
  edict_t *ent;
  /* When using metamod, this function should be passed as a "_Post"
   * function, and metamod handles the initial engine call. */ 

#ifdef USE_METAMOD 
        ent=META_RESULT_ORIG_RET(edict_t *); 
#else 
        ent=FIND_ENTITY_IN_SPHERE(pEdictStartSearchAfter, org, rad);
#endif

  /* If this found the timer ent, don't let game DLL have this; find the
   * next ent. */


  if(ent==pTimerEnt) {
    DEBUG_LOG( 3, ("Hiding timer entity from FindEntityInSphere") );
    ent=FIND_ENTITY_IN_SPHERE(ent, org, rad);
    DEBUG_LOG( 3, ("Returning next entity: %s", ent ? STRING(ent->v.classname) : "nil") );

#ifdef USE_METAMOD
   RETURN_META_VALUE(MRES_OVERRIDE, ent);
#endif
  }
#ifdef USE_METAMOD
  RETURN_META_VALUE(MRES_IGNORED, ent);
#else
  return(ent);
#endif
}



/* Engine functions we need to catch */
edict_t *am_EntitiesInPVS(edict_t *pplayer)
{
  edict_t *ent;
  /* When using metamod, this function should be passed as a "_Post"
   * function, and metamod handles the initial engine call. */ 

#ifdef USE_METAMOD 
        ent=META_RESULT_ORIG_RET(edict_t *); 
#else 
        ent=UTIL_EntitiesInPVS(pplayer);
#endif

  /* If this found the timer ent, don't let game DLL have this; find the
   * next ent. */


  if(ent==pTimerEnt) {
    DEBUG_LOG( 3, ("Hiding timer entity from FindEntityInSphere") );
    ent=UTIL_EntitiesInPVS(ent);
    DEBUG_LOG( 3, ("Returning next entity: %s",ent ? STRING(ent->v.classname) : "nil") );
#ifdef USE_METAMOD
   RETURN_META_VALUE(MRES_OVERRIDE, ent);
#endif
  }
#ifdef USE_METAMOD
  RETURN_META_VALUE(MRES_IGNORED, ent);
#else
  return(ent);
#endif
}



/* Engine functions we need to catch */
edict_t *am_FindEntityByVars(struct entvars_s *pvars)
{
  edict_t *ent;
  /* When using metamod, this function should be passed as a "_Post"
   * function, and metamod handles the initial engine call. */ 

#ifdef USE_METAMOD 
        ent=META_RESULT_ORIG_RET(edict_t *); 
#else 
        ent=(*g_engfuncs.pfnFindEntityByVars)(pvars);
#endif

  /* If this found the timer ent, don't let game DLL have this; find the
   * next ent. */


  if(ent==pTimerEnt) {
    DEBUG_LOG( 3, ("Hiding timer entity from FindEntityInSphere") );
    ent=NULL;
    DEBUG_LOG( 3, ("Returning next entity: %s",ent ? STRING(ent->v.classname) : "nil") );
#ifdef USE_METAMOD
   RETURN_META_VALUE(MRES_OVERRIDE, ent);
#endif
  }
#ifdef USE_METAMOD
  RETURN_META_VALUE(MRES_IGNORED, ent);
#else
  return(ent);
#endif
}

//
// CVars registered here
//
//extern cvar_t am_devel;
extern cvar_t admin_debug;
extern cvar_t admin_bot_protection;
extern cvar_t amv_autoban;

// This one is over here since we need it right from the beginning.
cvar_t am_devel = {"am_devel","0",FCVAR_EXTDLL, 0};  // used for extended debugging for developers

#ifdef _WIN32  
void WINAPI GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals ) {
#else
extern "C" void DLLEXPORT GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t
*pGlobals ) {
#endif  
  char game_dir[2048];
  memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
  gpGlobals = pGlobals;


  // This is the very first function called by either the engine or metamod.
  // We do some init stuff here for things that we need right from the beginning.
  // Register the devel logging cvar and get direct access to it.
  //CVAR_REGISTER(&am_devel);
  CVAR_REGISTER(&admin_bot_protection);
  CVAR_REGISTER(&admin_debug);
  CVAR_REGISTER(&amv_autoban);
  ptAM_devel = 0;
  ptAM_devel = CVAR_GET_POINTER( "developer" );
  ptAM_debug = 0;
  ptAM_debug = CVAR_GET_POINTER( "admin_debug" );
  ptAM_autoban = 0;
  ptAM_autoban = CVAR_GET_POINTER( "amv_autoban" );
  ptAM_botProtection = 0;
  ptAM_botProtection = CVAR_GET_POINTER( "admin_bot_protection" );



  // Greetings, earthling!  
  UTIL_LogPrintf( "[ADMIN] Admin Mod, the HL admin's choice\n");
  UTIL_LogPrintf( "[ADMIN] by Alfred Reynolds (http://www.adminmod.org)\n");
  UTIL_LogPrintf( "[ADMIN] Version: %s\n", MOD_VERSION);
  UTIL_LogPrintf( "[ADMIN] Compiled: %s %s (%s)\n", COMPILE_DTTM, TZONE,  OPT_TYPE );
  /*
  // If we haven't read server.cfg yet, allow devel logging
  // to be enabled with the "+developer" cmdline option.
  if ( ((int) CVAR_GET_FLOAT("developer") != 0) && ((int) CVAR_GET_FLOAT("am_devel") == 0) ) {
    CVAR_SET_FLOAT("am_devel", 2.0);
    DEVEL_LOG(2, ("+developer was set, enabling devel logging level 2"));
  }  // if
  */


  char dll_name[2048];
  char mod_name[32];

#ifndef USE_METAMOD
  int length;
  char *pFileList;
  if (h_Library !=NULL) { 
    UTIL_LogPrintf("PLUG-IN Already loaded\n");
  }
  
  GET_GAME_DIR(game_dir);
  char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME("admin.ini", &length );
  
  if (pFileList==NULL) {
    UTIL_LogPrintf("[ADMIN] Autodetecting dll to use\n");
    UTIL_LogPrintf("[ADMIN] Mod Dir: %s\n",GetModDir());
    strcpy(mod_name, GetModDir());
    
    int i=0;
    while(mod_struct[i].mod!=NULL) {
      if (!stricmp(mod_name,mod_struct[i].mod)) {
#ifdef _WIN32
        strcpy(dll_name,mod_struct[i].windir);
#else
        strcpy(dll_name,mod_struct[i].linuxdir);
#endif
        break;
      }
      i++;
    }
    if ( mod_struct[i].mod==NULL) {
      UTIL_LogPrintf("[ADMIN] ERROR: Mod %s not supported\n",mod_name);
      exit(1);
    }
  } else {
    // we found admin.ini and will load that dll
    if ( pFileList && length ) {
      char cBuf[1024]; // room for the name + password + access value
      int ret=sscanf( pFileList, "%s\n", cBuf);
      if ( ret>0) 
#ifndef _WIN32
        snprintf(dll_name,2048,"%s/%s",game_dir,cBuf);  
#else
      sprintf(dll_name,"%s\\%s",game_dir,cBuf); 
#endif
      FREE_FILE( aFileList );
    }
  }
#endif
  
  GET_GAME_DIR(game_dir);
  char script_dll[2048];
#ifdef _WIN32
  sprintf(script_dll,"%s\\%s",game_dir,SCRIPT_DLL);     
  h_AMX = LoadLibrary(script_dll);
#else
  snprintf(script_dll,2048,"%s/%s",game_dir,SCRIPT_DLL);        
  h_AMX = dlopen(script_dll, RTLD_NOW); 
#endif
  if ( h_AMX == NULL) {
#ifdef WIN32
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (%s) %s\n",script_dll,GetLastError());
#else
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (%s) %s\n",script_dll,dlerror());
#endif    
    exit(1);
  }
  
  amx_Init= (AMXINIT)GetProcAddress(h_AMX, "amx_Init");
  if (amx_Init == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_Init)\n");
    exit(1);
  }
  
  amx_Register= (AMXREGISTER)GetProcAddress(h_AMX, "amx_Register");
  if (amx_Register == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_Register)\n");
    exit(1);
  }
  
  amx_FindPublic= (AMXFINDPUBLIC)GetProcAddress(h_AMX, "amx_FindPublic");
  if (amx_FindPublic == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_FindPublic)\n");
    exit(1);
  }
  
  amx_Exec= (AMXEXEC)GetProcAddress(h_AMX, "amx_Exec");
  if (amx_Exec == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_Exec)\n");
    exit(1);
  }
  
  amx_GetAddr= (AMXGETADDR)GetProcAddress(h_AMX, "amx_GetAddr");
  if (amx_GetAddr == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_GetAddr)\n");
    exit(1);
  }
  
  amx_StrLen= (AMXSTRLEN)GetProcAddress(h_AMX, "amx_StrLen");
  if (amx_StrLen == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_StrLen)\n");
    exit(1);
  }
  
  amx_RaiseError= (AMXRAISEERROR)GetProcAddress(h_AMX, "amx_RaiseError");
  if (amx_RaiseError == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_RaiseError)\n");
    exit(1);
  }
  
  amx_SetString= (AMXSETSTRING)GetProcAddress(h_AMX, "amx_SetString");
  if (amx_SetString == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_SetString)\n");
    exit(1);
  }
  
  amx_GetString= (AMXGETSTRING)GetProcAddress(h_AMX, "amx_GetString");
  if (amx_GetString == NULL ){
    UTIL_LogPrintf( "[ADMIN] ERROR: Couldn't load scripting engine (amx_GetString)\n");
    exit(1);
  }
  
  BOOL bSymbolLoaded = FALSE;
  h_Library = 0;
#ifndef USE_METAMOD
#ifdef _WIN32
  h_Library = LoadLibrary(dll_name); 
#else
  h_Library = dlopen(dll_name, RTLD_NOW); 
#endif
  UTIL_LogPrintf( "[ADMIN] Opening dll:%s\n", dll_name);
  if (h_Library == NULL) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Failed to load DLL\n");
    exit(1);
  }
  other_GetEntityAPI = (GETENTITYAPI)GetProcAddress(h_Library, "GetEntityAPI");
  if (other_GetEntityAPI == NULL) {
    UTIL_LogPrintf( "[ADMIN] ERROR: no getentityapi stuff\n\n");
    UTIL_LogPrintf( "[ADMIN] ERROR: Failed to load DLL\n");
    exit(1);
  }
  bSymbolLoaded = TRUE;
  other_GetEntityAPI2 = (GETENTITYAPI2)GetProcAddress(h_Library, "GetEntityAPI2");
  if (other_GetEntityAPI2 == NULL) {
    UTIL_LogPrintf( "[ADMIN] No getentityapi2 stuff (sdk2.0).  This can be ignored.\n\n");
  }
  
  other_GetNewDLLFunctions = (GETNEWDLLFUNCTIONS)GetProcAddress(h_Library,
"GetNewDLLFunctions"); 
  if (other_GetNewDLLFunctions == NULL)  { 
    UTIL_LogPrintf( "[ADMIN] No GetNewDLLFunctions stuff (sdk2.0).  This can be ignored.\n\n");
  }
  
  other_GiveFnptrsToDll = (GIVEFNPTRSTODLL)GetProcAddress(h_Library, "GiveFnptrsToDll");
  if (other_GiveFnptrsToDll == NULL) {
    UTIL_LogPrintf( "[ADMIN] ERROR: no fntptodlls stuff\n\n");
    UTIL_LogPrintf( "[ADMIN] ERROR: Failed to load DLL\n");
    exit(1);
  }
#endif

#ifdef CS_TEAMINFO
  if ( strcmp(mod_name, "cstrike") == 0 ) {
      UTIL_LogPrintf( "[ADMIN] INFO: Loading extended Counter-Strike functionality.\n");


    if ( h_Library == 0 ) {
#ifdef _WIN32
      h_Library = LoadLibrary(dll_name); 
#else
      h_Library = dlopen(dll_name, RTLD_NOW); 
#endif
    }  // if
    
    if (h_Library == NULL) {
      UTIL_LogPrintf( "[ADMIN] INFO: Unable to load Counter-Strike DLL for extended functionality.\n");
    } else {

      char* pcError = 0;
      g_pGameRules = (CGameRules*)GetProcAddress( h_Library, "g_pGameRules" );
      if ( g_pGameRules == 0 ) {
        UTIL_LogPrintf( "[ADMIN] INFO: No GameRules found. This can be ignored\n");
      } else {
        bSymbolLoaded = TRUE;
      }  // if-else
      myCountTeams = (FIV) GetProcAddress( h_Library, "CountTeams__Fv" );
      if ( myCountTeams == 0 ) {
        UTIL_LogPrintf( "[ADMIN] INFO: Could not attach to CountTeams(). This can be ignored.\n");
      } else {
        bSymbolLoaded = TRUE;
      }  // if-else
      myCountTeamPlayers = (FII)GetProcAddress( h_Library, "CountTeamPlayers__Fi" );
      if ( myCountTeamPlayers == 0 ) {
        UTIL_LogPrintf( "[ADMIN] INFO: Could not attach to CountTeamPlayers(). Team counts may be inaccurate.\n");
      } else {
        bSymbolLoaded = TRUE;
      }  // if-else

      if ( !bSymbolLoaded ) {
#ifdef WIN32
#else
        dlclose( h_Library );
#endif
      }  // if
    }  // if-else
  } // if
#endif

  
  memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
  gpGlobals = pGlobals;
  // give my version of the engine functions to the other DLL...
#ifdef USE_METAMOD
  memset(&my_engfuncs, 0, sizeof(my_engfuncs));
#else /* not USE_METAMOD */
  memcpy(&my_engfuncs,&g_engfuncs,sizeof(enginefuncs_t));
#endif
  my_engfuncs.pfnFindEntityInSphere=am_FindEntityInSphere;
  my_engfuncs.pfnEntitiesInPVS =am_EntitiesInPVS;
  my_engfuncs.pfnFindEntityByVars =am_FindEntityByVars; 
#ifdef WITH_LOGPARSING
  my_engfuncs.pfnAlertMessage = am_AlertMessage;
#endif
 // You can override any engine functions here
  //my_engfuncs.pfnGetGameDir=GetGameDir;
#ifndef USE_METAMOD
  other_GiveFnptrsToDll(&my_engfuncs, pGlobals);
#endif
}

