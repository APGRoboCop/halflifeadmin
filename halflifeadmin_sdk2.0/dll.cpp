#include "extdll.h"


/* LINUX COMPILE */
#ifdef _WIN32

typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (FAR *GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
#else
typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS *, int *);
#endif


#ifdef USE_MYSQL
	#include <mysql.h>
	#include <errmsg.h>
#endif

#include "users.h"

extern GETENTITYAPI other_GetEntityAPI;
extern GETNEWDLLFUNCTIONS other_GetNewDLLFunctions;

extern enginefuncs_t g_engfuncs;
extern globalvars_t  *gpGlobals;

extern CBaseEntity *pPlayer;
extern DLL_GLOBAL BOOL          g_fLoadUsers;    
extern DLL_GLOBAL BOOL          g_fLoadNicks;
extern DLL_GLOBAL BOOL          g_fLoadModels;
extern DLL_GLOBAL BOOL          g_fLoadIPs;  
extern mapcycle_t mapcycle;

extern vote_struct votes[VOTE_LENGTH];
extern BOOL g_fGameOver;

static DLL_FUNCTIONS other_gFunctionTable;
#ifdef WIN32
static UINT msg_timer;
#endif

extern player_struct nicks[NICKS_SIZE];

//cvar_t log_say = {"log_say","0",FCVAR_SERVER}; // not possible in the plugin model 
cvar_t users_file = {"users_file","0",FCVAR_EXTDLL}; 
cvar_t maps_file = {"maps_file","0",FCVAR_EXTDLL}; 

cvar_t kick_ratio = {"kick_ratio","0",FCVAR_EXTDLL}; 
cvar_t map_ratio = {"map_ratio","0",FCVAR_EXTDLL}; 
cvar_t vote_wait_kick = {"vote_freq_kick","0",FCVAR_EXTDLL}; 
cvar_t vote_wait_map = {"vote_freq_map","0",FCVAR_EXTDLL}; 

cvar_t nicks_file = {"nicks_file","0",FCVAR_EXTDLL}; 
cvar_t nicks_kick_msg = {"nicks_kick_msg","0",FCVAR_EXTDLL}; 

cvar_t ips_file = {"ips_file","ips.ini",FCVAR_EXTDLL};

cvar_t reserve_slots = {"reserve_slots","0",FCVAR_SERVER};
 cvar_t reserve_slots_msg = {"reserve_slots_msg","0",FCVAR_EXTDLL}; 

cvar_t default_access = {"default_access","0",FCVAR_EXTDLL}; 
cvar_t password_field = {"password_field","pw",FCVAR_EXTDLL}; 
cvar_t admin_debug = {"admin_debug","0",FCVAR_EXTDLL}; 

cvar_t alarm_message= {"alarm_message","0",FCVAR_EXTDLL};
cvar_t alarm_time= {"alarm_time","0",FCVAR_EXTDLL};

cvar_t use_regex= {"use_regex","1",FCVAR_EXTDLL};

cvar_t models_file  = {"models_file","0",FCVAR_EXTDLL}; 
cvar_t models_password_field  = {"models_password_field","pw-model",FCVAR_EXTDLL}; 
cvar_t models_kick_msg = {"models_kick_msg","0",FCVAR_EXTDLL}; 

#ifdef USE_MYSQL
	cvar_t mysql_host = {"mysql_host","0",FCVAR_EXTDLL}; 
	cvar_t mysql_user = {"mysql_user","0",FCVAR_EXTDLL}; 
	cvar_t mysql_pass = {"mysql_pass","0",FCVAR_EXTDLL}; 
	cvar_t mysql_dbtable_nicks = {"mysql_dbtable_nicks","0",FCVAR_EXTDLL}; 
	cvar_t mysql_dbtable_users = {"mysql_dbtable_users","0",FCVAR_EXTDLL};
 	cvar_t mysql_dbtable_models = {"mysql_dbtable_models","0",FCVAR_EXTDLL};
	cvar_t mysql_dbtable_ips = {"mysql_dbtable_ips","0",FCVAR_EXTDLL};

	MYSQL mysql;
	BOOL mysql_ok=FALSE;
