/*
 * ===========================================================================
 *
 * $Id: h_export.cpp,v 1.7 2004/08/21 19:28:41 darope Exp $
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
 */

#include <cstdio> // vsnprintf, etc
#include "extdll.h"
#include "users.h"
#include "amutil.h"
#include "amlibc.h"
#include "version.h"

#ifdef USE_METAMOD
#define SDK_UTIL_H // extdll.h already #include's util.h
#include "meta_api.h"
#endif

/*
#ifdef _WIN32
#define DLLEXPORT (__declspec ( export ))
#else
#define DLLEXPORT
#endif
*/

enginefuncs_t g_engfuncs;
globalvars_t* gpGlobals;

#ifdef CS_TEAMINFO
extern CGameRules* g_pGameRules;
extern FIV myCountTeams;
extern FII myCountTeamPlayers;
#endif

extern AMXINIT amx_Init;
extern AMXREGISTER amx_Register;
extern AMXFINDPUBLIC amx_FindPublic;
extern AMXEXEC amx_Exec;
extern AMXGETADDR amx_GetAddr;
extern AMXSTRLEN amx_StrLen;
extern AMXRAISEERROR amx_RaiseError;
extern AMXSETSTRING amx_SetString;
extern AMXGETSTRING amx_GetString;

extern DLL_GLOBAL BOOL g_fUsersLoaded;
extern auth_struct g_AuthArray[MAX_PLAYERS + 1];

DLL_GLOBAL enginefuncs_t my_engfuncs;
extern DLL_GLOBAL edict_t* pTimerEnt;

// CS Timelimit var
DLL_GLOBAL float* g_pflTimeLimit = nullptr;

// this structure contains a list of supported mods and their dlls names
// To add support for another mod add an entry here, and add all the
// exported entities to link_func.cpp + Added Support for Wizard Wars, Wanted!, TS, NS, BB, BG, ZP, ESF and Sven v5
// 1st/2nd Party Mods for SteamPipe - [APG]RoboCop[CL]

