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
*   $Id: extdll.h,v 1.17 2004/08/19 22:23:02 the_wraith Exp $
*
****/
#ifndef EXTDLL_H
#define EXTDLL_H

#ifndef VERSION
#  include "resource.h"
#  define VERSION RC_VERSION
#  undef MOD_VERSION
#endif

#ifdef BETA
#  define BVERSION VERSION "-" BETA
#else
#  define BVERSION VERSION
#endif

#ifdef USE_METAMOD
#  ifdef USE_MYSQL
#    define MOD_VERSION BVERSION " (MM,mysql)"
#  else
#    ifdef USE_PGSQL
#      define MOD_VERSION BVERSION " (MM,pgsql)"
#    else
#      define MOD_VERSION BVERSION " (MM)"
#    endif
#  endif
#else
#  ifdef USE_MYSQL
#    define MOD_VERSION BVERSION " (mysql)"
#  else
#    ifdef USE_PGSQL
#      define MOD_VERSION BVERSION " (pgsql)"
#    else
#      define MOD_VERSION BVERSION
#    endif
#  endif
#  define WITH_LOGPARSING
#endif
#define MENU_SHOW 30 // how long to show menu's for (in sec)

#ifdef USE_MYSQL
  #include <mysql.h>
  #include <errmsg.h>
#endif

#ifdef USE_PGSQL
  #include <libpq-fe.h>
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
// disabled
//#define CS_TEAMINFO

// Silence certain warnings
#pragma warning(disable : 4244)		// int or float down-conversion
#pragma warning(disable : 4305)		// int or float data truncation
#pragma warning(disable : 4201)		// nameless struct/union
//#pragma warning(disable : 4514)		// unreferenced inline function removed
#pragma warning(disable : 4100)		// unreferenced formal parameter
#pragma warning(disable : 4018)		// signed/unsigned mismatch


#ifdef _WIN32
//#  define snprintf _snprintf
//#  define vsnprintf _vsnprintf
#  define HAS_SNPRINTF
#  define sleep(x)	Sleep(x*1000)
#  define strcasecmp	stricmp
#  define strncasecmp	_strnicmp
#else
#  define HAS_SNPRINTF
#endif


/* LINUX COMPILE */
#ifdef _WIN32    

  // Prevent tons of unused windows definitions
  #define WIN32_LEAN_AND_MEAN
  #define NOWINRES
  #define NOSERVICE
  #define NOMCX
  #define NOIME

  #define _WIN32_WINNT 0x0400
  #include "WINDOWS.H"
  #include "winbase.h"

  // Misc C-runtime library headers
  #include "STDIO.H"
  #include "STDLIB.H"
  #include "MATH.H"

#else
  #include <stdarg.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <math.h>
  #include <string.h>
  #include <ctype.h>
  #include <signal.h> 
  #include <unistd.h> 

  #define ULONG ulong
  #define FALSE 0
  #define TRUE  1

  //Fix for GCC 7 - [APG]RoboCop[CL]
  #include <algorithm> 
/*
  #ifndef max
    #define max(a,b)    (((a) > (b)) ? (a) : (b))
  #endif           

  #ifndef min
    #define min(a,b)    (((a) < (b)) ? (a) : (b))
  #endif

  #undef max
  #undef min

  using namespace std;
*/
  #define itoa(a,b,c) sprintf(b, "%d", a)

  typedef unsigned char BYTE;
  typedef int BOOL;
#endif
/* END LINUX COMPILE */        


// Header file containing definition of globalvars_t and entvars_t
//typedef int	 func_t;					//
//typedef int string_t;				// from engine's pr_comp.h;
typedef float vec_t;				// needed before including progdefs.h

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

#ifdef LINUX
#  include <limits.h>
#endif
#define SDK_UTIL_H
#include "osdep.h" // Metamod's PATH_MAX
#ifndef PATH_MAX
#  define PATH_MAX 2048
#endif


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


#include <ctime>
#include <cctype>  /* isprint() */

// scripting stuff
#include "amx.h"

// the CTimer class
#include "timer.h"

// Shared engine/DLL constants

