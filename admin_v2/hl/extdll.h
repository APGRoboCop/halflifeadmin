/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
*   $Id: extdll.h,v 1.24 2001/09/27 20:33:16 darope Exp $
*
****/
#ifndef _EXTDLL_H_
#define _EXTDLL_H_

/*
#  define WITH_LOGPARSING
*/


#ifdef USE_MYSQL
#  include <mysql.h>
#  include <errmsg.h>
#endif

//
// Global header file for extension DLLs
//

// Allow "DEBUG" in addition to default "_DEBUG"
#ifdef _DEBUG
// Since Adminmod's util.cpp doesn't include the DBG_EntOfVars and
// DBG_AssertFunction routines mentioned in the SDK util.h, we're _not_
// going to set DEBUG here, so that we can avoid the "unresolved symbol"
// errors when compiling MSVC "debug" target.
//
// #define DEBUG 1
#endif

// try to get the team info by tapping into the CS DLL
#define CS_TEAMINFO 

// Silence certain warnings
#pragma warning(disable : 4244)		// int or float down-conversion
#pragma warning(disable : 4305)		// int or float data truncation
#pragma warning(disable : 4201)		// nameless struct/union
#pragma warning(disable : 4514)		// unreferenced inline function removed
#pragma warning(disable : 4100)		// unreferenced formal parameter
#pragma warning(disable : 4018)		// signed/unsigned mismatch


#ifdef _WIN32
#  define snprintf _snprintf
#  define vsnprintf _vsnprintf
#  define HAS_SNPRINTF
#else
#  define HAS_SNPRINTF
#endif




/* LINUX COMPILE */
#ifdef _WIN32    

  // Prevent tons of unused windows definitions
#  define WIN32_LEAN_AND_MEAN
#  define NOWINRES
#  define NOSERVICE
#  define NOMCX
#  define NOIME

#  define _WIN32_WINNT 0x0400
#  include "WINDOWS.H"
#  include "winbase.h"

  // Misc C-runtime library headers
#  include "STDIO.H"
#  include "STDLIB.H"
#  include "MATH.H"

#else
#  include <stdarg.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <math.h>
#  include <string.h>
#  include <ctype.h>
#  include <signal.h> 
#  include <unistd.h> 

#  define ULONG ulong
#  define FALSE false
#  define TRUE  true


#  ifndef max
#    define max(a,b)    (((a) > (b)) ? (a) : (b))
#  endif           

#  ifndef min
#    define min(a,b)    (((a) < (b)) ? (a) : (b))
#  endif

#  define itoa(a,b,c) sprintf(b, "%d", a)

  typedef unsigned char BYTE;
  typedef int BOOL;
#endif
/* END LINUX COMPILE */        


// Header file containing definition of globalvars_t and entvars_t
typedef int	func_t;	       //
typedef int	string_t;      // from engine's pr_comp.h;
typedef float   vec_t;	       // needed before including progdefs.h

// Vector class
#include <vector.h>

// Defining it as a (bogus) struct helps enforce type-checking
#define vec3_t Vector

#include "const.h"
#include "progdefs.h"
#include "edict.h"


// Shared header describing protocol between engine and DLLs
#include "eiface.h"

// Shared header between the client DLL and the game DLLs
#include <cdll_dll.h>

// admin mods version!
#include <util.h>



//#include <saverestore.h>


#include <cbase.h>
//#include "includes/activity.h"
//#include "includes/activitymap.h"
//#include "includes/animation.h"
//#include <basemonster.h>

//#include "includes/cdll_dll.h"
//#include "includes/client.h"
//#include "includes/decals.h"
//#include "includes/doors.h"
//#include "includes/effects.h"
//#include "includes/enginecallback.h"
//#include "includes/explode.h"
//#include "includes/extdll.h"
//#include "includes/func_break.h"
//#include "includes/game.h"
#include <gamerules.h>
///#include "includes/hornet.h"
//#include "includes/items.h"
#include <maprules.h>      

