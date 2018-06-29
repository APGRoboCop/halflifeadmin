/*
 * ===========================================================================
 *
 * $Id: util.cpp,v 1.12 2004/08/19 22:23:01 the_wraith Exp $
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
 * This file implements some helper functions
 *
 */   

#ifdef USE_MYSQL
  #include <mysql.h>
  #include <errmsg.h>

#  include "extdll.h"
  extern MYSQL mysql;
  extern BOOL g_fUseMySQL;
#else
#ifdef USE_PGSQL
  #include <libpq-fe.h>

#  include "extdll.h"
  extern PGconn *pgsql;
  extern BOOL g_fUsePgSQL;
#else
#  include "extdll.h"
#endif
#endif

#include <ctype.h> /* for isprint */
#include "constants.h"
#include "amutil.h"
#include "amlibc.h"
#include "authid.h"
#include "users.h"

#define SDK_UTIL_H
#include "meta_api.h"

extern globalvars_t   *gpGlobals;
extern enginefuncs_t g_engfuncs;
extern mapcycle_t mapcycle;

DLL_GLOBAL const Vector     g_vecZero = Vector(0,0,0);   





/* Like System_Response, but will _always_ log. */
void System_Error(char* str, edict_t* pEntity) {
  UTIL_LogPrintf( "%s", str);
  if (pEntity != NULL) {
    if ((int)CVAR_GET_FLOAT("admin_debug") != 0) {
      CLIENT_PRINTF(pEntity, print_chat, str);
    } else {
      CLIENT_PRINTF(pEntity, print_console, str);
    }
  }
}

/*
 *  a generic way to print a msg to a user, whether they are on the
 *  console or on a client
 *
 *
 */
void System_Response(char *str,edict_t *pAdminEnt) {
  
  if(pAdminEnt==NULL) {
    UTIL_LogPrintf( "%s", str);
  } else {
    CLIENT_PRINTF( pAdminEnt, print_console,str);
  }
  
}


/*
  stop malformed names stuffing up checking
*/
int make_friendly(char *name, BOOL check) {
	int i; 
	int iLen = strlen(name);
	
	
	if(check && (g_NameCrashAction > 0 ) && ( iLen<=0 )  ) return 2; // the name is zero length....  
	
	for(i=0; i < iLen; i++) {
		if (name[i]=='%') {
			// replace '%' with ' ' like the engine
			name[i] = ' ';
		}
		// since we go through the string here anyway, we may just as well check for 
		// vicious characters
		if ( check && (g_NameCrashAction > 0 ) && !isprint(name[i]) ) return 2;
	}
	return 1;
}


/*
 *  escape a set of characters from a string by adding a backslash in front of them.
 *  returns a const pointer to a static string
 *
 * prototype in amutil.h
 *
 */
const char* escape_chars( const char* _pcString, const char* _pcChars ) {
	static char* pcResultString = 0;
	int iLength = strlen( _pcString );
	int iNumChars = strlen( _pcChars );
	int iNumEscapes = 0;

	// TODO: add a check for unprintable characters here

	// count the number of characters to escape.
	const char* pcPos = _pcString;
	while ( (pcPos = strpbrk(pcPos,_pcChars)) != 0 ) {
		iNumEscapes++;
		pcPos++;
	}  // while

	if ( iNumEscapes == 0 ) {
		// no characters found to be escaped
		return _pcString;
	}  // if

	if ( pcResultString != 0 ) delete[] pcResultString;
	pcResultString = new char[iLength + iNumEscapes + 1];
	if ( pcResultString == 0 ) return 0;

	// copy the string into our new string, escaping charaters
	char* pcSet = pcResultString;
	const char* pcCopy = _pcString;
	pcPos = _pcString;
	while( (pcPos = strpbrk(pcPos, _pcChars)) != 0 ) {
		while( pcCopy != pcPos ) {
			*pcSet = *pcCopy;
			pcSet++; pcCopy++;
		}  // while
		*pcSet = '\\';
		pcSet++;
		*pcSet = *pcCopy;
		pcSet++; pcCopy++;
		pcPos++;
	}  // while

	// copy the rest of the string
	strcpy( pcSet, pcCopy );
	pcResultString[iLength + iNumEscapes] = 0;

	return pcResultString;
}  // escape_chars()


#if 0
// create a message from a template. Possible placeholders in the template are
// %w: WONID of player entity 
// %n: nickname of player entity
int create_message( char* _pcTarget, size_t _iMaxLen, const char* _pcFormat, edict_t* _ptediEntity ) {
	char* pcPos;
	char* pcFormat;
	pcFormat = new char[strlen(_pcFormat)+1];
	if ( pcFormat == 0 ) return 0; // failed
	memcpy( pcFormat, _pcFormat, strlen(_pcFormat)+1 );
	pcPos = pcFormat;
	while ( (pcPos = strchr(pcPos, '%')) != 0 ) {
		pcPos++;
		switch( *pcPos ) {
		case '%':
			// move remaining string one to the left to delete double %
			memmove( pcPos-1, pcPos, strlen(pcPos)+1 );
			break;
		case 'w':
			// substitute with WONID
		}  // switch
	}  // while 
	
	return 1;
}  // create_message()
#endif



// Option cvars are string cvars which can set multiple options.
// Options are seperated by a colon(:), e.g. "nc1:sp0".
// Option format is an string identifier which is 
// a) directly followed by a number if it is a numeric option value: nc0
// b) followed by a period and then a string if it is a string option value: ssl off
//
// If the _pcStringReturn is not 0 then the option value will be written to the string passed. 
// The return value is the numeric value of the option or 0 if it is only a string

