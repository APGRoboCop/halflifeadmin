/* $Id: CPlugin.h,v 1.5 2001/05/28 03:51:27 darope Exp $ */

/* A class for handling the plugins.  Each instance
of CPlugin represents one actual Small plugin; includes
the filename, an AMX virtual machine, a list of exported
commands, etc. */

#define BUF_SIZE 100 // general buffer size
#define PLUGIN_CMD_SIZE 100
#define PLUGIN_HELP_SIZE 150

#define INVALID_INDEX -1
#define INT_MAX_PLAYERS 32

const int ACCESS_ALL = 0;
const int ACCESS_IMMUNITY = 4096;
const int ACCESS_RESERVE_NICK = 16384;
const int ACCESS_RESERVE_SPOT = 32768;

enum plugin_result {
	PLUGIN_HANDLED = -1,            // Handled successfully.  Do not continue.
	PLUGIN_ERROR = 0,               // Error.  Continue.
	PLUGIN_CONTINUE = 1,            // Handled successfully.  Continue.
	PLUGIN_NO_ACCESS = 2,           // Invalid access.  Continue.
};

#include <string.h>
#include "extdll.h"
#include "amx.h"
#include "CLinkList.cpp"

int GetUserAccess(edict_t* pEntity);
void System_Response(char *str,edict_t *);

/* Yeah, it's kludgy to redefine this stuff here, but I
didn't feel like messing with users.h. */

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

void UTIL_LogPrintf( char *fmt, ... );

typedef struct {
	char sCmd[PLUGIN_CMD_SIZE];
	int iAccess;
	int iIndex;
} command_struct;

typedef struct {
	char sCmd[PLUGIN_CMD_SIZE];
	char sHelp[PLUGIN_HELP_SIZE];
	int iAccess;
} help_struct;

class CPlugin {

private :
	int m_iEventCommandIndex;
	int m_iEventConnectIndex;
	int m_iEventDisconnectIndex;
	int m_iEventInfoIndex;
	int m_iEventLogIndex;
	int m_iInitIndex;
	char m_sFile[BUF_SIZE];
	char m_sName[BUF_SIZE];
	char m_sDesc[BUF_SIZE];
	char m_sVersion[BUF_SIZE];
	AMX* m_pAmx;
	void* m_pProgram;
	CLinkList<command_struct>* m_pCommands;

	void Cleanup();
	void InitValues();
	int LoadFile(char* filename);

public:
	CPlugin::CPlugin();
	CPlugin::~CPlugin();
	
	AMX* amx();
	BOOL AddCommand(char* Cmd, char* Function, int iAccess);
	plugin_result HandleCommand(edict_t* pEntity, char* sCmd, char* sData);
	plugin_result HandleConnect(edict_t* pEntity, char* sName, char* IPAddress);
	plugin_result HandleDisconnect(edict_t* pEntity);
	plugin_result HandleInfo(edict_t* pentity, char* sNewName);
	plugin_result HandleLog(char* sLog);
	BOOL LoadPlugin(char* filename);
	plugin_result StartPlugin();
	char* File();
	char* Name();
	void SetName(char* sName);
	char* Desc();
	void SetDesc(char* sDesc);
	char* Version();
	void SetVersion(char* sVersion);
};
