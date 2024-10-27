/*
 * ===========================================================================
 *
 * $Id: CPlugin.cpp,v 1.6 2004/07/23 22:26:01 darope Exp $
 *
 *
 * Copyright (c) 2002-2003 Alfred Reynolds, Florian Zschocke, Magua
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
 * A class for handling the plugins.  Each instance
 * of CPlugin represents one actual Small plugin; includes
 * the filename, an AMX virtual machine, a list of exported
 * commands, etc. 
 *
 */

#include "CPlugin.h"
#include "amutil.h"
#include "amlibc.h"
#include <amxconv_l.h>

#ifdef _WIN32
#define stricmp _stricmp
#endif

extern AMXINIT amx_Init;
extern AMXREGISTER amx_Register;
extern AMXFINDPUBLIC amx_FindPublic;
extern AMXEXEC amx_Exec;
extern AMXGETADDR amx_GetAddr;
extern AMXSTRLEN amx_StrLen;
extern AMXRAISEERROR amx_RaiseError;
extern AMXSETSTRING amx_SetString;
extern AMXGETSTRING amx_GetString;
extern AMX_NATIVE_INFO admin_Natives[];

extern DLL_GLOBAL edict_t* pAdminEnt;

// Constructor
CPlugin::CPlugin() {
	m_pAmx = nullptr;
	m_pCommands = nullptr;
	m_pProgram = nullptr;
	InitValues();
}

// Destructor
CPlugin::~CPlugin() { //Unstable that ~CPlugin is unwanted? [APG]RoboCop[CL]
	Cleanup();
}

// The plugin's Small VM
AMX* CPlugin::amx() const
{
	return m_pAmx;
}

// Attempts to register a command for this plugin, linked the
// command sCmd with the Small function sFunction, and allowing
// it to be called only if the caller has access iAccess. 
// Returns TRUE if successful, FALSE otherwise.
BOOL CPlugin::AddCommand(char* sCmd, char* sFunction, int iAccess) {
	int iIndex;

	// Verify that we're a loaded plugin
	if (m_pAmx == nullptr) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::AddCommand called when no AMX present for plugin '%s'.\n", m_sFile);
		return FALSE;
	}

	// Verify that the function exists
	const int iError = amx_FindPublic(m_pAmx, sFunction, &iIndex);
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
	if (m_pAmx != nullptr) {
		delete m_pAmx;
		m_pAmx = nullptr;
	}

	if (m_pProgram != nullptr) {
		delete[] static_cast<char*>(m_pProgram);
		m_pProgram = nullptr;
	}

	if (m_pCommands != nullptr) {
		delete m_pCommands;
		m_pCommands = nullptr;
	}
}

// Checks if this plugin implements a command and returns the access level
// associated with this command in the second parameter.
// Returns TRUE if the plugin implements the command, FALSE otherwise.
BOOL CPlugin::CheckCommand( const char* _pcCommand, unsigned int& _uiAccess ) const
{

	// Verify that we're a loaded plugin
	if (m_pAmx == nullptr) {
		//UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::CheckCommand called when no AMX present for plugin '%s'.\n", m_sFile);
		return FALSE;
	}

	// Verify that we hook any commands
	if (m_pCommands == nullptr) {
		return FALSE;
	}

  // Loop through all commands in our list until we find a match, or run out.
  CLinkItem<command_struct>* pLink = m_pCommands->FirstLink();
  command_struct* pCmd;
  
  while ( pLink != nullptr ) {
	  pCmd = pLink->Data();
	  if ( strcasecmp(_pcCommand, pCmd->sCmd) == 0 ) {
		  _uiAccess = pCmd->iAccess;
		  pCmd = nullptr;
		  pLink = nullptr;
		  return TRUE;
	  }  // if
	  pLink = pLink->NextLink();
  }  // while

  pCmd = nullptr;
  pLink = nullptr;
  return FALSE;
}




