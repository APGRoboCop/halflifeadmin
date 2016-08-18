/*
 * $Id: users.h,v 1.13 2001/09/27 20:33:16 darope Exp $
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
 *  defines for the various access levels in the admin_ commands
 *
 */

#ifndef _USERS_H_
#define _USERS_H_


/* Yes, the access levels have been moved to the constants file. 
 * Also the buffer sizes are in the constants file since they 
 * don't relate to any specific but to more general data structures.
 */

#include "constants.h"

#ifndef ulong
#  define ulong unsigned long
#endif


//#ifdef USE_MYSQL
// Query to use for NON preloaded users check, takes 3 params
// 1: %s - the db name
// 2: %s - the nickname from the user
// 3: %i - the WONID of the user
// default sample is:"SELECT pass,access FROM %s where nick='%s' or nick='%i'\n"


//#define USERS_QUERY "SELECT pass,access FROM %s where nick='%s' or nick='%i'\n"
//#define USERS_QUERY "SELECT pass,access FROM %s where '%s' REGEXP nick or nick='%i'\n"
//#endif


/*
 * auth_struct to keep connected users auth info in
 */
typedef struct {
  char sUserName[USERNAME_SIZE];  // User name of the player
  char sPassword[PASSWORD_SIZE];  // Password the player used
  char sIP[IP_SIZE];              // IP of the player
  int iSessionID;                 // Session ID of the player
  unsigned long ulAuthID;         // ID used to auth a player on reconnect. Can be WONID.
  int iTime;                      // Time the player was auth'ed
  int iAccess;                    // Access the player has
  int iUserIndex;                 // Index of the user record match (from user_struct.iIndex; 0 for none)
} auth_struct;


/*
 *   ip_struct to keep IP and netmask in
 */
typedef struct {
  char sIP[IP_SIZE + 1];          // IP address (as x.x.x.x string)
  char sMask[IP_SIZE + 1];        // Subnet mask (as x.x.x.x string)
  ulong lIP;                      // IP address (as bits)
  ulong lMask;                    // Subnet mask (as bits)
} ip_struct;


/*
 *   model_struct to keep listed model auth info in
 */
typedef struct {
  char sModelName[USERNAME_SIZE]; // Model name to reserve
  char sPassword[PASSWORD_SIZE];  // Password to go along with name
} model_struct;


/*
 *  spawn_struct  -  ugh =)
 */
typedef struct {
  char szClassname[BUF_SIZE];     // Class name of the spawned entity
  int iIdentity;                  // Unique identity of the spawned entity
  CBaseEntity *pEntity;           // Actual spawned entity
} spawn_struct;


/*
 *  user_struct to keep listed users info in
 */
typedef struct {
  char sUserName[USERNAME_SIZE];  // User name.  May be a WON ID, but will always be a string.
  char sPassword[PASSWORD_SIZE];  // Password.
  int  iAccess;                   // Access.
  int  iIndex;                    // Index of the user record (1 based)
} user_struct;


/*
 *  vault_struct to keep vaul file info in
 */
typedef struct {
	char sKey[BUF_SIZE];            // Key (for lookup purposes)
	char sData[BUF_SIZE];           // Data associated with that key
} vault_struct;


/* 
 * word_struct to keep wordlist words in
 */
typedef struct {
  char sWord[BUF_SIZE];           // Word up, G
} word_struct;


#include "amx.h"
#include "CPlugin.h"


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


#if USE_MYSQL
    // Callback function; for each row returned by the loading query, call this
    // function with it
#   include <mysql.h>
#   include <errmsg.h>
    typedef BOOL (*PARSE_MYSQL)(MYSQL_ROW pRow);
#endif


// Callback function; for each line returned from the loaded file, call this
// function with it
typedef BOOL (*PARSE_FILE)(char* sLine);


// Returns true if this line should be a comment, false otherwise.
inline BOOL IsComment(char sChar) {
  return ((sChar == '#' || sChar == '/' || sChar == ';') ? TRUE : FALSE);
}



void UTIL_ClientPrint_UR( entvars_t *client, int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 );

int AddSpawnEntity(const char* szClassname, CBaseEntity* pEntity);
spawn_struct* FindSpawnEntity(int iIdentity);
void InitSpawnEntityList();
void ListSpawnEntities(edict_t* pMsg, char* szFindClassname);
BOOL RemoveSpawnEntity(int iIdentity);
///CEM


// CEM - 02/03/01
BOOL AddHelpEntry(char* sCmd, char* sHelp, int iAccess = ACCESS_ALL);
BOOL LoadPlugins();
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
int GetFreeSlots(edict_t* pEntityIgnore);
int GetHighlanderIndex();
int GetModelAccess(char* sModel, edict_t* pEntity);
int GetUserAccess(edict_t* pEntity);
BOOL GetModelRecord(char* sModel, char* sPassword, model_struct* tModelRecord);
char* GetVaultData(char* sKey);
void InitAdminModData(BOOL fFull, BOOL fRun);
BOOL IsModelReserved(char* sModel);
BOOL IsNameReserved(char* sName, int iWONID, user_struct* tUserRecord);
void SetUserPassword(const char* sName, char* sSetPassword, edict_t* pEntity);
void SetVaultData(char* sKey, char* sData);
void UpdateUserAuth(edict_t* pEntity);
BOOL VerifyUserAuth(const char* sName, edict_t* pEntity);


#endif /* _USERS_H_ */
