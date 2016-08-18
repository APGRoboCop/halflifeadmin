#include "extdll.h"
#include "amutil.h"
#include "amlibc.h"
#include "users.h"
#include "builtin_commands.h"
#include "events.h"
#define SDK_UTIL_H
#include "meta_api.h"

//This is in admin_mod.cpp - no .h for some reason
unsigned int me_log_fix( bool Log, bool Fix );

//These also should be in a .h somewhere
extern auth_struct g_AuthArray[MAX_PLAYERS + 1];
extern DLL_GLOBAL edict_t *pTimerEnt;

plugin_result CAdminPasswordHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	if (pEntity == NULL) {
		UTIL_LogPrintf("Laf. Why are you trying to enter a password?  You're at the console!\n");
	} else if ( strlen(szData)>0) {
		SetUserPassword(STRING(pEntity->v.netname),szData,pEntity);
		VerifyUserAuth(STRING(pEntity->v.netname), pEntity);
	} else {
		CLIENT_PRINTF( pEntity, print_console,"Bad admin_password format, try: admin_password <password>\n");
	}
	return PLUGIN_HANDLED;
}


plugin_result CAdminHelpHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	HandleHelp(pEntity,szData,0);
	return PLUGIN_HANDLED;
}

plugin_result CAdminHelpHlratHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	/* Thrown in for HLRat.  admin_help, but formatted a bit nicer for machines,
	as opposed to humans. */
    HandleHelp(pEntity,szData,1);
    return PLUGIN_HANDLED;
}

plugin_result CAdminVersionHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	HandleVersion(pEntity);
	return PLUGIN_HANDLED;
}

plugin_result CAdminAdmHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	if ( szData != NULL && strcasecmp( szData, "mefix") == 0 ) {
		int ret = me_log_fix( false, true );
		if ( ret ) {
			System_Response( UTIL_VarArgs("Attempted to fix %i suspicious entities.\n", ret), pEntity );
		} else {
			System_Response( "No suspicious entities found to be fixed.\n", pEntity );
		}  // if
	}  // if
	return PLUGIN_HANDLED;
}

plugin_result CAdminStatusHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	int iIndx = 0;
	if ( szData == 0 || pEntity != 0 || *szData == '\0' || !stricmp(szData,"am i") ) {
		iIndx = ENTINDEX( pEntity );
	} else {
		iIndx = GetPlayerIndex( szData );
	}  // if-else
	char acString[1024];
	if ( iIndx == 0 ) {
		snprintf( acString, sizeof(acString), "[ADMIN] Status: No such player: '%s'\n", szData );
	} else {
		snprintf( acString, sizeof(acString), "[ADMIN] Status: Name: '%s', Access: %i \n", g_AuthArray[iIndx].sUserName, g_AuthArray[iIndx].iAccess );
	}  // if-else
	System_Response( acString, pEntity );
	return PLUGIN_HANDLED;
}
  
plugin_result CMenuSelectHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	if (strlen(szData)>0) {
		CTimer *pTimer = (CTimer *)GET_PRIVATE(pTimerEnt);
		if (pTimer->VoteInProgress()) {
			int iIndex;
			int iVote = atoi(CMD_ARGV(1));
    
			if (pEntity == NULL) {
				iIndex = 0;
			} else {
				iIndex = ENTINDEX(pEntity);
			}
			if (pTimer->GetPlayerVote(iIndex) != 0) {
				// This person has either already voted, or
				// was not shown the vote.  Either way, we ignore it.
				// Do nothing.
			} else if (iVote > pTimer->GetMaxVoteChoice()) {
				// The vote is out of range.  Ignore it.
			} else {
				pTimer->SetPlayerVote(iIndex, iVote);
				System_Response(UTIL_VarArgs("[ADMIN] Vote entered for option #%i\n",atoi(CMD_ARGV(1))),pEntity);

				if((int)CVAR_GET_FLOAT("admin_vote_echo") != 0) {
					if (pEntity == NULL) {
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%s voted for option #%i\n","Admin",atoi(CMD_ARGV(1))));
					} else {
						UTIL_ClientPrintAll(HUD_PRINTTALK, UTIL_VarArgs("%s voted for option #%i\n",STRING(pEntity->v.netname),atoi(CMD_ARGV(1))));
					}
				}
				return PLUGIN_HANDLED;
			}
		}
	}
	return PLUGIN_CONTINUE;
}

plugin_result CAdminCommandStatsHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	char acString[1024];
	int time = (int)gpGlobals->time/60;
	if (time<1) { time=1; }

	snprintf( acString, sizeof(acString), "[ADMIN] %i commands - %i per minute - largest spike = %i\n", 
				g_Dispatcher.CommandCount(),g_Dispatcher.CommandCount()/time,g_Dispatcher.MaxSpike());
	System_Response( acString, pEntity );
	snprintf( acString, sizeof(acString), "[ADMIN] %i spikes of 5 or more commands - %i per minute\n", 
				g_Dispatcher.SpikeCount(),g_Dispatcher.SpikeCount()/time);
	System_Response( acString, pEntity );
	return PLUGIN_HANDLED;
}

plugin_result CAdminListGameMessagesHandler::Dispatch(edict_t* pEntity,char* szCmd, char* szData) const {
	//Only available from server console
	if (pEntity!=NULL) {
		System_Response( "This command is only available to the server console\n", pEntity );
		return PLUGIN_HANDLED;
	}

	int nSize;
	const char* szMsgName;
	char acString[1024];

	System_Response( "Registered Messages are:", pEntity );
	for(int i=0;i<MAX_MESSAGEID;i++) {
		nSize=-1;
		szMsgName = GET_USER_MSG_NAME(PLID,i,&nSize);
		if (szMsgName) {
			snprintf( acString, sizeof(acString), "[ADMIN] %i: %s (%i bytes)\n", i,szMsgName,nSize);
			System_Response( acString, pEntity );
		}
	}
	return PLUGIN_HANDLED;
}

void RegisterBuiltinCommands() {
	g_Dispatcher.Register("admin_password",new CAdminPasswordHandler());
	g_Dispatcher.Register("admin_help",new CAdminHelpHandler());
	g_Dispatcher.Register("admin_help_hlrat",new CAdminHelpHlratHandler());
	g_Dispatcher.Register("admin_version",new CAdminVersionHandler());
	g_Dispatcher.Register("admin_adm",new CAdminAdmHandler());
	g_Dispatcher.Register("admin_status",new CAdminStatusHandler());
	g_Dispatcher.Register("menuselect",new CMenuSelectHandler());
	g_Dispatcher.Register("admin_commandstats",new CAdminCommandStatsHandler());
	g_Dispatcher.Register("admin_listgamemessages",new CAdminListGameMessagesHandler());
}