// Handles a command. Calls plugin_command, if this plugin implements it,
// and any functions associated with the command, if the plugin has any. 
// Note that with 'admin_command', it's possible for this proc to be called
// with a nullptr pEntity (ie, from the console)
plugin_result CPlugin::HandleCommand(edict_t* pEntity, char* sCmd, char* sData) {
  int iError;
  cell iResult = PLUGIN_INVAL_CMD;
  
  // Verify that we're a loaded plugin
  if (m_pAmx == nullptr) {
    UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleCommand called when no AMX present for plugin '%s'.\n", m_sFile);
    return PLUGIN_ERROR;
  }

  // Some safekeeping. A nullptr sCmd gets passed as a an empty string. Same for sData.
  // DOS line endings get transformed to proper format without the CR.
  if ( sCmd == nullptr ) {
	  sCmd = "";
  } else {
	  char* pcCR;
	  // check if the first '\r' from right is the last ("\r\0") or second to last (prob. "\r\l\0") character. If yes, end the string there.
	  if ( (pcCR = strrchr(sCmd, '\r')) != nullptr  && (*(pcCR+1) == '\0' || *(pcCR+2) == '\0') ) *pcCR = '\0';
  }  // if
  
  if ( sData == nullptr ) {
	  sData = "";
  } else {
	  char* pcCR;
	  // check if the first '\r' from right is the last ("\r\0") or second to last (prob. "\r\l\0") character. If yes, end the string there.
	  if ( (pcCR = strrchr(sData, '\r')) != nullptr && (*(pcCR+1) == '\0' || *(pcCR+2) == '\0') ) *pcCR = '\0';
  }  // if
  


  // First, check to see if we're hooking all commands (if plugin_command exists) */
  if (m_iEventCommandIndex != INVALID_INDEX) {
    // If we are, call that procedure.
    // The implementation of plugin_command is:
    // plugin_command(Command[], Data[], UserName[], UserIndex);
    if (pEntity == nullptr) {
      iError = amx_Exec(m_pAmx, &iResult, m_iEventCommandIndex, 4, reinterpret_cast<cell>(sCmd), reinterpret_cast<cell>(sData), reinterpret_cast<cell>("Admin"), 0);
    } else {
		char name[USERNAME_SIZE];
		am_strncpy(name,STRING(pEntity->v.netname),USERNAME_SIZE);
		make_friendly(name,TRUE);
		iError = amx_Exec(m_pAmx, &iResult, m_iEventCommandIndex, 4, reinterpret_cast<cell>(sCmd), reinterpret_cast<cell>(sData), reinterpret_cast<cell>(name),
		                  ENTINDEX(pEntity));
    }
    // Check for errors.
    if (iError != AMX_ERR_NONE) {
      UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_command\n",m_sFile,iError);
      if (pEntity != nullptr) {
		  CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_command\n",m_sFile,iError));
      }
      return PLUGIN_ERROR;
    }
    // If we get PLUGIN_HANDLED back, we should not process this further.
    if (iResult == PLUGIN_HANDLED) 
      return static_cast<plugin_result>(iResult);
  }
  

	// Verify that we hook any commands
	if (m_pCommands == nullptr) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s has no valid list of available commands.\n",m_sFile);
		return PLUGIN_ERROR;
	}

  // Otherwise, check to see if we actually handle this command; loop through
  // all commands in our list until we find a match, or run out.
  CLinkItem<command_struct>* pLink = m_pCommands->FirstLink();
  command_struct* pCmd = nullptr;
  
  while (pLink != nullptr) {
    pCmd = pLink->Data();
    if (stricmp(sCmd,pCmd->sCmd)==0) {
      DEBUG_LOG( 3, ("Match found on command %s (access:%i) for plugin %s / function %i",sCmd, pCmd->iAccess, m_sFile, pCmd->iIndex) );
      break;
    }
    pLink = pLink->NextLink();
  }
  
  // If we have no current command, we ran out, and so obviously don't handle this command.
  if (pLink == nullptr) {
    return PLUGIN_INVAL_CMD;
    // Then, unless this command is ACCESS_ALL or we're the console, we need to check for
    // valid access.
  }
  if (pCmd->iAccess != ACCESS_ALL && pEntity != nullptr) {
	  if ((GetUserAccess(pEntity) & pCmd->iAccess) != pCmd->iAccess) {
		  char* sRejectMsg = const_cast<char*>(CVAR_GET_STRING("admin_reject_msg"));
      
		  if (sRejectMsg == nullptr || FStrEq(sRejectMsg,"0")) {
			  CLIENT_PRINTF(pEntity, print_console, "You do not have access to this command.\n");
		  } else {
			  CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("%s\n", sRejectMsg));
		  }
		  UTIL_LogPrintf( "[ADMIN] INFO: '%s' attempted to use command '%s' without proper access.\n", STRING(pEntity->v.netname), pCmd->sCmd);
		  return PLUGIN_NO_ACCESS;
	  }
  }

  // Otherwise, call the procedure this command is associated with.
  // The implementation of an exported command is:
  // command(Command[], Data[], UserName[], UserIndex);
  if (pEntity == nullptr) {
    iError = amx_Exec(m_pAmx, &iResult, pCmd->iIndex, 4, reinterpret_cast<cell>(sCmd), reinterpret_cast<cell>(sData), reinterpret_cast<cell>("Admin"), 0);
  } else {
	// Possible instabilty error for 2.50 Linux SteamPipe build [APG]RoboCop[CL]
    iError = amx_Exec(m_pAmx, &iResult, pCmd->iIndex, 4, reinterpret_cast<cell>(sCmd), reinterpret_cast<cell>(sData), reinterpret_cast<cell>(STRING(pEntity->v.netname)), ENTINDEX(pEntity));
  }
  
  // Check for errors.
  if (iError != AMX_ERR_NONE) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing command %s\n",m_sFile,iError,sCmd);
    if (pEntity != nullptr) {
      CLIENT_PRINTF(pEntity, print_console,
                    UTIL_VarArgs("[ADMIN] ERROR: Plugin %s returned error %i when executing command %s\n", m_sFile, iError, sCmd));
    }
    return PLUGIN_ERROR;
  }
  return static_cast<plugin_result>(iResult);
}

