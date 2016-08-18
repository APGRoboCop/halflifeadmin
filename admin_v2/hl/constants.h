/*
 * $Id: constants.h,v 1.1 2001/09/27 20:33:16 darope Exp $
 *
 *
 * Copyright (c) 1999-2001 Alfred Reynolds, Florian Zschocke, Magua
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
 * Description: This file collects the definitions of various constants
 *              used throughout the adminmod source and formerly spread
 *              over various source files. To keep them in one common place
 *              I move them in here. 
 *              Add any new constants to this file. Exceptions are only 
 *              constants directly linked to one specific data structure
 *              like MAX_RULE_BUF. These are defined right the before the
 *              declaration of the specific data structure.
 *
 *
 */

#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

/* maximum path length if not provided from the compiler */
#ifndef PATH_MAX
#  define PATH_MAX 2048
#endif


/*
 * User access levels
 *
 *       1 - vote map, vote kick,
 *       2 - map, timelimit, fraglimit. (if they can change the map.. it
 *           doesn't make sense to lock these out)
 *       4 - prematch, reload
 *       8 - pause, unpause
 *      16 - pass, no_pass
 *      32 - teamplay, Friendly-Fire
 *      64 - admin_change other server variables.. (gravity, fall damage, etc.. )
 *     128 - admin_say, admin_kick
 *     256 - admin_ban, admin_unban 
 *
 *  leaves some space for other stuff.
 *
 *   32768 - reserved slot
*/


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


/* general setting constants */
#define MAX_PLAYERS 64              // max number of users allowed on a hl server. Its 32 but lets be safe :)
#define CENTER_SAY_LINE_SIZE 80     // max size of any one line in centersay
#define CENTER_SAY_SIZE 500         // max size of centersay "say" buffer
#define MENU_SHOW 30                // how long to show menu's for (in sec) 
 

/* buffer lenght sizes for strings */
#define BUF_SIZE 100                // max size in general (default)
#define LARGE_BUF_SIZE 256          // max size of a large buffer
#define HUGE_BUF_SIZE  512          // max size of a huge buffer
#define MAX_STRBUF_LEN 1024
#define LINE_SIZE 256               // max size of line in file

/* buffer sizes for some structures */
#define ACCESS_SIZE 10              // max size of an access number
#define COMMAND_SIZE 256            // max size of a server command
#define IP_SIZE 16                  // max size of an ip line
#define PASSWORD_SIZE 64            // max size of password (big cause of win32)
#define USERNAME_SIZE 64            // max size of username
#define VOTE_SIZE 1024              // max size of the vote message (including all options)



#endif  /* _CONSTANTS_H_ */