#endif



#ifndef _WIN32
void sig_alarm(int) {
#else
	VOID CALLBACK sig_alarm( HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime ) {
#endif

char *msg=(char *)CVAR_GET_STRING("alarm_message");
	if ( strlen(msg) > 0 && !FStrEq(msg,"0")) {
 		UTIL_ClientPrintAll( HUD_PRINTCENTER,(const char *)msg);	
#ifdef WIN32
	KillTimer(0,msg_timer); // kill the old timer
	UINT time=((UINT)CVAR_GET_FLOAT("alarm_time"))*1000;
	if (time==0) return;
	msg_timer=SetTimer( 0, 0, time, (TIMERPROC)sig_alarm ); // set one that matches alarm_time

#else
	if ((int)CVAR_GET_FLOAT("alarm_time")==0) return;
	alarm((int)CVAR_GET_FLOAT("alarm_time"));
#endif
	}
}



void GameDLLInit( void )
{
        CVAR_REGISTER (&users_file);
    CVAR_REGISTER (&maps_file);
   CVAR_REGISTER (&kick_ratio);
  CVAR_REGISTER (&map_ratio);
  CVAR_REGISTER (&vote_wait_kick);
CVAR_REGISTER (&vote_wait_map);

   CVAR_REGISTER (&nicks_file);
   CVAR_REGISTER(&nicks_kick_msg);
 CVAR_REGISTER(&default_access);
CVAR_REGISTER(&ips_file);
 CVAR_REGISTER(&reserve_slots);
CVAR_REGISTER(&reserve_slots_msg);
 CVAR_REGISTER(&admin_debug);
 CVAR_REGISTER(&password_field);
CVAR_REGISTER(&alarm_message);
CVAR_REGISTER(&alarm_time);
CVAR_REGISTER(&use_regex);

CVAR_REGISTER(&models_password_field);
 CVAR_REGISTER (&models_file);
CVAR_REGISTER(&models_kick_msg);



#ifdef USE_MYSQL
	 CVAR_REGISTER(&mysql_host);
	 CVAR_REGISTER(&mysql_user);
	 CVAR_REGISTER(&mysql_pass);
	 CVAR_REGISTER(&mysql_dbtable_nicks);
	 CVAR_REGISTER(&mysql_dbtable_users);
	 CVAR_REGISTER(&mysql_dbtable_models);
	 CVAR_REGISTER(&mysql_dbtable_ips);
#endif


      	g_fLoadUsers = TRUE;
	g_fLoadNicks = TRUE;
	g_fLoadIPs   = TRUE;
	g_fLoadModels= TRUE; 

//	num_reserve=0;         
	other_gFunctionTable.pfnGameInit();
	memset(votes,0x0,2*sizeof(vote_struct));

#ifndef _WIN32
	signal(SIGALRM,sig_alarm);
	alarm(10); // need to hardcode a time here, 10 sec sounds good :)
#else
	msg_timer=SetTimer( 0, 0, 0x1000, (TIMERPROC)sig_alarm );
   
     	MSG msg = { 0, 0, 0, 0 };
 
     	// Get the message, and dispatch it.  This causes sig_alarm to be
     	// invoked. BIG windows kludge!!!
     	if ( GetMessage(&msg, 0, 0, 0) )
        	DispatchMessage( &msg );
#endif
}

int DispatchSpawn( edict_t *pent )
{
//PRECACHE_MODEL( (char *)CVAR_GET_STRING("default_model") ); 
 //  return (*other_gFunctionTable.pfnSpawn)(pent);
return(other_gFunctionTable.pfnSpawn(pent));
}

void DispatchThink( edict_t *pent )
{
  // (*other_gFunctionTable.pfnThink)(pent);
other_gFunctionTable.pfnThink(pent);
}

void DispatchUse( edict_t *pentUsed, edict_t *pentOther )
{
 //  (*other_gFunctionTable.pfnUse)(pentUsed, pentOther);
other_gFunctionTable.pfnUse(pentUsed, pentOther);
}

void DispatchTouch( edict_t *pentTouched, edict_t *pentOther )
{
//   (*other_gFunctionTable.pfnTouch)(pentTouched, pentOther);
other_gFunctionTable.pfnTouch(pentTouched, pentOther);

}

void DispatchBlocked( edict_t *pentBlocked, edict_t *pentOther )
{
 //  (*other_gFunctionTable.pfnBlocked)(pentBlocked, pentOther);
other_gFunctionTable.pfnBlocked(pentBlocked, pentOther);

}

void DispatchKeyValue( edict_t *pentKeyvalue, KeyValueData *pkvd )
{
 //  (*other_gFunctionTable.pfnKeyValue)(pentKeyvalue, pkvd);
other_gFunctionTable.pfnKeyValue(pentKeyvalue, pkvd);
}

void DispatchSave( edict_t *pent, SAVERESTOREDATA *pSaveData )
{
 //  (*other_gFunctionTable.pfnSave)(pent, pSaveData);
other_gFunctionTable.pfnSave(pent, pSaveData);
}

int DispatchRestore( edict_t *pent, SAVERESTOREDATA *pSaveData, int globalEntity )
{
 //  return (*other_gFunctionTable.pfnRestore)(pent, pSaveData, globalEntity);
return(other_gFunctionTable.pfnRestore(pent, pSaveData, globalEntity));
}

void DispatchObjectCollsionBox( edict_t *pent )
{
 //  (*other_gFunctionTable.pfnSetAbsBox)(pent);
other_gFunctionTable.pfnSetAbsBox(pent);

}

void SaveWriteFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
{
 //  (*other_gFunctionTable.pfnSaveWriteFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
other_gFunctionTable.pfnSaveWriteFields(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveReadFields( SAVERESTOREDATA *pSaveData, const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
{
//   (*other_gFunctionTable.pfnSaveReadFields)(pSaveData, pname, pBaseData, pFields, fieldCount);
other_gFunctionTable.pfnSaveReadFields(pSaveData, pname, pBaseData, pFields, fieldCount);
}

void SaveGlobalState( SAVERESTOREDATA *pSaveData )
{
 //  (*other_gFunctionTable.pfnSaveGlobalState)(pSaveData);
other_gFunctionTable.pfnSaveGlobalState(pSaveData);
}

void RestoreGlobalState( SAVERESTOREDATA *pSaveData )
{
  // (*other_gFunctionTable.pfnRestoreGlobalState)(pSaveData);
other_gFunctionTable.pfnRestoreGlobalState(pSaveData);

}

void ResetGlobalState( void )
{
 //  (*other_gFunctionTable.pfnResetGlobalState)();
other_gFunctionTable.pfnResetGlobalState;
}




BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ]  )
{ 
   
char ip[IP_SIZE+1], *p;
int  res_slots = (int)CVAR_GET_FLOAT("reserve_slots");

load_users();
load_nicks();
load_models();
load_ips();

#ifdef USE_MYSQL
	if (mysql_ok==FALSE)
		{
		mysql_ok = TRUE;
		mysql_connect(&mysql, CVAR_GET_STRING("mysql_host"), CVAR_GET_STRING("mysql_user"), CVAR_GET_STRING("mysql_pass"));

		if (mysql_errno(&mysql))
			{
			UTIL_LogPrintf("[ADMIN] MySQL Error: %s\n", mysql_error(&mysql));
			mysql_close(&mysql);
			mysql_ok = FALSE;
			}
		}
#endif

 if (check_nick((char *)pszName,pEntity)==0  ) {
	strcpy(szRejectReason,"Bad Nick Password. Enter the correct password or change names.");
		return FALSE;
 }

if (check_model(NULL,pEntity)==0  ) {
	strcpy(szRejectReason,"Bad Model Password. Enter the correct password or change your model.");
		return FALSE;
 }


 int total_num = 0;

    for (int i = 1; i <= gpGlobals->maxClients; i++) {
        CBaseEntity *pPlayer = UTIL_PlayerByIndex(i);

        if (pPlayer == NULL)
            continue;

        if (FNullEnt(pPlayer->pev))
            continue;

        if (FStrEq(STRING(pPlayer->pev->netname), ""))
            continue;

	if (FStrEq(STRING(pPlayer->pev->netname), pszName))
            continue;

        total_num++;
    }



/*if ( (int)CVAR_GET_FLOAT("admin_debug")!=0) {
		UTIL_LogPrintf("[ADMIN] DEBUG: total_num:%i maxClients:%i reserve_slots:%i\n",total_num,gpGlobals->maxClients,(int)CVAR_GET_FLOAT("reserve_slots"));
} 
*/

strncpy(ip,pszAddress,IP_SIZE);
ip[IP_SIZE] = '\0';
if ( p = strchr(ip,':') ) *p = '\0';


if ( (int)CVAR_GET_FLOAT("admin_debug")!=0) {
UTIL_LogPrintf( "[ADMIN] DEBUG ip: %s t=%d m=%d r=%d n=%s\n",ip,total_num,gpGlobals->maxClients,res_slots,pszName);
}


if ( res_slots ) {     // we maintain spare slots
    if ( gpGlobals->maxClients-total_num <= res_slots) {
	if ( priority_ip(ip) ) {
	    // ip ok, priority_ip spits match
	} else if ( auth_user((char *)pszName,LEV32768) ) {
	    UTIL_LogPrintf( "[ADMIN] reserve pass OK for %s\n", pszName);
	} else {
	    if (FStrEq(CVAR_GET_STRING("reserve_slots_msg"),"0"))
		sprintf(szRejectReason,
"[ADMIN-MOD] The Server Admin has reserved %d of the %d slots. None of the %d unreserved slots are left. Try later...\n",res_slots, gpGlobals->maxClients, gpGlobals->maxClients-res_slots);
	    else
	        strcpy(szRejectReason,CVAR_GET_STRING("reserve_slots_msg"));
		
	    return FALSE;
	}
    }
}


return(other_gFunctionTable.pfnClientConnect(pEntity, pszName, pszAddress, szRejectReason));
}

void ClientDisconnect( edict_t *pEntity )
{

other_gFunctionTable.pfnClientDisconnect(pEntity);

}

void ClientKill( edict_t *pEntity )
{

printf("Got here with a client kill\n");
 //  (*other_gFunctionTable.pfnClientKill)(pEntity);
other_gFunctionTable.pfnClientKill(pEntity);

}

void ClientPutInServer( edict_t *pEntity )
{
 //  (*other_gFunctionTable.pfnClientPutInServer)(pEntity);
other_gFunctionTable.pfnClientPutInServer(pEntity);

}


char NextMap[32];

void ClientCommand( edict_t *pEntity )
{
 const char *pcmd = CMD_ARGV(0);
 const char *pstr;    

   if ((pstr = strstr(pcmd, "admin_")) != NULL ) {      // they have an admin command for us!
                do_admin_commands(pEntity);
		return; // don't process any more commands!
        } 


// okay, maybe its a vote of some sort

  else if  ( FStrEq( pcmd, "menuselect") &&(votes[MAP_VOTE].enabled==1 || votes[KICK_VOTE].enabled==1) && (((votes[MAP_VOTE].time+MENU_SHOW)>(int)time(NULL) ) ||(votes[KICK_VOTE].time+MENU_SHOW)>(int)time(NULL))  ) {      // they have a vote for us
	
	if ( FStrEq( pcmd, "menuselect" ) && votes[KICK_VOTE].enabled==1 ) { // they have a kick vote to cast
                  if ( CMD_ARGC() < 2 )
                      return;
		float kick_ratio = (float)CVAR_GET_FLOAT("kick_ratio");

                 if (atoi(CMD_ARGV(1))==1) votes[KICK_VOTE].num_yes++;
		else votes[KICK_VOTE].num_no++;
	
		 if((float)votes[KICK_VOTE].num_yes*100 > (float)(votes[KICK_VOTE].total_num*100*kick_ratio)) {
			char szCommand[256];

#ifndef _WIN32
			snprintf(szCommand,256,"kick %s\n",votes[KICK_VOTE].name);
#else
		sprintf(szCommand,"kick %s\n",votes[KICK_VOTE].name);
#endif
			//printf("%s\n\n",szCommand);
			SERVER_COMMAND(szCommand);
			votes[KICK_VOTE].num_yes=votes[KICK_VOTE].num_no=0;
			votes[KICK_VOTE].enabled=0;

			UTIL_LogPrintf( "[ADMIN] kicked %s due to vote\n",votes[KICK_VOTE].name );
	 		UTIL_ClientPrintAll( HUD_PRINTTALK,(const char *) UTIL_VarArgs(
"Kicked user %s due to vote\n", votes[KICK_VOTE].name));    
		} else
	if ((votes[KICK_VOTE].num_yes+votes[KICK_VOTE].num_no) ==votes[KICK_VOTE].total_num) {
		      // select the item from the current menu
			UTIL_ClientPrintAll( HUD_PRINTTALK, (const char *)UTIL_VarArgs(
"Vote to kick %s got %i vote/s for yes (needed  %0.0f percent yes votes)\n",votes[KICK_VOTE].name,votes[KICK_VOTE].num_yes,(kick_ratio*100))); 
   	votes[KICK_VOTE].num_yes=votes[KICK_VOTE].num_no=0;
			votes[KICK_VOTE].enabled=0;
		
		}

              
		return; // ALFRED HERE
	} else if ( FStrEq( pcmd, "menuselect" ) && votes[MAP_VOTE].enabled==1 ) {
        	char szCommand[256];      
	   
		if ( CMD_ARGC() < 2 )
                      return;
		float map_ratio = (float)CVAR_GET_FLOAT("map_ratio");

                 if (atoi( CMD_ARGV(1))==1) votes[MAP_VOTE].num_yes++;
		else votes[MAP_VOTE].num_no++;


		if((float)votes[MAP_VOTE].num_yes*100 > (float)(votes[MAP_VOTE].total_num*100*map_ratio)) {
		strncpy(NextMap,votes[MAP_VOTE].name,32);

		if ( !IS_MAP_VALID(NextMap) ) {
			return;
		}

		votes[MAP_VOTE].enabled=0;
		votes[MAP_VOTE].num_yes=votes[MAP_VOTE].num_no=0;
	
		UTIL_ClientPrintAll( HUD_PRINTTALK,(const char *) UTIL_VarArgs("Changed map to %s due to vote\n", NextMap)); 
		UTIL_LogPrintf( "[ADMIN] Map change to %s due to vote\n" ,NextMap);


#ifndef _WIN32
			snprintf(szCommand,256,"changelevel %s\n", NextMap);
#else
	sprintf(szCommand,"changelevel %s\n",NextMap );
#endif

	SERVER_COMMAND(szCommand);

		}else
		if ((votes[MAP_VOTE].num_yes+votes[MAP_VOTE].num_no) ==votes[MAP_VOTE].total_num) {
                  // select the item from the current menu
			UTIL_ClientPrintAll( HUD_PRINTTALK,(const char *) UTIL_VarArgs(
"Vote to change map to %s got %i vote/s for yes (needed  %0.0f percent yes votes)\n",votes[MAP_VOTE].name,votes[MAP_VOTE].num_yes,(map_ratio*100)));    
			votes[MAP_VOTE].enabled=0;
		votes[MAP_VOTE].num_yes=votes[MAP_VOTE].num_no=0;
		}
	return; // ALFRED HERE
	}
}
     
if (  FStrEq( pcmd, "buy" ) || FStrEq( pcmd, "buyequip" ) ||  FStrEq( pcmd, "buyammo1" ) ||  FStrEq( pcmd, "buyammo2" )) {
	if ( FStrEq(STRING(gpGlobals->mapname),"es_frantic")) { // cs beta 6.0 es_frantic bug!
  		char *infobuffer=g_engfuncs.pfnGetInfoKeyBuffer(pEntity); 
		if  (FStrEq(g_engfuncs.pfnInfoKeyValue( infobuffer, "model" ),"arab") ||FStrEq(g_engfuncs.pfnInfoKeyValue( infobuffer, "model" ),"terror") || FStrEq(g_engfuncs.pfnInfoKeyValue( infobuffer, "model" ),"arctic") ) {
		CLIENT_PRINTF( pEntity, print_center,UTIL_VarArgs("The terrorists aren't able to buy anything\n"));
		return;	
	}
}

} // FStrEq buy   

// must be a command from the other dll
other_gFunctionTable.pfnClientCommand(pEntity);
}




void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer )
{
	char szCommand[256];
//	UTIL_LogPrintf(infobuffer);


// check that the name or model is not reserved
   if (  g_engfuncs.pfnInfoKeyValue( infobuffer, "name" )!=NULL ) {
	if ( STRING(pEntity->v.netname)[0]!='\0' ) { 


		if ( check_model(g_engfuncs.pfnInfoKeyValue( infobuffer, "model" ),pEntity)==0 ||  check_nick( g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ), pEntity)==0) {

		CBaseEntity *pKick=UTIL_PlayerByName(STRING(pEntity->v.netname));

	if (pKick==NULL) {
		UTIL_LogPrintf( "Unable to find player %s\n",g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ));
		return;
	}

		if (!GETPLAYERUSERID( pKick->edict())) { // whoops, can't find wonid
			UTIL_LogPrintf( "Unable to find player %s's WONID (%u)\n",g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ),GETPLAYERWONID( pKick->edict()));
	}
#ifndef _WIN32
		snprintf(szCommand,256,"kick # %i\n", GETPLAYERUSERID( pKick->edict()));
#else
		sprintf(szCommand,"kick # %i\n",GETPLAYERUSERID( pKick->edict()) );
#endif

		SERVER_COMMAND(szCommand);

		UTIL_LogPrintf( "[ADMIN] kicked %s due to bad nickname/model password\n",g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ) );		

		}

                 
	}       
}



