/*
 * ===========================================================================
 *
 * $Id: plugin_CS.sma,v 1.6 2003/11/08 10:21:27 darope Exp $
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
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 * ===========================================================================
 *
 * Comments:
 *
 * This script contains commands specific for the Counter-Strike mod
 *
 * Additions mentioned below are copyrighted by their respective authors.
 *
 * Restart round voting by Nathan O'Sullivan
 *
 * Weapon restriction by GIJoe (CSController@yahoo.com)
 *
 * Weapon restricton reworked for CS 1.6 by 
 * Marco Hombach (marco.hombach@gmx.de), Felix Kumm (fkumm@web.de) 
 * and "Ravenous Bugblatter Beast" (ravenousbugblatterbeast@blueyonder.co.uk)
 *
 */


/* Includes */
#include <core>
#include <string>
#include <admin>
#include <plugin>
#include <adminlib>

new AM_VERSION_STRING[] = "2.51.04";


/* Accesslevel Constants */
#define ACCESS_VOTE_RESTART 1
#define ACCESS_RESTART 2
#define ACCESS_CHANGE_TEAM 128
#define ACCESS_RESTRICT 32
#define ACCESS_RESTRICT_SAVE 512
#define ACCESS_CVAR 512

/* Constants used in code. Don't change! */
#define V15 1

#define MAX_ITEMS   32
#define MAX_MENUS   9
#define MAX_CVARS 23

#define ITEM_ENABLED 0
#define ITEM_RESTRICTED 1

#define MENU_NONE       0
#define MENU_PISTOLS    1
#define MENU_SHOTGUNS   2
#define MENU_SMGS       3
#define MENU_RIFLES     4
#define MENU_MG         5
#define MENU_SECAMMO    6
#define MENU_PRIMAMMO   7
#define MENU_EQUIPMENT  8
#define MENU_INVALID    9
#define MENU_CANCEL     10
#define MENU_MAIN       11
#define MENU_RESTRICT   12

/* Global Variables */
new g_CS_Version; /* Stores the CS-Version currently in use, determined by the get_version-function */
/* these are the menu-names wich are matched against the data the user gives to the admin_(un)restrict-command */
new g_MenuNames[MAX_MENUS][] = {
    "",
    "pistols",
    "shotguns",
    "smg",
    "rifles",
    "machine guns",
    "",
    "",
    "equipment"
};
/* these are the full names of all items. they are displayed in menu and used for reports.
   also, they are matched against the data the user gives to the admin_(un)restrict-command */
new g_ItemName[MAX_ITEMS][]= {
    "9x19mm Sidearm",
    "K&M .45 Tactical",
    "228 Compact",
    "Night Hawk .50C",
    ".40 Dual Elites",
    "ES Five-Seven",
    "Leone 12 Gauge Super",
    "Leone YG1265 Auto Shotgun",
    "Ingram MAC-10",
    "K&M Sub-Machine Gun",
    "K&M UMP45",
    "ES C90",
    "Schmidt Machine Pistol",
    "IDF Defender",
    "CV-47",
    "Schmidt Scout",
    "Krieg 552 Commando",
    "Magnum Sniper Rifle",
    "D3/AU-1 Semi-Auto Sniper Rifle",
    "Clarion 5.56",
    "Bullpup",
    "Maverick M4A1 Carabine",
    "Krieg 550 Commando",
    "ES M249 Para",
    "Kevlar Vest",
    "Kevlar Vest & Helmet",
    "Flashbang",
    "HE Grenade",
    "Smoke Grenade",
    "Defuse Kit",
    "NightVision Goggles",
    "Tactical Shield"
};
/* these are the commands clients can use to buy the weapons in 1.6
   they also get matched against the admin_(un)restrict-data */
new g_ClientCommands1[MAX_ITEMS][]= {
    "glock",
    "usp",
    "p228",
    "deagle",
    "elites",
    "fn57",
    "m3",
    "xm1014",
    "mac10",
    "mp5",
    "ump45",
    "p90",
    "tmp",
    "galil",
    "ak47",
    "scout",
    "sg552",
    "awp",
    "g3sg1",
    "famas",
    "aug",
    "m4a1",
    "sg550",
    "m249",
    "vest",
    "vesthelm",
    "flash",
    "hegren",
    "sgren",
    "defuser",
    "nvgs",
    "shield"
};
/* the same, but the new names. both work as client commands.
   left-out items are the same as in g_ClientCommands1 */