//#include <monsterevent.h>
//#include <monsters.h>
//#include <nodes.h>
//#include <plane.h>
#include <player.h>
//
//#include "includes/schedule.h"
//#include "includes/scriptevent.h"
//#include "includes/skill.h"
//#include "includes/soundent.h"
//#include <spectator.h>
//#include <talkmonster.h>
//#include "includes/teamplay_gamerules.h"
//#include "includes/trains.h"
//#include "includes/users.h"

//#include "includes/vector.h"
//#include <weapons.h>        


#include <time.h>

// scripting stuff
#include "amx.h"

// the CTimer class
#include "timer.h"

#ifdef WIN32
#  define SCRIPT_DLL "dlls\\amx_admin.dll"
#else
#  define SCRIPT_DLL "dlls/amx_admin.so"
#endif


// Shared engine/DLL constants

#define DLL_GLOBAL    
#define CMD_ARGV               (*g_engfuncs.pfnCmd_Argv)  
#define CMD_ARGS               (*g_engfuncs.pfnCmd_Args)
#define SERVER_COMMAND         (*g_engfuncs.pfnServerCommand)  
#define ALERT                  (*g_engfuncs.pfnAlertMessage)
#define FREE_FILE              (*g_engfuncs.pfnFreeFile)
#define LOAD_FILE_FOR_ME       (*g_engfuncs.pfnLoadFileForMe)
#define CVAR_GET_STRING        (*g_engfuncs.pfnCVarGetString) 

#define ASSERT(f)
#define ASSERTSZ(f, sz)    

//#define STRING(offset)           (char *)(gpGlobals->pStringBase + (int)offset) 
//#define STRING(offset)		(const char *)(gpGlobals->pStringBase + (int)offset)

#define MESSAGE_END            (*g_engfuncs.pfnMessageEnd)  
#define CLIENT_PRINTF          (*g_engfuncs.pfnClientPrintf) 
#define IS_MAP_VALID           (*g_engfuncs.pfnIsMapValid)
#define CHANGE_LEVEL           (*g_engfuncs.pfnChangeLevel) 
#define WRITE_BYTE             (*g_engfuncs.pfnWriteByte)
#define WRITE_CHAR             (*g_engfuncs.pfnWriteChar)
#define WRITE_SHORT            (*g_engfuncs.pfnWriteShort)
#define WRITE_LONG             (*g_engfuncs.pfnWriteLong)
#define WRITE_ANGLE            (*g_engfuncs.pfnWriteAngle)
#define WRITE_COORD            (*g_engfuncs.pfnWriteCoord)
#define WRITE_STRING           (*g_engfuncs.pfnWriteString)
#define WRITE_ENTITY           (*g_engfuncs.pfnWriteEntity)  
#define CVAR_SET_FLOAT         (*g_engfuncs.pfnCVarSetFloat) 
#define CVAR_SET_STRING        (*g_engfuncs.pfnCVarSetString)  
#define ALLOC_STRING           (*g_engfuncs.pfnAllocString)  
#define CVAR_REGISTER          (*g_engfuncs.pfnCVarRegister) 
#define REG_USER_MSG           (*g_engfuncs.pfnRegUserMsg)
#define GET_GAME_DIR           (*g_engfuncs.pfnGetGameDir) 
#define FIND_ENTITY_BY_STRING  (*g_engfuncs.pfnFindEntityByString)  
#define GETPLAYERUSERID        (*g_engfuncs.pfnGetPlayerUserId) 
#define CMD_ARGC               (*g_engfuncs.pfnCmd_Argc)
#define CVAR_GET_FLOAT         (*g_engfuncs.pfnCVarGetFloat)
#define GETPLAYERWONID         (*g_engfuncs.pfnGetPlayerWONId)
#define NUMBER_OF_ENTITIES     (*g_engfuncs.pfnNumberOfEntities)  
#define PLAYER_CNX_STATS       (*g_engfuncs.pfnGetPlayerStats ) 
#define REG_SVR_COMMAND        (*g_engfuncs.pfnAddServerCommand)



