/*
 * ===========================================================================
 *
 * $Id: users.cpp,v 1.43 2004/08/21 19:28:41 darope Exp $
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
 * routines for handling user functions for the admin_ modules
 *
 */


#include <cstring>

#ifndef _WIN32
#  include <regex.h>
#  include <crypt.h>
#else
#  include "gnuregex.h"
#  include "ufc_crypt.h"
#endif



#if (defined(USE_MYSQL) || defined(USE_PGSQL))
#  ifdef USE_MYSQL
#    include <mysql.h>
#    include <errmsg.h>

#    include "extdll.h"
  extern MYSQL mysql;
  extern BOOL g_fUseMySQL;
#  endif

#  ifdef USE_PGSQL
#    include <libpq-fe.h>

#    include "extdll.h"
  extern PGconn *pgsql;
  extern BOOL g_fUsePgSQL;
#  endif
#else
#  include "extdll.h"
#endif

#include <meta_api.h>
#include "md5.h"
#include "users.h"
#include "amutil.h"
#include "amlibc.h"
#include "AmFSNode.h"

// This is it; the linked lists that we store all of our
// loaded admin mod data in.  Each list gets initialized in
// the corresponding Load*() functions (eg, m_pModelList
// gets initialized in LoadModels(), with the exception of
// m_pHelpList, which is initialized in LoadPlugins(),
// and m_pSpawnList, which is called in InitSpawnEntityList,
// and not loaded from a file at all)
CLinkList<help_struct>* m_pHelpList   = nullptr;
CLinkList<ip_struct>* m_pIPList     = nullptr;
CLinkList<model_struct>* m_pModelList  = nullptr;
CLinkList<CPlugin>* m_pPluginList = nullptr;
CLinkList<spawn_struct>* m_pSpawnList  = nullptr;
CLinkList<user_struct>* m_pUserList   = nullptr;
CLinkList<vault_struct>* m_pVaultList  = nullptr;
CLinkList<word_struct>* m_pWordList   = nullptr;

// Auth structure array.  +1 so we can go from 0 - MAX_PLAYERS, rather
// than 0 - MAX_PLAYERS - 1.
auth_struct g_AuthArray[MAX_PLAYERS + 1];


// Second auth struct array to keep a backup of auths. Only needed to overcome
// the inability of CS to do proper mapchanges
struct AuthBak {
	enum { SIZE = MAX_PLAYERS + (MAX_PLAYERS / 2) };

	auth_struct Array[SIZE];

	AuthBak() : m_head(0) { memset( Array, SIZE, 0 ); };
    int get_head() { if ( m_head >= SIZE ) m_head = 0; return m_head++; };
    int head() { if ( m_head >= SIZE ) m_head = 0; return m_head; };
    void clear() { memset( Array, SIZE, 0 ); };

private:
	int m_head;
};

static AuthBak g_AuthBak;


void clear_auth_bak_array() {
    g_AuthBak.clear();
} 

// The index of the current highlander.  This is set by
// GetUserAccess, and is used for tracking when the highlander
// changes.  Only useful if admin_highlander is != 0.
static int m_iHighlanderIndex = 0;

// The current spawn entity; each entity spawned should increment
// this by one.
static int m_iSpawnIdentity = 1;

// The current user index; this is used by LoadUsers() as it parses
// the incoming user records.
static int m_iUserIndex = 1;

const int c_iRecnctWindow = 30;


DLL_GLOBAL BOOL  g_fInitialized;
DLL_GLOBAL BOOL  g_fIPsLoaded;
DLL_GLOBAL BOOL  g_fModelsLoaded;
DLL_GLOBAL BOOL  g_fUsersLoaded;
DLL_GLOBAL BOOL  g_fVaultLoaded;
DLL_GLOBAL BOOL  g_fWordsLoaded;
DLL_GLOBAL BOOL  g_fRunPlugins;
DLL_GLOBAL BOOL  g_fRunScripts;

mapcycle_t mapcycle;

// These are to check if we left the regular mapcycle. Needed to return the proper nextmap.
int g_iForcedMapChange = 0;
char* g_pcNextMap = nullptr;
char g_acNextMap[BUF_SIZE];


/***************************
*
* Miscellaneous functions
*
***************************/
// Gets the file in sFilename (relative to the game's mod dir), and returns it as a 
// linked list of the lines. Empty lines aren't returned, and neither are commented
//  out lines (determined by is_comment) or those greater than LINE_SIZE in length.
// May return a nullptr pointer.  The linked list returned must be delete()'d, of course.
CLinkList<char, true>* GetFile(char* sFilename, CLinkList<char,true>* _pPreLineList) {
  int iBegin = 0;
  int iEnd = 0;
  int iLength;
  CLinkList<char,true>* pLineList = _pPreLineList;

  // Load the file up.
  char* sFile = reinterpret_cast<char*>(LOAD_FILE_FOR_ME(sFilename, &iLength));
  
  // Check to make sure that we've actually got a file, and it's actually got data.
  if (!(sFile && iLength)) {
	if ( _pPreLineList == nullptr ) {
		// This is a master file
    	UTIL_LogPrintf("[ADMIN] WARNING: File '%s' seems to be empty (length %i).\n", sFilename, iLength);
	} else {
		// This is a file included via #include
    	UTIL_LogPrintf("[ADMIN] WARNING: Included file '%s' seems to be empty (length %i).\n", sFilename, iLength);
	}  // if-else

  } else {
	// Allocate a new linked list if we didn't get one passed
    if ( _pPreLineList == nullptr ) pLineList = new CLinkList<char,true>();

    // Now, while we've still got data left...
    while (iEnd < iLength) {
      // Skip over any prefixed whitespace (ie, keep going until we hit real data)
      while (isspace(sFile[iBegin]) || sFile[iBegin]=='\n' || sFile[iBegin]=='\r')
	iBegin++;

      // We found the beginning of real data.  Starting here, go until we hit a 
      // line feed, or the end of the file.
      iEnd = iBegin;
      while (sFile[iEnd] != '\n' && sFile[iEnd] != '\r' && iEnd < iLength) 
	iEnd++;
      
      // Make sure what we have is not a blank line
      if ((iEnd - iBegin) <= 1) {
	// Do nothing

	// Make sure the line's not too long
      } else if ((iEnd - iBegin) > LINE_SIZE) {
	UTIL_LogPrintf("[ADMIN] ERROR: File '%s' has line too long (%i chars).  Skipping line.\n", sFilename, (iEnd - iBegin));

	// Check if this is an include directive
	  } else if ( strncmp(&sFile[iBegin], "#include ", 9) == 0 ) {
		// Looks like an include directive. 
		char* pIncludeFile = &sFile[iBegin+9];

		DEBUG_LOG(1, (" Looks like an include directive.") ); 
		// Skip whitespace.
		while ( isspace(*pIncludeFile) ) ++pIncludeFile;

		// Check if this is a <> include
		if ( *pIncludeFile == '<' ) {
			// Note the beginning of the filename
			++pIncludeFile;

			// Now make sure by searching for the closing tag.
			char* pEndTag = pIncludeFile;
			while ( *pEndTag != '>' && pEndTag < sFile+iEnd ) ++pEndTag;

			// If we found an end tag, this is an include directive.
			if ( *pEndTag == '>' && pEndTag != pIncludeFile ) {
				// Zero-terminate the filename and call us recursively with the filename
				*pEndTag = '\0';
				DEBUG_LOG(1, ("Parsing included file <%s>.", pIncludeFile) );
				GetFile( pIncludeFile, pLineList );
				DEBUG_LOG(1, ("Parsing continues with parent file '%s'.", sFilename) );
			}  // if

		// check if this is an "" include
		} else if ( *pIncludeFile == '"' ) {
			// Note the beginning of the filename
			++pIncludeFile;

			// Now make sure by searching for the closing tag.
			char* pEndTag = pIncludeFile;
			while ( *pEndTag != '"' && pEndTag < sFile+iEnd ) ++pEndTag;

			// If we found an end tag, this is an include directive.
			if ( *pEndTag == '"' && pEndTag != pIncludeFile ) {
				// Now we need to build the path name.
				// Start with a copy of the passed filename
				char acIncludeFile[PATH_MAX];
				memset( acIncludeFile, 0, PATH_MAX );
				am_strncpy( acIncludeFile, sFilename, PATH_MAX );

				// Find the start of the actual file name
				char* pPos = acIncludeFile;
				char* pLastDir = acIncludeFile;
				while ( *pPos != '\0' ) {
					// skip to the end of the full path, remembering the position of the last directory seperator
					if ( *pPos == '/' ) {
						pLastDir = pPos;
#ifdef WIN32
						// While we're at it, convert to windows format
						*pPos = '\\';
#endif
                    } else if ( *pPos == '\\' ) {
						pLastDir = pPos;
#ifdef LINUX
                        // While we're at it, convert to Linux format
						*pPos = '/';
#endif
					}  // if-else
					++pPos;
				}  // while
				
				// if we had directories, position us after the last slash
#ifdef LINUX
				if ( *pLastDir == '/' ) ++pLastDir;
#endif
#ifdef WIN32
				if ( *pLastDir == '\\' ) ++pLastDir;
#endif

				// Phew, now we have the path to the parent file. We shall now append 
				// the relative path of the include file
				memcpy( pLastDir, pIncludeFile, (pEndTag-pIncludeFile) );
			
				// zero-terminate
				*(pLastDir + (pEndTag-pIncludeFile)) = '\0';
	
				// Mmmmkay, finally time to call us recursively with the new filename
				DEBUG_LOG(1, ("Parsing included file \"%s\".", acIncludeFile) );
				GetFile( acIncludeFile, pLineList );
				DEBUG_LOG(1, ("Parsing continues with parent file '%s'.", sFilename) );
			}  // if
		}  // if-else

		// Otherwise it wasn't an include directive after all and we just return.

	// Make sure it's not a comment
      } else if (!is_comment(&sFile[iBegin])) {
	// It passes.  Add it in.
	char* sLine = new char[LINE_SIZE];
	strncpy(sLine, &sFile[iBegin], iEnd - iBegin);
	// Don't forget to null-terminate.
	sLine[iEnd - iBegin] = '\0';
	// Strip off ending whitespace
	int iPos = strlen(sLine) - 1;
	while (iPos > 0 && isspace(sLine[iPos]))
	  iPos--;
	if (iPos >= 0)
	  sLine[iPos + 1] = '\0';
	
	if (strlen(sLine) > 0) 
	  pLineList->AddLink(sLine);
	
      }
      iBegin = iEnd;
    }
  }
  // Don't forget to free the file, now...
  FREE_FILE(sFile);
  return pLineList;
}

inline CLinkList<char, true>* GetFile(char* sFilename) {
	return GetFile( sFilename, nullptr );
}

// Returns the number of free (non-reserved slots).  Can be given an entity
// to not 'count' in determining this number (useful if being called when said
// entity is disconnecting, for example).
int GetFreeSlots(edict_t* pEntityIgnore) {
	const int iPlayerCount = GetPlayerCount( pEntityIgnore );
	const int iResType = (int)CVAR_GET_FLOAT("reserve_type");
	int iResSlots = 0;
	
	// If we're ignoring someone, then we should decrement our player
	// count to count for the ignored party.
	//if (pEntityIgnore != nullptr && iPlayerCount > 0) iPlayerCount--;
	
	// If the player count is greater than or equal to our max number
	// of players, there are obviously no free slots.
	if (iPlayerCount >= gpGlobals->maxClients) {
		return 0;
	}
	
	// Determine the number of reserved slots.
	// For reserved type 0, just get the reserve_slots cvar.
	if (iResType == 0) {
		iResSlots = (int)CVAR_GET_FLOAT("reserve_slots");
		// For reserve type 1, the number of reserved slots is always going to be 1.
	} else if (iResType == 1) {
		iResSlots = 1;
		// For reserve type 2, the number of reserved slots is the cvar, decremented
		// by one for each slot actually taken already.
	} else if (iResType == 2) {
		iResSlots = (int)CVAR_GET_FLOAT("reserve_slots");

		for (int i = 1; i <= gpGlobals->maxClients; i++) {
			CBaseEntity* pPlayer = UTIL_PlayerByIndex(i);
			if(IsPlayerValid(pPlayer) && pPlayer->edict() != pEntityIgnore) {
				if (IsIPReserved(g_AuthArray[i].sIP) || (GetUserAccess(pPlayer->edict()) & ACCESS_RESERVE_SPOT) ) {
					iResSlots--;
					// Obviously, if we have no more reserved slots, we can stop this
					// part of the process.
					if (iResSlots <= 0) 
						break;
				}
			}
		}
	}
	// We can't have a negative number of reserved slots.
	if (iResSlots < 0)
		iResSlots = 0;
	
	// If the number of players + the number of reserved slots is greater than
	// the maximum number of players, there's nothing free.
	if (iPlayerCount + iResSlots >= gpGlobals->maxClients) {
		return 0;
		// Otherwise, the difference is the number of reserved slots.
	} else {
		return (gpGlobals->maxClients - iPlayerCount - iResSlots);
	}
}




// Resets all vars, etc, back to their original states
void InitAdminModData(BOOL fFull = FALSE, BOOL fRun = FALSE) {
  if (fFull) {
    UTIL_LogPrintf("[ADMIN] Fully initializing data.\n");
  } else {
    UTIL_LogPrintf("[ADMIN] Initializing data.\n");
  }

  if (fFull) {
    g_fInitialized = FALSE;
    g_fRunPlugins  = FALSE;
    g_fRunScripts  = FALSE;
  }
  
  g_fIPsLoaded    = FALSE;
  g_fModelsLoaded = FALSE;
  g_fUsersLoaded  = FALSE;
  g_fVaultLoaded  = FALSE;
  g_fWordsLoaded  = FALSE;
  
  if (fFull) {
    // No need to reset the auth records.  They get reset on connect
    // and disconnect anyways, and we need to preserve the passwords
    // across maps.
    // memset(g_AuthArray,0x0,sizeof(auth_struct)*(MAX_PLAYERS + 1));
    InitSpawnEntityList();
  }
  
  if (fRun) {
    LoadIPs();
    LoadModels();
    LoadUsers();
    LoadVault();
    LoadWords();
  }

  int iResSlots = (int)CVAR_GET_FLOAT("reserve_slots"); 
  if ( iResSlots <= 0 ) {
	iResSlots = 0;
  } else if ( iResSlots >= gpGlobals->maxClients ) {
	iResSlots =gpGlobals->maxClients;
  }  // if-else
 
  CVAR_SET_FLOAT( "public_slots_free", GetFreeSlots(nullptr) );

  if ( (int)CVAR_GET_FLOAT("amv_hide_reserved_slots") != 0 && iResSlots > 0 ) {
	CVAR_SET_FLOAT( "sv_visiblemaxplayers", (gpGlobals->maxClients - iResSlots) );
  }  // if

  // If we get called from admin_reload, we should update the user auth info, 
  // in case a password or access level changed.

  if ( fRun && !fFull ) {
	  for (int i = 1; i <= gpGlobals->maxClients; i++) {
		  CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
		  if ( IsPlayerValid(pPlayer) ) {
			  VerifyUserAuth( STRING(pPlayer->pev->netname), pPlayer->edict() );
		  }  // if
	  }  // for
  }  // if

}  // InitAdminModData()


// This procedure is a generic function that will load records from a file.  Given a
// user-readable type (for the messages), a linked list (which will get initialized),
// a cvar (holding the file name to read), and a function pointer, it will call the
// function pointer for each valid line in the file pointed to by the cvar.

template<class T, bool isArray> void LoadFile(char* sType, CLinkList<T,isArray>* pList, char* sFileVar, PARSE_FILE fpParse) {
	char* sFile = const_cast<char*>(get_cvar_file_value( sFileVar ));  // TODO: remove necessity for dirty const cast? 

	if (pList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: LoadFile for '%s' called with nullptr linked list.\n", sType);
    return;
  }
  pList->Init();
  
  // Make sure we have a file.
  if ( sFile == nullptr ) {
	DEBUG_LOG(1, ("LoadFile::%s cvar not set.  No %ss loaded.\n", sFileVar, sType) );
    return;
  }
  if ((int)CVAR_GET_FLOAT("admin_debug") >= 2) {
	  char acFile[PATH_MAX];
	  (*g_engfuncs.pfnGetGameDir)(acFile);
	  if ( (strlen(acFile) + strlen(sFile)) >= PATH_MAX ) {
		  am_strncpy( acFile, sFile, PATH_MAX );
	  } else {
		  strcat( acFile, "/" );
		  strcat( acFile, sFile );
	  }  // if-else
	  FormatPath(acFile);
	  UTIL_LogPrintf("[ADMIN] Loading %ss from file '%s'\n",sType,acFile);
	  
  } else {
	  FormatPath(sFile);
	  UTIL_LogPrintf("[ADMIN] Loading %ss from file '%s'\n",sType,sFile);
  }  // if-else

  // Get the file in linked-list format.
  CLinkList<char, true>* pFile = GetFile(sFile);
  if (pFile != nullptr) {
    // For every line in the file...
    CLinkItem<char, true>* pLine = pFile->FirstLink();
    while (pLine != nullptr) {
      // Parse the line.
      fpParse(pLine->Data());
      // Get the next line
      pLine = pLine->NextLink();
    }
    // Don't forget to clean up the linked list afterwards.
    delete(pFile);
  }
  UTIL_LogPrintf("[ADMIN] Loaded %i %ss\n", pList->Count(), sType);
}