other_gFunctionTable.pfnClientUserInfoChanged(pEntity, infobuffer);

}


void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
 
g_fLoadUsers=TRUE;
g_fLoadNicks=TRUE;
g_fLoadIPs=TRUE;
g_fLoadModels=TRUE;

other_gFunctionTable.pfnServerActivate(pEdictList, edictCount, clientMax);
}


void PlayerPreThink( edict_t *pEntity )
{
 //  (*other_gFunctionTable.pfnPlayerPreThink)(pEntity);
other_gFunctionTable.pfnPlayerPreThink(pEntity);
}

void PlayerPostThink( edict_t *pEntity )
{
 //  (*other_gFunctionTable.pfnPlayerPostThink)(pEntity);
other_gFunctionTable.pfnPlayerPostThink(pEntity);

}

void StartFrame( void )
{
  // (*other_gFunctionTable.pfnStartFrame)();
other_gFunctionTable.pfnStartFrame;
}

void ParmsNewLevel( void )
{
//   (*other_gFunctionTable.pfnParmsNewLevel)();

printf("\n\nIN HERE ******************************\n\n");
other_gFunctionTable.pfnParmsNewLevel;
}

void ParmsChangeLevel( void )
{
   //(*other_gFunctionTable.pfnParmsChangeLevel)();
other_gFunctionTable.pfnParmsChangeLevel;
}

