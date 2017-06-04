/*
 * ===========================================================================
 *
 * $Id: util.cpp,v 1.12 2004/08/19 22:23:01 the_wraith Exp $
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
 * This file implements some helper functions
 *
 */

#ifdef USE_MYSQL
#include <mysql.h>
#include <errmsg.h>

#include "extdll.h"
extern MYSQL mysql;
extern BOOL g_fUseMySQL;
#else
#ifdef USE_PGSQL
#include <libpq-fe.h>

#include "extdll.h"
extern PGconn* pgsql;
extern BOOL g_fUsePgSQL;
#else
#include "extdll.h"
#endif
#endif

#include <ctype.h> /* for isprint */
#include "constants.h"
#include "amutil.h"
#include "amlibc.h"
#include "authid.h"
#include "users.h"

#define SDK_UTIL_H
#include "meta_api.h"

extern globalvars_t* gpGlobals;
extern enginefuncs_t g_engfuncs;
extern mapcycle_t mapcycle;

DLL_GLOBAL const Vector g_vecZero = Vector(0, 0, 0);

/* Like System_Response, but will _always_ log. */
void System_Error(char* str, edict_t* pEntity)
{
    UTIL_LogPrintf("%s", str);
    if(pEntity != NULL) {
        if((int)CVAR_GET_FLOAT("admin_debug") != 0) {
            CLIENT_PRINTF(pEntity, print_chat, str);
        } else {
            CLIENT_PRINTF(pEntity, print_console, str);
        }
    }
}

/*
 *  a generic way to print a msg to a user, whether they are on the
 *  console or on a client
 *
 *
 */
void System_Response(char* str, edict_t* pAdminEnt)
{

    if(pAdminEnt == NULL) {
        UTIL_LogPrintf("%s", str);
    } else {
        CLIENT_PRINTF(pAdminEnt, print_console, str);
    }
}

/*
  stop malformed names stuffing up checking

*/
int make_friendly(char* name, BOOL check)
{
    int i;
    int iLen = strlen(name);

    if(check && (g_NameCrashAction > 0) && (iLen <= 0))
        return 2; // the name is zero length....

    for(i = 0; i < iLen; i++) {
        if(name[i] == '%') {
            // replace '%' with ' ' like the engine
            name[i] = ' ';
        }
        // since we go through the string here anyway, we may just as well check for
        // vicious characters
        if(check && (g_NameCrashAction > 0) && !isprint(name[i]))
            return 2;
    }
    return 1;
}

/*
 *  escape a set of characters from a string by adding a backslash in front of them.
 *  returns a const pointer to a static string
 *
 * prototype in amutil.h
 *
 */
const char* escape_chars(const char* _pcString, const char* _pcChars)
{
    static char* pcResultString = 0;
    int iLength = strlen(_pcString);
    int iNumChars = strlen(_pcChars);
    int iNumEscapes = 0;

    // TODO: add a check for unprintable characters here

    // count the number of characters to escape.
    const char* pcPos = _pcString;
    while((pcPos = strpbrk(pcPos, _pcChars)) != 0) {
        iNumEscapes++;
        pcPos++;
    } // while

    if(iNumEscapes == 0) {
        // no characters found to be escaped
        return _pcString;
    } // if

    if(pcResultString != 0)
        delete[] pcResultString;
    pcResultString = new char[iLength + iNumEscapes + 1];
    if(pcResultString == 0)
        return 0;

    // copy the string into our new string, escaping charaters
    char* pcSet = pcResultString;
    const char* pcCopy = _pcString;
    pcPos = _pcString;
    while((pcPos = strpbrk(pcPos, _pcChars)) != 0) {
        while(pcCopy != pcPos) {
            *pcSet = *pcCopy;
            pcSet++;
            pcCopy++;
        } // while
        *pcSet = '\\';
        pcSet++;
        *pcSet = *pcCopy;
        pcSet++;
        pcCopy++;
        pcPos++;
    } // while

    // copy the rest of the string
    strcpy(pcSet, pcCopy);
    pcResultString[iLength + iNumEscapes] = 0;

    return pcResultString;
} // escape_chars()

#if 0
// create a message from a template. Possible placeholders in the template are
// %w: WONID of player entity 
// %n: nickname of player entity

int create_message( char* _pcTarget, size_t _iMaxLen, const char* _pcFormat, edict_t* _ptediEntity ) {

	char* pcPos;
	char* pcFormat;

	pcFormat = new char[strlen(_pcFormat)+1];
	if ( pcFormat == 0 ) return 0; // failed

	memcpy( pcFormat, _pcFormat, strlen(_pcFormat)+1 );
	pcPos = pcFormat;

	while ( (pcPos = strchr(pcPos, '%')) != 0 ) {
		pcPos++;
		switch( *pcPos ) {
		case '%':
			// move remaining string one to the left to delete double %
			memmove( pcPos-1, pcPos, strlen(pcPos)+1 );
			break;
		case 'w':
			// substitute with WONID
		}  // switch
	}  // while 
	
	return 1;
}  // create_message()
#endif

// Option cvars are string cvars which can set multiple options.
// Options are seperated by a colon(:), e.g. "nc1:sp0".
// Option format is an string identifier which is
// a) directly followed by a number if it is a numeric option value: nc0
// b) followed by a period and then a string if it is a string option value: ssl off
//
// If the _pcStringReturn is not 0 then the option value will be written to the string passed.
// The return value is the numeric value of the option or 0 if it is only a string

long int get_option_cvar_value(const char* _pcCvarName,
    const char* _pcOption,
    long int _liNumDefault,
    char* _pcReturnString,
    size_t _tsLen,
    const char* _pcStringDefault)
{

    long int retval = _liNumDefault;
    size_t iOptionIDLen = strlen(_pcOption);

    // set if the option was found
    bool bOptionFound = false;
    // get the CVAR string
    const char* pcOptions = CVAR_GET_STRING(_pcCvarName);
    // find the option identifier
    const char* pcOpt = pcOptions;

    while((pcOpt = strstr(pcOpt, _pcOption)) != 0) {    // if the identifier string was found ...
        if(pcOpt == pcOptions || *(pcOpt - 1) == ':') { // ... and it is indeed the start of an identifier ...
            if((*(pcOpt + iOptionIDLen) == ' ' && *(pcOpt += iOptionIDLen + 1)) ||
                ((*(pcOpt + iOptionIDLen) >= 0x30 && *(pcOpt + iOptionIDLen) <= 0x39) && *(pcOpt += iOptionIDLen))) {
                // ... and it is not only a substring but a complete identifier

                bOptionFound = true;

                // get the numeric value
                retval = strtol(pcOpt, 0, 0);

                if(_pcReturnString != 0) {
                    // if we should return the string value, copy it
                    while((*pcOpt != ':') && (*pcOpt != '\0') && _tsLen) {
                        *_pcReturnString = *pcOpt;
                        ++pcOpt;
                        ++_pcReturnString;
                        --_tsLen;
                    } // while
                }     // if

                break;
            } // if
        }     // if
        ++pcOpt;
    } // while

    // in case the option was not found and we have a default string, copy it over
    if(!bOptionFound && _pcStringDefault != NULL) {
        am_strncpy(_pcReturnString, _pcStringDefault, _tsLen);
    } // if

    return retval;
} // get_option_cvar_value()

void ShowMenu(edict_t* pev, int bitsValidSlots, int nDisplayTime, BOOL fNeedMore, char pszText[1024])
{

    int msgShowMenu = 0;
    if((msgShowMenu = GET_USER_MSG_ID(PLID, "ShowMenu", 0)) == 0) {
        msgShowMenu = REG_USER_MSG("ShowMenu", -1);
    } // if

    MESSAGE_BEGIN(MSG_ONE, msgShowMenu, NULL, pev);

    WRITE_SHORT(bitsValidSlots);
    WRITE_CHAR(nDisplayTime);
    WRITE_BYTE(fNeedMore);
    WRITE_STRING(pszText);

    MESSAGE_END();
}

// Send menu in chunks (max. 512 chars for menu and 176 for one chunk)
void ShowMenu_Large(edict_t* pev, int bitsValidSlots, int nDisplayTime, char pszText[])
{

    int msgShowMenu = 0;
    if((msgShowMenu = GET_USER_MSG_ID(PLID, "ShowMenu", 0)) == 0) {
        msgShowMenu = REG_USER_MSG("ShowMenu", -1);
    } // if

    char* pMenuList = pszText;
    char* aMenuList = pszText;

    char szChunk[MAX_MENU_CHUNK + 1];
    int iCharCount = 0;

    while(pMenuList && *pMenuList) {
        am_strncpy(szChunk, pMenuList, MAX_MENU_CHUNK);

        iCharCount += strlen(szChunk); // Move text to next chunk

        pMenuList = aMenuList + iCharCount;

        MESSAGE_BEGIN(MSG_ONE, msgShowMenu, NULL, pev);
        WRITE_SHORT(bitsValidSlots);
        WRITE_CHAR(nDisplayTime);
        WRITE_BYTE(*pMenuList ? TRUE : FALSE); // Need more menu?
        WRITE_STRING(szChunk);
        MESSAGE_END();
    }
}

