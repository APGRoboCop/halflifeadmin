/*
 * $Id: users.cpp,v 1.28 2001/09/27 20:33:16 darope Exp $
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
 * routines for handling user functions for the admin_ modules
 *
 */
#include <string.h>

#ifndef _WIN32
#  include <regex.h>
#else
#  include "gnuregex.h"
#endif


#ifndef _WIN32
#  include <crypt.h>
#endif


#include "version.h"
#include "extdll.h"
#include "amutil.h"
#include "users.h"

#ifdef USE_MYSQL
#  include <mysql.h>
#  include <errmsg.h>
#  include "extdll.h"
  extern MYSQL mysql;
  extern BOOL g_fUseMySQL;
#else
#  include "extdll.h"
#endif


// This is it; the linked lists that we store all of our
// loaded admin mod data in.  Each list gets initialized in
// the corresponding Load*() functions (eg, m_pModelList
// gets initialized in LoadModels(), with the exception of
// m_pHelpList, which is initialized in LoadPlugins(),
// and m_pSpawnList, which is called in InitSpawnEntityList,
// and not loaded from a file at all)
CLinkList<help_struct>* m_pHelpList   = NULL;
CLinkList<ip_struct>* m_pIPList     = NULL;
CLinkList<model_struct>* m_pModelList  = NULL;
CLinkList<CPlugin>* m_pPluginList = NULL;
CLinkList<spawn_struct>* m_pSpawnList  = NULL;
CLinkList<user_struct>* m_pUserList   = NULL;
CLinkList<vault_struct>* m_pVaultList  = NULL;
CLinkList<word_struct>* m_pWordList   = NULL;

// Auth structure array.  +1 so we can go from 0 - MAX_PLAYERS, rather
// than 0 - MAX_PLAYERS - 1.
auth_struct g_AuthArray[MAX_PLAYERS + 1];

// The index of the current highlander.  This is set by
// GetUserAccess, and is used for tracking when the highlander
// changes.  Only useful if admin_highlander is != 0.
int m_iHighlanderIndex = 0;

// The current spawn entity; each entity spawned should increment
// this by one.
int m_iSpawnIdentity = 1;

// The current user index; this is used by LoadUsers() as it parses
// the incoming user records.
int m_iUserIndex = 1;

extern enginefuncs_t  g_engfuncs;
extern globalvars_t  *gpGlobals;
DLL_GLOBAL BOOL  g_fInitialized;
DLL_GLOBAL BOOL  g_fIPsLoaded;
DLL_GLOBAL BOOL  g_fModelsLoaded;
DLL_GLOBAL BOOL  g_fUsersLoaded;
DLL_GLOBAL BOOL  g_fVaultLoaded;
DLL_GLOBAL BOOL  g_fWordsLoaded;
DLL_GLOBAL BOOL  g_fRunPlugins;
DLL_GLOBAL BOOL  g_fRunScripts;

mapcycle_t mapcycle;

void ShowMenu (edict_t* pev, int bitsValidSlots, int nDisplayTime, BOOL fNeedMore, char pszText[1024]) {
  int gmsgShowMenu = 0;
  gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
  MESSAGE_BEGIN( MSG_ONE, gmsgShowMenu, NULL, pev);
  
  WRITE_SHORT( bitsValidSlots);
  WRITE_CHAR( nDisplayTime );
  WRITE_BYTE( fNeedMore );
  WRITE_STRING (pszText);
  
  MESSAGE_END();  
}

