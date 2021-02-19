/*
 * ===========================================================================
 * 
 * $Id: amutil.cpp,v 1.4 2003/05/25 16:41:30 darope Exp $
 *
 *
 * Copyright (c) 2001-2003 Alfred Reynolds, Florian Zschocke
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
 *              This file defines various utility functions. It exists next to
 *              the util.cpp file. The util.cpp file has functions defined which
 *              are declared anywhere but in the util.h header. That violates the
 *              source - header relationship, but including util.h keeps giving
 *              me trouble. Thus I start to move utility functions over here and
 *              declaring them in amutil.h. I start with functions completely
 *              unrelated to adminmod. For now, functions that use AM or HL 
 *              specififc data structures or functions are kept in, well, wherever
 *              they are now. =)
 *
 */

#include <cstring>
#include <cctype>
#include "amutil.h"



/*
 *  stop malformed names stuffing up checking
 */
extern int g_NameCrashAction;
int make_friendly(char *name, bool check) {
	const int iLen = strlen(name);
	
	
	if(check && (g_NameCrashAction > 0 ) && ( iLen<=0 )  ) return 2; // the name is zero length....  
	
	for(int i = 0; i < iLen; i++) {
		if (name[i]=='%') {
			// replace '%' with ' ' like the engine
			name[i] = ' ';
		}
		// since we go through the string here anyway, we may just as well check for 
		// vicious characters
		if ( check && (g_NameCrashAction > 0 ) && !isprint(name[i]) ) return 2;
	}
	return 1;
}


//
// covert a string to lowercase
//
void strtolower( char* string ) {
	const int length = strlen ( string );
  char character = 0;

  for ( int i = 0; i < length; i++ ) {
    character = tolower( string[i] );
    string[i] = character;
  }  // for

  return;

}  // strtolower()





//
// stristr: This is a crude version of strtsr ignoring case.
// brute force since we only have a short text / pattern
// and the guys are waiting for their server. 
// must be improved some day.
//
char* stristr( const char* haystack, const char* needle ) {
	const int iLength = strlen( haystack );
	const int iNeedleLen = strlen( needle );
  char* pcBegin = nullptr;

  int iH = 0;
  int iN = 0;

  if ( iNeedleLen > iLength ) return nullptr;

  for ( int iStart = 0; iStart < iLength-iNeedleLen+1; iStart++ ) {
    pcBegin = const_cast<char*>(haystack + iStart);

    iH = iStart;
    iN = 0;

    while ( (iH < iLength) && (iN < iNeedleLen) && (tolower(haystack[iH]) == tolower(needle[iN])) ) {
      iH++;
      iN++;
    }  // while

    // match
    if ( iN == iNeedleLen ) return pcBegin;

    // no match
    if ( iH == iLength ) return nullptr;
  }  // for

  return nullptr;

}  // stristr()


//
// Formats the string-literals '\n' and '^n' into line feeds (ASCII 10)
//
void FormatLine(char* sLine) {
	int iOffset = 0;
	char* sNewChar = sLine;
	char* sOldChar = sLine;
	
	while (*sOldChar != '\0') {
		if ((*sOldChar == '\\' || *sOldChar == '^') && *(sOldChar + 1) == 'n') {
			*sNewChar = '\n';
			sOldChar++;
		} else if (sOldChar != sNewChar) {
			*sNewChar = *sOldChar;
		}
		sNewChar++;
		sOldChar++;
	}
	*sNewChar = '\0';
}



//
// Formats a path so all the /s and \s are correct for that OS.
//
void FormatPath(char* sPath) {
  while ( sPath != nullptr && *sPath != '\0') {

#ifdef WIN32
    if ( *sPath == '/' ) {
      *sPath = '\\';
    }  // if
#else
    if ( *sPath == '\\' ) {
      *sPath = '/';
    }  // if
#endif						

    sPath++;
  }  // while
}


//
// Check a string for line lengths. Too long lines can cause failure,
// be truncated or be wrapped
//
int wrap_lines( char* _pcString, int _iLineLen, int _iWrap ) {

	char* pcLineStart = _pcString;
	char* pcLineEnd;
	char* pcStringEnd = _pcString + strlen(_pcString);

	int retval = 1;

	// find the first newline
	if ( !(pcLineEnd = strchr(pcLineStart, '\n')) ) pcLineEnd = pcStringEnd;

	// while we're not finished with the string
	while ( pcLineStart < pcStringEnd ) {

		// check for line length
		if ( (pcLineEnd - pcLineStart) > _iLineLen ) {

			if ( _iWrap == SW_NOWRAP ) {  // we just check the length
				return 0;
			} else if ( _iWrap == SW_TRUNC ) {  // we truncate the line
				memmove( pcLineStart+_iLineLen, pcLineEnd, strlen(pcLineEnd)+1 );
				retval = 1;
			} else if ( _iWrap == SW_WRAP ) {  // wrap too long lines
				pcLineEnd = pcLineStart + _iLineLen;
				while ( *pcLineEnd != ' ' && pcLineEnd > pcLineStart ) pcLineEnd--;
				if ( *pcLineEnd == ' ' ) {
					*pcLineEnd = '\n';
				} else { // This string is unwrappable
					return 0;
				}  // if-else
			}  // if-else
		}  // if

		// find the next newline
		pcLineStart = pcLineEnd + 1;
		if ( !(pcLineEnd = strchr(pcLineStart, '\n')) ) pcLineEnd = pcStringEnd;

	}  // while

	return retval;
}  // wrap()


//
// Print a MD5 digest in ASCII representation into a char array
//
void sprintmd5( char* _pcTarget, const unsigned char* _pucMD5Src ) {
	if ( _pcTarget == nullptr || _pucMD5Src == nullptr ) return;
 
    for ( unsigned char i = 0; i < 16; i++ ) {
        unsigned char num = _pucMD5Src[i] >> 4;
        if ( num < 10 ) {
            *_pcTarget++ = num + '0';
        } else {
            *_pcTarget++ = num - 10 + 'a';
        }  // if-else
 
        num = _pucMD5Src[i] & 0x0f;
        if ( num < 10 ) {
            *_pcTarget++ = num + '0';
        } else {
            *_pcTarget++ = num - 10 + 'a';
        }  // if-else
    }  // for

    *_pcTarget = '\0';
}  // sprintmd5()