new g_ClientCommands2[MAX_ITEMS][]= {
    "9x19mm",
    "km45",
    "229compact",
    "nighthawk",
    "",
    "fiveseven",
    "12gauge",
    "autoshotgun",
    "",
    "smg",
    "",
    "c90",
    "mp",
    "defender",
    "cv47",
    "",
    "krieg552",
    "magnum",
    "d3au1",
    "clarion",
    "bullup",
    "",
    "krieg550",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    ""
};
/* This array holds information about each item being restricted or not */
new g_ItemRestricted[2][MAX_ITEMS + 1] = {{ITEM_ENABLED},{ITEM_ENABLED}}; /* MAX_ITEMS + 1, so we have one fake item that is never restricted */
/* The Menues begin here in alle the above arrays. e.g. shotguns start with g_ItemIndexBegin[1] == 6*/
new g_ItemIndexBegin[9] = {0,6,8,13,23,24,24,24,32};
new g_UserMenuSelected[MAX_PLAYERS] = MENU_NONE;
new g_MenuEntriesIndexBegin[9] = {0,5,7,11,17,18,18,18,26};
new g_ItemIndexByKeys[2][29] = {
    {0,1,2,3,4,6,7,8,9,10,11,13,14,15,16,17,18,23,24,25,26,27,28,30,MAX_ITEMS,MAX_ITEMS,MAX_ITEMS,MAX_ITEMS,MAX_ITEMS}, // let non-existent key combinations point to the fake item
    {0,1,2,3,5,6,7,12,9,10,11,19,15,21,20,22,17,23,24,25,26,27,28,29,30,31,MAX_ITEMS,MAX_ITEMS,MAX_ITEMS}
};
new g_RestrictedItemsNum = 0;
new g_SubMenuTitles[8][] = {"Handgun^n(Secondary weapon)","Shotgun^n(Primary weapon)","Sub-Machine Gun^n(Primary weapon)","Rifle^n(Primary weapon)","Machine Gun^n(Primary weapon)","","","Equipment"};
new g_UserMenuOrder[MAX_PLAYERS];
new g_sCvar[MAX_CVARS][] = {
     "autokick",
     "autoteambalance",
     "buytime",
     "c4timer",
     "chattime",
     "consistency",
     "fadetoblack",
     "flashlight",
     "footsteps",
     "forcecamera",
     "forcechasecam",
     "freezetime",
     "ghostfrequency",
     "hostagepenalty",
     "limitteams",
     "kickpercent",
     "maxrounds",
     "mapvoteratio",
     "playerid",
     "roundtime",
     "startmoney",
     "tkpunish",
     "winlimit"
};
new g_sDescription[MAX_CVARS][] = {
     "admin_autokick <1 or 0>: mp_autokick on or off",
     "admin_autoteambalance <#>: setting for mp_autoteambalance",
     "admin_buytime <#>: setting for mp_buytime",
     "admin_c4timer <#>: setting for mp_c4timer",
     "admin_chattime <#>: setting for mp_chattime",
     "admin_consistency <#>: setting for mp_consistency",
     "admin_fadetoblack <1 or 0>: mp_fadetoblack on or off",
     "admin_flashlight <1 or 0>: mp_flashlight on or off",
     "admin_footsteps <1 or 0>: mp_footsteps on or off",
     "admin_forcecamera <#>: setting for mp_forcecamera",
     "admin_forcechasecam <#>: setting for mp_forcechasecam",
     "admin_freezetime <#>: setting for mp_freezetime",
     "admin_ghostfrequency <#>: setting for mp_ghostfrequency",
     "admin_hpenalty <#>: setting for mp_hostagepenalty",
     "admin_limitteams <1 or 0>: mp_limitteams on or off",
     "admin_kickpercent <#>: setting for mp_kickpercent",
     "admin_maxrounds <#>: setting for mp_maxrounds",
     "admin_mapvoteratio <#>: setting for mp_mapvoteratio",
     "admin_playerid <#>: setting for mp_playerid",
     "admin_roundtime <#>: setting for mp_roundtime",
     "admin_startmoney <800/16000>: setting for mp_startmoney",
     "admin_tkpunish <1 or 0>: mp_tkpunish on or off",
     "admin_winlimit <#>: setting for mp_winlimit"
};

/* Event Handler */
public plugin_init(){
    new i;
    g_CS_Version = get_version();
    plugin_registerinfo("Counter-Strike Plugin","Team change / Restart Round / Weapons Restrictions",AM_VERSION_STRING);
    plugin_registercmd("admin_ct","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_ct <target>: Changes target to the counter-terrorist team.");
    plugin_registercmd("admin_t","admin_changeteam",ACCESS_CHANGE_TEAM,"admin_t <target>: Changes target to the terrorist team.");
    plugin_registercmd("admin_vote_restart","admin_vote_restart",ACCESS_VOTE_RESTART,"admin_vote_restart: Starts a vote to restart the round.");
    plugin_registercmd("admin_restartround","admin_restartround",ACCESS_RESTART,"admin_restartround <seconds>: Restarts the round.");
    plugin_registercmd("admin_restart","admin_restartround",ACCESS_RESTART,"admin_restart <seconds>: Restarts the round.");

    plugin_registercmd("admin_restrict","SetRestrictions",ACCESS_ALL,"admin_restrict: Control restrictions - admin_restrict help for details.");
    plugin_registercmd("admin_unrestrict","SetRestrictions",ACCESS_RESTRICT,"admin_unrestrict: Control restrictions - admin_unrestrict help for details.");
    plugin_registercmd("admin_restrictmenu","RestrictMenu",ACCESS_RESTRICT,"admin_restrictmenu: Displays menu for changing weaponrestrictions.");
    plugin_registercmd("buy","buy",ACCESS_ALL);
    plugin_registercmd("buyequip","buyequip",ACCESS_ALL);
    plugin_registercmd("menuselect","menuselect",ACCESS_ALL);
    plugin_registercmd("radio1","ClearMenu",ACCESS_ALL);
    plugin_registercmd("radio2","ClearMenu",ACCESS_ALL);
    plugin_registercmd("radio3","ClearMenu",ACCESS_ALL);
    plugin_registercmd("chooseteam","ClearMenu",ACCESS_ALL);
    if(g_CS_Version != V15) {
        new Function[MAX_COMMAND_LENGTH];
        for(i=0;i<MAX_ITEMS;i++) { /* register client buy commands */
            strcpy(Function,"buy_",MAX_COMMAND_LENGTH);
            strcat(Function,g_ClientCommands1[i],MAX_COMMAND_LENGTH);
            plugin_registercmd(g_ClientCommands1[i],Function,ACCESS_ALL);
            if(g_ClientCommands2[i][0]) {
                plugin_registercmd(g_ClientCommands2[i],Function,ACCESS_ALL);
            }
        }
    } else { /* change array data according to 1.5 */
        strinit(g_ItemName[13]); /* erase items new in 1.6 */
        strinit(g_ItemName[19]);
        strinit(g_ItemName[31]);
        strinit(g_ClientCommands1[13]);
        strinit(g_ClientCommands1[19]);
        strinit(g_ClientCommands1[31]);
        strinit(g_ClientCommands2[13]);
        strinit(g_ClientCommands2[19]);
        strinit(g_ClientCommands2[31]);
        g_MenuEntriesIndexBegin = {0,6,8,13,21,22,22,22,29};
        new ItemIndexByKeys15[2][29] = { /* Downgrade Keys */
            {1,0,3,2,4,MAX_ITEMS,6,7,9,MAX_ITEMS,11,8,10,14,16,MAX_ITEMS,MAX_ITEMS,15,17,18,MAX_ITEMS,23,24,25,26,27,28,MAX_ITEMS,30},
            {1,0,3,2,MAX_ITEMS,5,6,7,9,12,11,MAX_ITEMS,10,MAX_ITEMS,MAX_ITEMS,21,20,15,17,MAX_ITEMS,22,23,24,25,26,27,28,29,30}
        };
        for(i=0;i<29;i++) {
            g_ItemIndexByKeys[0][i] = ItemIndexByKeys15[0][i];
            g_ItemIndexByKeys[1][i] = ItemIndexByKeys15[1][i];
        }
    }
    /* Read saved restrictions */
    new strKey[MAX_ITEMS+1];
    new strName[100];
    new strMap[100];
    currentmap(strMap,100);
    strcpy(strName,"WeaponRestrictions_",100);
    strcat(strName,strMap,100);
    if(get_vaultdata(strName,strKey,MAX_ITEMS+1)) {
        plugin_message("Map-specific saved weapon restrictions found.");
    } else if(get_vaultdata("WeaponRestrictions",strKey,MAX_ITEMS+1)) { /* If no map-specific restrictions found, load the defaults */
        plugin_message("Default saved weapon restrictions found.");
    } else {
        plugin_message("No saved weapon restrictions found.");
    }
    /* Apply any restrictions found */
    if(strKey[0] != 0) {
        for(i=0;i<MAX_ITEMS;i++) {
            if(strKey[i] == '1') {
                ChangeStatusItem(i,ITEM_RESTRICTED,1);
                g_ItemRestricted[1][i] = ITEM_RESTRICTED;
            }
        }
    }
    if(g_RestrictedItemsNum && !getvar("admin_cs_restrict")) {
        plugin_message("Saved restrictions will not take effect while admin_cs_restrict is 0.");
    }
    if(getvar("allow_client_exec")==0) {
        plugin_message("**************************************************************************************");
        plugin_message("**************************************************************************************");
        plugin_message("WARNING: The cvar allow_client_exec is 0. It is possible for players to bypass weapons");
        plugin_message("         restrictions unless you change this to 1 in your adminmod.cfg file.");
        plugin_message("");
        plugin_message("         If you are not using weapons restrictions, you may ignore this warning.");
        plugin_message("**************************************************************************************");
        plugin_message("**************************************************************************************");
    }
    new sCommand[MAX_TEXT_LENGTH];
    for(i=0;i<MAX_CVARS;i++){
        snprintf(sCommand,MAX_TEXT_LENGTH,"admin_%s",g_sCvar[i]);
        plugin_registercmd(sCommand,"set_cvar",ACCESS_ALL,g_sDescription[i]);
    }
    return PLUGIN_CONTINUE;
}