// users.cpp function prototypes
void LoadIPs(void);
void LoadModels(void);
void LoadUsers(void);
void LoadVault(void);
void LoadWords(void);
BOOL IsIPReserved(char *);
BOOL IsIPValid(char* );


// util.cpp function prototypes
void UTIL_LogPrintf( char *fmt, ... );
void UTIL_LogPrintfFNL( char *fmt, ... );
//void UTIL_ClientPrintAll( int msg_dest, const char *msg_name);
char* UTIL_VarArgs( char *format, ... );
CBaseEntity* UTIL_PlayerByIndex( int playerIndex );
CBaseEntity* UTIL_PlayerByName( const char *name );
void fix_string(char *str,int len);

int get_file_path( char* Path, char* Filename, int MaxLen, const char* AccessCvarName );
int GetPlayerIndex(char* PlayerText);
char* GetModDir();
int GetPlayerCount();
int get_player_team( CBaseEntity* );
#ifdef USE_MYSQL
MYSQL_RES *admin_mysql_query(char *,char *);
#endif



// Admin Mod base functions prototypes
const int RESULT_CONTINUE = 0;
const int RESULT_HANDLED  = 1;
void AM_AdminCommand(void);
int AM_ClientCommand( edict_t *pEntity );
BOOL AM_ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
int AM_ClientDisconnect( edict_t *pEntity );
void AM_ClientStart(edict_t *pEntity);
int AM_ClientUserInfoChanged( edict_t *pEntity, char *infobuffer );
int AM_DispatchThink( edict_t *pent );
int AM_GameDLLInit( void );
int AM_Initialize();
int AM_OnFreeEntPrivateData( edict_t *pent );
void KickHighestPinger( const char *pszName,char *ip,edict_t *pEntity);
void* LoadScript(AMX *amx,char *filename);

typedef struct {
	char *mod,*windir,*linuxdir;
} mod_struct_type;

/*******************************************************************
New map stuff

********************************************************************/

#define MAX_RULE_BUFFER 1024

typedef struct mapcycle_item_s
{
  struct mapcycle_item_s *next;
  
  char mapname[ 32 ];
  int  minplayers, maxplayers;
  char rulebuffer[ MAX_RULE_BUFFER ];
} mapcycle_item_t;

typedef struct mapcycle_s
{
  struct mapcycle_item_s *items;
  struct mapcycle_item_s *next_item;
} mapcycle_t;


void DestroyMapCycle( mapcycle_t *cycle );
int ReloadMapCycleFile( char *filename, mapcycle_t *cycle );
int allowed_map(char *map);
int check_map(char *map);
int listmaps(edict_t *);
mapcycle_item_s *CurrentMap(mapcycle_t *cycle);
int  match(const char *string, char *pattern);


void AMXAPI core_Init(void);   /* two functions from AMX_CORE.C */
void AMXAPI core_Exit(void);
void ShowMenu (edict_t*, int, int, BOOL, char pszText[1024]);



/*******************************************************************
  INLINE FUNCTIONS
********************************************************************/


// Returns TRUE if a given CBaseEntity pointer points to a valid, connected player,
// FALSE otherwise
inline BOOL IsPlayerValid(CBaseEntity* pPlayer) {
  return ((pPlayer) && (!FNullEnt(pPlayer->pev)) && (GETPLAYERUSERID(pPlayer->edict()) > 0) && (!FStrEq(STRING(pPlayer->pev->netname),"")) ? TRUE : FALSE);
}


/*******************************************************************

********************************************************************/

extern DLL_GLOBAL CGameRules* g_pGameRules;

#ifdef CS_TEAMINFO
typedef int (*FIV)(void);
typedef int (*FII)( int );
#endif

#endif //EXTDLL_H