long int get_option_cvar_value( const char* _pcCvarName, const char* _pcOption, long int _liNumDefault, char* _pcReturnString, size_t _tsLen, const char* _pcStringDefault ) {


	long int retval = _liNumDefault;
	size_t iOptionIDLen = strlen( _pcOption );

	// set if the option was found
	bool bOptionFound = false;
	// get the CVAR string
	const char* pcOptions = CVAR_GET_STRING( _pcCvarName );
	// find the option identifier
	const char* pcOpt = pcOptions;

	while ( (pcOpt=strstr(pcOpt, _pcOption)) != 0 ) {  // if the identifier string was found ...
		if ( pcOpt == pcOptions || *(pcOpt-1) == ':' ) {  // ... and it is indeed the start of an identifier ...
			if ( (*(pcOpt+iOptionIDLen) == ' ' && *(pcOpt += iOptionIDLen + 1))
				 || ((*(pcOpt+iOptionIDLen)>=0x30 && *(pcOpt+iOptionIDLen)<=0x39)  && *(pcOpt += iOptionIDLen)) ) {  
				// ... and it is not only a substring but a complete identifier

				bOptionFound = true;

				// get the numeric value
				retval = strtol( pcOpt, 0, 0 );

				if ( _pcReturnString != 0 ) {  
					// if we should return the string value, copy it
					while ( (*pcOpt != ':') && (*pcOpt != '\0') && _tsLen ) {
						*_pcReturnString = *pcOpt;
						++pcOpt; ++_pcReturnString; --_tsLen;
					}  // while
				}  // if

				break;
			}  // if
		}  // if
		++pcOpt;
	}  // while

	// in case the option was not found and we have a default string, copy it over
	if ( !bOptionFound && _pcStringDefault != NULL ) {
		am_strncpy( _pcReturnString, _pcStringDefault, _tsLen );
	}  // if

	return retval;
}  // get_option_cvar_value()




void ShowMenu (edict_t* pev, int bitsValidSlots, int nDisplayTime, BOOL fNeedMore, char pszText[1024]) {

  int msgShowMenu = 0;
  if ( (msgShowMenu = GET_USER_MSG_ID(PLID, "ShowMenu", 0)) == 0 ) {
	  msgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
  }  // if

  MESSAGE_BEGIN( MSG_ONE, msgShowMenu, NULL, pev);
  
  WRITE_SHORT( bitsValidSlots);
  WRITE_CHAR( nDisplayTime );
  WRITE_BYTE( fNeedMore );
  WRITE_STRING (pszText);
  
  MESSAGE_END();  
}




// Send menu in chunks (max. 512 chars for menu and 176 for one chunk)
void ShowMenu_Large (edict_t* pev, int bitsValidSlots, int nDisplayTime, char pszText[]) {
  
  int msgShowMenu = 0;
  if ( (msgShowMenu = GET_USER_MSG_ID(PLID, "ShowMenu", 0)) == 0 ) {
	  msgShowMenu = REG_USER_MSG( "ShowMenu", -1 );
  }  // if

  char *pMenuList = pszText;
  char *aMenuList = pszText;
  
  char szChunk[MAX_MENU_CHUNK+1];
  int iCharCount = 0;
  
  while ( pMenuList && *pMenuList ) 
  {
	  am_strncpy( szChunk, pMenuList, MAX_MENU_CHUNK );

	  iCharCount += strlen( szChunk );//Move text to next chunk

	  pMenuList = aMenuList + iCharCount;

	  MESSAGE_BEGIN( MSG_ONE, msgShowMenu, NULL, pev );	
	  WRITE_SHORT( bitsValidSlots );
	  WRITE_CHAR( nDisplayTime );
	  WRITE_BYTE(*pMenuList ? TRUE : FALSE); //Need more menu?
	  WRITE_STRING(szChunk);
	  MESSAGE_END();
  }
}




void ShowMOTD( edict_t* pev, const char *msg )
{
  int char_count = 0;

  int msgShowMOTD = 0;
  if ( (msgShowMOTD = GET_USER_MSG_ID(PLID, "MOTD", 0)) == 0 ) {
	  msgShowMOTD = REG_USER_MSG( "MOTD", -1 );
  }  // if

  while( msg && *msg && char_count < MAX_MOTD_LENGTH )
  { 
    char chunk[MAX_MOTD_CHUNK+1];

    if( strlen( msg ) < MAX_MOTD_CHUNK )
    {
	strcpy( chunk, msg );
    }
    else
    {
        strncpy( chunk, msg, MAX_MOTD_CHUNK);
        chunk[ MAX_MOTD_CHUNK ] = 0;
    }
  
    char_count += strlen(chunk);

    if ( char_count < MAX_MOTD_LENGTH && char_count < strlen(msg) )
	msg += strlen(chunk);
    else
	msg = NULL;

    MESSAGE_BEGIN( MSG_ONE, msgShowMOTD, NULL, pev );
	WRITE_BYTE( msg ? FALSE : TRUE );  //FALSE means there is still more message to come
	WRITE_STRING( chunk );
    MESSAGE_END();


  }

}





edict_t* UTIL_EntityByIndex( int playerIndex ){
	edict_t* pPlayerEdict = NULL;

	if ( playerIndex > 0 && playerIndex <= gpGlobals->maxClients ) {
		pPlayerEdict = INDEXENT( playerIndex );
		if ( !pPlayerEdict || pPlayerEdict->free ) {
			pPlayerEdict = NULL;
		}
	}
	return pPlayerEdict;
}      