public admin_changeteam(HLCommand,HLData,HLUserName,UserIndex) {
    new Command[MAX_COMMAND_LENGTH];
    new Data[MAX_DATA_LENGTH];
    new TargetName[MAX_NAME_LENGTH];
    new Text[MAX_TEXT_LENGTH];
    new User[MAX_NAME_LENGTH];
    convert_string(HLCommand,Command,MAX_NAME_LENGTH);
    convert_string(HLData,Data,MAX_NAME_LENGTH);
    convert_string(HLUserName,User,MAX_NAME_LENGTH);
    if(!getvar("allow_client_exec")) {
        selfmessage("You cannot use this command because allow_client_exec is 0.");
    } else if(!check_user(Data)) {
        selfmessage("Unrecognized player: ");
        selfmessage(Data);
    } else {
        get_username(Data,TargetName,MAX_NAME_LENGTH);
        if(check_immunity(TargetName) != 0) {
            snprintf(Text, MAX_TEXT_LENGTH, "Laf. You can't move %s onto another team, you silly bear.", TargetName);
            messageex(User,Text,print_chat);
        } else {
            say_command(User,Command,TargetName);
            if(!strcmp(Command,"admin_ct")) {
                execclient(TargetName, "chooseteam;menuselect 2");
            } else {
                execclient(TargetName, "chooseteam;menuselect 1");
            }
        }
    }
    return PLUGIN_HANDLED;
}

public admin_restartround(HLCommand,HLData,HLUserName,UserIndex) {
    new Command[MAX_COMMAND_LENGTH];
    new Data[MAX_DATA_LENGTH];
    new User[MAX_NAME_LENGTH];
    convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
    convert_string(HLData,Data,MAX_DATA_LENGTH);
    convert_string(HLUserName,User,MAX_NAME_LENGTH);
    execute_command(User,Command,"sv_restartround",Data);
    return PLUGIN_HANDLED;
}

public admin_vote_restart(HLCommand,HLData,HLUserName,UserIndex) {
    new ratio = getvar("map_ratio");
    if(!ratio) {
        say("Restart vote is not enabled");
    } else if(!vote_allowed()) {
        selfmessage( "Vote not allowed at this time.");
    } else {
        vote("Restart map & begin play?","Yes","No","HandleRestartVote","");
    }
    return PLUGIN_HANDLED;
}

public HandleRestartVote(WinningOption,HLUser,VoteCount,UserCount) {
    if(WinningOption == 1 ) {
        new ratio = getvar("map_ratio");
        if(VoteCount < (ratio/100)*UserCount) {
            say("Not enough votes for map restart");
        } else {
            new Timelimit[MAX_NUMBER_LENGTH];
            numtostr((getvar("mp_timelimit") * 60 - timeleft() + 10) / 60,Timelimit);
            setstrvar("mp_timelimit",Timelimit);
            setstrvar("sv_restartround","10");
            say("Map restart vote succeeded.");
        }
    } else {
        say("Map restart vote failed.");
    }
}

SaveRestrictions(Mapspecific = 0) {
    new strName[100] = "WeaponRestrictions";
    new strKey[MAX_ITEMS+1];
    new i;
    if(Mapspecific) {
        new strMap[100];
        currentmap(strMap,100);
        strcat(strName,"_",100);
        strcat(strName,strMap,100);
    }
    for(i=0;i<MAX_ITEMS;i++) {
        g_ItemRestricted[1][i] = g_ItemRestricted[0][i]; /* Update saved restriction array */
        if(g_ItemRestricted[0][i] == ITEM_RESTRICTED) {
            strKey[i] = '1';
        }
        else {
            strKey[i] = '0';
        }
    }
    strKey[MAX_ITEMS] = 0;
    set_vaultdata(strName,strKey);
}