#ifndef USE_METAMOD
mod_struct_type mod_struct[] = { { "cstrike", "cstrike\\dlls\\mp.dll", "cstrike/dlls/cs.so" },
    { "czero", "czero\\dlls\\mp.dll", "cstrike/dlls/cs.so" },
	{ "valve", "valve\\dlls\\hl.dll", "valve/dlls/hl.so" },
	{ "dmc", "dmc\\dlls\\dmc.dll", "dmc/dlls/dmc.so" },
    { "bg", "bg\\dlls\\bg.dll", "ts/dlls/bg_i386.so" },
    { "ts", "ts\\dlls\\mp.dll", "ts/dlls/ts_i686.so" },
    { "ns", "ns\\dlls\\ns.dll", "ns/dlls/ns.so" },
	{ "zp", "zp\\dlls\\mp.dll", "zp/dlls/hl_i386.so" },
    { "brainbread", "brainbread\\dlls\\bb.dll", "brainbread/dlls/bb_i386.so" },
    { "action", "action\\dlls\\ahl.dll", "action/dlls/ahl_i386.so" },
    { "tfc", "tfc\\dlls\\tfc.dll", "tfc/dlls/tfc.so" },
    { "phineas", "phineas\\dlls\\phineas.dll", "phineas/dlls/phineas_i386.so" },
    { "bot", "bot\\dlls\\bot.dll", "bot/dlls/bot_i386.so" },
    { "freeze", "freeze\\dlls\\mp.dll", "freeze/dlls/mp_i386.so" },
    { "firearms", "firearms\\dlls\\firearms.dll", "firearms/dlls/fa_i386.so" },
    { "goldeneye", "goldeneye\\dlls\\mp.dll", "goldeneye/dlls/golden_i386.so" },
    { "oz", "oz\\dlls\\mp.dll", "oz/dlls/mp_i386.so" },
	//{ "svencoop", "svencoop\\dlls\\hl.dll", "svencoop/dlls/hl_i386.so" },
	//{ "svencoop", "svencoop\\dlls\\server.dll", "svencoop/dlls/server.so" }, // SC 5 support [APG]RoboCop[CL]
    { "si", "si\\dlls\\si.dll", "si/dlls/si.so" },
    { "frontline", "frontline\\dlls\\frontline.dll", "frontline/dlls/front_i386.so" },
    { "arg", "arg\\dlls\\hl.dll", "arg/dlls/arg_i386.so" },
    { "gangstawars", "gangstawars\\dlls\\hl.dll", "gangstawars/dlls/gansta_i386.so" },
    { "wizardwars", "wizardwars\\dlls\\wizardwars.dll", "wizardwars/dlls/wizardwars_i386.so" },
	{ "wizwars", "wizwars/dlls/hl.dll", "wizwars\\dlls\\mp_i386.so" },
    { "swarm", "swarm\\dlls\\swarm.dll", "swarm/dlls/swarm_i386.so" },
    { "gearbox", "gearbox\\dlls\\opfor.dll", "gearbox/dlls/opfor.so" },
    { "dod", "dod\\dlls\\hl.dll", "dod/dlls/dod.so" },
    { "ricochet", "ricochet\\dlls\\mp.dll", "ricochet/dlls/ricochet.so" },
    { "wasteland", "wasteland\\dlls\\hl.dll", "wasteland/dlls/whl_linux.so" },
    { "wantedsp", "wantedsp\\dlls\\wanted.dll", "wantedsp/dlls/wanted.so" },
	{ "wantedhl", "wantedhl\\dlls\\wanted.dll", "wantedhl/dlls/wanted.so" },
    { "esf_openbeta", "esf_openbeta\\dlls\\hl.dll" },
	{ "esf", "esf\\dlls\\hl.dll", "esf/dlls/hl_i386.so" },
	
	// Source Code acquired from Metamod-P-37 for allowing legacy support [APG]RoboCop[CL]
	/*	{"action",            "ahl"MODARCH".so",          "ahl.dll",           "Action Half-Life"},
	{"ag",                "ag"MODARCH".so",           "ag.dll",            "Adrenaline Gamer Steam"},
	{"ag3",               "hl"MODARCH".so",           "hl.dll",            "Adrenalinegamer 3.x"},
	{"aghl",              "ag"MODARCH".so",           "ag.dll",            "Adrenalinegamer 4.x"},
	{"arg",               "arg"MODARCH".so",          "hl.dll",            "Arg!"},
	{"asheep",            "hl"MODARCH".so",           "hl.dll",            "Azure Sheep"},
	{"hcfrenzy",          "hcfrenzy.so",              "hcfrenzy.dll",      "Headcrab Frenzy" },
	{"bdef",              "../cl_dlls/server.so",     "../cl_dlls/server.dll", "Base Defense [Modification]" },
	{"bdef",              "server.so",                     "server.dll",        "Base Defense [Steam Version]" },
	{"bg",                "bg"MODARCH".so",           "bg.dll",            "The Battle Grounds"},
	{"bhl",               "none",                     "bhl.dll",           "Brutal Half-Life" },
	{"bot",               "bot"MODARCH".so",          "bot.dll",           "Bot"},
	{"brainbread",        "bb"MODARCH".so",           "bb.dll",            "BrainBread"},
	{"bumpercars",        "hl"MODARCH".so",           "hl.dll",            "Bumper Cars"},
	{"buzzybots",         "bb"MODARCH".so",           "bb.dll",            "BuzzyBots"},
	{"ckf3",              "none",                     "mp.dll",            "Chicken Fortress 3" },
	{"cs13",              "cs"MODARCH".so",           "mp.dll",            "Counter-Strike 1.3"},
	{"cstrike",           "cs"MODARCH".so",           "mp.dll",            "Counter-Strike"},
	{"csv15",             "cs"MODARCH".so",           "mp.dll",            "CS 1.5 for Steam"},
	{"czero",             "cs"MODARCH".so",           "mp.dll",            "Counter-Strike:Condition Zero"},
	{"dcrisis",           "dc"MODARCH".so",           "dc.dll",            "Desert Crisis"},
	{"dmc",               "dmc"MODARCH".so",          "dmc.dll",           "Deathmatch Classic"},
	{"dod",               "dod"MODARCH".so",          "dod.dll",           "Day of Defeat"},
	{"dpb",               "pb.i386.so",               "pb.dll",            "Digital Paintball"},
	{"dragonmodz",        "hl"MODARCH".so",           "mp.dll",            "Dragon Mod Z"},
	{"esf",               "hl"MODARCH".so",           "hl.dll",            "Earth's Special Forces"},
	{"existence",         "ex"MODARCH".so",           "existence.dll",     "Existence"},
	{"firearms",          "fa"MODARCH".so",           "firearms.dll",      "Firearms"},
	{"firearms25",        "fa"MODARCH".so",           "firearms.dll",      "Retro Firearms"},
	{"freeze",            "mp"MODARCH".so",           "mp.dll",            "Freeze"},
	{"frontline",         "front"MODARCH".so",        "frontline.dll",     "Frontline Force"},
	{"gangstawars",       "gangsta"MODARCH".so",      "gwars27.dll",       "Gangsta Wars"},
	{"gangwars",          "mp"MODARCH".so",           "mp.dll",            "Gangwars"},
	{"gearbox",           "opfor"MODARCH".so",        "opfor.dll",         "Opposing Force"},
	{"globalwarfare",     "gw"MODARCH".so",           "mp.dll",            "Global Warfare"},
	{"goldeneye",         "golden"MODARCH".so",       "mp.dll",            "Goldeneye"},
	{"hl15we",            "hl"MODARCH".so",           "hl.dll",            "Half-Life 1.5: Weapon Edition"},
	{"HLAinGOLDSrc",      "none",                     "hl.dll",            "Half-Life Alpha in GOLDSrc"},
	{"hlrally",           "hlr"MODARCH".so",          "hlrally.dll",       "HL-Rally"},
	{"holywars",          "hl"MODARCH".so",           "holywars.dll",      "Holy Wars"},
	{"hostileintent",     "hl"MODARCH".so",           "hl.dll",            "Hostile Intent"},
	{"ios",               "ios"MODARCH".so",          "ios.dll",           "International Online Soccer"},
	{"judgedm",           "judge"MODARCH".so",        "mp.dll",            "Judgement"},
	{"kanonball",         "hl"MODARCH".so",           "kanonball.dll",     "Kanonball"},
	{"monkeystrike",      "ms"MODARCH".so",           "monkey.dll",        "Monkeystrike"},
	{"MorbidPR",          "morbid"MODARCH".so",       "morbid.dll",        "Morbid Inclination"},
	{"movein",            "hl"MODARCH".so",           "hl.dll",            "Move In!"},
	{"msc",               "none",                     "ms.dll",            "Master Sword Continued" },
	{"ns",                "ns"MODARCH".so",           "ns.dll",            "Natural Selection"},
	{"nsp",               "ns"MODARCH".so",           "ns.dll",            "Natural Selection Beta"},
	{"oel",               "hl"MODARCH".so",           "hl.dll",            "OeL Half-Life"},
	{"og",                "og"MODARCH".so",           "og.dll",            "Over Ground"},
	{"ol",                "ol"MODARCH".so",           "hl.dll",            "Outlawsmod"},
	{"ops1942",           "spirit"MODARCH".so",       "spirit.dll",        "Operations 1942"},
	{"osjb",              "osjb"MODARCH".so",         "jail.dll",          "Open-Source Jailbreak"},
	{"outbreak",          "none",                     "hl.dll",            "Out Break"},
	{"oz",                "mp"MODARCH".so",           "mp.dll",            "Oz Deathmatch"},
	{"paintball",         "pb"MODARCH".so",           "mp.dll",            "Paintball"},
	{"penemy",            "pe"MODARCH".so",           "pe.dll",            "Public Enemy"},
	{"phineas",           "phineas"MODARCH".so",      "phineas.dll",       "Phineas Bot"},
	{"ponreturn",         "ponr"MODARCH".so",         "mp.dll",            "Point of No Return"},
	{"pvk",               "hl"MODARCH".so",           "hl.dll",            "Pirates, Vikings and Knights"},
	{"rc2",               "rc2"MODARCH".so",          "rc2.dll",           "Rocket Crowbar 2"},
	{"recbb2",            "recb"MODARCH".so",         "recb.dll",          "Resident Evil : Cold Blood" },
	{"retrocs",           "rcs"MODARCH".so",          "rcs.dll",           "Retro Counter-Strike"},
	{"rewolf",            "hl"MODARCH".so",           "gunman.dll",        "Gunman Chronicles"},
	{"ricochet",          "ricochet"MODARCH".so",     "mp.dll",            "Ricochet"},
	{"rockcrowbar",       "rc"MODARCH".so",           "rc.dll",            "Rocket Crowbar"},
	{"rspecies",          "hl"MODARCH".so",           "hl.dll",            "Rival Species"},
	{"scihunt",           "shunt.so",                 "shunt.dll",         "Scientist Hunt"},
	{"sdm",               "sdmmod"MODARCH".so",       "sdmmod.dll",        "Special Death Match"},
	{"Ship",              "ship"MODARCH".so",         "ship.dll",          "The Ship"},
	{"si",                "si"MODARCH".so",           "si.dll",            "Science & Industry"},
	{"snow",              "snow"MODARCH".so",         "snow.dll",          "Snow-War"},
	{"stargatetc",        "hl"MODARCH".so",           "hl.dll",            "StargateTC"},
	{"svencoop",          "hl"MODARCH".so",           "hl.dll",            "Sven Coop [Modification]" },
	{"svencoop",          "server.so",                "server.dll",        "Sven Coop [Steam Version]" },
	{"swarm",             "swarm"MODARCH".so",        "swarm.dll",         "Swarm"},
	{"tfc",               "tfc"MODARCH".so",          "tfc.dll",           "Team Fortress Classic"},
	{"thewastes",         "thewastes"MODARCH".so",    "thewastes.dll",     "The Wastes"},
	{"timeless",          "pt"MODARCH".so",           "timeless.dll",      "Project Timeless"},
	{"tod",               "hl"MODARCH".so",           "hl.dll",            "Tour of Duty"},
	{"trainhunters",      "th"MODARCH".so",           "th.dll",            "Train Hunters"},
	{"trevenge",          "trevenge.so",              "trevenge.dll",      "The Terrorist Revenge"},
	{"TS",                "ts"MODARCH".so",           "mp.dll",            "The Specialists"},
	{"tt",                "tt"MODARCH".so",           "tt.dll",            "The Trenches"},
	{"underworld",        "uw"MODARCH".so",           "uw.dll",            "Underworld Bloodline"},
	{"valve",             "hl"MODARCH".so",           "hl.dll",            "Half-Life Deathmatch"},
	{"vs",                "vs"MODARCH".so",           "mp.dll",            "VampireSlayer"},
	{"wantedhl",          "hl"MODARCH".so",           "wanted.dll",        "Wanted!"},
	{"wasteland",         "whl_linux.so",             "mp.dll",            "Wasteland"},
	{"weapon_wars",       "ww"MODARCH".so",           "hl.dll",            "Weapon Wars"},
	{"wizwars",           "mp"MODARCH".so",           "hl.dll",            "Wizard Wars"},
	{"wormshl",           "wormshl_i586.so",          "wormshl.dll",       "WormsHL"},
	{"zp",                "none",                     "mp.dll",            "Zombie Panic"},
*/
	
    //???? what is the win32 dll name?
    { nullptr, nullptr, nullptr } };