CBaseEntity* UTIL_PlayerByIndex( int playerIndex ){
  CBaseEntity* pPlayer = NULL;
  
  if ( playerIndex > 0 && playerIndex <= gpGlobals->maxClients ) {
    edict_t* pPlayerEdict = INDEXENT( playerIndex );
    if ( pPlayerEdict && !pPlayerEdict->free ) {
      pPlayer = CBaseEntity::Instance( pPlayerEdict );
    }
  }
  return pPlayer;
}      


CBaseEntity* UTIL_PlayerByName( const char *name ) {
  int i;
  CBaseEntity *pPlayer = NULL;
  for ( i = 1; i <= gpGlobals->maxClients; i++ ) {
    pPlayer = UTIL_PlayerByIndex(i);
    if (pPlayer) {
      if(FStrEq(STRING(pPlayer->pev->netname),name))
	return pPlayer;
    }
  }
  return NULL;
}


char* UTIL_VarArgs( char *format, ... ) {
  va_list         argptr;
  static char             acString[1024];
  
  va_start (argptr, format);
  vsnprintf (acString, sizeof(acString), format,argptr);
  va_end (argptr);
  
  return acString;  
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( char *fmt, ... ) {
  va_list                 argptr;
  static char             acString[1024];
  
  va_start ( argptr, fmt );
  vsnprintf ( acString, sizeof(acString), fmt, argptr );
  va_end   ( argptr );
  
  // Print to server console
  ALERT( at_logged, "%s", acString );
}

//=========================================================
// UTIL_LogPrintfFNL - Prints a logged message to console but filter out newlines.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintfFNL( char *fmt, ... ) {
  va_list                 argptr;
  static char             acString[1024];
  
  va_start ( argptr, fmt );
  vsnprintf ( acString, sizeof(acString), fmt, argptr );
  va_end   ( argptr );
  
  int iNumChar = strlen(acString);
  char* pcBufPos = acString;
  while ( (pcBufPos = strchr(pcBufPos, '\n')) ) {
    if ( *(pcBufPos + 1) == '\0' ) break;
    if ( 1023-iNumChar ) {
      memmove( pcBufPos+1, pcBufPos, (iNumChar-(pcBufPos-acString)+1) );
      pcBufPos[0] = '\\';
      pcBufPos[1] = 'n';
      iNumChar++;
    } else {
      *pcBufPos = ' ';
    }  // if-else
  }  // while
    
  // Print to server console
  ALERT( at_logged, "%s", acString );
}


//  commented out in favour of ClientPrint loop over all players
//  until Valve has fixed the bot problems -- Removed comments to check if it works on HLSDK 2.3-p4 - [APG]RoboCop[CL]
void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 ) {
  g_msgTextMsg = REG_USER_MSG( "TextMsg", -1 ); 
  
  MESSAGE_BEGIN( MSG_ALL, g_msgTextMsg );
  WRITE_BYTE( msg_dest );
  WRITE_STRING( msg_name );
  
  if ( param1 )
    WRITE_STRING( param1 );
  if ( param2 )
    WRITE_STRING( param2 );
  if ( param3 )
    WRITE_STRING( param3 );
  if ( param4 )
    WRITE_STRING( param4 );
  
  MESSAGE_END();
}

CBaseEntity *UTIL_FindEntityByString( CBaseEntity *pStartEntity, const
				      char *szKeyword, const char *szValue )
{
  edict_t *pentEntity;
  
  if (pStartEntity)
    pentEntity = pStartEntity->edict();
  else
    pentEntity = NULL;
  
  pentEntity = FIND_ENTITY_BY_STRING( pentEntity, szKeyword, szValue
				      );
  
  if (!FNullEnt(pentEntity))
    return CBaseEntity::Instance(pentEntity);
  return NULL;
}                        

CBaseEntity *UTIL_FindEntityByClassname( CBaseEntity *pStartEntity, const
					 char *szName )
{
  return UTIL_FindEntityByString( pStartEntity, "classname", szName);    
}      

void fix_string(char *str,int len)
{
  
  for(int i=0;i<len;i++) {
    if( str[i]=='>' || str[i]=='<' || str[i]=='%' )
      strcpy(str+i,str+i+1);
  }
  
  
}



/*********************************************************************
 * new map stuff
 * 
 *********************************************************************/


/*
  ==============
  DestroyMapCycle
  
  Clean up memory used by mapcycle when switching it
  ==============
*/
void DestroyMapCycle( mapcycle_t *cycle )
{
  mapcycle_item_t *p, *n, *start;
  p = cycle->items;
  if ( p )
    {
      start = p;
      p = p->next;
      while ( p != start )
	{
	  n = p->next;
	  delete p;
	  p = n;
	}
      
      delete cycle->items;
    }
  cycle->items = NULL;
  cycle->next_item = NULL;
}


static char com_token[ 1500 ];
/*
  ==============
  COM_Parse
  
  Parse a token out of a string
  ==============
*/
char *COM_Parse (char *data)
{
  int             c;
  int             len;
  
  len = 0;
  com_token[0] = 0;
  
  if (!data)
    return NULL;
  
  // skip whitespace
 skipwhite:
  while ( (c = *data) <= ' ')
    {
      if (c == 0)
	return NULL;                    // end of file;
      data++;
    }
  
  // skip // comments
  if (c=='/' && data[1] == '/')
    {
      while (*data && *data != '\n')
	data++;
      goto skipwhite;
    }
  
  
  // handle quoted strings specially
  if (c == '\"')
    {
      data++;
      while (1)
	{
	  c = *data++;
	  if (c=='\"' || !c)
	    {
	      com_token[len] = 0;
	      return data;
	    }
	  com_token[len] = c;
	  len++;
	}
    }
  
  // parse single characters
  if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
    {
      com_token[len] = c;
      len++;
      com_token[len] = 0;
      return data+1;
    }
  
  // parse a regular word
  do
    {
      com_token[len] = c;
      data++;
      len++;
      c = *data;
      if (c=='{' || c=='}'|| c==')'|| c=='(' || c=='\'' || c == ',' )
	break;
    } while (c>32);
  
  com_token[len] = 0;
  return data;
}