#ifdef USE_MYSQL
// This function is like LoadFile, above, but works on a MySQL database table
// instead of a file.  It takes one more parameter than LoadFile, that being the
// SQL query to execute.  This SQL query should contain one %s token, which will
// be filled with the name of the db table.
// Note that LoadTable returns a value; if LoadTable returns FALSE, the code
// will attempt to call LoadFile, above.

template<class T, bool isArray> BOOL LoadTable(char* sType, CLinkList<T,isArray>* pList, char* sTableVar, char* sSQL, PARSE_MYSQL fpParse) {
  int iResult;
  char sQuery[QUERY_BUF_SIZE];
  char* sTable = const_cast<char*>(get_cvar_string_value( sTableVar )); // TODO: remove necessity for dirty const cast?
  MYSQL_RES *pResult;
  MYSQL_ROW pRow = nullptr;

// if its a user query and we don't want to preload the file exit right now
  if( (FStrEq(sType,"user") || FStrEq(sType,"tags")) && (int)CVAR_GET_FLOAT("mysql_preload") == 0) return TRUE;

  if (pList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: LoadTable for '%s' called with nullptr linked list.\n", sType);
    return FALSE;
  }
  pList->Init();

  // Make sure that MySQL is initialized, and that we have a table.
  if (g_fUseMySQL == FALSE) {
    return FALSE;
  } else if ( sTable == nullptr ) {
	  DEBUG_LOG(1, ("LoadTable::%s cvar empty.  Switching to loading from file...\n", sTableVar) );
    return FALSE;
  }





  // Select everything in the table.
  UTIL_LogPrintf("[ADMIN] Loading %ss from MySQL table '%s'\n", sType, sTable);
  snprintf(sQuery, QUERY_BUF_SIZE, sSQL, sTable);
  iResult = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
  if (iResult) {
    if (mysql_errno(&mysql) == CR_SERVER_LOST || mysql_errno(&mysql) == CR_SERVER_GONE_ERROR) {
      iResult = mysql_ping(&mysql);
      if (!iResult) {
        UTIL_LogPrintf("[ADMIN] WARNING: Select query for %ss returned disconnect, reconnect succeeded.\n", sType);
        iResult = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
      } else {
        UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned disconnect, reconnect failed.\n", sType);
        return FALSE;
      }
    } else if (mysql_errno(&mysql) == CR_COMMANDS_OUT_OF_SYNC) {
      UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned commands out of sync!  NO FURTHER SQL QUERIES WILL WORK.\n", sType);
      return FALSE;
    } else {
      UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned error: \"%s\"\n", sType, mysql_error(&mysql));
      return FALSE;
    }
  }
  if (!iResult) {
    pResult = mysql_use_result(&mysql);
  }
  // Make sure we got a valid result back.
  if (iResult || !pResult) {
    UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned nullptr result.\n", sType);
  } else {
    // For every row in the result set...
    while ((pRow = mysql_fetch_row(pResult)) != nullptr) {

      //**** NOTE - NEVER to MySQL stuff in the parse function, except for
      //         reading row data! Otherwise we could get out of sync errs
      (*fpParse)(pRow);
    }
    mysql_free_result(pResult);
  }
  UTIL_LogPrintf("[ADMIN] Loaded %i %ss\n", pList->Count(), sType);
  return TRUE;
}
#endif

#ifdef USE_PGSQL
// This function is like LoadFile, above, but works on a PgSQL database table
// instead of a file.  It takes one more parameter than LoadFile, that being the
// SQL query to execute.  This SQL query should contain one %s token, which will
// be filled with the name of the db table.
// Note that LoadPgTable returns a value; if LoadPgTable returns FALSE, the code
// will attempt to call LoadFile, above.

template<class T, bool isArray> BOOL LoadPgTable(char* sType, CLinkList<T,isArray>* pList, char* sTableVar, char* sSQL, PARSE_PGSQL fpParse) {
  char sQuery[QUERY_BUF_SIZE];
  char* sTable = const_cast<char*>(get_cvar_string_value( sTableVar ));  // TODO: remove necessity for dirty const cast?
  PGresult *pgResult;
  int tup_num;

// if its a user query and we don't want to preload the file exit right now
  if( (FStrEq(sType,"user") || FStrEq(sType,"tags")) && (int)CVAR_GET_FLOAT("pgsql_preload") == 0) return TRUE;

  if (pList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: LoadPgTable for '%s' called with nullptr linked list.\n", sType);
    return FALSE;
  }
  pList->Init();

  // Make sure that PgSQL is initialized, and that we have a table.
  if (g_fUsePgSQL == FALSE) {
    return FALSE;
  } else if ( sTable == nullptr ) {
	  DEBUG_LOG(1, ("LoadPgTable::%s cvar empty.  Switching to loading from file...\n", sTableVar) );
    return FALSE;
  }

  // Select everything in the table.
  UTIL_LogPrintf("[ADMIN] Loading %ss from PgSQL table '%s'\n", sType, sTable);
  snprintf(sQuery, QUERY_BUF_SIZE, sSQL, sTable);
  if (!(pgResult = admin_pgsql_query(sQuery, "usertable")))
    return FALSE;

  // Make sure we got a valid result back.
  if (!PQntuples(pgResult)) {
    UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned nullptr result.\n", sType);
  } else {
    // For every row in the result set...
    for (tup_num = 0; tup_num < PQntuples(pgResult); tup_num++) {

      (*fpParse)(pgResult, tup_num);
    }
    PQclear(pgResult);
  }
  UTIL_LogPrintf("[ADMIN] Loaded %i %ss\n", pList->Count(), sType);
  return TRUE;
}
#endif

//
// Returns 1 if the strings match, 0 otherwise.  'Matching' may include
// a regex check, if use_regex is not 0; otherwise, it'll be a straight
// string check.
//
int match(const char *string, char *pattern) {
  // Null pointers match anything.
  if ( string == nullptr || pattern == nullptr ) {
    return 1;
    // Empty strings match nothing except other empty strings (this keeps
    // empty passwords from matching any pattern)
  } else if ( string[0] == '\0' && pattern[0] != '\0' ) {
    return 0;
  } else if ( string[0] != '\0' && pattern[0] == '\0' ) {
    return 0;
    // If we're not using regex, just do a simple case-insensitive comparison.
  } else if ((int)CVAR_GET_FLOAT("use_regex")==0)  {
    return (!stricmp(string, pattern));
    // Otherwise, do regex stuff.
  } else {
	  regex_t    re;
    
    if ( regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB|REG_ICASE) != 0) {
      return(0);      // report error
    }
	  const int status = regexec(&re, string, (size_t)0, nullptr, 0);
    regfree(&re);
    if (status != 0) {
      return(0);      // report error
    }
    return(1);
  }
}

//
// Returns a pointer to static data containing the password encrypted with the 
// method specified in the encrypt_password cvar.
// encrypt_password: == 0 none , == 1 crypt() , == 2 MD5 , == 3 mysqlPASSWORD
//
// pcPassword is the password to be encrypted;
// pcRefPassword is a reference password, e.g. to get the salt for crypt. 
//    Usually the password the server has (via users.ini or similar).
//
const char* pass_encrypt( const char* _pcPassword, const char* _pcRefPassword) {

	static char acEncryptPw[PASSWORD_SIZE];
	const char* pcEncryptPw = nullptr;

	const int iEncryptMethod = (int)CVAR_GET_FLOAT( "encrypt_password" );
 
	if ( _pcPassword == nullptr ) {
		UTIL_LogPrintf("[ADMIN] ERROR: pass_encrypt called with nullptr pointer\n");
		return nullptr;
	}  // if
	

	switch ( iEncryptMethod ) {
	case 1:
		if ( _pcRefPassword == nullptr ) {
			UTIL_LogPrintf("[ADMIN] ERROR: pass_encrypt called with nullptr reference password, method crypt()\n");
			return nullptr;
		}  // if
		// Encrypt with a salt based on the user password.
		pcEncryptPw = crypt( _pcPassword, _pcRefPassword);
		break;

	case 2:
		MD5_CTX md5Ctx;
		MD5Init( &md5Ctx );
		MD5Update( &md5Ctx, reinterpret_cast<const unsigned char*>(_pcPassword), strlen(_pcPassword) );
		MD5Final( &md5Ctx );
		sprintmd5( acEncryptPw, md5Ctx.digest );
		pcEncryptPw = acEncryptPw;
		break;

	case 3:
#ifdef USE_MYSQL
		// use the mySQL PASSWORD() function for encryption
		pcEncryptPw = am_mysql_password_encrypt( _pcPassword );
#else 
		// No encryption here.  Just use the password as-is.
		pcEncryptPw = _pcPassword;
#endif  /* USE_MYSQL */
		break;

	default:
		// No valid encryption method: do not encrypt.
		pcEncryptPw = _pcPassword;
	}  // switch

	DEVEL_LOG(3, ("Encrypted pw '%s' is '%s'.", _pcPassword, pcEncryptPw) );

	return pcEncryptPw;
}

//
// Returns 1 if the passwords match, 0 otherwise. 'Matching' may include
// encryption, if encrypt_passwords is not 0.  Otherwise, it'll be straight
// string matching.
// sServerPassword is the password the server has (in users.ini, or whatever);
// sPlayerPassword is the password the player has (via admin_password, or whatever).
//
int pass_compare( const char* sServerPassword, const char* sPlayerPassword) {
	const char *sEncrypt = nullptr;
	
	if ( sServerPassword == nullptr || sPlayerPassword == nullptr ) {
		UTIL_LogPrintf( "[ADMIN] ERROR: pass_compare called with nullptr pointer\n" );
		return 0;
	}  // if
	
	// if the server password is empty, we don't need one.
	if ( sServerPassword[0] == '\0' ) return 1;
	
	// Encrypt the password 
	sEncrypt = pass_encrypt( sPlayerPassword, sServerPassword );

    if (nullptr == sEncrypt ) {
    	UTIL_LogPrintf ( "[ADMIN] ERROR: pass_compare: encryption returned an error\n" );
    	return 0;
    }

	DEVEL_LOG(3, ("Comparing server pw '%s' with client pw '%s'", sServerPassword, sEncrypt) );

	// Return the comparison between the two.
	return( !strncmp(sEncrypt,sServerPassword,PASSWORD_SIZE) );
}



/***************************
 *
 * Model functions
 *
 ***************************/
// Returns 1 if the entity has access to use sModel, 0 otherwise.
int GetModelAccess(char* sModel, edict_t* pEntity) {
	const int iIndex = ENTINDEX(pEntity);
  model_struct tModel;
  
  // Verify the entity is valid
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetModelAccess: User '%s' has out of bounds entity index %i\n", STRING(pEntity->v.netname), iIndex);
    return 0;
  }
  // If a model exists with the given name, and the user's password, then they
  // have access.  Otherwise, they don't.
  return (GetModelRecord(sModel, g_AuthArray[iIndex].sPassword, &tModel) ? 1 : 0);
}

// Given a model name, and optionally password, fills the model_struct record 
// with the first info that matches that name/password combination.  A nullptr password
// bypasses the password match (useful for testing for existance).
// Returns TRUE if successful, FALSE otherwise.
BOOL GetModelRecord(char* sModel, char* sPassword, model_struct* tModelRecord) {
  // Make sure we're passed a pointer to use.
  if (tModelRecord == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetModelRecord called with nullptr tModelRecord.\n");
    return FALSE;
  }
  
  // Make sure the models are loaded.
  if (g_fModelsLoaded == FALSE)
    LoadModels();
  
  // If we have no model list, we can't match.
  if (m_pModelList == nullptr) 
    return FALSE;

  CLinkItem<model_struct>* pModel = m_pModelList->FirstLink();
  model_struct* tModel = nullptr;

  // Go through each model in the list.
  while (pModel != nullptr) {
    tModel = pModel->Data();
    // If the model's names match...
    if (match(sModel,tModel->sModelName)==1) {
      // A nullptr password matches anything; otherwise, we need
      // to compare passwords.
      if (sPassword == nullptr) {
	break;
      } else if (pass_compare(tModel->sPassword,sPassword)) {
	break;
      }
    }
    pModel = pModel->NextLink();
  }
  
  // We got a match
  if (pModel != nullptr) {
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf( "[ADMIN] DEBUG: Model '%s' matches model entry '%s'\n",sModel,tModel->sModelName);
    }
    memcpy(tModelRecord,tModel,sizeof(model_struct));
    return TRUE;
  }
  // No matching model, huh?
  return FALSE;
}

// Returns TRUE if a model is reserved, FALSE otherwise.
// A model is reserved if there is any entry for it (with any password).
BOOL IsModelReserved(char* sModel) {
  model_struct tModel;
  
  return GetModelRecord(sModel, nullptr, &tModel);
}

// Loads a model record from line in a file.  Returns TRUE if successful, 
// FALSE otherwise. The format is:
// <model name>:<password>
BOOL ParseModel(char* sLine) {
  char sDelimiter[] = ":";

  char* sNameToken = strtok(sLine, sDelimiter);
  if (sNameToken == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: No Model name found: '%s'\n", sLine);
  } else if ((int)strlen(sNameToken) > USERNAME_SIZE) {
    UTIL_LogPrintf("[ADMIN] ERROR: Model name too long: '%s'\n", sNameToken);
  } else {
    char* sPasswordToken = strtok(nullptr, sDelimiter);
    if (sPasswordToken == nullptr) {
      UTIL_LogPrintf("[ADMIN] ERROR: No Model password found: '%s'\n", sLine);
    } else if ((int)strlen(sPasswordToken) > PASSWORD_SIZE) {
		if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
			UTIL_LogPrintf("[ADMIN] ERROR: Model password too long: '%s'\n", sPasswordToken);
		} else {
			UTIL_LogPrintf("[ADMIN] ERROR: Model password too long.\n");
		}  // if-else			
    } else {
      model_struct* tModel = new model_struct;
      if(tModel == nullptr) {
	UTIL_LogPrintf( "[ADMIN] ERROR: ParseModel::'new' failed for tModel record.\n");
	return FALSE;
      }
      memset(tModel,0x0,sizeof(model_struct));
      strcpy(tModel->sModelName,sNameToken);
      strcpy(tModel->sPassword,sPasswordToken);
      m_pModelList->AddLink(tModel);
      
      if((int)CVAR_GET_FLOAT("amv_log_passwords") == 1) {
		  DEBUG_LOG(1, ("Model loaded: Name '%s', Password '%s'",tModel->sModelName, tModel->sPassword) );
	  } else {
		  DEBUG_LOG(1, ("Model loaded: Name '%s', Password '********'",tModel->sModelName) );		  
      }
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef USE_MYSQL
// Loads a model record from a MySQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseModelSQL(MYSQL_ROW pRow) {
  model_struct* tModel;

  tModel = new model_struct;
  if(tModel == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseModelSQL::'new' failed for tModel record.\n");
    return FALSE;
  }
  memset(tModel,0x0,sizeof(model_struct));
  strcpy(tModel->sModelName,pRow[0]);
  strcpy(tModel->sPassword,pRow[1]);
  m_pModelList->AddLink(tModel);

      if((int)CVAR_GET_FLOAT("amv_log_passwords") == 1) {
		  DEBUG_LOG(1, ("Model loaded: Name '%s', Password '%s'",tModel->sModelName, tModel->sPassword) );
	  } else {
		  DEBUG_LOG(1, ("Model loaded: Name '%s', Password '********'",tModel->sModelName) );		  
      }
  return TRUE;
}
#endif

#ifdef USE_PGSQL
// Loads a model record from a PgSQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseModelPgSQL(const PGresult *res, int tup_num) {
  model_struct* tModel;

  tModel = new model_struct;
  if(tModel == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseModelPgSQL::'new' failed for tModel record.\n");
    return FALSE;
  }
  memset(tModel,0x0,sizeof(model_struct));
  strcpy(tModel->sModelName,PQgetvalue(res, tup_num, 0));
  strcpy(tModel->sPassword,PQgetvalue(res, tup_num, 1));
  m_pModelList->AddLink(tModel);

      if((int)CVAR_GET_FLOAT("amv_log_passwords") == 1) {
		  DEBUG_LOG(1, ("Model loaded: Name '%s', Password '%s'",tModel->sModelName, tModel->sPassword) );
	  } else {
		  DEBUG_LOG(1, ("Model loaded: Name '%s', Password '********'",tModel->sModelName) );		  
      }
  return TRUE;
}
#endif

void LoadModels() {
  // Make sure we're not already loaded.
  if (g_fModelsLoaded == TRUE)
    return;
  // Note that we're now loaded.
  g_fModelsLoaded = TRUE;

  // Create a new linked list, if necessary.
  if (m_pModelList == nullptr)
    m_pModelList = new CLinkList<model_struct>();

  // Load the Models
#ifdef USE_MYSQL
  if (!LoadTable("model", m_pModelList, "mysql_dbtable_models", "SELECT nick,pass FROM %s\n", &ParseModelSQL))
#endif
#ifdef USE_PGSQL
   if (!LoadPgTable("model", m_pModelList, "pgsql_dbtable_models", "SELECT nick,pass FROM %s\n", &ParseModelPgSQL))
#endif
    LoadFile("model", m_pModelList, "models_file", &ParseModel);
}


void UnloadModels() {
  g_fModelsLoaded = FALSE;

  if (m_pModelList == nullptr) return;
  delete m_pModelList;
  m_pModelList = nullptr;

}


/***************************
 *
 * IP functions
 *
 ***************************/
// Given an IP string (xxx.xxx.xxx.xxx), returns it in unsigned long (32-bit) format.
void IPStringToBits(char* sIP, ulong* lIP) {
	char* sChar = sIP;
  
  *lIP = 0x00000000;
  for( int iShift = 24; *sChar && iShift >= 0; iShift -= 8) {
	  ulong lByte = atoi(sChar);
    if (lByte > 255) 
      lByte = 255;
    *lIP |= lByte << iShift;
    while (isdigit(*sChar))
      sChar++;
    if (*sChar == '.')
      sChar++;
  }
}