ismenustring(string[],&num) {
    new i;
    new match = -1;
    for(i=0;i<MAX_MENUS;i++) {
        if(g_MenuNames[i][0] && strstr(g_MenuNames[i],string) != -1) {
            if(match != -1) {
                new Data[100];
                snprintf(Data,100,"The data supplied matches both '%s' and '%s'",g_MenuNames[match],g_MenuNames[i]);
                selfmessage(Data);
                return 0;
            }
            match = i;
        }
    }
    for(i=0;i<MAX_MENUS;i++) {
        if(g_MenuNames[i][0] && !strcmp(g_MenuNames[i],string)) {
            num = i;
            return 1;
        }
    }
    if(match != -1) {
        num = match;
        return 1;
    }
    return 0;
}

isweaponstring(string[],&num) {
    new i;
    new match = -1;
    for(i=0;i<MAX_ITEMS;i++) {
        if(!g_ClientCommands1[i][0]) continue;
        if(!strcmp(g_ClientCommands1[i],string) || g_ClientCommands2[i][0] && !strcmp(g_ClientCommands2[i],string)) {
            num = i;
            return 2;
        }
    }
    for(i=0;i<MAX_ITEMS;i++) {
        if(!g_ClientCommands1[i][0]) continue;
        if(strstr(g_ClientCommands1[i],string) != -1 || g_ClientCommands2[i][0] && strstr(g_ClientCommands2[i],string) != -1) {
            if(match != -1) {
                new Data[100];
                snprintf(Data,100,"The data supplied matches both '%s' and '%s'",g_ItemName[match],g_ItemName[i]);
                selfmessage(Data);
                return 0;
            }
            match = i;
        }
    }
    if(match != -1) {
        num = match;
        return 1;
    }
    return 0;
}

isfullweaponstring(string[],&num) {
    new i;
    new match = -1;
    for(i=0;i<MAX_ITEMS;i++) {
        if(g_ItemName[i][0] && strcasestr(g_ItemName[i],string) != -1) {
            if(match != -1) {
                new Data[100];
                snprintf(Data,100,"The data supplied matches both '%s' and '%s'",g_ItemName[match],g_ItemName[i]);
                selfmessage(Data);
                return 0;
            }
            match = i;
        }
    }
    if(match != -1) {
        num = match;
        return 1;
    }
    return 0;
}

