#include "extdll.h"

enginefuncs_t  g_engfuncs;
globalvars_t                               *gpGlobals;          


// this structure contains a list of supported mods and their dlls names
// To add support for another mod add an entry here, and add all the 
// exported entities to link_func.cpp
mod_struct_type mod_struct[] = {
{"cstrike","cstrike\\dlls\\mp.dll","cstrike/dlls/cs_i386.so"},
{"valve","valve\\dlls\\hl.dll","valve/dlls/hl_i386.so"},
{"action","action\\dlls\\mp.dll","action/dlls/mp_i386.so"},
{"tfc","tfc\\dlls\\tfc.dll","tfc/dlls/tfc_i386.so"},
{"phineas","phineas\\dlls\\phineas.dll","phineas/dlls/phineas_i386.so"},
{"bot","bot\\dlls\\bot.dll","bot/dlls/bot_i386.so"},
{"freeze","freeze\\dlls\\mp.dll","freeze/dlls/mp_i386.so"},
{"firearms","firearms\\dlls\\firearms.dll","firearms/dlls/fa_i386.so"},
{"goldeneye","goldeneye\\dlls\\mp.dll","goldeneye/dlls/golden_i386.so"},
{"oz","oz\\dlls\\mp.dll","Oz/dlls/mp_i386.so"},
{NULL,NULL,NULL}
};


extern DLL_GLOBAL BOOL          g_fLoadUsers;  

#ifdef _WIN32
typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
#else
typedef void (*GIVEFNPTRSTODLL) ( enginefuncs_t* , globalvars_t *  );  
#endif


#ifdef _WIN32

HINSTANCE h_Library = NULL;
typedef int (FAR *GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
#else

void *h_Library=NULL;
typedef int (*GETENTITYAPI)(DLL_FUNCTIONS *, int);
typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
typedef int (*GIVESPAWN)(char *);
#include <dlfcn.h>
#define GetProcAddress dlsym    
#endif


GETENTITYAPI other_GetEntityAPI;
GIVEFNPTRSTODLL other_GiveFnptrsToDll;
//GIVESPAWN other_Spawn;

#ifdef _WIN32
// Required DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
   if (fdwReason == DLL_PROCESS_ATTACH)
   {
   }
   else if (fdwReason == DLL_PROCESS_DETACH)
   {
      if (h_Library!=NULL)
         FreeLibrary(h_Library);
   }

   return TRUE;
}
#endif


static enginefuncs_t my_engfuncs;

#ifdef _WIN32  
void DLLEXPORT GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
#else
extern "C" void DLLEXPORT GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals ) 
#endif  
{
char dll_name[2048];
char game_dir[2048];

  memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
   gpGlobals = pGlobals;

if (h_Library !=NULL) { 
	UTIL_LogPrintf("PLUG-IN Already loaded\n");
}


GET_GAME_DIR(game_dir);

int length;
char *pFileList;

   int pos;
   char mod_name[32];
char *aFileList = pFileList = (char*)LOAD_FILE_FOR_ME("admin.ini", &length );

UTIL_LogPrintf( "[ADMIN] By Alfred Reynolds\n");
UTIL_LogPrintf( "[ADMIN] Version:%s\n", MOD_VERSION);

if (pFileList==NULL) {
	UTIL_LogPrintf("[ADMIN] Autodetecting dll to use\n");
	
 (*g_engfuncs.pfnGetGameDir)(game_dir);

   pos = strlen(game_dir) - 1;

   // scan backwards till first directory separator...
   while ((pos) && (game_dir[pos] != '/'))
      pos--;

   if (pos == 0)
   {
      	UTIL_LogPrintf("[ADMIN] Error in detecting mod name\n");
	exit(1);
   }

   pos++;
   strcpy(mod_name, &game_dir[pos]);
   int i=0;

  while(mod_struct[i].mod!=NULL) {
	if (!stricmp(mod_name,mod_struct[i].mod)) {
#ifdef _WIN32
		strcpy(dll_name,mod_struct[i].windir);
#else
		strcpy(dll_name,mod_struct[i].linuxdir);
#endif
		break;
	}
	i++;
  }

	if ( mod_struct[i].mod==NULL) {
		UTIL_LogPrintf("[ADMIN] Mod %s not supported\n",mod_name);
		exit(1);
	}
} else {
// we found admin.ini and will load that dll
	if ( pFileList && length )
	{
			char cBuf[1024]; // room for the name + password + access value
			int ret=sscanf( pFileList, "%s\n", cBuf);
			if ( ret>0) 
#ifndef _WIN32
				snprintf(dll_name,2048,"%s/%s",game_dir,cBuf);	
#else
				sprintf(dll_name,"%s\\%s",game_dir,cBuf);	
#endif
		FREE_FILE( aFileList );
	}
}



#ifdef _WIN32
 h_Library = LoadLibrary(dll_name); 

#else
h_Library = dlopen(dll_name, RTLD_NOW); 
#endif
  	UTIL_LogPrintf( "[ADMIN] Opening dll:%s\n", dll_name);

      if (h_Library == NULL)
      {
		UTIL_LogPrintf( "[ADMIN] Failed to load DLL\n");
        exit(1);
      }

      other_GetEntityAPI = (GETENTITYAPI)GetProcAddress(h_Library, "GetEntityAPI");

      if (other_GetEntityAPI == NULL)
	{
		UTIL_LogPrintf( "[ADMIN] ahh, no getentityapi stuff\n\n");
		UTIL_LogPrintf( "[ADMIN] Failed to load DLL\n");
        	exit(1);
	}

      other_GiveFnptrsToDll = (GIVEFNPTRSTODLL)GetProcAddress(h_Library, "GiveFnptrsToDll");

      if (other_GiveFnptrsToDll == NULL) {
			UTIL_LogPrintf( "[ADMIN] ahh, no fntptodlls stuff\n\n");
			UTIL_LogPrintf( "[ADMIN] Failed to load DLL\n");
       		exit(1);
	}

	    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
   gpGlobals = pGlobals;

  
 // give my version of the engine functions to the other DLL...

memcpy(&my_engfuncs,&g_engfuncs,sizeof(enginefuncs_t));

// You can override any engine functions here
//my_engfuncs.pfnGetGameDir=GetGameDir;


 other_GiveFnptrsToDll(&my_engfuncs, pGlobals);
}




