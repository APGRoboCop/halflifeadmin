#include "extdll.h"

extern globalvars_t                               *gpGlobals;
extern enginefuncs_t g_engfuncs;
extern mapcycle_t mapcycle;



CBaseEntity     *UTIL_PlayerByIndex( int playerIndex )
{
        CBaseEntity *pPlayer = NULL;

        if ( playerIndex > 0 && playerIndex <= gpGlobals->maxClients )
        {
                edict_t *pPlayerEdict = INDEXENT( playerIndex );
                if ( pPlayerEdict && !pPlayerEdict->free )
                {
                        pPlayer = CBaseEntity::Instance( pPlayerEdict );
                }
        }

        return pPlayer;
}      


CBaseEntity     *UTIL_PlayerByName( char *name ) {
int i;
CBaseEntity     *pKick;
 for ( i = 1; i <= gpGlobals->maxClients; i++ )
        {
                pKick = UTIL_PlayerByIndex( i );
                if ( pKick) {
                //      printf("%s %s\n",STRING(pKick->pev->netname),name);
                        if(FStrEq(STRING(pKick->pev->netname),name))
                        return pKick;
                }
        }
return NULL;
}


char* UTIL_VarArgs( char *format, ... )
{
        va_list         argptr;
        static char             string[1024];
        
        va_start (argptr, format);
        vsprintf (string, format,argptr);
        va_end (argptr);

        return string;  
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf( char *fmt, ... )
{
        va_list                 argptr;
        static char             string[1024];
        
        va_start ( argptr, fmt );
        vsprintf ( string, fmt, argptr );
        va_end   ( argptr );

        // Print to server console
        ALERT( at_logged, "%s", string );
}


int gmsgTextMsg;

void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char *param3, const char *param4 )
{
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
        return UTIL_FindEntityByString( pStartEntity, "classname", szName
);    }      

void fix_string(char *str,int len)
{

for(int i=0;i<len;i++) {
        if( str[i]=='>' || str[i]=='<' || str[i]=='%' )
strcpy(str+i,str+i+1);
}
 

}



/*********************************************************************
new map stuff

**********************************************************************/


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

