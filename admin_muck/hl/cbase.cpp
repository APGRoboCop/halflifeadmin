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
* $Id: cbase.cpp,v 1.2 2001/03/28 10:38:41 darope Exp $
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


//extern DLL_FUNCTIONS gFunctionTable;


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