// Handles the connect event; calls plugin_connect, if this plugin implements it.
plugin_result CPlugin::HandleConnect(edict_t* pEntity, char* sName, char* IPAddress) {
	cell iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == nullptr) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleConnect called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	// Verify that we have a valid entity
	} else if (pEntity == nullptr) {
		UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::HandleConnect called with nullptr entity.\n");
		return PLUGIN_ERROR;
	}

	// Make sure the entity's index is valid.
	const int iIndex = ENTINDEX(pEntity);
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
	const int iError = amx_Exec(m_pAmx, &iResult, m_iEventConnectIndex, 3, reinterpret_cast<cell>(sName),
	                            reinterpret_cast<cell>(IPAddress),
	                            static_cast<cell>(iIndex));
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_connect\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}
	return static_cast<plugin_result>(iResult);
}

// Handles the disconnect event; calls plugin_disconnect, if this plugin implements it.
plugin_result CPlugin::HandleDisconnect(edict_t* pEntity) {
	cell iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == nullptr) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleDisconnect called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	// Verify that we have a valid entity
	} else if (pEntity == nullptr) {
		UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::HandleDisconnect called with nullptr entity.\n");
		return PLUGIN_ERROR;
	}

	// Make sure the entity's index is valid.
	const int iIndex = ENTINDEX(pEntity);
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
	const int iError = amx_Exec(m_pAmx, &iResult, m_iEventDisconnectIndex, 2,
	                            reinterpret_cast<cell>(STRING(pEntity->v.netname)), static_cast<cell>(iIndex));
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_disconnect\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}

	return static_cast<plugin_result>(iResult);
}

// Handles the info change event; calls plugin_info, if this plugin implements it.  Note
// that at this point, if the name has changed, STRING(pEntity->v.netname) will still
// return the old name; thus, the new name gets passed in as a seperate procedure 
plugin_result CPlugin::HandleInfo(edict_t* pEntity, char* sNewName) {
	cell iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == nullptr) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleInfo called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	// Verify that we have a valid entity
	} else if (pEntity == nullptr) {
		UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::HandleInfo called with nullptr entity.\n");
		return PLUGIN_ERROR;
	}

	// Make sure the entity's index is valid.
	const int iIndex = ENTINDEX(pEntity);
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
	const int iError = amx_Exec(m_pAmx, &iResult, m_iEventInfoIndex, 3,
	                            reinterpret_cast<cell>(STRING(pEntity->v.netname)), reinterpret_cast<cell>(sNewName),
	                            static_cast<cell>(iIndex));
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_info\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}
	return static_cast<plugin_result>(iResult);
}

// Handles the log event; calls plugin_log, if this plugin implements it.
plugin_result CPlugin::HandleLog(char* sLog) {
	cell iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == nullptr) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::HandleLog called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	}

	// If the plugin doesn't implement plugin_disconnect, we can leave.
	if (m_iEventLogIndex == INVALID_INDEX)
		return PLUGIN_CONTINUE;

	// Otherwise, call the procedure.
	// The implementation of plugin_log is:
	// plugin_log(Log[]);
	const int iError = amx_Exec(m_pAmx, &iResult, m_iEventLogIndex, 1, reinterpret_cast<cell>(sLog));
	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s returned error %i when executing plugin_log\n",m_sFile,iError);
		return PLUGIN_ERROR;
	}

	return static_cast<plugin_result>(iResult);
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

	m_sFile[0] = 0;
	m_sName[0] = 0;
	m_sDesc[0] = 0;
	m_sVersion[0] = 0;
}

