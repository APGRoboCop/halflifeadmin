/*
 * ===========================================================================
 *
 * $Id: users.h,v 1.14 2004/08/21 19:28:41 darope Exp $
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
 *  defines for the various access levels in the admin_ commands
 *
 */

//#ifdef USE_MYSQL
// Query to use for NON preloaded users check, takes 3 params
// 1: %s - the db name
// 2: %s - the nickname from the user
// 3: %i - the WONID of the user
// default sample is:"SELECT pass,access FROM %s where nick='%s' or nick='%i'\n"


//#define USERS_QUERY "SELECT pass,access FROM %s where nick='%s' or nick='%i'\n"
//#define USERS_QUERY "SELECT pass,access FROM %s where '%s' REGEXP nick or nick='%i'\n"
//#endif

#include "constants.h"
#include "authid.h"


// Declare external globals
extern int g_iForcedMapChange;
extern char* g_pcNextMap;
extern char g_acNextMap[BUF_SIZE];


struct auth_struct {
	enum { dirty = 0, clean };      // Possible player states
	int state;                      // State of the player: dirty: not put in server yet, clean: put in server
	char sUserName[USERNAME_SIZE];  // User name of the player
	char sPassword[PASSWORD_SIZE];  // Password the player used
	char sIP[IP_SIZE];              // IP of the player
	int iPort;                      // Port of the player
	int iSessionID;                 // Session ID of the player
	time_t iTime;                   // Time the player was auth'ed
	int iAccess;                    // Access the player has
	int iUserIndex;                 // Index of the user record match (from user_struct.iIndex; 0 for none)
	int iIndex;                     // Index of the player slot. Only for diagnostic purposes.
	edict_t* pPlayerEdict;          // Pointer to the player edict.
    AMAuthId oaiAuthID;             // ID used to auth a player on reconnect. Can be Wonid or Steamid.
};

void clear_auth_bak_array( void );


#define ulong unsigned long
typedef struct {
  char sIP[IP_SIZE + 1];          // IP address (as x.x.x.x string)
  char sMask[IP_SIZE + 1];        // Subnet mask (as x.x.x.x string)
  ulong lIP;                      // IP address (as bits)
  ulong lMask;                    // Subnet mask (as bits)
} ip_struct;

typedef struct {
  char sModelName[MODELNAME_SIZE]; // Model name to reserve
  char sPassword[PASSWORD_SIZE];   // Password to go along with name
} model_struct;

typedef struct {
  char szClassname[BUF_SIZE];     // Class name of the spawned entity
  int iIdentity;                  // Unique identity of the spawned entity
  CBaseEntity *pEntity;           // Actual spawned entity
} spawn_struct;

typedef struct {
  char sUserName[USERNAME_SIZE];  // User name.  May be a WON ID, but will always be a string.
  char sPassword[PASSWORD_SIZE];  // Password.
  int  iAccess;                   // Access.
  int  iIndex;                    // Index of the user record (1 based)
} user_struct;

typedef struct {
	char sKey[BUF_SIZE];            // Key (for lookup purposes)
	char sData[BUF_SIZE];           // Data associated with that key
} vault_struct;

typedef struct {
  char sWord[BUF_SIZE];           // Word up, G
} word_struct;

#if USE_MYSQL
	// Callback function; for each row returned by the loading query, call this
	// function with it
	#include <mysql.h>
  #include <errmsg.h>
	typedef BOOL (*PARSE_MYSQL)(MYSQL_ROW pRow);
#endif

#if USE_PGSQL
	#include <libpq-fe.h>
	typedef BOOL (*PARSE_PGSQL)(const PGresult *res, int tup_num);
#endif


enum uidt {uid_none = 0, uid_invalid, uid_index, uid_sessionID, uid_wonID};


#ifdef USE_METAMOD
#  ifdef WIN32
#    define SCRIPT_DLL "amx_admin.dll"
#  else
#    ifdef __amd64__
#      define SCRIPT_DLL "amx_admin_amd64.so"
#    else
#      define SCRIPT_DLL "amx_admin.so"
#    endif
#  endif
#else
#  ifdef WIN32
#    define SCRIPT_DLL_WIN "dlls\\amx_admin.dll"
#    define SCRIPT_DLL     "dlls/amx_admin.dll"
#  else
#    define SCRIPT_DLL     "dlls/amx_admin.so"
#  endif
#endif


#include "CPlugin.h"
#include "amx.h"