// Given an unsigned long (32-bit) IP, returns it in string format (xxx.xxx.xxx.xxx)
void IPBitsToString(ulong lNbo, char* sIP) {
  sprintf(sIP, "%d.%d.%d.%d", (int)((lNbo>>24)&0xFF),(int)((lNbo>>16)&0xFF),(int)((lNbo>>8)&0xFF),(int)((lNbo)&0xFF));
}

// Returns TRUE if a given string is in a valid IP format, FALSE otherwise.
BOOL IsIPValid( const char* sIP ) {
  int iDotCount = 0;
  int iDigCount = 0;
  const char* sChar = sIP;
  
  // If the string is nullptr, it's obviously invalid.
  if (sIP == nullptr) {
    return FALSE;
    // If the string is too big, it's invalid.
  } else if ((int)strlen(sIP) > IP_SIZE) {
    return FALSE;
  } else {
	  while (*sChar != '\0') {
		  if (*sChar == '.') {
			  iDotCount++;
			  // If the string has more than three periods, it's invalid.
			  if (iDotCount > 3) return FALSE;
			  // if we have less than one or more than three digits between dots, it's invalid
			  if ( (iDigCount < 1) || (iDigCount > 3) ) return FALSE;
			  // else reset the digit count
			  iDigCount = 0;
		  // If the string has characters that aren't digits and aren't periods, it's invalid.
		  } else if (!isdigit(*sChar)) {
			  return FALSE;
		  } else {
			  iDigCount++;
		  }  // if-else
		  sChar++;
	  }  // while
  }  // if-else
  // if we have less than one or more than three digits after the last dot, it's invalid
  if ( (iDigCount < 1) || (iDigCount > 3) || (iDotCount < 3) ) return FALSE;
  // Otherwise, it's good.
  return TRUE;
}

// Returns TRUE if a given IP matches any records in the IP list, FALSE otherwise.
// An IP matches if there is an IP record where, when the mask is applied to both
// the IP and the record, the result is the same.
// (Ie, if the mask is '0.0.0.0', any IPs will match.  If the mask is '255.255.255.255',
// the IPs have to be exactly the same to match.  If the mask is '255.255.255.0', the IPs
// have to be the same for the first 24 bits to match, but the last 8 bits can be
// completely different.)
BOOL IsIPReserved(char *sIP) {
  // Make sure the IPs are loaded.
  if (g_fIPsLoaded == FALSE)
    LoadIPs();
  
  // If we have no IP list, we can't match.
  if (m_pIPList == nullptr)
    return FALSE;
  
  CLinkItem<ip_struct>* pLink = m_pIPList->FirstLink();
  ip_struct* tIP = nullptr;
  ulong lIP;
  
  // Convert the IP to bits.
  IPStringToBits(sIP,&lIP);
  // For each record...
  while (pLink != nullptr) {
    tIP = pLink->Data();
    // If the record's IP and mask equals the given IP and mask, we have a match.
    if ((tIP->lIP & tIP->lMask) == (lIP & tIP->lMask)) {
      return TRUE;
    }
    pLink = pLink->NextLink();
  }
  // Otherwise, no match.
  return FALSE;
}

// Loads an IP record from a line in a file.  Returns TRUE if successful,
// FALSE otherwise. The format is:
// <IP address - xxx.xxx.xxx.xxx>[/<mask - xxx.xxx.xxx.xxx>]
BOOL ParseIP(char* sLine) {
	char sDelimiter[] = "/";

	char* sIPToken = strtok(sLine, sDelimiter);
  if (sIPToken == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: No IP found: '%s'\n", sLine);
  } else if ((int)strlen(sIPToken) > IP_SIZE) {
    UTIL_LogPrintf("[ADMIN] ERROR: IP too long: '%s'\n", sIPToken);
  } else if (!IsIPValid(sIPToken)) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Invalid IP address: %s\n", sIPToken);
  } else {
    // It's allowable to not have a mask.
    int iHasMask = 0;
    char* sMaskToken = strtok(nullptr, sDelimiter);
    if (sMaskToken != nullptr) {
      if ((int)strlen(sMaskToken) > IP_SIZE) {
	UTIL_LogPrintf("[ADMIN] ERROR: IP mask too long: '%s'\n", sMaskToken);
      } else if (!IsIPValid(sMaskToken)) {
	UTIL_LogPrintf( "[ADMIN] ERROR: Invalid IP Mask: %s\n", sMaskToken);
      } else {
	iHasMask = 1;
      }
    }
    ip_struct* tIP = new ip_struct;
    if(tIP == nullptr) {
      UTIL_LogPrintf( "[ADMIN] ERROR: LoadIPs::'new' failed for tIP record.\n");
      return FALSE;
    }
    memset(tIP,0x0,sizeof(ip_struct));
    strcpy(tIP->sIP, sIPToken);
    IPStringToBits(tIP->sIP, &tIP->lIP);
    // If we have a mask, use it; otherwise, use a 255.255.255.255 (full) mask.
    if (iHasMask == 1) {
      strcpy(tIP->sMask, sMaskToken);
    } else {
      strcpy(tIP->sMask,"255.255.255.255");
    }
    IPStringToBits(tIP->sMask, &tIP->lMask);
    m_pIPList->AddLink(tIP);
    
    if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
      UTIL_LogPrintf( "[ADMIN] DEBUG: IP loaded: IP '%s', Mask '%s'\n",tIP->sIP, tIP->sMask);
    }
    return TRUE;
  }
  return FALSE;
}

#ifdef USE_MYSQL
// Loads an IP record from a MySQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseIPSQL(MYSQL_ROW pRow) {
  ip_struct* tIP;

  tIP = new ip_struct;
  if(tIP == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseIPSQL::'new' failed for tIP record.\n");
    return FALSE;
  }
  memset(tIP,0x0,sizeof(ip_struct));
  strcpy(tIP->sIP, pRow[0]);
  IPStringToBits(tIP->sIP, &tIP->lIP);
  strcpy(tIP->sMask,"255.255.255.255");
  IPStringToBits(tIP->sMask, &tIP->lMask);
  m_pIPList->AddLink(tIP);

  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf( "[ADMIN] DEBUG: IP loaded: IP '%s', Mask '%s'\n",tIP->sIP, tIP->sMask);
  }
  return TRUE;
}
#endif

#ifdef USE_PGSQL
// Loads an IP record from a PgSQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseIPPgSQL(const PGresult *res, int tup_num) {
  ip_struct* tIP;

  tIP = new ip_struct;
  if(tIP == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseIPPgSQL::'new' failed for tIP record.\n");
    return FALSE;
  }
  memset(tIP,0x0,sizeof(ip_struct));
  strcpy(tIP->sIP, PQgetvalue(res, tup_num, 0));
  IPStringToBits(tIP->sIP, &tIP->lIP);
  strcpy(tIP->sMask,"255.255.255.255");
  IPStringToBits(tIP->sMask, &tIP->lMask);
  m_pIPList->AddLink(tIP);

  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf( "[ADMIN] DEBUG: IP loaded: IP '%s', Mask '%s'\n",tIP->sIP, tIP->sMask);
  }
  return TRUE;
}
#endif

void LoadIPs() {
  // Make sure we're not already loaded.
  if (g_fIPsLoaded == TRUE)
    return;
  // Note that we're now loaded.
  g_fIPsLoaded = TRUE;

  // Create a new linked list, if necessary.
  if (m_pIPList == nullptr)
    m_pIPList = new CLinkList<ip_struct>();

  // Load the IPs
#ifdef USE_MYSQL
  if (!LoadTable("IP", m_pIPList, "mysql_dbtable_ips", "SELECT ip FROM %s\n", &ParseIPSQL))
#endif
#ifdef USE_PGSQL
   if (!LoadPgTable("IP", m_pIPList, "pgsql_dbtable_ips", "SELECT ip FROM %s\n", &ParseIPPgSQL))
#endif
    LoadFile("IP", m_pIPList, "ips_file", &ParseIP);
}

void UnloadIPs() {
  g_fIPsLoaded = FALSE;

  if (m_pIPList == nullptr) return;

  delete m_pIPList;
  m_pIPList = nullptr;
}


// Utility function to get a users IP address from other functions
// Given the users server index it sets the pointer argument to the
// users IP address in the auth struct.
bool user_ip( int _iIndex, const char** const _pcIP, ulong* _plIP ) {

	if ( g_AuthArray[_iIndex].sIP != nullptr && g_AuthArray[_iIndex].sIP[0] != 0 ) {

		if ( _pcIP != nullptr ) *_pcIP = g_AuthArray[_iIndex].sIP;

		if ( _plIP != nullptr ) {
			IPStringToBits( g_AuthArray[_iIndex].sIP, _plIP );
		}  // if

		return true;
	}  // if


	return false;
}  // user_ip()

/***************************
 *
 * Word detection functions
 *
 ***************************/

// Loads a word record from a line in a file.  Returns TRUE if successful,
// FALSE otherwise.  The format is:
// <word>
BOOL ParseWord(char* sLine) {
	word_struct* tWord = new word_struct;
  if(tWord == nullptr) {
    UTIL_LogPrintf( "[ADMIN] ERROR: ParseWord::'new' failed for tWord record.\n");
    return FALSE;
  }
  memset(tWord,0x0,sizeof(word_struct));
  strcpy(tWord->sWord,sLine);

  for (int i = 0; i < (int)strlen(tWord->sWord); i++)
    tWord->sWord[i] = tolower(tWord->sWord[i]);

  m_pWordList->AddLink(tWord);
  
  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf( "[ADMIN] DEBUG: Word loaded: '%s'\n",tWord->sWord);
  }
  return TRUE;
}

#ifdef USE_MYSQL
// Loads a word record from a MySQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseWordSQL(MYSQL_ROW pRow) {
  char sWord[LINE_SIZE];
  
  strcpy(sWord, pRow[0]);
  return ParseWord(sWord);
}
#endif

#ifdef USE_PGSQL
// Loads a word record from a PgSQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseWordPgSQL(const PGresult *res, int tup_num) {
  char sWord[LINE_SIZE];
  
  strcpy(sWord, PQgetvalue(res, tup_num, 0));
  return ParseWord(sWord);
}
#endif

void LoadWords() {
  // Make sure we're not already loaded.
  if (g_fWordsLoaded == TRUE)
    return;
  // Note that we're now loaded.
  g_fWordsLoaded = TRUE;
  
  // Create a new linked list, if necessary.
  if (m_pWordList == nullptr) 
    m_pWordList = new CLinkList<word_struct>();
  
  // Load the words
#ifdef USE_MYSQL
  if (!LoadTable("word", m_pWordList, "mysql_dbtable_words", "SELECT word FROM %s\n", &ParseWordSQL))
#endif
#ifdef USE_PGSQL
   if (!LoadPgTable("word", m_pWordList, "pgsql_dbtable_words", "SELECT word FROM %s\n", &ParseWordPgSQL))
#endif
    LoadFile("word", m_pWordList, "words_file", &ParseWord);
}



void UnloadWords() {
  g_fWordsLoaded = FALSE;
  
  if (m_pWordList == nullptr) return;

  delete m_pWordList;
  m_pWordList = nullptr;
}


/***************************
 *
 * User functions
 *
 ***************************/


// Another dirty trick needed to keep passwords safe in the current version.
// Hopefully this can be removed again in a future version.

typedef struct TmpPwd {
  time_t iSet;
  char acPwd[PASSWORD_SIZE];
} tmppwd_t;

static tmppwd_t m_aoTmpPwd[MAX_PLAYERS+1];


// Look up a player's enitity pointer by providing the server index,
// the session id or the won id.

edict_t* get_player_edict( uint32_t _uiID, uidt _uidType ) {

    int i;
	switch ( _uidType ) {
	case uid_index:
		if ( _uiID < 1 || _uiID > gpGlobals->maxClients ) return nullptr;
		return g_AuthArray[_uiID].pPlayerEdict;
		break;

	case uid_sessionID:
		for ( i = 1; i <= gpGlobals->maxClients; i++ ) {
			if ( g_AuthArray[i].iSessionID == _uiID ) return g_AuthArray[i].pPlayerEdict;
		};
		break;

	case uid_wonID:
		for ( i = 1; i <= gpGlobals->maxClients; i++ ) {
			if ( g_AuthArray[i].oaiAuthID == _uiID ) return g_AuthArray[i].pPlayerEdict;
		};
		break;

	}  // switch

	return nullptr;
}  // get_player_edict()


edict_t* get_player_edict( const AMAuthId& _oaiID, uidt _uidType ) {
	for ( int i = 1; i <= gpGlobals->maxClients; i++ ) {
		if ( g_AuthArray[i].oaiAuthID == _oaiID ) return g_AuthArray[i].pPlayerEdict;
	};

	return nullptr;
}  // get_player_edict()