const char *GetGameDescription( void )
{
 //  return (*other_gFunctionTable.pfnGetGameDescription)();
return(other_gFunctionTable.pfnGetGameDescription());
}

void PlayerCustomization( edict_t *pEntity, customization_t *pCust )
{
 //  (*other_gFunctionTable.pfnPlayerCustomization)(pEntity, pCust);
other_gFunctionTable.pfnPlayerCustomization(pEntity, pCust);

}

void SpectatorConnect( edict_t *pEntity )
{
//total_num++;
 //  (*other_gFunctionTable.pfnSpectatorConnect)(pEntity);
other_gFunctionTable.pfnSpectatorConnect(pEntity);

}

void SpectatorDisconnect( edict_t *pEntity )
{
//total_num--;
 //  (*other_gFunctionTable.pfnSpectatorDisconnect)(pEntity);
other_gFunctionTable.pfnSpectatorDisconnect(pEntity);

}

void SpectatorThink( edict_t *pEntity )
{
 //  (*other_gFunctionTable.pfnSpectatorThink)(pEntity);
other_gFunctionTable.pfnSpectatorThink(pEntity);
}


// NEW SDK 2.0 defns
void ServerDeactivate( void ) {
other_gFunctionTable.pfnServerDeactivate;
}

void Sys_Error( const char *error_string )
{
other_gFunctionTable.pfnSys_Error(error_string);
}