#endif // !USE_METAMOD

/*
#ifdef _WIN32
typedef void (DLLEXPORT *GIVEFNPTRSTODLL)(enginefuncs_t *, globalvars_t *);
#else
typedef void (*GIVEFNPTRSTODLL) ( enginefuncs_t* , globalvars_t *  );
#endif
*/

#ifdef _WIN32
HINSTANCE h_Library = nullptr;
HINSTANCE h_AMX = nullptr;
HINSTANCE CS_Library = nullptr;
#else
void* h_Library = nullptr;
void* h_AMX = nullptr;
void* CS_Library = nullptr;
#include <dlfcn.h>
#define GetProcAddress dlsym
#endif

#ifndef USE_METAMOD
#ifdef _WIN32
typedef int(FAR* GETENTITYAPI)(DLL_FUNCTIONS*, int);
typedef void(__declspec(dllexport)* GIVEFNPTRSTODLL)(enginefuncs_t*, globalvars_t*);
typedef int(__declspec(dllexport)* GETENTITYAPI2)(DLL_FUNCTIONS*, int*);
typedef int(FAR* GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS*, int*);
#else
typedef int (*GETENTITYAPI)(DLL_FUNCTIONS*, int);
typedef void (*GIVEFNPTRSTODLL)(enginefuncs_t*, globalvars_t*);
// typedef int (*GIVESPAWN)(char *);
typedef int (*GETENTITYAPI2)(DLL_FUNCTIONS*, int*);
typedef int (*GETNEWDLLFUNCTIONS)(NEW_DLL_FUNCTIONS*, int*);
#endif
GETENTITYAPI other_GetEntityAPI;
GIVEFNPTRSTODLL other_GiveFnptrsToDll;
GETENTITYAPI2 other_GetEntityAPI2;
GETNEWDLLFUNCTIONS other_GetNewDLLFunctions = nullptr;
#endif

