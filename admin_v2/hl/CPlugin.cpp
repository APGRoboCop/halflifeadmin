/*
 * $Id: CPlugin.cpp,v 1.3 2001/09/27 20:33:16 darope Exp $
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
 *
 *===========================================================================
 *
 * Comments:
 *   A class for handling the plugins.  Each instance
 *   of CPlugin represents one actual Small plugin; includes
 *   the filename, an AMX virtual machine, a list of exported
 *   commands, etc. 
 *
 */

#include "amutil.h"
#include "CPlugin.h"

extern AMXINIT amx_Init;
extern AMXREGISTER amx_Register;
extern AMXFINDPUBLIC amx_FindPublic;
extern AMXEXEC amx_Exec;
extern AMXGETADDR amx_GetAddr;
extern AMXSTRLEN amx_StrLen;
extern AMXRAISEERROR amx_RaiseError;
extern AMXSETSTRING amx_SetString;
extern AMXGETSTRING amx_GetString;

extern AMX_NATIVE_INFO fixed_Natives[];
extern AMX_NATIVE_INFO admin_Natives[];

// Constructor
CPlugin::CPlugin() {
	m_pAmx = NULL;
	m_pCommands = NULL;
	m_pProgram = NULL;
	InitValues();
}

// Destructor
CPlugin::~CPlugin() {
	Cleanup();
}

// The plugin's Small VM
AMX* CPlugin::amx() {
	return m_pAmx;
}

// Attempts to register a command for this plugin, linked the
// command sCmd with the Small function sFunction, and allowing
// it to be called only if the caller has access iAccess. 
// Returns TRUE if successful, FALSE otherwise.
BOOL CPlugin::AddCommand(char* sCmd, char* sFunction, int iAccess) {
	int iError;
	int iIndex;

	// Verify that we're a loaded plugin
	if (m_pAmx == NULL) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::AddCommand called when no AMX present for plugin '%s'.\n", m_sFile);
		return FALSE;
	}

	// Verify that the function exists
	iError = amx_FindPublic(m_pAmx, sFunction, &iIndex);
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s wants to hook command '%s' to non-existant function '%s'\n",m_sFile,sCmd,sFunction);
		return FALSE;
	}

	// Add the function/command to our list
	command_struct* pCmd = new command_struct;
	strcpy(pCmd->sCmd, sCmd);
	pCmd->iAccess = iAccess;
	pCmd->iIndex = iIndex;
	m_pCommands->AddLink(pCmd);
	return TRUE;
}

// General clean up, for when we're destroyed, or need to
// re-initialize ourselves
void CPlugin::Cleanup() {
	if (m_pAmx != NULL) {
		delete(m_pAmx);
		m_pAmx = NULL;
	}

	if (m_pProgram != NULL) {
		free(m_pProgram);
		m_pProgram = NULL;
	}

	if (m_pCommands != NULL) {
		delete(m_pCommands);
		m_pCommands = NULL;
	}
}