void ShowMOTD(edict_t* pev, const char* msg)
{
    int char_count = 0;

    int msgShowMOTD = 0;
    if((msgShowMOTD = GET_USER_MSG_ID(PLID, "MOTD", 0)) == 0) {
        msgShowMOTD = REG_USER_MSG("MOTD", -1);
    } // if

    while(msg && *msg && char_count < MAX_MOTD_LENGTH) {
        char chunk[MAX_MOTD_CHUNK + 1];

        if(strlen(msg) < MAX_MOTD_CHUNK) {
            strcpy(chunk, msg);
        } else {
            strncpy(chunk, msg, MAX_MOTD_CHUNK);
            chunk[MAX_MOTD_CHUNK] = 0;
        }

        char_count += strlen(chunk);

        if(char_count < MAX_MOTD_LENGTH && char_count < strlen(msg))
            msg += strlen(chunk);
        else
            msg = NULL;

        MESSAGE_BEGIN(MSG_ONE, msgShowMOTD, NULL, pev);
        WRITE_BYTE(msg ? FALSE : TRUE); // FALSE means there is still more message to come
        WRITE_STRING(chunk);
        MESSAGE_END();
    }
}

edict_t* UTIL_EntityByIndex(int playerIndex)
{
    edict_t* pPlayerEdict = NULL;

    if(playerIndex > 0 && playerIndex <= gpGlobals->maxClients) {
        pPlayerEdict = INDEXENT(playerIndex);
        if(!pPlayerEdict || pPlayerEdict->free) {
            pPlayerEdict = NULL;
        }
    }
    return pPlayerEdict;
}

CBaseEntity* UTIL_PlayerByIndex(int playerIndex)
{
    CBaseEntity* pPlayer = NULL;

    if(playerIndex > 0 && playerIndex <= gpGlobals->maxClients) {
        edict_t* pPlayerEdict = INDEXENT(playerIndex);
        if(pPlayerEdict && !pPlayerEdict->free) {
            pPlayer = CBaseEntity::Instance(pPlayerEdict);
        }
    }
    return pPlayer;
}

CBaseEntity* UTIL_PlayerByName(const char* name)
{
    int i;
    CBaseEntity* pPlayer = NULL;
    for(i = 1; i <= gpGlobals->maxClients; i++) {
        pPlayer = UTIL_PlayerByIndex(i);
        if(pPlayer) {
            if(FStrEq(STRING(pPlayer->pev->netname), name))
                return pPlayer;
        }
    }
    return NULL;
}

char* UTIL_VarArgs(char* format, ...)
{
    va_list argptr;
    static char acString[1024];

    va_start(argptr, format);
    vsnprintf(acString, sizeof(acString), format, argptr);
    va_end(argptr);

    return acString;
}

//=========================================================
// UTIL_LogPrintf - Prints a logged message to console.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintf(char* fmt, ...)
{
    va_list argptr;
    static char acString[1024];

    va_start(argptr, fmt);
    vsnprintf(acString, sizeof(acString), fmt, argptr);
    va_end(argptr);

    // Print to server console
    ALERT(at_logged, "%s", acString);
}

//=========================================================
// UTIL_LogPrintfFNL - Prints a logged message to console but filter out newlines.
// Preceded by LOG: ( timestamp ) < message >
//=========================================================
void UTIL_LogPrintfFNL(char* fmt, ...)
{
    va_list argptr;
    static char acString[1024];

    va_start(argptr, fmt);
    vsnprintf(acString, sizeof(acString), fmt, argptr);
    va_end(argptr);

    int iNumChar = strlen(acString);
    char* pcBufPos = acString;
    while((pcBufPos = strchr(pcBufPos, '\n'))) {
        if(*(pcBufPos + 1) == '\0')
            break;
        if(1023 - iNumChar) {
            memmove(pcBufPos + 1, pcBufPos, (iNumChar - (pcBufPos - acString) + 1));
            pcBufPos[0] = '\\';
            pcBufPos[1] = 'n';
            iNumChar++;
        } else {
            *pcBufPos = ' ';
        } // if-else
    }     // while

    // Print to server console
    ALERT(at_logged, "%s", acString);
}

//  commented out in favour of ClientPrint loop over all players
//  until Valve has fixed the bot problems -- Removed comments to check if it works on HLSDK 2.3-p4 - [APG]RoboCop[CL]
extern int gmsgTextMsg, gmsgSayText;
void UTIL_ClientPrintAll(int msg_dest,
    const char* msg_name,
    const char* param1,
    const char* param2,
    const char* param3,
    const char* param4)
{
    MESSAGE_BEGIN(MSG_ALL, gmsgTextMsg);
    WRITE_BYTE(msg_dest);
    WRITE_STRING(msg_name);

    if(param1)
        WRITE_STRING(param1);
    if(param2)
        WRITE_STRING(param2);
    if(param3)
        WRITE_STRING(param3);
    if(param4)
        WRITE_STRING(param4);

    MESSAGE_END();
}

CBaseEntity* UTIL_FindEntityByString(CBaseEntity* pStartEntity, const char* szKeyword, const char* szValue)
{
    edict_t* pentEntity;

    if(pStartEntity)
        pentEntity = pStartEntity->edict();
    else
        pentEntity = NULL;

    pentEntity = FIND_ENTITY_BY_STRING(pentEntity, szKeyword, szValue);

    if(!FNullEnt(pentEntity))
        return CBaseEntity::Instance(pentEntity);
    return NULL;
}

CBaseEntity* UTIL_FindEntityByClassname(CBaseEntity* pStartEntity, const char* szName)
{
    return UTIL_FindEntityByString(pStartEntity, "classname", szName);
}

void fix_string(char* str, int len)
{

    for(int i = 0; i < len; i++) {
        if(str[i] == '>' || str[i] == '<' || str[i] == '%')
            strcpy(str + i, str + i + 1);
    }
}

/*********************************************************************
 * new map stuff
 *
 *********************************************************************/
/*
  ==============
  DestroyMapCycle

  Clean up memory used by mapcycle when switching it
  ==============
*/
void DestroyMapCycle(mapcycle_t* cycle)
{
    mapcycle_item_t *p, *n, *start;
    p = cycle->items;
    if(p) {
        start = p;
        p = p->next;
        while(p != start) {
            n = p->next;
            delete p;
            p = n;
        }

        delete cycle->items;
    }
    cycle->items = NULL;
    cycle->next_item = NULL;
}