// Initializes the authorization record for the given user.  Also calls
// SetUserPassword and VerifyUserAuth in attempt to find user's access
// level.
void AddUserAuth(char* sName, char* sIP, edict_t* pEntity) {
	bool bAuthNeeded = true;
	char cTmpChar;
	char* pcIpPortSep = strchr(sIP,':');
	const int iIndex = ENTINDEX(pEntity);
  int iPrevIndex = 0;
  int iPort = 0;
  AMAuthId oaiAuthID;
  int iReconnTime = 300;
  int iReconnectWindow = (int)CVAR_GET_FLOAT("amv_reconnect_time");
  auth_struct* poPrevAuth = nullptr;
  auth_struct* poPrevAuthBak = nullptr;
	const time_t ttiNow = time(nullptr);

  if ( iReconnectWindow > 90 ) iReconnectWindow = 90;

  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: AddUserAuth: User '%s' has out of bounds entity index %i\n", sName, iIndex);
    return;
  }

  if ( (int)CVAR_GET_FLOAT("sv_lan") ) {
    oaiAuthID = nsAuthid::LAN_ID;
  } else {
    oaiAuthID = GETPLAYERAUTHID( pEntity );
  }  // if-else

	const int iSessionID = GETPLAYERUSERID(pEntity); 
  
  iReconnTime = (int)CVAR_GET_FLOAT( "admin_reconnect_timeout" );


  DEVEL_LOG(2, ("Prv(idx:%i): Index: %i, SessionID: %i, AuthID: %s, Time: %i, IP: '%s:%i', Name: '%s', Access: %i", 
				iIndex,
				g_AuthArray[iIndex].iIndex,
				g_AuthArray[iIndex].iSessionID,
				static_cast<const char*>(g_AuthArray[iIndex].oaiAuthID),
				g_AuthArray[iIndex].iTime,
				g_AuthArray[iIndex].sIP,
				g_AuthArray[iIndex].iPort,
				g_AuthArray[iIndex].sUserName,
				g_AuthArray[iIndex].iAccess) ); 
  DEVEL_LOG(2, ("Cur(idx:%i): Index: %i, SessionID: %i, AuthID: %s, Time: %i, IP: '%s', Name: '%s'", 
				iIndex,
				iIndex,
				iSessionID,
				static_cast<const char*>(oaiAuthID),
				ttiNow,
				sIP, 
				sName) ); 

  // Seperate IP from port
  if ( pcIpPortSep ) {
	  cTmpChar = *pcIpPortSep;
	  iPort = atoi( pcIpPortSep +1);
	  *pcIpPortSep = 0;
  }



  if (   g_AuthArray[iIndex].oaiAuthID == oaiAuthID
	  && g_AuthArray[iIndex].iSessionID == iSessionID
	  && ((ttiNow - g_AuthArray[iIndex].iTime) < 180) 
	  // && g_AuthArray[iIndex].iPort == iPort 
	  && strcmp(sName, g_AuthArray[iIndex].sUserName) == 0
      && strcmp(sIP, g_AuthArray[iIndex].sIP) == 0 ) {
    DEBUG_LOG( 1, ("AddUserAuth: Mapchange for index %i.", iIndex) );
    g_AuthArray[iIndex].iTime = ttiNow;
	g_AuthArray[iIndex].pPlayerEdict = pEntity;
	g_AuthArray[iIndex].iIndex = iIndex;
 
    // No need to check for a changed password
    bAuthNeeded = false;

  } else if (   g_AuthArray[iIndex].oaiAuthID == oaiAuthID 
			 && ((ttiNow - g_AuthArray[iIndex].iTime) < iReconnTime) 
			 // && g_AuthArray[iIndex].iPort == iPort
			 && strcmp(sName, g_AuthArray[iIndex].sUserName) == 0
			 && strcmp(sIP, g_AuthArray[iIndex].sIP) == 0 ) {
	DEBUG_LOG( 1, ("AddUserAuth: Reconnect for index %i.", iIndex) );
    if ( iReconnTime > 600 ) {
      UTIL_LogPrintf("[ADMIN] WARNING: Your reconnect timeout is higher than 600 seconds. This may pose a security risk.\n" );
      UTIL_LogPrintf("                 It is recommended to have a timeout of only 300 seconds.\n" );
    }  // if
    g_AuthArray[iIndex].iTime = ttiNow;
	g_AuthArray[iIndex].iSessionID = iSessionID;
	g_AuthArray[iIndex].pPlayerEdict = pEntity;
	g_AuthArray[iIndex].iIndex = iIndex;

  } else {
	// Somewhat dirty hack to overcome the map change reauth problems with CS
	// This has to be revised for better security.

	// setting the reconnect time window to 0 disables this feature
	if ( iReconnectWindow != 0 ) {
	  if ( !oaiAuthID.is_lanid() ) {
		  // check if we have a record with the same AuthID, IP:port and NAME
		  // in the AuthArray
		  for ( int i = 1; i <= gpGlobals->maxClients; i++ ) {
	   		  if ( g_AuthArray[i].oaiAuthID == oaiAuthID
				   //&& g_AuthArray[i].iPort == iPort
				   && strcmp(sIP, g_AuthArray[i].sIP) == 0
				   && strcmp(sName, g_AuthArray[i].sUserName) == 0) {
				  poPrevAuth = &g_AuthArray[i];
				  iPrevIndex = i;
				  break;
			  }  // if
		  }  // for

		  // if there was none, check if we have a record with the same AuthID, 
		  // IP:port and NAME in the AuthArray backup copy
		  if ( poPrevAuth == nullptr ) {
			  for ( int i = 0; i < g_AuthBak.SIZE; i++ ) {
				  if ( g_AuthBak.Array[i].oaiAuthID == oaiAuthID
					   //&& g_AuthBak.Array[i].iPort == iPort
					   && strcmp(sIP, g_AuthBak.Array[i].sIP) == 0 
					   && strcmp(sName, g_AuthBak.Array[i].sUserName) == 0) {
					  poPrevAuthBak = &(g_AuthBak.Array[i]);
					  iPrevIndex = i;
					  break;
				  }  // if
			  }  // for
		  }  // if
	  } else {
		// no need to check for the AuthID, since it is a LAN id anyway
		// we just search for the same name and IP:port
		// wow this is getting friggin insecure
		for ( int i = 1; i <= gpGlobals->maxClients; i++ ) {
		  if ( strcmp(sIP, g_AuthArray[i].sIP) == 0
			   && g_AuthArray[i].iPort == iPort
			   && strcmp(sName, g_AuthArray[i].sUserName) == 0) {
			poPrevAuth = &g_AuthArray[i];
			iPrevIndex = i;
			break;
		  }  // if
		}  // for

		// again, if we didn't find it, check the backup copies
		if ( poPrevAuth == nullptr ) {
			for ( int i = 0; i < g_AuthBak.SIZE; i++ ) {
				if ( strcmp(sIP, g_AuthBak.Array[i].sIP) == 0
				     &&  g_AuthBak.Array[i].iPort == iPort
					 && strcmp(sName, g_AuthBak.Array[i].sUserName) == 0) {
					poPrevAuthBak = &(g_AuthBak.Array[i]);
					iPrevIndex = i;
					break;
				}  // if
			}  // for
		}  // if
	  }  // if-else
	}  // if

	if ( poPrevAuth && ((ttiNow - poPrevAuth->iTime) < iReconnectWindow) && !UTIL_PlayerByIndex(iPrevIndex) ) {
		// If we found this user in a different auth struct 
		// and his recconection happened within x seconds ( specified in iReconnectWindow ) 
		// and the user we found is no longer connected
		DEBUG_LOG(1, ("AddUserAuth: Reconnect for index %i into new slot %i within %us.",
					  iPrevIndex, iIndex, (ttiNow - poPrevAuth->iTime)) );
    	// make a back copy of the user data in the slot this user connects into
        DEBUG_LOG( 2, ("AddUserAuth: Making backup copy of data for index %i into backup index %i.", iIndex, g_AuthBak.head()) );
        memcpy( &(g_AuthBak.Array[g_AuthBak.get_head()]), &g_AuthArray[iIndex], sizeof(auth_struct) );
 
    	// then copy his data over and reauth him 
		memcpy( &g_AuthArray[iIndex], poPrevAuth, sizeof(auth_struct) );
    	// don't forget to update the elements that changed on this connect
		g_AuthArray[iIndex].iTime = ttiNow;
        g_AuthArray[iIndex].iSessionID = iSessionID;
		g_AuthArray[iIndex].pPlayerEdict = pEntity;
		g_AuthArray[iIndex].iIndex = iIndex;
		// I say he is dirty as hell
		g_AuthArray[iIndex].state = auth_struct::dirty;

	} else if ( poPrevAuthBak && ((ttiNow - poPrevAuthBak->iTime) < iReconnectWindow) ) {
		// If we found this user in a different auth struct backup copy
		// and his recconection happened within x seconds ( specified in iReconnectWindow ) 
		DEBUG_LOG(1, ("AddUserAuth: Reconnect for index %i into new slot %i (BAK) within %us",
					  iPrevIndex, iIndex, (ttiNow - poPrevAuthBak->iTime)) );
 
		// make a back copy of the user data in the slot this user connects into
        DEBUG_LOG( 2, ("AddUserAuth: Making backup copy of data for index %i into backup index %i.", iIndex, g_AuthBak.head()) );
        memcpy( &(g_AuthBak.Array[g_AuthBak.get_head()]), &g_AuthArray[iIndex], sizeof(auth_struct) );
 
		// then copy his data over and reauth him
		memcpy( &g_AuthArray[iIndex], poPrevAuthBak, sizeof(auth_struct) );
		// don't forget to update the elements that changed on this connect
		g_AuthArray[iIndex].iTime = ttiNow;
		g_AuthArray[iIndex].iSessionID = iSessionID;
		g_AuthArray[iIndex].pPlayerEdict = pEntity;
		g_AuthArray[iIndex].iIndex = iIndex;
        // I say he is dirty as hell
        g_AuthArray[iIndex].state = auth_struct::dirty;

		// clean up the backup copy of the record.
		memset( poPrevAuthBak, 0, sizeof(auth_struct) );

	} else {

		if ( poPrevAuth ) {
			DEBUG_LOG( 1, ("AddUserAuth: User %s access rights timed out: %lus, limit:%is.", 
						   sName, (ttiNow - poPrevAuth->iTime), iReconnectWindow) );
		} else if ( poPrevAuthBak ) {
			DEBUG_LOG( 1, ("AddUserAuth: User %s access rights (BAK) timed out: %lus, limit:%is.", 
						   sName, (ttiNow - poPrevAuthBak->iTime), iReconnectWindow) );
		} else {
			DEBUG_LOG( 1, ("AddUserAuth: No matching previous record available for user %s.", sName) );
		}  // if-else
			
	  // Clear and repopulate the record.
	  DEBUG_LOG( 2, ("AddUserAuth: Making backup copy of data for index %i into backup index %i.", iIndex, g_AuthBak.head()) );
	  memcpy( &(g_AuthBak.Array[g_AuthBak.get_head()]), &g_AuthArray[iIndex], sizeof(auth_struct) );

	  DEBUG_LOG( 1, ("AddUserAuth: Clearing index %i for new connect.", iIndex) );
	  memset(&g_AuthArray[iIndex],0x0,sizeof(auth_struct));
	  g_AuthArray[iIndex].iAccess = CVAR_GET_FLOAT("default_access");
	  g_AuthArray[iIndex].iSessionID = iSessionID;
	  g_AuthArray[iIndex].oaiAuthID = oaiAuthID;
	  g_AuthArray[iIndex].iTime = ttiNow;
	  g_AuthArray[iIndex].iUserIndex = 0;
	  g_AuthArray[iIndex].iIndex = iIndex;
	  g_AuthArray[iIndex].pPlayerEdict = pEntity;
	  g_AuthArray[iIndex].iPort = iPort;
	  strncpy(g_AuthArray[iIndex].sUserName, sName, USERNAME_SIZE);
	  //strcpy(g_AuthArray[iIndex].sPassword,"");
	  strncpy(g_AuthArray[iIndex].sIP,sIP,IP_SIZE);

	}  // if-else

  }  // if-else

  if ( bAuthNeeded ) {
    // Try to get the user's password from the setinfo buffer
    SetUserPassword(sName, nullptr, pEntity);
    // Try to find a matching user record. We do this in any case or don't we?
    VerifyUserAuth(sName, pEntity);
  }

  DEVEL_LOG(2, ("Now(idx:%i): Index: %i, SessionID: %i, AuthId: %s, Time: %i, IP: '%s:%i', Name: '%s', Access: %i", 
				iIndex,
				g_AuthArray[iIndex].iIndex,
				g_AuthArray[iIndex].iSessionID,
				static_cast<const char*>(g_AuthArray[iIndex].oaiAuthID),
				g_AuthArray[iIndex].iTime,
				g_AuthArray[iIndex].sIP,
				g_AuthArray[iIndex].iPort,
				g_AuthArray[iIndex].sUserName,
				g_AuthArray[iIndex].iAccess) );  

  // Lets quickly restore the IP string.
  if ( pcIpPortSep ) *pcIpPortSep = cTmpChar;

}
// Returns the index of the Highlander, or 0 if there is no Highlander.
// The Highlander is returned only if admin_highlander is non-zero; the
// Highlander is the person online with the most access.  In the case of
// ties, it is the person with the most access and the smallest user index
// (ie, the one closest to the top of the list)
int GetHighlanderIndex( edict_t* pIgnoreEntity ) {
	int iPlayerIndex = 0;
  int iIgnoreIndex = 0;
  int iHighlanderIndex = 0;
  int iMaxAccess = 0;
  int iMinUserIndex = 9999;
  CBaseEntity* pPlayer;

  // We may want to ignore a player when searching for the Highlander,
  // e.g. when that person is about to disconnect.  
  if ( pIgnoreEntity != nullptr ) iIgnoreIndex = ENTINDEX( pIgnoreEntity );

  // If admin_highlander is 0, there is no highlander.
  if((int)CVAR_GET_FLOAT("admin_highlander")==0) {
    return 0;
  }
  // Otherwise, get the highest access with the smallest user index.
  for(int i = 1; i <= gpGlobals->maxClients; i++) {
	  pPlayer = UTIL_PlayerByIndex( i );
    if ( i != iIgnoreIndex && g_AuthArray[i].iUserIndex != 0 && IsPlayerValid(pPlayer) ) {
		// This person beats the current highlander if:
		// 1) their access is greater, or
		// 2) their access is the same and their user index is smaller
		if (g_AuthArray[i].iAccess > iMaxAccess || (g_AuthArray[i].iAccess == iMaxAccess && g_AuthArray[i].iUserIndex < iMinUserIndex)) {
			iHighlanderIndex = i;
			iMaxAccess = g_AuthArray[i].iAccess;
			iMinUserIndex = g_AuthArray[i].iUserIndex;
		}
    }
  }
  // If the highlander we found is different from the last one...
  if (iHighlanderIndex != m_iHighlanderIndex) {
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: Old Highlander index: %i\n", m_iHighlanderIndex);
    }
    // If there is an old highlander, try to let him know he's no longer.
    if (m_iHighlanderIndex != 0) {
      pPlayer = UTIL_PlayerByIndex(m_iHighlanderIndex);
      if (IsPlayerValid(pPlayer)) {
		  ClientPrint(&pPlayer->edict()->v, HUD_PRINTTALK, "You are no longer the Admin Highlander!\n");
		  CLIENT_PRINTF( pPlayer->edict(), print_console, "You are no longer the Admin Highlander!\n");
      }
    }
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: New Highlander index: %i\n", iHighlanderIndex);
    }
    // If there is a new highlander, try to let him know he's the new one.
    if (iHighlanderIndex != 0) {
      pPlayer = UTIL_PlayerByIndex(iHighlanderIndex);
      if (IsPlayerValid(pPlayer)) {
		  ClientPrint(&pPlayer->edict()->v, HUD_PRINTTALK, "You are the Admin Highlander! There can be only one!\n");
		  CLIENT_PRINTF(pPlayer->edict(), print_console, "You are the Admin Highlander! There can be only one!\n");
      }
    }
    m_iHighlanderIndex = iHighlanderIndex;
  }
  return iHighlanderIndex;
}

// Returns the access associated with an entity.
int GetUserAccess(edict_t* pEntity) {
	// Console has complete access.
  if (pEntity == nullptr) {
    return -1;
  }
	const int iIndex = ENTINDEX(pEntity);
  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetUserAccess: User '%s' has out of bounds entity index %i\n", STRING(pEntity->v.netname), iIndex);
    return 0;
  }
  // Check to see if we're in highlander mode.  If we are, anybody besides the
  // highlander should only get reserve-name, reserve-spot, immunity, and default access.
	const int iHighlanderIndex = GetHighlanderIndex();
  if (iHighlanderIndex != 0 && iHighlanderIndex != iIndex) {
    return (g_AuthArray[iIndex].iAccess & (ACCESS_RESERVE_NICK | ACCESS_RESERVE_SPOT | ACCESS_IMMUNITY | (int)
	    CVAR_GET_FLOAT("default_access")));
    // Otherwise, return the person's access.
  } else {
	// for a listenserver user we add reserved nick and slot access
	return g_AuthArray[iIndex].iAccess | (g_AuthArray[iIndex].oaiAuthID.is_loopid() ? 
											(ACCESS_RESERVE_NICK|ACCESS_RESERVE_SPOT) : 0 );
  }
}