#ifdef _WIN32
// Required DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if(fdwReason == DLL_PROCESS_ATTACH) {
    } else if(fdwReason == DLL_PROCESS_DETACH) {
        if(h_Library != nullptr)
            FreeLibrary(h_Library);
    }
    return TRUE;
}
#endif

#ifdef WITH_LOGPARSING
/* Engine functions we need to catch */
void am_AlertMessage(ALERT_TYPE atype, char* szFmt, ...)
{

    plugin_result eResult = PLUGIN_CONTINUE;
    int iParse = (int)CVAR_GET_FLOAT("admin_parse_logs");
// If we're using metamod, we don't need to worry about sending
// the alert message on to the other DLL; MM does that for us.
// If we're not using metamod, we _do_ need to do this.
#ifdef USE_METAMOD
    if(iParse == 0) {
        RETURN_META(MRES_IGNORED);
    }
#endif

    char buf[MAX_STRBUF_LEN];
    va_list ap;
    va_start(ap, szFmt);
    vsnprintf(buf, MAX_STRBUF_LEN, szFmt, ap);
    va_end(ap);
    if(iParse != 0) {
        eResult = HandleLog(buf);
    }

#ifdef USE_METAMOD
    if(eResult == PLUGIN_HANDLED) {
        RETURN_META(MRES_HANDLED);
    } else {
        RETURN_META(MRES_IGNORED);
    }
#else
    ALERT(atype, buf);
#endif
}
#endif

