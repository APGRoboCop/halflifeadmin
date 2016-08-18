#include "extdll.h"

extern globalvars_t                               *gpGlobals;
extern enginefuncs_t g_engfuncs;




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
		//	printf("%s %s\n",STRING(pKick->pev->netname),name);
			if(FStrEq(STRING(pKick->pev->netname),name))
			return pKick;
		}
	}
return NULL;
}


char* UTIL_VarArgs( char *format, ... )
{
	va_list		argptr;
	static char		string[1024];
	
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
	va_list			argptr;
	static char		string[1024];
	
	va_start ( argptr, fmt );
	vsprintf ( string, fmt, argptr );
	va_end   ( argptr );

	// Print to server console
	ALERT( at_logged, "%s", string );
}


int gmsgTextMsg;

void UTIL_ClientPrintAll( int msg_dest, const char *msg_name)
{
 gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 );  
	MESSAGE_BEGIN( MSG_ALL, gmsgTextMsg );
		WRITE_BYTE( msg_dest );
		WRITE_STRING( msg_name );

	MESSAGE_END();
}



CBaseEntity *UTIL_FindEntityByString( CBaseEntity *pStartEntity, const
char *szKeyword, const char *szValue )                                          {
        edict_t *pentEntity;

        if (pStartEntity)                                                                       pentEntity = pStartEntity->edict();
        else    
           pentEntity = NULL;

        pentEntity = FIND_ENTITY_BY_STRING( pentEntity, szKeyword, szValue );
                                                                                        if (!FNullEnt(pentEntity))
                return CBaseEntity::Instance(pentEntity);
        return NULL;
}                        
                                                                                 CBaseEntity *UTIL_FindEntityByClassname( CBaseEntity *pStartEntity, const char *szName )                                                                  {
        return UTIL_FindEntityByString( pStartEntity, "classname", szName );    }      

void fix_string(char *str,int len)
{

for(int i=0;i<len;i++) {
	if( str[i]=='>' || str[i]=='<' || str[i]=='%' ) strcpy(str+i,str+i+1);
}
 

}