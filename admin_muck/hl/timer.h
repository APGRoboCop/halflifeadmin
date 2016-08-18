/***
*
*	Copyright (c) 2000, Alfred Reynolds
*
*    $Id: timer.h,v 1.6 2001/04/29 09:46:55 magua Exp $
*
****/

/*  
    ===== timer.cpp ========================================================
    
    the timer class
*/

#ifndef _TIMER_H_
#define _TIMER_H_

#define BUF_SIZE 100                // max size in general (default)
#define INVALID_TIMER -1
#define MAX_PLAYERS 64              // max number of users allowed on a hl server. Its 32 but lets be safe :)
#define NUM_TIMERS 512

typedef struct {
  char sFunction[BUF_SIZE];       // Function to call when timer triggered
  char sParam[BUF_SIZE];          // Parameter to feed to function when timer triggered
  int iStart;                     // Time timer was started at
  int iWait;                      // Time it takes timer to trigger (iStart + iWait = trigger time)
  int iRepeatCount;               // How many times should this timer be triggered (1 = just once, 99999 = infinite)
  edict_t *pEntity;               // Entity who started this timer
  AMX* amx;                       // AMX virtual machine to find function in
} timer_struct;

class CTimer: public CBaseEntity {
  //class CBaseEntity {
  //class CTimer : public CBaseDelay { 
 public:    
  
  void Spawn( void );
  void Precache(void);
  
  
  
  
  //edict_t  edict_t *edict() { return ENT( pev ); }
  //int       entindex( ) { return ENTINDEX( edict() ); }  
  
  void KeyValue( KeyValueData *pkvd ) { pkvd->fHandled = FALSE; }        
  int  Save( CSave &save ){ return -1;} 
  int  Restore( CRestore &restore ) {return -1;}    
  
  int  ObjectCaps( void ) { return FCAP_ACROSS_TRANSITION; }    
  void Activate( void ) {}
  void SetObjectCollisionBox( void ){}
  int  Classify ( void ) { return CLASS_NONE; };      
  void DeathNotice ( entvars_t *pevChild ) {}// monster maker children use 
  void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) {}
  int  TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType ){return -1;}
  int  TakeHealth( float flHealth, int bitsDamageType ){return -1;}
  void Killed( entvars_t *pevAttacker, int iGib ){}
  int  BloodColor( void ) { return DONT_BLEED; }
  void TraceBleed( float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType ) {}
  BOOL IsTriggered( CBaseEntity *pActivator ) {return TRUE;}
  //  CBaseMonster *MyMonsterPointer( void ) { return NULL;}
  //CSquadMonster *MySquadMonsterPointer( void ) { return NULL;}
  int   GetToggleState( void ) { return TS_AT_TOP; }
  void  AddPoints( int score, BOOL bAllowNegativeScore ) {}
  void  AddPointsToTeam( int score, BOOL bAllowNegativeScore ) {}
  BOOL  AddPlayerItem( CBasePlayerItem *pItem ) { return 0; }
  BOOL  RemovePlayerItem( CBasePlayerItem *pItem ) { return 0; }
  int   GiveAmmo( int iAmount, char *szName, int iMax ) { return -1; };
  float GetDelay( void ) { return 0; }
  int   IsMoving( void ) { return pev->velocity != g_vecZero; }
  void  OverrideReset( void ) {}
  int   DamageDecal( int bitsDamageType ) {return -1;}
  // This is ONLY used by the node graph to test movement through a door
  void  SetToggleState( int state ) {}
  void  StartSneaking( void ) {}
  void  StopSneaking( void ) {}
  BOOL  OnControls( entvars_t *pev ) { return FALSE; }             
  BOOL  IsSneaking( void ) { return FALSE; }
  BOOL  IsAlive( void ) { return 0; }
  BOOL  IsBSPModel( void ) { return 0; }
  BOOL  ReflectGauss( void ) { return ( 0 ); }
  BOOL  HasTarget( string_t targetname ) { return 0 ; }
  BOOL  IsInWorld( void ) {return FALSE;}
  BOOL  IsPlayer( void ) { return FALSE; }
  BOOL  IsNetClient( void ) { return FALSE; }
  const char *TeamID( void ) { return ""; }           
  CBaseEntity *GetNextTarget( void ) { return this;}

  void Think(void);

  void Touch( CBaseEntity *pOther ) {  };
  void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
  {
  }
  void Blocked( CBaseEntity *pOther ) { };          
  CBaseEntity *Respawn( void ) { return this; }  
  void UpdateOwner( void ) { return; };    
  BOOL FBecomeProne( void ) {return FALSE;};   
  Vector Center( ) { return (pev->absmax + pev->absmin) * 0.5; }; // center point of entity
  Vector EyePosition( ) { return pev->origin + pev->view_ofs; };                  // position of eyes
  Vector EarPosition( ) { return pev->origin + pev->view_ofs; };                  // position of ears
  Vector BodyTarget( const Vector &posSrc ) { return Center( ); };                // position to shoot at
  
  int Illumination( ) { return 0; };
  BOOL FVisible ( CBaseEntity *pEntity ) { return FALSE;}
  BOOL FVisible ( const Vector &vecOrigin )  { return FALSE;}     
  static CBaseEntity *Create( char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner = NULL );
  void    MakeDormant( void ) {};

  void SetTimer(int amount);
  int AddTimer(AMX* amx, int iWait, int iRepeat, char* sFunction, char* sParam, edict_t *pEntity);
  void CalcNextTimer();
  int DeleteTimer(int iTimer, int iForceDelete);
	int GetMaxVoteChoice();
  int GetPlayerVote(int iIndex);
  void SetPlayerVote(int iIndex, int iVote);
  BOOL StartVote(AMX* amx, char* sText, int iChoiceCount, int iBits, char* sFunction, char* sParam, edict_t *pEntity);
  BOOL ValidTimerIndex(int iTimer);
  BOOL VoteAllowed( void );
  BOOL VoteInProgress( void );
  
 private:
	int m_iMaxVoteChoice;
  int m_iNextTimer;
  int m_iNextVoteTime;
  int m_iVote;
  int m_iPlayerVotes[MAX_PLAYERS + 1];
  timer_struct timers[NUM_TIMERS];                                            
};

#endif //TIMER_H