public SetRestrictions(HLCommand,HLData,HLUserName,UserIndex) {
    new Command[MAX_COMMAND_LENGTH];
    new Data[MAX_DATA_LENGTH];
    new Param[2][MAX_DATA_LENGTH];
    new UserName[MAX_NAME_LENGTH];
    new Status;
    new Params;
    new num;
    new num2;
    convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
    convert_string(HLData,Data,MAX_DATA_LENGTH);
    convert_string(HLUserName,UserName,MAX_NAME_LENGTH);
    strstripquotes(Data);
    new i;
    for(i=0;Data[i];i++) { /* make data lower-case */
        Data[i] = tolower(Data[i]);
    }
    Status = Command[6] == 'r' ? ITEM_RESTRICTED : ITEM_ENABLED; /* Look at the command name to see if we should lock or unlock */
    Params = strsplit(Data," ",Param[0],MAX_DATA_LENGTH,Param[1],MAX_DATA_LENGTH);
    if(Params != 0 && !access(ACCESS_RESTRICT)) {
        selfmessage("You do not have access to modify weapon restrictions.");
        selfmessage("Type admin_restrict to see current restrictions.");
        return PLUGIN_HANDLED;
    } else if(Params == 0) { /* Check if an argument was supplied */
        if(Status || access(ACCESS_RESTRICT) == 0) {
            showrestrictions();
            return PLUGIN_HANDLED;
        }
    } else if(Params == 1) { /* If one parameter */
        if(!isnumeric(Param[0],num)) { /* if param is not a number */
            if(Status) { /* some commands should only work with admin_restrict... */
                if(strcmp(Param[0],"off") == 0) { /* Check if we are disabling weapon restrictions */
                    if(getvar("admin_cs_restrict")) {
                        setstrvar("admin_cs_restrict","0");
                        say_command(UserName,Command,Data);
                        selfmessage("Weapon restrictions have been disabled.");
                    } else {
                        selfmessage("Weapon restrictions already are disabled.");
                    }
                    return PLUGIN_HANDLED;
                }
                if(strcmp(Param[0],"on") == 0) { /* Check if we are enabling weapons restrictions */
                    if(!getvar("admin_cs_restrict")) {
                        setstrvar("admin_cs_restrict","1");
                        say_command(UserName,Command,Data);
                        if(g_RestrictedItemsNum) {
                            selfmessage("Weapon restrictions have been enabled.");
                        } else {
                            selfmessage("Weapon restriction functions are now enabled, but no weapons are");
                            selfmessage("currenty marked as restricted. Use the admin_restrict command to");
                            selfmessage("select which weapons to restrict.");
                        }
                    } else {
                        selfmessage("Weapon restrictions already are enabled.");
                    }
                    return PLUGIN_HANDLED;
                }
                if(strcmp(Param[0],"restore") == 0) {
                    for(i=0;i<MAX_ITEMS;i++) {
                        ChangeStatusItem(i,g_ItemRestricted[1][i],0);
                    }
                    say_command(UserName,Command,Data);
                    selfmessage("Weapon restrictions have been reset to default.");
                    MessageUser(Status);
                    return PLUGIN_HANDLED;
                }
            }
            if(!strcmp(Param[0],"help")) { /* we want to view the help text */
                show_help(Command);
                return PLUGIN_HANDLED;
            } else if(!strcmp(Param[0],"all")) { /* we want to restrict EVERYTHING */
                ChangeStatusAll(Status,1);
                say_command(UserName,Command,Data);
                MessageUser(Status);
                return PLUGIN_HANDLED;
            } else if(!strcmp(Param[0],"weapons")) { /* we want to restrict EVERYTHING EXCEPT EQUIPMENT */
                ChangeStatusWeapons(Status,1);
                say_command(UserName,Command,Data);
                MessageUser(Status);
                return PLUGIN_HANDLED;
            }
        }
    } else { /* if two paramemters */
        if(Status) { /* again, some functions only should work with admin_restrict */
            if(strcmp(Param[0],"save") == 0) { /* Check if we are saving restrictions */
                if(!access(ACCESS_RESTRICT_SAVE)) {
                    reject_message();
                    return PLUGIN_HANDLED;
                }
                if(strcmp(Param[1],"map") == 0) {
                    log_command(UserName,Command,Data);
                    SaveRestrictions(1);
                    return PLUGIN_HANDLED;
                } else if(strcmp(Param[1],"default") == 0) {
                    log_command(UserName,Command,Data);
                    SaveRestrictions(0);
                    return PLUGIN_HANDLED;
                }
            }
        }
        if(isnumeric(Param[0],num)) { /* parameter one is a number */
            if((0 < num < MENU_PRIMAMMO || num == MENU_EQUIPMENT) && isnumeric(Param[1],num2) && 0 < num2 <= g_MenuEntriesIndexBegin[num] - g_MenuEntriesIndexBegin[num - 1]) { /* parameter 1 is a valid menu slot and parameter 2 a valid item of this menu, else invalid data */
                i = g_MenuEntriesIndexBegin[num - 1] + num2 - 1; /* get itemindex */
                new Item = g_ItemIndexByKeys[0][i];
                new Item2 = g_ItemIndexByKeys[1][i];
                if(g_CS_Version == V15) {
                    new RestrictItem = MAX_ITEMS;
                    if(Item != MAX_ITEMS) RestrictItem = Item;
                    else if(Item2 != MAX_ITEMS) RestrictItem = Item2;
                    if(RestrictItem != MAX_ITEMS) {
                        ChangeStatusItem(RestrictItem,Status,1);
                        say_command(UserName,Command,Data);
                        MessageUser(Status);
                        return PLUGIN_HANDLED;
                    }
                } else {
                    if(Item != Item2 && Item != MAX_ITEMS) { /* if this combo has two items, dont restrict, but print an error */
                        snprintf(Data,100,"The data supplied matches both '%s' and '%s'",g_ItemName[Item],g_ItemName[Item2]);
                        selfmessage(Data);
                    } else { /* Handle restriction */
                        ChangeStatusItem(Item2,Status,1);
                        say_command(UserName,Command,Data);
                        MessageUser(Status);
                        return PLUGIN_HANDLED;
                    }
                }
            }
        } else if(!strcmp(Param[0],"menu")) {
            if(isnumeric(Param[1],num2)) {
                if(0 < num2 < MENU_PRIMAMMO || num == MENU_EQUIPMENT) {
                    ChangeStatusMenu(num2,Status,1);
                    say_command(UserName,Command,Data);
                    MessageUser(Status);
                    return PLUGIN_HANDLED;
                }
            } else {
                if(ismenustring(Param[1],num2)) {
                    ChangeStatusMenu(num2,Status,1);
                    say_command(UserName,Command,Data);
                    MessageUser(Status);
                    return PLUGIN_HANDLED;
                }
            }
        } else if(ismenustring(Param[0],num) && isnumeric(Param[1],num2) && 0 < num2 <= g_MenuEntriesIndexBegin[num] - g_MenuEntriesIndexBegin[num - 1]) {
            i = g_MenuEntriesIndexBegin[num - 1] + num2 - 1; /* get itemindex */
            new Item = g_ItemIndexByKeys[0][i];
            new Item2 = g_ItemIndexByKeys[1][i];
            if(g_CS_Version == V15) {
                new RestrictItem = MAX_ITEMS;
                if(Item != MAX_ITEMS) RestrictItem = Item;
                else if(Item2 != MAX_ITEMS) RestrictItem = Item2;
                if(RestrictItem != MAX_ITEMS) {
                    ChangeStatusItem(RestrictItem,Status,1);
                    say_command(UserName,Command,Data);
                    MessageUser(Status);
                    return PLUGIN_HANDLED;
                }
            } else {
                if(Item != Item2 && Item != MAX_ITEMS) { /* if this combo has two items, dont restrict, but print an error */
                    snprintf(Data,100,"The data supplied matches both '%s' and '%s'",g_ItemName[Item],g_ItemName[Item2]);
                    selfmessage(Data);
                } else { /* Handle restriction */
                    ChangeStatusItem(Item2,Status,1);
                    say_command(UserName,Command,Data);
                    MessageUser(Status);
                    return PLUGIN_HANDLED;
                }
            }
        }
    } /* now its time do do some weapon substring-matching */
    i = isweaponstring(Data,num);
    if(i == 2) {
        ChangeStatusItem(num,Status,1);
        say_command(UserName,Command,Data);
        MessageUser(Status);
        return PLUGIN_HANDLED;
    } /* now do a substring-match trough all the full names */
    if(isfullweaponstring(Data,num2)) {
        if(i == 0 || num == num2) { /* only if first search found nothing or they both found the same */
            ChangeStatusItem(num,Status,1);
            say_command(UserName,Command,Data);
            MessageUser(Status);
            return PLUGIN_HANDLED;
        } else if(num != num2) { /* both function got different items */
            snprintf(Data,100,"The data supplied matches both '%s' and '%s'",g_ItemName[num],g_ItemName[num2]);
            selfmessage(Data);
            return PLUGIN_HANDLED;
        }
    }
    if(Status) { /* when we came here, the data did not match anything. it must be invalid */
        selfmessage("The data you supplied was invalid. Type ^"admin_restrict help^" to view a help-text.");
    } else {
        selfmessage("The data you supplied was invalid. Type ^"admin_unrestrict help^" to view a help-text.");
    }
    return PLUGIN_HANDLED;
}

