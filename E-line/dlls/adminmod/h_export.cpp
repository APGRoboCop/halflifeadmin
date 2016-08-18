/*
 * ===========================================================================
 *
 * $Id: h_export.cpp,v 1.10 2003/11/08 10:21:23 darope Exp $
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
 */


#include <stdio.h>			// vsnprintf, etc
#include "extdll.h"
#include "users.h"
#include "amutil.h"
#include "amlibc.h"
#include "version.h"
#include "events.h"


#define SDK_UTIL_H      // extdll.h already #include's util.h
#include "meta_api.h"


/*
#ifdef _WIN32
#define DLLEXPORT (__declspec ( export ))
#else
#define DLLEXPORT
#endif
*/

enginefuncs_t  g_engfuncs;
globalvars_t *gpGlobals;
CMessageData s_MessageData;
          
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


// CS Timelimit var
DLL_GLOBAL float* g_pflTimeLimit = NULL;



// this structure contains a list of supported mods and their dlls names
// To add support for another mod add an entry here, and add all the 
// exported entities to link_func.cpp

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
HINSTANCE CS_Library = NULL;
#else
void *h_Library=NULL;
void *h_AMX=NULL;
void *CS_Library=NULL;
#include <dlfcn.h>
#define GetProcAddress dlsym    
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


#ifdef WITH_LOGPARSING 
/* Engine functions we need to catch */
void am_AlertMessage(ALERT_TYPE atype, char *szFmt, ...) {

  plugin_result eResult = PLUGIN_CONTINUE;
  int iParse = (int)CVAR_GET_FLOAT("admin_parse_logs");
  // If we're using metamod, we don't need to worry about sending
  // the alert message on to the other DLL; MM does that for us.
  // If we're not using metamod, we _do_ need to do this.
  if (iParse == 0) {
    RETURN_META(MRES_IGNORED);
  }



  char buf[MAX_STRBUF_LEN];
  va_list ap;
  va_start(ap, szFmt);
  vsnprintf(buf, MAX_STRBUF_LEN, szFmt, ap);
  va_end(ap);
  if (iParse != 0) {
    eResult = HandleLog(buf);
  }


  if (eResult == PLUGIN_HANDLED) {
    RETURN_META(MRES_HANDLED);
  } else {
    RETURN_META(MRES_IGNORED);
  }

}
#endif


edict_t *am_FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, 
                float rad)
{
  edict_t *ent;
  /* When using metamod, this function should be passed as a "_Post"
   * function, and metamod handles the initial engine call. */ 

        ent=META_RESULT_ORIG_RET(edict_t *); 

  /* If this found the timer ent, don't let game DLL have this; find the
   * next ent. */


  if(ent==pTimerEnt) {
    DEBUG_LOG( 5, ("Hiding timer entity from FindEntityInSphere.") );
    ent=FIND_ENTITY_IN_SPHERE(ent, org, rad);
    DEBUG_LOG( 5, ("Returning next entity: %s.", ent ? STRING(ent->v.classname) : "nil") );

   RETURN_META_VALUE(MRES_OVERRIDE, ent);

  }

  RETURN_META_VALUE(MRES_IGNORED, ent);



}



/* Engine functions we need to catch */
edict_t *am_EntitiesInPVS(edict_t *pplayer)
{
  edict_t *ent;
  /* When using metamod, this function should be passed as a "_Post"
   * function, and metamod handles the initial engine call. */ 


        ent=META_RESULT_ORIG_RET(edict_t *); 




  /* If this found the timer ent, don't let game DLL have this; find the
   * next ent. */


  if(ent==pTimerEnt) {

    DEBUG_LOG( 5, ("Hiding timer entity from FindEntityInPVS.") );
    ent=UTIL_EntitiesInPVS(ent);
    DEBUG_LOG( 5, ("Returning next entity: %s.",ent ? STRING(ent->v.classname) : "nil") );

   RETURN_META_VALUE(MRES_OVERRIDE, ent);

  }

  RETURN_META_VALUE(MRES_IGNORED, ent);



}



/* Engine functions we need to catch */
edict_t *am_FindEntityByVars(struct entvars_s *pvars)
{
  edict_t *ent;
  /* When using metamod, this function should be passed as a "_Post"
   * function, and metamod handles the initial engine call. */ 

 
        ent=META_RESULT_ORIG_RET(edict_t *); 
 
  


  /* If this found the timer ent, don't let game DLL have this; find the
   * next ent. */


  if(ent==pTimerEnt) {
    DEBUG_LOG( 5, ("Hiding timer entity from FindEntityByVars.") );
    ent=NULL;
    DEBUG_LOG( 5, ("Returning next entity: %s.",ent ? STRING(ent->v.classname) : "nil") );

   RETURN_META_VALUE(MRES_OVERRIDE, ent);

  }

  RETURN_META_VALUE(MRES_IGNORED, ent);

  

}