/*
  ==============
  COM_TokenWaiting
  
  Returns 1 if additional data is waiting to be processed on this line
  ==============
*/
int COM_TokenWaiting( char *buffer )
{
  char *p;
  
  p = buffer;
  while ( *p && *p!='\n')
    {
      if ( !isspace( *p ) || isalnum( *p ) )
	return 1;
      
      p++;
    }
  
  return 0;
}

/*
  ==============
  ReloadMapCycleFile
  
  
  Parses mapcycle.txt file into mapcycle_t structure
  ==============
*/
int ReloadMapCycleFile( char *filename, mapcycle_t *cycle )
{
	char szBuffer[ MAX_RULE_BUFFER ];
	char szMap[ MAP_NAME_LENGTH ];
	int length;
	char *pFileList;
	char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( filename, &length );
	int hasbuffer;
	mapcycle_item_s *item = NULL, *last = NULL;
	
	if (aFileList == NULL || pFileList == NULL ) return 0;
	
	if ( pFileList && length ) {
		// the first map name in the file becomes the default
		while ( 1 ) {
			hasbuffer = 0;
			memset( szBuffer, 0, MAX_RULE_BUFFER );
			
			pFileList = COM_Parse( pFileList );
			if ( strlen( com_token ) <= 0 ) break;
	  
			am_strncpy( szMap, com_token, sizeof(szMap) );
			
			// Any more tokens on this line?
			if ( COM_TokenWaiting( pFileList ) ) {
				pFileList = COM_Parse( pFileList );
				if ( strlen( com_token ) > 0 ) {
					hasbuffer = 1;
					am_strncpy( szBuffer, com_token, sizeof(szMap) );
				}  // if
			}  // if
	  
			// Check map
			if ( IS_MAP_VALID( szMap ) ) {
	     
				item = new mapcycle_item_s;
				// only set when first item
				if ( !last ) cycle->items = item;
				if ( !last ) last = item;
				
				strcpy( item->mapname, szMap );
				item->next = cycle->items;
				last->next = item;
				last = item;
			} else {
				ALERT( at_console, "Skipping %s from mapcycle, not a valid map\n", szMap );
			}  // if-else

		}  // while
      
		FREE_FILE( aFileList );
    }  // if
  
	// Didn't parse anything
	if ( !item ) {
		return 0;
    }  // if
  
	cycle->next_item = cycle->items->next;
  
	return 1;
}

mapcycle_item_s *CurrentMap(mapcycle_t *cycle)
{
  mapcycle_item_s *item;
  for ( item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next )	{
    if (!stricmp(item->mapname , STRING(gpGlobals->mapname)) ) break;
  }
  
  return(item->next);
}


/*********************************************************************************************
 *
 *
 * check_map - check if we are allowed to vote for that map
 *
 *
 **********************************************************************************************/
int allowed_map(char *map) { //is this map in maps.ini ? 1=yes, 0=no
  
  char *mapcfile = const_cast<char*>(get_cvar_file_value( "maps_file" ));
  if ( mapcfile == NULL ) return 0;
  int length;
  char *pFileList;
  char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( mapcfile, &length );
  if ( pFileList && length )
    {
      
      
      // keep pulling mapnames out of the list until we find "map", else return 0
      while ( 1 )
	{
	  while ( *pFileList && isspace( *pFileList ) ) pFileList++; // skip over any whitespace
	  if ( !(*pFileList) )
	    break;
	  
	  char cBuf[32];
	  int ret = sscanf( pFileList, " %32s", cBuf );
	  // Check the map name is valid
	  if ( ret != 1 || *cBuf < 13 )
	    break;
	  
	  if ( FStrEq( cBuf, map ) )
	    {  // we've found our map;  they can do it!
	      return 1;
	    }
	  
	  pFileList += strlen( cBuf );
	}
      
      FREE_FILE( aFileList );
    }
  return 0;
  
}


int listmaps(edict_t *pAdminEnt) {
  mapcycle_item_s *item;
  char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
  DestroyMapCycle( &mapcycle );
  ReloadMapCycleFile( mapcfile, &mapcycle );
  
  for ( item = mapcycle.next_item; item->next!= mapcycle.next_item; item = item->next )	{
    System_Response(UTIL_VarArgs("%s\n",item->mapname),pAdminEnt);
  }
  System_Response(UTIL_VarArgs("%s\n",item->mapname),pAdminEnt);
  
  return 0;
}



int check_map(char *map, int bypass_allowed_map)
{
  
  if ( FStrEq(map,"next_map")) { // next map in the cycle
    
    mapcycle_item_s *item;
    char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
    DestroyMapCycle( &mapcycle );
    ReloadMapCycleFile( mapcfile, &mapcycle );
    item=CurrentMap(&mapcycle);
    strcpy(map,item->mapname);
    return (IS_MAP_VALID(item->mapname));
    
    
  } else { // they have defined the maps.ini file for us
    
    if ( !cvar_file_is_set("maps_file") ) { // no maps.ini file defined, just check it is good
      return (IS_MAP_VALID(map));
    }
    if (!IS_MAP_VALID(map)) return 0; // bad map name

	if (bypass_allowed_map)
		{
		return 1;
		}
	else
		{
		return(allowed_map(map));
		}
    
  } // end of maps.ini else check
  
  
  
  return 0;
  
}