// Given a user name and/or WON ID, and optionally password, fills the
// user_struct record with the first info that matches that name/password
// combination.  A nullptr password bypasses the password match (useful
// for testing for existance). A nullptr user record means testing for
// existance only. No record is returned.
// Returns TRUE if successful, FALSE otherwise.
BOOL GetUserRecord(const char* sName, const AMAuthId& oaiAuthID, const char* sIP, char* sPassword, user_struct* ptUserRecord) {
  // If we have no record to write to we only do test. Else clear the record.
  if (ptUserRecord == nullptr) {
    DEBUG_LOG(2, ("GetUserRecord testing for existance") );
  } else {
	  memset( ptUserRecord, 0, sizeof(user_struct) );
  }  // if-else


  if ( sName == nullptr ) sName = "";
  if ( sIP == nullptr ) sIP = "0.0.0.0";

  // For a listenserver user we don't check the password
  if ( oaiAuthID.is_loopid() ) sPassword = nullptr;

  // Make sure the users are loaded.
  if (g_fUsersLoaded == FALSE)
    LoadUsers();

  user_struct *ptUser = nullptr;

  CLinkItem<user_struct>*  pUser= nullptr;

#ifdef USE_MYSQL

 user_struct User; // temporary store for user info pulled from db
 //int iResult;      // used for db func return values
 // char *sType="user"; // the "type" of query we are doing
 MYSQL_RES *pResult;
 MYSQL_ROW pRow = nullptr;	
 BOOL valid_user=FALSE;

 if( (int)CVAR_GET_FLOAT("mysql_preload")==0 ) {
	 

	 //TODO: Why is this here and never used? Looks like a bug to me.
	 bool bUsersTableIsSet = true;
	 bool bTagsTableIsSet = true;
	 const char* pcCVar = get_cvar_string_value( "mysql_dbtable_users" );
	 if ( pcCVar == nullptr ) {
		 bUsersTableIsSet = false;
		 DEBUG_LOG(2, ("mySQL users table is not set.") );
	 }  // if
	 pcCVar = get_cvar_string_value( "mysql_dbtable_tags" );
	 if ( pcCVar == nullptr ) {
		 bTagsTableIsSet = false;
		 DEBUG_LOG(2, ("mySQL tags table is not set.") );
	 }  // if

	 char sQuery[QUERY_BUF_SIZE];

   memset( &User, 0, sizeof(user_struct) );

   snprintf(sQuery,QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("mysql_users_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_users"),escape_chars(sName, '\''),(const char*)oaiAuthID);

   pResult=admin_mysql_query(sQuery,"user");
  
   // Make sure we got a valid result back.
   if ( !pResult) {
	   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
   } else {

	   // For every row in the result set...
	   while ((pRow = mysql_fetch_row(pResult)) != nullptr) {
		   valid_user=TRUE; // there was at least one user...
                   if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                      DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", pRow[0], pRow[1]) );
                   } else {
                      DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", pRow[1]) );
                   }  // if-else
	   if ( sPassword == nullptr ) {
			   // The nullptr password matches anything, setup the Users struct and copy it across
			   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
			   am_strncpy(User.sPassword,pRow[0],PASSWORD_SIZE);
			   User.iAccess=atoi(pRow[1]);
			   User.iIndex= GetPlayerIndex((char*)sName);
			   ptUser=&User;
			   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
			   // KLUDGE, KLUDE!! sets pUser to non null to show a match
			   pUser=(CLinkItem<user_struct, false> *)1;
			   break;
		   } else if(pass_compare(pRow[0],sPassword)) {
			   // if the users PW matches the DB one then setup the Users struct and copy it across
			   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
			   am_strncpy(User.sPassword,sPassword,PASSWORD_SIZE);
			   User.iAccess=atoi(pRow[1]);
			   User.iIndex= GetPlayerIndex((char*)sName);
			   ptUser=&User;
			   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
			   // KLUDGE, KLUDE!! sets pUser to non null to show a match
			   pUser=(CLinkItem<user_struct, false> *)1;
			   break;
		   }  // if-else
   }  // while
	   mysql_free_result(pResult);
   }


   if ( valid_user==FALSE ) { // try to search for an IP
	   snprintf(sQuery,QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("mysql_users_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_users"),sIP,(const char*)oaiAuthID);

	   pResult=admin_mysql_query(sQuery,"user");

	   // Make sure we got a valid result back.
	   if ( !pResult) {
		   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
	   } else {

		   // For every row in the result set...
		   while ((pRow = mysql_fetch_row(pResult)) != nullptr) {
                        if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                           DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", pRow[0], pRow[1]) );
                        } else {
                           DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", pRow[1]) );
                        }  // if-else
			   valid_user=TRUE; // there was at least one user...
			   if ( sPassword == 0 ) {
				   // The nullptr password matches anything, setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,pRow[0],PASSWORD_SIZE);
				   User.iAccess=atoi(pRow[1]);
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   } else if(pass_compare(pRow[0],sPassword)) {
				   // if the users PW matches the DB one then setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,sPassword,PASSWORD_SIZE);
				   User.iAccess=atoi(pRow[1]);
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   }  // if-else
		   }  // while
		   mysql_free_result(pResult);
	   }

   }  // if


   if (valid_user==FALSE && bTagsTableIsSet ) {
	   snprintf(sQuery,QUERY_BUF_SIZE,(char*)CVAR_GET_STRING("mysql_tags_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_tags"),escape_chars(sName,'\''),(const char*)oaiAuthID);
	   if ((int)CVAR_GET_FLOAT("admin_debug") != 0)
		   UTIL_LogPrintf("[ADMIN]Trying tags db with %s\n",sQuery);

	   pResult=admin_mysql_query(sQuery,"user");

	   // Make sure we got a valid result back.
	   if ( !pResult) {
		   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
	   } else {

		   // For every row in the result set...
		   while ((pRow = mysql_fetch_row(pResult)) != nullptr) {
                       if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                          DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", pRow[0], pRow[1]) );
                       } else {
                          DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", pRow[1]) );
                       }  // if-else

			   if ( sPassword == 0 ) {
				   // The nullptr password machtes anything, setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,pRow[0],PASSWORD_SIZE);
				   User.iAccess=atoi(pRow[1]);
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   } else if(pass_compare(pRow[0],sPassword)) {
				   // if the users PW matches the DB one then setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,pRow[0],PASSWORD_SIZE);
				   User.iAccess=atoi(pRow[1]);
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   }  // if-else

		   }  // while
		   mysql_free_result(pResult);

	   } // if-else
   } // if(valid_user==FALSE)



 } else {
#endif

#ifdef USE_PGSQL
 user_struct User; // temporary store for user info pulled from db
 // char *sType="user"; // the "type" of query we are doing
 PGresult *pgResult;  // store for result set
 int tup_num;        // current tuple number in result set
 BOOL pg_valid_user=FALSE;

 if( (int)CVAR_GET_FLOAT("pgsql_preload")==0 ) {
	 
	 //TODO: Why is this here and never used? Looks like a bug to me.
	 bool bUsersTableIsSet = true;
	 bool bTagsTableIsSet = true;
	 const char* pcCVar = get_cvar_string_value( "pgsql_dbtable_users" );
	 if ( pcCVar == nullptr ) {
		 bUsersTableIsSet = false;
		 DEBUG_LOG(2, ("PgSQL users table is not set.") );
	 }  // if
	 pcCVar = get_cvar_string_value( "pgsql_dbtable_tags" );
	 if ( pcCVar == nullptr ) {
		 bTagsTableIsSet = false;
		 DEBUG_LOG(2, ("PgSQL tags table is not set.") );
	 }  // if

	 char sQuery[QUERY_BUF_SIZE];

   memset( &User, 0, sizeof(user_struct) );

   snprintf(sQuery,QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("pgsql_users_sql"),(char*)CVAR_GET_STRING("pgsql_dbtable_users"),escape_chars(sName, '\''),(const char*)oaiAuthID);

   pgResult=admin_pgsql_query(sQuery,"user");
  
   // Make sure we got a valid result back.
   if ( !pgResult) {
	   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
   } else {

	   // For every row in the result set...
	   for (tup_num = 0; tup_num < PQntuples(pgResult); tup_num++) {
		   pg_valid_user=TRUE; // there was at least one user...
                   if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                      DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", PQgetvalue(pgResult, tup_num, 0), PQgetvalue(pgResult, tup_num, 1)) );
                   } else {
                      DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", PQgetvalue(pgResult, tup_num, 1)) );
                   }  // if-else
		   if ( sPassword == nullptr ) {
			   // The nullptr password matches anything, setup the Users struct and copy it across
			   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
			   am_strncpy(User.sPassword,PQgetvalue(pgResult, tup_num, 0),PASSWORD_SIZE);
			   User.iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));
			   User.iIndex= GetPlayerIndex((char*)sName);
			   ptUser=&User;
			   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
			   // KLUDGE, KLUDE!! sets pUser to non null to show a match
			   pUser=(CLinkItem<user_struct, false> *)1;
			   break;
		   } else if(pass_compare(PQgetvalue(pgResult, tup_num, 0),sPassword)) {
			   // if the users PW matches the DB one then setup the Users struct and copy it across
			   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
			   am_strncpy(User.sPassword,sPassword,PASSWORD_SIZE);
			   User.iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));
			   User.iIndex= GetPlayerIndex((char*)sName);
			   ptUser=&User;
			   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
			   // KLUDGE, KLUDE!! sets pUser to non null to show a match
			   pUser=(CLinkItem<user_struct, false> *)1;
			   break;
		   }  // if-else
	   }  // for
	   PQclear(pgResult);
   }


   if ( pg_valid_user==FALSE ) { // try to search for an IP
	   snprintf(sQuery,QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("pgsql_users_sql"),(char*)CVAR_GET_STRING("pgsql_dbtable_users"),sIP,(const char*)oaiAuthID);

	   pgResult=admin_pgsql_query(sQuery,"user");

	   // Make sure we got a valid result back.
	   if ( !pgResult) {
		   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
	   } else {

		   // For every row in the result set...
		   for (tup_num = 0; tup_num < PQntuples(pgResult); tup_num++) {
                        if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                           DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", PQgetvalue(pgResult, tup_num, 0), PQgetvalue(pgResult, tup_num, 1)) );
                        } else {
                           DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", PQgetvalue(pgResult, tup_num, 1)) );
                        }  // if-else
			   pg_valid_user=TRUE; // there was at least one user...
			   if ( sPassword == 0 ) {
				   // The nullptr password matches anything, setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,PQgetvalue(pgResult, tup_num, 0),PASSWORD_SIZE);
				   User.iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   } else if(pass_compare(PQgetvalue(pgResult, tup_num, 0),sPassword)) {
				   // if the users PW matches the DB one then setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,sPassword,PASSWORD_SIZE);
				   User.iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   }  // if-else
		   }  // for
		   PQclear(pgResult);
	   }

   }  // if


   if (pg_valid_user==FALSE && bTagsTableIsSet ) {
	   snprintf(sQuery,QUERY_BUF_SIZE,(char*)CVAR_GET_STRING("pgsql_tags_sql"),(char*)CVAR_GET_STRING("pgsql_dbtable_tags"),escape_chars(sName,'\''),(const char*)oaiAuthID);
	   if ((int)CVAR_GET_FLOAT("admin_debug") != 0)
		   UTIL_LogPrintf("[ADMIN]Trying tags db with %s\n",sQuery);

	   pgResult=admin_pgsql_query(sQuery,"user");

	   // Make sure we got a valid result back.
	   if ( !pgResult) {
		   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
	   } else {

		   // For every row in the result set...
		   for (tup_num = 0; tup_num < PQntuples(pgResult); tup_num++) {
                       if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                          DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", PQgetvalue(pgResult, tup_num, 0), PQgetvalue(pgResult, tup_num, 1)) );
                       } else {
                          DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", PQgetvalue(pgResult, tup_num, 1)) );
                       }  // if-else

			   if ( sPassword == 0 ) {
				   // The nullptr password machtes anything, setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,PQgetvalue(pgResult, tup_num, 0),PASSWORD_SIZE);
				   User.iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   } else if(pass_compare(PQgetvalue(pgResult, tup_num, 0),sPassword)) {
				   // if the users PW matches the DB one then setup the Users struct and copy it across
				   am_strncpy(User.sUserName,sName,USERNAME_SIZE);
				   am_strncpy(User.sPassword,PQgetvalue(pgResult, tup_num, 0),PASSWORD_SIZE);
				   User.iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));
				   User.iIndex= GetPlayerIndex((char*)sName);
				   ptUser=&User;
				   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
				   // KLUDGE, KLUDE!! sets pUser to non null to show a match
				   pUser=(CLinkItem<user_struct, false> *)1;
				   break;
			   }  // if-else

		   }  // for
		   PQclear(pgResult);

	   } // if-else
   } // if(pg_valid_user==FALSE)



 } else {
#endif
    // If we have no user list, we can't match.
    if (m_pUserList == nullptr)
      return FALSE;

   pUser= m_pUserList->FirstLink();
   // user_struct* ptUser = nullptr;

    // For each user record...
    while (pUser != nullptr) {
		//TBR char* pcEndptr = 0; int iNum = 0;
		ptUser = pUser->Data();
		AMAuthId oaiID = ptUser->sUserName;
		//TBR	iNum = strtol( ptUser->sUserName, &pcEndptr, 10 );

		// Try to get a match; this match can be a name/name match, or a name/WONID match, or a name/IP match.
		if ( (!oaiID.is_set() && !IsIPValid(sName) && match(sName,ptUser->sUserName)!=0)   // a user name
			 || (oaiID.is_set() && oaiID == oaiAuthID)                // an auth id
			 || (IsIPValid(sIP) && match(sIP, ptUser->sUserName)) ) { // a valid IP number
			// A nullptr password matches anything.  Otherwise, we need to compare
			// passwords.
			if (sPassword == nullptr) {
				break;
			} else if (pass_compare(ptUser->sPassword,sPassword)) {
				break;
			}
		}
		pUser = pUser->NextLink();
    }  // while
#ifdef USE_PGSQL
 }  // if-else
#endif
#ifdef USE_MYSQL
 }  // if-else
#endif

  // We got a match.
  if (pUser != nullptr) {
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: Name '%s' / AUTHID '%s' matches user entry '%s'\n", sName,
                     static_cast<const char*>(oaiAuthID),
                     ptUser->sUserName);
    }
	if ( ptUserRecord != nullptr ) {
		memcpy(ptUserRecord,ptUser,sizeof(user_struct));
	}  // if
    return 1;
  }
  // No matching user, huh?
  return 0;
}





// Given a name and/or WONID, returns TRUE if that name/WONID is reserved
// (has access 16384), FALSE otherwise.
// Note that this will return TRUE if any record that matches sName has 16384
// access.
BOOL IsNameReserved(const char* sName, const AMAuthId& oaiAuthID, const char* sIP, user_struct* ptUserRecord) {
  if (ptUserRecord == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: IsNameReserved called with nullptr ptUserRecord.\n");
    return FALSE;
  }


  memset( ptUserRecord, 0, sizeof(user_struct) );

  if ( sName == nullptr ) sName = "";
  if ( sIP == nullptr ) sIP = "0.0.0.0";

  if (g_fUsersLoaded == FALSE)
    LoadUsers();


  user_struct* ptUser = nullptr;


#ifdef USE_MYSQL


  //user_struct User; // tempory struct to store user info pulled from DB
  //int iResult;      // return val check for db funcs
  // char *sType="user";  // "type" of check
  MYSQL_RES *pResult;   // store for result set
  MYSQL_ROW pRow = nullptr; // store for rows from results

  if( (int)CVAR_GET_FLOAT("mysql_preload")==0) { // if we didn't preload the users file

	  //TODO: Why is this here and never used? Looks like a bug to me.

	 bool bUsersTableIsSet = true;
	 bool bTagsTableIsSet = true;
	 const char* pcCVar = get_cvar_string_value( "mysql_dbtable_users" );
	 if ( pcCVar == nullptr ) {
		 bUsersTableIsSet = false;
		 DEBUG_LOG(2, ("mySQL users table is not set.") );
	 }  // if
	 pcCVar = get_cvar_string_value( "mysql_dbtable_tags" );
	 if ( pcCVar == nullptr) {
		 bTagsTableIsSet = false;
		 DEBUG_LOG(2, ("mySQL tags table is not set.") );
	 }  // if

    char sQuery[QUERY_BUF_SIZE];

    snprintf(sQuery, QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("mysql_users_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_users"),escape_chars(sName, '\''),(const char*)oaiAuthID);

    pResult=admin_mysql_query(sQuery,"user");


    bool valid_user = false;
    // Make sure we got a valid result back.
    if ( !pResult) {
		UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
    } else {
		// For every row in the result set...
		while ((pRow = mysql_fetch_row(pResult)) != nullptr) {
			valid_user = true;
                        if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                           DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", pRow[0], pRow[1]) );
                        } else {
                           DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", pRow[1]) );
                        }  // if-else
			ptUserRecord->iAccess=atoi(pRow[1]);

			if ((ptUserRecord->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {

				am_strncpy(ptUserRecord->sUserName,sName,USERNAME_SIZE);
				am_strncpy(ptUserRecord->sPassword,pRow[0],PASSWORD_SIZE);
				ptUserRecord->iIndex= GetPlayerIndex((char*)sName);
				//memcpy(ptUserRecord,&User,sizeof(user_struct));

				if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
					UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, ptUserRecord->iAccess);
				}  // if
				mysql_free_result(pResult);
				return TRUE;
			}  // if
		}  // while
    }  // if-else
    mysql_free_result(pResult);

   if ( valid_user == false ) { // try to search for an IP
	   snprintf(sQuery, QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("mysql_users_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_users"),sIP,(const char*)oaiAuthID);

	   pResult=admin_mysql_query(sQuery,"user");

	   // Make sure we got a valid result back.
	   if ( !pResult) {
		   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
	   } else {
		   // For every row in the result set...
		   while ((pRow = mysql_fetch_row(pResult)) != nullptr) {
                       if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                          DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", pRow[0], pRow[1]) );
                       } else {
                          DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", pRow[1]) );
                       }  // if-else
			   ptUserRecord->iAccess=atoi(pRow[1]);

			   if ((ptUserRecord->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {

				   am_strncpy(ptUserRecord->sUserName,sName,USERNAME_SIZE);
				   am_strncpy(ptUserRecord->sPassword,pRow[0],PASSWORD_SIZE);
				   ptUserRecord->iIndex= GetPlayerIndex((char*)sName);
				   //memcpy(ptUserRecord,&User,sizeof(user_struct));

				   if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
					   UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, ptUserRecord->iAccess);
				   }  // if
				   mysql_free_result(pResult);
				   return TRUE;
			   }  // if
		   }  // while
	   }  // if-else
	   mysql_free_result(pResult);

   }  // if

    // Didn't find a username, check for a tag :)

	if ( bTagsTableIsSet ) {
		snprintf(sQuery, QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("mysql_tags_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_tags"),escape_chars(sName,'\''),(const char*)oaiAuthID);
		if ((int)CVAR_GET_FLOAT("admin_debug") != 0)
			UTIL_LogPrintf("[ADMIN]Trying tags db with %s\n",sQuery);

		pResult=admin_mysql_query(sQuery,"user");

		if ( !pResult) {
			UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
		} else {
			// For every row in the result set...
			while ((pRow = mysql_fetch_row(pResult)) != nullptr) {
                            if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                               DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", pRow[0], pRow[1]) );
                            } else {
                               DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", pRow[1]) );
                            }  // if-else
				ptUserRecord->iAccess=atoi(pRow[1]);

				if ((ptUserRecord->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {

					am_strncpy(ptUserRecord->sUserName,sName,USERNAME_SIZE);
					am_strncpy(ptUserRecord->sPassword,pRow[0],PASSWORD_SIZE);

					ptUserRecord->iIndex= GetPlayerIndex((char*)sName);
					//memcpy(ptUserRecord,&User,sizeof(user_struct));


					if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
						UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, ptUserRecord->iAccess);
					}  // if
					mysql_free_result(pResult);
					return TRUE;
				}  // if
			}  // while
		}  // if-else

		mysql_free_result(pResult);
	}  // if


  } else {
#endif

#ifdef USE_PGSQL

  //user_struct User; // tempory struct to store user info pulled from DB
  //int iResult;      // return val check for db funcs
  // char *sType="user";  // "type" of check
  PGresult *pgResult;  // store for result set
  int tup_num;        // current tuple number in result set

  if( (int)CVAR_GET_FLOAT("pgsql_preload")==0) { // if we didn't preload the users file


	  //TODO: WHy is this here and never used? Looks like a bug to me.
	 bool bUsersTableIsSet = true;
	 bool bTagsTableIsSet = true;
	 const char* pcCVar = get_cvar_string_value( "pgsql_dbtable_users" );
	 if ( pcCVar == nullptr ) {
		 bUsersTableIsSet = false;
		 DEBUG_LOG(2, ("PgSQL users table is not set.") );
	 }  // if
	 pcCVar = get_cvar_string_value( "pgsql_dbtable_tags" );
	 if ( pcCVar == nullptr ) {
		 bTagsTableIsSet = false;
		 DEBUG_LOG(2, ("PgSQL tags table is not set.") );
	 }  // if

    char sQuery[QUERY_BUF_SIZE];

    snprintf(sQuery, QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("pgsql_users_sql"),(char*)CVAR_GET_STRING("pgsql_dbtable_users"),escape_chars(sName, '\''),(const char*)oaiAuthID);

    pgResult=admin_pgsql_query(sQuery,"user");


    bool pg_valid_user = false;
    // Make sure we got a valid result back.
    if ( !pgResult) {
		UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
    } else {
		// For every row in the result set...
		for (tup_num = 0; tup_num < PQntuples(pgResult); tup_num++) {
			pg_valid_user = true;
                        if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                           DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", PQgetvalue(pgResult, tup_num, 0), PQgetvalue(pgResult, tup_num, 1)) );
                        } else {
                           DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", PQgetvalue(pgResult, tup_num, 1)) );
                        }  // if-else
			ptUserRecord->iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));

			if ((ptUserRecord->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {

				am_strncpy(ptUserRecord->sUserName,sName,USERNAME_SIZE);
				am_strncpy(ptUserRecord->sPassword,PQgetvalue(pgResult, tup_num, 0),PASSWORD_SIZE);
				ptUserRecord->iIndex= GetPlayerIndex((char*)sName);
				//memcpy(ptUserRecord,&User,sizeof(user_struct));

				if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
					UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, ptUserRecord->iAccess);
				}  // if
				PQclear(pgResult);
				return TRUE;
			}  // if
		}  // for
    }  // if-else
    PQclear(pgResult);

   if ( pg_valid_user == false ) { // try to search for an IP
	   snprintf(sQuery, QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("pgsql_users_sql"),(char*)CVAR_GET_STRING("pgsql_dbtable_users"),sIP,(const char*)oaiAuthID);

	   pgResult=admin_pgsql_query(sQuery,"user");

	   // Make sure we got a valid result back.
	   if ( !pgResult) {
		   UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
	   } else {
		   // For every row in the result set...
		   for (tup_num = 0; tup_num < PQntuples(pgResult); tup_num++) {
                       if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                          DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", PQgetvalue(pgResult, tup_num, 0), PQgetvalue(pgResult, tup_num, 1)) );
                       } else {
                          DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", PQgetvalue(pgResult, tup_num, 1)) );
                       }  // if-else
			   ptUserRecord->iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));

			   if ((ptUserRecord->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {

				   am_strncpy(ptUserRecord->sUserName,sName,USERNAME_SIZE);
				   am_strncpy(ptUserRecord->sPassword,PQgetvalue(pgResult, tup_num, 0),PASSWORD_SIZE);
				   ptUserRecord->iIndex= GetPlayerIndex((char*)sName);
				   //memcpy(ptUserRecord,&User,sizeof(user_struct));

				   if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
					   UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, ptUserRecord->iAccess);
				   }  // if
				   PQclear(pgResult);
				   return TRUE;
			   }  // if
		   }  // for
	   }  // if-else
	   PQclear(pgResult);

   }  // if

    // Didn't find a username, check for a tag :)

	if ( bTagsTableIsSet ) {
		snprintf(sQuery, QUERY_BUF_SIZE, (char*)CVAR_GET_STRING("pgsql_tags_sql"),(char*)CVAR_GET_STRING("pgsql_dbtable_tags"),escape_chars(sName,'\''),(const char*)oaiAuthID);
		if ((int)CVAR_GET_FLOAT("admin_debug") != 0)
			UTIL_LogPrintf("[ADMIN]Trying tags db with %s\n",sQuery);

		pgResult=admin_pgsql_query(sQuery,"user");

		if ( !pgResult) {
			UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned nullptr result.\n");
		} else {
			// For every row in the result set...
			for (tup_num = 0; tup_num < PQntuples(pgResult); tup_num++) {
                            if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
                               DEBUG_LOG(3, ("SQL Query returned password '%s', access '%s'", PQgetvalue(pgResult, tup_num, 0), PQgetvalue(pgResult, tup_num, 1)) );
                            } else {
                               DEBUG_LOG(3, ("SQL Query returned password  '********', access '%s'", PQgetvalue(pgResult, tup_num, 1)) );
                            }  // if-else
				ptUserRecord->iAccess=atoi(PQgetvalue(pgResult, tup_num, 1));

				if ((ptUserRecord->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {

					am_strncpy(ptUserRecord->sUserName,sName,USERNAME_SIZE);
					am_strncpy(ptUserRecord->sPassword,PQgetvalue(pgResult, tup_num, 0),PASSWORD_SIZE);

					ptUserRecord->iIndex= GetPlayerIndex((char*)sName);
					//memcpy(ptUserRecord,&User,sizeof(user_struct));


					if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
						UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, ptUserRecord->iAccess);
					}  // if
					PQclear(pgResult);
					return TRUE;
				}  // if
			}  // for
		}  // if-else

		PQclear(pgResult);
	}  // if


  } else {
#endif


	  CLinkItem<user_struct>* pUser = m_pUserList->FirstLink();

	  while (pUser != nullptr) {
		  //TBR char* pcEndptr = 0;
		  ptUser = pUser->Data();
		  ///TBR int iNum = strtol( ptUser->sUserName, &pcEndptr, 10 );
		  AMAuthId oaiID( ptUser->sUserName );
		  // Try to get a match; this match can be a name/name match, or a name/WONID match, or a name/IP match.
		  if ( (!oaiID.is_set() && !IsIPValid(sName) && match(sName,ptUser->sUserName)!=0)       // a user name
			   || (oaiID.is_set() && oaiID == oaiAuthID)                    // an auth id
			   || (IsIPValid(sIP) && match(sIP, ptUser->sUserName)==1) ) {  // a valid IP number
			  if ((ptUser->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {
				  memcpy(ptUserRecord,ptUser,sizeof(user_struct));
				  
				  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
					  UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",ptUser->sUserName, ptUser->iAccess);
				  }  // if
				  
				  return TRUE;
			  }  // if
		  }
		  pUser = pUser->NextLink();
	  }
#ifdef USE_PGSQL
  }
#endif
#ifdef USE_MYSQL
  }
