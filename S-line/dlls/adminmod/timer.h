/*
 * ===========================================================================
 *
 * $Id: timer.h,v 1.3 2003/03/26 20:44:11 darope Exp $
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
 *   the timer class
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
  
  void Spawn() override;
  void Precache() override;
  
  //edict_t  edict_t *edict() { return ENT( pev ); }
  //int       entindex( ) { return ENTINDEX( edict() ); }  
  
  void KeyValue( KeyValueData *pkvd ) override { pkvd->fHandled = FALSE; }        
  int  Save( CSave &save ) override { return -1;} 
  int  Restore( CRestore &restore ) override {return -1;}    
  
  int  ObjectCaps() override { return FCAP_ACROSS_TRANSITION; }    
  void Activate() override {}
  void SetObjectCollisionBox() override {}
  int  Classify () override { return CLASS_NONE; };      
  void DeathNotice ( entvars_t *pevChild ) override {}// monster maker children use 
  void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType) override {}
  int  TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType ) override {return -1;}
  int  TakeHealth( float flHealth, int bitsDamageType ) override {return -1;}
  void Killed( entvars_t *pevAttacker, int iGib ) override {}
  int  BloodColor() override { return DONT_BLEED; }
  void TraceBleed( float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType ) override {}
  BOOL IsTriggered( CBaseEntity *pActivator ) override {return TRUE;}
  //  CBaseMonster *MyMonsterPointer() { return nullptr;}
  //CSquadMonster *MySquadMonsterPointer() { return nullptr;}
  int   GetToggleState() override { return TS_AT_TOP; }
  void  AddPoints( int score, BOOL bAllowNegativeScore ) override {}
  void  AddPointsToTeam( int score, BOOL bAllowNegativeScore ) override {}
  BOOL  AddPlayerItem( CBasePlayerItem *pItem ) override { return 0; }
  BOOL  RemovePlayerItem( CBasePlayerItem *pItem ) override { return 0; }
  int   GiveAmmo( int iAmount, char *szName, int iMax ) override { return -1; };
  float GetDelay() override { return 0; }
  int   IsMoving() override { return pev->velocity != g_vecZero; }
  void  OverrideReset() override {}
  int   DamageDecal( int bitsDamageType ) override {return -1;}
  // This is ONLY used by the node graph to test movement through a door
  void  SetToggleState( int state ) override {}
  void  StartSneaking() override {}
  void  StopSneaking() override {}
  BOOL  OnControls( entvars_t *pev ) override { return FALSE; }             
  BOOL  IsSneaking() override { return FALSE; }
  BOOL  IsAlive() override { return 0; }
  BOOL  IsBSPModel() override { return 0; }
  BOOL  ReflectGauss() override { return ( 0 ); }
  BOOL  HasTarget( string_t targetname ) override { return 0 ; }
  BOOL  IsInWorld() override {return FALSE;}
  BOOL  IsPlayer() override { return FALSE; }
  BOOL  IsNetClient() override { return FALSE; }
  const char *TeamID() override { return ""; }           
  CBaseEntity *GetNextTarget() override { return this;}

  void Think() override;

  void Touch( CBaseEntity *pOther ) override {  };
  void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value ) override
  {
  }
  void Blocked( CBaseEntity *pOther ) override { };          
  CBaseEntity *Respawn() override { return this; }  
  void UpdateOwner() override { return; };    
  BOOL FBecomeProne() override {return FALSE;};   
  Vector Center( ) override { return (pev->absmax + pev->absmin) * 0.5; }; // center point of entity
  Vector EyePosition( ) override { return pev->origin + pev->view_ofs; };                  // position of eyes
  Vector EarPosition( ) override { return pev->origin + pev->view_ofs; };                  // position of ears
  Vector BodyTarget( const Vector &posSrc ) override { return Center( ); };                // position to shoot at
  
  int Illumination( ) override { return 0; };
  BOOL FVisible ( CBaseEntity *pEntity ) override { return FALSE;}
  BOOL FVisible ( const Vector &vecOrigin ) override { return FALSE;}     
  static CBaseEntity *Create( char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner = nullptr );
  static void    MakeDormant() {};

  void SetTimer(int amount) const;
  int AddTimer(AMX* amx, int iWait, int iRepeat, char* sFunction, char* sParam, edict_t *pEntity);
  void CalcNextTimer();
  int DeleteTimer(int iTimer, int iForceDelete);
	int GetMaxVoteChoice() const;
  int GetPlayerVote(int iIndex);
  void SetPlayerVote(int iIndex, int iVote);
  BOOL StartVote(AMX* amx, char* sText, int iChoiceCount, int iBits, char* sFunction, char* sParam, edict_t *pEntity);
  static BOOL ValidTimerIndex(int iTimer);
  BOOL VoteAllowed() const;
  BOOL VoteInProgress() const;
 
  AMX* GetTimerAMX( int iTimer ) const
  { 
		  return ValidTimerIndex( iTimer ) ? timers[iTimer].amx : nullptr; 
  }
  
 private:
	int m_iMaxVoteChoice;
  int m_iNextTimer;
  int m_iNextVoteTime;
  int m_iVote;
  int m_iPlayerVotes[MAX_PLAYERS + 1];
  timer_struct timers[NUM_TIMERS];                                            
};

#endif //TIMER_H