#define DLL_GLOBAL    
#define CMD_ARGV                              (*g_engfuncs.pfnCmd_Argv)  
#define CMD_ARGS                                       (*g_engfuncs.pfnCmd_Args)
//#define SERVER_COMMAND (*g_engfuncs.pfnServerCommand)  
#define ALERT                  (*g_engfuncs.pfnAlertMessage)
#define FREE_FILE                              (*g_engfuncs.pfnFreeFile)
#define LOAD_FILE_FOR_ME               (*g_engfuncs.pfnLoadFileForMe)
#define CVAR_GET_STRING        (*g_engfuncs.pfnCVarGetString) 

#define ASSERT(f)
#define ASSERTSZ(f, sz)    

//#define STRING(offset)           (char *)(gpGlobals->pStringBase + (int)offset) 
//#define STRING(offset)		(const char *)(gpGlobals->pStringBase + (int)offset)

#define MESSAGE_END            (*g_engfuncs.pfnMessageEnd)  
#define CLIENT_PRINTF_ENG             (*g_engfuncs.pfnClientPrintf) 
#undef  CLIENT_PRINTF
#define CLIENT_PRINTF             ClientPrintf 
#undef  CLIENT_COMMAND
#define CLIENT_COMMAND  ClientCommand
#define CLIENT_COMMAND_ENG     (*g_engfuncs.pfnClientCommand)
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
#define FIND_ENTITY_BY_VARS    (*g_engfuncs.pfnFindEntityByVars)  
#define GETPLAYERUSERID        (*g_engfuncs.pfnGetPlayerUserId) 
#define CMD_ARGC               (*g_engfuncs.pfnCmd_Argc)
#define CVAR_GET_FLOAT         (*g_engfuncs.pfnCVarGetFloat)
#define GETPLAYERWONID         (*g_engfuncs.pfnGetPlayerWONId)
#define GETPLAYERAUTHID        (*g_engfuncs.pfnGetPlayerAuthId)
#define NUMBER_OF_ENTITIES     (*g_engfuncs.pfnNumberOfEntities)  
#define REG_SVR_COMMAND        (*g_engfuncs.pfnAddServerCommand)
#define PRECACHE_GENERIC       (*g_engfuncs.pfnPrecacheGeneric)
#define FORCE_UNMODIFIED       (*g_engfuncs.pfnForceUnmodified)


/*******************************************************************
  SOME UGLY GLOBAL VARIABLES HOPEFULLY TO BE REPLACED WITH A DECENT STRUCT
******************************************************************/

extern int g_NameCrashAction;
extern int g_PreventClientExploit;
extern int g_SpectatorCheatAction;
extern int g_msgTextMsg;




// users.cpp function prototypes
void LoadIPs(void);
void UnloadIPs(void);
void LoadModels(void);
void UnloadModels(void);
void LoadUsers(void);
void UnloadUsers(void);
void LoadVault(void);
void UnloadVault(void);
void LoadWords(void);
void UnloadWords(void);
BOOL IsIPReserved(char *);
BOOL IsIPValid(const char* );

/**************************************************************/
// UTIL.CPP function prototypes
/**************************************************************/
void UTIL_LogPrintf( char *fmt, ... );
void UTIL_LogPrintfFNL( char *fmt, ... );
//void UTIL_ClientPrintAll( int msg_dest, const char *msg_name);
char* UTIL_VarArgs( char *format, ... );
CBaseEntity* UTIL_PlayerByIndex( int playerIndex );
CBaseEntity* UTIL_PlayerByName( const char *name );
void ClientPrintf ( edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg );
void ClientCommand (edict_t* pEdict, char* szFmt, ...);

void fix_string(char *str,int len);
long int get_option_cvar_value( const char* CvarName, const char* Option, long int NumDefault, char* ReturnString, size_t Len, const char* StringDefault = NULL );
static inline long int get_option_cvar_value( const char* CvarName, const char* Option, char* ReturnString, size_t Len ) {
	return get_option_cvar_value( CvarName, Option, 0, ReturnString, Len, NULL );
}
static inline long int get_option_cvar_value( const char* CvarName, const char* Option, long int NumDefault ) {
	return get_option_cvar_value( CvarName, Option, NumDefault, NULL, 0, NULL );
}
static inline long int get_option_cvar_value( const char* CvarName, const char* Option ) {
	return get_option_cvar_value( CvarName, Option, 0, NULL, 0, NULL );
}

/* This one is added to check against atempted exploits of the client
 * via Admin Mod commands. It can be disabled because it may have some
 * unacceptable performance hit on slower machines. To disable it, set
 * prex0 in amv_anti_cheat_options.
 */  