#endif
  
  return FALSE;
}

// Loads a user record from a line in a file.  Returns TRUE if successful,
// FALSE otherwise.  The format is:
// <user name or WON ID>:<password>:<access>:
BOOL ParseUser(char* pcLine) {
	int iNumColon = -2;
  char* sAccessToken;
  char sDelimiter[] = ":";
	char* sPasswordToken;
  char* pcDelim = nullptr;

	if ( pcLine == nullptr ) return FALSE;

	const int iLineLength = strlen(pcLine);
  char* sLine = new char[iLineLength+1];
  memset (sLine, 0, (iLineLength + 1) );
  memcpy( sLine, pcLine, iLineLength );

  // Count the number of colons in the line
  pcDelim = sLine + iLineLength;
  while ( pcDelim > sLine ) {
	  if ( *pcDelim == ':' && *(pcDelim-1) != '\\' ) ++iNumColon;
	  --pcDelim;
  }  // while

  // There must be at least two and at most four delimiters for the line to be valid
  if ( iNumColon < 0 || iNumColon > 2 ) {
	  delete[] sLine;
	  return FALSE;
  }  // if

  char* sNameToken = sLine;
  pcDelim = strchr( sLine, ':' );
  // If we have an authid (steamid or valveid) in this entry we have a colon within the first column.
  // In that case we have to skip it and search for the next colon.
  if ( AMAuthId::is_authid(sLine) ) {
	  for ( ; iNumColon > 0; --iNumColon ) pcDelim = strchr( (pcDelim+1), ':' );
  }  // if

  if ( pcDelim > sLine ) {
    while ( *(pcDelim - 1) == '\\' ) {  // accept the colon as part of the name
      memmove( (pcDelim - 1), pcDelim, (iLineLength - (pcDelim - sLine) + 1) );
      pcDelim = strchr( (pcDelim + 1), ':' );
      if ( pcDelim == nullptr ) break;
    }  // while
  }  // if

  if ( pcDelim == nullptr ) {
    sNameToken = nullptr;
  } else {
    *pcDelim = 0;
    pcDelim++;
  }  // if-else

  if (sNameToken == nullptr) {
	  UTIL_LogPrintf("[ADMIN] ERROR: No user name found: '%s'\n", sLine);
  } else if ((int)strlen(sNameToken) > USERNAME_SIZE) {
	  UTIL_LogPrintf("[ADMIN] ERROR: User name too long: '%s'\n", sNameToken);
  } else {
	  if ( *pcDelim == ':' ) {
		  // empty password
		  sPasswordToken = pcDelim;
		  *pcDelim = 0;
		  pcDelim++;
	  } else {
		  //sPasswordToken = strtok(pcDelim,sDelimiter);
		  sPasswordToken = pcDelim;
		  pcDelim = strchr( pcDelim, ':' );
		  // If we have a authid in this entry we have a colon within the column.
		  // In that case we have to skip it and search for the next colon.
		  if (nullptr != pcDelim && AMAuthId::is_authid(sPasswordToken) ) pcDelim = strchr( (pcDelim+1), ':' );
		  if (nullptr == pcDelim ) sPasswordToken = nullptr;
		  else {
			  *pcDelim = 0;
			  ++pcDelim;
		  }  // if-else
	  }  // if-else

	  if (sPasswordToken == nullptr) {
		  UTIL_LogPrintf("[ADMIN] ERROR: No user password found: '%s'\n", sLine);
	  } else if ((int)strlen(sPasswordToken) > PASSWORD_SIZE) {
		  if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
			  UTIL_LogPrintf("[ADMIN] ERROR: User password too long: '%s'\n", sPasswordToken);
		  } else {
			  UTIL_LogPrintf("[ADMIN] ERROR: User password too long: %i\n", strlen(sPasswordToken));
		  }
	  } else {
		  if ( *pcDelim != ':' ) {
			  sAccessToken = strtok( pcDelim, sDelimiter );
		  } else {
			  // empty access field, assuming 0
			  sAccessToken = "0";
		  }  // if-else

		  if (sAccessToken == nullptr) {
			  UTIL_LogPrintf("[ADMIN] ERROR: No user access found: '%s'\n", sLine);
			  /* AccessToken too long? Oh please, all we do is pass it to atoi(). Let's comment this out.
				 } else if ((int)strlen(sAccessToken) > ACCESS_SIZE) {
				 UTIL_LogPrintf("[ADMIN] ERROR: User access too long: '%s'\n", sAccessToken);
			  */
		  } else {
			  user_struct* tUser = new user_struct;
			  if(tUser == nullptr) {
				  UTIL_LogPrintf( "[ADMIN] ERROR: ParseUser::'new' failed for tUser record.\n");
				  delete[] sLine;
				  sLine = nullptr;
				  return FALSE;
			  }
			  memset(tUser,0x0,sizeof(user_struct));
			  strcpy(tUser->sUserName,sNameToken);
			  strcpy(tUser->sPassword,sPasswordToken);
			  const int iAccess = atoi(sAccessToken);
			  tUser->iAccess = iAccess;
			  tUser->iIndex = m_iUserIndex++;
			  m_pUserList->AddLink(tUser);
			  
			  if((int)CVAR_GET_FLOAT("amv_log_passwords") == 1) {
				  DEBUG_LOG(1, ("User loaded: Index: %i, Name '%s', Password '%s', Access '%i'", tUser->iIndex, tUser->sUserName, tUser->sPassword, tUser->iAccess) );
			  } else {
				  DEBUG_LOG(1, ("User loaded: Index: %i, Name '%s', Password '********', Access '%i'",tUser->iIndex, tUser->sUserName, tUser->iAccess) );
			  }
			  delete[] sLine;
			  sLine = nullptr;
			  return TRUE;
		  }
	  }
  }
  delete[] sLine;
  sLine = nullptr;
  return FALSE;
}

#ifdef USE_MYSQL
// Loads a user record from a MySQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseUserSQL(MYSQL_ROW pRow) {
  user_struct* tUser;
  
  tUser = new user_struct;
  if(tUser == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseUserSQL::'new' failed for tUser record.\n");
    return FALSE;
  }
  memset(tUser,0x0,sizeof(user_struct));
  strcpy(tUser->sUserName,pRow[0]);
  strcpy(tUser->sPassword,pRow[1]);
  tUser->iAccess = atoi(pRow[2]);
  tUser->iIndex = m_iUserIndex++;
  m_pUserList->AddLink(tUser);
  
	if((int)CVAR_GET_FLOAT("amv_log_passwords") == 1) {
	  DEBUG_LOG(1, ("User loaded: Name '%s', Password '%s', Access '%i'",tUser->sUserName, tUser->sPassword, tUser->iAccess) );
	} else {
	  DEBUG_LOG(1, ("User loaded: Name '%s', Password '********', Access '%i'",tUser->sUserName, tUser->iAccess) );
	}
  return TRUE;
}
#endif

#ifdef USE_PGSQL
// Loads a user record from a PgSQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseUserPgSQL(const PGresult *res, int tup_num) {
  user_struct* tUser;
  
  tUser = new user_struct;
  if(tUser == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseUserPgSQL::'new' failed for tUser record.\n");
    return FALSE;
  }
  memset(tUser,0x0,sizeof(user_struct));
  strcpy(tUser->sUserName,PQgetvalue(res, tup_num, 0));
  strcpy(tUser->sPassword,PQgetvalue(res, tup_num, 1));
  tUser->iAccess = atoi(PQgetvalue(res, tup_num, 2));
  tUser->iIndex = m_iUserIndex++;
  m_pUserList->AddLink(tUser);
  
	if((int)CVAR_GET_FLOAT("amv_log_passwords") == 1) {
	  DEBUG_LOG(1, ("User loaded: Name '%s', Password '%s', Access '%i'",tUser->sUserName, tUser->sPassword, tUser->iAccess) );
	} else {
	  DEBUG_LOG(1, ("User loaded: Name '%s', Password '********', Access '%i'",tUser->sUserName, tUser->iAccess) );
	}
  return TRUE;
}
#endif

void LoadUsers() {
  // Make sure we're not already loaded.
  if (g_fUsersLoaded == TRUE)
    return;
  // Note that we're now loaded.
  g_fUsersLoaded = TRUE;
  
  // Reset the user index (which ParseUser will increment)
  m_iUserIndex = 1;
  
  // Create a new linked list, if necessary.
  if (m_pUserList == nullptr) 
    m_pUserList = new CLinkList<user_struct>();
  
  // Load the users
#ifdef USE_MYSQL
  if (!LoadTable("user", m_pUserList, "mysql_dbtable_users", "SELECT nick,pass,access FROM %s\n", &ParseUserSQL))
#endif
#ifdef USE_PGSQL
   if (!LoadPgTable("user", m_pUserList, "pgsql_dbtable_users", "SELECT nick,pass,access FROM %s\n", &ParseUserPgSQL))
#endif
    LoadFile("user", m_pUserList, "users_file", &ParseUser);
}


void UnloadUsers() {
  g_fUsersLoaded = FALSE;
  
  // Reset the user index 
  m_iUserIndex = 1;
  
  // Create a new linked list, if necessary.
  if (m_pUserList == nullptr)  return;

  delete m_pUserList;
  m_pUserList = nullptr;
}


// Sets the user password.  If sSetPassword is nullptr, this will attempt to read it
// (and then clear it) from the player's setinfo buffer.
void SetUserPassword(const char* sName, char* sSetPassword, edict_t* pEntity) {
	const int iIndex = ENTINDEX(pEntity);
  int iSetPassword = 0;
  char sPassword[PASSWORD_SIZE];
  char* sPasswordField = const_cast<char*>(get_cvar_string_value( "password_field" ));
  char* infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity);
  char* pcClientPwBufferCopy = nullptr;
  char szCommand[128];
  
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: SetUserPassword: User '%s' has out of bounds entity index %i.\n", sName, iIndex);
    return;
  }
  
  strncpy(g_AuthArray[iIndex].sUserName, sName, USERNAME_SIZE);
  memset(sPassword,0x0,PASSWORD_SIZE);

  // If we're passed a password, use that.  Otherwise, grab it
  // from their setinfo key, if possible.
  if (sSetPassword != nullptr) {
    strncpy(sPassword, sSetPassword, PASSWORD_SIZE);
    iSetPassword = 1;

  } else if ( sPasswordField != nullptr ) {
    pcClientPwBufferCopy = g_engfuncs.pfnInfoKeyValue(infobuffer,sPasswordField);

    // If we got a password from their setinfo buffer, use it and clear it.
    if ( pcClientPwBufferCopy != nullptr && !FStrEq(pcClientPwBufferCopy,"")) {
	  strncpy(sPassword, pcClientPwBufferCopy, PASSWORD_SIZE);
/*TBR: This should not be needed anymore since we don't touch the user password 
       in CUIC() before we get to CC().
	  // Store a temporary copy which will timeout after some seconds.
	  strncpy(m_aoTmpPwd[iIndex].acPwd, pcClientPwBufferCopy, PASSWORD_SIZE);
	  m_aoTmpPwd[iIndex].iSet = time(nullptr);
	  if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
		  DEBUG_LOG(2, ("Storing password '%s'.", m_aoTmpPwd[iIndex].acPwd) ); 
	  } else {
		  DEBUG_LOG(2, ("Storing password.") ); 
	  }  // if-else
*/

      iSetPassword = 2;
	  // Clear the clients password buffer
      snprintf(szCommand,128,"setinfo %s \"\"\n",sPasswordField);
      CLIENT_COMMAND(pEntity,szCommand);
	  DEBUG_LOG(1, ("Client password buffer cleared.") );
/*TBR: I don't think this does anything. Actually, I know that it only works for the info the server has but doesn't affect the client.
	  //*pcClientPwBufferCopy = 0; ;
	  g_engfuncs.pfnSetClientKeyValue( iIndex, infobuffer, sPasswordField, "" );
	  DEBUG_LOG(2, ("Server knows pw '%s' now", g_engfuncs.pfnInfoKeyValue(infobuffer,sPasswordField)) );
*/
/*TBR: Uhm, we don't have a backup copy anymore. See above.
	  // Else if we have a backup copy, use that one.
    } else if ( m_aoTmpPwd[iIndex].iSet && ((time(nullptr)-m_aoTmpPwd[iIndex].iSet) < 5) ) {
		strncpy( sPassword, m_aoTmpPwd[iIndex].acPwd, PASSWORD_SIZE );
		if ( (int)CVAR_GET_FLOAT("amv_log_passwords") == 1 ) {
			DEBUG_LOG(2, ("Restoring password '%s'.", m_aoTmpPwd[iIndex].acPwd) ); 
		} else {
			DEBUG_LOG(2, ("Restoring password.") ); 
		}
		iSetPassword = 3;

		// else if we have a backup copy but it timed out, clear it.
	} else if ( m_aoTmpPwd[iIndex].iSet ) {
		// clear the temporary password buffer
		memset( (reinterpret_cast<tmppwd_t*>(&m_aoTmpPwd[iIndex])), 0, sizeof(tmppwd_t) );
*/
	}  // if-else
  }

  // If we changed their password, reset their auth access (need to call
  // VerifyUserAuth again).
  if (iSetPassword >= 1) {
    strncpy(g_AuthArray[iIndex].sPassword, sPassword, PASSWORD_SIZE);
	if ( iSetPassword < 3 ) {
		DEBUG_DO(1, System_Response("[ADMIN] Password received. Authentication pending...\n", pEntity); );
		if ( (int)CVAR_GET_FLOAT("amv_log_passwords") ) {
			DEBUG_LOG(1, ("SetUserPassword: User '%s' entered password '%s'.",sName,sPassword) );
		} else {
			DEBUG_LOG(1, ("SetUserPassword: User '%s' entered password '********'.", sName) );
		}  // if
	} else {
		if ( (int)CVAR_GET_FLOAT("amv_log_passwords") ) {
			DEBUG_LOG(1, ("SetUserPassword: Used previously entered password '%s' for user '%s'.", sPassword, sName) );
		} else {
			DEBUG_LOG(1, ("SetUserPassword: Used previously entered password '********' for user '%s'.", sName) );
		}  // if-else
	}  // if-else
  } else {
	DEBUG_LOG(1, ("SetUserPassword: No password set for user '%s'.", sName) );
  }  // if-else
}