static unsigned short FixedUnsigned16( float value, float scale )
{
  int output;
  
  output = value * scale;
  if ( output < 0 )
    output = 0;
  if ( output > 0xFFFF )
    output = 0xFFFF;
  
  return (unsigned short)output;
}



static short FixedSigned16( float value, float scale )
{
  int output;
  
  output = value * scale;
  
  if ( output > 32767 )
    output = 32767;
  
  if ( output < -32768 )
    output = -32768;
  
  return (short)output;
}


int ClientCheck(CBaseEntity *pPlayer) {
	if ( !pPlayer  
		 || ((int)GETPLAYERWONID(pPlayer->edict()) == 0)
		 //||  (pPlayer->pev->flags&FL_SPECTATOR)
		 //||  (pPlayer->pev->flags&FL_PROXY) 
		 )
		return 0;
	else
		return 1;

}


int ClientCheck(edict_t *pEntity) {
	if ( !pEntity
		 || ((int)GETPLAYERWONID(pEntity) == 0)
		 //||  (pPlayer->pev->flags&FL_SPECTATOR)
		 //||  (pPlayer->pev->flags&FL_PROXY) 
		 )
		return 0;
	else
		return 1;

}


int ClientCheck(entvars_t *pEntVars) {
	edict_t* pEntity = FIND_ENTITY_BY_VARS( pEntVars );
	if ( !pEntity
		 || ((int)GETPLAYERWONID(pEntity) == 0)
		 //||  (pPlayer->pev->flags&FL_SPECTATOR)
		 //||  (pPlayer->pev->flags&FL_PROXY) 
		 )
		return 0;
	else
		return 1;

}


void UTIL_HudMessage( CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage )
{
  
 if( ClientCheck(pEntity) == 0 ) return;

  MESSAGE_BEGIN( MSG_ONE, SVC_TEMPENTITY, NULL, pEntity->edict() );
  WRITE_BYTE( TE_TEXTMESSAGE );
  WRITE_BYTE( textparms.channel & 0xFF );
  
  WRITE_SHORT( FixedSigned16( textparms.x, 1<<13 ) );
  WRITE_SHORT( FixedSigned16( textparms.y, 1<<13 ) );
  WRITE_BYTE( textparms.effect );
  
  WRITE_BYTE( textparms.r1 );
  WRITE_BYTE( textparms.g1 );
  WRITE_BYTE( textparms.b1 );
  WRITE_BYTE( textparms.a1 );
  
  WRITE_BYTE( textparms.r2 );
  WRITE_BYTE( textparms.g2 );
  WRITE_BYTE( textparms.b2 );
  WRITE_BYTE( textparms.a2 );
  
  WRITE_SHORT( FixedUnsigned16( textparms.fadeinTime, 1<<8 ) );
  WRITE_SHORT( FixedUnsigned16( textparms.fadeoutTime, 1<<8 ) );
  WRITE_SHORT( FixedUnsigned16( textparms.holdTime, 1<<8 ) );
  
  if ( textparms.effect == 2 )
    WRITE_SHORT( FixedUnsigned16( textparms.fxTime, 1<<8 ) );
  
  if ( strlen( pMessage ) < 512 )
    {
      WRITE_STRING( pMessage );
    }
  else
    {
      char tmp[512];
      strncpy( tmp, pMessage, 511 );
      tmp[511] = 0;
      WRITE_STRING( tmp );
    }
  MESSAGE_END();
}



void UTIL_HudMessageAll( const hudtextparms_t &textparms, const char *pMessage )
{
  int			i;
  
  for ( i = 1; i <= gpGlobals->maxClients; i++ )
    {
      CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

      if ( pPlayer )
		  UTIL_HudMessage( pPlayer, textparms, pMessage );
    }
}





char* GetModDir() {
  int iPos;
  static char strGameDir[2048];
  
  (*g_engfuncs.pfnGetGameDir)(strGameDir);
  iPos = strlen(strGameDir) - 1;

 if(strchr(strGameDir,'/') )
  {  
	// scan backwards till first directory separator...
  	while ((iPos) && (strGameDir[iPos] != '/') )
    		iPos--;
  
  	if (iPos == 0) {
  	  // Hrmm.  Well, this might be a problem. Return no mod.
  	  return 0;
  	}
  	iPos++;
  	return &strGameDir[iPos];
 }

 return strGameDir;
}
/* /CEM */