static char com_token[1500];
/*
  ==============
  COM_Parse

  Parse a token out of a string
  ==============
*/
char* COM_Parse(char* data)
{
    int c;
    int len;

    len = 0;
    com_token[0] = 0;

    if(!data)
        return NULL;

// skip whitespace
skipwhite:
    while((c = *data) <= ' ') {
        if(c == 0)
            return NULL; // end of file;
        data++;
    }

    // skip // comments
    if(c == '/' && data[1] == '/') {
        while(*data && *data != '\n')
            data++;
        goto skipwhite;
    }

    // handle quoted strings specially
    if(c == '\"') {
        data++;
        while(1) {
            c = *data++;
            if(c == '\"' || !c) {
                com_token[len] = 0;
                return data;
            }
            com_token[len] = c;
            len++;
        }
    }

    // parse single characters
    if(c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',') {
        com_token[len] = c;
        len++;
        com_token[len] = 0;
        return data + 1;
    }

    // parse a regular word
    do {
        com_token[len] = c;
        data++;
        len++;
        c = *data;
        if(c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
            break;
    } while(c > 32);

    com_token[len] = 0;
    return data;
}

/*
  ==============
  COM_TokenWaiting

  Returns 1 if additional data is waiting to be processed on this line
  ==============
*/
int COM_TokenWaiting(char* buffer)
{
    char* p;

    p = buffer;
    while(*p && *p != '\n') {
        if(!isspace(*p) || isalnum(*p))
            return 1;

        p++;
    }

    return 0;
}

/*
  ==============
  ReloadMapCycleFile


  Parses mapcycle.txt file into mapcycle_t structure
  ==============
*/
int ReloadMapCycleFile(char* filename, mapcycle_t* cycle)
{
    char szBuffer[MAX_RULE_BUFFER];
    char szMap[MAP_NAME_LENGTH];
    int length;
    char* pFileList;
    char* aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(filename, &length);
    int hasbuffer;
    mapcycle_item_s *item = NULL, *last = NULL;

    if(aFileList == NULL || pFileList == NULL)
        return 0;

    if(pFileList && length) {
        // the first map name in the file becomes the default
        while(1) {
            hasbuffer = 0;
            memset(szBuffer, 0, MAX_RULE_BUFFER);

            pFileList = COM_Parse(pFileList);
            if(strlen(com_token) <= 0)
                break;

            am_strncpy(szMap, com_token, sizeof(szMap));

            // Any more tokens on this line?
            if(COM_TokenWaiting(pFileList)) {
                pFileList = COM_Parse(pFileList);
                if(strlen(com_token) > 0) {
                    hasbuffer = 1;
                    am_strncpy(szBuffer, com_token, sizeof(szMap));
                } // if
            }     // if

            // Check map
            if(IS_MAP_VALID(szMap)) {

                item = new mapcycle_item_s;
                // only set when first item
                if(!last)
                    cycle->items = item;
                if(!last)
                    last = item;

                strcpy(item->mapname, szMap);
                item->next = cycle->items;
                last->next = item;
                last = item;
            } else {
                ALERT(at_console, "Skipping %s from mapcycle, not a valid map\n", szMap);
            } // if-else

        } // while

        FREE_FILE(aFileList);
    } // if

    // Didn't parse anything
    if(!item) {
        return 0;
    } // if

    cycle->next_item = cycle->items->next;

    return 1;
}

mapcycle_item_s* CurrentMap(mapcycle_t* cycle)
{
    mapcycle_item_s* item;
    for(item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next) {
        if(!stricmp(item->mapname, STRING(gpGlobals->mapname)))
            break;
    }

    return (item->next);
}

/*********************************************************************************************
 *
 *
 * check_map - check if we are allowed to vote for that map
 *
 *
 **********************************************************************************************/
int allowed_map(char* map)
{ // is this map in maps.ini ? 1=yes, 0=no

    char* mapcfile = const_cast<char*>(get_cvar_file_value("maps_file"));
    if(mapcfile == NULL)
        return 0;
    int length;
    char* pFileList;
    char* aFileList = pFileList = (char*)LOAD_FILE_FOR_ME(mapcfile, &length);
    if(pFileList && length) {

        // keep pulling mapnames out of the list until we find "map", else return 0
        while(1) {
            while(*pFileList && isspace(*pFileList))
                pFileList++; // skip over any whitespace
            if(!(*pFileList))
                break;

            char cBuf[32];
            int ret = sscanf(pFileList, " %31s", cBuf); // used %31s to prevent overflowing cBuf[32] [APG]RoboCop[CL]
            // Check the map name is valid
            if(ret != 1 || *cBuf < 13)
                break;

            if(FStrEq(cBuf, map)) { // we've found our map;  they can do it!
                return 1;
            }

            pFileList += strlen(cBuf);
        }

        FREE_FILE(aFileList);
    }
    return 0;
}

int listmaps(edict_t* pAdminEnt)
{
    mapcycle_item_s* item;
    char* mapcfile = (char*)CVAR_GET_STRING("mapcyclefile");
    DestroyMapCycle(&mapcycle);
    ReloadMapCycleFile(mapcfile, &mapcycle);

    for(item = mapcycle.next_item; item->next != mapcycle.next_item; item = item->next) {
        System_Response(UTIL_VarArgs("%s\n", item->mapname), pAdminEnt);
    }
    System_Response(UTIL_VarArgs("%s\n", item->mapname), pAdminEnt);

    return 0;
}

int check_map(char* map, int bypass_allowed_map)
{

    if(FStrEq(map, "next_map")) { // next map in the cycle

        mapcycle_item_s* item;
        char* mapcfile = (char*)CVAR_GET_STRING("mapcyclefile");
        DestroyMapCycle(&mapcycle);
        ReloadMapCycleFile(mapcfile, &mapcycle);
        item = CurrentMap(&mapcycle);
        strcpy(map, item->mapname);
        return (IS_MAP_VALID(item->mapname));

    } else { // they have defined the maps.ini file for us

        if(!cvar_file_is_set("maps_file")) { // no maps.ini file defined, just check it is good
            return (IS_MAP_VALID(map));
        }
        if(!IS_MAP_VALID(map))
            return 0; // bad map name

        if(bypass_allowed_map) {
            return 1;
        } else {
            return (allowed_map(map));
        }

    } // end of maps.ini else check

    return 0;
}

static unsigned short FixedUnsigned16(float value, float scale)
{
    int output;

    output = value * scale;
    if(output < 0)
        output = 0;
    if(output > 0xFFFF)
        output = 0xFFFF;

    return (unsigned short)output;
}

static short FixedSigned16(float value, float scale)
{
    int output;

    output = value * scale;

    if(output > 32767)
        output = 32767;

    if(output < -32768)
        output = -32768;

    return (short)output;
}

int ClientCheck(CBaseEntity* pPlayer)
{
    if(!pPlayer || ((int)GETPLAYERWONID(pPlayer->edict()) == 0)
        //||  (pPlayer->pev->flags&FL_SPECTATOR)
        //||  (pPlayer->pev->flags&FL_PROXY)
        )
        return 0;
    else
        return 1;
}

int ClientCheck(edict_t* pEntity)
{
    if(!pEntity || ((int)GETPLAYERWONID(pEntity) == 0)
        //||  (pPlayer->pev->flags&FL_SPECTATOR)
        //||  (pPlayer->pev->flags&FL_PROXY)
        )
        return 0;
    else
        return 1;
}

int ClientCheck(entvars_t* pEntVars)
{
    edict_t* pEntity = FIND_ENTITY_BY_VARS(pEntVars);
    if(!pEntity || ((int)GETPLAYERWONID(pEntity) == 0)
        //||  (pPlayer->pev->flags&FL_SPECTATOR)
        //||  (pPlayer->pev->flags&FL_PROXY)
        )
        return 0;
    else
        return 1;
}

void UTIL_HudMessage(CBaseEntity* pEntity, const hudtextparms_t& textparms, const char* pMessage)
{

    if(ClientCheck(pEntity) == 0)
        return;

    MESSAGE_BEGIN(MSG_ONE, SVC_TEMPENTITY, NULL, pEntity->edict());
    WRITE_BYTE(TE_TEXTMESSAGE);
    WRITE_BYTE(textparms.channel & 0xFF);

    WRITE_SHORT(FixedSigned16(textparms.x, 1 << 13));
    WRITE_SHORT(FixedSigned16(textparms.y, 1 << 13));
    WRITE_BYTE(textparms.effect);

    WRITE_BYTE(textparms.r1);
    WRITE_BYTE(textparms.g1);
    WRITE_BYTE(textparms.b1);
    WRITE_BYTE(textparms.a1);

    WRITE_BYTE(textparms.r2);
    WRITE_BYTE(textparms.g2);
    WRITE_BYTE(textparms.b2);
    WRITE_BYTE(textparms.a2);

    WRITE_SHORT(FixedUnsigned16(textparms.fadeinTime, 1 << 8));
    WRITE_SHORT(FixedUnsigned16(textparms.fadeoutTime, 1 << 8));
    WRITE_SHORT(FixedUnsigned16(textparms.holdTime, 1 << 8));

    if(textparms.effect == 2)
        WRITE_SHORT(FixedUnsigned16(textparms.fxTime, 1 << 8));

    if(strlen(pMessage) < 512) {
        WRITE_STRING(pMessage);
    } else {
        char tmp[512];
        strncpy(tmp, pMessage, 511);
        tmp[511] = 0;
        WRITE_STRING(tmp);
    }
    MESSAGE_END();
}

void UTIL_HudMessageAll(const hudtextparms_t& textparms, const char* pMessage)
{
    int i;

    for(i = 1; i <= gpGlobals->maxClients; i++) {
        CBaseEntity* pPlayer = UTIL_PlayerByIndex(i);

        if(pPlayer)
            UTIL_HudMessage(pPlayer, textparms, pMessage);
    }
}

char* GetModDir()
{
    int iPos;
    static char strGameDir[2048];

    (*g_engfuncs.pfnGetGameDir)(strGameDir);
    iPos = strlen(strGameDir) - 1;

    if(strchr(strGameDir, '/')) {
        // scan backwards till first directory separator...
        while((iPos) && (strGameDir[iPos] != '/'))
            iPos--;

        if(iPos == 0) {
            // Hrmm.  Well, this might be a problem. Return no mod.
            return 0;
        }
        iPos++;
        return &strGameDir[iPos];
    }

    return strGameDir;
}
/* /CEM */

/* Rope's stuff */
void ClientPrint(entvars_t* client,
    int msg_dest,
    const char* msg_name,
    const char* param1,
    const char* param2,
    const char* param3,
    const char* param4)
{

    if(ClientCheck(client) == 0)
        return;

    if((g_msgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", 0)) == 0) {
        g_msgTextMsg = REG_USER_MSG("TextMsg", -1);
    } // if

    MESSAGE_BEGIN(MSG_ONE, g_msgTextMsg, NULL, client);
    WRITE_BYTE(msg_dest);
    WRITE_STRING(msg_name);

    if(param1)
        WRITE_STRING(param1);

    if(param2)
        WRITE_STRING(param2);

    if(param3)
        WRITE_STRING(param3);

    if(param4)
        WRITE_STRING(param4);

    MESSAGE_END();
}

// No longer required [APG]RoboCop[CL]
/*void UTIL_ClientPrintAll( int msg_dest, const char *msg_name, const char *param1, const char *param2, const char
*param3, const char *param4 ) {

  int			i;

  for ( i = 1; i <= gpGlobals->maxClients; i++ )
    {
      CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

      if ( pPlayer )
                  ClientPrint( &(pPlayer->edict()->v), msg_dest, msg_name, param1, param2, param3, param4 );
    }
}*/

void UTIL_ClientPrint_UR(entvars_t* client,
    int msg_dest,
    const char* msg_name,
    const char* param1,
    const char* param2,
    const char* param3,
    const char* param4)
{

    if(ClientCheck(client) == 0)
        return;

    if((g_msgTextMsg = GET_USER_MSG_ID(PLID, "TextMsg", 0)) == 0) {
        g_msgTextMsg = REG_USER_MSG("TextMsg", -1);
    } // if

    MESSAGE_BEGIN(MSG_ONE_UNRELIABLE, g_msgTextMsg, NULL, client);
    WRITE_BYTE(msg_dest);
    WRITE_STRING(msg_name);

    if(param1)
        WRITE_STRING(param1);

    if(param2)
        WRITE_STRING(param2);

    if(param3)
        WRITE_STRING(param3);

    if(param4)
        WRITE_STRING(param4);

    MESSAGE_END();
}

//
// Our own versions of ClientPrintf() and ClientCommand() to try
// and make them more secure for bots until Valve fixes the engine
//
void ClientPrintf(edict_t* pEdict, PRINT_TYPE ptype, const char* szMsg)
{
    if(ptAM_botProtection && (int)ptAM_botProtection->value == 1) {
        if(pEdict && GETPLAYERWONID(pEdict) == 0) {
            return;
        } // if
    }     // if

    CLIENT_PRINTF_ENG(pEdict, ptype, szMsg);
}

void ClientCommand(edict_t* pEdict, char* szFmt, ...)
{

    if(ptAM_botProtection && (int)ptAM_botProtection->value == 1) {
        if(pEdict && GETPLAYERWONID(pEdict) == 0) {
            return;
        } // if
    }     // if

    va_list argptr;
    static char string[1024];

    va_start(argptr, szFmt);
    vsnprintf(string, 1024, szFmt, argptr);
    va_end(argptr);

    CLIENT_COMMAND_ENG(pEdict, string);
}

extern DLL_GLOBAL edict_t* pAdminEnt;

// CEM - Rope's version of my GetPlayerIndex, for partial name matching.
int GetPlayerIndex(char* PlayerText)
{
    int PlayerNumber = 0;
    int i;
    int found = 0;
    int index = 0;
    bool bVerbatim = false;

    // if the string is empty, return failure
    if(*PlayerText == '\0')
        return 0;

    // Check if the passed string is to be taken verbatim (starts with a backslash).
    // This means that it will not be interpreted as an authid or as an wonid.
    // A number will have to be escaped to be interpreted as a name.

    if(*PlayerText == '\\') {
        // cut off the first backslash
        ++PlayerText;
        if(!(*PlayerText == '\\')) {
            // the backslash did not escape another backslash. Switch on verbatim mode
            bVerbatim = true;
        } // if
    }     // if

    AMAuthId oaiAuthID;

    // Get "numeric" representations of the passed string.
    oaiAuthID = PlayerText;
    //-- Set up a numeric id in case we got a SessionID.
    char* pcEndptr = 0;
    PlayerNumber = strtol(PlayerText, &pcEndptr, 10);
    //-- Check if we had a valid number or a string starting with a number.
    //-- Following whitespaces do not count as a string, ie. "123  " is 123.
    for(; *pcEndptr != '\0'; ++pcEndptr) {
        if(*pcEndptr != ' ' && *pcEndptr != '\t') {
            PlayerNumber = 0;
            break;
        } // if
    }     // for

    bool bIsId = (oaiAuthID.is_set() || (PlayerNumber != 0));

    if(!bVerbatim) { // Verbatim means a number is a number. Don't match it on a name.
        for(i = 1; i <= gpGlobals->maxClients; i++) {
            CBaseEntity* pPlayer = UTIL_PlayerByIndex(i);
            if(IsPlayerValid(pPlayer)) {
                // This is only enabled in verbatim mode.
                if(stricmp(STRING(pPlayer->pev->netname), PlayerText) == 0) {
                    return i;
                }

                if(stristr(STRING(pPlayer->pev->netname), PlayerText) != 0) {
                    index = i;
                    found++;
                } // if
            } else {
                DEBUG_LOG(5, ("GetPlayerIndex: player from index %d lookup failed.", i));
            } // if-else
        }     // for

        if(found == 1)
            return index;

    } // if

    if(bIsId) { // We only need to check for Wonid or Sessionid if it actually is an ID.
        for(i = 1; i <= gpGlobals->maxClients; i++) {
            CBaseEntity* pPlayer = UTIL_PlayerByIndex(i);
            if(IsPlayerValid(pPlayer)) {
                if(oaiAuthID.is_set() && (oaiAuthID == GETPLAYERAUTHID(pPlayer->edict()))) {
                    return i;
                }
                if(PlayerNumber != 0 && (PlayerNumber == GETPLAYERUSERID(pPlayer->edict()))) {
                    return i;
                }
            } else {
                DEBUG_LOG(5, ("GetPlayerIndex: player from index %d lookup failed.", i));
            } // if-else
        }     // for
    }         // if

    if(found > 1) {
        System_Response(UTIL_VarArgs("Ambiguous player specification: %s. Be more specific.\n", PlayerText), pAdminEnt);
    }
    // /FZ
    return 0;
}
/* /Rope's stuff */

int GetPlayerCount(edict_t* peIgnorePlayer)
{
    int i;
    int iPlayerCount;

    iPlayerCount = 0;
    for(i = 1; i <= gpGlobals->maxClients; i++) {
        edict_t* pPlayer = UTIL_EntityByIndex(i);
        if(IsPlayerValid(pPlayer) && (pPlayer != peIgnorePlayer)) {
            iPlayerCount++;
        } // if
    }     // for
    return iPlayerCount;
}

/*
 * get_file_path( )
 *
 * Get the path of a filename relative to the game directory. The path is adapted to the OS.
 *
 * pcPath      : string into which the path is copied
 * pcFilename  : name of the file to search. Can contain a relative path.
 * iMaxLen     : maximum path length
 * pcAccessCvar: cvar name that control access to this file operation, eg. file_access_read
 *               If set to NULL no access permissions are checked
 *
 * Returns 1 if path was created, -1 on error or 0 if access denied.
 *
 */
int get_file_path(char* pcPath, char* pcFilename, int iMaxLen, const char* pcAccessCvar)
{
#ifdef WIN32
    const char c_acDirSep[] = "\\";
    const char c_cDirSep = '\\';
#else
    const char c_acDirSep[] = "/";
    const char c_cDirSep = '/';
#endif

    if(pcAccessCvar == 0 || (int)CVAR_GET_FLOAT(pcAccessCvar) == 1) {

        char acFilePath[PATH_MAX];
        memset(acFilePath, 0, PATH_MAX);

        GET_GAME_DIR(acFilePath);

        int iPathLen = strlen(acFilePath);

        /* check if the file path is too long for our buffer */
        iPathLen += 1 + strlen(pcFilename);
        if(iPathLen >= PATH_MAX || iPathLen >= iMaxLen) {
            return -1;
        } // if

        /* everything looks ok, we continue */
        strcat(acFilePath, c_acDirSep);
        strcat(acFilePath, pcFilename);

        char* pcPathSep = acFilePath;
        while(pcPathSep != 0 && *pcPathSep != '\0') {
#ifdef WIN32
            if(*pcPathSep == '/') {
                *pcPathSep = '\\';
            } // if
#else
            if(*pcPathSep == '\\') {
                *pcPathSep = '/';
            } // if
#endif
            pcPathSep++;
        } // while

        pcPathSep = 0;

        strncpy(pcPath, acFilePath, iMaxLen);

        return 1;
    } else {

        return 0;
    } // if-else

} // getpath()

int get_player_team(CBaseEntity* poPlayer)
{

    // check for a spectator or the proxy

    if((poPlayer->pev->flags & FL_PROXY)) {
        return AM_TEAM_PROXY;
    } // if

    if((poPlayer->pev->flags & FL_SPECTATOR)) {
        return AM_TEAM_SPECTATOR;
    } // if

    char* pcMod = GetModDir();

    if(strcmp(pcMod, "cstrike") == 0) {

        /* special care for CS */
        char* pcInfobuffer = g_engfuncs.pfnGetInfoKeyBuffer(poPlayer->edict());
        char acModel[30];

        memset(acModel, 0, 30);
        strncpy(acModel, g_engfuncs.pfnInfoKeyValue(pcInfobuffer, "model"), 30);

        if(!strcmp(acModel, "vip"))
            return 4;

        if(!strcmp(acModel, "sas") || !strcmp(acModel, "gsg9") || !strcmp(acModel, "gign") ||
            !strcmp(acModel, "urban")) {
            return 2;
        } // if

        if(!strcmp(acModel, "arctic") || !strcmp(acModel, "leet") || !strcmp(acModel, "arab") ||
            !strcmp(acModel, "guerilla") || !strcmp(acModel, "terror")) {
            return 1;
        } // if

    } else {
        return poPlayer->edict()->v.team;
    } // if-else

    return -1;
} // get_player_team()

int util_kick_player(int _iSessionId, const char* _pcKickMsg)
{
    if(NULL == _pcKickMsg) {
        DEBUG_LOG(2, ("Running server command 'kick # %i'", _iSessionId));
        SERVER_COMMAND(UTIL_VarArgs("kick # %i\n", _iSessionId));
    } else {
        DEBUG_LOG(2, ("Running server command 'kick # %i %s'", _iSessionId, _pcKickMsg));
        SERVER_COMMAND(UTIL_VarArgs("kick # %i %s\n", _iSessionId, _pcKickMsg));
    }

    return 1;
}

int util_kick_player(edict_t* _peEntity, const char* _pcKickMsg)
{
    if(NULL == _peEntity)
        return 0;
    int iSID = GETPLAYERUSERID(_peEntity);
    if(0 == iSID)
        return 0;
    util_kick_player(iSID, _pcKickMsg);
    return 1;
}

void util_kill_player(CBaseEntity* pPlayer)
{

#if 0 // Try number 1
	CBasePlayer *pl = (CBasePlayer*) CBasePlayer::Instance( pPlayer->pev );
	pPlayer->edict()->v.health = 0;
	//pPlayer->Killed( pPlayer->pev, GIB_NEVER );
	pPlayer->edict()->v.deadflag = DEAD_DYING;
#endif

    MDLL_ClientKill(pPlayer->edict());

} // util_kill_player()

#ifdef USE_MYSQL

MYSQL_RES* admin_mysql_query(char* sQuery /* the SQL query */, char* sType /* string name for the type of query*/)
{

    if(sQuery == NULL)
        return NULL;

    int iSQLErr; // used for db func return values
    MYSQL_RES* pResult = NULL;
    MYSQL_ROW pRow = NULL;

    if((int)CVAR_GET_FLOAT("admin_debug") != 0)
        UTIL_LogPrintf("[ADMIN] MYSQL: Running query \"%s\"\n", sQuery);

    iSQLErr = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
    if(iSQLErr) {
        if(mysql_errno(&mysql) == CR_SERVER_LOST || mysql_errno(&mysql) == CR_SERVER_GONE_ERROR) {
            iSQLErr = mysql_ping(&mysql);
            if(!iSQLErr) {
                UTIL_LogPrintf(
                    "[ADMIN] WARNING: Select query for %ss returned disconnect, reconnect succeeded.\n", sType);
                iSQLErr = mysql_real_query(&mysql, sQuery, (unsigned int)strlen(sQuery));
            } else {
                UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned disconnect, reconnect failed.\n", sType);
                return NULL;
            }
        } else if(mysql_errno(&mysql) == CR_COMMANDS_OUT_OF_SYNC) {
            UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned commands out of sync!  NO FURTHER SQL QUERIES "
                           "WILL WORK.\n",
                sType);
            return NULL;
        } else {
            UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned error: \"%s\"\n", sType, mysql_error(&mysql));
            return NULL;
        }
    }
    if(!iSQLErr) {
        pResult = mysql_store_result(&mysql);
    }

    return pResult;
} // admin_mysql_query()

