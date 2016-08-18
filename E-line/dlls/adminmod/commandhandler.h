
#ifndef _COMMANDHANDLER_H
#define _COMMANDHANDLER_H

#include <string.h>
#include "extdll.h"
#include "amx.h"

//STL includes
#include <string>
#include <list>
#include <map>

class CPlugin;

enum plugin_result {
	PLUGIN_HANDLED = -1,            // Handled successfully.  Do not continue.
	PLUGIN_ERROR = 0,               // Error.  Continue.
	PLUGIN_CONTINUE = 1,            // Handled successfully.  Continue.
	PLUGIN_NO_ACCESS = 2,           // Invalid access.  Continue.
	PLUGIN_INVAL_CMD = 3,           // Invalid command, not implemented by plugin.
};

//Virtual base class for a command handler
class CCommandHandler {
public:
	virtual int Priority() const 
	{
		return 0;
	}

	virtual int Access() const
	{
		return 0;
	}

	virtual CPlugin* Plugin() const 
	{
		return NULL;
	}

	//Pure function - deriving classes must implement this.
	virtual	plugin_result Dispatch(edict_t* pEntity, char* szCmd, char* szData) const =0;
};

//Command handler for functions implemented in AMX
class CAMXCommandHandler : public CCommandHandler {
protected:
	CPlugin* m_pPlugin;
	int m_iIndex;
	int m_iAccess;
public:

	CAMXCommandHandler(CPlugin* pPlugin, int iIndex, int iAccess) {
		m_pPlugin = pPlugin;
		m_iIndex=iIndex;
		m_iAccess=iAccess;
	}

	plugin_result Dispatch(edict_t* pEntity, char* szCmd, char* szData) const;

	int Priority() const;

	int Access() const {
		return m_iAccess;
	}

	int Index() const {
		return m_iIndex;
	}

	CPlugin* Plugin() const {
		return m_pPlugin;
	}

};

typedef std::list<CCommandHandler*> CCommandList;
typedef std::map<std::string, CCommandList*> CCommandTree;
typedef std::pair<std::string, CCommandList*> CCommandPair;

class CCommandDispatcher {
private:
	CCommandList m_PluginCommands;
	CCommandTree m_RegisteredCommands;
	
	int m_iCommandCount;
	int m_iLastCommandTime;
	int m_iCurrentSpike;
	int m_iMaxSpike;
	int m_iSpikeCount;
public:
	
	~CCommandDispatcher() {
		Empty();
	}

	int CommandCount() { return m_iCommandCount; }
	int MaxSpike() { return m_iMaxSpike; }
	int SpikeCount() { return m_iSpikeCount; }

	void Empty();
	void Register(const char* szName, CCommandHandler* pHandler);
	void RegisterPluginCommand(CCommandHandler* pHandler);

	plugin_result Dispatch(edict_t* pEntity, char* szCmd, char* szData);
	int CheckCommand( edict_t* pEntity, const char* szCmd, unsigned int& iAccess, BOOL bCheckUserAcc, BOOL bPrintOut ) const;
};

extern CCommandDispatcher g_Dispatcher;

#endif