// Given a file name (relative to the game's mod dir), attempts to load 
// that file into an AMX virtual machine.  Returns 1 if successful, 0 otherwise.
int CPlugin::LoadFile(char* sFilename) {
	FILE *fp;
	AMX_LINUX_HEADER hdr;

	delete[] static_cast<char*>(m_pProgram);

	m_pProgram = nullptr;
	m_pAmx = new AMX;

	// Attempt to open the file
	if ( (fp = fopen( sFilename, "rb" )) != nullptr ) {
		int iAMXsize;
		bool bFileNeedsConv;

		// If we opened it, read the puppy in
		fread(&hdr, sizeof hdr, 1, fp);
		// check .amx file type
		const int iFileType = check_header_type(hdr, iAMXsize);

		if ( iFileType == INVAL_AMX_HDR ) {
			// This is no valid AMX file
			UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: File '%s' is not a valid Admin Mod script binary.\n", sFilename);
			fclose( fp );
			return 0;

		} else if ( iFileType == INVAL_AMX_VER ) {
			// This is a AMX file version that we do not support
			UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: File '%s' has a file version not supported by Admin Mod (%i,%i).\n", sFilename, hdr.file_version, hdr.amx_version );
			fclose( fp );
			return 0;
			
#ifdef __amd64__
		} else if ( iFileType != AMD64_AMX ) {
			// This is a AMX file format that we do not support
			UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: File '%s' is not a 64-bit file.\n", sFilename );
			fclose( fp );
			return 0;
#endif

#ifdef LINUX
		} else if ( iFileType == LINUX_AMX ) {			
#else
		} else if ( iFileType == WIN32_AMX ) {
#endif
			// No conversion needed since it is of the same architecture.
			bFileNeedsConv = false;
			iAMXsize = hdr.stp;

#ifdef LINUX
		} else if ( iFileType == WIN32_AMX ) {
#else
		} else if ( iFileType == LINUX_AMX ) {
#endif
			// This file has the wrong format, we need to convert it
			bFileNeedsConv = true;
			iAMXsize = iAMXsize + hdr.stp;
		
		} else if ( iFileType == AMD64_AMX ) {
#ifdef __amd64__
			bFileNeedsConv = false;
			iAMXsize = hdr.stp;
#else
			// This is a AMX file format that we do not support
			UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: File '%s' has a format not supported by this version of Admin Mod: 64-bit script binary.\n", sFilename );
			fclose( fp );
			return 0;
#endif

		} else {
			// Should not happen.
			UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: Unexpected error on checking version of file '%s', cannot load.\n", sFilename );
			fclose( fp );
			return 0;
		}  // if-else


		// Get a memory chunk big enough for the file
		m_pProgram = new char[ iAMXsize ];
		if ( m_pProgram != nullptr ) {
			// Go to the start of the file again
			rewind( fp );
			
			if ( ! bFileNeedsConv ) {
				// we can simply copy the file in and be done
				fread( m_pProgram, 1, hdr.size, fp );
				fclose( fp );
				
			} else {
				// otherwise we need to read it to a temp memory space first
				char* pTmpMemFile = new char[hdr.size];
				char* pCodeFrom, *pCodeTo;
				long lCodeSize;
				if ( pTmpMemFile == nullptr ) {
					UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::LoadFile: Loading file '%s' to memory failed: not enough memory (%i)\n", sFilename, hdr.size );
					delete[] static_cast<char*>(m_pProgram);
					fclose( fp );
					return 0;
				}  // if
				
				// copy the file to memory
				fread( pTmpMemFile, 1, hdr.size, fp );
				fclose( fp );
				
				// convert the header, storing it into the program memory
#ifdef LINUX
				DEBUG_LOG(2, ("CPlugin::LoadFile: Loading Win32 script file '%s', converting to Linux format.", sFilename) );
				if ( hdr.file_version == 4 ) {
					pCodeFrom = convert_header( pTmpMemFile, *(reinterpret_cast<AMX_LINUX_HEADER_V4*>(m_pProgram)) );
					pCodeTo = static_cast<char*>(m_pProgram) + sizeof(AMX_LINUX_HEADER_V4);
					lCodeSize = hdr.size - sizeof(AMX_WIN32_HEADER_V4);
				} else {
					pCodeFrom = convert_header( pTmpMemFile, *(reinterpret_cast<AMX_LINUX_HEADER*>(m_pProgram)) );
					pCodeTo = static_cast<char*>(m_pProgram) + sizeof(AMX_LINUX_HEADER);
					lCodeSize = hdr.size - sizeof(AMX_WIN32_HEADER);
				}  // if-else
#else
				DEBUG_LOG(2, ("CPlugin::LoadFile: Loading Linux script file '%s', converting to Win32 format.", sFilename) );
				if ( hdr.file_version == 4 ) {
					pCodeFrom = convert_header( pTmpMemFile, *static_cast<AMX_WIN32_HEADER_V4*>(m_pProgram) );
					pCodeTo = static_cast<char*>(m_pProgram) + sizeof(AMX_WIN32_HEADER_V4);
					lCodeSize = hdr.size - sizeof(AMX_LINUX_HEADER_V4);
				} else {
					pCodeFrom = convert_header( pTmpMemFile, *static_cast<AMX_WIN32_HEADER*>(m_pProgram) );
					pCodeTo = static_cast<char*>(m_pProgram) + sizeof(AMX_WIN32_HEADER);
					lCodeSize = hdr.size - sizeof(AMX_LINUX_HEADER);
				}  // if-else
#endif
				
				// copy the code part to the program memory
				memcpy( pCodeTo, pCodeFrom, lCodeSize );
				
				// delete the temporary memory file
				delete[] pTmpMemFile;
			}  // if-else
			
			// Initialize the AMX space
			memset(m_pAmx, 0, sizeof(AMX));
			// Now attempt to 'start' the file in the new AMX space
			// Ignore error indicating that native functions aren't found. The admin functions get registered later.
			const int iError = amx_Init(m_pAmx, m_pProgram);
			if ( iError != AMX_ERR_NONE && iError != AMX_ERR_NOTFOUND ) {
				UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: Call to amx_Init on plugin '%s' returned error #%i.\n",sFilename, iError);
				delete[] static_cast<char*>(m_pProgram);
				return 0;
			}
			
		} else {
			UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: malloc for hdr.stp (%i) on plugin '%s' failed: %s\n", iAMXsize, sFilename, strerror(errno));
			fclose( fp );
			return 0;
		}

	} else {
		UTIL_LogPrintf("[ADMIN] ERROR: CPlugin::LoadFile: fopen '%s' failed: %s.\n", sFilename, strerror(errno));
		return 0;
	}

	return 1;
}