void am_MessageBegin( int msg_dest, int msg_type, const float *pOrigin, edict_t *ed )
{
	g_EventDispatcher.EventStart( msg_type, msg_dest, ed);
	//g_pEventManager->StartEvent( msg_type, ed, msg_dest );
	RETURN_META(MRES_IGNORED);
}

void am_MessageEnd( void )
{
	g_EventDispatcher.EventEnd();
	//g_pEventManager->FireEvent();
	RETURN_META(MRES_IGNORED);
}

void am_WriteGenericInt( int iValue )
{
	s_MessageData.m_Type = meInteger;
	s_MessageData.m_nValue = iValue;
	g_EventDispatcher.EventData(&s_MessageData);
	RETURN_META(MRES_IGNORED);
}

void am_WriteGenericFloat(float fValue )
{
	s_MessageData.m_Type = meFloat;
	s_MessageData.m_fValue = fValue;
	g_EventDispatcher.EventData(&s_MessageData);
	RETURN_META(MRES_IGNORED);
}

void am_WriteString( const char *sz )
{
	s_MessageData.m_Type = meString;
	s_MessageData.m_szValue = sz;
	g_EventDispatcher.EventData(&s_MessageData);
	RETURN_META(MRES_IGNORED);
}


//
// CVars registered here
//
extern cvar_t admin_devel;
extern cvar_t admin_debug;
extern cvar_t admin_bot_protection;
//extern cvar_t amv_autoban;
extern cvar_t reserve_slots;
extern cvar_t amv_hide_reserved_slots;
//extern cvar_t amv_keyfiles;


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
  CVAR_REGISTER(&admin_devel);
  CVAR_REGISTER(&admin_bot_protection);
  CVAR_REGISTER(&admin_debug);
  //CVAR_REGISTER(&amv_autoban);
  CVAR_REGISTER(&reserve_slots);
  CVAR_REGISTER(&amv_hide_reserved_slots);
  //CVAR_REGISTER(&amv_keyfiles);
  ptAM_devel = 0;
  ptAM_devel = CVAR_GET_POINTER( "admin_devel" );
  ptAM_debug = 0;
  ptAM_debug = CVAR_GET_POINTER( "admin_debug" );
  //ptAM_autoban = 0;
  //ptAM_autoban = CVAR_GET_POINTER( "amv_autoban" );
  ptAM_botProtection = 0;
  ptAM_botProtection = CVAR_GET_POINTER( "admin_bot_protection" );
  ptAM_reserve_slots = 0;
  ptAM_reserve_slots = CVAR_GET_POINTER( "reserve_slots" );
  ptAM_hide_reserved_slots = 0;
  ptAM_hide_reserved_slots = CVAR_GET_POINTER( "amv_hide_reserved_slots" );
  //ptAM_keyfiles = 0;
  //ptAM_keyfiles = CVAR_GET_POINTER( "amv_keyfiles" );


  // Greetings, earthling!
  UTIL_LogPrintf( "[ADMIN] Admin Mod, the HLDS admin's choice\n");
  UTIL_LogPrintf( "[ADMIN] by Alfred Reynolds (http://www.adminmod.org)\n");
  UTIL_LogPrintf( "[ADMIN] Version: %s\n", MOD_VERSION);
  UTIL_LogPrintf( "[ADMIN] Compiled: %s %s (%s)\n", COMPILE_DTTM, TZONE,  OPT_TYPE );
  /*
  // If we haven't read server.cfg yet, allow devel logging
  // to be enabled with the "+developer" cmdline option.
  if ( ((int) CVAR_GET_FLOAT("developer") != 0) && ((int) CVAR_GET_FLOAT("amv_devel") == 0) ) {
    CVAR_SET_FLOAT("amv_devel", 2.0);
    DEVEL_LOG(2, ("+developer was set, enabling devel logging level 2"));
  }  // if
  */ 


  char dll_name[2048];
  char mod_name[32];

#ifndef EXTMM
  GET_GAME_DIR(game_dir);
  am_strncpy(mod_name,  GetModDir(), 32);

  if ( strcasecmp(mod_name, "cstrike") == 0 ) {
#  ifdef _WIN32
	  snprintf( dll_name, 2048, "%s\\dlls\\mp.dll", game_dir );
#  else
	  snprintf( dll_name, 2048, "%s/dlls/cs_i386.so", game_dir );
#  endif
  }  // if

#endif
  
  
  BOOL bSymbolLoaded = FALSE;
  h_Library = 0;