const char* am_mysql_password_encrypt(const char* _pcPassword)
{

    static char acPassword[PASSWORD_SIZE];

    if(_pcPassword == NULL)
        return NULL;

    // Password cache: if we get passed the same password twice we can
    // reuse the result from the last encryption.
    static char acPwCache[PASSWORD_SIZE] = "\01\02";
    if(strncmp(acPwCache, _pcPassword, PASSWORD_SIZE) == 0) {
        DEVEL_LOG(3, ("mysql_password_encrypt() returning cached password: '%s'", acPassword));
        return acPassword;
    } // if
    am_strncpy(acPwCache, _pcPassword, PASSWORD_SIZE);

    int iSQLErr;
    MYSQL_RES* pResult = NULL;
    MYSQL_ROW pRow = NULL;

    char acQuery[17 + PASSWORD_SIZE + 3] = "SELECT PASSWORD('";
    strncat(acQuery, _pcPassword, PASSWORD_SIZE);
    strcat(acQuery, "')");

    iSQLErr = mysql_query(&mysql, acQuery);
    if(iSQLErr) {
        if(mysql_errno(&mysql) == CR_SERVER_LOST || mysql_errno(&mysql) == CR_SERVER_GONE_ERROR) {
            iSQLErr = mysql_ping(&mysql);
            if(!iSQLErr) {
                UTIL_LogPrintf(
                    "[ADMIN] WARNING: Select query for PASSWORD() returned disconnect, reconnect succeeded.\n");
                iSQLErr = mysql_query(&mysql, acQuery);
            } else {
                UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned disconnect, reconnect failed.\n");
                return NULL;
            }
        } else if(mysql_errno(&mysql) == CR_COMMANDS_OUT_OF_SYNC) {
            UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned commands out of sync!  NO FURTHER SQL "
                           "QUERIES WILL WORK.\n");
            return NULL;
        } else {
            UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned error: \"%s\"\n", mysql_error(&mysql));
            return NULL;
        } // if-else
    }     // if

    if(!iSQLErr) {
        pResult = mysql_use_result(&mysql);
    } // if

    if(iSQLErr || pResult == NULL) {
        UTIL_LogPrintf("[ADMIN] ERROR: Select query for PASSWORD() returned NULL result.\n");
        return NULL;
    } else {
        if((pRow = mysql_fetch_row(pResult)) != NULL) {
            am_strncpy(acPassword, pRow[0], PASSWORD_SIZE);
        } else {
            mysql_free_result(pResult);
            return NULL;
        }

        mysql_free_result(pResult);
    } // if-else

    DEVEL_LOG(3, ("mysql PASSWORD() encrypted password is '%s'", acPassword));

    return acPassword;
} // am_mysql_password_encrypt()