/***************************
*
* Miscellaneous functions
*
***************************/
// Gets the file in sFilename (relative to the game's mod dir), and returns it as a 
// linked list of the lines. Empty lines aren't returned, and neither are commented
//  out lines (determined by IsComment) or those greater than LINE_SIZE in length.
// May return a NULL pointer.  The linked list returned must be delete()'d, of course.
CLinkList<char, true>* GetFile(char* sFilename) {
  int iBegin = 0;
  int iEnd = 0;
  int iLength;
  int iPos;
  char* sFile;
  char* sLine;
  CLinkList<char,true>* pLineList = NULL;

  // Load the file up.
  sFile = (char*)LOAD_FILE_FOR_ME(sFilename,&iLength);
  // Check to make sure that we've actually got a file, and it's actually got data.
  if (!(sFile && iLength)) {
    UTIL_LogPrintf("[ADMIN] WARNING: File '%s' seems to be empty (length %i).\n", sFilename, iLength);
  } else {
    pLineList = new CLinkList<char,true>();
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
	// Make sure it's not a comment
      } else if (!IsComment(sFile[iBegin])) {
	// It passes.  Add it in.
	sLine = new char[LINE_SIZE];
	strncpy(sLine, &sFile[iBegin], iEnd - iBegin);
	// Don't forget to null-terminate.
	sLine[iEnd - iBegin] = '\0';
	// Strip off ending whitespace
	iPos = strlen(sLine) - 1;
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

// Returns the number of free (non-reserved slots).  Can be given an entity
// to not 'count' in determining this number (useful if being called when said
// entity is disconnecting, for example).
int GetFreeSlots(edict_t* pEntityIgnore = NULL) {
  int iPlayerCount = GetPlayerCount();
  int iResType = (int)CVAR_GET_FLOAT("reserve_type");
  int iResSlots = 0;
  
  // If we're ignoring someone, then we should decrement our player
  // count to count for the ignored party.
  if (pEntityIgnore != NULL && iPlayerCount > 0)
    iPlayerCount--;
  
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
    int i;
    
    for (i = 1; i <= gpGlobals->maxClients; i++) {
      CBaseEntity* pPlayer = UTIL_PlayerByIndex(i);
      if(IsPlayerValid(pPlayer) && pPlayer->edict() != pEntityIgnore) {
	if (IsIPReserved(g_AuthArray[i].sIP) || GetUserAccess(pPlayer->edict())) {
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
  
  CVAR_SET_FLOAT("public_slots_free",GetFreeSlots(NULL));
  CVAR_SET_FLOAT("sv_visiblemaxplayers",GetFreeSlots(NULL));
}


// This procedure is a generic function that will load records from a file.  Given a
// user-readable type (for the messages), a linked list (which will get initialized),
// a cvar (holding the file name to read), and a function pointer, it will call the
// function pointer for each valid line in the file pointed to by the cvar.

template<class T, bool isArray> void LoadFile(char* sType, CLinkList<T,isArray>* pList, char* sFileVar, PARSE_FILE fpParse) {
  char* sFile = (char*)CVAR_GET_STRING(sFileVar);
  CLinkItem<char,true>* pLine;
  CLinkList<char,true>* pFile;
  
  if (pList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: LoadFile for '%s' called with NULL linked list.\n", sType);
    return;
  }
  pList->Init();
  
  // Make sure we have a file.
  if (sFile == NULL || strlen(sFile) == 0 || FStrEq(sFile,"0")) {
		if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
			UTIL_LogPrintf("[ADMIN] DEBUG: LoadFile::%s cvar empty.  No %ss loaded.\n", sFileVar, sType);
		}
    return;
  }
  FormatPath(sFile);
  UTIL_LogPrintf("[ADMIN] Loading %ss from file '%s'\n",sType,sFile);
  // Get the file in linked-list format.
  pFile = GetFile(sFile);
  if (pFile != NULL) {
    // For every line in the file...
    pLine = pFile->FirstLink();
    while (pLine != NULL) {
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
  char sQuery[4096];
  char* sTable = (char*)CVAR_GET_STRING(sTableVar);
  MYSQL_RES *pResult;
  MYSQL_ROW pRow = NULL;

// if its a user query and we don't want to preload the file exit right now
  if( FStrEq(sType,"user") && (int)CVAR_GET_FLOAT("mysql_preload") == 0) return TRUE;

  if (pList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: LoadTable for '%s' called with NULL linked list.\n", sType);
    return FALSE;
  }
  pList->Init();

  // Make sure that MySQL is initialized, and that we have a table.
  if (g_fUseMySQL == FALSE) {
    return FALSE;
  } else if (sTable == NULL || strlen(sTable) == 0 || FStrEq(sTable,"0")) {
		if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
			UTIL_LogPrintf("[ADMIN] DEBUG: LoadTable::%s cvar empty.  Switching to loading from file...\n", sTableVar);
		}
    return FALSE;
  }





  // Select everything in the table.
  UTIL_LogPrintf("[ADMIN] Loading %ss from MySQL table '%s'\n", sType, sTable);
  sprintf(sQuery, sSQL, sTable);
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
    UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned NULL result.\n", sType);
  } else {
    // For every row in the result set...
    while ((pRow = mysql_fetch_row(pResult)) != NULL) {

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

// Returns 1 if the strings match, 0 otherwise.  'Matching' may include
// a regex check, if use_regex is not 0; otherwise, it'll be a straight
// string check.
int match(const char *string, char *pattern) {
  // Null pointers match anything.
  if ( string == NULL || pattern == NULL ) {
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
    int    status;
    regex_t    re;
    
    if ( regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB|REG_ICASE) != 0) {
      return(0);      // report error
    }
    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0) {
      return(0);      // report error
    }
    return(1);
  }
}

// Returns 1 if the passwords match, 0 otherwise. 'Matching' may include
// encryption, if encrypt_passwords is not 0.  Otherwise, it'll be straight
// string matching.
// sServerPassword is the password the server has (in users.ini, or whatever);
// sPlayerPassword is the password the player has (via admin_password, or whatever).
int pass_compare( char* sServerPassword,char* sPlayerPassword) {
  char *sEncrypt = 0;

  if ( sServerPassword == 0 || sPlayerPassword == 0 ) {
    UTIL_LogPrintf("[ADMIN] ERROR: pass_compare called with NULL pointer\n");
    return 0;
  }  // if
  
#ifndef _WIN32 
  if ((int)CVAR_GET_FLOAT("encrypt_password")==0) {
    // No encryption here.  Just use the password as-is.
    sEncrypt=(char *)&sPlayerPassword[0];
  } else {
    // Otherwise, encrypt with a salt based on the user password.
    //char sSalt[2];
    //strncpy(sSalt,sServerPassword,2);
    sEncrypt = crypt(sPlayerPassword,sServerPassword);
  }
#else
  // No encryption here.  Just use the password as-is.
  sEncrypt=(char *)&sPlayerPassword[0];
#endif
  // Return the comparison between the two.
  return(!strncmp(sEncrypt,sServerPassword,PASSWORD_SIZE));
}

/***************************
 *
 * Model functions
 *
 ***************************/
// Returns 1 if the entity has access to use sModel, 0 otherwise.
int GetModelAccess(char* sModel, edict_t* pEntity) {
  int iIndex = ENTINDEX(pEntity);
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
// with the first info that matches that name/password combination.  A NULL password
// bypasses the password match (useful for testing for existance).
// Returns TRUE if successful, FALSE otherwise.
BOOL GetModelRecord(char* sModel, char* sPassword, model_struct* tModelRecord) {
  // Make sure we're passed a pointer to use.
  if (tModelRecord == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetModelRecord called with NULL tModelRecord.\n");
    return FALSE;
  }
  
  // Make sure the models are loaded.
  if (g_fModelsLoaded == FALSE)
    LoadModels();
  
  // If we have no model list, we can't match.
  if (m_pModelList == NULL) 
    return FALSE;

  CLinkItem<model_struct>* pModel = m_pModelList->FirstLink();
  model_struct* tModel = NULL;

  // Go through each model in the list.
  while (pModel != NULL) {
    tModel = pModel->Data();
    // If the model's names match...
    if (match(sModel,tModel->sModelName)==1) {
      // A NULL password matches anything; otherwise, we need
      // to compare passwords.
      if (sPassword == NULL) {
	break;
      } else if (pass_compare(tModel->sPassword,sPassword)) {
	break;
      }
    }
    pModel = pModel->NextLink();
  }
  
  // We got a match
  if (pModel != NULL) {
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
  
  return GetModelRecord(sModel, NULL, &tModel);
}

// Loads a model record from line in a file.  Returns TRUE if successful, 
// FALSE otherwise. The format is:
// <model name>:<password>
BOOL ParseModel(char* sLine) {
  char sDelimiter[] = ":";
  char* sNameToken;
  char* sPasswordToken;
  model_struct* tModel;
  
  sNameToken = strtok(sLine,sDelimiter);
  if (sNameToken == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: No Model name found: '%s'\n", sLine);
  } else if ((int)strlen(sNameToken) > USERNAME_SIZE) {
    UTIL_LogPrintf("[ADMIN] ERROR: Model name too long: '%s'\n", sNameToken);
  } else {
    sPasswordToken = strtok(NULL,sDelimiter);
    if (sPasswordToken == NULL) {
      UTIL_LogPrintf("[ADMIN] ERROR: No Model password found: '%s'\n", sLine);
    } else if ((int)strlen(sPasswordToken) > PASSWORD_SIZE) {
      UTIL_LogPrintf("[ADMIN] ERROR: Model password too long: '%s'\n", sPasswordToken);
    } else {
      tModel = new model_struct;
      if(tModel == NULL) {
	UTIL_LogPrintf( "[ADMIN] ERROR: ParseModel::'new' failed for tModel record.\n");
	return FALSE;
      }
      memset(tModel,0x0,sizeof(model_struct));
      strcpy(tModel->sModelName,sNameToken);
      strcpy(tModel->sPassword,sPasswordToken);
      m_pModelList->AddLink(tModel);
      
      if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
	UTIL_LogPrintf( "[ADMIN] DEBUG: Model loaded: Name '%s', Password '%s'\n",tModel->sModelName, tModel->sPassword);
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
  if(tModel == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseModelSQL::'new' failed for tModel record.\n");
    return FALSE;
  }
  memset(tModel,0x0,sizeof(model_struct));
  strcpy(tModel->sModelName,pRow[0]);
  strcpy(tModel->sPassword,pRow[1]);
  m_pModelList->AddLink(tModel);

  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf( "[ADMIN] DEBUG: Model loaded: Name '%s', Password '%s'\n",tModel->sModelName, tModel->sPassword);
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
  if (m_pModelList == NULL)
    m_pModelList = new CLinkList<model_struct>();

  // Load the Models
#ifdef USE_MYSQL
  if (!LoadTable("model", m_pModelList, "mysql_dbtable_models", "SELECT nick,pass FROM %s\n", &ParseModelSQL))
#endif
    LoadFile("model", m_pModelList, "models_file", &ParseModel);
}

/***************************
 *
 * IP functions
 *
 ***************************/
// Given an IP string (xxx.xxx.xxx.xxx), returns it in unsigned long (32-bit) format.
void IPStringToBits(char* sIP, ulong* lIP) {
  int iShift;
  ulong lByte;
  char* sChar = sIP;
  
  *lIP = 0x00000000;
  for( iShift = 24; *sChar && iShift >= 0; iShift -= 8) {
    lByte = atoi(sChar);
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
BOOL IsIPValid(char* sIP) {
  int iDotCount = 0;
  char* sChar = sIP;
  
  // If the string is NULL, it's obviously invalid.
  if (sIP == NULL) {
    return FALSE;
    // If the string is too big, it's invalid.
  } else if ((int)strlen(sIP) > IP_SIZE) {
    return FALSE;
  } else {
    while (sChar != NULL) {
      if (*sChar == '.') {
	iDotCount++;
	// If the string has more than three periods, it's invalid.
	if (iDotCount > 3)
	  return FALSE;
	// If the string has characters that aren't digits and aren't periods, it's invalid.
      } else if (!isdigit(*sChar)) {
	return FALSE;
      }
      sChar++;
    }
  }
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
  if (m_pIPList == NULL)
    return FALSE;
  
  CLinkItem<ip_struct>* pLink = m_pIPList->FirstLink();
  ip_struct* tIP = NULL;
  ulong lIP;
  
  // Convert the IP to bits.
  IPStringToBits(sIP,&lIP);
  // For each record...
  while (pLink != NULL) {
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
  int iHasMask;
  char sDelimiter[] = "/";
  char* sIPToken;
  char* sMaskToken;
  ip_struct* tIP;
  
  sIPToken = strtok(sLine,sDelimiter);
  if (sIPToken == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: No IP found: '%s'\n", sLine);
  } else if ((int)strlen(sIPToken) > IP_SIZE) {
    UTIL_LogPrintf("[ADMIN] ERROR: IP too long: '%s'\n", sIPToken);
  } else if (!IsIPValid(sIPToken) == 0) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Invalid IP address: %s\n", sIPToken);
  } else {
    // It's allowable to not have a mask.
    iHasMask = 0;
    sMaskToken = strtok(NULL,sDelimiter);
    if (sMaskToken != NULL) {
      if ((int)strlen(sMaskToken) > PASSWORD_SIZE) {
	UTIL_LogPrintf("[ADMIN] ERROR: IP mask too long: '%s'\n", sMaskToken);
      } else if (!IsIPValid(sMaskToken) == 0) {
	UTIL_LogPrintf( "[ADMIN] ERROR: Invalid IP Mask: %s\n", sMaskToken);
      } else {
	iHasMask = 1;
      }
    }
    tIP = new ip_struct;
    if(tIP == NULL) {
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
  if(tIP == NULL) {
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

void LoadIPs() {
  // Make sure we're not already loaded.
  if (g_fIPsLoaded == TRUE)
    return;
  // Note that we're now loaded.
  g_fIPsLoaded = TRUE;

  // Create a new linked list, if necessary.
  if (m_pIPList == NULL)
    m_pIPList = new CLinkList<ip_struct>();

  // Load the IPs
#ifdef USE_MYSQL
  if (!LoadTable("IP", m_pIPList, "mysql_dbtable_ips", "SELECT ip FROM %s\n", &ParseIPSQL))
#endif
    LoadFile("IP", m_pIPList, "ips_file", &ParseIP);
}

/***************************
 *
 * Word detection functions
 *
 ***************************/

// Loads a word record from a line in a file.  Returns TRUE if successful,
// FALSE otherwise.  The format is:
// <word>
BOOL ParseWord(char* sLine) {
  int i;
  word_struct* tWord;

  tWord = new word_struct;
  if(tWord == NULL) {
    UTIL_LogPrintf( "[ADMIN] ERROR: ParseWord::'new' failed for tWord record.\n");
    return FALSE;
  }
  memset(tWord,0x0,sizeof(word_struct));
  strcpy(tWord->sWord,sLine);

  for (i = 0; i < (int)strlen(tWord->sWord); i++)
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

void LoadWords(void) {
  // Make sure we're not already loaded.
  if (g_fWordsLoaded == TRUE)
    return;
  // Note that we're now loaded.
  g_fWordsLoaded = TRUE;
  
  // Create a new linked list, if necessary.
  if (m_pWordList == NULL) 
    m_pWordList = new CLinkList<word_struct>();
  
  // Load the words
#ifdef USE_MYSQL
  if (!LoadTable("word", m_pWordList, "mysql_dbtable_words", "SELECT word FROM %s\n", &ParseWordSQL))
#endif
    LoadFile("word", m_pWordList, "words_file", &ParseWord);
}

/***************************
 *
 * User functions
 *
 ***************************/
// Initializes the authorization record for the given user.  Also calls
// SetUserPassword and VerifyUserAuth in attempt to find user's access
// level.
void AddUserAuth(char* sName, char* sIP, edict_t* pEntity) {
  int iIndex = ENTINDEX(pEntity);
  unsigned long ulAuthID;
  int iReconnTime = 300;

  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: AddUserAuth: User '%s' has out of bounds entity index %i\n", sName, iIndex);
    return;
  }

  if ( (int)CVAR_GET_FLOAT("sv_lan") ) {
    ulAuthID = 0;
  } else {
    ulAuthID = GETPLAYERWONID( pEntity );
  }  // if-else
  
  iReconnTime = (int)CVAR_GET_FLOAT( "admin_reconnect_timeout" );


  if (g_AuthArray[iIndex].iSessionID == GETPLAYERUSERID(pEntity) && !strcmp(sName, g_AuthArray[iIndex].sUserName) 
      && !strcmp(sIP, g_AuthArray[iIndex].sIP) && ((time(NULL) - g_AuthArray[iIndex].iTime) < 180)) {
    if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: AddUserAuth: Mapchange for index %i.\n", iIndex);
    }
    g_AuthArray[iIndex].iTime = time(NULL);
   
  } else if (g_AuthArray[iIndex].ulAuthID == ulAuthID && !strcmp(sName, g_AuthArray[iIndex].sUserName) 
      && !strcmp(sIP, g_AuthArray[iIndex].sIP) && ((time(NULL) - g_AuthArray[iIndex].iTime) < iReconnTime)) {
    if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: AddUserAuth: Reconnect for index %i.\n", iIndex);
    }
    if ( iReconnTime > 600 ) {
      UTIL_LogPrintf("[ADMIN] WARNING: Your reconnect timeout is higher than 600 seconds. This may pose a security risk.\n" );
      UTIL_LogPrintf("                 It is recommended to have a timeout of only 300 seconds.\n" );
    }  // if
    g_AuthArray[iIndex].iTime = time(NULL);
   
  } else {
    // Clear and repopulate the record.
    memset(&g_AuthArray[iIndex],0x0,sizeof(auth_struct));
    if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: AddUserAuth: Clearing index %i\n", iIndex);
    }
    g_AuthArray[iIndex].iAccess = CVAR_GET_FLOAT("default_access");
    g_AuthArray[iIndex].iSessionID = GETPLAYERUSERID(pEntity);
    g_AuthArray[iIndex].ulAuthID = ulAuthID;
    g_AuthArray[iIndex].iTime = time(NULL);
    g_AuthArray[iIndex].iUserIndex = 0;
    strncpy(g_AuthArray[iIndex].sUserName, sName, USERNAME_SIZE);
    strcpy(g_AuthArray[iIndex].sPassword,"");
    strncpy(g_AuthArray[iIndex].sIP,sIP,IP_SIZE);
    // Try to get the user's password from the setinfo buffer
    SetUserPassword(sName, NULL, pEntity);
    // Try to find a matching user record.
    VerifyUserAuth(sName, pEntity);
  }
}

// Returns the index of the Highlander, or 0 if there is no Highlander.
// The Highlander is returned only if admin_highlander is non-zero; the
// Highlander is the person online with the most access.  In the case of
// ties, it is the person with the most access and the smallest user index
// (ie, the one closest to the top of the list)
int GetHighlanderIndex() {
  int i;
  int iHighlanderIndex = 0;
  int iMaxAccess = 0;
  int iMinUserIndex = 9999;
  CBaseEntity* pPlayer;
  
  // If admin_highlander is 0, there is no highlander.
  if((int)CVAR_GET_FLOAT("admin_highlander")==0) {
    return 0;
  }
  // Otherwise, get the highest access with the smallest user index.
  for(i = 1; i <= gpGlobals->maxClients; i++) {
    if (g_AuthArray[i].iUserIndex != 0) {
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
      }
    }
    m_iHighlanderIndex = iHighlanderIndex;
  }
  return iHighlanderIndex;
}

// Returns the access associated with an entity.
int GetUserAccess(edict_t* pEntity) {
  int iHighlanderIndex;
  int iIndex;
  
  // Console has complete access.
  if (pEntity == NULL) {
    return -1;
  }
  iIndex = ENTINDEX(pEntity);
  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetUserAccess: User '%s' has out of bounds entity index %i\n", STRING(pEntity->v.netname), iIndex);
    return 0;
  }
  // Check to see if we're in highlander mode.  If we are, anybody besides the
  // highlander should only get reserve-name, reserve-spot, immunity, and default access.
  iHighlanderIndex = GetHighlanderIndex();
  if (iHighlanderIndex != 0 && iHighlanderIndex != iIndex) {
    return (g_AuthArray[iIndex].iAccess & (ACCESS_RESERVE_NICK | ACCESS_RESERVE_SPOT | ACCESS_IMMUNITY | (int)CVAR_GET_FLOAT("default_access")));
    // Otherwise, return the person's access.
  } else {
    return g_AuthArray[iIndex].iAccess;
  }
}

// Given a user name and/or WON ID, and optionally password, fills the
// user_struct record with the first info that matches that name/password
// combination.  A NULL password bypasses the password match (useful
// for testing for existance).
// Returns TRUE if successful, FALSE otherwise.
BOOL GetUserRecord(const char* sName, int iWONID, char* sPassword, user_struct* tUserRecord) {
  // Make sure we have a record to write to.
  if (tUserRecord == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetUserRecord called with NULL tUserRecord.\n");
    return 0;
  }

  // Make sure the users are loaded.
  if (g_fUsersLoaded == FALSE)
    LoadUsers();

user_struct *tUser = NULL;

CLinkItem<user_struct>*  pUser=NULL;

#ifdef USE_MYSQL

 user_struct User; // temporary store for user info pulled from db
 //int iResult;      // used for db func return values
 // char *sType="user"; // the "type" of query we are doing
 MYSQL_RES *pResult;
 MYSQL_ROW pRow = NULL;	
 BOOL valid_user=FALSE;

 if( (int)CVAR_GET_FLOAT("mysql_preload")==0 ) {
  	
   char sQuery[BUF_SIZE];

   sprintf(sQuery,(char*)CVAR_GET_STRING("mysql_users_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_users"),sName,iWONID);

   pResult=admin_mysql_query(sQuery,"user");
  
   // Make sure we got a valid result back.
   if ( !pResult) {
     UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned NULL result.\n");
   } else {

     // For every row in the result set...
     while ((pRow = mysql_fetch_row(pResult)) != NULL) {
       valid_user=TRUE; // there was at least one user...
       if(pass_compare(pRow[0],sPassword)) {
	 // if the users PW matches the DB one then setup the Users struct and copy it across
	 strncpy(User.sUserName,sName,USERNAME_SIZE);
	 strncpy(User.sPassword,sPassword,PASSWORD_SIZE);
	 User.iAccess=atoi(pRow[1]);
	 User.iIndex= GetPlayerIndex((char*)sName);
	 tUser=&User;
	 //printf("User found:%s %i\n",User.sUserName,User.iAccess);
	 // KLUDGE, KLUDE!! sets pUser to non null to show a match
	 pUser=(CLinkItem<user_struct, false> *)1;
       }
      
     }
     mysql_free_result(pResult);
   }


   if (valid_user==FALSE) {
     sprintf(sQuery,(char*)CVAR_GET_STRING("mysql_tags_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_tags"),sName,iWONID);
     if ((int)CVAR_GET_FLOAT("admin_debug") != 0) 
       UTIL_LogPrintf("[ADMIN]Trying tags db with %s\n",sQuery);
      
     pResult=admin_mysql_query(sQuery,"user");
 
     // Make sure we got a valid result back.
     if ( !pResult) {
       UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned NULL result.\n");
     } else {
	
       // For every row in the result set...
       while ((pRow = mysql_fetch_row(pResult)) != NULL) {
	 if(pass_compare(pRow[0],sPassword)) {
	   // if the users PW matches the DB one then setup the Users struct and copy it across
	   strncpy(User.sUserName,sName,USERNAME_SIZE);
	   strncpy(User.sPassword,sPassword,PASSWORD_SIZE);
	   User.iAccess=atoi(pRow[1]);
	   User.iIndex= GetPlayerIndex((char*)sName);
	   tUser=&User;
	   //printf("User found:%s %i\n",User.sUserName,User.iAccess);
	   // KLUDGE, KLUDE!! sets pUser to non null to show a match
	   pUser=(CLinkItem<user_struct, false> *)1;
	 }
	  
       }
       mysql_free_result(pResult);
	
     } // else
   } // if(valid_user==FALSE)


    
 } else {
#endif
    // If we have no user list, we can't match.
    if (m_pUserList == NULL)
      return FALSE;
    
   pUser= m_pUserList->FirstLink();
   // user_struct* tUser = NULL;
    
    // For each user record...
    while (pUser != NULL) {
      tUser = pUser->Data();
      // Try to get a match; this match can be a name/name match, or a name/WONID match.
      if ((match(sName,tUser->sUserName)!=0) || (atoi(tUser->sUserName) != 0 && atoi(tUser->sUserName)==iWONID)) {
	// A NULL password matches anything.  Otherwise, we need to compare
	// passwords.
	if (sPassword == NULL) {
	  break;
	} else if (pass_compare(tUser->sPassword,sPassword)) {
	  break;
	}
      }
      pUser = pUser->NextLink();
    }
#ifdef USE_MYSQL
  }
#endif
  
  // We got a match.
  if (pUser != NULL) {
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf( "[ADMIN] DEBUG: Name '%s' / WONID '%i' matches user entry '%s'\n",sName,iWONID,tUser->sUserName);
    }
    memcpy(tUserRecord,tUser,sizeof(user_struct));
    return 1;
  }
  // No matching user, huh?
  return 0;
}

// Given a name and/or WONID, returns TRUE if that name/WONID is reserved
// (has access 16384), FALSE otherwise.
// Note that this will return TRUE if any record that matches sName has 16384
// access.
BOOL IsNameReserved(char* sName, int iWONID, user_struct* tUserRecord) {
  if (tUserRecord == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: IsNameReserved called with NULL tUserRecord.\n");
    return FALSE;
  }
  
  if (g_fUsersLoaded == FALSE)
    LoadUsers();
  

#ifdef USE_MYSQL

  //user_struct User; // tempory struct to store user info pulled from DB
  //int iResult;      // return val check for db funcs
  // char *sType="user";  // "type" of check
  MYSQL_RES *pResult;   // store for result set
  MYSQL_ROW pRow = NULL; // store for rows from results
 
  if( (int)CVAR_GET_FLOAT("mysql_preload")==0) { // if we didn't preload the users file
  	
    char sQuery[BUF_SIZE];
    
    sprintf(sQuery,(char*)CVAR_GET_STRING("mysql_users_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_users"),sName,iWONID);
    
    pResult=admin_mysql_query(sQuery,"user");
    
    
    
    // Make sure we got a valid result back.
    if ( !pResult) {
      UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned NULL result.\n");
    } else {
      // For every row in the result set...
      while ((pRow = mysql_fetch_row(pResult)) != NULL) {
	user_struct User;
	User.iAccess=atoi(pRow[1]);

	if ((User.iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {
	
	  /*strncpy(User.sUserName,sName,USERNAME_SIZE);
	    strncpy(User.sPassword,pRow[0],PASSWORD_SIZE);

	    User.iIndex= GetPlayerIndex((char*)sName);
	    memcpy(tUserRecord,&User,sizeof(user_struct));
	  */
	  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
	    UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, User.iAccess);
	  }  // if
	  mysql_free_result(pResult);
	  return TRUE;
	}

      }
  
    }
    mysql_free_result(pResult);
 
    // Didn't find a username, check for a tag :)
    sprintf(sQuery,(char*)CVAR_GET_STRING("mysql_tags_sql"),(char*)CVAR_GET_STRING("mysql_dbtable_tags"),sName,iWONID);
    if ((int)CVAR_GET_FLOAT("admin_debug") != 0) 
      UTIL_LogPrintf("[ADMIN]Trying tags db with %s\n",sQuery);
    
    pResult=admin_mysql_query(sQuery,"user");
      
    if ( !pResult) {
      UTIL_LogPrintf("[ADMIN] ERROR: Select query for users returned NULL result.\n");
    } else {
      // For every row in the result set...
      while ((pRow = mysql_fetch_row(pResult)) != NULL) {
	user_struct User;
	User.iAccess=atoi(pRow[1]);
	  
	if ((User.iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {
	    
	  /*strncpy(User.sUserName,sName,USERNAME_SIZE);
	    strncpy(User.sPassword,pRow[0],PASSWORD_SIZE);
	      
	    User.iIndex= GetPlayerIndex((char*)sName);
	    memcpy(tUserRecord,&User,sizeof(user_struct));
	  */
	    
	    
	  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
	    UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",sName, User.iAccess);
	  }  // if
	  mysql_free_result(pResult);
	  return TRUE;
	}
	  
      }
	
    }

    mysql_free_result(pResult);
  


    
  } else {
#endif



  CLinkItem<user_struct>* pUser = m_pUserList->FirstLink();
  user_struct* tUser = NULL;
  
  while (pUser != NULL) {
    tUser = pUser->Data();
    if ((match(sName,tUser->sUserName)==1) || (atoi(tUser->sUserName) != 0 && atoi(tUser->sUserName)==iWONID)) {
      if ((tUser->iAccess & ACCESS_RESERVE_NICK) == ACCESS_RESERVE_NICK) {
	memcpy(tUserRecord,tUser,sizeof(user_struct));

	if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
	  UTIL_LogPrintf( "[ADMIN] DEBUG: Name is reserved: Name '%s',  Access '%i'\n",tUser->sUserName, tUser->iAccess);
	}  // if

	return TRUE;
      }  // if
    }
    pUser = pUser->NextLink();
  }
#ifdef USE_MYSQL
}
#endif

  return FALSE;
}

// Loads a user record from a line in a file.  Returns TRUE if successful,
// FALSE otherwise.  The format is:
// <user name or WON ID>:<password>:<access>:
BOOL ParseUser(char* pcLine) {
  int iAccess;
  char* sAccessToken;
  char sDelimiter[] = ":";
  char* sNameToken;
  char* sPasswordToken;
  char* pcDelim = 0;
  user_struct* tUser;

  if ( pcLine == 0 ) return FALSE;

  int iLineLength = strlen(pcLine);
  char* sLine = new char[iLineLength+1];
  memset (sLine, 0, (iLineLength + 1) );
  memcpy( sLine, pcLine, iLineLength );

  //sNameToken = strtok(sLine,sDelimiter);

  sNameToken = sLine;
  pcDelim = strchr( sLine, ':' );
  if ( pcDelim > sLine ) {
    while ( *(pcDelim - 1) == '\\' ) {  // accept the colon as part of the name
      memmove( (pcDelim - 1), pcDelim, (iLineLength - (pcDelim - sLine) + 1) );
      pcDelim = strchr( (pcDelim + 1), ':' );
      if ( pcDelim == 0 ) break;
    }  // while
  }  // if

  if ( pcDelim == 0 ) {
    sNameToken = 0;
  } else {
    *pcDelim = 0;
    pcDelim++;
  }  // if-else

  if (sNameToken == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: No user name found: '%s'\n", sLine);
  } else if ((int)strlen(sNameToken) > USERNAME_SIZE) {
    UTIL_LogPrintf("[ADMIN] ERROR: User name too long: '%s'\n", sNameToken);
  } else {
    if ( *pcDelim == ':' ) {
      sPasswordToken = pcDelim;
      *pcDelim = 0;
      pcDelim++;
    } else {
      sPasswordToken = strtok(pcDelim,sDelimiter);
      pcDelim = 0;
    }  // if-else
    if (sPasswordToken == NULL) {
      UTIL_LogPrintf("[ADMIN] ERROR: No user password found: '%s'\n", sLine);
    } else if ((int)strlen(sPasswordToken) > PASSWORD_SIZE) {
      UTIL_LogPrintf("[ADMIN] ERROR: User password too long: '%s'\n", sPasswordToken);
    } else {
      if ( pcDelim == 0 ) {
	sAccessToken = strtok(NULL,sDelimiter);
      } else {
	sAccessToken = pcDelim;
      }  // if-else
      if (sAccessToken == NULL) {
	UTIL_LogPrintf("[ADMIN] ERROR: No user access found: '%s'\n", sLine);
      } else if ((int)strlen(sAccessToken) > ACCESS_SIZE) {
	UTIL_LogPrintf("[ADMIN] ERROR: User access too long: '%s'\n", sAccessToken);
      } else {
	tUser = new user_struct;
	if(tUser == NULL) {
	  UTIL_LogPrintf( "[ADMIN] ERROR: ParseUser::'new' failed for tUser record.\n");
	  delete[] sLine;
	  sLine = 0;
	  return FALSE;
	}
	memset(tUser,0x0,sizeof(user_struct));
	strcpy(tUser->sUserName,sNameToken);
	strcpy(tUser->sPassword,sPasswordToken);
	iAccess = atoi(sAccessToken);
	tUser->iAccess = iAccess;
	tUser->iIndex = m_iUserIndex++;
	m_pUserList->AddLink(tUser);
	
	if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
	  UTIL_LogPrintf( "[ADMIN] DEBUG: User loaded: Name '%s', Password '%s', Access '%i'\n",tUser->sUserName, tUser->sPassword, tUser->iAccess);
	}
	delete[] sLine;
	sLine = 0;
	return TRUE;
      }
    }
  }
  delete[] sLine;
  sLine = 0;
  return FALSE;
}

#ifdef USE_MYSQL
// Loads a user record from a MySQL result.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParseUserSQL(MYSQL_ROW pRow) {
  user_struct* tUser;
  
  tUser = new user_struct;
  if(tUser == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: ParseUserSQL::'new' failed for tUser record.\n");
    return FALSE;
  }
  memset(tUser,0x0,sizeof(user_struct));
  strcpy(tUser->sUserName,pRow[0]);
  strcpy(tUser->sPassword,pRow[1]);
  tUser->iAccess = atoi(pRow[2]);
  tUser->iIndex = m_iUserIndex++;
  m_pUserList->AddLink(tUser);
  
  if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf( "[ADMIN] DEBUG: User loaded: Name '%s', Password '%s', Access '%i'\n",tUser->sUserName, tUser->sPassword, tUser->iAccess);
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
  if (m_pUserList == NULL) 
    m_pUserList = new CLinkList<user_struct>();
  
  // Load the users
#ifdef USE_MYSQL
  if (!LoadTable("user", m_pUserList, "mysql_dbtable_users", "SELECT nick,pass,access FROM %s\n", &ParseUserSQL))
#endif
    LoadFile("user", m_pUserList, "users_file", &ParseUser);
}

// Sets the user password.  If sSetPassword is NULL, this will attempt to read it
// (and then clear it) from the player's setinfo buffer.
void SetUserPassword(const char* sName, char* sSetPassword, edict_t* pEntity) {
  int iIndex = ENTINDEX(pEntity);
  int iSetPassword = 0;
  char sPassword[PASSWORD_SIZE];
  char* sPasswordField = (char*)CVAR_GET_STRING("password_field");
  char* infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity);
  char szCommand[128];
  
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: SetUserPassword: User '%s' has out of bounds entity index %i\n", sName, iIndex);
    return;
  }
  
  strncpy(g_AuthArray[iIndex].sUserName, sName, USERNAME_SIZE);
  memset(sPassword,0x0,PASSWORD_SIZE);
  // If we're passed a password, use that.  Otherwise, grab it
  // from their setinfo key, if possible.
  if (sSetPassword != NULL) {
    strncpy(sPassword, sSetPassword, PASSWORD_SIZE);
    iSetPassword = 1;
  } else if (sPasswordField != NULL && !FStrEq(sPasswordField,"0")) {
    strncpy(sPassword, g_engfuncs.pfnInfoKeyValue(infobuffer,sPasswordField), PASSWORD_SIZE);
    // If we got a password from their setinfo buffer, clear it.
    if (sPassword != NULL && !FStrEq(sPassword,"")) {
#ifdef HAS_SNPRINTF
      snprintf(szCommand,128,"setinfo %s \"\"\n",sPasswordField);
#else
      sprintf(szCommand,"setinfo %s \"\"\n",sPasswordField);
#endif
      CLIENT_COMMAND(pEntity,szCommand);
      iSetPassword = 1;
    }
  }
  // If we changed their password, reset their auth access (need to call
  // VerifyUserAuth again).
  if (iSetPassword == 1) {
    strncpy(g_AuthArray[iIndex].sPassword, sPassword, PASSWORD_SIZE);
    System_Response("[ADMIN] Password received.\n", pEntity);
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: SetUserPassword: User '%s' entered password '%s'\n",sName,sPassword);
    }
  }
}

void UpdateUserAuth(edict_t* pEntity) {
  int iIndex = ENTINDEX(pEntity);
  
  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: UpdateUserAuth: User '%s' has out of bounds entity index %i\n", pEntity->v.netname, iIndex);
    return;
  }
  g_AuthArray[iIndex].iTime = time(NULL);
}

// Attempts to find a matching user record for this player.  
BOOL VerifyUserAuth(const char* sName, edict_t* pEntity) {
  BOOL fResult = FALSE;
  int iIndex = ENTINDEX(pEntity);
  user_struct tUser;
  
  // Make sure we have a valid index.
  if (iIndex < 1 || iIndex > gpGlobals->maxClients) {
    UTIL_LogPrintf("[ADMIN] ERROR: VerifyUserAuth: User '%s' has out of bounds entity index %i\n", sName, iIndex);
    return FALSE;
  }
  // Try to get a record that matches this player's name or WON ID, and their password.
  if (GetUserRecord(sName, GETPLAYERWONID(pEntity),g_AuthArray[iIndex].sPassword,&tUser)) {
    // We got one.  Note down it's access.
    g_AuthArray[iIndex].iAccess = tUser.iAccess;
    g_AuthArray[iIndex].iUserIndex = tUser.iIndex;
    System_Response(UTIL_VarArgs("[ADMIN] Password accepted for user '%s'\n",tUser.sUserName), pEntity);
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: VerifyUserAuth: User '%s' matched correct password for entry '%s'\n",sName,tUser.sUserName);
    }
    fResult = TRUE;
    // Otherwise, reset their access
  } else {
    g_AuthArray[iIndex].iAccess = (int)CVAR_GET_FLOAT("default_access");
    g_AuthArray[iIndex].iUserIndex = 0;
    System_Response(UTIL_VarArgs("[ADMIN] Password rejected for user '%s'\n",tUser.sUserName), pEntity);
    if ((int)CVAR_GET_FLOAT("admin_debug")!=0) {
      UTIL_LogPrintf("[ADMIN] DEBUG: VerifyUserAuth: User '%s' did not match correct password for any entry.\n",sName);
    }
    fResult = FALSE;
  }
  g_AuthArray[iIndex].iTime = time(NULL);
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
  spawn_struct* tSpawn = NULL;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  int iIdentity;
  
  // Make a new link
  tSpawn = new spawn_struct;
  // Make sure the link is a valid address
  if(tSpawn == NULL) {
    UTIL_LogPrintf("[ADMIN] AddSpawnEntity: 'new' failed for tSpawn record.\n");
    return 0;
  }
  
  // Initialize the link's data
  memset(tSpawn, 0x0, sizeof(spawn_struct));
  iIdentity = m_iSpawnIdentity++;
  strcpy(tSpawn->szClassname, szClassname);
  tSpawn->iIdentity = iIdentity;
  tSpawn->pEntity = pEntity;
  m_pSpawnList->AddLink(tSpawn);
  return iIdentity;
}

// Find a spawn record, given a spawn's identity.
// May return NULL for no matching id.
spawn_struct* FindSpawnEntity(int iIdentity) {
  spawn_struct* tSpawn = NULL;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  
  while(pLink != NULL) {
    // For each link on the list, check to see if the identities match.
    // Check the classname only if the identity we're looking for is 0.
    tSpawn = pLink->Data();
    if (iIdentity == tSpawn->iIdentity) {
      break;
    } else {
      pLink = pLink->NextLink();
    }
  }
  return (pLink == NULL ? NULL : tSpawn);
}

// Initializes the linked list.  DOES NOT REMOVE THE ENTITIES.
void InitSpawnEntityList() {
  if (m_pSpawnList == NULL) {
    m_pSpawnList = new CLinkList<spawn_struct>();
  } else {
    m_pSpawnList->Init();
  }
}

// Lists the contents of the spawn linked list that match
// szFindClassname (or all, if szFindClassname is NULL or empty)
void ListSpawnEntities(edict_t* pMsg, char* szFindClassname) {
  int iIdentity;
  int iLength = strlen(szFindClassname);
  char szClassname[BUF_SIZE];
  spawn_struct* tSpawn = NULL;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  
  System_Response(UTIL_VarArgs("Identity            ClassName         \n"), pMsg);
  while(pLink != NULL) {
    tSpawn = pLink->Data();
    iIdentity = tSpawn->iIdentity;
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
  spawn_struct* tSpawn = NULL;
  CLinkItem<spawn_struct>* pLink = m_pSpawnList->FirstLink();
  
  // Search through the list, like in FindSpawnEntity.
  while(pLink != NULL) {
    tSpawn = pLink->Data();
    if (iIdentity == tSpawn->iIdentity) {
      break;
    } else {
      pLink = pLink->NextLink();
    }
  }
  
  // If we found nothing, we can't remove anything.
  if(pLink == NULL) {
    return FALSE;
  } else {
    // Remove the entity and free the link.
    REMOVE_ENTITY(tSpawn->pEntity->edict());
    delete(tSpawn);
    
    m_pSpawnList->DeleteLink(pLink);
    return TRUE;
  }
}

/***************************
 *
 * Plugin functions
 *
 ***************************/
// Adds an entry to the help linked list.  Returns TRUE
// if successful, FALSE otherwise.
BOOL AddHelpEntry(char* sCmd, char* sHelp, int iAccess) {
  int iCompare;
  help_struct* tHelp;
  CLinkItem<help_struct>* pLink;
  CLinkItem<help_struct>* pOldLink = NULL;
  
  // Make sure our help list is initialized
  if (m_pHelpList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: AddHelpEntry called when help list not initialized.\n");
    return FALSE;
  }
  // Compare this entry to the entries we currently have; we want to
  // avoid duplicates.
  pLink = m_pHelpList->FirstLink();
  while (pLink != NULL) {
    tHelp = pLink->Data();
    iCompare = stricmp(sCmd, tHelp->sCmd);
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
  // (or NULL, if we're putting it at the end.)  Let's add it.
  tHelp = new help_struct;
  if(tHelp == NULL) {
    UTIL_LogPrintf( "[ADMIN] ERROR: AddHelpEntry::'new' failed for tHelp record.\n");
    return FALSE;
  }
  strcpy(tHelp->sCmd, sCmd);
  strcpy(tHelp->sHelp, sHelp);
  tHelp->iAccess = iAccess;
  m_pHelpList->AddLink(tHelp, pLink);
  return TRUE;
}

// Finds a CPlugin object based on an AMX pointer.
CPlugin* GetPlugin(AMX* amx) {
  CLinkItem<CPlugin>* pLink;
  CPlugin* pPlugin;
  
  // Make sure our list is initialized.
  if (m_pPluginList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetPlugin called when plugin list not initialized.\n");
    return NULL;
  }
  // For each plugin, compare the AMX pointers.
  pLink = m_pPluginList->FirstLink();
  while (pLink != NULL) {
    pPlugin = pLink->Data();
    // We got a match?  Cool.
    if (amx == pPlugin->amx()) {
      break;
    }
    pLink = pLink->NextLink();
  }
  
  // No match?  Bummer.
  if (pLink == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: GetPlugin failed.\n");
    return NULL;
  }
  return pPlugin;
}

// For each plugin, call it's HandleCommand method.  Break if
// any of them return PLUGIN_HANDLED.
plugin_result HandleCommand(edict_t* pEntity, char* sCmd, char* sData) {
  plugin_result iResult = PLUGIN_CONTINUE;
  CLinkItem<CPlugin>* pLink;
  CPlugin* pPlugin;
  
  if (m_pPluginList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleCommand called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  pLink = m_pPluginList->FirstLink();
  while (pLink != NULL) {
    pPlugin = pLink->Data();
    iResult = pPlugin->HandleCommand(pEntity, sCmd, sData);
    if (iResult == PLUGIN_HANDLED) {
      break;
    }
    pLink = pLink->NextLink();
  }
  return iResult;
}

// For each plugin, call it's HandleConnect method.  Break if
// any of them return PLUGIN_HANDLED.
plugin_result HandleConnect(edict_t* pEntity, char* sName, char* sIPAddress) {
  plugin_result iResult = PLUGIN_CONTINUE;
  CLinkItem<CPlugin>* pLink;
  CPlugin* pPlugin;
  
  if (m_pPluginList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleConnect called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  pLink = m_pPluginList->FirstLink();
  while (pLink != NULL) {
    pPlugin = pLink->Data();
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
  CLinkItem<CPlugin>* pLink;
  CPlugin* pPlugin;
  
  if (m_pPluginList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleDisconnect called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  pLink = m_pPluginList->FirstLink();
  while (pLink != NULL) {
    pPlugin = pLink->Data();
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
  int iAccess;
  int iCount = 0;
  int iLength = 10;
  int iMaxCount = 0;
  int iStart = 1;
  char sDelimiter = ' ';
  char* sColon;
  char sCommand[COMMAND_SIZE];
  char* sFilter = NULL;
  char sFilterText[BUF_SIZE];
  char sHelp[BUF_SIZE];
  char sParam[BUF_SIZE];
  char* sSpace;
  char* sToken = NULL;
  CLinkItem<help_struct>* pLink;
  help_struct* tHelp;

  // Verify our list is initialized.
  if (m_pHelpList == NULL) {
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
  iAccess = GetUserAccess(pEntity);
  // Tokenize the data.  This is kludgy, but the data format is:
  // [<search string>] [<start> [<length>]] 
  // If the first token is alphabetic, it's search string...otherwise, it's start.
  // If there's a second token, it has to be numeric; if the first was alphabetic,
  // then the second is start.  Otherwise, the second is length.
  // If there's a third token, it's used only if the first was alphabetic.  In that
  // case, it's length. 
  // Confused?
  if (pcData != NULL) {
    sToken = strtok(pcData, &sDelimiter);
    if (sToken != NULL) {
      // If the first token is alphabetic, it's the search string.
      if (atoi(sToken) == 0 && *sToken != '0') {
	sFilter = sToken;
	// Get the second token.
	sToken = strtok(NULL, &sDelimiter);
      }
      if (sToken != NULL) {
	// Otherwise, the first (or second) token is start.
	iStart = atoi(sToken);
	if (iStart < 1)
	  iStart = 1;
	sToken = strtok(NULL, &sDelimiter);
	if (sToken != NULL) {
	  // And the second (or third) token is length.
	  iLength = atoi(sToken);
	  if (iLength < 1)
	    iLength = 1;
	}
      }
    }
  }
  
  if (sFilter == NULL || strlen(sFilter) == 0) {
    strcpy(sFilterText, "");
  } else {
    sprintf(sFilterText, " for '%s'", sFilter);
  }
  
  if (iFormat == 1) {
    System_Response(UTIL_VarArgs("----- Start -----\n"),pEntity);
  }
  
  // For each help entry...
  pLink = m_pHelpList->FirstLink();
  while (pLink != NULL) {
    tHelp = pLink->Data();
    // Make sure that they have access to this entry, and it matches the filter they give
    // (or they didn't give a filter).
    if (((iAccess & tHelp->iAccess) == tHelp->iAccess)
	&& (sFilter == NULL || stristr(tHelp->sCmd, sFilter) != NULL || stristr(tHelp->sHelp, sFilter) != NULL )) {
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
	    sColon = strchr(tHelp->sHelp, ':');
	    sSpace = strchr(tHelp->sHelp, ' ');
	    if (sColon == NULL) {
	      strcpy(sCommand, "Unknown/Bad Format");
	      strcpy(sParam, "");
	      strcpy(sHelp, tHelp->sHelp);
	    } else if (sSpace == NULL || sSpace > sColon) {
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
	if (sFilter == NULL) {
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
  CLinkItem<CPlugin>* pLink;
  CPlugin* pPlugin;
  
  if (m_pPluginList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleInfo called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }
  pLink = m_pPluginList->FirstLink();
  while (pLink != NULL) {
    pPlugin = pLink->Data();
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
  CLinkItem<CPlugin>* pLink;
  CPlugin* pPlugin;
  
	// Unlike the other procs, this one will be called before initialization. 
	// So don't display an error.
  if (m_pPluginList == NULL) {
    return iResult;
  }
  pLink = m_pPluginList->FirstLink();
  while (pLink != NULL) {
    pPlugin = pLink->Data();
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
  CLinkItem<CPlugin>* pLink;
  CPlugin* pPlugin;
  
  // Make sure our plugin data is initialized.
  if (m_pPluginList == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: HandleVersion called when plugin list not initialized.\n");
    return PLUGIN_ERROR;
  }

	System_Response(UTIL_VarArgs( "Admin Mod version %s\n",MOD_VERSION), pEntity);
  // For each plugin...
  pLink = m_pPluginList->FirstLink();
  while (pLink != NULL) {
    pPlugin = pLink->Data();
    strcpy(sName, pPlugin->Name());
    if (sName[0] == 0) {
      strcpy(sName, pPlugin->File());
    }
    strcpy(sDesc, pPlugin->Desc());
    strcpy(sVersion, pPlugin->Version());
    System_Response(UTIL_VarArgs( "* Plugin #%2i: %s (v. %s)\n",i++, sName, sVersion) ,pEntity);
    if (sDesc != NULL && strlen(sDesc) > 0)
      System_Response(UTIL_VarArgs("  %s \n", sDesc), pEntity);
    pLink = pLink->NextLink();
  }
  return PLUGIN_HANDLED;
}

// Loads a plugin from a line in a file.  Returns TRUE if successful,
// FALSE otherwise.
BOOL ParsePlugin(char* sLine) {
  char sGameDir[2048];
  char sPlugin[LINE_SIZE];
  CPlugin* pPlugin;
  
  (*g_engfuncs.pfnGetGameDir)(sGameDir);
  memset(sPlugin,0x0,LINE_SIZE);
  // Get the plugin, relative to the game mod's dlls dir.
  sprintf(sPlugin,"%s/%s",sGameDir,sLine);
  FormatPath(sPlugin);
  if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
    UTIL_LogPrintf( "[ADMIN] Found plugin '%s'\n",sPlugin);
  }
  pPlugin = new CPlugin();
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

// Loads the plugins.  Returns TRUE if at least one plugin is loaded successfully,
// FALSE otherwise.
BOOL LoadPlugins(void) {
  // Debug: Print out the game dir
  if((int)CVAR_GET_FLOAT("admin_debug")!=0) {
    char sGameDir[2048];
    (*g_engfuncs.pfnGetGameDir)(sGameDir);
		FormatPath(sGameDir);
    UTIL_LogPrintf("[ADMIN] LoadPlugins: Game dir is '%s'\n", sGameDir);
  }
  
  // Create a new linked list, if necessary.
  if (m_pPluginList == NULL) 
    m_pPluginList = new CLinkList<CPlugin>();
  
  // Init the help linked list
  if (m_pHelpList == NULL) 
    m_pHelpList = new CLinkList<help_struct>();
  m_pHelpList->Init();
  
  // Load the plugins
#ifdef USE_MYSQL
  if (!LoadTable("plugin", m_pPluginList, "mysql_dbtable_plugins", "SELECT plugin FROM %s\n", &ParsePluginSQL))
#endif
    LoadFile("plugin", m_pPluginList, "admin_plugin_file", &ParsePlugin);
  
  return ((m_pPluginList->Count() > 0) ? TRUE : FALSE);
}

/***************************
 *
 * Vault functions
 *
 ***************************/
vault_struct* FindVaultData(char* sKey) {
  CLinkItem<vault_struct>* pLink;
  vault_struct* tVault;
  
  // Make sure the vault is loaded
  if (g_fVaultLoaded == FALSE)
    LoadVault();
  
  if (m_pVaultList == NULL) 
    return NULL;
  
  pLink = m_pVaultList->FirstLink();
  while (pLink != NULL) {
    tVault = pLink->Data();
    if (!stricmp(sKey, tVault->sKey)) {
      return tVault;
    }
    pLink = pLink->NextLink();
  }
  return NULL;
}

char* GetVaultData(char* sKey) {
  vault_struct* tVault;
  
  tVault = FindVaultData(sKey);
  if (tVault == NULL) {
    return NULL;
  } else {
    return tVault->sData;
  }
}

// Loads a vault record from line in a file.  Returns TRUE if successful, 
// FALSE otherwise. The format is:
// <key> <data>
BOOL ParseVault(char* sLine) {
  char sDelimiter[] = " ";
  char* sKeyToken = 0;
  char* sDataToken = 0;
  vault_struct* tVault;
  int iLineLen = strlen( sLine );

  sKeyToken = strtok(sLine,sDelimiter);
  if (sKeyToken == NULL) {
    UTIL_LogPrintf("[ADMIN] ERROR: No vault key found: '%s'\n", sLine);
  } else if ((int)strlen(sKeyToken) > BUF_SIZE) {
    UTIL_LogPrintf("[ADMIN] ERROR: Vault key too long: '%s'\n", sKeyToken);
  } else {

    //sDataToken = strtok(NULL,sDelimiter);
    int iKeyLen = strlen( sKeyToken );
    if ( iLineLen > iKeyLen ) {
      sDataToken = sLine + iKeyLen + 1;
      // skip leading whitespace
      sDataToken += strspn( sDataToken, " \t" );
    }  // if
    if (sDataToken == NULL) {
      UTIL_LogPrintf("[ADMIN] ERROR: No vault data found: '%s'\n", sLine);
    } else if ((int)strlen(sDataToken) > BUF_SIZE) {
      UTIL_LogPrintf("[ADMIN] ERROR: Vault data too long: '%s'\n", sDataToken);
    } else {
      tVault = new vault_struct;
      if(tVault == NULL) {
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
  if (m_pVaultList == NULL) 
    m_pVaultList = new CLinkList<vault_struct>();
  
  // Load the vault
  LoadFile("vault record", m_pVaultList, "admin_vault_file", &ParseVault);
}

void SaveVault() {
  char* sFile;
  char sGameDir[2048];
  char sVaultFile[LINE_SIZE];
  FILE *pFile;
  CLinkItem<vault_struct>* pLink;
  vault_struct* tVault;
  
  if (m_pVaultList == NULL || !g_fVaultLoaded) 
    return;
  
  sFile = (char*)CVAR_GET_STRING("admin_vault_file");
  if (sFile == NULL || strlen(sFile) == 0 || FStrEq(sFile,"0")) 
    return;
  
  (*g_engfuncs.pfnGetGameDir)(sGameDir);
  sprintf(sVaultFile,"%s/%s",sGameDir,sFile);
  FormatPath(sVaultFile);
  
  pFile = fopen(sVaultFile,"w");
  if (pFile == NULL) 
    return;
  
  pLink = m_pVaultList->FirstLink();
  while (pLink != NULL) {
    tVault = pLink->Data();
    fprintf(pFile,"%s %s\n", tVault->sKey, tVault->sData);
    pLink = pLink->NextLink();
  }
  fclose(pFile);
}

void SetVaultData(char* sKey, char* sData) {
  CLinkItem<vault_struct>* pLink;
  vault_struct* tVault;
  
  tVault = FindVaultData(sKey);
  if (tVault == NULL) {
    tVault = new vault_struct;
    if(tVault == NULL) {
      UTIL_LogPrintf( "[ADMIN] ERROR: SetVaultData::'new' failed for tVault record.\n");
      return;
    }
    strcpy(tVault->sKey, sKey);
    strcpy(tVault->sData, sData);
    m_pVaultList->AddLink(tVault);
  } else {
    if (sData == NULL || strlen(sData) == 0) {
      pLink = m_pVaultList->FindLink(tVault);
      if (pLink != NULL) 
	m_pVaultList->DeleteLink(pLink);
    } else {
      strcpy(tVault->sData, sData);
    }
  }
  SaveVault();
}
