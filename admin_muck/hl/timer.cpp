/***
*
*	Copyright (c) 2000, Alfred Reynolds
*
*   $Id: timer.cpp,v 1.10 2001/06/12 23:56:44 darope Exp $
*
****/

/*
  ===== timer.cpp ========================================================
  the timer class
*/

#include "extdll.h"
#include "users.h"
#include "amutil.h"

extern AMXINIT amx_Init;
extern AMXREGISTER amx_Register;
extern AMXFINDPUBLIC amx_FindPublic;
extern AMXEXEC amx_Exec;
extern AMXGETADDR amx_GetAddr;
extern AMXSTRLEN amx_StrLen;
extern AMXRAISEERROR amx_RaiseError;
extern AMXSETSTRING amx_SetString;
extern AMXGETSTRING amx_GetString;

extern DLL_GLOBAL BOOL  g_fRunScripts;
extern DLL_GLOBAL BOOL  g_fRunPlugins;

extern DLL_GLOBAL  edict_t *pAdminEnt;

extern DLL_GLOBAL mapcycle_t mapcycle;
extern DLL_GLOBAL timer_struct timers[NUM_TIMERS];

LINK_ENTITY_TO_CLASS( adminmod_timer, CTimer );
 
// Adds a new timer event
int CTimer::AddTimer(AMX* amx, int iWait, int iRepeat, char* sFunction, char* sParam, edict_t *pEntity) {
  int i;
  
  for(i = 0;i < NUM_TIMERS; i++) {
    if(timers[i].iWait == 0) 
      break;
  }
  if (i == NUM_TIMERS) {
    return INVALID_TIMER;
  }
  
  timers[i].iStart = time(NULL);
  timers[i].iWait = iWait;
  timers[i].iRepeatCount = iRepeat;
  strcpy(timers[i].sFunction,sFunction);
  if (sParam != NULL) {
    strcpy(timers[i].sParam, sParam);
  } else {
    strcpy( timers[i].sParam,"");
  }
  timers[i].pEntity=pEntity;
  timers[i].amx = amx;
  DEBUG_LOG( 3, ("CTimer::AddTimer: Timer %i added for function %s\n",i,timers[i].sFunction) );
  CalcNextTimer();
  return i;
}

// Calculates when the next timer event is due to fire
void CTimer::CalcNextTimer() {
  int iCurrentTime = time(NULL);
  int iNextTime = iCurrentTime + 99999;
  int i;
  
  m_iNextTimer = INVALID_TIMER;
  for(i = 0; i < NUM_TIMERS; i++) {
    if ((timers[i].iWait > 0) && (timers[i].iStart > 0) && (timers[i].iStart + timers[i].iWait) < iNextTime) {
      iNextTime = timers[i].iStart + timers[i].iWait;
      m_iNextTimer = i;
    }
  }
  
  if (m_iNextTimer == INVALID_TIMER) {
    DEBUG_LOG( 3, ("CTimer::CalcNextTimer: No next timer event found.\n") );
    SetTimer(-1);
  } else {
    DEBUG_LOG( 3, ("CTimer::CalcNextTimer: Next timer event found: #%i -- %s.\n",m_iNextTimer,timers[m_iNextTimer].sFunction) );
    SetTimer(iNextTime - iCurrentTime);
  }
}