#endif

#ifdef USE_PGSQL

PGresult* admin_pgsql_query(char* sQuery /* the SQL query */, char* sType /* string name for the type of query*/)
{

    if(sQuery == NULL)
        return NULL;

    if(g_fUsePgSQL == FALSE) {
        return FALSE;
    }

    PGresult* pResult = NULL;

    if((int)CVAR_GET_FLOAT("admin_debug") != 0)
        UTIL_LogPrintf("[ADMIN] PGSQL: Running query \"%s\"\n", sQuery);

    pResult = PQexec(pgsql, sQuery);
    if((PQresultStatus(pResult) != PGRES_TUPLES_OK) && (PQstatus(pgsql) != CONNECTION_OK)) {
        PQreset(pgsql);
        if(PQstatus(pgsql) == CONNECTION_OK) {
            UTIL_LogPrintf("[ADMIN] WARNING: Select query for %ss returned disconnect, reconnect succeeded.\n", sType);
            pResult = PQexec(pgsql, sQuery);
        } else {
            UTIL_LogPrintf("[ADMIN] ERROR: Select query for %ss returned disconnect, reconnect failed.\n", sType);
            return NULL;
        }
    }

    if(PQresultStatus(pResult) != PGRES_TUPLES_OK) {
        UTIL_LogPrintf(
            "[ADMIN] ERROR: Select query for %ss returned error: \"%s\"\n", sType, PQresultErrorMessage(pResult));
        return NULL;
    }

    return pResult;
} // admin_pgsql_query()

#endif