// Handles a command. Calls plugin_command, if this plugin implements it,
// and any functions associated with the command, if the plugin has any. 
// Note that with 'admin_command', it's possible for this proc to be called
// with a NULL pEntity (ie, from the console)
plugin_result CPlugin::HandleCommand(edict_t* pEntity, char* sCmd, char* sData) {
  int iError;
  long iResult = PLUGIN_CONTINUE;
  
  // Verify that we're a loaded plugin
  if (m_pAmx == NULL) {
    UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleCommand called when no AMX present for plugin '%s'.\n", m_sFile);
    return PLUGIN_ERROR;
  }
  
  // First, check to see if we're hooking all commands (if plugin_command exists) */
  if (m_iEventCommandIndex != INVALID_INDEX) {
    // If we are, call that procedure.
    // The implementation of plugin_command is:
    // plugin_command(Command[], Data[], UserName[], UserIndex);
    if (pEntity == NULL) {
      iError = amx_Exec(m_pAmx, &iResult, m_iEventCommandIndex, 4, sCmd, sData, "Admin", 0);
    } else {
      iError = amx_Exec(m_pAmx, &iResult, m_iEventCommandIndex, 4, sCmd, sData, STRING(pEntity->v.netname), ENTINDEX(pEntity));
    }
    // Check for errors.
    if (iError != AMX_ERR_NONE) {
      UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_command\n",m_sFile,iError);
      if (pEntity != NULL) {
	CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_command\n",m_sFile,iError));
      }
      return PLUGIN_ERROR;
    }
    // If we get PLUGIN_HANDLED back, we should not process this further.
    if (iResult == PLUGIN_HANDLED) 
      return (plugin_result)iResult;
  }
  
  // Otherwise, check to see if we actually handle this command; loop through
  // all commands in our list until we find a match, or run out.
  CLinkItem<command_struct>* pLink = m_pCommands->FirstLink();
  command_struct* pCmd;
  
  while (pLink != NULL) {
    pCmd = pLink->Data();
    if (stricmp(sCmd,pCmd->sCmd)==0) {
      DEBUG_LOG( 1, ("Match found on command %s for plugin %s / function %i\n",sCmd, m_sFile, pCmd->iIndex) );
      break;
    }
    pLink = pLink->NextLink();
  }
  
  // If we have no current command, we ran out, and so obviously don't handle this command.
  if (pLink == NULL) {
    return PLUGIN_CONTINUE;
    // Then, unless this command is ACCESS_ALL or we're the console, we need to check for
    // valid access.
  } else if (pCmd->iAccess != ACCESS_ALL && pEntity != NULL) {
    if ((GetUserAccess(pEntity) & pCmd->iAccess) != pCmd->iAccess) {
      char* sRejectMsg = (char*)CVAR_GET_STRING("admin_reject_msg");
      
      if (sRejectMsg == NULL || FStrEq(sRejectMsg,"0")) {
	CLIENT_PRINTF(pEntity, print_console, "You do not have access to this command.\n");
      } else {
	CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("%s\n", sRejectMsg));
      }
      DEBUG_LOG( 1, ("%s attempted to use command '%s' without proper access.\n", STRING(pEntity->v.netname), pCmd->sCmd) );
      return PLUGIN_NO_ACCESS;
    }
  }
  
  // Otherwise, call the procedure this command is associated with.
  // The implementation of an exported command is:
  // command(Command[], Data[], UserName[], UserIndex);
  if (pEntity == NULL) {
    iError = amx_Exec(m_pAmx, &iResult, pCmd->iIndex, 4, sCmd, sData, "Admin", 0);
  } else {
    iError = amx_Exec(m_pAmx, &iResult, pCmd->iIndex, 4, sCmd, sData, STRING(pEntity->v.netname), ENTINDEX(pEntity));
  }
  
  // Check for errors.
  if (iError != AMX_ERR_NONE) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing command %s\n",m_sFile,iError,sCmd);
    if (pEntity != NULL) {
      CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("[ADMIN] ERROR: Plugin %s returned error %i when executing command %s\n",m_sFile,iError,sCmd));
    }
    return PLUGIN_ERROR;
  }
  return (plugin_result)iResult;
}

// Handles the connect event; calls plugin_connect, if this plugin implements it.
plugin_result CPlugin::HandleConnect(edict_t* pEntity, char* sName, char* IPAddress) {
	int iError;
	int iIndex;
	long iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == NULL) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleConnect called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	// Verify that we have a valid entity
	} else if (pEntity == NULL) {
		UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::HandleConnect called with NULL entity.\n");
		return PLUGIN_ERROR;
	}

	// Make sure the entity's index is valid.
	iIndex = ENTINDEX(pEntity);
	if (!(iIndex >= 1 && iIndex <= INT_MAX_PLAYERS)) {
		UTIL_LogPrintf("[ADMIN] WARNING: CPlugin::HandleConnect called with invalid index %i entity.\n", iIndex);
		return PLUGIN_ERROR;
	}

	// If the plugin doesn't implement plugin_connect, we can leave.
	if (m_iEventConnectIndex == INVALID_INDEX)
		return PLUGIN_CONTINUE;

	// Otherwise, call the procedure.
	// The implementation of plugin_connect is:
	// plugin_connect(UserName[], IP[], UserIndex);
	iError = amx_Exec(m_pAmx, &iResult, m_iEventConnectIndex, 3, sName, IPAddress, iIndex);
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_connect\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}
	return (plugin_result)iResult;
}

// Handles the disconnect event; calls plugin_disconnect, if this plugin implements it.
plugin_result CPlugin::HandleDisconnect(edict_t* pEntity) {
	int iError;
	int iIndex;
	long iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == NULL) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleDisconnect called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	// Verify that we have a valid entity
	} else if (pEntity == NULL) {
		UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::HandleDisconnect called with NULL entity.\n");
		return PLUGIN_ERROR;
	}

	// Make sure the entity's index is valid.
	iIndex = ENTINDEX(pEntity);
	if (!(iIndex >= 1 && iIndex <= INT_MAX_PLAYERS)) {
		UTIL_LogPrintf("[ADMIN] WARNING: CPlugin::HandleDisconnect called with invalid index %i entity.\n", iIndex);
		return PLUGIN_ERROR;
	}

	// If the plugin doesn't implement plugin_disconnect, we can leave.
	if (m_iEventDisconnectIndex == INVALID_INDEX)
		return PLUGIN_CONTINUE;

	// Otherwise, call the procedure.
	// The implementation of plugin_disconnect is:
	// plugin_disconnect(UserName[], UserIndex);
	iError = amx_Exec(m_pAmx, &iResult, m_iEventDisconnectIndex, 2, STRING(pEntity->v.netname), iIndex);
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_disconnect\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}

	return (plugin_result)iResult;
}

