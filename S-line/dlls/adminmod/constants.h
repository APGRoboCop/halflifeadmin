/*
 * ===========================================================================
 *
 * $Id: constants.h,v 1.1 2004/01/01 20:12:42 darope Exp $
 *
 *
 * Copyright (c) 2004 Alfred Reynolds, Florian Zschocke
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
 * This file keeps various defines and constants in one central place.
 * 
 */

#ifndef AM_CONSTANTS_H
#define AM_CONSTANTS_H


 
// Access levels
#define ALL 0
#define LEV1 1
#define LEV2 1<<1
#define LEV4  1<<2
#define LEV8 1<<3
#define LEV16 1<<4
#define LEV32 1<<5 
#define LEV64 1<<6 
#define LEV128 1<<7
#define LEV256 1<<8
#define LEV512 1<<9
#define LEV1024 1<<10
#define LEV2048  1<< 11
#define LEV4096  1<<12
#define LEV8192 1<<13
#define LEV16384 1<<14
#define LEV32768 1<<15

#define MAX_PLAYERS 64              // max number of users allowed on a hl server. Its 32 but lets be safe :)
#define MAX_STRBUF_LEN 1024

#define ACCESS_SIZE           10    // max size of an access number
#define BUF_SIZE             100    // max size in general (default)
#define LARGE_BUF_SIZE       256    // max size of a large buffer
#define HUGE_BUF_SIZE        512    // max size of a huge buffer
#define INFO_KEYVAL_SIZE     128    // max size of an infobuffer key or value string
#define QUERY_BUF_SIZE       500    // max size of a SQL query
#define CENTER_SAY_LINE_SIZE  78    // max size of any one line in centersay
#define CENTER_SAY_SIZE      500    // max size of centersay "say" buffer
#define COMMAND_SIZE         256    // max size of a server command
#define IP_SIZE               16    // max size of an ip line
#define IPPORT_SIZE           23    // max size of an ip:port line
#define LINE_SIZE            256    // max size of line in file
#define PASSWORD_SIZE         64    // max size of password (big cause of win32)
#define USERNAME_SIZE         33    // max size of username
#define MODELNAME_SIZE        33    // max size of model name
#define VOTE_SIZE           1024    // max size of the vote message (including all options)

// MOTD defines
#define MAX_MOTD_CHUNK       60     // max chunk to send to client at a time
#define MAX_MOTD_LENGTH    1536     // max size of MOTD message

// Menu defines
#define MAX_MENU_CHUNK      176     // max chunk to send to client at a time

#define AM_TEAM_SPECTATOR 500
#define AM_TEAM_PROXY     600

#endif /* AM_CONSTANTS_H */