//---Required for newer AdminMod Project? [APG]RoboCop[CL]
// --------------------------------------------------------------
//
// CSave
//
// --------------------------------------------------------------
static int gSizes[FIELD_TYPECOUNT] = 
{
	sizeof(float),		// FIELD_FLOAT
	sizeof(int),		// FIELD_STRING
	sizeof(int),		// FIELD_ENTITY
	sizeof(int),		// FIELD_CLASSPTR
	sizeof(int),		// FIELD_EHANDLE
	sizeof(int),		// FIELD_entvars_t
	sizeof(int),		// FIELD_EDICT
	sizeof(float)*3,	// FIELD_VECTOR
	sizeof(float)*3,	// FIELD_POSITION_VECTOR
	sizeof(int *),		// FIELD_POINTER
	sizeof(int),		// FIELD_INTEGER
#ifdef GNUC
	sizeof(int *)*2,		// FIELD_FUNCTION
#else
	sizeof(int *),		// FIELD_FUNCTION	
#endif
	sizeof(int),		// FIELD_BOOLEAN
	sizeof(short),		// FIELD_SHORT
	sizeof(char),		// FIELD_CHARACTER
	sizeof(float),		// FIELD_TIME
	sizeof(int),		// FIELD_MODELNAME
	sizeof(int),		// FIELD_SOUNDNAME
};


// Base class includes common SAVERESTOREDATA pointer, and manages the entity table
CSaveRestoreBuffer :: CSaveRestoreBuffer( void )
{
	m_pdata = NULL;
}


CSaveRestoreBuffer :: CSaveRestoreBuffer( SAVERESTOREDATA *pdata )
{
	m_pdata = pdata;
}


CSaveRestoreBuffer :: ~CSaveRestoreBuffer( void )
{
}

int	CSaveRestoreBuffer :: EntityIndex( CBaseEntity *pEntity )
{
	if ( pEntity == NULL )
		return -1;
	return EntityIndex( pEntity->pev );
}


int	CSaveRestoreBuffer :: EntityIndex( entvars_t *pevLookup )
{
	if ( pevLookup == NULL )
		return -1;
	return EntityIndex( ENT( pevLookup ) );
}

int	CSaveRestoreBuffer :: EntityIndex( EOFFSET eoLookup )
{
	return EntityIndex( ENT( eoLookup ) );
}


int	CSaveRestoreBuffer :: EntityIndex( edict_t *pentLookup )
{
	if ( !m_pdata || pentLookup == NULL )
		return -1;

	int i;
	ENTITYTABLE *pTable;

	for ( i = 0; i < m_pdata->tableCount; i++ )
	{
		pTable = m_pdata->pTable + i;
		if ( pTable->pent == pentLookup )
			return i;
	}
	return -1;
}


edict_t *CSaveRestoreBuffer :: EntityFromIndex( int entityIndex )
{
	if ( !m_pdata || entityIndex < 0 )
		return NULL;

	int i;
	ENTITYTABLE *pTable;

	for ( i = 0; i < m_pdata->tableCount; i++ )
	{
		pTable = m_pdata->pTable + i;
		if ( pTable->id == entityIndex )
			return pTable->pent;
	}
	return NULL;
}


int	CSaveRestoreBuffer :: EntityFlagsSet( int entityIndex, int flags )
{
	if ( !m_pdata || entityIndex < 0 )
		return 0;
	if ( entityIndex > m_pdata->tableCount )
		return 0;

	m_pdata->pTable[ entityIndex ].flags |= flags;

	return m_pdata->pTable[ entityIndex ].flags;
}


void CSaveRestoreBuffer :: BufferRewind( int size )
{
	if ( !m_pdata )
		return;

	if ( m_pdata->size < size )
		size = m_pdata->size;

	m_pdata->pCurrentData -= size;
	m_pdata->size -= size;
}

#ifndef _WIN32
extern "C" {
unsigned _rotr ( unsigned val, int shift)
{
        register unsigned lobit;        /* non-zero means lo bit set */
        register unsigned num = val;    /* number to rotate */

        shift &= 0x1f;                  /* modulo 32 -- this will also make
                                           negative shifts work */

        while (shift--) {
                lobit = num & 1;        /* get high bit */
                num >>= 1;              /* shift right one bit */
                if (lobit)
                        num |= 0x80000000;  /* set hi bit if lo bit was set */
        }

        return num;
}
}
#endif

unsigned int CSaveRestoreBuffer :: HashString( const char *pszToken )
{
	unsigned int	hash = 0;

	while ( *pszToken )
		hash = _rotr( hash, 4 ) ^ *pszToken++;

	return hash;
}

unsigned short CSaveRestoreBuffer :: TokenHash( const char *pszToken )
{
	unsigned short	hash = (unsigned short)(HashString( pszToken ) % (unsigned)m_pdata->tokenCount );
	
#if _DEBUG
	static int tokensparsed = 0;
	tokensparsed++;
	if ( !m_pdata->tokenCount || !m_pdata->pTokens )
		ALERT( at_error, "No token table array in TokenHash()!" );
#endif

	for ( int i=0; i<m_pdata->tokenCount; i++ )
	{
#if _DEBUG
		static qboolean beentheredonethat = FALSE;
		if ( i > 50 && !beentheredonethat )
		{
			beentheredonethat = TRUE;
			ALERT( at_error, "CSaveRestoreBuffer :: TokenHash() is getting too full!" );
		}
#endif

		int	index = hash + i;
		if ( index >= m_pdata->tokenCount )
			index -= m_pdata->tokenCount;

		if ( !m_pdata->pTokens[index] || strcmp( pszToken, m_pdata->pTokens[index] ) == 0 )
		{
			m_pdata->pTokens[index] = (char *)pszToken;
			return index;
		}
	}
		
	// Token hash table full!!! 
	// [Consider doing overflow table(s) after the main table & limiting linear hash table search]
	ALERT( at_error, "CSaveRestoreBuffer :: TokenHash() is COMPLETELY FULL!" );
	return 0;
}

void CSave :: WriteData( const char *pname, int size, const char *pdata )
{
	BufferField( pname, size, pdata );
}


void CSave :: WriteShort( const char *pname, const short *data, int count )
{
	BufferField( pname, sizeof(short) * count, (const char *)data );
}


void CSave :: WriteInt( const char *pname, const int *data, int count )
{
	BufferField( pname, sizeof(int) * count, (const char *)data );
}


void CSave :: WriteFloat( const char *pname, const float *data, int count )
{
	BufferField( pname, sizeof(float) * count, (const char *)data );
}


void CSave :: WriteTime( const char *pname, const float *data, int count )
{
	int i;
	Vector tmp, input;

	BufferHeader( pname, sizeof(float) * count );
	for ( i = 0; i < count; i++ )
	{
		float tmp = data[0];

		// Always encode time as a delta from the current time so it can be re-based if loaded in a new level
		// Times of 0 are never written to the file, so they will be restored as 0, not a relative time
		if ( m_pdata )
			tmp -= m_pdata->time;

		BufferData( (const char *)&tmp, sizeof(float) );
		data ++;
	}
}


void CSave :: WriteString( const char *pname, const char *pdata )
{
#ifdef TOKENIZE
	short	token = (short)TokenHash( pdata );
	WriteShort( pname, &token, 1 );
#else
	BufferField( pname, strlen(pdata) + 1, pdata );
#endif
}


void CSave :: WriteString( const char *pname, const int *stringId, int count )
{
	int i, size;

#ifdef TOKENIZE
	short	token = (short)TokenHash( STRING( *stringId ) );
	WriteShort( pname, &token, 1 );
#else
#if 0
	if ( count != 1 )
		ALERT( at_error, "No string arrays!\n" );
	WriteString( pname, (char *)STRING(*stringId) );
#endif

	size = 0;
	for ( i = 0; i < count; i++ )
		size += strlen( STRING( stringId[i] ) ) + 1;

	BufferHeader( pname, size );
	for ( i = 0; i < count; i++ )
	{
		const char *pString = STRING(stringId[i]);
		BufferData( pString, strlen(pString)+1 );
	}
#endif
}


void CSave :: WriteVector( const char *pname, const Vector &value )
{
	WriteVector( pname, &value.x, 1 );
}


void CSave :: WriteVector( const char *pname, const float *value, int count )
{
	BufferHeader( pname, sizeof(float) * 3 * count );
	BufferData( (const char *)value, sizeof(float) * 3 * count );
}



void CSave :: WritePositionVector( const char *pname, const Vector &value )
{

	if ( m_pdata && m_pdata->fUseLandmark )
	{
		Vector tmp = value - m_pdata->vecLandmarkOffset;
		WriteVector( pname, tmp );
	}

	WriteVector( pname, value );
}


void CSave :: WritePositionVector( const char *pname, const float *value, int count )
{
	int i;
	Vector tmp, input;

	BufferHeader( pname, sizeof(float) * 3 * count );
	for ( i = 0; i < count; i++ )
	{
		Vector tmp( value[0], value[1], value[2] );

		if ( m_pdata && m_pdata->fUseLandmark )
			tmp = tmp - m_pdata->vecLandmarkOffset;

		BufferData( (const char *)&tmp.x, sizeof(float) * 3 );
		value += 3;
	}
}


