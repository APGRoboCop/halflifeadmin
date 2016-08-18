#ifndef BUILTIN_COMMANDS_H
#define BUILTIN_COMMANDS_H

#include "commandhandler.h"

class CAdminPasswordHandler: public CCommandHandler {
public:
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};

class CAdminHelpHandler: public CCommandHandler {
public:
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};

class CAdminHelpHlratHandler: public CCommandHandler {
public:
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};

class CAdminVersionHandler: public CCommandHandler {
public:
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};

class CAdminAdmHandler: public CCommandHandler {
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};

class CAdminStatusHandler : public CCommandHandler {
public:
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};
  
class CMenuSelectHandler : public CCommandHandler {
public:
	int Priority() const { return 1000000; }	//Run this last after the plugins have a chance
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};

class CAdminCommandStatsHandler: public CCommandHandler {
public:
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};

class CAdminListGameMessagesHandler: public CCommandHandler {
public:
	plugin_result Dispatch(edict_t* pEntity,char* szCmd, char* szData) const;
};


void RegisterBuiltinCommands();
  
#endif