edict_t* am_FindEntityInSphere(edict_t* pEdictStartSearchAfter, const float* org, float rad)
{
    edict_t* ent;
/* When using metamod, this function should be passed as a "_Post"
 * function, and metamod handles the initial engine call. */

#ifdef USE_METAMOD
    ent = META_RESULT_ORIG_RET(edict_t*);
#else
    ent = FIND_ENTITY_IN_SPHERE(pEdictStartSearchAfter, org, rad);
#endif

    /* If this found the timer ent, don't let game DLL have this; find the
     * next ent. */

    if(ent == pTimerEnt) {
        DEBUG_LOG(5, ("Hiding timer entity from FindEntityInSphere."));
        ent = FIND_ENTITY_IN_SPHERE(ent, org, rad);
        DEBUG_LOG(5, ("Returning next entity: %s.", ent ? STRING(ent->v.classname) : "nil"));
#ifdef USE_METAMOD
        RETURN_META_VALUE(MRES_OVERRIDE, ent);
#endif
    }
#ifdef USE_METAMOD
    RETURN_META_VALUE(MRES_IGNORED, ent);
#else
    return (ent);
#endif
}

/* Engine functions we need to catch */
edict_t* am_EntitiesInPVS(edict_t* pplayer)
{
    edict_t* ent;
/* When using metamod, this function should be passed as a "_Post"
 * function, and metamod handles the initial engine call. */

#ifdef USE_METAMOD
    ent = META_RESULT_ORIG_RET(edict_t*);
#else
    ent = UTIL_EntitiesInPVS(pplayer);
#endif

    /* If this found the timer ent, don't let game DLL have this; find the
     * next ent. */

    if(ent == pTimerEnt) {

        DEBUG_LOG(5, ("Hiding timer entity from FindEntityInPVS."));
        ent = UTIL_EntitiesInPVS(ent);
        DEBUG_LOG(5, ("Returning next entity: %s.", ent ? STRING(ent->v.classname) : "nil"));
#ifdef USE_METAMOD
        RETURN_META_VALUE(MRES_OVERRIDE, ent);
#endif
    }
#ifdef USE_METAMOD
    RETURN_META_VALUE(MRES_IGNORED, ent);
#else
    return (ent);
#endif
}

/* Engine functions we need to catch */
edict_t* am_FindEntityByVars(struct entvars_s* pvars)
{
    edict_t* ent;
/* When using metamod, this function should be passed as a "_Post"
 * function, and metamod handles the initial engine call. */

#ifdef USE_METAMOD
    ent = META_RESULT_ORIG_RET(edict_t*);
#else
    ent = (*g_engfuncs.pfnFindEntityByVars)(pvars);
#endif

    /* If this found the timer ent, don't let game DLL have this; find the
     * next ent. */

    if(ent == pTimerEnt) {
        DEBUG_LOG(5, ("Hiding timer entity from FindEntityByVars."));
        ent = nullptr;
        DEBUG_LOG(5, ("Returning next entity: %s.", ent ? STRING(ent->v.classname) : "nil"));
#ifdef USE_METAMOD
        RETURN_META_VALUE(MRES_OVERRIDE, ent);
#endif
    }
#ifdef USE_METAMOD
    RETURN_META_VALUE(MRES_IGNORED, ent);
#else
    return (ent);
#endif
}

//
// CVars registered here
//
extern cvar_t admin_devel;
extern cvar_t admin_debug;
extern cvar_t admin_bot_protection;
// extern cvar_t amv_autoban;
extern cvar_t reserve_slots;
extern cvar_t amv_hide_reserved_slots;
// extern cvar_t amv_keyfiles;