// Deletes a timer event
int CTimer::DeleteTimer(int iTimer, int iForceDelete = 0) {
  if (!ValidTimerIndex(iTimer)) 
    return 0;
  if(timers[iTimer].iRepeatCount <= 1 || iForceDelete) {
    if ((int)CVAR_GET_FLOAT("admin_debug") >1) {
      if (iForceDelete) {
	UTIL_LogPrintf("[ADMIN] DEBUG: CTimer::DeleteTimer: Timer #%i force-deleted.\n", iTimer);
      } else {
	UTIL_LogPrintf("[ADMIN] DEBUG: CTimer::DeleteTimer: Timer #%i has no repeats left.  Deleting.\n", iTimer);
      }
    }
    memset(&timers[iTimer],0x0,sizeof(timer_struct));
  } else {
    if (timers[iTimer].iRepeatCount < 99999)
      timers[iTimer].iRepeatCount--;
    
    DEBUG_LOG( 3, ("CTimer::DeleteTimer: Timer #%i repeats decremented to %i.\n", iTimer, timers[iTimer].iRepeatCount) );
    timers[iTimer].iStart = time(NULL);
  }
  CalcNextTimer();
  return 1;
}

int CTimer::GetMaxVoteChoice() {
	if (m_iVote == INVALID_TIMER) {
		return INVALID_TIMER;
	} else {
		return m_iMaxVoteChoice;
	}
}

int CTimer::GetPlayerVote(int iIndex) {
  if (m_iVote == INVALID_TIMER) {
    return INVALID_TIMER;
  } else if (iIndex < 0 || iIndex > MAX_PLAYERS) {
    return INVALID_TIMER;
  } else {
    return m_iPlayerVotes[iIndex];
  }
}

void CTimer::SetPlayerVote(int iIndex, int iVote) {
  if (m_iVote != INVALID_TIMER && iIndex >= 0 && iIndex <= MAX_PLAYERS) {
    m_iPlayerVotes[iIndex] = iVote;
  }
}

BOOL CTimer::StartVote(AMX* amx, char* sText, int iChoiceCount, int iBits, char* sFunction, char* sParam, edict_t *pEntity) {
  int i;
  int iTimer;
  CBaseEntity* pPlayer;
  
  if (!VoteAllowed())
    return FALSE;
  
  iTimer = AddTimer(amx,30,1,sFunction,sParam,pEntity);
  if (iTimer == INVALID_TIMER)
    return FALSE;
  
	m_iMaxVoteChoice = iChoiceCount;
  m_iVote = iTimer;
  // The next time a vote can start is now, plus 30 seconds (for when
  // this vote is over) plus the frequency.
  m_iNextVoteTime = time(NULL) + 30 + (int)CVAR_GET_FLOAT("vote_freq");
  
  // Initialize the array to -1 (invalid vote)
  memset(m_iPlayerVotes,-1,(MAX_PLAYERS + 1) * sizeof(int));
  // Show the vote
  for ( i = 1; i <= gpGlobals->maxClients; i++ ) { // send the vote to everyone
    pPlayer = UTIL_PlayerByIndex(i);
    if (IsPlayerValid(pPlayer)) {
      ShowMenu (ENT(pPlayer->pev), iBits, MENU_SHOW, 0, sText);
      // Initialize to no vote
      m_iPlayerVotes[i] = 0;
    }
  }	
  
  return TRUE;
}

BOOL CTimer::ValidTimerIndex(int iTimer) {
  return (iTimer >= 0 && iTimer <= NUM_TIMERS);
}

BOOL CTimer::VoteAllowed() {
  return (time(NULL) >= m_iNextVoteTime);
}

BOOL CTimer::VoteInProgress() {
  return (m_iVote != INVALID_TIMER);
}

void CTimer :: Spawn(void) {
  m_iNextTimer = INVALID_TIMER;
  m_iNextVoteTime = time(NULL);
  m_iVote = INVALID_TIMER;
  memset(timers,0x0,NUM_TIMERS * sizeof(timer_struct));
  memset(m_iPlayerVotes,0x0,(MAX_PLAYERS + 1) * sizeof(int));
  
  SetThink( &CTimer::Think );
  CalcNextTimer();
}                  


void CTimer::Precache(void) {    
  // Do nothing
}

void CTimer::SetTimer(int amount) {    
  if (amount==-1) {
    pev->nextthink = gpGlobals->time + 10000;
  } else {
    pev->nextthink = gpGlobals->time + amount;
  }
  DEBUG_LOG( 3, ( "CTimer::SetTimer: Timer will next fire in %f seconds\n",pev->nextthink - gpGlobals->time) );
}