/* Rope's stuff */
void ClientPrint( entvars_t *client, int msg_dest, const char *msg_name, 
		  const char *param1, const char *param2, const char *param3, 
		  const char *param4 ) {

	if ( ClientCheck(client) == 0 ) return;
  
  if ( (g_msgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", 0)) == 0 ) {
	  g_msgTextMsg = REG_USER_MSG( "TextMsg", -1 );
  }  // if
 

 
  
  MESSAGE_BEGIN( MSG_ONE, g_msgTextMsg, NULL, client );
  WRITE_BYTE( msg_dest );
  WRITE_STRING( msg_name );

  if ( param1 )
    WRITE_STRING( param1 );

  if ( param2 )
    WRITE_STRING( param2 );

  if ( param3 )
    WRITE_STRING( param3 );

  if ( param4 )
    WRITE_STRING( param4 );

  MESSAGE_END();

}

// Possible crash for using commands and says on certain HL1 Mods like TFC and 3rd Party Mods? [APG]RoboCop[CL]
/*void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 ) {
	
  int			i;
  
  for ( i = 1; i <= gpGlobals->maxClients; i++ )
    {
      CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

      if ( pPlayer )
		  ClientPrint( &(pPlayer->edict()->v), msg_dest, msg_name, param1, param2, param3, param4 );
    }
}*/


void UTIL_ClientPrint_UR( entvars_t *client, int msg_dest, const char *msg_name, 
			  const char *param1, const char *param2, const char *param3, 
			  const char *param4 )
{

	if ( ClientCheck(client) == 0 ) return;

  if ( (g_msgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", 0)) == 0 ) {
	  g_msgTextMsg = REG_USER_MSG( "TextMsg", -1 );
  }  // if

  MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, g_msgTextMsg, NULL, client );
  WRITE_BYTE( msg_dest );
  WRITE_STRING( msg_name );

  if ( param1 )
    WRITE_STRING( param1 );

  if ( param2 )
    WRITE_STRING( param2 );

  if ( param3 )
    WRITE_STRING( param3 );

  if ( param4 )
    WRITE_STRING( param4 );

  MESSAGE_END();

}






//
// Our own versions of ClientPrintf() and ClientCommand() to try
// and make them more secure for bots until Valve fixes the engine
//
void ClientPrintf ( edict_t* pEdict, PRINT_TYPE ptype, const char *szMsg ) {
  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( pEdict && GETPLAYERWONID(pEdict) == 0 ) {
      return; 
    }  // if 
  }  // if

  CLIENT_PRINTF_ENG( pEdict, ptype, szMsg );
}


void ClientCommand (edict_t* pEdict, char* szFmt, ...) {

  if ( ptAM_botProtection && (int)ptAM_botProtection->value == 1 ) {
    if ( pEdict && GETPLAYERWONID(pEdict) == 0 ) {
      return; 
    }  // if 
  }  // if

  va_list                 argptr;
  static char             string[1024];
  
  va_start ( argptr, szFmt );
  vsnprintf ( string, 1024, szFmt, argptr );
  va_end   ( argptr );

  CLIENT_COMMAND_ENG( pEdict, string  );

}



extern DLL_GLOBAL  edict_t *pAdminEnt;




// CEM - Rope's version of my GetPlayerIndex, for partial name matching. 
int GetPlayerIndex(char *PlayerText) {
  int PlayerNumber = 0;
  int i;
  int found = 0;
  int index = 0;
  bool bVerbatim = false;

  // if the string is empty, return failure
  if ( *PlayerText == '\0' ) 
	  return 0;

  // Check if the passed string is to be taken verbatim (starts with a backslash).
  // This means that it will not be interpreted as an authid or as an wonid. 
  // A number will have to be escaped to be interpreted as a name. 

  if ( *PlayerText == '\\' ) {
	  // cut off the first backslash
	  ++PlayerText;
	  if ( ! (*PlayerText == '\\') ) {
		  // the backslash did not escape another backslash. Switch on verbatim mode
		  bVerbatim = true;
	  }  // if
  }  // if
  
  AMAuthId oaiAuthID;
	  
  // Get "numeric" representations of the passed string.
  oaiAuthID = PlayerText;
  //-- Set up a numeric id in case we got a SessionID.
  char* pcEndptr = 0;
  PlayerNumber = strtol( PlayerText, &pcEndptr, 10);
  //-- Check if we had a valid number or a string starting with a number.
  //-- Following whitespaces do not count as a string, ie. "123  " is 123.
  for ( ; *pcEndptr != '\0'; ++pcEndptr ) {
	  if ( *pcEndptr != ' '  && *pcEndptr != '\t' ) {
		  PlayerNumber = 0;
		  break;
	  }  // if
  }  // for


  bool bIsId = (oaiAuthID.is_set() || (PlayerNumber != 0));


  if ( !bVerbatim ) { // Verbatim means a number is a number. Don't match it on a name.
	  for (i = 1; i <= gpGlobals->maxClients; i++) {
		  CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
		  if ( IsPlayerValid(pPlayer) ) {
			  // This is only enabled in verbatim mode. 
			  if ( stricmp(STRING(pPlayer->pev->netname), PlayerText) == 0) { return i;}
			  
			  if ( stristr(STRING(pPlayer->pev->netname), PlayerText) != 0 ) {
				  index = i;
				  found++;
			  }  // if
		  } else {  
			  DEBUG_LOG(5, ("GetPlayerIndex: player from index %d lookup failed.", i) );
		  }  // if-else
	  }  // for
	  
	  if ( found == 1 ) return index;

  }  // if


  if ( bIsId ) {  // We only need to check for Wonid or Sessionid if it actually is an ID. 
	  for (i = 1; i <= gpGlobals->maxClients; i++) {
		  CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
		  if ( IsPlayerValid(pPlayer) ) {
			  if ( oaiAuthID.is_set() && (oaiAuthID == GETPLAYERAUTHID(pPlayer->edict())) ) { return i;}
			  if ( PlayerNumber != 0 && (PlayerNumber == GETPLAYERUSERID(pPlayer->edict())) ) { return i;}
		  } else {  
			  DEBUG_LOG(5, ("GetPlayerIndex: player from index %d lookup failed.", i) );
		  }  // if-else
	  }  // for
  }  // if
  
  if ( found > 1 ) {
	  System_Response(UTIL_VarArgs( "Ambiguous player specification: %s. Be more specific.\n",PlayerText), pAdminEnt);
  }
  // /FZ
  return 0;
}
/* /Rope's stuff */








