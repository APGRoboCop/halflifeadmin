/***
*
*	Copyright (c) 1999, 2000 Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
* $Id: cbase.cpp,v 1.4 2003/05/07 21:07:33 darope Exp $
*
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
****/
#include	"extdll.h"
#include	"util.h"
//#include	"cbase.h"
#include	"saverestore.h"
#include	"client.h"
#include	"decals.h"
//#include	"gamerules.h"
#include	"game.h"

extern "C" void PM_Move ( struct playermove_s *ppmove, int server );
extern "C" void PM_Init ( struct playermove_s *ppmove  );
extern "C" char PM_FindTextureType( char *name );

void EntvarsKeyvalue( entvars_t *pev, KeyValueData *pkvd );

//extern Vector VecBModelOrigin( entvars_t* pevBModel );
//extern DLL_GLOBAL Vector	g_vecAttackDir;
//extern DLL_GLOBAL int	g_iSkillLevel;


extern DLL_FUNCTIONS gFunctionTable;


// give health
int CBaseEntity :: TakeHealth( float flHealth, int bitsDamageType )
{
  
  return 1;
}

// inflict damage on this entity.  bitsDamageType indicates type of damage inflicted, ie: DMG_CRUSH

int CBaseEntity :: TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
  
  return 1;
}


void CBaseEntity :: Killed( entvars_t *pevAttacker, int iGib )
{
  
}


CBaseEntity *CBaseEntity::GetNextTarget( void )
{
  return NULL;
  
}

// Global Savedata for Delay
/*TYPEDESCRIPTION	CBaseEntity::m_SaveData[] = 
  {
  };
*/

int CBaseEntity::Save( CSave &save )
{
  
  return 0;
}

int CBaseEntity::Restore( CRestore &restore )
{
  
  return 0;
}


// Initialize absmin & absmax to the appropriate box
void SetObjectCollisionBox( entvars_t *pev )
{
  
}


void CBaseEntity::SetObjectCollisionBox( void )
{
}


int	CBaseEntity :: Intersects( CBaseEntity *pOther )
{
  
  return 1;
}

void CBaseEntity :: MakeDormant( void )
{
  
}

int CBaseEntity :: IsDormant( void )
{
  return 1;
}

BOOL CBaseEntity :: IsInWorld( void )
{
  
  return TRUE;
}

int CBaseEntity::ShouldToggle( USE_TYPE useType, BOOL currentState )
{
  
  return 1;
}


int	CBaseEntity :: DamageDecal( int bitsDamageType )
{
  
  return -1;
  
}





// NOTE: szName must be a pointer to constant memory, e.g. "monster_class" because the entity
// will keep a pointer to it after this call.
CBaseEntity * CBaseEntity::Create( char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner )
{
  
  //  CBaseEntity *pCheckEntity;
  CBaseEntity *pEntity;
  //	char szClassname[200];
  
  edict_t *pEnt;
  UTIL_LogPrintf( "[ADMIN] spawning\n");
  /*		pCheckEntity = UTIL_FindEntityByClassname(NULL, szName);
		if(pCheckEntity==NULL) {
		UTIL_LogPrintf("[ADMIN] spawn: The entity to be spawned was not cached at map load, so cannot be spawned.\n");
		return 0;
		}
  */
  
  pEnt = CREATE_NAMED_ENTITY(MAKE_STRING(szName));
  if(FNullEnt(pEnt)) {
    UTIL_LogPrintf( "[ADMIN] spawn: Null entity in spawn.\n");
    return 0;
  }
  
  pEntity = (CBaseEntity *)GET_PRIVATE(pEnt);
  pEntity->edict()->v.owner = NULL;
  pEntity->edict()->v.origin.x = 0;
  pEntity->edict()->v.origin.y = 0;
  pEntity->edict()->v.origin.z = 0;
  pEntity->edict()->v.angles.x = 0;
  pEntity->edict()->v.angles.y = 0;
  pEntity->edict()->v.angles.z = 0;
  
  //pEntity->Spawn();
  //DispatchSpawn( pEntity->edict() );
  
  return pEntity;
}


/*
  ================
  TraceAttack
  ================
*/
void CBaseEntity::TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
  
  
}

void CBaseEntity :: TraceBleed( float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
  
}

BOOL CBaseEntity :: FVisible ( CBaseEntity *pEntity ) { 
  return FALSE;
}

BOOL CBaseEntity :: FVisible ( const Vector &vecOrigin ) {
  
  return FALSE;
}



int DispatchSpawn( edict_t *pent ) {

  return (*gFunctionTable.pfnSpawn)(pent);
/*       CBaseEntity *pEntity = (CBaseEntity *)GET_PRIVATE(pent);
 
        if (pEntity)
        {
                // Initialize these or entities who don't link to the world won't have anyth$
                pEntity->pev->absmin = pEntity->pev->origin - Vector(1,1,1);
                pEntity->pev->absmax = pEntity->pev->origin + Vector(1,1,1);
        
                pEntity->Spawn(); 
 	}
                         
        return 0;
*/
}
  
void UTIL_SetSize( entvars_t *pev, const Vector &vecMin, const Vector &vecMax )
{
        SET_SIZE( ENT(pev), vecMin, vecMax ); 
}
