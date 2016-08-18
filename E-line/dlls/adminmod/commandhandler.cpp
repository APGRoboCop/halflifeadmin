
#include "commandhandler.h"
#include "CPlugin.h"

CCommandDispatcher g_Dispatcher;

void lcase(std::string& s) {
	std::string::iterator i;
	for(i=s.begin();i!=s.end();i++) {
		if ((*i>='A') && (*i<='Z')) {
			(*i)+=32;
		}
	}
}

int CAMXCommandHandler::Priority() const {
	return m_pPlugin->Priority();
}

plugin_result CAMXCommandHandler::Dispatch(edict_t* pEntity, char* szCmd, char* szData) const {
	return m_pPlugin->HandleCommand(this,pEntity,szCmd,szData);
}

//Emtpies the list of registered commands
void CCommandDispatcher::Empty()
{
	while (m_RegisteredCommands.size() >0) {
		CCommandList* pList= m_RegisteredCommands.begin()->second;
		while (pList->size() >0) {
			delete pList->front();
			pList->pop_front();
		}
		m_RegisteredCommands.erase(m_RegisteredCommands.begin());
		delete pList;
	}

	while (m_PluginCommands.size() >0) {
		delete m_PluginCommands.front();
		m_PluginCommands.pop_front();
	}

	m_iCommandCount=0;
	m_iLastCommandTime=0;
	m_iCurrentSpike=0;
	m_iMaxSpike=0;
	m_iSpikeCount=0;

}

//Records the registration of a command by either a plugin or as a built-in command
void CCommandDispatcher::Register(const char* szName, CCommandHandler* pHandler) {
	std::string strCommand = szName;
	lcase(strCommand);
	CCommandTree::iterator itTree = m_RegisteredCommands.find(strCommand);
	CCommandList* pList;
	if (itTree==m_RegisteredCommands.end()) {
		pList= new CCommandList();
		m_RegisteredCommands.insert(CCommandPair(szName,pList));
	}
	else {
		pList = itTree->second;
	}

	//Todo use STL search for pPriority > iPriority
	CCommandList::iterator itList;
	for (itList=pList->begin(); itList!=pList->end() ; itList++) {
		if ((*itList)->Priority() > pHandler->Priority()) {
			pList->insert(itList,pHandler);
			return;
		}
	}
	pList->push_back(pHandler);
}

//Records the registration of a plugin_command handler in AMX
void CCommandDispatcher::RegisterPluginCommand(CCommandHandler* pHandler) {
	CCommandList::iterator itList;
	for (itList=m_PluginCommands.begin(); itList!=m_PluginCommands.end() ; itList++) {
		if ((*itList)->Priority() > pHandler->Priority()) {
			m_PluginCommands.insert(itList,pHandler);
			return;
		}
	}
	m_PluginCommands.push_back(pHandler);

}

//Dispatches a command from HL to all registered handlers
plugin_result CCommandDispatcher::Dispatch(edict_t* pEntity,char* szCmd, char* szData) {

	//Update command proceessing stats
	m_iCommandCount++;
	if ((int)gpGlobals->time == m_iLastCommandTime) {
		m_iCurrentSpike++;
	}
	else {
		m_iCurrentSpike=1;
	}
	if (m_iCurrentSpike>m_iMaxSpike) {
		m_iMaxSpike=m_iCurrentSpike;
	}
	if (m_iCurrentSpike==5) {
		m_iSpikeCount++;
	}
	m_iLastCommandTime = (int)gpGlobals->time;

	std::string strCommand=szCmd;
	lcase(strCommand);
	plugin_result nRet=PLUGIN_CONTINUE;
	CCommandTree::const_iterator itTree;
	itTree=m_RegisteredCommands.find(strCommand);
	CCommandList::const_iterator itList;
	itList=m_PluginCommands.begin();

	if (itTree==m_RegisteredCommands.end()) {
		// Command isn't registered - process plugin command only
		while ((itList != m_PluginCommands.end()) && (nRet==PLUGIN_CONTINUE)) {
			nRet=(*itList)->Dispatch(pEntity,szCmd,szData);	//TODO: data,user index, user name
			itList++;
		}
	}
	else {
		// Command is registered. Process registrations & plugin_commands
		// in priority order.
		CCommandList::iterator itList2=itTree->second->begin();
		while (((itList != m_PluginCommands.end()) || (itList2 != itTree->second->end())) && (nRet==PLUGIN_CONTINUE)) {
			if (itList!= m_PluginCommands.end()) {
				if (itList2!= itTree->second->end()) {
					if ((*itList)->Priority() <= (*itList2)->Priority()) {
						nRet=(*itList)->Dispatch(pEntity,szCmd,szData);
						itList++;
					}
					else {
						nRet=(*itList2)->Dispatch(pEntity,szCmd,szData);
						itList2++;
					}
				} else {
					nRet=(*itList)->Dispatch(pEntity,szCmd,szData);
					itList++;
				}
			}
			else {
				nRet=(*itList2)->Dispatch(pEntity,szCmd,szData);
				itList2++;
			}
		}
	}
	return nRet;
}

int CCommandDispatcher::CheckCommand( edict_t* pEntity, const char* szCmd, unsigned int& iAccess, BOOL bCheckUserAcc, BOOL bPrintOut ) const {
	//Find the command in the tree. If its not there, return 0 implementations
	iAccess=0;
	CCommandTree::const_iterator itTree=m_RegisteredCommands.find(szCmd);
	if (itTree==m_RegisteredCommands.end()) {
		System_Response( UTIL_VarArgs("Command %s was not found.\n", szCmd), pEntity );
		return 0;
	}

	//Get the access level of the caller
	int iUserAccess=0;
	if (bCheckUserAcc) {
		// Note, that GetUserAccess() respects Highlander mode. This may mess up things.
		iUserAccess=GetUserAccess(pEntity);

	}



	CCommandList::const_iterator itList;

	int iCount=0;

	iAccess=~0;	//Set all bits so lowest access level is returned.

	for(itList=itTree->second->begin();itList!=itTree->second->end();itList++) {

		if (!bCheckUserAcc || ((*itList)->Access()==0) || ((iUserAccess & (*itList)->Access()) != 0)) {
			iCount++;
			if ( (*itList)->Access() < iAccess) {
				iAccess = (*itList)->Access();
			}

			if ( bPrintOut ) {
				if ((*itList)->Plugin()) {
					System_Response( UTIL_VarArgs("Command %s found in plugin %s\n",
						szCmd, (*itList)->Plugin()->Name()), pEntity );
				}
				else {
					System_Response( UTIL_VarArgs("Command %s is native to adminmod.\n",
						szCmd), pEntity );
				}
			}
		}

	}

	return iCount;

}