int GetPlayerCount( edict_t* peIgnorePlayer ) {
	int i;
	int iPlayerCount;
	
	iPlayerCount = 0;
	for ( i = 1; i <= gpGlobals->maxClients; i++ ) {
		edict_t* pPlayer = UTIL_EntityByIndex(i);
		if ( IsPlayerValid(pPlayer) && (pPlayer != peIgnorePlayer) ) {
			iPlayerCount++;
		}  // if
	}  // for
	return iPlayerCount;
}







/* 
 * get_file_path( )
 *
 * Get the path of a filename relative to the game directory. The path is adapted to the OS.
 *
 * pcPath      : string into which the path is copied
 * pcFilename  : name of the file to search. Can contain a relative path.
 * iMaxLen     : maximum path length
 * pcAccessCvar: cvar name that control access to this file operation, eg. file_access_read
 *               If set to NULL no access permissions are checked
 *
 * Returns 1 if path was created, -1 on error or 0 if access denied.
 *
 */
int get_file_path( char* pcPath, char* pcFilename, int iMaxLen, const char* pcAccessCvar ) {
#ifdef WIN32
  const char c_acDirSep[] = "\\";
  const char c_cDirSep = '\\';
#else
  const char c_acDirSep[] = "/";
  const char c_cDirSep = '/';
#endif
  
  if ( pcAccessCvar == 0 || (int)CVAR_GET_FLOAT(pcAccessCvar) == 1 ) {  

    char acFilePath[PATH_MAX];
    memset( acFilePath, 0, PATH_MAX );

    GET_GAME_DIR(acFilePath);

    int iPathLen = strlen( acFilePath );

    /* check if the file path is too long for our buffer */
    iPathLen += 1 + strlen( pcFilename );
    if ( iPathLen >= PATH_MAX || iPathLen >= iMaxLen ) {
      return -1;
    }  // if

    /* everything looks ok, we continue */
    strcat( acFilePath, c_acDirSep );
    strcat( acFilePath, pcFilename );

    char* pcPathSep = acFilePath;
    while ( pcPathSep != 0 && *pcPathSep != '\0' ) {
#ifdef WIN32
      if ( *pcPathSep == '/' ) {
	*pcPathSep = '\\';
      }  // if
#else
      if ( *pcPathSep == '\\' ) {
	*pcPathSep = '/';
      }  // if
#endif
      pcPathSep++;
    }  // while

    pcPathSep = 0;

    strncpy( pcPath, acFilePath, iMaxLen );

    return 1;
  } else {

    return 0;
  }  // if-else

}  // getpath()




int get_player_team( CBaseEntity* poPlayer ) {

	// check for a spectator or the proxy
	
	if ( (poPlayer->pev->flags & FL_PROXY) ) {
		return AM_TEAM_PROXY;
	}  // if

	if ( (poPlayer->pev->flags & FL_SPECTATOR) ) {
		return AM_TEAM_SPECTATOR;
	}  // if


  char* pcMod = GetModDir();

  if ( strcmp(pcMod, "cstrike") == 0 ) {
    
    /* special care for CS */
    char* pcInfobuffer=g_engfuncs.pfnGetInfoKeyBuffer( poPlayer->edict() );
    char acModel[30];

    memset( acModel, 0, 30 );
    strncpy( acModel, g_engfuncs.pfnInfoKeyValue(pcInfobuffer,"model"), 30 );

    if ( !strcmp(acModel, "vip") )
      return 4;

    if ( !strcmp(acModel, "sas") 
	 || !strcmp(acModel, "gsg9" )
	 || !strcmp(acModel, "gign")
	 || !strcmp(acModel, "urban")
	 || !strcmp(acModel, "spetsnaz") ) {
      return 2;
    }  // if

    if ( !strcmp(acModel, "arctic")
	 || !strcmp(acModel, "leet")
	 || !strcmp(acModel, "arab")
	 || !strcmp(acModel, "guerilla")
	 || !strcmp(acModel, "terror")
	 || !strcmp(acModel, "militia")	 ) {
      return 1;
    }  // if

  } else {
    return poPlayer->edict()->v.team;
  }  // if-else

  return -1;
}  // get_player_team()



int util_kick_player( int _iSessionId, const char* _pcKickMsg )
{
	if ( NULL == _pcKickMsg ) {
		DEBUG_LOG(2, ("Running server command 'kick # %i'", _iSessionId) );
		SERVER_COMMAND( UTIL_VarArgs("kick # %i\n", _iSessionId) );
	} else {
		DEBUG_LOG(2, ("Running server command 'kick # %i %s'", _iSessionId, _pcKickMsg) );
		SERVER_COMMAND( UTIL_VarArgs("kick # %i %s\n", _iSessionId, _pcKickMsg) );
	}

	return 1;
}

int util_kick_player( edict_t* _peEntity, const char* _pcKickMsg )
{
	if ( NULL == _peEntity ) return 0;
	int iSID = GETPLAYERUSERID( _peEntity );
	if ( 0 == iSID ) return 0;
	util_kick_player( iSID, _pcKickMsg );
	return 1;
}



void util_kill_player( CBaseEntity* pPlayer ) {

#if 0 // Try number 1
	CBasePlayer *pl = (CBasePlayer*) CBasePlayer::Instance( pPlayer->pev );
	pPlayer->edict()->v.health = 0;
	//pPlayer->Killed( pPlayer->pev, GIB_NEVER );
	pPlayer->edict()->v.deadflag = DEAD_DYING;
#endif

	MDLL_ClientKill( pPlayer->edict() );

}  // util_kill_player()

#ifdef USE_MYSQL