void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas )
{
other_gFunctionTable.pfnSetupVisibility(pViewEntity,pClient,pvs,pas);
}

void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd )
{
other_gFunctionTable.pfnUpdateClientData(ent,sendweapons,cd);
}


int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet )
{
return (other_gFunctionTable.pfnAddToFullPack(state,e,ent,host,hostflags,player,pSet));
}

void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs )
{
other_gFunctionTable.pfnCreateBaseline(player,eindex,baseline,entity,playermodelindex,player_mins,player_maxs);
}


void RegisterEncoders( void )
{
other_gFunctionTable.pfnRegisterEncoders;
}


int GetWeaponData( struct edict_s *player, struct weapon_data_s *info )
{
return(other_gFunctionTable.pfnGetWeaponData(player,info));
}

void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed )
{
other_gFunctionTable.pfnCmdStart(player,cmd,random_seed);
}

void CmdEnd ( const edict_t *player )
{
other_gFunctionTable.pfnCmdEnd(player);
}

int ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
return (other_gFunctionTable.pfnConnectionlessPacket(net_from,args,response_buffer,response_buffer_size));
}


int GetHullBounds( int hullnumber, float *mins, float *maxs )
{
return (other_gFunctionTable.pfnGetHullBounds(hullnumber,mins,maxs));
}