void UpdateUserAuth(edict_t* pEntity) {
	const int iIndex = ENTINDEX(pEntity);
  
  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: UpdateUserAuth: User '%s' has out of bounds entity index %i\n", pEntity->v.netname, iIndex);
    return;
  }
  g_AuthArray[iIndex].iTime = time(nullptr);

  DEBUG_LOG( 4, ("UpdateUserAuth: %d: %ld.", iIndex, g_AuthArray[iIndex].iTime) );
}

// Attempts to find a matching user record for this player.  
BOOL VerifyUserAuth(const char* sName, edict_t* pEntity) {
  BOOL fResult = FALSE;
  const int iIndex = ENTINDEX(pEntity);
  const char* pcIP = nullptr;
  user_struct tUser;
  
  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: VerifyUserAuth: User '%s' has out of bounds entity index %i\n", sName, iIndex);
    return FALSE;
  }

  user_ip( iIndex, &pcIP, nullptr );
 
  DEBUG_DO(1, System_Response("[ADMIN] Checking password for user access...\n", pEntity); );

  // Try to get a record that matches this player's name or WON ID, and their password.
  if (GetUserRecord(sName, AMAuthId(GETPLAYERAUTHID(pEntity)),pcIP,g_AuthArray[iIndex].sPassword,&tUser)) {
    // We got one.  Note down it's access.
    g_AuthArray[iIndex].iAccess = tUser.iAccess;
    g_AuthArray[iIndex].iUserIndex = tUser.iIndex;
    System_Response(UTIL_VarArgs("[ADMIN] Password accepted for user '%s'. Access granted: %i\n",tUser.sUserName,tUser.iAccess), pEntity);
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: VerifyUserAuth: User '%s' matched correct password for entry '%s'.\n",sName,tUser.sUserName);
    }
    fResult = TRUE;
    // Otherwise, reset their access
  } else {
    g_AuthArray[iIndex].iAccess = (int)CVAR_GET_FLOAT("default_access");
    g_AuthArray[iIndex].iUserIndex = 0;
    DEBUG_DO(1, System_Response(UTIL_VarArgs("[ADMIN] Wrong or no password for user '%s'.\n[ADMIN] Only default access granted (%i).\n",sName, g_AuthArray[iIndex].iAccess), pEntity); );
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: VerifyUserAuth: User '%s' did not match correct password for any entry.\n",sName);
    }
    fResult = FALSE;
  }

  DEBUG_LOG(1, ("VerifyUserAuth: Access granted: %i", g_AuthArray[iIndex].iAccess) );  

  g_AuthArray[iIndex].iTime = time(nullptr);
  // Because someone's access may have changed, we should see if the highlander has changed.
  GetHighlanderIndex();
  return fResult;
}

/***************************
 *
 * Spawning functions
 *
 ***************************/
// Adds a new spawn record to the linked list.  Returns
// the identity if successful, 0 otherwise.
int AddSpawnEntity(const char* szClassname, CBaseEntity* pEntity) {
  spawn_struct* tSpawn = nullptr;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();

  // Make a new link
  tSpawn = new spawn_struct;
  // Make sure the link is a valid address
  if(tSpawn == nullptr) {
    UTIL_LogPrintf("[ADMIN] AddSpawnEntity: 'new' failed for tSpawn record.\n");
    return 0;
  }
  
  // Initialize the link's data
  memset(tSpawn, 0x0, sizeof(spawn_struct));
  const int iIdentity = m_iSpawnIdentity++;
  strcpy(tSpawn->szClassname, szClassname);
  tSpawn->iIdentity = iIdentity;
  tSpawn->pEntity = pEntity;
  m_pSpawnList->AddLink(tSpawn);
  return iIdentity;
}

// Find a spawn record, given a spawn's identity.
// May return nullptr for no matching id.
spawn_struct* FindSpawnEntity(int iIdentity) {
  spawn_struct* tSpawn = nullptr;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  
  while(pLink != nullptr) {
    // For each link on the list, check to see if the identities match.
    // Check the classname only if the identity we're looking for is 0.
    tSpawn = pLink->Data();
    if (iIdentity == tSpawn->iIdentity) {
      break;
    } else {
      pLink = pLink->NextLink();
    }
  }
  return (pLink == nullptr ? nullptr : tSpawn);
}

// Initializes the linked list.  DOES NOT REMOVE THE ENTITIES.
void InitSpawnEntityList() {
  if (m_pSpawnList == nullptr) {
    m_pSpawnList = new CLinkList<spawn_struct>();
  } else {
    m_pSpawnList->Init();
  }
}

// Lists the contents of the spawn linked list that match
// szFindClassname (or all, if szFindClassname is nullptr or empty)
void ListSpawnEntities(edict_t* pMsg, char* szFindClassname) {
	const int iLength = strlen(szFindClassname);
  char szClassname[BUF_SIZE];
  spawn_struct* tSpawn = nullptr;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  
  System_Response(UTIL_VarArgs("Identity            ClassName         \n"), pMsg);
  while(pLink != nullptr) {
    tSpawn = pLink->Data();
    const int iIdentity = tSpawn->iIdentity;
    strcpy(szClassname, tSpawn->szClassname);
    if(iLength==0 || strnicmp(szFindClassname, szClassname, iLength)==0) {
      System_Response(UTIL_VarArgs( "  <%d>              %s\n",iIdentity,szClassname), pMsg);
    }
    pLink = pLink->NextLink();
  }
}

// Removes a spawn record by identity from the linked list.
// Returns TRUE if successful, FALSE otherwise.
BOOL RemoveSpawnEntity(int iIdentity) {
  spawn_struct* tSpawn = nullptr;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  
  // Search through the list, like in FindSpawnEntity.
  while(pLink != nullptr) {
    tSpawn = pLink->Data();
    if (iIdentity == tSpawn->iIdentity) {
      break;
    } else {
      pLink = pLink->NextLink();
    }
  }
  
  // If we found nothing, we can't remove anything.
  if(pLink == nullptr) {
    return FALSE;
  } else {
    // Remove the entity and free the link.
    REMOVE_ENTITY(tSpawn->pEntity->edict());
    delete(tSpawn);
    
    m_pSpawnList->DeleteLink(pLink);
    return TRUE;
  }
}



// Removes a spawn record by identity from the linked list.
// Returns TRUE if successful, FALSE otherwise.
void DeleteSpawnEntityList() 
{
  spawn_struct* tSpawn = nullptr;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  
  // Search through the list, like in FindSpawnEntity.
  while(pLink != nullptr) {
    tSpawn = pLink->Data();
    // Remove the entity and free the link.
    REMOVE_ENTITY(tSpawn->pEntity->edict());
    delete(tSpawn);
    pLink = pLink->NextLink();
  }
  
  delete m_pSpawnList;
}




/***************************
 *
 * Plugin functions
 *
 ***************************/
// Adds an entry to the help linked list.  Returns TRUE
// if successful, FALSE otherwise.
BOOL AddHelpEntry(char* sCmd, char* sHelp, int iAccess) {
	help_struct* tHelp;
	CLinkItem<help_struct>* pOldLink = nullptr;
  
  // Make sure our help list is initialized
  if (m_pHelpList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: AddHelpEntry called when help list not initialized.\n");
    return FALSE;
  }
  // Compare this entry to the entries we currently have; we want to
  // avoid duplicates.
  CLinkItem<help_struct>* pLink = m_pHelpList->FirstLink();
  while (pLink != nullptr) {
    tHelp = pLink->Data();
    const int iCompare = stricmp(sCmd, tHelp->sCmd);
    // If iCompare < 0, then this entry belongs in front of the one
    // we're currently at, so we can break and insert it.
    if (iCompare < 0) {
      break;
      // Otherwise, if they're equal, check if the text itself is equal.
      // If so, we don't need to include it again and can leave.
    } else if (iCompare == 0) {
      if (!stricmp(sHelp, tHelp->sHelp)) {
	// The entries are identical, we don't need to add anything.
	if((int)CVAR_GET_FLOAT("admin_debug")!=0) {
	  UTIL_LogPrintf("[ADMIN] WARNING: Ignoring duplicate help entry for '%s'.\n", sCmd);
	}
	return TRUE;
      }
    }
    pOldLink = pLink;
    pLink = pLink->NextLink();
  }
  
  // Ok.  We found the entry that we're going to put our new one before
  // (or nullptr, if we're putting it at the end.)  Let's add it.
  tHelp = new help_struct;
  if(tHelp == nullptr) {
    UTIL_LogPrintf( "[ADMIN] ERROR: AddHelpEntry::'new' failed for tHelp record.\n");
    return FALSE;
  }
  strcpy(tHelp->sCmd, sCmd);
  strncpy(tHelp->sHelp, sHelp,PLUGIN_HELP_SIZE);
  tHelp->iAccess = iAccess;
  m_pHelpList->AddLink(tHelp, pLink);
  return TRUE;
}

// Finds a CPlugin object based on an AMX pointer.
CPlugin* GetPlugin(AMX* amx) {
	CPlugin* pPlugin;
  
  // Make sure our list is initialized.
  if (m_pPluginList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetPlugin called when plugin list not initialized.\n");
    return nullptr;
  }
  // For each plugin, compare the AMX pointers.
  CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
  while (pLink != nullptr) {
    pPlugin = pLink->Data();
    if (nullptr != pPlugin ) {
      // We got a match?  Cool.
      if (amx == pPlugin->amx()) {
        break;
      }
	}
    pLink = pLink->NextLink();
  }
  
  // No match?  Bummer.
  if (pLink == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetPlugin failed.\n");
    return nullptr;
  }
  return pPlugin;
}



// For each plugin, check if it implements Command. Returns the number of plugins that 
// implement this Command. Returns the highest access level in Access. If PrintOut is
// TRUE each find will be announced to the caller.

int CheckCommand( edict_t* _pEntity, const char* _pcCommand, unsigned int& _uiAccess, BOOL _bCheckUserAcc, BOOL _bPrintOut ) {
	int iResult = 0; // number of plugins that implement this command
	unsigned int uiAccess = 0;

	if ( m_pPluginList == nullptr ) {
		UTIL_LogPrintf( "[ADMIN] ERROR: CheckCommand() called when plugin list not initialized.\n" );
		return 0;
	}  // if

	// We return the lowest access value. Set returned _uiAccess to max pivot.
	_uiAccess = ~0;

	CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
	while ( pLink != nullptr ) {
		CPlugin* pPlugin = pLink->Data();
		// ATTN: the next line would usually be placed at the end of the while-loop. 
		// But since we don't need the pLink later anymore, we have it here to be able 
		// to shortcut to the beginning of the while-loop further down. If the pLink
		// would be needed, this has to be moved down again and the shortcut redone.
		pLink = pLink->NextLink();

		if ( pPlugin->CheckCommand(_pcCommand, uiAccess) ) {
			// If we check if the user can access this command
			// we have to compare access rights here.
			// Note, that GetUserAccess() respects Highlander mode. This may mess up things.
			if( _bCheckUserAcc && uiAccess != 0 && !(uiAccess & GetUserAccess(_pEntity)) ) continue; // shortcut to next cycle
			++iResult;
			if ( uiAccess < _uiAccess ) _uiAccess = uiAccess;  
			if ( _bPrintOut ) {
				System_Response( UTIL_VarArgs("Command %s found in plugin %s\n", _pcCommand, pPlugin->Name()), _pEntity );
			}  // if
		}  // if

	}  // while

	return iResult;
}