MYSQL_RES* admin_mysql_query(char *sQuery/* the SQL query */,char *sType /* string name for the type of query*/) {

  if ( sQuery == NULL ) return NULL;

  int iSQLErr;      // used for db func return values
  MYSQL_RES* pResult = NULL;
  MYSQL_ROW pRow = NULL;	


  if ((int)CVAR_GET_FLOAT("admin_debug") != 0) 
    UTIL_LogPrintf("[ADMIN] MYSQL: Running query \"%s\"\n",sQuery);

  iSQLErr = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
  if (iSQLErr) {
    if (mysql_errno(&mysql) == CR_SERVER_LOST || mysql_errno(&mysql) == CR_SERVER_GONE_ERROR) {
      iSQLErr = mysql_ping(&mysql);
      if (!iSQLErr) {
        UTIL_LogPrintf("[ADMIN] WARNING: Select query for %ss returned disconnect, reconnect succeeded.\n", sType);
        iSQLErr = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
      } else {
        UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned disconnect, reconnect failed.\n", sType);
        return NULL;
      }
    } else if (mysql_errno(&mysql) == CR_COMMANDS_OUT_OF_SYNC) {
      UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned commands out of sync!  NO FURTHER SQL QUERIES WILL WORK.\n", sType);
      return NULL;
    } else {
      UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned error: \"%s\"\n", sType, mysql_error(&mysql));
      return NULL;
    }
  }
  if (!iSQLErr) {
    pResult = mysql_store_result(&mysql);
  }
 
  return pResult;
} // admin_mysql_query()


const char* am_mysql_password_encrypt( const char* _pcPassword ) {

	static char acPassword[PASSWORD_SIZE];

	if ( _pcPassword == NULL ) return NULL;

	// Password cache: if we get passed the same password twice we can
	// reuse the result from the last encryption.
	static char acPwCache[PASSWORD_SIZE] = "\01\02";
	if ( strncmp(acPwCache, _pcPassword, PASSWORD_SIZE) == 0 ) {
		DEVEL_LOG(3, ("mysql_password_encrypt() returning cached password: '%s'", acPassword) );
		return acPassword;
	}  // if
	am_strncpy( acPwCache, _pcPassword, PASSWORD_SIZE );
		

	int iSQLErr;
	MYSQL_RES *pResult = NULL;
	MYSQL_ROW pRow = NULL;

	char acQuery[17+PASSWORD_SIZE+3] = "SELECT PASSWORD('";
	strncat( acQuery, _pcPassword, PASSWORD_SIZE );
	strcat( acQuery, "')" );

	iSQLErr = mysql_query( &mysql, acQuery );
	if (iSQLErr) {
		if (mysql_errno(&mysql) == CR_SERVER_LOST || mysql_errno(&mysql) == CR_SERVER_GONE_ERROR) {
			iSQLErr = mysql_ping(&mysql);
			if (!iSQLErr) {
				UTIL_LogPrintf("[ADMIN] WARNING: Select query for PASSWORD() returned disconnect, reconnect succeeded.\n" );
				iSQLErr = mysql_query(&mysql, acQuery );
			} else {
				UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned disconnect, reconnect failed.\n" );
				return NULL;
			}
		} else if (mysql_errno(&mysql) == CR_COMMANDS_OUT_OF_SYNC) {
			UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned commands out of sync!  NO FURTHER SQL QUERIES WILL WORK.\n" );
			return NULL;
		} else {
			UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned error: \"%s\"\n", mysql_error(&mysql));
			return NULL;
		}  // if-else
	}  // if

	if (!iSQLErr) {
		pResult = mysql_use_result(&mysql);
	}  // if

	if ( iSQLErr || pResult==NULL ) {
		UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned NULL result.\n" );
		return NULL;
	} else {
		if  ( (pRow = mysql_fetch_row(pResult)) != NULL ) {
			am_strncpy( acPassword, pRow[0], PASSWORD_SIZE );
		} else {
			mysql_free_result( pResult );
			return NULL;
		}

		mysql_free_result(pResult);
	}  // if-else

	DEVEL_LOG(3, ("mysql PASSWORD() encrypted password is '%s'", acPassword) );

	return acPassword;		
}  // am_mysql_password_encrypt()




#endif

#ifdef USE_PGSQL

PGresult* admin_pgsql_query(char *sQuery/* the SQL query */,char *sType /* string name for the type of query*/) {

  if ( sQuery == NULL ) return NULL;

  if (g_fUsePgSQL == FALSE) {
    return FALSE;
  }

  PGresult* pResult = NULL;

  if ((int)CVAR_GET_FLOAT("admin_debug") != 0) 
    UTIL_LogPrintf("[ADMIN] PGSQL: Running query \"%s\"\n",sQuery);

  pResult = PQexec(pgsql, sQuery);
  if ((PQresultStatus(pResult) != PGRES_TUPLES_OK) && (PQstatus(pgsql) != CONNECTION_OK)) {
    PQreset(pgsql);
    if (PQstatus(pgsql) == CONNECTION_OK) {
      UTIL_LogPrintf("[ADMIN] WARNING: Select query for %ss returned disconnect, reconnect succeeded.\n", sType);
      pResult = PQexec(pgsql, sQuery);
    } else {
      UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned disconnect, reconnect failed.\n", sType);
      return NULL;
    }
  }

  if (PQresultStatus(pResult) != PGRES_TUPLES_OK) {
    UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned error: \"%s\"\n", sType, PQresultErrorMessage(pResult));
    return NULL;
  }

  return pResult;
} // admin_pgsql_query()

#endif