show_help(Command[]) {
    new Data[100];
    snprintf(Data,100,"%s usage:^n",Command);
    selfmessage(Data);
    if(Command[6] == 'r') {
        selfmessage("admin_restrict: Show current restrictions");
        selfmessage("admin_restrict on: Enable restrictions");
        selfmessage("admin_restrict off: Disable restrictions");
        selfmessage("admin_restrict save default: Save current restrictions as server default");
        selfmessage("admin_restrict save map: Save current restrictions for this map");
        selfmessage("admin_restrict restore: Restore saved (map-)defaults");
        selfmessage("");
        selfmessage("admin_restrict all: Restrict all weapons and equipment.");
        selfmessage("admin_restrict weapons: Restrict all weapons.");
        selfmessage("admin_restrict menu <menu name or number>: Restrict menu.");
        selfmessage("admin_restrict <item name or number>: Restrict item.");
        selfmessage("admin_restrict <menu name or number> <item number>: Restrict item.");
    } else {
        selfmessage("admin_unrestrict all: Allow all weapons and equipment.");
        selfmessage("admin_unrestrict weapons: Allow all weapons.");
        selfmessage("admin_unrestrict menu <menu name or number>: Allow menu.");
        selfmessage("admin_unrestrict <item name or number>: Allow item.");
        selfmessage("admin_unrestrict <menu name or number> <item number>: Allow item.");
    }
    selfmessage("");
    selfmessage("All weapon / menu names should be entered exactly as they appear on-screen.");
    selfmessage("You cannot restrict ammunition.");
}

/*FILL THE ARRAYS WITH DATA*/
ChangeStatusAll(Status,Verbose = 0) {
    new i;
    if(Verbose) {
        selfmessage(Status ? "All items restricted" : "All items enabled");
    }
    for(i=0;i<MAX_ITEMS;i++) {
        ChangeStatusItem(i,Status,0);
    }
}

ChangeStatusWeapons(Status,Verbose = 0) {
    new i;
    if(Verbose) {
        selfmessage(Status ? "All weapons restricted" : "All weapons enabled");
    }
    for(i=0;i<g_ItemIndexBegin[MENU_EQUIPMENT];i++) {
        ChangeStatusItem(i,Status,0);
    }
}

ChangeStatusMenu(Menu,Status,Verbose = 0) {
    new i;
    if(Verbose) {
        new Text[100];
        snprintf(Text,100,"All items from menu %s %s",g_MenuNames[Menu],Status ? "restricted" : "enabled");
        selfmessage(Text);
    }
    for(i=g_ItemIndexBegin[Menu - 1];i<g_ItemIndexBegin[Menu];i++) {
        ChangeStatusItem(i,Status,0);
    }
}

ChangeStatusItem(Item,Status,Verbose = 0) {
    if(!g_ClientCommands1[Item][0]) return; /* Check if item exists (1.6 has new items) */
    if(Verbose) {
        new Text[100];
        if(Status != g_ItemRestricted[0][Item]) {
            snprintf(Text,100,"Item %s %s",g_ItemName[Item],Status ? "restricted" : "enabled");
        } else {
            snprintf(Text,100,"Item %s already is %s",g_ItemName[Item],Status ? "restricted" : "enabled");
        }
        selfmessage(Text);
    }
    if(Status != g_ItemRestricted[0][Item]) {
        g_ItemRestricted[0][Item] = Status;
        g_RestrictedItemsNum += Status ? 1 : -1;
    }
}

MessageUser(Status) {
    if(Status && !getvar("admin_cs_restrict")) {
        selfmessage("Restriction configured. This will take effect when you use admin_restrict on");
    }
}

showclass(class) {
    new msg[MAX_DATA_LENGTH];
    new strUnlocked[MAX_DATA_LENGTH];
    new strLocked[MAX_DATA_LENGTH];
    new iRestrictStart = g_ItemIndexBegin[class - 1];
    new iRestrictEnd = g_ItemIndexBegin[class] - 1;
    new i;

    new fLocked=1;
    new fUnlocked=1;
    new fFirstLocked=1;
    new fFirstUnlocked=1;

    for (i=iRestrictStart;i<=iRestrictEnd;i++) {
        if(!g_ClientCommands1[i][0]) continue; /* skip items new in 1.6 */
        if (g_ItemRestricted[0][i] == ITEM_RESTRICTED) {
            fUnlocked=0;
            if (fFirstLocked==1) {
                fFirstLocked=0;
            }
            else {
                strcat(strLocked,", ",MAX_DATA_LENGTH);
            }
            strcat(strLocked,g_ItemName[i],MAX_DATA_LENGTH);
        }
        else {
            fLocked=0;
            if (fFirstUnlocked==1) {
                fFirstUnlocked=0;
            }
            else {
                strcat(strUnlocked,", ",MAX_DATA_LENGTH);
            }
            strcat(strUnlocked,g_ItemName[i],MAX_DATA_LENGTH);
        }
    }

    if (fLocked) {
        snprintf(msg,MAX_DATA_LENGTH,"%s: All restricted",g_MenuNames[class]);
    }
    else if (fUnlocked) {
        snprintf(msg,MAX_DATA_LENGTH,"%s: All available",g_MenuNames[class]);
    }
    else {
        if (strlen(strLocked)<strlen(strUnlocked)) {
            snprintf(msg,MAX_DATA_LENGTH,"%s: %s restricted",g_MenuNames[class],strLocked);
        }
        else {
            snprintf(msg,MAX_DATA_LENGTH,"%s: Only %s available",g_MenuNames[class],strUnlocked);
        }
    }
    msg[0] = toupper(msg[0]); /* First char of menu name in capital letter */
    selfmessage(msg);
}

showrestrictions(){
    new enforced = (getvar("admin_cs_restrict")!=0);
    new admin = (access(ACCESS_RESTRICT)!=0);

    if (enforced || admin) {
        selfmessage("^nCurrent weapon restrictions:^n");
        showclass(1);
        showclass(2);
        showclass(3);
        showclass(4);
        showclass(5);
        showclass(8);
    }
    else {
        selfmessage("^nNo weapon restrictions are currently in force.");
    }

    if (!enforced && admin) {
        selfmessage("^nNOTE: Weapon restrictions are not currently enforced.");
        selfmessage("To enforce them use the command: admin_restict on");
    }
    else if (enforced) {
        selfmessage("^nNOTE: Further weapon restrictions may be hard coded into this map.");
    }
    else {
        selfmessage("^nNOTE: Some weapon restrictions may be hard coded into this map.");
    }

}