// Handles the info change event; calls plugin_info, if this plugin implements it.  Note
// that at this point, if the name has changed, STRING(pEntity->v.netname) will still
// return the old name; thus, the new name gets passed in as a seperate procedure 
plugin_result CPlugin::HandleInfo(edict_t* pEntity, char* sNewName) {
	int iError;
	int iIndex;
	long iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == NULL) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleInfo called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	// Verify that we have a valid entity
	} else if (pEntity == NULL) {
		UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::HandleInfo called with NULL entity.\n");
		return PLUGIN_ERROR;
	}

	// Make sure the entity's index is valid.
	iIndex = ENTINDEX(pEntity);
	if (!(iIndex >= 1 && iIndex <= INT_MAX_PLAYERS)) {
		UTIL_LogPrintf("[ADMIN] WARNING: CPlugin::HandleInfo called with invalid index %i entity.\n", iIndex);
		return PLUGIN_ERROR;
	}

	// If the plugin doesn't implement plugin_info, we can leave.
	if (m_iEventInfoIndex == INVALID_INDEX)
		return PLUGIN_CONTINUE;

	// Otherwise, call the procedure.
	// The implementation of plugin_info is:
	// plugin_info(OldUserName[],NewUserName[],UserIndex);
	iError = amx_Exec(m_pAmx, &iResult, m_iEventInfoIndex, 3, STRING(pEntity->v.netname), sNewName, iIndex);
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_info\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}
	return (plugin_result)iResult;
}

// Handles the log event; calls plugin_log, if this plugin implements it.
plugin_result CPlugin::HandleLog(char* sLog) {
	int iError;
	long iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == NULL) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleLog called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	}

	// If the plugin doesn't implement plugin_disconnect, we can leave.
	if (m_iEventLogIndex == INVALID_INDEX)
		return PLUGIN_CONTINUE;

	// Otherwise, call the procedure.
	// The implementation of plugin_log is:
	// plugin_log(Log[]);
	iError = amx_Exec(m_pAmx, &iResult, m_iEventLogIndex, 1, sLog);
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_log\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}

	return (plugin_result)iResult;
}

// Initialize various values, for when we're instantiated, or need
// to re-initialize ourselves.
void CPlugin::InitValues() {
	Cleanup();

	m_iEventCommandIndex = INVALID_INDEX;
	m_iEventConnectIndex = INVALID_INDEX;
	m_iEventDisconnectIndex = INVALID_INDEX;
	m_iEventInfoIndex = INVALID_INDEX;
	m_iEventLogIndex = INVALID_INDEX;
	m_iInitIndex = INVALID_INDEX;

	m_sFile[0] = NULL;
	m_sName[0] = NULL;
	m_sDesc[0] = NULL;
	m_sVersion[0] = NULL;
}

// Given a file name (relative to the game's mod dir), attempts to load 
// that file into an AMX virtual machine.  Returns 1 if successful, 0 otherwise.
int CPlugin::LoadFile(char* sFilename) {

  int iError;
  FILE *fp;
  AMX_HEADER hdr;
 
  DEVEL_LOG(2, ("CPlugin::LoadFile() called."));
 
  m_pAmx = new AMX;
  // Attempt to open the file
  if ( (fp = fopen( sFilename, "rb" )) != NULL ) {
    // If we opened it, read the puppy in
    fread(&hdr, sizeof hdr, 1, fp);
    // We do a check for the file version here because if it doesn't match our 
    // AMX version we get very ugly Bohr bugs.
#ifndef MIN_FILE_VERSION
#  define MIN_FILE_VERSION 4
#endif
#ifndef CUR_FILE_VERSION
#  define CUR_FILE_VERSION 5
#endif
    if ( hdr.file_version < CUR_FILE_VERSION ) {
      UTIL_LogPrintf( "[ADMIN] ERROR: The plugin binary has an outdated format. (version %i)\n", hdr.file_version );
      UTIL_LogPrintf( "[ADMIN]        Please recompile the plugin with the compiler\n" );
      UTIL_LogPrintf( "[ADMIN]        that comes with this version of Adminmod.\n" );
      fclose( fp );
      return 0;
    }  // if
    // Get a memory chunk big enough for the file
    m_pProgram = calloc(1, (int)hdr.stp);
    if ( m_pProgram != NULL ) {
      // Copy the file in
      rewind( fp );
      fread( m_pProgram, 1, (int)hdr.size, fp );
      fclose( fp );
      // Initialize the AMX space
      memset(m_pAmx, 0, sizeof(AMX));
      // Now attempt to 'start' the file in the new AMX space
      DEVEL_LOG(3, ("Initializing abstract machine"));
      iError = amx_Init(m_pAmx, m_pProgram);
      DEVEL_LOG(3, ("Abstract machine initialized: %i", iError));
      if ( iError != AMX_ERR_NONE ) {
	UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: Call to amx_Init on plugin '%s' returned error #%i.\n",sFilename, iError);
	return 0;
      }
    } else {
      UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: malloc for hdr.stp (%i) on plugin '%s' failed\n",hdr.stp,sFilename);
      return 0;
    }
  } else {
    DEBUG_DO(1, UTIL_LogPrintf("[ADMIN] ERROR: fopen: %s\n", strerror(errno)); );
    UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: fopen '%s' failed.\n", sFilename);
    return 0;
  }
  DEVEL_LOG(3, ("CPlugin::LoadFile() returns."));
  return 1;
}