void CreateInstancedBaselines ( void )
{
other_gFunctionTable.pfnCreateInstancedBaselines;
}

int	InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message )
{
return (other_gFunctionTable.pfnInconsistentFile(player,filename,disconnect_message));
}


void PM_Move ( struct playermove_s *ppmove, int server )
{
other_gFunctionTable.pfnPM_Move(ppmove,server);
}

void PM_Init( struct playermove_s *ppmove )
{
other_gFunctionTable.pfnPM_Init(ppmove);
}


char PM_FindTextureType( char *name ) 
{
return(other_gFunctionTable.pfnPM_FindTextureType(name));
}


static DLL_FUNCTIONS gFunctionTable = 
{
	GameDLLInit,				//pfnGameInit
	DispatchSpawn,				//pfnSpawn
	DispatchThink,				//pfnThink
	DispatchUse,				//pfnUse
	DispatchTouch,				//pfnTouch
	DispatchBlocked,			//pfnBlocked
	DispatchKeyValue,			//pfnKeyValue
	DispatchSave,				//pfnSave
	DispatchRestore,			//pfnRestore
	DispatchObjectCollsionBox,	//pfnAbsBox

	SaveWriteFields,			//pfnSaveWriteFields
	SaveReadFields,				//pfnSaveReadFields

	SaveGlobalState,			//pfnSaveGlobalState
	RestoreGlobalState,			//pfnRestoreGlobalState
	ResetGlobalState,			//pfnResetGlobalState

	ClientConnect,				//pfnClientConnect
	ClientDisconnect,			//pfnClientDisconnect
	ClientKill,					//pfnClientKill
	ClientPutInServer,			//pfnClientPutInServer
	ClientCommand,				//pfnClientCommand
	ClientUserInfoChanged,		//pfnClientUserInfoChanged
	ServerActivate,				//pfnServerActivate
	ServerDeactivate,			//pfnServerDeactivate

	PlayerPreThink,				//pfnPlayerPreThink
	PlayerPostThink,			//pfnPlayerPostThink

	StartFrame,					//pfnStartFrame
	ParmsNewLevel,				//pfnParmsNewLevel
	ParmsChangeLevel,			//pfnParmsChangeLevel

	GetGameDescription,         //pfnGetGameDescription    Returns string describing current .dll game.
	PlayerCustomization,        //pfnPlayerCustomization   Notifies .dll of new customization for player.

	SpectatorConnect,			//pfnSpectatorConnect      Called when spectator joins server
	SpectatorDisconnect,        //pfnSpectatorDisconnect   Called when spectator leaves the server
	SpectatorThink,				//pfnSpectatorThink        Called when spectator sends a command packet (usercmd_t)
	
	Sys_Error,					//pfnSys_Error				Called when engine has encountered an error

	PM_Move,					//pfnPM_Move
	PM_Init,					//pfnPM_Init				Server version of player movement initialization
	PM_FindTextureType,			//pfnPM_FindTextureType
	
	SetupVisibility,			//pfnSetupVisibility        Set up PVS and PAS for networking for this client
	UpdateClientData,			//pfnUpdateClientData       Set up data sent only to specific client
	AddToFullPack,				//pfnAddToFullPack
	CreateBaseline,				//pfnCreateBaseline			Tweak entity baseline for network encoding, allows setup of player baselines, too.
	RegisterEncoders,			//pfnRegisterEncoders		Callbacks for network encoding
	GetWeaponData,				//pfnGetWeaponData
	CmdStart,					//pfnCmdStart
	CmdEnd,						//pfnCmdEnd
	ConnectionlessPacket,		//pfnConnectionlessPacket
	GetHullBounds,				//pfnGetHullBounds
	CreateInstancedBaselines,   //pfnCreateInstancedBaselines
	InconsistentFile,			//pfnInconsistentFile
};



