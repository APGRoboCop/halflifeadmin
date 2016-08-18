/* 
 * This file implements some helper functions
 *
 * $Id: util.cpp,v 1.21 2001/09/16 03:33:10 alfred Exp $
 *
 */   

#ifdef USE_MYSQL
  #include <mysql.h>
  #include <errmsg.h>

#  include "extdll.h"
  extern MYSQL mysql;
  extern BOOL g_fUseMySQL;
#else
#  include "extdll.h"
#endif


extern globalvars_t   *gpGlobals;
extern enginefuncs_t g_engfuncs;
extern mapcycle_t mapcycle;

DLL_GLOBAL const Vector     g_vecZero = Vector(0,0,0);   

/* Like System_Response, but will _always_ log. */
void System_Error(char* str, edict_t* pEntity) {
  UTIL_LogPrintf(str);
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
    UTIL_LogPrintf(str);
  } else {
    CLIENT_PRINTF( pAdminEnt, print_console,str);
  }
  
}


#include <ctype.h> /* for isprint */
/*
  stop malformed names stuffing up checking

*/
int make_friendly(char *name, BOOL check) {
  int i; 
  int iLen = strlen(name);
  
  if(check && iLen<=0 ) return 2; // the name is zero length....  

  for(i=0; i < iLen; i++) {
    if (name[i]=='%') {
      name[i] = ' ';
    }
    // since we go through the string here anyway, we may just as well check for 
    // vicious characters
    if ( check &&  !isprint(name[i]) ) return 2;
  }
  return 1;
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
  static char             string[1024];
  
  va_start (argptr, format);
  vsnprintf (string, 1024, format,argptr);
  va_end (argptr);
  
  return string;  
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( char *fmt, ... ) {
  va_list                 argptr;
  static char             string[1024];
  
  va_start ( argptr, fmt );
  vsnprintf ( string, 1024, fmt, argptr );
  va_end   ( argptr );
  
  // Print to server console
  ALERT( at_logged, "%s", string );
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console but filter out newlines.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintfFNL( char *fmt, ... ) {
  va_list                 argptr;
  static char             string[1024];
  
  va_start ( argptr, fmt );
  vsnprintf ( string, 1024, fmt, argptr );
  va_end   ( argptr );
  
  int iNumChar = strlen(string);
  char* pcBufPos = string;
  while ( (pcBufPos = strchr(pcBufPos, '\n')) ) {
    if ( *(pcBufPos + 1) == '\0' ) break;
    if ( 1023-iNumChar ) {
      memmove( pcBufPos+1, pcBufPos, (iNumChar-(pcBufPos-string)+1) );
      pcBufPos[0] = '\\';
      pcBufPos[1] = 'n';
      iNumChar++;
    } else {
      *pcBufPos = ' ';
    }  // if-else
  }  // while
    
  // Print to server console
  ALERT( at_logged, "%s", string );
}

int gmsgTextMsg;

void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 ) {
  gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 ); 
  
  MESSAGE_BEGIN( MSG_ALL, gmsgTextMsg );
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
  char szMap[ 32 ];
  int length;
  char *pFileList;
  char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( filename, &length );
  int hasbuffer;
  mapcycle_item_s *item, *newlist = NULL, *next;
  
  if (aFileList == NULL || pFileList == NULL ) return 0;
  
  if ( pFileList && length )
    {
      // the first map name in the file becomes the default
      while ( 1 )
	{
	  hasbuffer = 0;
	  memset( szBuffer, 0, MAX_RULE_BUFFER );
	  
	  pFileList = COM_Parse( pFileList );
	  if ( strlen( com_token ) <= 0 )
	    break;
	  
	  strcpy( szMap, com_token );
	  
	  // Any more tokens on this line?
	  if ( COM_TokenWaiting( pFileList ) )
	    {
	      pFileList = COM_Parse( pFileList );
	      if ( strlen( com_token ) > 0 )
		{
		  hasbuffer = 1;
		  strcpy( szBuffer, com_token );
		}
	    }
	  
	  // Check map
	  if ( IS_MAP_VALID( szMap ) )
	    {
	      
	      item = new mapcycle_item_s;
	      
	      strcpy( item->mapname, szMap );
	      
	      item->next = cycle->items;
	      cycle->items = item;
	    }
	  else
	    {
	      ALERT( at_console, "Skipping %s from mapcycle, not a valid map\n", szMap );
	    }
	  
	  
	}
      
      FREE_FILE( aFileList );
    }
  
  // Fixup circular list pointer
  item = cycle->items;
  
  // Reverse it to get original order
  while ( item )
    {
      next = item->next;
      item->next = newlist;
      newlist = item;
      item = next;
    }
  cycle->items = newlist;
  item = cycle->items;
  
  // Didn't parse anything
  if ( !item )
    {
      return 0;
    }
  
  while ( item->next )
    {
      item = item->next;
    }
  item->next = cycle->items;
  
  cycle->next_item = item->next;
  
  
  
  
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
  
  char *mapcfile = (char*)CVAR_GET_STRING( "maps_file" );
  if (mapcfile==NULL || FStrEq(mapcfile,"0")) return 0;
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



int check_map(char *map)
{
  
  char *mapcfile = (char*)CVAR_GET_STRING( "maps_file" );
  
  if ( FStrEq(map,"next_map")) { // next map in the cycle
    
    mapcycle_item_s *item;
    char *mapcfile = (char*)CVAR_GET_STRING( "mapcyclefile" );
    DestroyMapCycle( &mapcycle );
    ReloadMapCycleFile( mapcfile, &mapcycle );
    item=CurrentMap(&mapcycle);
    strcpy(map,item->mapname);
    return (IS_MAP_VALID(item->mapname));
    
    
  } else { // they have defined the maps.ini file for us
    
    char *mapcfile = (char*)CVAR_GET_STRING( "maps_file" );
    if (mapcfile==NULL || FStrEq(mapcfile,"0") ) { // no maps.ini file defined, just check it is good
      return (IS_MAP_VALID(map));
    }
    if (!IS_MAP_VALID(map)) return 0; // bad map name
    return(allowed_map(map));
    
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



void UTIL_HudMessage( CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage )
{
  if ( !pEntity )
    return;
  
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
/* /CEM */



/* Rope's stuff */
void ClientPrint( entvars_t *client, int msg_dest, const char *msg_name, 
		  const char *param1, const char *param2, const char *param3, 
		  const char *param4 ) {
  
  gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 ); 
  
  MESSAGE_BEGIN( MSG_ONE, gmsgTextMsg, NULL, client );
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



void UTIL_ClientPrint_UR( entvars_t *client, int msg_dest, const char *msg_name, 
			  const char *param1, const char *param2, const char *param3, 
			  const char *param4 )
{

  gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 ); 

  MESSAGE_BEGIN( MSG_ONE_UNRELIABLE, gmsgTextMsg, NULL, client );
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



extern DLL_GLOBAL  edict_t *pAdminEnt;

void strtolower( char* string ) {
  
  int length = strlen ( string );
  char character = 0;

  for ( int i = 0; i < length; i++ ) {
    character = tolower( string[i] );
    string[i] = character;
  }  // for

  return;

}  // strtolower()





//
// stristr: This is a crude version of strtsr ignoring case.
// brute force since we only have a short text / pattern
// and the guys are waiting for their server. 
// must be improved some day.
//

char* stristr( const char* haystack, const char* needle ) {

  int iLength = strlen( haystack );
  int iNeedleLen = strlen( needle );
  char* pcBegin = 0;

  int iH = 0;
  int iN = 0;

  if ( iNeedleLen > iLength ) return 0;

  for ( int iStart = 0; iStart < iLength-iNeedleLen+1; iStart++ ) {
    pcBegin = (char*)(haystack + iStart);

    iH = iStart;
    iN = 0;

    while ( (iH < iLength) && (iN < iNeedleLen) && (tolower(haystack[iH]) == tolower(needle[iN])) ) {
      iH++;
      iN++;
    }  // while

    // match
    if ( iN == iNeedleLen ) return pcBegin;

    // no match
    if ( iH == iLength ) return 0;
  }  // for

  return 0;

}  // stristr()



/* /FZ */



// CEM - Rope's version of my GetPlayerIndex, for partial name matching. 
int GetPartialPlayerIndex(char *PlayerText) {
  int PlayerNumber = 0;
  int i;
  int found = 0;
  int index = 0;
  
  if ( strlen( PlayerText ) == 0 ) 
    return 0;
  
  if (atoi(PlayerText)!=0) {
    PlayerNumber=atoi(PlayerText);
  }  // if
  
  for (i = 1; i <= gpGlobals->maxClients; i++) {
    CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
    if (pPlayer) {
      if (PlayerNumber != 0 && ((int)GETPLAYERWONID(pPlayer->edict()) == PlayerNumber)) return i;
      if (PlayerNumber != 0 && (GETPLAYERUSERID(pPlayer->edict()) == PlayerNumber)) return i;
      if (!stricmp(STRING(pPlayer->pev->netname), PlayerText)) return i;
      // FZ: partial name match 
      if ( stristr(STRING(pPlayer->pev->netname), PlayerText) != 0) {
	index = i;
	found++;
      }  // if
      // /FZ
    }  // if
  }  // for
  // FZ ADDED
  if ( found == 1 ) 
    return index;
  
  if ( found > 1 ) {
    System_Response(UTIL_VarArgs( "Ambiguous player specification: %s. Be more specific.\n",PlayerText), pAdminEnt);
  }
  // /FZ
  return 0;
}
/* /Rope's stuff */




int GetPlayerIndex (char* PlayerText) {
  return GetPartialPlayerIndex( PlayerText );
}

/* CEM */
/* Replaced by GetPartialPlayerIndex

int GetPlayerIndex(char *PlayerText) {
  int PlayerNumber = 0;
  int i;
  
  if (atoi(PlayerText)!=0)
    PlayerNumber=atoi(PlayerText);
  
  for (i = 1; i <= gpGlobals->maxClients; i++) {
    CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
    if (IsPlayerValid(pPlayer)) {
      if (PlayerNumber!=0 && ((int)GETPLAYERWONID(pPlayer->edict())==PlayerNumber)) return i;
      if (PlayerNumber!=0 && (GETPLAYERUSERID(pPlayer->edict())==PlayerNumber)) return i;
      if (!stricmp(STRING(pPlayer->pev->netname),PlayerText)) return i;
    }
  }
  return 0;
}
*/
/* /CEM */




int GetPlayerCount() {
  int i;
  int iPlayerCount;
  
  iPlayerCount = 0;
  for ( i = 1; i <= gpGlobals->maxClients; i++ ) {
    CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);
    if (IsPlayerValid(pPlayer))
      iPlayerCount++;
  }
  return iPlayerCount;
}






// Formats the string-literals '\n' and '^n' into line feeds (ASCII 10)
void FormatLine(char* sLine) {
	int iOffset = 0;
	char* sNewChar = sLine;
	char* sOldChar = sLine;
	
	while (*sOldChar != '\0') {
		if ((*sOldChar == '\\' || *sOldChar == '^') && *(sOldChar + 1) == 'n') {
			*sNewChar = '\n';
			sOldChar++;
		} else if (sOldChar != sNewChar) {
			*sNewChar = *sOldChar;
		}
		sNewChar++;
		sOldChar++;
	}
	*sNewChar = '\0';
}



// Formats a path so all the /s and \s are correct for that OS.
void FormatPath(char* sPath) {
  while ( sPath != 0 && *sPath != '\0') {

#ifdef WIN32
    if ( *sPath == '/' ) {
      *sPath = '\\';
    }  // if
#else
    if ( *sPath == '\\' ) {
      *sPath = '/';
    }  // if
#endif						

    sPath++;
  }  // while
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
	 || !strcmp(acModel, "urban") ) {
      return 2;
    }  // if

    if ( !strcmp(acModel, "arctic")
	 || !strcmp(acModel, "leet")
	 || !strcmp(acModel, "arab")
	 || !strcmp(acModel, "guerilla")
	 || !strcmp(acModel, "terror") ) {
      return 1;
    }  // if

  } else {
    return poPlayer->edict()->v.team;
  }  // if-else

  return 0;
}  // get_player_team()


#ifdef USE_MYSQL

MYSQL_RES *admin_mysql_query(char *sQuery/* the SQL query */,char *sType /* string name for the type of query*/) {

  int iResult;      // used for db func return values
  MYSQL_RES *pResult=NULL;
  MYSQL_ROW pRow = NULL;	


  if ((int)CVAR_GET_FLOAT("admin_debug") != 0) 
    UTIL_LogPrintf("[ADMIN] MYSQL: Running query \"%s\"\n",sQuery);

  iResult = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
  if (iResult) {
    if (mysql_errno(&mysql) == CR_SERVER_LOST || mysql_errno(&mysql) == CR_SERVER_GONE_ERROR) {
      iResult = mysql_ping(&mysql);
      if (!iResult) {
        UTIL_LogPrintf("[ADMIN] WARNING: Select query for %ss returned disconnect, reconnect succeeded.\n", sType);
        iResult = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
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
  if (!iResult) {
    pResult = mysql_use_result(&mysql);
  }
 
  return pResult;
} // admin_mysql_query()
#endif