// Given a file name (relative to the game's mod dir), attempts to load
// and initialize that plugin.  Returns TRUE if successful, FALSE otherwise.
BOOL CPlugin::LoadPlugin(char* sFilename) {
	int iError = AMX_ERR_NONE;
	int iIndex = 0;

	// Make sure everything is in a pristine state before we start
	Cleanup();
	InitValues();
	

	// If we can't load the file, we've got nothing else to do.
	strcpy(m_sFile,sFilename);
	if (!LoadFile(sFilename)) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::LoadFile failed for plugin '%s'\n",m_sFile);
		Cleanup();
		return FALSE;
	}

	DEVEL_LOG(4, ("Registering natives"));
	// Ignore these error return values - sometimes, the VM will return error 19
	// when there's actually nothing wrong.  Dunno why.
	iError = amx_Register(m_pAmx, fixed_Natives, -1);
	iError = amx_Register(m_pAmx, admin_Natives, -1);

	DEVEL_LOG(4, ("Searching for plugin_init functions in plugin."));
	// Verify that the plugin implements plugin_init; this is required
	iError = amx_FindPublic(m_pAmx,"plugin_init",&iIndex);
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s doesn't support plugin_init.\n",m_sFile);
		Cleanup();
		return FALSE;
	}
	m_iInitIndex = iIndex;

	DEVEL_LOG(4, ("Searching for other plugin functions in plugin."));
	// Now, verify which of the other plugin_* functions (plugin_command,
	// plugin_connect, plugin_disconnect, plugin_info) the plugin implements.
	// These are not required.
	iError = amx_FindPublic(m_pAmx, "plugin_command",&iIndex);
	if (iError == AMX_ERR_NONE) {
		m_iEventCommandIndex = iIndex;
	}

	iError = amx_FindPublic(m_pAmx, "plugin_connect",&iIndex);
	if (iError == AMX_ERR_NONE) {
		m_iEventConnectIndex = iIndex;
	}

	iError = amx_FindPublic(m_pAmx, "plugin_disconnect",&iIndex);
	if (iError == AMX_ERR_NONE) {
		m_iEventDisconnectIndex = iIndex;
	}

	iError = amx_FindPublic(m_pAmx, "plugin_info",&iIndex);
	if (iError == AMX_ERR_NONE) {
		m_iEventInfoIndex = iIndex;
	}

	iError = amx_FindPublic(m_pAmx, "plugin_log",&iIndex);
	if (iError == AMX_ERR_NONE) {
		m_iEventLogIndex = iIndex;
	}

	// Initialize the command linked list.
	m_pCommands = new CLinkList<command_struct>();
	return TRUE;
}

plugin_result CPlugin::StartPlugin() {
	int iError = AMX_ERR_NONE;
	long iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == NULL) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::StartPlugin called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	}

	// Call plugin_init.  We know the plugin has to implement plugin_init,
	// because we check for this in LoadPlugin.
	iError = amx_Exec(m_pAmx, &iResult, m_iInitIndex, 0);
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Executing plugin_init on plugin %s returned error %i\n", m_sFile, iError);
		Cleanup();
		return PLUGIN_ERROR;
	}

	return (plugin_result)iResult;
}

// The file name of the plugin; relative to the game's mod dir
char* CPlugin::File() {
	return m_sFile;
}

// The name of the plugin
char* CPlugin::Name() {
	return m_sName;
}

void CPlugin::SetName(char* sName) {
	strcpy(m_sName,sName);
}

// The description of the plugin
char* CPlugin::Desc() {
	return m_sDesc;
}

void CPlugin::SetDesc(char* sDesc) {
	strcpy(m_sDesc,sDesc);
}

// The version of the plugin
char* CPlugin::Version() {
	return m_sVersion;
}

void CPlugin::SetVersion(char* sVersion) {
	strcpy(m_sVersion,sVersion);
}