// Given a file name (relative to the game's mod dir), attempts to load
// and initialize that plugin.  Returns TRUE if successful, FALSE otherwise.
BOOL CPlugin::LoadPlugin(char* sFilename) {
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

	// Register the Admin Mod native functions. If the amx_Register() call returns an error,
	// there are unresolved native calls in the plugin so execution is not possible.
	int iError = amx_Register(m_pAmx, admin_Natives, -1);
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] WARNING: Plugin %s could not register all native functions.\n",m_sFile);
		Cleanup();
		return FALSE;
	}

	// Verify that the plugin implements plugin_init; this is required
	iError = amx_FindPublic(m_pAmx,"plugin_init",&iIndex);
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Plugin %s doesn't support plugin_init.\n",m_sFile);
		Cleanup();
		return FALSE;
	}
	m_iInitIndex = iIndex;

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
	cell iResult = PLUGIN_CONTINUE;

	// Verify that we're a loaded plugin
	if (m_pAmx == nullptr) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CPlugin::StartPlugin called when no AMX present for plugin '%s'.\n", m_sFile);
		return PLUGIN_ERROR;
	}

	// Save the pAdminEnt pointer to call the plugin_init() function as server.
	const edict_t* pTempEnt = pAdminEnt;
	pAdminEnt = nullptr;

	// Call plugin_init.  We know the plugin has to implement plugin_init,
	// because we check for this in LoadPlugin.
	int iError = amx_Exec(m_pAmx, &iResult, m_iInitIndex, 0);

	// Restore the pAdminEnt pointer.
	pAdminEnt = const_cast<edict_t*>(pTempEnt);
	pTempEnt = nullptr;

	// Check for errors.
	if (iError != AMX_ERR_NONE) {
		UTIL_LogPrintf( "[ADMIN] ERROR: Executing plugin_init on plugin %s returned error %i\n", m_sFile, iError);
		Cleanup();
		return PLUGIN_ERROR;
	}

    if (iResult == PLUGIN_ERROR) {
        UTIL_LogPrintf( "[ADMIN] ERROR: Executing plugin_init on plugin %s returned PLUGIN_FAILURE\n", m_sFile);
        Cleanup();
        return PLUGIN_ERROR;
    } 

	return static_cast<plugin_result>(iResult);
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
