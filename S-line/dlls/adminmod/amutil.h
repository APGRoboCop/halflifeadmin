/*
 * ===========================================================================
 *
 * $Id: amutil.h,v 1.8 2004/08/21 21:29:08 darope Exp $
 *
 *
 * Copyright (c) 2001-2003 Alfred Reynolds, Florian Zschocke, Magua
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
 * Description: This file declares various utility functions and macros.
 *              It should be safely included in all AM source files.
 *              I got sick and tired of getting endless errors when 
 *              including users.h or util.h due to unknown requirements
 *              and dependencies.
 *              I'm starting to move some functions over here from users.h
 *
 */

#ifndef _AMUTIL_H_
#define _AMUTIL_H_


#include <cvardef.h>
#include <cerrno>
#include <cstdlib>                 // exit()
#ifndef _WIN32
#  include <unistd.h>                 // _exit()
#endif

#define USERNAME_SIZE 33            // max size of username

extern cvar_t* ptAM_debug;
extern cvar_t* ptAM_devel;
extern cvar_t* ptAM_botProtection;
//extern cvar_t* ptAM_autoban;
extern cvar_t* ptAM_reserve_slots;
extern cvar_t* ptAM_hide_reserved_slots;
extern cvar_t* ptAM_keyfiles;

/*
 *  some macros to be used around
 */

#define DEBUG_LOG( level, args )   do{  if ( ptAM_debug && ptAM_debug->value >= level ) \
                 UTIL_LogPrintf( "[ADMIN] DEBUG(%i): %s\n", level, UTIL_VarArgs args );} while(0)

#define DEVEL_LOG( level, args )   do{  if ( ptAM_devel && ptAM_devel->value >= level ) \
                 UTIL_LogPrintf( "[ADMIN] DEVEL(%i): %s\n", level, UTIL_VarArgs args );} while(0)

/* debugging and devel macros for conditional execution */
#define DEBUG_DO( level, commands ) do{ if ( ptAM_debug && ptAM_debug->value >= level ) \
                 { commands } } while(0)

#define DEVEL_DO( level, commands ) do{ if ( ptAM_devel && ptAM_devel->value >= level ) \
                 { commands } } while(0)




/*
 * Inline replacement for exit.
 */
inline void am_exit( int e )
{
	_exit( e );
}




/*
 *  Functions defined in amutil.cpp
 */


// stop malformed names stuffing up checking 
//
int make_friendly(char* name, bool check = false);


// convert a string to all-lower 
//
void strtolower( char* string );


// a crude version of strstr() but ignoring case
//
char* stristr(const char* haystack, const char* needle );


// Formats the string-literals '\n' and '^n' into line feeds (ASCII 10)
//
void FormatLine(char* sLine);


// Formats a path so all the /s and \s are correct for that OS.
//
void FormatPath(char* sPath);

const char* escape_chars( const char* _pcString, const char* _pcChars );
inline const char* escape_chars( const char* _pcString, char _cChar ) {
	char acChars[2];
	acChars[0] = _cChar;
	acChars[1] = 0;
	return escape_chars( _pcString, acChars );
}  // escape_chars()

// Check a string for violations of maximum line length. 
// When a line is too long, if can fail, truncate or
// wrap the line. If the line cannot be handled, 0 is returned, 
// 1 otherwise
//
#define SW_NOWRAP 0
#define SW_WRAP 1
#define SW_TRUNC 2
int wrap_lines( char* String, int _iLineLen, int _iWrap);


void sprintmd5( char* _pcTarget, const unsigned char* _pucMD5Src );
 
// Returns true if this line should be a comment, false otherwise. 
inline bool is_comment(char cChar) {
	return ((cChar == '#' || cChar == '/' || cChar == ';') ? true : false);
} 


inline bool is_comment(char* pcChar) {
	return ((*pcChar == '#' || (*pcChar == '/' && *(pcChar+1) == '/') || *pcChar == ';') ? true : false);
} 

#endif /* _AMUTIL_H_ */