#ifdef _WIN32
void WINAPI GiveFnptrsToDll(enginefuncs_t* pengfuncsFromEngine, globalvars_t* pGlobals)
{
#else
extern "C" void DLLEXPORT GiveFnptrsToDll(enginefuncs_t* pengfuncsFromEngine, globalvars_t* pGlobals)
{
#endif
    //char game_dir[2048];
    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;

    // This is the very first function called by either the engine or metamod.
    // We do some init stuff here for things that we need right from the beginning.
    // Register the devel logging cvar and get direct access to it.
    CVAR_REGISTER(&admin_devel);
    CVAR_REGISTER(&admin_bot_protection);
    CVAR_REGISTER(&admin_debug);
    // CVAR_REGISTER(&amv_autoban);
    CVAR_REGISTER(&reserve_slots);
    CVAR_REGISTER(&amv_hide_reserved_slots);
    // CVAR_REGISTER(&amv_keyfiles);
    ptAM_devel = nullptr;
    ptAM_devel = CVAR_GET_POINTER("admin_devel");
    ptAM_debug = nullptr;
    ptAM_debug = CVAR_GET_POINTER("admin_debug");
    // ptAM_autoban = 0;
    // ptAM_autoban = CVAR_GET_POINTER( "amv_autoban" );
    ptAM_botProtection = nullptr;
    ptAM_botProtection = CVAR_GET_POINTER("admin_bot_protection");
    ptAM_reserve_slots = nullptr;
    ptAM_reserve_slots = CVAR_GET_POINTER("reserve_slots");
    ptAM_hide_reserved_slots = nullptr;
    ptAM_hide_reserved_slots = CVAR_GET_POINTER("amv_hide_reserved_slots");
    // ptAM_keyfiles = 0;
    // ptAM_keyfiles = CVAR_GET_POINTER( "amv_keyfiles" );

    // Greetings, earthling!
    UTIL_LogPrintf("[ADMIN] Admin Mod, the HLDS admin's choice\n");
    UTIL_LogPrintf("[ADMIN] by Alfred Reynolds (http://www.adminmod.org)\n");
    UTIL_LogPrintf("[ADMIN] Version: %s\n", MOD_VERSION);
    UTIL_LogPrintf("[ADMIN] Compiled: %s %s (%s)\n", COMPILE_DTTM, TZONE, OPT_TYPE);
    /*
    // If we haven't read server.cfg yet, allow devel logging
    // to be enabled with the "+developer" cmdline option.
    if ( ((int) CVAR_GET_FLOAT("developer") != 0) && ((int) CVAR_GET_FLOAT("amv_devel") == 0) ) {
      CVAR_SET_FLOAT("amv_devel", 2.0);
      DEVEL_LOG(2, ("+developer was set, enabling devel logging level 2"));
    }  // if
    */

    char dll_name[2048];
    char mod_name[32];

/*#ifndef EXTMM
    GET_GAME_DIR(game_dir);
    am_strncpy(mod_name, GetModDir(), 32);

    if(strcasecmp(mod_name, "cstrike") == 0) {
#ifdef _WIN32
        snprintf(dll_name, 2048, "%s\\dlls\\%s.dll", game_dir);
#else
        snprintf(dll_name, 2048, "%s/dlls/%s.so", game_dir);
#endif
    } else {
#ifdef _WIN32 
        snprintf(dll_name, 2048, "cstrike\\dlls\\cs.dll", game_dir);
#else
        snprintf(dll_name, 2048, "cstrike/dlls/cs.so", game_dir);
#endif
	}*/
#ifndef USE_METAMOD
    int length;
    char* pFileList;
    if(h_Library != nullptr) {
        UTIL_LogPrintf("PLUG-IN Already loaded\n");
    }

    GET_GAME_DIR(game_dir);
    char* aFileList = pFileList = (char*)LOAD_FILE_FOR_ME("admin.ini", &length);

    if(pFileList == nullptr) {
        UTIL_LogPrintf("[ADMIN] Autodetecting dll to use\n");
        UTIL_LogPrintf("[ADMIN] Mod Dir: %s\n", GetModDir());
        strcpy(mod_name, GetModDir());

        int i = 0;
        while(mod_struct[i].mod != nullptr) {
            if(!stricmp(mod_name, mod_struct[i].mod)) {
#ifdef _WIN32
                strcpy(dll_name, mod_struct[i].windir);
#else
                strcpy(dll_name, mod_struct[i].linuxdir);
#endif
                break;
            }
            i++;
        }
        if(mod_struct[i].mod == nullptr) {
            UTIL_LogPrintf("[ADMIN] ERROR: Mod %s not supported\n", mod_name);
            am_exit(1);
        }
    } else {
        // we found admin.ini and will load that dll
        if(pFileList && length) {
            char cBuf[1024]; // room for the name + password + access value
            int ret = sscanf(pFileList, "%s\n", cBuf);
            if(ret > 0)
#ifndef _WIN32
                snprintf(dll_name, 2048, "%s/%s", game_dir, cBuf);
#else
                sprintf(dll_name, "%s\\%s", game_dir, cBuf);
#endif
            FREE_FILE(aFileList);
        }
    }
#endif

    BOOL bSymbolLoaded = FALSE;
    h_Library = nullptr;

#ifndef USE_METAMOD
    GET_GAME_DIR(game_dir);

    char script_dll[2048];
#ifdef _WIN32
    sprintf(script_dll, "%s\\%s", game_dir, SCRIPT_DLL_WIN);
    h_AMX = LoadLibrary(script_dll);
#else
    snprintf(script_dll, 2048, "%s/%s", game_dir, SCRIPT_DLL);
    h_AMX = dlopen(script_dll, RTLD_NOW);
#endif
    if(h_AMX == nullptr) {
#ifdef WIN32
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (%s) %s\n", script_dll, GetLastError());
#else
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (%s) %s\n", script_dll, dlerror());
#endif
        am_exit(1);
    }

    amx_Init = (AMXINIT)GetProcAddress(h_AMX, "amx_Init");
    if(amx_Init == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_Init)\n");
        am_exit(1);
    }

    amx_Register = (AMXREGISTER)GetProcAddress(h_AMX, "amx_Register");
    if(amx_Register == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_Register)\n");
        am_exit(1);
    }

    amx_FindPublic = (AMXFINDPUBLIC)GetProcAddress(h_AMX, "amx_FindPublic");
    if(amx_FindPublic == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_FindPublic)\n");
        am_exit(1);
    }

    amx_Exec = (AMXEXEC)GetProcAddress(h_AMX, "amx_Exec");
    if(amx_Exec == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_Exec)\n");
        am_exit(1);
    }

    amx_GetAddr = (AMXGETADDR)GetProcAddress(h_AMX, "amx_GetAddr");
    if(amx_GetAddr == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_GetAddr)\n");
        am_exit(1);
    }

    amx_StrLen = (AMXSTRLEN)GetProcAddress(h_AMX, "amx_StrLen");
    if(amx_StrLen == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_StrLen)\n");
        am_exit(1);
    }

    amx_RaiseError = (AMXRAISEERROR)GetProcAddress(h_AMX, "amx_RaiseError");
    if(amx_RaiseError == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_RaiseError)\n");
        am_exit(1);
    }

    amx_SetString = (AMXSETSTRING)GetProcAddress(h_AMX, "amx_SetString");
    if(amx_SetString == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_SetString)\n");
        am_exit(1);
    }

    amx_GetString = (AMXGETSTRING)GetProcAddress(h_AMX, "amx_GetString");
    if(amx_GetString == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Couldn't load scripting engine (amx_GetString)\n");
        am_exit(1);
    }