/* LINUX COMPILE */
#ifdef _WIN32
#define EXPORT  _declspec( dllexport )
#else
#define EXPORT
#endif
/* END LINUX COMPILE */                                                                                           

#ifndef _WIN32
extern "C" EXPORT int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion );  

int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion )     
#else
extern "C" _declspec( dllexport) int GetEntityAPI( DLL_FUNCTIONS *pFunctionTable, int interfaceVersion )
#endif
{
   // check if engine's pointer is valid and version is correct...

   if ( !pFunctionTable || interfaceVersion != INTERFACE_VERSION ) {
      UTIL_LogPrintf( "[ADMIN] bad interface version\n" );
    return FALSE;
}

   if (!other_GetEntityAPI(&other_gFunctionTable, INTERFACE_VERSION)) {
   	UTIL_LogPrintf( "[ADMIN] unable to get function table\n" );
	  return FALSE;  // error initializing function table!!!
   }

	// copy the other dlls function table to ours
   memcpy(&gFunctionTable , &other_gFunctionTable, sizeof( DLL_FUNCTIONS ) );


// override any functions we want

   gFunctionTable.pfnClientCommand=ClientCommand; // add the admin_commands and parse the vote commands
   gFunctionTable.pfnClientConnect=ClientConnect; // load the users/nicks file on startup
   gFunctionTable.pfnGameInit=GameDLLInit; // register our extra cvars
   gFunctionTable.pfnClientUserInfoChanged=ClientUserInfoChanged; // check for name changes to reserved nicks
   gFunctionTable.pfnServerActivate=ServerActivate;// reset load files

   memcpy( pFunctionTable, &gFunctionTable, sizeof( DLL_FUNCTIONS ) );

   return TRUE;
}