void CSave :: WriteFunction( const char *pname, void **data, int count )
{
	const char *functionName;

	functionName = NAME_FOR_FUNCTION( (uint32)*data );
	if ( functionName )
		BufferField( pname, strlen(functionName) + 1, functionName );
	else
		ALERT( at_error, "Invalid function pointer in entity!" );
}


void EntvarsKeyvalue( entvars_t *pev, KeyValueData *pkvd )
{
	int i;
	TYPEDESCRIPTION		*pField;

	for ( i = 0; i < ENTVARS_COUNT; i++ )
	{
		pField = &gEntvarsDescription[i];

		if ( !stricmp( pField->fieldName, pkvd->szKeyName ) )
		{
			switch( pField->fieldType )
			{
			case FIELD_MODELNAME:
			case FIELD_SOUNDNAME:
			case FIELD_STRING:
				(*(int *)((char *)pev + pField->fieldOffset)) = ALLOC_STRING( pkvd->szValue );
				break;

			case FIELD_TIME:
			case FIELD_FLOAT:
				(*(float *)((char *)pev + pField->fieldOffset)) = atof( pkvd->szValue );
				break;

			case FIELD_INTEGER:
				(*(int *)((char *)pev + pField->fieldOffset)) = atoi( pkvd->szValue );
				break;

			case FIELD_POSITION_VECTOR:
			case FIELD_VECTOR:
				UTIL_StringToVector( (float *)((char *)pev + pField->fieldOffset), pkvd->szValue );
				break;

			default:
			case FIELD_EVARS:
			case FIELD_CLASSPTR:
			case FIELD_EDICT:
			case FIELD_ENTITY:
			case FIELD_POINTER:
				ALERT( at_error, "Bad field in entity!!\n" );
				break;
			}
			pkvd->fHandled = TRUE;
			return;
		}
	}
}



int CSave :: WriteEntVars( const char *pname, entvars_t *pev )
{
	return WriteFields( pname, pev, gEntvarsDescription, ENTVARS_COUNT );
}



int CSave :: WriteFields( const char *pname, void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount )
{
	int				i, j, actualCount, emptyCount;
	TYPEDESCRIPTION	*pTest;
	int				entityArray[MAX_ENTITYARRAY];

	// Precalculate the number of empty fields
	emptyCount = 0;
	for ( i = 0; i < fieldCount; i++ )
	{
		void *pOutputData;
		pOutputData = ((char *)pBaseData + pFields[i].fieldOffset );
		if ( DataEmpty( (const char *)pOutputData, pFields[i].fieldSize * gSizes[pFields[i].fieldType] ) )
			emptyCount++;
	}

	// Empty fields will not be written, write out the actual number of fields to be written
	actualCount = fieldCount - emptyCount;
	WriteInt( pname, &actualCount, 1 );

	for ( i = 0; i < fieldCount; i++ )
	{
		void *pOutputData;
		pTest = &pFields[ i ];
		pOutputData = ((char *)pBaseData + pTest->fieldOffset );

		// UNDONE: Must we do this twice?
		if ( DataEmpty( (const char *)pOutputData, pTest->fieldSize * gSizes[pTest->fieldType] ) )
			continue;

		switch( pTest->fieldType )
		{
		case FIELD_FLOAT:
			WriteFloat( pTest->fieldName, (float *)pOutputData, pTest->fieldSize );
		break;
		case FIELD_TIME:
			WriteTime( pTest->fieldName, (float *)pOutputData, pTest->fieldSize );
		break;
		case FIELD_MODELNAME:
		case FIELD_SOUNDNAME:
		case FIELD_STRING:
			WriteString( pTest->fieldName, (int *)pOutputData, pTest->fieldSize );
		break;
		case FIELD_CLASSPTR:
		case FIELD_EVARS:
		case FIELD_EDICT:
		case FIELD_ENTITY:
		case FIELD_EHANDLE:
			if ( pTest->fieldSize > MAX_ENTITYARRAY )
				ALERT( at_error, "Can't save more than %d entities in an array!!!\n", MAX_ENTITYARRAY );
			for ( j = 0; j < pTest->fieldSize; j++ )
			{
				switch( pTest->fieldType )
				{
					case FIELD_EVARS:
						entityArray[j] = EntityIndex( ((entvars_t **)pOutputData)[j] );
					break;
					case FIELD_CLASSPTR:
						entityArray[j] = EntityIndex( ((CBaseEntity **)pOutputData)[j] );
					break;
					case FIELD_EDICT:
						entityArray[j] = EntityIndex( ((edict_t **)pOutputData)[j] );
					break;
					case FIELD_ENTITY:
						entityArray[j] = EntityIndex( ((EOFFSET *)pOutputData)[j] );
					break;
					case FIELD_EHANDLE:
						entityArray[j] = EntityIndex( (CBaseEntity *)(((EHANDLE *)pOutputData)[j]) );
					break;
				}
			}
			WriteInt( pTest->fieldName, entityArray, pTest->fieldSize );
		break;
		case FIELD_POSITION_VECTOR:
			WritePositionVector( pTest->fieldName, (float *)pOutputData, pTest->fieldSize );
		break;
		case FIELD_VECTOR:
			WriteVector( pTest->fieldName, (float *)pOutputData, pTest->fieldSize );
		break;

		case FIELD_BOOLEAN:
		case FIELD_INTEGER:
			WriteInt( pTest->fieldName, (int *)pOutputData, pTest->fieldSize );
		break;

		case FIELD_SHORT:
			WriteData( pTest->fieldName, 2 * pTest->fieldSize, ((char *)pOutputData) );
		break;

		case FIELD_CHARACTER:
			WriteData( pTest->fieldName, pTest->fieldSize, ((char *)pOutputData) );
		break;

		// For now, just write the address out, we're not going to change memory while doing this yet!
		case FIELD_POINTER:
			WriteInt( pTest->fieldName, (int *)(char *)pOutputData, pTest->fieldSize );
		break;

		case FIELD_FUNCTION:
			WriteFunction( pTest->fieldName, (void **)pOutputData, pTest->fieldSize );
		break;
		default:
			ALERT( at_error, "Bad field type\n" );
		}
	}

	return 1;
}


void CSave :: BufferString( char *pdata, int len )
{
	char c = 0;

	BufferData( pdata, len );		// Write the string
	BufferData( &c, 1 );			// Write a null terminator
}


int CSave :: DataEmpty( const char *pdata, int size )
{
	for ( int i = 0; i < size; i++ )
	{
		if ( pdata[i] )
			return 0;
	}
	return 1;
}


void CSave :: BufferField( const char *pname, int size, const char *pdata )
{
	BufferHeader( pname, size );
	BufferData( pdata, size );
}


void CSave :: BufferHeader( const char *pname, int size )
{
	short	hashvalue = TokenHash( pname );
	if ( size > 1<<(sizeof(short)*8) )
		ALERT( at_error, "CSave :: BufferHeader() size parameter exceeds 'short'!" );
	BufferData( (const char *)&size, sizeof(short) );
	BufferData( (const char *)&hashvalue, sizeof(short) );
}


void CSave :: BufferData( const char *pdata, int size )
{
	if ( !m_pdata )
		return;

	if ( m_pdata->size + size > m_pdata->bufferSize )
	{
		ALERT( at_error, "Save/Restore overflow!" );
		m_pdata->size = m_pdata->bufferSize;
		return;
	}

	memcpy( m_pdata->pCurrentData, pdata, size );
	m_pdata->pCurrentData += size;
	m_pdata->size += size;
}



// --------------------------------------------------------------
//
// CRestore
//
// --------------------------------------------------------------

