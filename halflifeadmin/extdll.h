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
****/
#ifndef EXTDLL_H
#define EXTDLL_H

#define MOD_VERSION "0.84"
#define MENU_SHOW 20 // how long to show menu's for


//
// Global header file for extension DLLs
//

// Allow "DEBUG" in addition to default "_DEBUG"
#ifdef _DEBUG
#define DEBUG 1
#endif

// Silence certain warnings
#pragma warning(disable : 4244)		// int or float down-conversion
#pragma warning(disable : 4305)		// int or float data truncation
#pragma warning(disable : 4201)		// nameless struct/union
#pragma warning(disable : 4514)		// unreferenced inline function removed
#pragma warning(disable : 4100)		// unreferenced formal parameter


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


#ifndef max
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#endif           

#ifndef min
#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#define itoa(a,b,c) sprintf(b, "%d", a)

typedef unsigned char BYTE;
typedef int BOOL;
#endif
/* END LINUX COMPILE */        


// Header file containing definition of globalvars_t and entvars_t
typedef int	func_t;					//
typedef int	string_t;				// from engine's pr_comp.h;
typedef float vec_t;				// needed before including progdefs.h

// Vector class
#include "vector.h"

// Defining it as a (bogus) struct helps enforce type-checking
#define vec3_t Vector

#include "const.h"
#include "progs.h"

// Shared header describing protocol between engine and DLLs
#include "eiface.h"

// Shared header between the client DLL and the game DLLs
#include "cdll_dll.h"

#include "includes/util.h"
#include "includes/saverestore.h"


#include "includes/cbase.h"
//#include "includes/activity.h"
//#include "includes/activitymap.h"
//#include "includes/animation.h"
#include "includes/basemonster.h"

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
//#include "includes/gamerules.h"
///#include "includes/hornet.h"
//#include "includes/items.h"
//#include "includes/maprules.h"      

#include "includes/monsterevent.h"
#include "includes/monsters.h"
#include "includes/nodes.h"
#include "includes/plane.h"
#include "includes/player.h"
//
//#include "includes/schedule.h"
//#include "includes/scriptevent.h"
//#include "includes/skill.h"
//#include "includes/soundent.h"
#include "includes/spectator.h"
#include "includes/talkmonster.h"
//#include "includes/teamplay_gamerules.h"
//#include "includes/trains.h"
//#include "includes/users.h"

//#include "includes/vector.h"
#include "includes/weapons.h"        



#include <time.h>

// Shared engine/DLL constants

#define DLL_GLOBAL    
#define CMD_ARGV                              (*g_engfuncs.pfnCmd_Argv)  
#define CMD_ARGS                                       (*g_engfuncs.pfnCmd_Args)
#define SERVER_COMMAND (*g_engfuncs.pfnServerCommand)  
#define ALERT                  (*g_engfuncs.pfnAlertMessage)
#define FREE_FILE                              (*g_engfuncs.pfnFreeFile)
#define LOAD_FILE_FOR_ME               (*g_engfuncs.pfnLoadFileForMe)
#define CVAR_GET_STRING        (*g_engfuncs.pfnCVarGetString) 

#define ASSERT(f)
#define ASSERTSZ(f, sz)    
#define STRING(offset)           (char *)(gpGlobals->pStringBase + (int)offset) 
#define MESSAGE_END            (*g_engfuncs.pfnMessageEnd)  
#define CLIENT_PRINTF             (*g_engfuncs.pfnClientPrintf) 
#define IS_MAP_VALID                   (*g_engfuncs.pfnIsMapValid)
#define CHANGE_LEVEL   (*g_engfuncs.pfnChangeLevel) 
#define WRITE_BYTE             (*g_engfuncs.pfnWriteByte)
#define WRITE_CHAR             (*g_engfuncs.pfnWriteChar)
#define WRITE_SHORT            (*g_engfuncs.pfnWriteShort)
#define WRITE_LONG             (*g_engfuncs.pfnWriteLong)
#define WRITE_ANGLE            (*g_engfuncs.pfnWriteAngle)
#define WRITE_COORD            (*g_engfuncs.pfnWriteCoord)
#define WRITE_STRING   (*g_engfuncs.pfnWriteString)
#define WRITE_ENTITY   (*g_engfuncs.pfnWriteEntity)  
#define CVAR_SET_FLOAT (*g_engfuncs.pfnCVarSetFloat) 
#define CVAR_SET_STRING        (*g_engfuncs.pfnCVarSetString)  
#define ALLOC_STRING   (*g_engfuncs.pfnAllocString)  
#define CVAR_REGISTER  (*g_engfuncs.pfnCVarRegister) 
#define REG_USER_MSG                           (*g_engfuncs.pfnRegUserMsg)
#define GET_GAME_DIR                   (*g_engfuncs.pfnGetGameDir) 
#define FIND_ENTITY_BY_STRING  (*g_engfuncs.pfnFindEntityByString)  
#define GETPLAYERUSERID (*g_engfuncs.pfnGetPlayerUserId) 
#define CMD_ARGC            (*g_engfuncs.pfnCmd_Argc)
#define CVAR_GET_FLOAT (*g_engfuncs.pfnCVarGetFloat)
#define GETPLAYERWONID (*g_engfuncs.pfnGetPlayerWONId)
#define NUMBER_OF_ENTITIES             (*g_engfuncs.pfnNumberOfEntities)  



// users.cpp function prototypes
void do_admin_commands(edict_t *);
void load_users(void);
void load_nicks(void);
void load_models(void);
void load_ips(void);
int check_nick(char *,edict_t *);
int auth_user(char *,int);
int priority_ip(char *);
int check_model(char *,edict_t *);

// util.cpp function prototypes
void UTIL_LogPrintf( char *fmt, ... );
void UTIL_ClientPrintAll( int msg_dest, const char *msg_name);
char* UTIL_VarArgs( char *format, ... );
CBaseEntity     *UTIL_PlayerByIndex( int playerIndex );
CBaseEntity     *UTIL_PlayerByName( char *name );
void fix_string(char *str,int len);

typedef struct {
	char *mod,*windir,*linuxdir;
} mod_struct_type;


// user commands
void admin_help(edict_t *pEntity);
void admin_userlist(edict_t *pEntity);
void admin_nextmap(edict_t *pEntity);
void admin_vote_kick(edict_t *pEntity);
void admin_vote_map(edict_t *pEntity);
void admin_map(edict_t *pEntity);
void admin_pass(edict_t *pEntity);
void admin_nopass(edict_t *pEntity);
void admin_say(edict_t *pEntity);
void admin_friendlyfire(edict_t *pEntity);
void admin_teamplay(edict_t *pEntity);
void admin_fraglimit(edict_t *pEntity);
void admin_kick(edict_t *pEntity);
void admin_status(edict_t *pEntity);
void admin_cfg(edict_t *pEntity);
void admin_ban(edict_t *pEntity);
void admin_unban(edict_t *pEntity);
void admin_reload(edict_t *pEntity);
void admin_pause(edict_t *pEntity);
void admin_unpause(edict_t *pEntity);
void admin_version(edict_t *pEntity);
void admin_timelimit(edict_t *pEntity);
void admin_prematch(edict_t *pEntity);
void admin_timeleft(edict_t *pEntity);
void admin_restartround(edict_t *pEntity);
#endif //EXTDLL_H

