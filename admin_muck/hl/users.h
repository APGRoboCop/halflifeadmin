/***
*
*	Copyright (c) 2000, Alfred Reynolds
*
*   $Id: users.h,v 1.20 2001/07/22 11:56:07 alfred Exp $
*
****/

/*
  ===== users.h ========================================================
  
  defines for the various access levels in the admin_ commands
  
*/

/*

1 - vote map, vote kick,
2, - map, timelimit, fraglimit. (if they can change the map.. it
doesn't make sense to lock these out)
4 - prematch, reload
8 - pause, unpause
16 - pass, no_pass
32 - teamplay, Friendly-Fire
64 - admin_change other server variables.. (gravity, fall damage,
etc.. )
128 - admin_say, admin_kick
256 - admin_ban, admin_unban
leaves some space for other stuff.

32768 - reserved slot
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

#define ALL 0
#define LEV1 1
#define LEV2 1<<1
#define LEV4  1<<2
#define LEV8 1<<3
#define LEV16 1<<4
#define LEV32 1<<5 //32
#define LEV64 1<<6 // 64
#define LEV128 1<<7
#define LEV256 1<<8
#define LEV512 1<<9
#define LEV1024 1<<10
#define LEV2048  1<< 11
#define LEV4096  1<<12
#define LEV8192 1<<13
#define LEV16384 1<<14
#define LEV32768 1<<15

#define MAX_PLAYERS 64              // max number of users allowed on a hl server. Its 32 but lets be safe :)
#define MAX_STRBUF_LEN 1024

#define ACCESS_SIZE 10              // max size of an access number
#define BUF_SIZE 100                // max size in general (default)
#define LARGE_BUF_SIZE 256          // max size of a large buffer
#define HUGE_BUF_SIZE  512          // max size of a huge buffer
#define CENTER_SAY_LINE_SIZE 80     // max size of any one line in centersay
#define CENTER_SAY_SIZE 500         // max size of centersay "say" buffer
#define COMMAND_SIZE 256            // max size of a server command
#define IP_SIZE 16                  // max size of an ip line
#define LINE_SIZE 256               // max size of line in file
#define PASSWORD_SIZE 64            // max size of password (big cause of win32)
#define USERNAME_SIZE 64            // max size of username
#define VOTE_SIZE 1024              // max size of the vote message (including all options)

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

#define ulong unsigned long
typedef struct {
  char sIP[IP_SIZE + 1];          // IP address (as x.x.x.x string)
  char sMask[IP_SIZE + 1];        // Subnet mask (as x.x.x.x string)
  ulong lIP;                      // IP address (as bits)
  ulong lMask;                    // Subnet mask (as bits)
} ip_struct;

typedef struct {
  char sModelName[USERNAME_SIZE]; // Model name to reserve
  char sPassword[PASSWORD_SIZE];  // Password to go along with name
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

#ifdef WIN32
  #define SCRIPT_DLL "dlls\\amx_admin.dll"
#else
  #define SCRIPT_DLL "dlls/amx_admin.so"
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


/* these moved into amutil.h
// some macros to be used around
#define DEBUG_LOG( args )   do{  if ( (int)CVAR_GET_FLOAT("admin_debug")!=0 ) \
                 UTIL_LogPrintf( "[ADMIN] DEBUG: %s\n", UTIL_VarArgs args );} while(0)

#define DEVEL_LOG( level, args )   do{  if ( (int)CVAR_GET_FLOAT("developer")>=level ) \
                 UTIL_LogPrintf( "[ADMIN] DEVEL: %s\n", UTIL_VarArgs args );} while(0)
*/









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
BOOL IsComment(char sChar);
BOOL IsModelReserved(char* sModel);
BOOL IsNameReserved(char* sName, int iWONID, user_struct* tUserRecord);
void SetUserPassword(const char* sName, char* sSetPassword, edict_t* pEntity);
void SetVaultData(char* sKey, char* sData);
void UpdateUserAuth(edict_t* pEntity);
BOOL VerifyUserAuth(const char* sName, edict_t* pEntity);