static inline void handle_unprintables( char* _pcString ) {  
	if ( g_PreventClientExploit != 0 ) {
		while ( *_pcString != '\0' ) {
   			if ( !isprint(*_pcString) ) *_pcString = '.';
		   ++_pcString;	
		}  // while
	}  // if  
}  // handle_unprintables() 

int get_file_path( char* Path, char* Filename, int MaxLen, const char* AccessCvarName );
int GetPlayerIndex(char* PlayerText);
char* GetModDir();
int GetPlayerCount( edict_t* pIgnorePlayer = NULL );
//BOOL IsPlayerValid(CBaseEntity* pPlayer);
int get_player_team( CBaseEntity* );
int util_kick_player( int sessionID, const char* kickMessage );
int util_kick_player( edict_t* playerEdict, const char* kickMessage );
void util_kill_player( CBaseEntity* );

#ifdef USE_MYSQL
MYSQL_RES *admin_mysql_query(char *,char *);
const char* am_mysql_password_encrypt( const char* );
#endif

#ifdef USE_PGSQL
PGresult *admin_pgsql_query(char *, char *);
#endif



// Admin Mod base functions prototypes
const int RESULT_CONTINUE = 0;
const int RESULT_HANDLED  = 1;
void AM_AdminCommand(void);
int AM_ClientCommand( edict_t *pEntity );
BOOL AM_ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ], bool );
BOOL AM_ClientConnect_Post( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
int AM_ClientDisconnect( edict_t *pEntity );
int AM_StartFrame( void );
void AM_ClientStart(edict_t *pEntity);
void AM_Stop( void );
int AM_PlayerPreThink(edict_t *pEntity);
int AM_ClientPutInServer( edict_t* pEntity );
int AM_ServerDeactivate( void );
int AM_ClientUserInfoChanged( edict_t *pEntity, char *infobuffer, bool force );
int AM_GetGameDescription( const char* Description );
int AM_InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message );
int AM_DispatchThink( edict_t *pent );
int AM_GameDLLInit( void );
int AM_Initialize();
int AM_OnFreeEntPrivateData( edict_t *pent );
void KickHighestPinger( const char *pszName,char *ip,edict_t *pEntity);
void* LoadScript(AMX *amx, const char *filename);

typedef struct {
	char *mod,*windir,*linuxdir;
} mod_struct_type;

/*******************************************************************
New map stuff

********************************************************************/