void CTimer::Think(void) { 
  int iError;
  int iIndex;
  int iTimer = m_iNextTimer;
  cell iReturn;
  
  if (iTimer == INVALID_TIMER) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Timer entity triggered, but no timers need to run.\n");
  } else if(!g_fRunScripts && !g_fRunPlugins) {
    UTIL_LogPrintf( "[ADMIN] ERROR: Timer triggered, but scripting is disabled.\n");
  } else {
    
    pAdminEnt=timers[iTimer].pEntity;
    if (strlen(timers[iTimer].sFunction) <= 0 ) {
      UTIL_LogPrintf( "[ADMIN] ERROR: Timer triggered, but function name is empty.\n");
    } else {
      iError = amx_FindPublic(timers[iTimer].amx,timers[iTimer].sFunction,&iIndex);
      
      if (iError != AMX_ERR_NONE) {
	UTIL_LogPrintf( "[ADMIN] ERROR: Timer triggered, but finding function name %s returned error %i\n",timers[iTimer].sFunction,iError);
      } else {
	// Are we answering a vote?
	if (iTimer == m_iVote) {
	  int iMaxPossibleVotes = 0;
	  int iMaxVotes = 0;
	  int iPlayer;
	  int iVote;
	  int iVotes[11] = {0};
	  int iWinningVote = 0;
	  
	  DEBUG_LOG( 3, ("CTimer::Think: Vote ended.\n") );
	  
	  for (iPlayer = 0; iPlayer <= MAX_PLAYERS; iPlayer++) {
	    iVote = m_iPlayerVotes[iPlayer];
	    
	    if (iVote >= 0)
	      iMaxPossibleVotes++;
	    
	    if (iVote > 0) {
	      iVotes[iVote]++;
	      if (iVotes[iVote] > iMaxVotes) {
	      	iMaxVotes = iVotes[iVote];
					iWinningVote = iVote;
	      }
	    }
	  }
	  
	  iError = amx_Exec(timers[iTimer].amx, &iReturn, iIndex, 4, iWinningVote, timers[iTimer].sParam, iVotes[iWinningVote], iMaxPossibleVotes);
	  if (iError != AMX_ERR_NONE) {
	    UTIL_LogPrintf( "[ADMIN] ERROR: Calling vote function %s returned error %i\n",timers[iTimer].sFunction,iError);
	  }
	  
	  m_iVote = INVALID_TIMER;
	  memset(m_iPlayerVotes,0x0,(MAX_PLAYERS + 1) * sizeof(int));
	  
	  // Or are we just calling a function?
	} else {
	  // pEntity may be null, if this was triggered by the console
	  // We offset the timer index by one that we feed to the script
	  // because we don't want to feed an index 0 timer (hard to
	  // distinguish that from an error)
	  if(timers[iTimer].pEntity!=NULL) {
	    iError = amx_Exec(timers[iTimer].amx, &iReturn, iIndex, 4, iTimer + 1, timers[iTimer].iRepeatCount, STRING(timers[iTimer].pEntity->v.netname), timers[iTimer].sParam);
	  } else {
	    iError = amx_Exec(timers[iTimer].amx, &iReturn, iIndex, 4, iTimer + 1, timers[iTimer].iRepeatCount, "Admin", timers[iTimer].sParam);
	  }
	  if (iError != AMX_ERR_NONE) {
	    UTIL_LogPrintf( "[ADMIN] ERROR: Calling timer function %s returned error %i\n",timers[iTimer].sFunction,iError);
	  }
	}
      }
      
      // Call the delete timer function to reduce the repeat count,
      // and delete the timer if necessary
      DeleteTimer(iTimer, 0);
    }
  }
  CalcNextTimer();
}