public RestrictMenu(HLCommand,HLData,HLUserName,UserIndex) {
    if(!UserIndex) {
        selfmessage("This command cannot be used from console.");
        return PLUGIN_HANDLED;
    } else if(!ismenuenabled()) {
        selfmessage("This command cannot be used, because menues are disabled on this server.");
        selfmessage("Menues currently only have beta-status and need to get enabled by setting");
        selfmessage("amv_enable_beta to ^"menu1^" in the adminmod.cfg.");
        return PLUGIN_HANDLED;
    }
    DrawMainMenu(UserIndex);
    return PLUGIN_HANDLED;
}

public plugin_connect(HLUserName,HLIP,UserIndex) {
    if (0 < UserIndex < MAX_PLAYERS) {
        g_UserMenuSelected[UserIndex] = MENU_NONE;
    }
    return PLUGIN_CONTINUE;
}

public plugin_disconnect(HLUserName,UserIndex) {
    if (0 < UserIndex < MAX_PLAYERS) {
        g_UserMenuSelected[UserIndex] = MENU_NONE;
    }
    return PLUGIN_CONTINUE;
}

public buy(HLCommand,HLData,HLUserName,UserIndex){
    g_UserMenuSelected[UserIndex] = MENU_MAIN;
    return PLUGIN_CONTINUE;
}

public buyequip(HLCommand,HLData,HLUserName,UserIndex){
    g_UserMenuSelected[UserIndex] = MENU_EQUIPMENT;
    return PLUGIN_CONTINUE;
}

public ClearMenu(HLCommand,HLData,HLUserName,UserIndex) {
    g_UserMenuSelected[UserIndex] = MENU_NONE;
    return PLUGIN_CONTINUE;
}

public menuselect(HLCommand,HLData,HLUserName,UserIndex) {
    if(g_UserMenuSelected[UserIndex] != MENU_NONE) {
        static Data[MAX_DATA_LENGTH];
        new SelectedNumber;
        convert_string(HLData,Data,MAX_DATA_LENGTH);
        strstripquotes(Data);
        if(!isnumeric(Data,SelectedNumber)) return PLUGIN_HANDLED; /* Player tried to enter something invalid */
        if(!(0 < SelectedNumber < 11)) return PLUGIN_CONTINUE;
        if(SelectedNumber == MENU_CANCEL) {
            if(g_UserMenuSelected[UserIndex] < 0) {
                DrawMainMenu(UserIndex);
            } else {
                g_UserMenuSelected[UserIndex] = MENU_NONE;
            }
        } else if(g_UserMenuSelected[UserIndex] == MENU_RESTRICT) {
            DrawSubMenu(UserIndex,SelectedNumber);
            return PLUGIN_HANDLED;
        } else if(g_UserMenuSelected[UserIndex] < 0) {
            new i;
            if(g_CS_Version != V15) {
                if(SelectedNumber == MENU_INVALID) {
                    g_UserMenuOrder[UserIndex] = g_UserMenuOrder[UserIndex] ? 0 : 1;
                } else {
                    i = g_ItemIndexByKeys[g_UserMenuOrder[UserIndex]][g_MenuEntriesIndexBegin[-g_UserMenuSelected[UserIndex]-1]+SelectedNumber-1];
                    plugin_exec(g_ItemRestricted[0][i] == ITEM_RESTRICTED ? "admin_unrestrict" : "admin_restrict",g_ClientCommands2[i][0] ? g_ClientCommands2[i] : g_ClientCommands1[i]);
                }
            } else {
                i = g_ItemIndexByKeys[0][g_MenuEntriesIndexBegin[-g_UserMenuSelected[UserIndex]-1]+SelectedNumber-1];
                if(i == MAX_ITEMS) i = g_ItemIndexByKeys[1][g_MenuEntriesIndexBegin[-g_UserMenuSelected[UserIndex]-1]+SelectedNumber-1];
                plugin_exec(g_ItemRestricted[0][i] == ITEM_RESTRICTED ? "admin_unrestrict" : "admin_restrict",g_ClientCommands2[i][0] ? g_ClientCommands2[i] : g_ClientCommands1[i]);
            }
            DrawSubMenu(UserIndex,-g_UserMenuSelected[UserIndex]);
            return PLUGIN_HANDLED;
        } else if(g_RestrictedItemsNum && getvar("admin_cs_restrict")) {
            static UserName[MAX_NAME_LENGTH];
            convert_string(HLUserName,UserName,MAX_NAME_LENGTH);
            if(g_UserMenuSelected[UserIndex] == MENU_MAIN) {
                if(SelectedNumber == MENU_PRIMAMMO || SelectedNumber == MENU_SECAMMO || SelectedNumber == MENU_INVALID) {
                    if(getvar("allow_client_exec")) {
                        execclient(UserName,"menuselect 10");
                    }
                } else {
                    g_UserMenuSelected[UserIndex] = SelectedNumber;
                }
            } else {
                if(g_CS_Version != V15) {
                    if(getvar("allow_client_exec")) {
                        execclient(UserName,"menuselect 10");
                    }
                }
                if(SelectedNumber <= g_MenuEntriesIndexBegin[g_UserMenuSelected[UserIndex]] - g_MenuEntriesIndexBegin[g_UserMenuSelected[UserIndex] - 1]) {
                    new Team;
                    get_userTeam(UserName,Team);
                    Team = clamp(Team,1,2); /* to avoid AMX-error 4 (array out of bounds), keep Team between 1 and 2 */
                    if(g_ItemRestricted[0][g_ItemIndexByKeys[Team - 1][g_MenuEntriesIndexBegin[g_UserMenuSelected[UserIndex] - 1] + SelectedNumber - 1]] == ITEM_RESTRICTED) {
                        messageex(UserName,"Item currently not allowed",print_center);
                        return PLUGIN_HANDLED;
                    }
                }
            }
        }
    }
    return PLUGIN_CONTINUE;
}