#define MAX_RULE_BUFFER 1024
#define MAP_NAME_LENGTH 32
typedef struct mapcycle_item_s
{
  struct mapcycle_item_s *next;
  
  char mapname[ MAP_NAME_LENGTH ];
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
int check_map(char *map,int bypass_allowed_map);
int listmaps(edict_t *);
mapcycle_item_s *CurrentMap(mapcycle_t *cycle);
int  match(const char *string, char *pattern);


void AMXAPI core_Init(void);   /* two functions from AMX_CORE.C */
void AMXAPI core_Exit(void);
void ShowMenu (edict_t*, int, int, BOOL, char pszText[1024]);
void ShowMenu_Large (edict_t* pev, int bitsValidSlots, int nDisplayTime, char pszText[]);
void ShowMOTD( edict_t* pev, const char* msg);



/*******************************************************************
  INLINE FUNCTIONS
********************************************************************/


// Returns TRUE if a given CBaseEntity pointer points to a valid, connected player,
// FALSE otherwise
inline BOOL IsPlayerValid(CBaseEntity* pPlayer) {
  return ( (pPlayer) && (!FNullEnt(pPlayer->pev)) && (GETPLAYERUSERID(pPlayer->edict()) > 0) && (!FStrEq(STRING(pPlayer->pev->netname),"")) ) ? TRUE : FALSE;
}


// Returns TRUE if a given edict_t pointer points to a valid, connected player,
// FALSE otherwise
inline BOOL IsPlayerValid(edict_t* pPlayer) {
  return ( (pPlayer) && (!FNullEnt(pPlayer)) && (GETPLAYERUSERID(pPlayer) > 0) && (!FStrEq(STRING(pPlayer->v.netname),"")) ) ? TRUE : FALSE;
}






///////////////////////////////////////
///
/// \brief Check if a cvar specifying a string is set
///
/// This function takes a string which is the value of a cvar used to
/// set a string value and checks if it is set. A cvar is set if it exists,
/// the value is not an empty string and is not the string "0".
///
/// If the AllowEmptyString parameter is set to "true" a cvar is considered set
/// even if the cvar value string is empty, i.e. "".
///
//////////////////////////////////////

inline bool cvar_string_value_is_set( const char* _pcCvarValueString, bool _bAllowEmptyString = false ) {

	// if the cvar doesn't exist, i.e. the char pointer passed is NULL, return false
	if ( _pcCvarValueString == NULL ) return false;

	// if the string is empty and we do not allow this case, return false
	if ( *_pcCvarValueString == '\0' && !_bAllowEmptyString ) return false;

	// if the string equals "0", return false
	if ( *_pcCvarValueString == '0' && *(_pcCvarValueString+1) == '\0' ) return false;

	// Otherwise the cvar is set, return true
	return true;
}
 



///////////////////////////////////////
///
/// \brief Check if a cvar specifying a string is set
///
/// This function takes a cvar name used to set a string value 
/// and checks if it is set. A cvar is set if it exists,
/// the value is not an empty string and is not the string "0".
///
/// If the AllowEmptyString parameter is set to "true" a cvar is considered set
/// even if the cvar value string is empty, i.e. "".
///
//////////////////////////////////////

inline bool cvar_string_is_set( const char* _pcCvarName, bool _bAllowEmptyString = false ) {
	
	// get the CVAR string
	const char* pcCvarValue = CVAR_GET_STRING( _pcCvarName );
	return cvar_string_value_is_set( pcCvarValue, _bAllowEmptyString );
}




///////////////////////////////////////
///
/// \brief Check if a cvar specifying a file name is set
///
/// This function takes a string which is the value of a cvar used to
/// set a file name and check if it is set. A cvar is set if it exists,
/// the value is not an empty string and is not the string "0".
///
/// \note
/// This is really only for readability reasons and calls cvar_string_is_set().
///
//////////////////////////////////////

inline bool cvar_file_value_is_set( const char* _pcCvarValueFilename ) {
	return cvar_string_value_is_set( _pcCvarValueFilename, false );
}




///////////////////////////////////////
///
/// \brief Check if a cvar specifying a file name is set
///
/// This function takes a cvar name used to set a file name 
/// and checks if it is set. A cvar is set if it exists,
/// the value is not an empty string and is not the string "0".
///
//////////////////////////////////////

inline bool cvar_file_is_set( const char* _pcCvarName ) {
	
	// get the CVAR string
	const char* pcCvarValue = CVAR_GET_STRING( _pcCvarName );
	return cvar_string_value_is_set( pcCvarValue, false );
}




/////////////////////////////////////
///
/// \brief Returns the value of a cvar specifying a string, 
///        NULL if the cvar is not set according to cvar_string_is_set().
///
/// This function gets passed the name of a cvar and returns it's value
/// as a const char*. If the cvar is not set as according to cvar_string_is_set()
/// the return value is NULL.
///
/// If the AllowEmptyString parameter is set to "true" a cvar is considered set
/// even if the cvar value string is empty, i.e. "".
///
////////////////////////////////////

inline const char* get_cvar_string_value( const char* _pcCvarName, bool _bAllowEmptyString = false ) {

	// get the CVAR string
	const char* pcCvarValue = CVAR_GET_STRING( _pcCvarName );
	if ( cvar_string_value_is_set(pcCvarValue, _bAllowEmptyString) ) return pcCvarValue;
	else return NULL;
}




/////////////////////////////////////
///
/// \brief Returns the value of a cvar specifying a file name, 
///        NULL if the cvar is not set according to cvar_string_is_set().
///
/// This function gets passed the name of a cvar and returns it's value
/// as a const char*. If the cvar is not set as according to cvar_string_is_set()
/// the return value is NULL.
///
/// \note
/// This is added really only for readability reasons.
///
////////////////////////////////////

inline const char* get_cvar_file_value( const char* _pcCvarName ) {

	// get the CVAR string
	const char* pcCvarValue = CVAR_GET_STRING( _pcCvarName );
	if ( cvar_file_value_is_set(pcCvarValue) ) return pcCvarValue;
	else return NULL;
}





/*******************************************************************

********************************************************************/

extern DLL_GLOBAL float* g_pflTimeLimit;
extern DLL_GLOBAL CGameRules* g_pGameRules;

#ifdef CS_TEAMINFO
typedef int (*FIV)(void);
typedef int (*FII)( int );
#endif

#endif //EXTDLL_H