#define EXTDEBUG
#if (! defined EXTMM)

  if ( strcmp(mod_name, "cstrike") == 0 ) {
#  if (defined LINUX) || (defined EXTDEBUG)
      UTIL_LogPrintf( "[ADMIN] INFO: Loading extended Counter-Strike functionality.\n" );
#  endif

    if ( CS_Library == 0 ) {
#  ifdef _WIN32
      CS_Library = LoadLibrary(dll_name); 
#  else
      CS_Library = dlopen(dll_name, RTLD_NOW); 
#  endif
    }  // if


    if ( CS_Library == NULL) {
#  if (defined LINUX) || (defined EXTDEBUG)
      UTIL_LogPrintf( "[ADMIN] INFO: Unable to load Counter-Strike DLL for extended functionality.\n");
#  endif
    } else {

      char* pcError = 0;

      g_pflTimeLimit = (float*)GetProcAddress( CS_Library, "g_flTimeLimit" );
      if ( g_pflTimeLimit == NULL ) {
#  if (defined LINUX) || (defined EXTDEBUG)
        UTIL_LogPrintf( "[ADMIN] INFO: Could not find CS' TimeLimit. AM's timeleft may differ from CS' timeleft.\n");
#  endif  // LINUX
      } else {
        bSymbolLoaded = TRUE;
#  if (defined LINUX) || (defined EXTDEBUG)
		UTIL_LogPrintf( "[ADMIN] INFO: Successfully attached to CS TimeLimit.\n" );
#  endif  // LINUX
      }  // if-else


#  ifdef CS_TEAMINFO
      g_pGameRules = (CGameRules*)GetProcAddress( CS_Library, "g_pGameRules" );
      if ( g_pGameRules == 0 ) {
#  if (defined LINUX) || (defined EXTDEBUG)
        UTIL_LogPrintf( "[ADMIN] INFO: No GameRules found. This can be ignored\n");
#  endif
      } else {
        bSymbolLoaded = TRUE;
      }  // if-else
      myCountTeams = (FIV) GetProcAddress( CS_Library, "CountTeams__Fv" );
      if ( myCountTeams == 0 ) {
#  if (defined LINUX) || (defined EXTDEBUG)
        UTIL_LogPrintf( "[ADMIN] INFO: Could not attach to CountTeams(). This can be ignored.\n");
#  endif
      } else {
        bSymbolLoaded = TRUE;
      }  // if-else
      myCountTeamPlayers = (FII)GetProcAddress( CS_Library, "CountTeamPlayers__Fi" );
      if ( myCountTeamPlayers == 0 ) {
#  if (defined LINUX) || (defined EXTDEBUG)
        UTIL_LogPrintf( "[ADMIN] INFO: Could not attach to CountTeamPlayers(). Team counts may be inaccurate.\n");
#  endif
      } else {
        bSymbolLoaded = TRUE;
      }  // if-else
#  endif // CS_TEAMINFO


      if ( !bSymbolLoaded ) {
#  ifdef WIN32
        FreeLibrary( CS_Library );
#  else
        dlclose( CS_Library );
#  endif
#  if (defined LINUX) || (defined EXTDEBUG)
		UTIL_LogPrintf( "[ADMIN] INFO: Could not enable extended CS functions. Standard Admin Mod functionality applies.\n" );
#  endif
      }  // if
    }  // if-else
  } // if
#endif // !EXTMM

  
  memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
  gpGlobals = pGlobals;
  // give my version of the engine functions to the other DLL...

  memset(&my_engfuncs, 0, sizeof(my_engfuncs));

  

  my_engfuncs.pfnFindEntityInSphere=am_FindEntityInSphere;
  my_engfuncs.pfnEntitiesInPVS =am_EntitiesInPVS;
  my_engfuncs.pfnFindEntityByVars =am_FindEntityByVars; 
#ifdef WITH_LOGPARSING
  my_engfuncs.pfnAlertMessage = am_AlertMessage;
#endif

  //Only hook these if event handling is enabled
	UTIL_LogPrintf( "[ADMIN] Experimental event handling features enabled\n");
	my_engfuncs.pfnMessageBegin = am_MessageBegin;
	my_engfuncs.pfnMessageEnd = am_MessageEnd;
	my_engfuncs.pfnWriteByte = am_WriteGenericInt;
	my_engfuncs.pfnWriteChar = am_WriteGenericInt;
	my_engfuncs.pfnWriteShort = am_WriteGenericInt;
	my_engfuncs.pfnWriteLong = am_WriteGenericInt;
	my_engfuncs.pfnWriteAngle = am_WriteGenericFloat;
	my_engfuncs.pfnWriteCoord = am_WriteGenericFloat;
	my_engfuncs.pfnWriteString = am_WriteString;
	my_engfuncs.pfnWriteEntity = am_WriteGenericInt;

}