// For each plugin, call it's HandleCommand method.  Break if
// any of them return PLUGIN_HANDLED.
plugin_result HandleCommand(edict_t* pEntity, char* sCmd, char* sData) {
  plugin_result iResult = PLUGIN_INVAL_CMD;
  plugin_result iReturn = PLUGIN_INVAL_CMD;

  if (m_pPluginList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleCommand called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
  while (pLink != nullptr) {
    CPlugin* pPlugin = pLink->Data();
    iResult = pPlugin->HandleCommand(pEntity, sCmd, sData);
	if ( iResult != PLUGIN_INVAL_CMD ) iReturn = iResult;
    if (iResult == PLUGIN_HANDLED) {
      break;
    }
    pLink = pLink->NextLink();
  }

  return iReturn;
}

// For each plugin, call it's HandleConnect method.  Break if
// any of them return PLUGIN_HANDLED.
plugin_result HandleConnect(edict_t* pEntity, char* sName, char* sIPAddress) {
  plugin_result iResult = PLUGIN_CONTINUE;

  if (m_pPluginList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleConnect called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
  while (pLink != nullptr) {
    CPlugin* pPlugin = pLink->Data();
    iResult = pPlugin->HandleConnect(pEntity, sName, sIPAddress);
    if (iResult == PLUGIN_HANDLED) {
      break;
    }
    pLink = pLink->NextLink();
  }
  return iResult;
}

// For each plugin, call it's HandleDisconnect method.  Break if
// any of them return PLUGIN_HANDLED.
plugin_result HandleDisconnect(edict_t* pEntity) {
  plugin_result iResult = PLUGIN_CONTINUE;

  if (m_pPluginList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleDisconnect called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
  while (pLink != nullptr) {
    CPlugin* pPlugin = pLink->Data();
    iResult = pPlugin->HandleDisconnect(pEntity);
    if (iResult == PLUGIN_HANDLED) {
      break;
    }
    pLink = pLink->NextLink();
  }
  return iResult;
}

// A doozy of a procedure.  Parse the data that is sent in
// to find out a) what the user is looking for, b) where they
// want to start, and c) how much they want returned.
// Then show it to them.
plugin_result HandleHelp(edict_t* pEntity, char* sData, int iFormat = 0) {
	int iCount = 0;
  int iLength = 10;
  int iMaxCount = 0;
  int iStart = 1;
  char sDelimiter = ' ';
	char sCommand[COMMAND_SIZE];
  char* sFilter = nullptr;
  char sFilterText[BUF_SIZE];
  char sHelp[BUF_SIZE];
  char sParam[BUF_SIZE];
	char* sToken = nullptr;

	// Verify our list is initialized.
  if (m_pHelpList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleHelp called when help list not initialized.\n");
    return PLUGIN_ERROR;
  }

  // okay, this may not be necessary but I'm a coward and it is a rule to never use 
  // strtok() on external strings. (Actually the man page says "never use this function")
  // Thus we copy the sData string into our internal array and work on that one.
  char* pcData = new char[strlen(sData)+1];
  memset( pcData, 0, (strlen(sData)+1) );
  strncpy( pcData, sData, strlen(sData) );

  // Get the access of the user; this will determine what they can see.
	const int iAccess = GetUserAccess(pEntity);
  DEBUG_LOG(3, ("HandleHelp: User Access: %d", iAccess) );
  // Tokenize the data.  This is kludgy, but the data format is:
  // [<search string>] [<start> [<length>]] 
  // If the first token is alphabetic, it's search string...otherwise, it's start.
  // If there's a second token, it has to be numeric; if the first was alphabetic,
  // then the second is start.  Otherwise, the second is length.
  // If there's a third token, it's used only if the first was alphabetic.  In that
  // case, it's length. 
  // Confused?
  if (pcData != nullptr) {
    sToken = strtok(pcData, &sDelimiter);
    if (sToken != nullptr) {
      // If the first token is alphabetic, it's the search string.
      if (atoi(sToken) == 0 && *sToken != '0') {
	sFilter = sToken;
	// Get the second token.
	sToken = strtok(nullptr, &sDelimiter);
      }
      if (sToken != nullptr) {
	// Otherwise, the first (or second) token is start.
	iStart = atoi(sToken);
	if (iStart < 1)
	  iStart = 1;
	sToken = strtok(nullptr, &sDelimiter);
	if (sToken != nullptr) {
	  // And the second (or third) token is length.
	  iLength = atoi(sToken);
	  if (iLength < 1)
	    iLength = 1;
	}
      }
    }
  }
  
  if (sFilter == nullptr || strlen(sFilter) == 0) {
    strcpy(sFilterText, "");
  } else {
    sprintf(sFilterText, " for '%s'", sFilter);
  }
  
  if (iFormat == 1) {
    System_Response(UTIL_VarArgs("----- Start -----\n"),pEntity);
  }
  
  // For each help entry...
  CLinkItem<help_struct>* pLink = m_pHelpList->FirstLink();
  while (pLink != nullptr) {
    help_struct* tHelp = pLink->Data();
    // Make sure that they have access to this entry, and it matches the filter they give
    // (or they didn't give a filter).
    if (((iAccess & tHelp->iAccess) == tHelp->iAccess)
	&& (sFilter == nullptr || stristr(tHelp->sCmd, sFilter) != nullptr || stristr(tHelp->sHelp, sFilter) != nullptr )) {
      iMaxCount++;
      // Make sure we're underneath their end point.
      if (iCount < (iStart + iLength - 1)) {
	iCount++;
	// See if we reached their start point.
	if (iCount >= iStart) {
	  // If this is our first one, show the header.
	  if (iCount == 1 && iFormat != 1) {
	    System_Response(UTIL_VarArgs("----- Admin Help Entries %s -----\n", sFilter),pEntity);
	  }
	  // Show the entry.
	  if (iFormat == 1) {
	    // We need to try to break apart the help entry into its various components
	    char* sColon = strchr(tHelp->sHelp, ':');
	    char* sSpace = strchr(tHelp->sHelp, ' ');
	    if (sColon == nullptr) {
	      strcpy(sCommand, "Unknown/Bad Format");
	      strcpy(sParam, "");
	      strcpy(sHelp, tHelp->sHelp);
	    } else if (sSpace == nullptr || sSpace > sColon) {
	      strncpy(sCommand, tHelp->sHelp, sColon - tHelp->sHelp);
	      sCommand[sColon - tHelp->sHelp] = '\0';
	      strcpy(sParam, "");
	      while (*sColon == ':' || *sColon == ' ')
		sColon++;
	      strcpy(sHelp, sColon);
	    } else {
	      strncpy(sCommand, tHelp->sHelp, sSpace - tHelp->sHelp);
	      sCommand[sSpace - tHelp->sHelp] = '\0';
	      while (*sSpace == ':' || *sSpace == ' ')
		sSpace++;
	      strncpy(sParam, sSpace, sColon - sSpace);
	      sParam[sColon - sSpace] = '\0';
	      while (*sColon == ':' || *sColon == ' ')
		sColon++;
	      strcpy(sHelp, sColon);
	    }
	    System_Response(UTIL_VarArgs("%3i:%s:%s:%s\n", iCount, sCommand, sParam, sHelp), pEntity);
	  } else {
	    System_Response(UTIL_VarArgs("%3i: %s\n", iCount, tHelp->sHelp), pEntity);
	  }
	}
      }
    }
    pLink = pLink->NextLink();
  }
  // If we reached their start point, we showed the header and at least one entry.
  // So now we need to show the footer.
  if (iCount >= iStart) {
    if (iFormat == 1) {
      System_Response(UTIL_VarArgs("----- %i:%i:%i -----\n",iStart,iCount,iMaxCount),pEntity);
    } else {
      System_Response(UTIL_VarArgs("----- Entries %i - %i Of %i %s -----\n", iStart, iCount, iMaxCount, sFilterText),pEntity);
      // If there are some that we didn't show them, let them know.
      if (iCount < iMaxCount) {
	if (sFilter == nullptr) {
	  System_Response(UTIL_VarArgs("----- Use 'admin_help %i' For More -----\n",iCount + 1),pEntity);
	} else {
	  System_Response(UTIL_VarArgs("----- Use 'admin_help %s %i' For More -----\n",sFilter, iCount + 1),pEntity);
	}
      }
    }
    // Otherwise, they put in a start that's too high.  Let them know.
  } else if (iCount != 0) {
    if (iFormat == 1) {
      System_Response(UTIL_VarArgs("----- %i:%i:%i -----\n",0,0,iMaxCount),pEntity);
    } else {
      System_Response(UTIL_VarArgs("There are less than %i help entries %s.\n", iStart, sFilter), pEntity);
    }
    // Otherwise, there are just no topics matching their filter.  Let them know.
  } else {
    if (iFormat == 1) {
      System_Response(UTIL_VarArgs("----- %i:%i:%i -----\n",0,0,0),pEntity);
    } else {
      System_Response(UTIL_VarArgs("No help topics found %s.\n",sFilter),pEntity);
    }
  }
  if (iFormat == 1) {
    System_Response(UTIL_VarArgs("----- End -----\n"),pEntity);
  }

  delete[] pcData;
  return PLUGIN_HANDLED;
}

// For each plugin, call it's HandleInfo method.  Break if
// any of them return PLUGIN_HANDLED.
plugin_result HandleInfo(edict_t* pEntity, char* sNewName) {
  plugin_result iResult = PLUGIN_CONTINUE;

  if (m_pPluginList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleInfo called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
  while (pLink != nullptr) {
    CPlugin* pPlugin = pLink->Data();
    iResult = pPlugin->HandleInfo(pEntity, sNewName);
    if (iResult == PLUGIN_HANDLED) {
      break;
    }
    pLink = pLink->NextLink();
  }
  return iResult;
}

// For each plugin, call it's HandleLog procedure.
plugin_result HandleLog(char* sLog) {
  plugin_result iResult = PLUGIN_CONTINUE;

  // Unlike the other procs, this one will be called before initialization. 
	// So don't display an error.
  if (m_pPluginList == nullptr) {
    return iResult;
  }
  CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
  while (pLink != nullptr) {
    CPlugin* pPlugin = pLink->Data();
    iResult = pPlugin->HandleLog(sLog);
    if (iResult == PLUGIN_HANDLED) {
      break;
    }
    pLink = pLink->NextLink();
  }
  return iResult;
}

// For each plugin, show it's version information.
plugin_result HandleVersion(edict_t* pEntity) {
  int i = 1;
  char sName[BUF_SIZE];
  char sDesc[BUF_SIZE];
  char sVersion[BUF_SIZE];

  // Make sure our plugin data is initialized.
  if (m_pPluginList == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleVersion called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }

	// if no plugins are loaded, report that fact
	if ( m_pPluginList->Count() == 0 ) {
		System_Response( "No plugins for Admin Mod are loaded.\n", pEntity );
		return PLUGIN_HANDLED;
	}  // if

  // Otherwise report the version for each plugin.
  CLinkItem<CPlugin>* pLink = m_pPluginList->FirstLink();
  while (pLink != nullptr) {
    CPlugin* pPlugin = pLink->Data();
    strcpy(sName, pPlugin->Name());
    if (sName[0] == 0) {
      strcpy(sName, pPlugin->File());
    }
    strcpy(sDesc, pPlugin->Desc());
    strcpy(sVersion, pPlugin->Version());
    System_Response(UTIL_VarArgs( "* Plugin #%2i: %s (v. %s)\n",i++, sName, sVersion) ,pEntity);
    if (sDesc != nullptr && strlen(sDesc) > 0)
      System_Response(UTIL_VarArgs("  %s \n", sDesc), pEntity);
    pLink = pLink->NextLink();
  }
  return PLUGIN_HANDLED;
}



// Loads a plugin from a line in a file.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParsePlugin( char* sLine ) {
  char sGameDir[PATH_MAX];
  char sPlugin[PATH_MAX];
  AmFSNode oNode;
 
  (*g_engfuncs.pfnGetGameDir)(sGameDir);
  memset(sPlugin,0x0,PATH_MAX);
  // Get the plugin, relative to the game mod's dlls dir.
  snprintf(sPlugin,PATH_MAX,"%s/%s",sGameDir,sLine);
  FormatPath(sPlugin);

  // Check if this is a directory
  oNode.set( sPlugin );
  if ( oNode.is_directory() ) {
	// Add all scripts (.amx files) from this directory
	AmDir* poDir = oNode.get_directory_handle();
	poDir->sort();
	const char* pcNode;
	while (nullptr != (pcNode = poDir->get_next_entry(oNode)) ) {
		// Check if this is a file with ending '.amx' (or '.amx64' on 64bit)
#ifdef __amd64__
		const char* const pcAmxExt = ".amx64";
#else
		const char* const pcAmxExt = ".amx";
#endif
		if (oNode.is_file() && strcasecmp(pcNode+strlen(pcNode)-4, pcAmxExt) == 0 ) {
			DEBUG_LOG(1, ("Checking plugin '%s' in directory '%s'", pcNode, sPlugin) );
			char acDirPl[PATH_MAX];
			snprintf( acDirPl, PATH_MAX, "%s/%s", sLine, pcNode );
			// Add this plugin
			ParsePlugin( acDirPl );
		}  // if
	}	// while

	// we're done here, let's return
	return TRUE;
  }

  DEBUG_LOG(1, ( "Found plugin '%s'\n",sPlugin) );
  CPlugin* pPlugin = new CPlugin;
  // Attempt to load the plugin.
  if(pPlugin->LoadPlugin(sPlugin)) {
    m_pPluginList->AddLink(pPlugin);
    // Attempt to start the plugin.
    if(pPlugin->StartPlugin()) {
      // Goody.  It worked.
      return TRUE;
    } else {
      UTIL_LogPrintf("[ADMIN] ERROR: StartPlugin on plugin '%s' failed. Plugin not loaded.\n",sPlugin);
    }
  } else {
    UTIL_LogPrintf( "[ADMIN] ERROR: LoadPlugin on plugin '%s' failed. Plugin not loaded.\n",sPlugin);
	delete pPlugin;
  }
  return FALSE;
}

#ifdef USE_MYSQL
// Loads a plugin from a MySQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParsePluginSQL(MYSQL_ROW pRow) {
  char sPlugin[LINE_SIZE];
  
  strcpy(sPlugin, pRow[0]);
  return ParsePlugin(sPlugin);
}
#endif

#ifdef USE_PGSQL
// Loads a plugin from a PgSQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParsePluginPgSQL(const PGresult *res, int tup_num) {
  char sPlugin[LINE_SIZE];
  
  strcpy(sPlugin, PQgetvalue(res, tup_num, 0));
  return ParsePlugin(sPlugin);
}
#endif

// Loads the plugins.  Returns TRUE if at least one plugin is loaded successfully,
// FALSE otherwise.
BOOL LoadPlugins() {
  // Debug: Print out the game dir
  char sPluginDir[PATH_MAX];

  if((int)CVAR_GET_FLOAT("admin_debug")!=0) {
    (*g_engfuncs.pfnGetGameDir)(sPluginDir);
	FormatPath(sPluginDir);
    UTIL_LogPrintf("[ADMIN] LoadPlugins: Game dir is '%s'\n", sPluginDir);
  }
  
  // Create a new linked list, if necessary.
  if (m_pPluginList == nullptr) 
    m_pPluginList = new CLinkList<CPlugin>();
  
  // Init the help linked list
  if (m_pHelpList == nullptr) 
    m_pHelpList = new CLinkList<help_struct>();
  m_pHelpList->Init();
  
  // Load the plugins
#ifdef USE_MYSQL
  if (!LoadTable("plugin", m_pPluginList, "mysql_dbtable_plugins", "SELECT plugin FROM %s\n", &ParsePluginSQL)) {
#endif
#ifdef USE_PGSQL
   if (!LoadPgTable("plugin", m_pPluginList, "pgsql_dbtable_plugins", "SELECT plugin FROM %s\n", &ParsePluginPgSQL)) {
#endif
	// check if the admin_plugin_file cvar holds a directory name
	char* sFile = const_cast<char*>(get_cvar_file_value( "admin_plugin_file" ));
	if ( sFile == nullptr ) {
		DEBUG_LOG(1, ("LoadPlugins()::admin_plugin_file cvar not set.  No plugins loaded.\n") );
	} else {
		snprintf( sPluginDir, PATH_MAX, "%s/%s", GET_GAME_INFO(PLID,GINFO_GAMEDIR), sFile );
		FormatPath(sPluginDir);
		if ( AmFSNode(sPluginDir).is_directory() ) {
			// load from this directory
			m_pPluginList->Init();
			ParsePlugin( sFile );
  			UTIL_LogPrintf( "[ADMIN] Loaded %i plugins\n", m_pPluginList->Count() );
		} else {
			// Load from a file
			LoadFile("plugin", m_pPluginList, "admin_plugin_file", &ParsePlugin);
		}  // if
	}  // if
#ifdef USE_PGSQL
	}
#endif
#ifdef USE_MYSQL
  }
#endif
  
  return ((m_pPluginList->Count() > 0) ? TRUE : FALSE);
}


void UnloadPlugins() {

  DEBUG_LOG(3, ("UnloadPlugins\n") );
  
  delete m_pPluginList;
  m_pPluginList = nullptr;
  
  delete m_pHelpList;
  m_pHelpList = nullptr;
}


/***************************
 *
 * Vault functions
 *
 ***************************/
vault_struct* FindVaultData(char* sKey) {
	// Make sure the vault is loaded
  if (g_fVaultLoaded == FALSE)
    LoadVault();
  
  if (m_pVaultList == nullptr) 
    return nullptr;
  
  CLinkItem<vault_struct>* pLink = m_pVaultList->FirstLink();
  while (pLink != nullptr) {
    vault_struct* tVault = pLink->Data();
    if (!stricmp(sKey, tVault->sKey)) {
      return tVault;
    }
    pLink = pLink->NextLink();
  }
  return nullptr;
}

char* GetVaultData(char* sKey) {
	vault_struct* tVault = FindVaultData(sKey);
  if (tVault == nullptr) {
    return nullptr;
  } else {
    return tVault->sData;
  }
}

// Loads a vault record from line in a file.  Returns TRUE if successful, 
// FALSE otherwise. The format is:
// <key> <data>
BOOL ParseVault(char* sLine) {
  char sDelimiter[] = " ";
  char* sKeyToken = nullptr;
  char* sDataToken = nullptr;
  const int iLineLen = strlen( sLine );

  sKeyToken = strtok(sLine,sDelimiter);
  if (sKeyToken == nullptr) {
    UTIL_LogPrintf("[ADMIN] ERROR: No vault key found: '%s'\n", sLine);
  } else if ((int)strlen(sKeyToken) > BUF_SIZE) {
    UTIL_LogPrintf("[ADMIN] ERROR: Vault key too long: '%s'\n", sKeyToken);
  } else {

    //sDataToken = strtok(nullptr,sDelimiter);
    const int iKeyLen = strlen( sKeyToken );
    if ( iLineLen > iKeyLen ) {
      sDataToken = sLine + iKeyLen + 1;
      // skip leading whitespace
      sDataToken += strspn( sDataToken, " \t" );
    }  // if
    if (sDataToken == nullptr) {
      UTIL_LogPrintf("[ADMIN] ERROR: No vault data found: '%s'\n", sLine);
    } else if ((int)strlen(sDataToken) > BUF_SIZE) {
      UTIL_LogPrintf("[ADMIN] ERROR: Vault data too long: '%s'\n", sDataToken);
    } else {
      vault_struct* tVault = new vault_struct;
      if(tVault == nullptr) {
	UTIL_LogPrintf( "[ADMIN] ERROR: ParseVault::'new' failed for tVault record.\n");
	return FALSE;
      }
      memset(tVault,0x0,sizeof(vault_struct));
      strcpy(tVault->sKey,sKeyToken);
      strcpy(tVault->sData,sDataToken);
      m_pVaultList->AddLink(tVault);
      
      return TRUE;
    }
  }
  return FALSE;
}

void LoadVault() {
   // Make sure we're not already loaded.
  if (g_fVaultLoaded == TRUE)
    return;
  // Note that we're now loaded.
  g_fVaultLoaded = TRUE;

	// Create a new linked list, if necessary.
  if (m_pVaultList == nullptr) 
    m_pVaultList = new CLinkList<vault_struct>();
  
  // Load the vault
  LoadFile("vault record", m_pVaultList, "admin_vault_file", &ParseVault);
}


void UnloadVault() {

  g_fVaultLoaded = FALSE;

  if ( m_pVaultList == nullptr ) return;

  delete  m_pVaultList;
  m_pVaultList = nullptr;
  
}


void SaveVault() {
	char sGameDir[2048];
  char sVaultFile[LINE_SIZE];

	if (m_pVaultList == nullptr || !g_fVaultLoaded) 
    return;
  
  const char* sFile = get_cvar_file_value("admin_vault_file");
  if ( sFile == nullptr ) return;
  
  (*g_engfuncs.pfnGetGameDir)(sGameDir);
  sprintf(sVaultFile,"%s/%s",sGameDir,sFile);
  FormatPath(sVaultFile);
  
  FILE* pFile = fopen(sVaultFile, "w");
  if (pFile == nullptr) 
    return;
  
  CLinkItem<vault_struct>* pLink = m_pVaultList->FirstLink();
  while (pLink != nullptr) {
    vault_struct* tVault = pLink->Data();
    fprintf(pFile,"%s %s\n", tVault->sKey, tVault->sData);
    pLink = pLink->NextLink();
  }
  fclose(pFile);
}

void SetVaultData(char* sKey, char* sData) {
	vault_struct* tVault = FindVaultData(sKey);
  if (tVault == nullptr) {
    tVault = new vault_struct;
    if(tVault == nullptr) {
      UTIL_LogPrintf( "[ADMIN] ERROR: SetVaultData::'new' failed for tVault record.\n");
      return;
    }
    strcpy(tVault->sKey, sKey);
    strcpy(tVault->sData, sData);
    m_pVaultList->AddLink(tVault);
  } else {
    if (sData == nullptr || strlen(sData) == 0) {
      CLinkItem<vault_struct>* pLink = m_pVaultList->FindLink(tVault);
      if (pLink != nullptr) 
	m_pVaultList->DeleteLink(pLink);
    } else {
      strcpy(tVault->sData, sData);
    }
  }
  SaveVault();
}