public set_cvar(HLCommand,HLData,HLUserName,UserIndex) {
    new Text[MAX_TEXT_LENGTH];
    new Command[MAX_COMMAND_LENGTH];
    new Data[MAX_DATA_LENGTH];
    new User[MAX_NAME_LENGTH];
    new sCvar[MAX_COMMAND_LENGTH];
    convert_string(HLCommand,Command,MAX_COMMAND_LENGTH);
    convert_string(HLData,Data,MAX_DATA_LENGTH);
    convert_string(HLUserName,User,MAX_NAME_LENGTH);
    snprintf(sCvar,MAX_COMMAND_LENGTH,"mp_%s",Command[6]);
    if(Data[0] != 0 && access(ACCESS_CVAR)){
        execute_command(User,Command,sCvar,Data);
    } else if(Data[0] != 0) {
        snprintf(Data,100,"You cannot change the value of %s.^nType %s to view current setting.",sCvar,Command);
        selfmessage(Data);
    } else {
        getstrvar(sCvar,Data,MAX_DATA_LENGTH);
        snprintf(Text,MAX_TEXT_LENGTH,"%s is currently set to %s",sCvar,Data);
        selfmessage(Text);
    }
    return PLUGIN_HANDLED;
}

isnumeric(string[],&num) {
    new i;
    new n = 0;
    if(string[0] == 0) return 0;
    for(i=0;string[i];i++) {
        if('0' <= string[i] <= '9') {
            n *= 10;
            n += (string[i] - '0');
        } else {
            return 0;
        }
    }
    num = n;
    return 1;
}

get_version() {
    new data[100];
    getstrvar("sv_region",data,100);
    setstrvar("sv_region","1");
    if(getvar("sv_region")) {
        setstrvar("sv_region",data);
        return V15 + 1; /* return anything, but not V15 */
    }
    return V15;
}

ismenuenabled() {
    new data[100];
    getstrvar("amv_enable_beta",data,100);
    if(strstr(data,"menu1") != -1) {
        return 1;
    }
    return 0;
}

DrawMainMenu(UserIndex) {
    new UserName[MAX_NAME_LENGTH];
    new iTeam;
    g_UserMenuSelected[UserIndex] = MENU_RESTRICT;
    playerinfo(UserIndex,UserName,MAX_NAME_LENGTH);
    get_userTeam(UserName,iTeam);
    g_UserMenuOrder[UserIndex] = clamp(iTeam - 1,0,1);
    menu(UserName,"\yRestrict Item\w^n^n1. Handgun^n2. Shotgun^n3. Sub-Machine Gun^n4. Rifle^n5. Machine Gun^n^n6. Primary weapon ammo^n7. Secondary weapon ammo^n^n8. Equipment^n^n0. Exit",671,0);
}

DrawSubMenu(UserIndex,Menu) {
    new Menutext[512];
    new Text[100];
    new UserName[MAX_NAME_LENGTH];
    new i,j,k;
    new Keys;
    snprintf(Menutext,512,"\y Restrict %s^n",g_SubMenuTitles[Menu-1]);
    for(i=g_MenuEntriesIndexBegin[Menu-1];i<g_MenuEntriesIndexBegin[Menu];i++) {
        j++;
        if(g_CS_Version == V15) {
            k = g_ItemIndexByKeys[0][i];
            if(k == MAX_ITEMS) {
                k = g_ItemIndexByKeys[1][i];
            }
        } else {
            k = g_ItemIndexByKeys[g_UserMenuOrder[UserIndex]][i];
            if(k == MAX_ITEMS) break;
        }
        snprintf(Text,100,"^n\w%i. %s%s",j,g_ItemRestricted[0][k] == ITEM_RESTRICTED ? "\r" : "",g_ItemName[k]);
        strcat(Menutext,Text,512);
    }
    Keys = (1<<j)+511;
    if(g_CS_Version != V15) {
        Keys |= 256;
        if(g_UserMenuOrder[UserIndex] == 0) {
            strcat(Menutext,"^n^n\w9. \ySwitch to CT Layout",512);
        } else {
            strcat(Menutext,"^n^n\w9. \ySwitch to Terrorist Layout",512);
        }
    }
    strcat(Menutext,"^n^n\w0. Back",512);
    playerinfo(UserIndex,UserName,MAX_NAME_LENGTH);
    g_UserMenuSelected[UserIndex] = -Menu;
    menu(UserName,Menutext,Keys,0);
}

public buy_glock(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(0,HLUserName);
}

public buy_usp(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(1,HLUserName);
}

public buy_p228(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(2,HLUserName);
}

public buy_deagle(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(3,HLUserName);
}

public buy_elites(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(4,HLUserName);
}

public buy_fn57(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(5,HLUserName);
}

public buy_m3(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(6,HLUserName);
}

public buy_xm1014(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(7,HLUserName);
}

public buy_mac10(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(8,HLUserName);
}

public buy_mp5(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(9,HLUserName);
}

public buy_ump45(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(10,HLUserName);
}

public buy_p90(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(11,HLUserName);
}

public buy_tmp(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(12,HLUserName);
}

public buy_galil(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(13,HLUserName);
}

public buy_ak47(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(14,HLUserName);
}

public buy_scout(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(15,HLUserName);
}

public buy_sg552(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(16,HLUserName);
}

public buy_awp(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(17,HLUserName);
}

public buy_g3sg1(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(18,HLUserName);
}

public buy_famas(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(19,HLUserName);
}

public buy_aug(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(20,HLUserName);
}

public buy_m4a1(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(21,HLUserName);
}

public buy_sg550(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(22,HLUserName);
}

public buy_m249(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(23,HLUserName);
}

public buy_vest(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(24,HLUserName);
}

public buy_vesthelm(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(25,HLUserName);
}

public buy_flash(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(26,HLUserName);
}

public buy_hegren(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(27,HLUserName);
}

public buy_sgren(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(28,HLUserName);
}

public buy_defuser(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(29,HLUserName);
}

public buy_nvgs(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(30,HLUserName);
}

public buy_shield(HLCommand, HLData, HLUserName, UserIndex) {
    return buy_item(31,HLUserName);
}

buy_item(iItem,HLUserName) {
    if(g_ItemRestricted[0][iItem] == ITEM_RESTRICTED && getvar("admin_cs_restrict")) {
        new User[MAX_NAME_LENGTH];
        convert_string(HLUserName,User,MAX_NAME_LENGTH);
        messageex(User,"Item currently not allowed",print_center);
        return PLUGIN_HANDLED;
    }
    return PLUGIN_CONTINUE;
}