#ifdef _WIN32
    h_Library = LoadLibrary(dll_name);
#else
    h_Library = dlopen(dll_name, RTLD_NOW);
#endif
    UTIL_LogPrintf("[ADMIN] Opening dll:%s\n", dll_name);
    if(h_Library == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: Failed to load DLL\n");
        am_exit(1);
    }
    other_GetEntityAPI = (GETENTITYAPI)GetProcAddress(h_Library, "GetEntityAPI");
    if(other_GetEntityAPI == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: no getentityapi stuff\n\n");
        UTIL_LogPrintf("[ADMIN] ERROR: Failed to load DLL\n");
        am_exit(1);
    }
    bSymbolLoaded = TRUE;
    other_GetEntityAPI2 = (GETENTITYAPI2)GetProcAddress(h_Library, "GetEntityAPI2");
    if(other_GetEntityAPI2 == nullptr) {
        UTIL_LogPrintf("[ADMIN] No getentityapi2 stuff (sdk2.0).  This can be ignored.\n\n");
    }

    other_GetNewDLLFunctions = (GETNEWDLLFUNCTIONS)GetProcAddress(h_Library, "GetNewDLLFunctions");
    if(other_GetNewDLLFunctions == nullptr) {
        UTIL_LogPrintf("[ADMIN] No GetNewDLLFunctions stuff (sdk2.0).  This can be ignored.\n\n");
    }

    other_GiveFnptrsToDll = (GIVEFNPTRSTODLL)GetProcAddress(h_Library, "GiveFnptrsToDll");
    if(other_GiveFnptrsToDll == nullptr) {
        UTIL_LogPrintf("[ADMIN] ERROR: no fntptodlls stuff\n\n");
        UTIL_LogPrintf("[ADMIN] ERROR: Failed to load DLL\n");
        am_exit(1);
    }