#ifdef _WIN32

typedef int (FAR *AMXINIT)(AMX *,void *);
typedef int (FAR *AMXREGISTER)(AMX *,AMX_NATIVE_INFO *,int);
typedef int (FAR *AMXFINDPUBLIC)(AMX *,char *,int *);
typedef int (FAR *AMXEXEC)(AMX *,cell *,int, int,...);
typedef int (FAR *AMXGETADDR)(AMX *,cell,cell **);
typedef int (FAR *AMXSTRLEN)(cell *,int *);
typedef int (FAR *AMXRAISEERROR)(AMX *,int);
typedef int (FAR *AMXGETSTRING)(char *,cell *);
typedef int (FAR *AMXSETSTRING)(cell *,char *,int);

#else

typedef int (*AMXINIT)(AMX *,void *);
typedef int (*AMXREGISTER)(AMX *,AMX_NATIVE_INFO *,int);
typedef int (*AMXFINDPUBLIC)(AMX *,char *,int *);
typedef int (*AMXEXEC)(AMX *,cell *,int, int,...);
typedef int (*AMXGETADDR)(AMX *,cell,cell **);
typedef int (*AMXSTRLEN)(cell *,int *);
typedef int (*AMXRAISEERROR)(AMX *,int);
typedef int (*AMXGETSTRING)(char *,cell *);
typedef int (*AMXSETSTRING)(cell *,char *,int);

#endif


// Callback function; for each line returned from the loaded file, call this
// function with it
typedef BOOL (*PARSE_FILE)(char* sLine);










void UTIL_ClientPrint_UR( entvars_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 );

int AddSpawnEntity(const char* szClassname, CBaseEntity* pEntity);
spawn_struct* FindSpawnEntity(int iIdentity);
void InitSpawnEntityList();
void DeleteSpawnEntityList();
void ListSpawnEntities(edict_t* pMsg, char* szFindClassname);
BOOL RemoveSpawnEntity(int iIdentity);
///CEM


// CEM - 02/03/01
BOOL AddHelpEntry(char* sCmd, char* sHelp, int iAccess = ACCESS_ALL);
BOOL LoadPlugins();
void UnloadPlugins( void );
int CheckCommand( edict_t* Entity, const char* Command, unsigned int& Access, BOOL CeckUserAcc = FALSE, BOOL PrintOut = FALSE );
plugin_result HandleCommand(edict_t* pEntity, char* sCmd, char* sData);
plugin_result HandleConnect(edict_t* pEntity, char* sName, char* IPAddress);
plugin_result HandleDisconnect(edict_t* pEntity);
plugin_result HandleHelp(edict_t* pEntity, char* sData, int iFormat);
plugin_result HandleInfo(edict_t* pEntity, char* sNewName);
plugin_result HandleLog(char* sLog);
plugin_result HandleVersion(edict_t* pEntity);
CPlugin* GetPlugin(AMX* amx);
///CEM

void System_Error(char* str, edict_t* pEntity);
void System_Response(char *str,edict_t *);

void AddUserAuth(char* sName, char* sIP, edict_t* pEntity);
int GetFreeSlots(edict_t* pEntityIgnore = NULL);
int GetHighlanderIndex( edict_t* IgnoreEntity = NULL );
int GetModelAccess(char* sModel, edict_t* pEntity);
int GetUserAccess(edict_t* pEntity);
BOOL GetUserRecord(const char* sName, const AMAuthId&, const char* IP, char* sPassword, user_struct* ptUserRecord);
BOOL GetModelRecord(char* sModel, char* sPassword, model_struct* tModelRecord);
char* GetVaultData(char* sKey);
void InitAdminModData(BOOL fFull, BOOL fRun);
BOOL IsModelReserved(char* sModel);
BOOL IsNameReserved(const char* sName, const AMAuthId&, const char* IP, user_struct* tUserRecord);
void SetUserPassword(const char* sName, char* sSetPassword, edict_t* pEntity);
void SetVaultData(char* sKey, char* sData);
void UpdateUserAuth(edict_t* pEntity);
BOOL VerifyUserAuth(const char* sName, edict_t* pEntity);
int pass_compare( const char* sServerPassword, const char* sPlayerPassword);


bool user_ip( int userIndex, const char** const stringIP, ulong* uintIP = 0 );
edict_t* get_player_edict( uint32_t ID, uidt type = uid_none );
edict_t* get_player_edict( const AMAuthId& ID, uidt type = uid_none );