int CRestore::ReadField( void *pBaseData, TYPEDESCRIPTION *pFields, int fieldCount, int startField, int size, char *pName, void *pData )
{
	int i, j, stringCount, fieldNumber, entityIndex;
	TYPEDESCRIPTION *pTest;
	float	time, timeData;
	Vector	position;
	edict_t	*pent;
	char	*pString;

	time = 0;
	position = Vector(0,0,0);

	if ( m_pdata )
	{
		time = m_pdata->time;
		if ( m_pdata->fUseLandmark )
			position = m_pdata->vecLandmarkOffset;
	}

	for ( i = 0; i < fieldCount; i++ )
	{
		fieldNumber = (i+startField)%fieldCount;
		pTest = &pFields[ fieldNumber ];
		if ( !stricmp( pTest->fieldName, pName ) )
		{
			if ( !m_global || !(pTest->flags & FTYPEDESC_GLOBAL) )
			{
				for ( j = 0; j < pTest->fieldSize; j++ )
				{
					void *pOutputData = ((char *)pBaseData + pTest->fieldOffset + (j*gSizes[pTest->fieldType]) );
					void *pInputData = (char *)pData + j * gSizes[pTest->fieldType];

					switch( pTest->fieldType )
					{
					case FIELD_TIME:
						timeData = *(float *)pInputData;
						// Re-base time variables
						timeData += time;
						*((float *)pOutputData) = timeData;
					break;
					case FIELD_FLOAT:
						*((float *)pOutputData) = *(float *)pInputData;
					break;
					case FIELD_MODELNAME:
					case FIELD_SOUNDNAME:
					case FIELD_STRING:
						// Skip over j strings
						pString = (char *)pData;
						for ( stringCount = 0; stringCount < j; stringCount++ )
						{
							while (*pString)
								pString++;
							pString++;
						}
						pInputData = pString;
						if ( strlen( (char *)pInputData ) == 0 )
							*((int *)pOutputData) = 0;
						else
						{
							int string;

							string = ALLOC_STRING( (char *)pInputData );
							
							*((int *)pOutputData) = string;

							if ( !FStringNull( string ) && m_precache )
							{
								if ( pTest->fieldType == FIELD_MODELNAME )
									PRECACHE_MODEL( (char *)STRING( string ) );
								else if ( pTest->fieldType == FIELD_SOUNDNAME )
									PRECACHE_SOUND( (char *)STRING( string ) );
							}
						}
					break;
					case FIELD_EVARS:
						entityIndex = *( int *)pInputData;
						pent = EntityFromIndex( entityIndex );
						if ( pent )
							*((entvars_t **)pOutputData) = VARS(pent);
						else
							*((entvars_t **)pOutputData) = NULL;
					break;
					case FIELD_CLASSPTR:
						entityIndex = *( int *)pInputData;
						pent = EntityFromIndex( entityIndex );
						if ( pent )
							*((CBaseEntity **)pOutputData) = CBaseEntity::Instance(pent);
						else
							*((CBaseEntity **)pOutputData) = NULL;
					break;
					case FIELD_EDICT:
						entityIndex = *( int *)pInputData;
						pent = EntityFromIndex( entityIndex );
						*((edict_t **)pOutputData) = pent;
					break;
					case FIELD_EHANDLE:
						// Input and Output sizes are different!
						pOutputData = (char *)pOutputData + j*(sizeof(EHANDLE) - gSizes[pTest->fieldType]);
						entityIndex = *( int *)pInputData;
						pent = EntityFromIndex( entityIndex );
						if ( pent )
							*((EHANDLE *)pOutputData) = CBaseEntity::Instance(pent);
						else
							*((EHANDLE *)pOutputData) = NULL;
					break;
					case FIELD_ENTITY:
						entityIndex = *( int *)pInputData;
						pent = EntityFromIndex( entityIndex );
						if ( pent )
							*((EOFFSET *)pOutputData) = OFFSET(pent);
						else
							*((EOFFSET *)pOutputData) = 0;
					break;
					case FIELD_VECTOR:
						((float *)pOutputData)[0] = ((float *)pInputData)[0];
						((float *)pOutputData)[1] = ((float *)pInputData)[1];
						((float *)pOutputData)[2] = ((float *)pInputData)[2];
					break;
					case FIELD_POSITION_VECTOR:
						((float *)pOutputData)[0] = ((float *)pInputData)[0] + position.x;
						((float *)pOutputData)[1] = ((float *)pInputData)[1] + position.y;
						((float *)pOutputData)[2] = ((float *)pInputData)[2] + position.z;
					break;

					case FIELD_BOOLEAN:
					case FIELD_INTEGER:
						*((int *)pOutputData) = *( int *)pInputData;
					break;

					case FIELD_SHORT:
						*((short *)pOutputData) = *( short *)pInputData;
					break;

					case FIELD_CHARACTER:
						*((char *)pOutputData) = *( char *)pInputData;
					break;

					case FIELD_POINTER:
						*((int *)pOutputData) = *( int *)pInputData;
					break;
					case FIELD_FUNCTION:
						if ( strlen( (char *)pInputData ) == 0 )
							*((int *)pOutputData) = 0;
						else
							*((int *)pOutputData) = FUNCTION_FROM_NAME( (char *)pInputData );
					break;

					default:
						ALERT( at_error, "Bad field type\n" );
					}
				}
			}
#if 0
			else
			{
				ALERT( at_console, "Skipping global field %s\n", pName );
			}
#endif
			return fieldNumber;
		}
	}

	return -1;
}

BOOL UTIL_ShouldShowBlood( int color )
{
	if ( color != DONT_BLEED )
	{
		if ( color == BLOOD_COLOR_RED )
		{
			if ( CVAR_GET_FLOAT("violence_hblood") != 0 )
				return TRUE;
		}
		else
		{
			if ( CVAR_GET_FLOAT("violence_ablood") != 0 )
				return TRUE;
		}
	}
	return FALSE;
}

int UTIL_PointContents(	const Vector &vec )
{
	return POINT_CONTENTS(vec);
}

void UTIL_BloodStream( const Vector &origin, const Vector &direction, int color, int amount )
{
	if ( !UTIL_ShouldShowBlood( color ) )
		return;

	if ( g_Language == LANGUAGE_GERMAN && color == BLOOD_COLOR_RED )
		color = 0;

	
	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, origin );
		WRITE_BYTE( TE_BLOODSTREAM );
		WRITE_COORD( origin.x );
		WRITE_COORD( origin.y );
		WRITE_COORD( origin.z );
		WRITE_COORD( direction.x );
		WRITE_COORD( direction.y );
		WRITE_COORD( direction.z );
		WRITE_BYTE( color );
		WRITE_BYTE( min( amount, 255 ) );
	MESSAGE_END();
}				

void UTIL_BloodDrips( const Vector &origin, const Vector &direction, int color, int amount )
{
	if ( !UTIL_ShouldShowBlood( color ) )
		return;

	if ( color == DONT_BLEED || amount == 0 )
		return;

	if ( g_Language == LANGUAGE_GERMAN && color == BLOOD_COLOR_RED )
		color = 0;

	if ( g_pGameRules->IsMultiplayer() )
	{
		// scale up blood effect in multiplayer for better visibility
		amount *= 2;
	}

	if ( amount > 255 )
		amount = 255;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, origin );
		WRITE_BYTE( TE_BLOODSPRITE );
		WRITE_COORD( origin.x);								// pos
		WRITE_COORD( origin.y);
		WRITE_COORD( origin.z);
		WRITE_SHORT( g_sModelIndexBloodSpray );				// initial sprite model
		WRITE_SHORT( g_sModelIndexBloodDrop );				// droplet sprite models
		WRITE_BYTE( color );								// color index into host_basepal
		WRITE_BYTE( min( max( 3, amount / 10 ), 16 ) );		// size
	MESSAGE_END();
}				

Vector UTIL_RandomBloodVector( void )
{
	Vector direction;

	direction.x = RANDOM_FLOAT ( -1, 1 );
	direction.y = RANDOM_FLOAT ( -1, 1 );
	direction.z = RANDOM_FLOAT ( 0, 1 );

	return direction;
}


void UTIL_BloodDecalTrace( TraceResult *pTrace, int bloodColor )
{
	if ( UTIL_ShouldShowBlood( bloodColor ) )
	{
		if ( bloodColor == BLOOD_COLOR_RED )
			UTIL_DecalTrace( pTrace, DECAL_BLOOD1 + RANDOM_LONG(0,5) );
		else
			UTIL_DecalTrace( pTrace, DECAL_YBLOOD1 + RANDOM_LONG(0,5) );
	}
}


void UTIL_DecalTrace( TraceResult *pTrace, int decalNumber )
{
	short entityIndex;
	int index;
	int message;

	if ( decalNumber < 0 )
		return;

	index = gDecals[ decalNumber ].index;

	if ( index < 0 )
		return;

	if (pTrace->flFraction == 1.0)
		return;

	// Only decal BSP models
	if ( pTrace->pHit )
	{
		CBaseEntity *pEntity = CBaseEntity::Instance( pTrace->pHit );
		if ( pEntity && !pEntity->IsBSPModel() )
			return;
		entityIndex = ENTINDEX( pTrace->pHit );
	}
	else 
		entityIndex = 0;

	message = TE_DECAL;
	if ( entityIndex != 0 )
	{
		if ( index > 255 )
		{
			message = TE_DECALHIGH;
			index -= 256;
		}
	}
	else
	{
		message = TE_WORLDDECAL;
		if ( index > 255 )
		{
			message = TE_WORLDDECALHIGH;
			index -= 256;
		}
	}
	
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( message );
		WRITE_COORD( pTrace->vecEndPos.x );
		WRITE_COORD( pTrace->vecEndPos.y );
		WRITE_COORD( pTrace->vecEndPos.z );
		WRITE_BYTE( index );
		if ( entityIndex )
			WRITE_SHORT( entityIndex );
	MESSAGE_END();
}