#endif

// #define EXTDEBUG
#if(!defined EXTMM)

    if(strcmp(mod_name, "cstrike") == 0) {
#if(defined LINUX) || (defined EXTDEBUG)
        UTIL_LogPrintf("[ADMIN] INFO: Loading extended Counter-Strike functionality.\n");
#endif

        if(CS_Library == nullptr) {
#ifdef _WIN32
            CS_Library = LoadLibrary(dll_name);
#else
            CS_Library = dlopen(dll_name, RTLD_NOW);
#endif
        } // if

        if(CS_Library == nullptr) {
#if(defined LINUX) || (defined EXTDEBUG)
            UTIL_LogPrintf("[ADMIN] INFO: Unable to load Counter-Strike DLL for extended functionality.\n");
#endif
        } else {

            //char* pcError = nullptr;

            g_pflTimeLimit = reinterpret_cast<float*>(GetProcAddress(CS_Library, "g_flTimeLimit"));
            if(g_pflTimeLimit == nullptr) {
#if(defined LINUX) || (defined EXTDEBUG)
                UTIL_LogPrintf(
                    "[ADMIN] INFO: Could not find CS' TimeLimit. AM's timeleft may differ from CS' timeleft.\n");
#endif // LINUX
            } else {
                bSymbolLoaded = TRUE;
#if(defined LINUX) || (defined EXTDEBUG)
                UTIL_LogPrintf("[ADMIN] INFO: Successfully attached to CS TimeLimit.\n");
#endif        // LINUX
            } // if-else

#ifdef CS_TEAMINFO
            g_pGameRules = (CGameRules*)GetProcAddress(CS_Library, "g_pGameRules");
            if(g_pGameRules == 0) {
#if(defined LINUX) || (defined EXTDEBUG)
                UTIL_LogPrintf("[ADMIN] INFO: No GameRules found. This can be ignored\n");
#endif
            } else {
                bSymbolLoaded = TRUE;
            } // if-else
            myCountTeams = (FIV)GetProcAddress(CS_Library, "CountTeams__Fv");
            if(myCountTeams == 0) {
#if(defined LINUX) || (defined EXTDEBUG)
                UTIL_LogPrintf("[ADMIN] INFO: Could not attach to CountTeams(). This can be ignored.\n");
#endif
            } else {
                bSymbolLoaded = TRUE;
            } // if-else
            myCountTeamPlayers = (FII)GetProcAddress(CS_Library, "CountTeamPlayers__Fi");
            if(myCountTeamPlayers == 0) {
#if(defined LINUX) || (defined EXTDEBUG)
                UTIL_LogPrintf(
                    "[ADMIN] INFO: Could not attach to CountTeamPlayers(). Team counts may be inaccurate.\n");
#endif
            } else {
                bSymbolLoaded = TRUE;
            } // if-else
#endif        // CS_TEAMINFO

            if(!bSymbolLoaded) {
#ifdef WIN32
                FreeLibrary(CS_Library);
#else
                dlclose(CS_Library);
#endif
#if(defined LINUX) || (defined EXTDEBUG)
                UTIL_LogPrintf("[ADMIN] INFO: Could not enable extended CS functions. Standard Admin Mod functionality "
                               "applies.\n");
#endif
            } // if
        }     // if-else
    }         // if
#endif        // !EXTMM

    memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
    gpGlobals = pGlobals;
// give my version of the engine functions to the other DLL...
#ifdef USE_METAMOD
    memset(&my_engfuncs, 0, sizeof(my_engfuncs));
#else /* not USE_METAMOD */
    memcpy(&my_engfuncs, &g_engfuncs, sizeof(enginefuncs_t));
#endif
    my_engfuncs.pfnFindEntityInSphere = am_FindEntityInSphere;
    my_engfuncs.pfnEntitiesInPVS = am_EntitiesInPVS;
    my_engfuncs.pfnFindEntityByVars = am_FindEntityByVars;
#ifdef WITH_LOGPARSING
    my_engfuncs.pfnAlertMessage = am_AlertMessage;
#endif
// You can override any engine functions here
// my_engfuncs.pfnGetGameDir=GetGameDir;
#ifndef USE_METAMOD
    other_GiveFnptrsToDll(&my_engfuncs, pGlobals);
#endif
}
